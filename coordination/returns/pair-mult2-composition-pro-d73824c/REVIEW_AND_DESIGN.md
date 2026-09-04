# Review and design: Pair arithmetic result into first Mult2

## 1. Disposition

**CANDIDATE_READY_FOR_CODEX_HOSTED_EXECUTION**

This is a bounded, test-only drafting result. No production defect was found in
the supplied Add/Sub/Mult2 path, and no production correction is included.
The candidate closes one concrete regression gap: a genuine
`ReadyForFirstMult` result of public Pair `Add` or `Sub` is now proposed as the
left input to the first public `Mult2`.

The static result is not a compiled or executed result. Final integration and
acceptance remain Codex's responsibility.

## 2. Actual reviewer and environment

- Actual model: **GPT-5.6 Pro**.
- Working environment: isolated Linux x86-64 container,
  kernel `6.18.35`, Python `3.13.5`, Git `2.47.3`.
- No other agent was dispatched.
- No external message, repository push, merge, CI dispatch, rerun or cancel was
  performed.
- No OpenFHE/project configure, C++ compile, CTest, encryption, decryption,
  benchmark or dependency installation was performed.
- Scratch Git metadata was created only outside the supplied project snapshot
  to generate and replay raw patches. It is not included in the delivery and
  did not touch a supplied `.git` directory.

## 3. Exact scope and source identity

- Input archive: `pair-mult2-composition-d73824c.zip`
- Input bytes: `1,361,787`
- Input SHA-256:
  `5eda78d7d49f0eb3ed565c0d18577608027ce31db1c5a165906487b1d79150d6`
- Current documentation head:
  `1610ee522a39949a3f50a08e08ef3a9a8bcc126c`
- Selected tested production/tests source:
  `d73824c2d382013c3aadbd7cb29c57008e839714`
- Branch identity: `codex/integration-01`
- Official pristine OpenFHE pin:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Nested complete context archive bytes: `1,305,833`
- Nested context SHA-256:
  `e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce`

The outer ZIP contains 11 safe regular members and passes CRC checking. Its 10
manifest-declared payloads match their exact byte sizes and hashes. The nested
context ZIP contains no duplicate, unsafe or symlink member and passes CRC
checking. Its 70 manifest-declared payloads match their exact sizes and hashes.
Both manifests explicitly exclude themselves. The input archive was rehashed
again after the review work with the same SHA-256.

Machine-readable identity and closure are in `INPUT_OUTPUT_MANIFEST.json`.

## 4. Source finding before drafting

### 4.1 Paper mapping

The supplied paper provides the required algebraic seams:

1. Section 2.1 defines ordinary ciphertext addition and subtraction as
   `[ct + ct']_Q` and `[ct - ct']_Q`, with left-minus-right subtraction.
2. The introductory paragraph of Section 4 states that multi-precision Pair
   addition and subtraction are performed componentwise on Pair
   representations.
3. The multiplication construction is `Mult2 = RS2 o Relin2 o Tensor2`.

Therefore, for Pair representations `(high, low)`, the relevant first-input
semantics are exactly:

```text
Add: (high_A + high_B, low_A + low_B)
Sub: (high_A - high_B, low_A - low_B)
```

No low-low multiplication rule is introduced by Add/Sub. The existing first
`Mult2` remains the only multiplication exercised by this slice.

### 4.2 Current production source

The selected source already implements the correct path:

- `DoubleCKKS::Add` validates left, validates right, validates compatibility,
  clones the corresponding left high and low ciphertexts, and applies direct
  DCRT `+=` to matching members.
- `DoubleCKKS::Sub` follows the same order and applies direct left-minus-right
  DCRT `-=`.
- Both construct a fresh Pair with the left Pair's validated descriptors and
  call `ValidatePair(result)`.
- `DoubleCKKS::Mult2` remains literally
  `RS2(Relin2(Tensor2(left, right)))`.

No hidden rescale, automatic alignment, key switch, normalization, refresh or
second multiplication was found in Add/Sub. Because this task is test-only,
none of those production bodies or their header signatures is changed.

