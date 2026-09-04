# TEST PLAN — fixed-key BV bound candidate

## 1. Candidate status

The supplied code is a **test-only PROBE**, not an adopted green patch.

It changes only:

`project/tests/mult2_e2e_oracle_test.cpp`

It does not add a public seam, production helper, CMake target, CTest registration, threshold, backend, rescale, refresh, or exception suppression. Existing `Tensor2`, `Relin2`, `RS2`, and `Mult2` calls remain the public pipeline under test.

The candidate must remain labelled conditional because the input packet does not contain the exact implementation or an authoritative all-residue contract for `NativePoly::SwitchModulus`. No artificial missing-feature RED is supplied: production behavior already exists and the new work is additional proof-oriented regression coverage.

## 2. Exact fixed fixture

| Item | Value |
|---|---|
| OpenFHE | pristine 1.5.0 pin `df495ba2e91739a6dc8f1de254fc5a41155ce504` |
| tested project source | `9bf86cb53a1bbae3a3627fe5efc385d2a29c89ce` |
| ring dimension | `N=64` |
| batch size | 16 |
| host vector length | 8 |
| CKKS data types | REAL and COMPLEX |
| `p` / scaling mod size | 30 |
| first mod size | 35 |
| multiplicative depth | 7 |
| scaling technique | `FIXEDMANUAL` |
| key switch | BV for candidate; HYBRID retained regression |
| BV digit size | 0 |
| max relinearization secret-key degree | 2 |
| secret distribution | `UNIFORM_TERNARY` |
| security level | `HEStd_NotSet` — functional only |
| active rows/towers | 7 |
| full raised-high towers | 8 |
| `q_div` | 1,073,741,953 |
| `q_l` | 1,073,741,441 |
| `Q_l` | 52,656,049,226,897,061,758,347,970,843,194,892,279,389,197,066,160,739,197,584,863,617 |
| decoded functional tolerance | frozen `1e-3` |
| `h` | measured from each fresh ternary key; not frozen |
| `noiseScale` | read from the runtime crypto parameters and printed; not inferred from retained logs |

The exact ordered active moduli and digit radii are listed in `BOUND-DERIVATION.md` and checked by `evidence/static_witnesses.py`.

## 3. Independent oracle design

### 3.1 Pre-ciphertext key-only calculation

For BV cases, the candidate executes these steps immediately after `KeyGen` and `EvalMultKeyGen`, before plaintext construction, encryption, Tensor2, Relin2, or any observed ciphertext error (`candidate/project/tests/mult2_e2e_oracle_test.cpp:1471-1482`):

1. run a deterministic centered-digit boundary probe;
2. retrieve the fixed multiplication evaluation key by key tag;
3. freeze copies of its A and B vectors;
4. restrict rows 0 through 6 to `Q_l`;
5. independently compute `b_i + a_i*s - G_i(s^2)` using coefficient-form tower residues, schoolbook negacyclic multiplication, `cpp_int` CRT, and centered reconstruction;
6. compute

   \[
   B_{\mathrm{path}}=N\sum_i\lfloor q_i/2\rfloor\|\rho_i\|_\infty
   \]

   and `B_pair=2*B_path`;
7. verify that the evaluation key is byte/value unchanged.

The bound does not read `highPathRelinError`, `lowPathRelinError`, `pairRelinError`, plaintext coefficients, ciphertext components, decoded slots, or the accepted result.

### 3.2 Existing independent ciphertext arithmetic

The current test already uses:

- coefficient-form tower extraction;
- exact `cpp_int` CRT reconstruction;
- independent `c0+c1*s+c2*s^2` evaluation;
- schoolbook negacyclic multiplication;
- independent pair recombination;
- exact integer input-product comparison;
- per-path ordinary-Relinearize identity checks;
- explicit coefficient and decoded-slot checks.

Those oracles and the frozen final execution-bound assertion remain unchanged. Production RCB is used only for the separately labelled decoded path, not for the coefficient expected values.

