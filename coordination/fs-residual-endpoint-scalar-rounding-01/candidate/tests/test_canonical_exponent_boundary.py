"""One public-seam regression; synthetic scalar values, no FHE or files."""
from fractions import Fraction
import unittest

import endpoint_evidence_primitives as ep


class CanonicalExponentBoundary(unittest.TestCase):
    def test_lower_boundary_carry_precedes_exponent_rejection(self):
        binary_denominator = 1 << 332955
        decimal_denominator = 10 ** 99999
        value = Fraction(binary_denominator // decimal_denominator,
                         binary_denominator)
        minimum = Fraction(1, decimal_denominator)
        self.assertLessEqual(value.numerator.bit_length(), 768)
        self.assertLess(value, minimum)
        self.assertLess(minimum - value, Fraction(1, 2 * 10 ** 100109))
        for sign, prefix in ((1, "+"), (-1, "-")):
            with self.subTest(sign=sign):
                self.assertEqual(ep.canonical_decimal(sign * value),
                                 prefix + "1." + "0" * 109 + "e-99999")
        with self.assertRaises(ep.EvidenceError):
            ep.canonical_decimal(Fraction(1, 10 ** 100000))


if __name__ == "__main__":
    unittest.main(verbosity=2)
