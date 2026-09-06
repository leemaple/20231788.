#!/usr/bin/env python3
"""Review checker unit/negative tests; no encrypted test or production patch."""
import json
import pathlib
import tempfile
import unittest
import independent_endpoint_check as audit

class ArithmeticAndPreservationTests(unittest.TestCase):
    def test_outward_arithmetic_and_difference_recurrence(self):
        self.assertEqual(audit.self_tests(), {'result':'PASS','assertions':78})

    def test_canonical_zero_and_nonzero(self):
        zero='+'+'0.'+'0'*109+'e+00000'
        self.assertEqual(audit.fields(zero),(audit.F(0),audit.F(0)))
        for invalid in ('-'+zero[1:], zero[:-5]+'00001', '+'+'0.'+'1'*109+'e-00025'):
            with self.subTest(invalid=invalid):
                with self.assertRaises(ValueError): audit.fields(invalid)

    def test_fail_closed_source_and_dispositions(self):
        # Valid values needed only through the ordered status validation: all
        # altered cases must reject before opening any gzip/log or arithmetic.
        good={'source_commit':audit.SOURCE,'production_source':audit.PRODUCTION,
              'openfhe_pin':audit.PIN,'host':'linux','github_run_id':audit.RUN,
              'github_run_attempt':'1','evidence_state':'COMPLETE','reason':'NONE',
              'ctest_exit_code':8,'E80_disposition':'FAIL','A_disposition':'NOT_ADOPTED',
              'observer_disposition':'PASS','packer_disposition':'PASS',
              'chain_count':1,'row_count':16384,'numeric_gate_failures':9}
        changes={'source_commit':'0'*40,'E80_disposition':'PASS',
                 'A_disposition':'ADOPTED','numeric_gate_failures':8,
                 'row_count':16383,'chain_count':2,'ctest_exit_code':0}
        for key,value in changes.items():
            with self.subTest(field=key):
                with tempfile.TemporaryDirectory(prefix='fs-review-negative-') as tmp:
                    root=pathlib.Path(tmp)
                    dest=root/'project/coordination/fs-endpoint-live-run-01/linux'
                    dest.mkdir(parents=True)
                    (dest/'fixture.status.json').write_text(json.dumps({**good,key:value}))
                    with self.assertRaisesRegex(ValueError, 'status identity/disposition: '+key):
                        audit.run(root,'linux')

    def test_paper_printed_tensor_sign_counterexample(self):
        b=a=bp=ap=secret=1
        expected=(b+a*secret)*(bp+ap*secret)
        printed=b*bp-(a*bp+ap*b)*secret+a*ap*secret**2
        upstream=b*bp+(a*bp+ap*b)*secret+a*ap*secret**2
        self.assertEqual((expected,printed,upstream),(4,0,4))

    def test_scale_perturbation_growth_not_metadata_fix(self):
        for k in (1,4):
            old,new=audit.F(2**100),audit.F(2**(100+k))
            for stage in range(1,9):
                divisor=audit.Q[-1]*audit.Q[10-stage]
                old,new=old**2/divisor,new**2/divisor
                self.assertEqual(new/old,2**(k*2**stage))

if __name__=='__main__': unittest.main(verbosity=2)