## 4. Deterministic static witnesses

### S1 — exact modulus and unit witness

Script: `evidence/static_witnesses.py`

Assertions:

- exact active moduli, `q_div`, `q_l`, `Q_l`, bit length, and scale ratio;
- each modulus is odd;
- `gcd(q_div,q_i)=1` for every active row;
- exact `floor(q_i/2)` values;
- exact `N*sum D_i = 1,305,669,939,200`.

Purpose: fixes the domain and proves that multiplying active high towers by `q_div` does not shrink their residue domain.

### S2 — factor-`N` exhaustive ring witness

Toy ring: `Z_15[X]/(X^2+1)`, source moduli 3 and 5, fixed residual rows

```text
rho_0=(1,-1)
rho_1=(-1,1)
```

The script exhausts all 225 digit-pair combinations. The exact maximum is 6, equal to

```text
N*(D_0*||rho_0|| + D_1*||rho_1||) = 2*(1+2) = 6.
```

A formula omitting `N` gives 3 and is falsified.

### S3 — centered-lift gap witness

For source modulus 3 and residue 2:

```text
centered lift = -1
canonical lift = 2
D=floor(3/2)=1
```

Purpose: proves that the row residual alone is not sufficient; the cross-modulus lift contract is independently necessary.

### S4 — wrap-versus-triangle witness

For `Q=101`:

```text
40 + 40 -> Center_Q(80) = -21
|-21| <= |40|+|40|
```

Purpose: prevents the modular triangle inequality from being mislabelled as a no-wrap proof.

All four static witnesses were executed. Their exact output is retained in `evidence/static-witnesses.txt`.

## 5. Runtime probes embedded in existing BV tests

No new CTest name is introduced. The assertions run inside:

- `mult2_e2e_bv_real`;
- `mult2_e2e_bv_complex`.

### P1 — `CheckBvCenteredDigitLiftBoundaryProbe`

Candidate lines 228-281.

For every active source tower, construct exact source residues at four decisive points:

```text
0
floor(q_i/2)
floor(q_i/2)+1
q_i-1
```

Expected signed lifts:

```text
0
+floor(q_i/2)
-floor(q_i/2)
-1
```

For every target tower, assert that `CRTDecompose(0)` emits those integers modulo the target modulus.

Independence: expected values are mathematical constants from the centered-lift contract, not copied from a production pair error.

Limitation: this is a boundary regression probe, not proof over all `q_i` residues. Promotion still requires inspection of the exact pinned `SwitchModulus` implementation or a normative postcondition.

### P2 — `BuildFixedKeyBvBound`

Candidate lines 283-413.

Assertions:

- backend is exactly BV with `digitSize=0`;
- full basis has active `Q_l` followed by `q_div`;
- secret and eval-key row bases and row counts are exact;
- `q_div` is a unit in every active source tower;
- exactly one multiplication evaluation key is selected for `maxRelinSkDeg=2`;
- row residual is reconstructed independently in `Q_l`;
- exact floor digit radius is used;
- `noiseScale` is recorded but not multiplied into a residual that already contains it;
- evaluation-key A/B vectors are unchanged.

### P3 — `CheckBvRelin2DigitDomains`

Candidate lines 589-613.

Assertions on the actual Tensor2 result before Relin2:

- raised-high `c2` decomposes into 8 digits;
- low `c2` decomposes into 7 digits;
- every coefficient in every tower of raised-high digit 7 is zero.

This separates “allocated full-basis rows” from “effective rows” and prevents accidental inclusion of the `q_div` row in the active pair bound.

### P4 — `CheckFixedKeyBvApplication`

Candidate lines 1196-1266.

Assertions:

