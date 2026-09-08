"""Parser-only public CLI contracts; every input fails before any transform."""
import argparse
import importlib.util
from fractions import Fraction
import math
from pathlib import Path
import subprocess
import sys
from tempfile import TemporaryDirectory
import unittest


ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(add_help=False)
parser.add_argument('--candidate-module', type=Path,
                    default=ROOT/'diagnostics/certify_public_encoder.py')
options, remaining = parser.parse_known_args()
CANDIDATE = options.candidate_module.resolve()
spec = importlib.util.spec_from_file_location('public_cap_scalar_tdd', CANDIDATE)
module = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = module
spec.loader.exec_module(module)


class AlgebraicOracleBoundary(unittest.TestCase):
    def test_cap_classification_below_equal_above_and_straddling(self):
        # 16129/16384 is exactly representable on the candidate's binary grid.
        cap = 16129*(module.UNIT//16384)
        cases = [(cap-2, cap-1, 'ENCODER_CAP_CERTIFIED'),
                 (cap, cap, 'ENCODER_CAP_CERTIFIED'),
                 (cap+1, cap+2, 'ENCODER_CAP_REFUTED'),
                 (cap-1, cap+1, 'INCONCLUSIVE_INTERVAL')]
        for lower, upper, expected in cases:
            with self.subTest(expected=expected, lower=lower):
                self.assertEqual(module.classify_cap(module.Interval(lower, upper)), expected)

    def test_false_point_above_sqrt_is_not_a_containment_certificate(self):
        # Importing the remote models does not run their test cases/transforms.
        oracle_spec = importlib.util.spec_from_file_location(
            'public_cap_models', ROOT/'tests/public_encoder_transform_contract_test.py')
        oracle = importlib.util.module_from_spec(oracle_spec)
        oracle_spec.loader.exec_module(oracle)
        denominator = 1 << 224
        floor_root = math.isqrt(2*denominator*denominator)
        false_point = 10+5*Fraction(floor_root+1, denominator)
        with self.assertRaises(AssertionError):
            oracle.assert_sqrt_enclosure(self, false_point, false_point)


class PublicFileBoundary(unittest.TestCase):
    def test_exact_limit_is_read_but_one_more_byte_is_rejected(self):
        with TemporaryDirectory(prefix='public-encoder-file-test-') as directory:
            source = Path(directory)/'public.json'
            payload = b' '*11_999_999+b'\n'
            with source.open('xb') as stream:
                stream.write(payload)
            self.assertEqual(module.read_public_bytes(source), payload)
            with source.open('ab') as stream:
                stream.write(b'\n')
            with self.assertRaisesRegex(ValueError, 'bounded LF'):
                module.read_public_bytes(source)

    def test_symlink_is_not_accepted_as_regular_input(self):
        with TemporaryDirectory(prefix='public-encoder-file-test-') as directory:
            source = Path(directory)/'public.json'
            with source.open('xb') as stream:
                stream.write(b'{}\n')
            alias = Path(directory)/'alias.json'
            alias.symlink_to(source)
            with self.assertRaises((ValueError, OSError)):
                module.read_public_bytes(alias)

    def test_directory_is_not_regular_input(self):
        with TemporaryDirectory(prefix='public-encoder-file-test-') as directory:
            with self.assertRaises((ValueError, OSError)):
                module.read_public_bytes(Path(directory))


class PublicEncoderParserContract(unittest.TestCase):
    def assert_rejected(self, payload):
        with TemporaryDirectory(prefix='public-encoder-parser-test-') as directory:
            source = Path(directory)/'public.json'
            output = Path(directory)/'certificate.json'
            with source.open('xb') as stream:
                stream.write(payload)
            result = subprocess.run(
                [sys.executable, '-B', '-I', str(CANDIDATE),
                 '--public-encoding', str(source), '--out', str(output),
                 '--allow-transform-after-root-review'],
                capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 4, result.stderr)
            self.assertIn('PUBLIC_CAP_REJECTED:', result.stderr)
            self.assertNotIn('Traceback', result.stderr)
            self.assertFalse(output.exists(), 'Rejected input must not create a certificate')

    def test_deep_json_is_controlled_rejection(self):
        self.assert_rejected(b'['*10000+b'0'+b']'*10000+b'\n')

    def test_duplicate_fields_are_rejected_by_actual_decoder(self):
        self.assert_rejected(b'{"schema":"x","schema":"y"}\n')

    def test_truncated_json_is_rejected(self):
        self.assert_rejected(b'{"schema":\n')

    def test_invalid_utf8_is_rejected(self):
        self.assert_rejected(b'{"schema":"\xff"}\n')

    def test_crlf_is_rejected(self):
        self.assert_rejected(b'{}\r\n')

    def test_missing_final_lf_is_rejected(self):
        self.assert_rejected(b'{}')


if __name__ == '__main__':
    unittest.main(argv=[sys.argv[0], *remaining])