### 4.3 Confirmed gap

The existing 53-test suite exercises Pair Add/Sub at the three Pair lifecycle
states, and separately exercises first Mult2, but it does not feed an Add/Sub
**result** at `ReadyForFirstMult` into that multiplication. This candidate adds
only that missing composition observation.

## 5. Proposed patch sequence

### 5.1 Patch 0001: Add result into first Mult2

`patches/0001-add-input-first-mult2-regression.patch` adds:

- CTest name `mult2_pair_add_input_hybrid_complex`;
- selector `pair_add_input_hybrid_complex`;
- frozen A, B, C, A+B and `(A+B)*C` literals;
- a narrow shared Pair-input fixture;
- an exact independent Pair recombination oracle;
- state, mutation, key-presence, staged/direct composition and certificate
  checks.

Static expected registration count after this patch: **54**. This is not an
observed CTest count.

### 5.2 Patch 0002: Sub result into first Mult2

`patches/0002-sub-input-first-mult2-regression.patch` preserves the Add case and
adds:

- the `Sub` branch of the narrow operation selector;
- explicit `BigInt(left - right)` materialization;
- frozen A-B and `(A-B)*C` literals;
- CTest name `mult2_pair_sub_input_hybrid_complex`;
- selector `pair_sub_input_hybrid_complex`.

Static expected registration count after this patch: **55**. This is not an
observed CTest count.

### 5.3 Aggregate equivalence

`patches/candidate-final.patch` is the exact baseline-to-final aggregate. It is
provided for inspection or one-step application and must not be applied after
the numbered patches.

Both numbered patches were applied sequentially to a fresh copy of the exact
selected project bytes. A separate fresh copy received the aggregate patch.
Each resulting 54-file project tree was byte-for-byte equal to the proposed
final tree.

## 6. Exact public flow in each new case

Each selector runs as a separate executable process and therefore creates a
fresh context/key fixture:

1. Create the existing diagnostic context with:
   - ring dimension 64;
   - batch size 16;
   - multiplicative depth 7;
   - scaling modulus size 30;
   - first modulus size 35;
   - `FIXEDMANUAL`;
   - `HYBRID`;
   - digit size 0;
   - maximum relinearization secret-key degree 2;
   - `UNIFORM_TERNARY`;
   - `HEStd_NotSet`;
   - `COMPLEX`.
2. Assert the generated context reports `COMPLEX`.
3. Generate a fresh key pair and call `EvalMultKeyGen`.
4. Construct three literal complex plaintexts A, B and C at the existing input
   noise-scale degree 2 and level 0.
5. Before encryption, assert that the cached first-slot imaginary components
   are `-0.0625`, `+0.03125` and `+0.125`, respectively. This detects a REAL
   fixture that silently discards the imaginary witness.
6. Encrypt all three plaintexts.
7. Call public `DCP` on A, B and C.
8. Snapshot all three encrypted inputs and all three original Pairs before the
   Add/Sub-plus-Mult2 window.
9. Independently decrypt the original A and B Pairs through the existing
   CRT/secret-key oracle.
10. Call public `Add(A,B)` or `Sub(A,B)`.
11. Require the composed result and C to retain the exact first-multiplication
    input contract.
12. Independently decrypt the composed Pair and check its recombination against
    the centered sum/difference of the two original Pair recombinations.
13. Snapshot the composed Pair before multiplication.
14. Assert that the fixture evaluation-key row exists; do not remove it.
15. Exercise both:
    - public `Tensor2 -> Relin2 -> RS2`;
    - public `Mult2(composed, C)`.
16. Compare staged and direct ciphertext results only as composition-wiring
    evidence.
17. Reuse the existing independent coefficient/negacyclic product certificate
    and conditional per-path Relin2 checks on the composed Pair and C.
18. Call the existing public-RCB decoded-slot observation against the literal
    `(A+B)*C` or `(A-B)*C` array with the unchanged `1e-3` tolerance.