1. bound basis and ring equal the actual public-pipeline basis and ring;
2. `B_path < Q_l/2` and `B_pair < Q_l/2`;
3. independent high-path error `<= B_path`;
4. independent low-path error `<= B_path`;
5. independent pair error `<= B_pair`;
6. strong pre-RS target-plus-bound inequality passes;
7. the existing independent final coefficient error satisfies

   \[
   \frac{NM_{low}^2}{q_{div}q_l}
   +\frac{B_{pair}}{q_l}
   +\frac{h+1}{2};
   \]

8. the final target plus conservative error is below `Q_{l-1}/2` using the exact cross-multiplied integer inequality.

The direct pair term has no extra `+h`; the existing RS2 `(h+1)/2` term is retained.

### P5 — labels

Required BV output additions:

```text
fixed_key_bv_bound_available=true
fixed_key_bv_bound_status=CANDIDATE_FIXED_KEY_CIPHERTEXT_UNIFORM_CONDITIONAL_ON_CENTERED_DIGIT_LIFT
fixed_key_bv_noise_scale=<actual>
fixed_key_bv_active_rows=7
fixed_key_bv_raised_high_q_div_digit=ZERO
fixed_key_bv_row_residual_norms=<seven values>
fixed_key_bv_per_path_raw_bound=<integer>
fixed_key_bv_pair_raw_bound=<integer>
fixed_key_bv_integer_lift_nonwrap=true
fixed_key_bv_unconditional_gaussian_key_bound=false
fixed_key_bv_universal_theorem_gate=UNPROVED
centered_digit_boundary_probe=PASSED
```

Required existing labels, unchanged:

```text
execution_certificate=PER_PATH_CONDITIONAL
conservative_E_Relin_available=false
universal_theorem_gate=UNPROVED
```

HYBRID output must not claim the BV bound.

## 6. Claim-to-test matrix

| Claim | Independent basis of expectation | Mandatory assertion | What failure means |
|---|---|---|---|
| BV row sign/scale is `-ns*e_i` | pinned key-generation equation | row residual is computed as `b+a*s-gadget`; `ns` printed but not reapplied | source/derivation mismatch or test bug |
| one digit per active tower | pinned `CRTDecompose(0)` branch | low count 7; high count 8 | basis/decomposition mismatch |
| high `q_div` digit contributes zero | production appends a zero tower; mathematical lift of zero | all coefficients/towers of final high digit are zero | invalid domain reduction; candidate must stop |
| centered digit radius is `floor(q_i/2)` | independent boundary values | P1 for all active source/target towers | missing centered mapping; bound not adoptable |
| row residuals are key-only | call order and static inspection | bound built before plaintext/encryption | circular certificate design |
| fixed-key path bound covers all ciphertexts in domain | theorem in `BOUND-DERIVATION.md` | actual independent high and low errors are each `<=B_path` | derivation premise, basis, oracle, or backend mismatch |
| pair bound has no extra `h` | direct sum of two path errors | actual pair `<=2B_path` | recombination/path identity or derivation mismatch |
| centered norm is not mistaken for no-wrap | exact half-modulus conditions | `B_pair<Q_l/2`, pre-RS and final lift inequalities | only modular evidence is available; no integer theorem claim |
| final product normalization | independent integer input product | exact numerator uses `q_div*q_l` | scale algebra mismatch |
| existing behavior remains | frozen vectors/contracts | all 44 tests and API targets pass | regression; reject candidate |
| no universal/precision overclaim | exact output labels | old false/unproved labels retained | claim-boundary regression |

## 7. Hosted build and test commands — NOT EXECUTED here

### 7.1 Linux GCC

Against the exact pristine install:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$OPENFHE_PREFIX"
cmake --build build --parallel 2
cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
cmake --build build --target mult2_api_contract_test --parallel 2
ctest --test-dir build --verbose --output-on-failure
```

Focused first failure:

```bash
ctest --test-dir build -R '^mult2_e2e_bv_real$' --verbose --output-on-failure
```

Both BV cases:

```bash
ctest --test-dir build -R '^mult2_e2e_bv_(real|complex)$' \
  --verbose --output-on-failure
