from decimal import Decimal
from fractions import Fraction
import os
from types import SimpleNamespace
import unittest

import paper_endpoint_sidecar_replay as replay


ZERO = "+0." + "0" * 109 + "e+00000"
POSITIVE_HALF = "+5." + "0" * 109 + "e-00001"
NEGATIVE_HALF = "-5." + "0" * 109 + "e-00001"
POSITIVE_QUARTER = "+2.5" + "0" * 108 + "e-00001"
NEGATIVE_QUARTER = "-2.5" + "0" * 108 + "e-00001"
POSITIVE_EIGHTH = "+1.25" + "0" * 107 + "e-00001"
POSITIVE_THREE_QUARTERS = "+7.5" + "0" * 108 + "e-00001"
EXTREME = "+1." + "0" * 109 + "e+99999"
TINY_ONE = "+1." + "0" * 109 + "e-00100"
TINY_NEAR = "+1." + "0" * 27 + "1" + "0" * 81 + "e-00100"
TINY_NEGATIVE_TWO = "-2." + "0" * 109 + "e-00100"


class SidecarReplayTests(unittest.TestCase):
    def test_public_summary_distinguishes_decimal_after_digit_28(self):
        zero = (Decimal(0), Decimal(0))
        lower = replay.ScalarReplay(
            e0=(Decimal(1), Decimal(0)), e8=zero, fresh=zero,
            z_power=zero, fresh_power=zero, terminal=zero, i8=zero, a8=zero,
            identity=zero)
        higher_value = Decimal("1." + "0" * 27 + "1")
        higher = replay.ScalarReplay(
            e0=(higher_value, Decimal(0)), e8=zero, fresh=zero,
            z_power=zero, fresh_power=zero, terminal=zero, i8=zero, a8=zero,
            identity=zero)
        summary = replay.summarize_scalars(((2, lower), (5, higher)))
        self.assertEqual(summary.e0.maximum,
                         Fraction(10 ** 28 + 1, 10 ** 28))
        self.assertEqual((summary.e0.argmax_slot, summary.e0.argmax_component),
                         (5, "real"))

    def test_bounded_summary_preserves_full_complex_tuple_and_ties(self):
        zero = (Fraction(0), Fraction(0))
        first = replay.ScalarReplay(
            e0=(Fraction(2), Fraction(-2)), e8=(Fraction(3), Fraction(4)),
            fresh=zero, z_power=zero, fresh_power=zero, terminal=zero,
            i8=(Fraction(5), Fraction(6)), a8=(Fraction(7), Fraction(8)),
            identity=zero)
        later_tie = replay.ScalarReplay(
            e0=(Fraction(-2), Fraction(0)), e8=zero, fresh=zero,
            z_power=zero, fresh_power=zero, terminal=zero, i8=zero, a8=zero,
            identity=zero)
        summary = replay.summarize_scalars(((2, first), (5, later_tie)))
        self.assertEqual(summary.e0.maximum, Fraction(2))
        self.assertEqual((summary.e0.argmax_slot, summary.e0.argmax_component),
                         (2, "real"))
        self.assertEqual(summary.e0.signed_tuple,
                         (first.e0, first.e8, first.i8, first.a8))

    def test_public_bounds_seam_uses_exact_nonzero_k_and_rejects_excess(self):
        meta = {
            "coefficient_l1_fresh": 1,
            "coefficient_l1_terminal": 1,
            "scale0": Fraction(1 << 100),
            "scale8": Fraction(1 << 100),
        }
        bounds = replay.derive_replay_bounds(
            meta, maximum_e0_exponent=-20, maximum_e8_exponent=-21)
        self.assertEqual(bounds.serialization_e0, Fraction(1, 10 ** 129))
        self.assertEqual(bounds.serialization_e8, Fraction(1, 10 ** 130))
        self.assertEqual(bounds.replay_e0,
                         Fraction(1, 1 << 856) + Fraction(1, 1 << 504) +
                         Fraction(1, 10 ** 129))
        with self.assertRaisesRegex(replay.ReplayUnresolved, "budget"):
            replay.derive_replay_bounds(
                meta, maximum_e0_exponent=99999, maximum_e8_exponent=None)

    def test_replays_eight_explicit_squares_from_nonzero_worked_values(self):
        result = replay.replay_complex(
            z=(Fraction(1, 2), Fraction(0)),
            e0_text=(POSITIVE_HALF, ZERO),
            e8_text=(POSITIVE_QUARTER, ZERO),
        )
        two_minus_256 = Fraction(1, 1 << 256)
        self.assertEqual(result.fresh, (Fraction(1), Fraction(0)))
        self.assertEqual(result.z_power, (two_minus_256, Fraction(0)))
        self.assertEqual(result.fresh_power, (Fraction(1), Fraction(0)))
        self.assertEqual(result.terminal, (Fraction(1, 4) + two_minus_256, Fraction(0)))
        self.assertEqual(result.i8, (Fraction(1) - two_minus_256, Fraction(0)))
        self.assertEqual(result.a8, (two_minus_256 - Fraction(3, 4), Fraction(0)))
        self.assertEqual(result.identity, (Fraction(0), Fraction(0)))

    def test_replays_nonzero_imaginary_worked_values(self):
        result = replay.replay_complex(
            z=(Fraction(1, 2), Fraction(1, 2)),
            e0_text=(NEGATIVE_HALF, ZERO),
            e8_text=(NEGATIVE_QUARTER, POSITIVE_EIGHTH),
        )
        z_power = Fraction(1, 1 << 128)
        fresh_power = Fraction(1, 1 << 256)
        self.assertEqual(result.fresh, (Fraction(0), Fraction(1, 2)))
        self.assertEqual(result.z_power, (z_power, Fraction(0)))
        self.assertEqual(result.fresh_power, (fresh_power, Fraction(0)))
        self.assertEqual(result.terminal,
                         (z_power - Fraction(1, 4), Fraction(1, 8)))
        self.assertEqual(result.i8, (fresh_power - z_power, Fraction(0)))
        self.assertEqual(result.a8,
                         (z_power - Fraction(1, 4) - fresh_power, Fraction(1, 8)))
        self.assertEqual(result.identity, (Fraction(0), Fraction(0)))

    def test_public_row_lookup_retains_values_for_primary_slot_reconciliation(self):
        values = tuple(SimpleNamespace(text=text) for text in
                       (POSITIVE_HALF, ZERO, NEGATIVE_QUARTER, POSITIVE_EIGHTH))
        row = SimpleNamespace(slot=0, values=values)
        result = replay.replay_row(0, row)
        self.assertEqual(result.e0, (Fraction(1, 2), Fraction(0)))
        self.assertEqual(result.e8, (Fraction(-1, 4), Fraction(1, 8)))

    def test_rejects_incomplete_sidecar_without_treating_sample_as_full_slot(self):
        value = SimpleNamespace(text=ZERO)
        sidecar = SimpleNamespace(meta={}, rows=(SimpleNamespace(
            slot=0, values=(value, value, value, value)),))
        with self.assertRaisesRegex(replay.ReplayError, "16384"):
            replay.replay_sidecar(sidecar)

    def test_rejects_serialization_budget_before_decimal_squaring(self):
        zero = SimpleNamespace(text=ZERO)
        extreme = SimpleNamespace(text=EXTREME)
        rows = tuple(SimpleNamespace(slot=slot, values=(extreme, zero, zero, zero))
                     for slot in range(16384))
        sidecar = SimpleNamespace(meta={
            "coefficient_l1_fresh": 0,
            "coefficient_l1_terminal": 0,
            "scale0": Fraction(1),
            "scale8": Fraction(1),
        }, rows=rows)
        with self.assertRaisesRegex(replay.ReplayUnresolved, "budget"):
            replay.replay_sidecar(sidecar)

    def test_reconstructs_frozen_dyadic_input_with_phase_and_sign(self):
        self.assertEqual(replay.frozen_input(0),
                         (Fraction(1015, 1024), Fraction(1, 1024)))
        self.assertEqual(replay.frozen_input(256),
                         (-Fraction(1, 1024),
                          Fraction(1015, 1024) + Fraction(1, 1 << 67)))
        self.assertEqual(replay.frozen_input(1024),
                         (Fraction(1015, 1024) + Fraction(1, 1 << 65),
                          -Fraction(1, 1024)))

    def test_rejects_malformed_scalar_and_exact_disk_excess(self):
        with self.assertRaisesRegex(replay.ReplayError, "grammar"):
            replay.replay_complex(
                z=(Fraction(0), Fraction(0)),
                e0_text=("+1." + "0" * 109 + "e-00000", ZERO),
                e8_text=(ZERO, ZERO))
        with self.assertRaisesRegex(replay.ReplayUnresolved, "3/2 disk"):
            replay.replay_complex(
                z=(Fraction(1), Fraction(0)),
                e0_text=(POSITIVE_THREE_QUARTERS, ZERO),
                e8_text=(ZERO, ZERO))

    @unittest.skipUnless(os.environ.get("RUN_FULL_ENDPOINT_REPLAY") == "1",
                         "HOSTED NOT RUN: complete 16384-row Decimal256 replay")
    def test_hosted_replays_complete_nonzero_synthetic_sidecar(self):
        values = tuple(SimpleNamespace(text=text) for text in
                       (TINY_ONE, ZERO, ZERO, TINY_NEGATIVE_TWO))
        near_values = tuple(SimpleNamespace(text=text) for text in
                            (TINY_NEAR, ZERO, ZERO, TINY_NEGATIVE_TWO))
        rows = tuple(SimpleNamespace(
            slot=slot, values=near_values if slot == 9 else values)
                     for slot in range(16384))
        sidecar = SimpleNamespace(meta={
            "coefficient_l1_fresh": 1,
            "coefficient_l1_terminal": 1,
            "scale0": Fraction(1 << 100),
            "scale8": Fraction(1 << 100),
        }, rows=rows)
        result = replay.replay_sidecar(sidecar)
        self.assertEqual(result.row_count, 16384)
        self.assertEqual((result.e0.argmax_slot, result.e0.argmax_component),
                         (9, "real"))
        self.assertEqual((result.e8.argmax_slot, result.e8.argmax_component),
                         (0, "imag"))
        self.assertEqual(result.e0.signed_tuple[0],
                         (Fraction(10 ** 28 + 1, 10 ** 128), Fraction(0)))
        self.assertEqual(result.e0.signed_tuple[1],
                         (Fraction(0), Fraction(-2, 10 ** 100)))


if __name__ == "__main__":
    unittest.main(verbosity=2)