19. Recheck all original encrypted inputs, all original Pairs and the composed
    Pair against the existing enumerated nonmutation snapshots.
20. Confirm the fixture evaluation-key row is still present.
21. Reuse `PrintCertificate`, which prints the distinct case name, actual
    HYBRID/COMPLEX identity and observed errors while preserving:
    - `execution_certificate=PER_PATH_CONDITIONAL`;
    - `conservative_E_Relin_available=false`;
    - `universal_theorem_gate=UNPROVED`.

## 7. Independent oracle derivation

### 7.1 Pair-input recombination oracle

For coefficient index `j`, let the existing exact CRT/secret-key oracle produce
centered Pair recombinations:

```text
x_A[j] = Center(q_div * high_A[j] + low_A[j], Q)
x_B[j] = Center(q_div * high_B[j] + low_B[j], Q)
```

The new expected composed coefficient is computed from the two **original**
operand oracle results:

```text
Add expected[j] = Center(BigInt(x_A[j] + x_B[j]), Q)
Sub expected[j] = Center(BigInt(x_A[j] - x_B[j]), Q)
```

The `BigInt` additions and subtractions are explicitly materialized in separate
branches. No Boost conditional expression template crosses the branch boundary.
The actual composed value is obtained by independently decrypting the public
Add/Sub result. Expected values do not come from:

- production Add or Sub;
- public RCB;
- OpenFHE `EvalAdd`, `EvalSub` or `EvalMult`;
- a DCRT operation result reused as the expected value;
- an object cache copied from the result.

`IndependentDecryptPair` itself reconstructs every source member through the
existing exact tower/CRT/secret-key machinery and validates high/low basis
agreement.

### 7.2 First-Mult2 coefficient certificate

After the Pair-input oracle has established the exact composed recombination,
the existing `CheckIndependentArithmetic` is reused unchanged on
`(composed, C)` and on the staged intermediates. It independently:

- decrypts the two Pair inputs through the exact CRT/secret oracle;
- computes the integer negacyclic product of their recombined coefficients;
- checks actual execution-specific non-wrap;
- separately measures high-path and low-path Relin2 errors;
- applies the existing per-path conditional Relin2 gate;
- checks the first-Mult2 output coefficient error expression;
- does not use public RCB as the coefficient expected-value path.

This is an execution-specific conditional certificate when the test is run. It
is not a conservative universal BV `E_Relin` theorem.

### 7.3 Staged equality is not the oracle

The equality

```text
Mult2(composed, C) == RS2(Relin2(Tensor2(composed, C)))
```

is retained only to check public composition wiring. Because production Mult2
is defined by the same staged operations, that equality is not treated as an
arithmetic expected-value oracle. Arithmetic acceptance instead depends on the
independent Pair recombination, negacyclic coefficient certificate and frozen
host product arrays.

### 7.4 Public decoded-slot observation

The existing `CheckDecodedSlots` calls public `RCB(result)`, decrypts it, applies
the already documented logical-to-recorded scale ratio, and compares the
logical slots against the literal final product vector with the unchanged
absolute tolerance `1e-3`.

That decoded double path is a functional observation only. It is not an exact
coefficient oracle, not a shipping codec and not evidence of greater-than-53-bit
precision. Tiny vector slots are controls; the tolerance cannot establish their
bit-level accuracy.

## 8. Frozen host vectors and exact arithmetic

The seven arrays in `FROZEN-HOST-VECTORS.json` were independently parsed as
binary64 values and converted with `Fraction.from_float`. For each of eight
slots, exact rational complex arithmetic verified:

```text
sum             = A + B
difference      = A - B
sumTimesC       = sum * C
differenceTimesC= difference * C
```

This is 32 exact complex identities, or 64 real/imaginary scalar equalities.
The final C++ literal-return functions were then parsed independently and all
56 complex literals were compared to the JSON values as exact rationals.

Maximum L1 values are:

```text
A           3/8
B           3/16
C           3/4
A+B         7/32
A-B         9/16
```