```

### 7.2 Windows MinGW64 / MSYS2

Use the packet workflow's exact path setup, then:

```bash
prefix="$(cygpath -u "$OPENFHE_PREFIX")"
build="$(cygpath -u "$PROJECT_BUILD")"
cmake -S . -B "$build" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$prefix"
cmake --build "$build" --parallel 2
cmake --build "$build" --target relin2_api_contract_test --parallel 2
cmake --build "$build" --target rs2_api_contract_test --parallel 2
cmake --build "$build" --target mult2_api_contract_test --parallel 2
export PATH="$prefix/bin:$prefix/lib:$PATH"
ctest --test-dir "$build" --verbose --output-on-failure
```

The supplied CMake applies warning-as-error settings to the oracle test and production targets (`project/CMakeLists.txt:85-100`).

## 8. Fresh-key falsification loop — planned, not theorem evidence

Each direct test-binary invocation generates a fresh context, secret key, and evaluation key. After the mandatory single full run passes, use a predeclared finite repetition count without changing assertions. One practical probe is 16 fresh processes per data type and host:

```bash
for i in $(seq 1 16); do
  build/mult2_e2e_oracle_test bv_real || exit 1
  build/mult2_e2e_oracle_test bv_complex || exit 1
done
```

On Windows, invoke the corresponding `.exe` under MSYS2.

Purpose:

- expose key-dependent row residual, sign, basis, digit, and no-wrap mistakes;
- show that the candidate is not tuned to one retained key or observed maximum.

It is **not** a statistical proof, confidence interval, DGG tail estimate, or replacement for the all-residue `SwitchModulus` contract. The count 16 is an engineering falsifier only.

## 9. Fail-fast criteria

Reject or retain the candidate as unproved immediately if any of these occurs:

1. outer/nested hashes or manifest closure differ;
2. baseline test file is not SHA-256 `b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26`;
3. pristine OpenFHE pin differs;
4. warning-as-error compilation fails;
5. any centered-lift boundary value differs;
6. high/low digit counts differ or the final high digit is nonzero;
7. active row count is not seven, basis order differs, or `q_div` is not a unit;
8. `B_path` or `B_pair` is only the trivial half-modulus bound;
9. any independent path/pair error exceeds its key-only bound;
10. either no-wrap inequality fails;
11. any of the 44 existing tests fails;
12. any public API target fails;
13. any input, pair, or evaluation-key immutability check fails;
14. any frozen vector, tolerance, rejection, backend, scale normalization, or label is changed;
15. output claims `conservative_E_Relin_available=true`, `universal_theorem_gate=PROVED`, secure parameters, or double precision.

No observed maximum may be copied into the bound, and no failure may be cured by increasing a threshold.

## 10. Promotion decision after hosted validation

### Still PROBE

Retain as PROBE if the runtime assertions pass but the exact pinned `SwitchModulus` all-residue contract is still unavailable. The run then provides implementation evidence for the tested boundary values and keys only.

### Fixed-key conditional green

The patch may be promoted only to a label such as

```text
FIXED_KEY_CIPHERTEXT_UNIFORM_ON_DECLARED_BASIS
```

when:

- the exact pinned `SwitchModulus` implementation/postcondition proves the centered lift for every residue;
- both hosted warning-as-error builds pass;
- all 44 tests and explicit API targets pass;
- fresh-key probes do not expose a mismatch;
- all no-wrap assertions pass;
- the old universal and precision labels remain false/unproved.

Even then, do not rename it a universal `E_Relin`, paper theorem, security proof, or precision certificate.

## 11. Limitations

The plan does not cover:

- a numerical DGG tail probability;
- secure ring dimensions or modulus budgets;
- canonical-embedding error;
- genuine high-precision input/output;
- more than 53 reliable bits;
- refresh or second multiplication;
- the separate RS2 mixed-format branch;
- Add/Sub;
- performance or Table 3 claims.

Those remain separate integration and precision tasks.
