"""Public typed-reason tests for endpoint sidecar parsing and scalar replay."""
from decimal import Decimal
from fractions import Fraction
from pathlib import Path
import tempfile
import unittest
from unittest import mock

import paper_endpoint_sidecar_reader as reader
import paper_endpoint_sidecar_replay as replay
from test_paper_endpoint_sidecar_reader import SOURCE, valid_bytes


ZERO = "+0." + "0" * 109 + "e+00000"
POSITIVE_THREE_QUARTERS = "+7.5" + "0" * 108 + "e-00001"


class EndpointErrorReasonTests(unittest.TestCase):
    def read_sidecar(self, data, **overrides):
        expected = dict(expected_scope="synthetic", expected_source_commit=SOURCE,
                        expected_host="linux", expected_run_id="17",
                        expected_run_attempt="2")
        expected.update(overrides)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-") as directory:
            path = Path(directory) / "candidate.tsv"
            path.write_bytes(data)
            return reader.read_sidecar(path, **expected)

    def over_ceiling_fresh_cross(self, distance=Fraction(0)):
        lines = valid_bytes().decode("ascii").splitlines()
        coefficient = 1 << 473  # With S0=2^100, Kfresh is exactly 2^373.
        for index, line in enumerate(lines):
            if line == "meta\tcoefficient_l1_fresh\t3":
                lines[index] = "meta\tcoefficient_l1_fresh\t" + str(coefficient)
            if line.startswith("check\tfresh.cross\t"):
                fields = line.split("\t")
                allowance = Fraction(1, 1 << 127) + Fraction(1, 1 << 383)
                fields[3:7] = [str(distance.numerator), str(distance.denominator),
                               str(allowance.numerator), str(allowance.denominator)]
                lines[index] = "\t".join(fields)
        return ("\n".join(lines) + "\n").encode("ascii")

    def test_exception_constructors_keep_messages_and_expose_stable_defaults(self):
        cases = (
            (reader.SidecarError("sidecar detail"), "FORMAT", "sidecar detail"),
            (replay.ReplayError("replay detail"), "REPLAY", "replay detail"),
            (replay.ReplayUnresolved("conditioning detail"),
             "CONDITIONING", "conditioning detail"),
        )
        for error, reason, message in cases:
            with self.subTest(error=type(error).__name__):
                self.assertEqual(error.reason, reason)
                self.assertEqual(str(error), message)

    def test_sidecar_distinguishes_malformed_expected_identity_from_artifact_mismatch(self):
        with self.assertRaises(reader.SidecarError) as malformed:
            self.read_sidecar(valid_bytes(), expected_source_commit="not-a-sha")
        self.assertEqual(malformed.exception.reason, "FORMAT")

        with self.assertRaises(reader.SidecarError) as mismatch:
            self.read_sidecar(valid_bytes(), expected_source_commit="b" * 40)
        self.assertEqual(mismatch.exception.reason, "IDENTITY")

        malformed_artifact = valid_bytes().replace(
            b"meta\tsource_commit\t" + SOURCE.encode("ascii") + b"\n",
            b"meta\tsource_commit\t" + b"g" * 40 + b"\n", 1)
        with self.assertRaises(reader.SidecarError) as malformed_record:
            self.read_sidecar(malformed_artifact)
        self.assertEqual(malformed_record.exception.reason, "FORMAT")

    def test_sidecar_distinguishes_semantic_contradiction_from_conditioning(self):
        wrong_model = valid_bytes().replace(
            b"meta\tmodel\tconditional-binary-nearest-direct-trig8u-v1\n",
            b"meta\tmodel\tunsupported-model\n", 1)
        with self.assertRaises(reader.SidecarError) as semantic:
            self.read_sidecar(wrong_model)
        self.assertEqual(semantic.exception.reason, "INTEGRITY")

        radius_excess = valid_bytes().replace(
            b"meta\tfresh_max_l1_numerator\t0\n",
            b"meta\tfresh_max_l1_numerator\t2\n", 1)
        with self.assertRaises(reader.SidecarError) as conditioning:
            self.read_sidecar(radius_excess)
        self.assertEqual(conditioning.exception.reason, "CONDITIONING")

    def test_sidecar_reports_derived_allowance_ceiling_as_unresolved_reason(self):
        with self.assertRaises(reader.SidecarError) as raised:
            self.read_sidecar(self.over_ceiling_fresh_cross())
        self.assertEqual(raised.exception.reason, "ESTIMATOR_CEILING")

    def test_sidecar_raw_excess_precedes_estimator_ceiling(self):
        with self.assertRaises(reader.SidecarError) as raised:
            self.read_sidecar(self.over_ceiling_fresh_cross(
                Fraction(1, 1 << 119)))
        self.assertEqual(raised.exception.reason, "INTEGRITY")

    def test_sidecar_reports_well_formed_wrong_allowance_as_integrity(self):
        lines = valid_bytes().decode("ascii").splitlines()
        for index, line in enumerate(lines):
            if line.startswith("check\tcontrol.constant.512\t"):
                fields = line.split("\t")
                wrong = 2 * Fraction(int(fields[5]), int(fields[6]))
                fields[5:7] = [str(wrong.numerator), str(wrong.denominator)]
                lines[index] = "\t".join(fields)
                break
        with self.assertRaises(reader.SidecarError) as raised:
            self.read_sidecar(("\n".join(lines) + "\n").encode("ascii"))
        self.assertEqual(raised.exception.reason, "INTEGRITY")

    def test_sidecar_reports_well_formed_semantic_receipt_conflicts_as_integrity(self):
        base = valid_bytes().decode("ascii").splitlines()
        mutations = {}

        scale = base[:]
        index = next(i for i, line in enumerate(scale)
                     if line.startswith("meta\tscale0_numerator\t"))
        scale[index] = "meta\tscale0_numerator\t" + str((1 << 100) + 1)
        mutations["scale"] = scale

        e80 = base[:]
        index = e80.index("meta\tE80_disposition\tFAIL")
        e80[index] = "meta\tE80_disposition\tPASS"
        mutations["E80"] = e80

        decision = base[:]
        index = next(i for i, line in enumerate(decision)
                     if line.startswith("check\tcontrol.constant.512\t"))
        fields = decision[index].split("\t")
        distance = 2 * Fraction(int(fields[5]), int(fields[6]))
        fields[3:5] = [str(distance.numerator), str(distance.denominator)]
        decision[index] = "\t".join(fields)
        mutations["decision"] = decision

        horner = base[:]
        index = next(i for i, line in enumerate(horner)
                     if line.startswith("check\tfresh.horner.512\t"))
        fields = horner[index].split("\t")
        fields[3:5] = fields[5:7]
        fields[7] = "2"
        horner[index] = "\t".join(fields)
        mutations["horner argmax"] = horner

        zero_tie = base[:]
        index = next(i for i, line in enumerate(zero_tie)
                     if line.startswith("check\tcontrol.constant.512\t"))
        fields = zero_tie[index].split("\t")
        fields[7:9] = ["1", "imag"]
        zero_tie[index] = "\t".join(fields)
        mutations["zero tie"] = zero_tie

        for label, lines in mutations.items():
            with self.subTest(label=label):
                with self.assertRaises(reader.SidecarError) as raised:
                    self.read_sidecar(("\n".join(lines) + "\n").encode("ascii"))
                self.assertEqual(raised.exception.reason, "INTEGRITY")

    def test_sidecar_translates_only_filesystem_read_failure_to_io_error(self):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-") as directory:
            path = Path(directory) / "candidate.tsv"
            path.write_bytes(valid_bytes())
            with mock.patch.object(reader.Path, "open",
                                   side_effect=OSError("synthetic read fault")):
                with self.assertRaises(reader.SidecarError) as raised:
                    reader.read_sidecar(
                        path, expected_scope="synthetic",
                        expected_source_commit=SOURCE, expected_host="linux",
                        expected_run_id="17", expected_run_attempt="2")
        self.assertEqual(raised.exception.reason, "IO_ERROR")
        self.assertIsInstance(raised.exception.__cause__, OSError)

    def test_replay_distinguishes_conditioning_from_estimator_ceiling(self):
        with self.assertRaises(replay.ReplayUnresolved) as conditioning:
            replay.replay_complex(
                z=(Fraction(1), Fraction(0)),
                e0_text=(POSITIVE_THREE_QUARTERS, ZERO),
                e8_text=(ZERO, ZERO))
        self.assertEqual(conditioning.exception.reason, "CONDITIONING")

        meta = {
            "coefficient_l1_fresh": 1,
            "coefficient_l1_terminal": 1,
            "scale0": Fraction(1 << 100),
            "scale8": Fraction(1 << 100),
        }
        with self.assertRaises(replay.ReplayUnresolved) as ceiling:
            replay.derive_replay_bounds(
                meta, maximum_e0_exponent=109,
                maximum_e8_exponent=None)
        self.assertEqual(ceiling.exception.reason, "ESTIMATOR_CEILING")

    def test_replay_distinguishes_grammar_record_integrity_and_arithmetic(self):
        malformed = "+1." + "0" * 109 + "e-00000"
        cases = (
            ("grammar", lambda: replay.replay_complex(
                z=(Fraction(0), Fraction(0)), e0_text=(malformed, ZERO),
                e8_text=(ZERO, ZERO)), "FORMAT"),
            ("record", lambda: replay.derive_replay_bounds(
                {}, maximum_e0_exponent=None,
                maximum_e8_exponent=None), "INTEGRITY"),
            ("arithmetic", lambda: replay.replay_complex(
                z=(Fraction(1, 3), Fraction(0)), e0_text=(ZERO, ZERO),
                e8_text=(ZERO, ZERO)), "REPLAY"),
        )
        for label, operation, reason in cases:
            with self.subTest(label=label):
                with self.assertRaises(replay.ReplayError) as raised:
                    operation()
                self.assertEqual(raised.exception.reason, reason)

    def test_replay_reports_nonfinite_without_suppressing_programming_faults(self):
        zero = (Decimal(0), Decimal(0))
        nonfinite = replay.ScalarReplay(
            e0=(Decimal("Infinity"), Decimal(0)), e8=zero, fresh=zero,
            z_power=zero, fresh_power=zero, terminal=zero, i8=zero, a8=zero,
            identity=zero)
        with self.assertRaises(replay.ReplayError) as raised:
            replay.summarize_scalars(((0, nonfinite),))
        self.assertEqual(raised.exception.reason, "NONFINITE")

        with self.assertRaises(TypeError):
            replay.replay_complex(z=None, e0_text=(ZERO, ZERO),
                                  e8_text=(ZERO, ZERO))


if __name__ == "__main__":
    unittest.main(verbosity=2)
