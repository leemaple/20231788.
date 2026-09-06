import json
from fractions import Fraction
from math import prod
from pathlib import Path
import unittest

from static_profile import certify


class StaticProfileTests(unittest.TestCase):
    def test_candidate_has_exact_scale_chain_but_no_precision_or_security_pass(self):
        profile = json.loads(Path(__file__).with_name('candidate.json').read_text())
        result = certify(profile)
        self.assertEqual(result['static_status'], 'PASS')
        self.assertEqual(result['profile_id'], 'experimental-s116-d56-b58-v1')
        self.assertEqual(result['scale0'], {'numerator': str(2**116), 'denominator': '1'})
        self.assertEqual(len(result['scales']), 9)
        self.assertEqual(result['native_tower_bits'], [58,58,60,60,60,60,60,60,60,60,56])
        self.assertEqual(result['family_q_counts'], [11,10,9,8,7,6,5,4])
        self.assertEqual(result['scale_drift_less_than_one_percent'], True)
        self.assertEqual(result['terminal_q_over_scale_between_99_100_and_1'], True)
        self.assertEqual(result['security_status'], 'UNRESOLVED')
        self.assertEqual(result['E80_status'], 'NOT_TESTED')
        self.assertEqual(result['adoption_status'], 'NOT_ADOPTED')
        self.assertEqual(result['intermediate_nonwrap'], 'NOT_PROVED')

    def test_corrupt_prime_witness_is_rejected(self):
        profile = json.loads(Path(__file__).with_name('candidate.json').read_text())
        profile['new_primes'][0]['witness'] = 1
        with self.assertRaisesRegex(ValueError, 'primality witness'):
            certify(profile)

    def test_nonprimitive_root_is_rejected(self):
        profile = json.loads(Path(__file__).with_name('candidate.json').read_text())
        profile['new_primes'][1]['root'] = 1
        with self.assertRaisesRegex(ValueError, 'primitive NTT root'):
            certify(profile)

    def test_old_scale_metadata_is_rejected(self):
        profile = json.loads(Path(__file__).with_name('candidate.json').read_text())
        profile['base_metadata_bits'] = 50
        with self.assertRaisesRegex(ValueError, 'metadata'):
            certify(profile)

    def test_base_collision_is_rejected(self):
        profile = json.loads(Path(__file__).with_name('candidate.json').read_text())
        profile['new_primes'][1] = dict(profile['new_primes'][0])
        with self.assertRaisesRegex(ValueError, 'collision'):
            certify(profile)

    def test_another_valid_prime_or_root_cannot_reuse_the_frozen_profile_identity(self):
        for change in ('prime', 'root'):
            with self.subTest(change=change):
                profile = json.loads(Path(__file__).with_name('candidate.json').read_text())
                if change == 'prime':
                    profile['new_primes'][0] = {
                        'modulus': 2**58-91*2**32+1,
                        'root': 253977303509751871, 'witness': 7}
                else:
                    r = profile['new_primes'][0]
                    r['root'] = pow(r['root'], r['modulus']-2, r['modulus'])
                with self.assertRaisesRegex(ValueError, 'frozen candidate identity'):
                    certify(profile)

    def test_all_scales_and_families_match_independent_closed_product_specification(self):
        profile = json.loads(Path(__file__).with_name('candidate.json').read_text())
        result = certify(profile)
        # Original pinned source's ordered consumed primes; do not import the
        # implementation's constants or its recursive scale calculation.
        m = [1152921504598720513,1152921504597016577,1152921504595968001,
             1152921504595640321,1152921504593412097,1152921504592822273,
             1152921504592429057,1152921504589938689]
        bases = [288230191468118017,288230165698314241]
        d = 72057589742960641
        consumed = list(reversed(m))
        for stage, emitted in enumerate(result['scales']):
            denominator = prod((d*consumed[j])**(2**(stage-j-1)) for j in range(stage))
            expected = Fraction(2**(116*2**stage), denominator)
            self.assertEqual(Fraction(int(emitted['numerator_hex'],16),
                                      int(emitted['denominator_hex'],16)), expected)
        for stage, family in enumerate(result['families']):
            self.assertEqual(family['Q'], bases+m[:8-stage]+[d])
            self.assertEqual(family['consumed_modulus'], consumed[stage])
        self.assertEqual(len(result['families']),8)


if __name__ == '__main__':
    unittest.main(verbosity=2)
