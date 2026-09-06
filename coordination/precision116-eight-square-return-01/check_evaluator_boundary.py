"""Lexical regression for the reviewed test-function boundary, not a C++ proof."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCE = (ROOT / "tests/experimental_precision116_eight_square_test.cpp").read_text()
EVALUATOR = SOURCE.split("Evaluation Evaluate(const Plan& plan, const Cipher& input) {", 1)[1].split("\nvoid CheckBoundState", 1)[0]
CLIENT = SOURCE.split("Measurements RunCandidate(", 1)[1].split("\nMeasurements Run(", 1)[0]


class EvaluatorBoundary(unittest.TestCase):
    def test_no_independent_oracle_calls_inside_evaluator(self):
        forbidden = ("ScaleOracle(", "CheckReturned(", "InspectAncestry(", "EmitReturned(",
                     "CandidateSecret(", "CandidatePolynomial(", "CandidateRecombined(",
                     "ObserveAnchors(", "ObserveEndpoint(", "CheckFinalIdeal(", "CheckFinalActual(",
                     "xp::CheckCipher(")
        self.assertEqual([name for name in forbidden if name in EVALUATOR], [])
        self.assertNotIn(".Decrypt(", EVALUATOR)
        self.assertNotIn(".Encrypt(", EVALUATOR)

    def test_same_chain_and_client_checks_are_retained(self):
        self.assertEqual(EVALUATOR.count("evaluator.DCP("), 1)
        self.assertEqual(EVALUATOR.count("evaluator.Mult2(pair, pair)"), 1)
        self.assertIn("round = 1; round <= 8; ++round", EVALUATOR)
        for name in ("ScaleOracle(", "CheckReturned(", "InspectAncestry(",
                     "CandidateSecret(", "CandidatePolynomial(", "CandidateRecombined(",
                     "ObserveAnchors(", "ObserveEndpoint(", "CheckFinalIdeal(", "CheckFinalActual("):
            self.assertIn(name, CLIENT)


if __name__ == "__main__":
    unittest.main(verbosity=2)
