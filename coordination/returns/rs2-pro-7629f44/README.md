# RS2 clean-room engineering handoff

## Status

**Verdict: `READY_FOR_CODEX_INTEGRATION`** — this means the four patches are a
source-reviewed, sequentially replayable integration candidate. It does **not**
mean they have compiled, passed CTest, passed CI, or been accepted.

The required declared upstream start is:

`7629f446517413a3ae65551e7efe51b74fd70f00`

The supplied ZIP did not contain `.git` objects, so this handoff binds to the
exact project bytes in the packet and records the declared commit; it cannot
independently recompute that upstream Git object ID.

## Patch order

1. `patches/0001-R1-wrong-lifecycle-red.patch`
2. `patches/0002-G1-wrong-lifecycle-green.patch`
3. `patches/0003-R2-valid-rs2-oracle-red.patch`
4. `patches/0004-G2-valid-rs2-green.patch`

Apply in that order. The series was replayed from a clean copy of the packet
project, and the replayed result was byte-for-byte equal to `final-tree/` for
the three changed files.

## Vertical slices

| Slice | Production change | Test change | Intended checkpoint |
|---|---|---|---|
| R1 | none | add exact wrong-lifecycle/immutability behavior | scaffold fails with wrong exception type/message |
| G1 | validation plus `ReadyForRS2` guard only | none | R1 becomes green; valid input still reaches scaffold |
| R2 | none | add genuine pipeline, independent CRT oracle, state and immutability checks | G1 fails by normal noncompletion |
| G2 | implement Definition 4.5 and terminal validation | none | candidate for R1+R2 and inherited suite green |

## Implemented RS2 mapping

For `CT=(high, low)` on active basis `[q0,...,q_l]`:

```text
rescaledHigh       = Rescale(high)
rescaledRecombined = Rescale(q_div * high + low)
newLow             = rescaledRecombined - q_div * rescaledHigh
RS2(CT)            = (rescaledHigh, newLow)
```

The G2 body contains exactly two output-returning public `Rescale` calls, one
exact `NativeInteger` `EvalMultNoCheck` call, and validated direct DCRT element
subtraction. It contains no `RescaleInPlace`, ciphertext `EvalSub`, production
`try`, or production `catch`.

## Metadata transition

Let `n = |[q0,...,q_l]|`, `q_l` be the final active native modulus, and

```text
f_recorded = GetModReduceFactor(n - 1)
```

Then:

```text
recordedSF_out = recordedSF_in / f_recorded
logicalHigh_out = logicalHigh_in / q_l
logicalRecombined_out = logicalRecombined_in / q_l
level_out = level_in + 1        (1 -> 2)
noiseScaleDegree_out = degree_in - 1  (3 -> 2)
activeBasis_out = [q0,...,q_(l-1)]
lifecycle_out = RefreshRequired
```

`q_l`, `q_div`, and `f_recorded` are kept as separately named and validated
roles. No numerical inequality between `q_l` and `f_recorded` is assumed.

## Evidence boundary

Performed in this handoff:

- ZIP and manifest hash verification;
- complete source inspection within the attachment;
- sequential `git apply --check`/`git apply` replay against packet bytes;
- final-tree equality check;
- diff whitespace checks and static call-pattern checks.

Not performed:

- CMake configure;
- C++ compilation or linking;
- binary execution or CTest;
- Ubuntu/Windows CI;
- GitHub/private-state access;
- any test run for these future patches.

The packet also omits two files referenced by its CMake file,
`tests/tensor2_api_contract_test.cpp` and
`tests/relin2_api_contract_test.cpp`; therefore the packet alone is not a
complete configure/build checkout.

See `SOURCE_FACT_AUDIT.md`, `ORACLE_AND_TEST_CONTRACT.md`,
`STATIC_VERIFICATION.txt`, and `RISK_AND_VERDICT.md` before integration.