Thus all required source/composed vectors remain inside the existing unit L1
envelope. `3/4` is the maximum when C is included; `9/16` is the maximum of the
sum/difference vectors alone. No envelope or test threshold was enlarged.

Slot 0 gives exact discriminators:

```text
(A+B)*C       = (-11/256,  1/32) = (-0.04296875, 0.03125)
(A-B)*C       = ( -1/256,  1/32) = (-0.00390625, 0.03125)
A*C if B lost = ( -3/128,  1/32) = (-0.0234375,  0.03125)
(B-A)*C       = (  1/256, -1/32)
```

Full exact fractions are recorded in `HOST_VECTOR_VERIFICATION.json`.

## 9. State and lifecycle assertions

Before Tensor2/Mult2, the candidate requires both the composed Pair and C to
have:

- lifecycle `ReadyForFirstMult`;
- the exact expected context identity;
- unchanged divisor;
- identical ordered active RNS basis;
- level 1;
- recorded scaling factor equal to the reference DCP Pair;
- noise-scale degree 2;
- identical key tag;
- identical slots;
- evaluation format;
- two RLWE components in each high/low member;
- all four paper/logical scale descriptor values unchanged.

It also reuses `CheckPhysicalCiphertextState` for each high/low member, including
context, CKKS encoding, level, degree, recorded scale, key tag, slots, two
components, evaluation format and every active tower modulus in order.

The output remains required to be `RefreshRequired` through the unchanged
`CheckPublicResultState`. No result is fed to a second multiplication.

## 10. Mutation, key and provenance boundary

### 10.1 Snapshots taken

Before the Add/Sub-plus-Mult2 window, the test snapshots:

- original encrypted A;
- original encrypted B;
- original encrypted C;
- original DCP Pair A;
- original DCP Pair B;
- original DCP Pair C.

After public Add/Sub and before multiplication, it additionally snapshots the
composed Pair. The Pair wrapper supplements the existing Mult2 `PairSnapshot`
with all paper/logical scale fields.

After staged and direct multiplication plus the independent/public observations,
all snapshots are checked using the existing enumerated contract.

### 10.2 Evaluation key

The fixture calls `EvalMultKeyGen` and verifies the row for the fixture key tag
exists before and after composition. It deliberately does **not** remove that
row, because Relin2/Mult2 requires it.

This candidate does not deep-snapshot every key object or every hidden cache.
The key assertions establish required row presence, not an exhaustive key-cache
nonmutation theorem.

### 10.3 Exact blind spots

The existing Mult2 ciphertext snapshot retains the original pointer identity
and a `Clone()` for semantic equality. It does not independently serialize or
instrument every hidden OpenFHE field. Official `Clone` behavior can share
metadata-entry pointers and parameter provenance. Consequently, the test covers
the enumerated per-call values and descriptors exercised here, but it does not
establish:

- arbitrary future caller-mutation isolation;
- absence of all hidden read-only cache accesses;
- allocator or global precomputation invariance;
- every unknown metadata subclass;
- concurrency or data-race behavior;
- every parameter object's internal field independently of shared provenance.

No broader claim is made.

## 11. Defect discrimination

