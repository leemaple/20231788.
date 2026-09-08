#!/usr/bin/env python3
"""UNRUN in author turn. Root-only tiny transform TDD, never encryption."""
import importlib.util
from pathlib import Path
import sys
from fractions import Fraction as F
import math
import unittest
p=Path(__file__).resolve().parents[1]/'certify_public_encoder.py'
spec=importlib.util.spec_from_file_location('public_cap_transform_tdd',p)
m=importlib.util.module_from_spec(spec);sys.modules[spec.name]=m;spec.loader.exec_module(m)

def interval(row):
    v=row['maximum_squared_modulus'];return F(int(v['lower_numerator']),int(v['denominator'])),F(int(v['upper_numerator']),int(v['denominator']))

class RootTransformModels(unittest.TestCase):
    def test_zero(self):
        r=m.canonical_bounds([0]*8,1)
        self.assertEqual(interval(r),(0,0));self.assertEqual(r['status'],'ENCODER_CAP_CERTIFIED')
    def test_monomial(self):
        r=m.canonical_bounds([0,1]+[0]*6,1);a,b=interval(r)
        self.assertLessEqual(a,1);self.assertGreaterEqual(b,1)
        self.assertLess(b-a,F(1,1<<160));self.assertEqual(r['status'],'ENCODER_CAP_REFUTED')
    def test_nontrivial_exact_N4(self):
        a,b=interval(m.canonical_bounds([3,1,1,2],1))
        self.assertLessEqual(a,15);self.assertGreaterEqual(b,15)
        self.assertLess(b-a,F(1,1<<160))
    def test_centering_is_not_canonical_contraction(self):
        a,b=interval(m.canonical_bounds([-2,1,1,2],1))
        # Compare against a rigorous rational sqrt(2) enclosure, not floating sqrt.
        D=1<<224;t=math.isqrt(2*D*D)
        self.assertLessEqual(a,10+5*F(t+1,D));self.assertGreaterEqual(b,10+5*F(t,D))
        self.assertGreater(a,15);self.assertLess(b-a,F(1,1<<160))
if __name__=='__main__':unittest.main()
