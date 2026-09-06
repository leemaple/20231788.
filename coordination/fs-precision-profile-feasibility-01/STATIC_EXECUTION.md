# Static slice execution ledger

Date: 2026-09-07 Asia/Shanghai. Root source baseline `fb68da960d2bf741fa5ad8643f4a8767268af356`. All commands used bundled Python3.12.14 `-B -I`; no external packages, compile, crypto, transform or numerical full-slot replay.

## Selection, not a randomized experiment

For requested bit sizes58 (two primes) and56 (one divisor), a finite scalar search tried odd j=1..511 for `q=2^bits-j*2^32+1`, and witness integers2..64. A candidate was retained only when `witness^((q-1)/2) == -1 mod q`, under the Proth shape with odd multiplier less than2^32. Each root was `witness^((q-1)/65536) mod q` and both half/full-order identities were checked. This search contains no secrets, key generation, noise samples or encrypted trials.

Selected immutable input is `candidate.json` (SHA256 `94189f13c259778f5c48a0e18d485aceade79645a620c5e223cea6c96a498f3f`). New moduli:288230191468118017,288230165698314241,72057589742960641; witnesses5,7,11. Root selection is for an experimental static candidate, not adoption into production.

## Actual RED → GREEN

The test seam and scope were recorded in `STATIC_SEAM.md` before test creation. The initial one-test load failed with exit1, `ModuleNotFoundError: No module named 'static_profile'` (1 loader error,0.000s), because the module did not exist. This is an honest missing-behavior RED, not a regression in the existing algorithm.

After writing the minimum scalar certificate implementation, the same positive test ran1 test in0.001s and passed, exit0. Four negative regressions were then added for corrupted primality witness, nonprimitive root, incoherent old metadata and duplicate base primes. Existing validation already rejected them; do not describe these as four additional observed RED cycles. The five-test suite ran5 tests in0.002s and passed, exit0.

Test invocation was:

```python
import sys, unittest
sys.path.insert(0, '/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905/coordination/fs-precision-profile-feasibility-01')
result = unittest.TextTestRunner(verbosity=2).run(
    unittest.defaultTestLoader.loadTestsFromName('test_static_profile'))
raise SystemExit(not result.wasSuccessful())
```

The standalone command `python3 -B -I coordination/fs-precision-profile-feasibility-01/static_profile.py` then exited0 and produced the exact stored `STATIC_CERTIFICATE.json`. It uses hex integer strings for large exact scale numerators/denominators to avoid decimal conversion limits. Original Q/P integers in the JSON must be read with arbitrary-precision integers, not JavaScript Number. Root stored the original stdout string directly; an intermediate model-visible JS-parsed summary rounded some >2^53 integers, but that summary was **not** used to write the certificate or candidate. The on-disk exact JSON is authoritative.

Static outputs: scale8/scale0≈1.0000152026932967; terminal Q/scale≈0.9999834266523009; min consumed/divisor≈16.0000009534; total root QP≈711.9999979361bits. All nine exact scale ratios remain within1% ofS0. The positive prospective recombined coefficient-capacity checks assume component error<=T (converted conservatively to complex allowance2T); they do not prove actual E80 or intermediate nonwrap.

Root corrected the capacity explanation from a stronger complex<=T assumption to the actual component<=T implication using2T before the final five-test execution. This is analytical clarity, not evidence of a production defect. `security_status=UNRESOLVED`, `E80_status=NOT_TESTED`, `adoption_status=NOT_ADOPTED`, and `intermediate_nonwrap=NOT_PROVED` remain explicit.

Independent code/mathematical reviews of the root-authored static slice were actually requested from the two existing owners after their initial source-seam and feasibility work. Their findings must be recorded before accepting this slice. No full implementation completion is claimed.

## Review-driven RED → GREEN and final checks

Independent review found that another valid prime/root could reuse the same profile ID, scale tests trusted returned booleans, and family counts were declared rather than derived. Root added regressions before corrections. The seven-test run exited1 in0.008s: two failing substitution subtests (`ValueError not raised`) and one missing-family error (`KeyError: families`); the original five tests remained passing. The closed-product comparisons already passed for the existing scale calculation before the missing-family error; do not claim an arithmetic regression was found.

Root then bound the complete exact `(modulus,root,witness)` triples, derived eight family bases by actual second-last-tower deletion, drove scales from their actual consumed moduli, and used an independent closed-product test with literal frozen consumed order to compare all nine exact scales. Final root suite: **7 PASS,0.005s,exit0**. Standalone execution output was independently compared byte-for-byte with the stored certificate and matched. A repeat after the all-slot premise wording refinement also passed7 tests in0.005s and the byte comparison.

The independent workflow reviewer reran7 tiny tests, PASS in0.012s in its invocation, and independently checked standalone bytes. The mathematical reviewer performed source-only final rereview, without rerunning tests; its earlier independent Fraction computation checked the unchanged mathematical expressions. Both final reviews reported no remaining actionable findings. See `STATIC_ACCEPTANCE.md` for hashes and the precise accepted boundary.