| Candidate defect | Discriminating evidence | Boundaries |
|---|---|---|
| Drop B and multiply only A by C | Exact Pair recombination oracle fails before Mult2; slot 0 final differs by `5/256` in real part | Does not identify which internal Add component was dropped without inspecting the earlier coefficient mismatch |
| Implement Sub as B-A | Exact centered source coefficient subtraction fails; slot 0 final becomes `(1/256,-1/32)` | Add is commutative, so operand order is meaningful only for Sub |
| Use Add in the Sub case | Pair recombination oracle fails; slot 0 products differ by `5/128` in real part | A defect contrived to preserve all exact coefficients is observationally equivalent on this fixture |
| Swap high and low members | Existing independent high/low Pair machinery and later product certificate encounter different recombination/product coefficients because the members are distinct | No single witness is claimed to detect every coordinated compensating transformation |
| Omit one RLWE component | Exact secret-key decryption/product certificate changes; state also requires two components | A mathematically zero omitted component in another fixture could be invisible, but these are genuine nonzero encrypted inputs |
| Return the wrong lifecycle | Explicit `ReadyForFirstMult` assertion fails before Tensor2 | Does not diagnose the internal line that changed the descriptor |
| Hidden rescale/alignment | Basis, level, degree, recorded scale, logical scale and physical tower checks fail | Shared provenance blind spots remain as listed above |
| Mutate A, B, C or the composed Pair | Post-window snapshot checks fail for enumerated values/metadata | Not exhaustive hidden-state or concurrency proof |
| Delete or omit the fixture evaluation key | Presence assertion or Relin2/Mult2 execution fails | Presence alone does not deep-prove key-value immutability |
| Make Mult2 diverge from its public staged definition | Staged/direct equality fails | Equality alone is not used as the arithmetic oracle |
| Return numerically wrong first-Mult2 output | Independent negacyclic coefficient certificate and/or literal decoded-slot observation fails | Decoded `1e-3` path alone is not high-precision evidence |

## 12. Existing-contract preservation

Static diff inspection establishes:

- changed project paths are exactly `CMakeLists.txt` and
  `tests/mult2_e2e_oracle_test.cpp`;
- no production/header/workflow/upstream/other-test path changes;
- no existing CMake test registration is deleted or changed;
- all 53 baseline name-command bindings remain byte-logically identical and in
  the same order after each stage;
- patch 0001 deletes only the old usage-string line needed to append the Add
  selector;
- patch 0002 deletes only the stage-1 usage-string line needed to append the Sub
  selector;
- no old assertion, four-case body, frozen old vector, context constant,
  certificate, threshold or backend line is deleted;
- the new frozen vector functions are invoked only after all four existing case
  branches, so those cases do not construct the new vectors;
- no new include or dependency is added;
- `kLogicalDecodedAbsoluteTolerance` remains exactly `1.0e-3`;
- the final CMake file still declares 15 executable targets, and all 55 test
  commands resolve to one of those targets;
- the inherited Mult2 end-to-end target still uses
  `-Wall -Wextra -Wpedantic -Werror` on non-MSVC builds;
- the inherited workflow still contains both-host explicit builds for Relin2,
  RS2, Mult2, Add and Sub API contract targets.

The complete closure is in `CMAKE_TEST_CLOSURE.json` and
`CMAKE_TEST_CLOSURE.md`.

## 13. Source-line map in the final changed test

The supplied final changed file places the new material at approximately:

- operation dispatch: lines 1175-1191;
- explicit BigInt coefficient oracle: lines 1193-1206;
- Pair recombination oracle: lines 1208-1229;
- frozen host-literal check: lines 1231-1257;
- supplemental Pair snapshot: lines 1259-1291;
- first-multiplication state check: lines 1293-1346;
- shared public composition fixture: lines 1348-1474;
- A/B/C/Add literals: lines 1476-1529;
- Sub literals: lines 1531-1553;
- new selectors: lines 1595-1614;
- usage closure: lines 1621-1625.

CMake registrations are at final `CMakeLists.txt` lines 188-191.

Line numbers are descriptive for the delivered final files; patch context and
hashes are authoritative.

## 14. CMake and hosted execution plan

The two exact new bindings are:

```text
mult2_pair_add_input_hybrid_complex
  -> mult2_e2e_oracle_test pair_add_input_hybrid_complex

mult2_pair_sub_input_hybrid_complex
  -> mult2_e2e_oracle_test pair_sub_input_hybrid_complex
```

After each patch, Codex can use the existing hosted build configuration and run
the focused test before the full suite. Representative pending commands are:

```bash
cmake --build build --target mult2_e2e_oracle_test --parallel 2
ctest --test-dir build --output-on-failure \
  -R '^mult2_pair_add_input_hybrid_complex$'
ctest --test-dir build --output-on-failure
```

After patch 0002:

