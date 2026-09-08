"""Scalar-only public integration seams; never imports numerical candidates."""
import importlib.util
from pathlib import Path
import tempfile
import shutil
import unittest

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('ecd_harness_contract', HERE / 'harness_contract.py')
contract = importlib.util.module_from_spec(spec)
spec.loader.exec_module(contract)


class HarnessContract(unittest.TestCase):
    def test_delivery_bytes_are_bound_before_any_returned_code_can_execute(self):
        original = HERE.parent / 'reproduction-adjudication-20260909' / 'pro'
        self.assertEqual(contract.verify_delivery_bytes(original), 41)
        with tempfile.TemporaryDirectory(prefix='ecd-integrity-contract-') as temp:
            shadow = Path(temp) / 'untrusted'
            shutil.copytree(original, shadow)
            verifier = shadow / 'tools/verify_delivery.py'
            verifier.write_bytes(verifier.read_bytes() + b'\n# changed payload\n')
            with self.assertRaises(ValueError):
                contract.verify_delivery_bytes(shadow)
            (shadow / 'MANIFEST.sha256.json').write_text('{}\n')
            with self.assertRaises(ValueError):
                contract.verify_delivery_bytes(shadow)

    def test_output_reservation_is_canonical_outside_and_once_only(self):
        with tempfile.TemporaryDirectory(prefix='ecd-path-contract-') as temp:
            base = Path(temp).resolve()
            bundle = base / 'immutable'
            bundle.mkdir()
            (base / 'alias').symlink_to(bundle, target_is_directory=True)
            existing = base / 'existing'
            existing.mkdir()
            (base / 'dangling').symlink_to(base / 'missing')
            invalid = [Path('relative'), bundle, bundle / 'new',
                       base / 'existing' / '..' / 'immutable' / 'new',
                       base / 'alias' / 'new', existing, base / 'dangling']
            for path in invalid:
                with self.subTest(path=str(path)):
                    with self.assertRaises(ValueError):
                        contract.reserve_output(bundle, path)
                    self.assertEqual(list(bundle.iterdir()), [])
            result = contract.reserve_output(bundle, base / 'accepted')
            self.assertEqual(result, base / 'accepted')
            self.assertTrue(result.is_dir())
            self.assertEqual(result.stat().st_mode & 0o777, 0o700)
            with self.assertRaises(ValueError):
                contract.reserve_output(bundle, result)

    def test_status_declared_exit_and_observed_exit_must_agree(self):
        cases = [('ECD_ROUNDING_CERTIFIED', 0),
                 ('ECD_ROUNDING_REFUTED', 3), ('INCONCLUSIVE_INTERVAL', 4)]
        for status, code in cases:
            with self.subTest(status=status):
                self.assertEqual(contract.validate_outcome(
                    {'status': status, 'exit_code': code}, code), code)
                for declared in [True, False, str(code), None, (code + 1) % 5]:
                    with self.assertRaises(ValueError):
                        contract.validate_outcome({'status': status, 'exit_code': declared}, code)
                for observed in [True, False, str(code), None, (code + 1) % 5]:
                    with self.assertRaises(ValueError):
                        contract.validate_outcome({'status': status, 'exit_code': code}, observed)
                with self.assertRaises(ValueError):
                    contract.validate_outcome({'status': status}, code)
        with self.assertRaises(ValueError):
            contract.validate_outcome({'status': 'PASS', 'exit_code': 0}, 0)


if __name__ == '__main__':
    unittest.main(verbosity=2)