```bash
cmake --build build --target mult2_e2e_oracle_test --parallel 2
ctest --test-dir build --output-on-failure \
  -R '^mult2_pair_(add|sub)_input_hybrid_complex$'
ctest --test-dir build --output-on-failure
```

The candidate does not report those commands as executed.

## 15. Supplied baseline evidence versus this candidate

The retained logs state that exact source `d73824c` passed the existing 53 cases
on both hosts under run `33854419062`:

- Linux job `100964299802`: supplied `53/53`, `0.68 s`;
- Windows job `100964299593`: supplied `53/53`, `2.27 s`.

Those are supplied hosted records and were not generated by this reviewer.
They establish only the old baseline. They do not establish that either new
54th or 55th case compiles or passes.

## 16. Risks and remaining limits

1. **Compilation remains pending.** Static source/API inspection gives a
   plausible candidate, not a compiler result. GCC and MinGW64 warning builds
   are required.
2. **Runtime remains pending.** The exact CRT, per-path and decoded assertions
   must run on both hosted platforms.
3. **No new missing-feature red-green claim.** Add, Sub and first Mult2 already
   have authentic earlier red/green histories. These are additional regression
   cases. A natural failure, if observed, should be retained before any fix;
   shipping code must not be deliberately broken to manufacture a red.
4. **Ordinary-precision diagnostic context only.** Ring dimension 64 and
   `HEStd_NotSet` are functional test settings, not paper Table 3 or a security
   parameter set.
5. **No precision theorem.** The public decoded observation uses doubles and
   `1e-3`; no 53-bit, 106-bit or other precision claim follows.
6. **No universal BV bound.** The retained certificate is per-path and
   execution-conditional; `conservative_E_Relin_available=false` and
   `universal_theorem_gate=UNPROVED` remain mandatory.
7. **No repeated multiplication.** Both cases stop after the first Mult2 at
   `RefreshRequired`.
8. **No overall project acceptance.** High-precision I/O, repeated lifecycle,
   conservative BV error bounds, paper-scale parameters, security and
   performance remain open tasks.

## 17. Requirement-to-candidate mapping

| Required point | Candidate location/evidence |
|---|---|
| Two stages, Add then Sub | numbered patches 0001 and 0002 |
| Exact CTest names/selectors | final CMake lines 188-191; selector lines 1595-1614 |
| Fresh HYBRID/COMPLEX context and key | shared runner lines 1369-1374 |
| Literal imaginary value survives before Encrypt | lines 1383-1393 |
| DCP all three then public Add/Sub | lines 1400-1418 |
| ReadyForFirstMult and exact metadata | `CheckReadyForFirstMultInputState`, lines 1293-1346 |
| Exact source-pair recombination +/- oracle | lines 1193-1229, 1416-1426 |
| Materialized BigInt branches | lines 1197-1204 |
| Staged and public Mult2 paths | lines 1438-1441 |
| Independent negacyclic/per-path certificate | existing helper invoked lines 1449-1450 |
| Public RCB decoded literal product | existing helper invoked lines 1451-1452 |
| Original/composed nonmutation snapshots | lines 1406-1414 and 1454-1466 |
| Keep required EvalMultKey | generation line 1374, presence lines 1430-1433 and 1467-1470 |
| Distinct labels and conditional certificate text | selector names plus unchanged `PrintCertificate` invoked lines 1472-1473 |
| Frozen vectors and envelope | lines 1360-1367 and 1476-1553; exact JSON report |
| Existing cases and thresholds preserved | diff deletion audit and exact CMake closure |
| No production/workflow change | patch path closure and full-project byte replay |
| Honest execution boundary | `EXECUTION-LEDGER.md` |

## 18. Final bounded conclusion

The supplied production source already supports the requested composition. The
smallest defensible continuation is therefore test-only. The two proposed
patches add genuine complex encrypted Add-result and Sub-result first-Mult2
observations, use independent source-pair and product oracles, preserve every
old CTest binding and avoid any production, workflow or threshold change.

The candidate is ready for Codex to integrate and execute one stage at a time.
It is not yet a green result.
