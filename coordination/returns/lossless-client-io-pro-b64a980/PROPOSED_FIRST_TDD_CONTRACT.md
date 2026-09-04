# Proposed first public-seam TDD contract

**Status:** frozen proposal only. No test code, implementation code, CMake edit, build, or CI run has been performed or authorized. The contract becomes actionable only after the user explicitly confirms `CONFIRM_LOSSLESS_CLIENT_IO_SEAM_V1` and the coordinating workflow records it.

## Contract identity

```text
Proposed executable target : precision_client_io_first_mult2_contract_test
Proposed CTest name        : precision_client_io_first_mult2_contract
Source baseline            : b64a98041c0ca639ef47318f122273f5969caac2
Active tested bytes        : 47907783a6141d0174da79eae264d779fc598f28
OpenFHE pin                : df495ba2e91739a6dc8f1de254fc5a41155ce504
Claim scope                : N64/S16 first-Mult2 public client-I/O diagnostic only
```

## Purpose

Prove one end-to-end public path without using the stale-cache test fixture as a client contract:

```text
ClientReal100 vectors
  -> public HighPrecisionClientIO::Encrypt
  -> retained DoubleCKKS::DCP
  -> retained DoubleCKKS::Mult2
  -> retained DoubleCKKS::RCB
  -> public HighPrecisionClientIO::BindFirstMult2Rcb
  -> public HighPrecisionClientIO::Decrypt
  -> ClientReal100 slots
```

The evaluator receives no secret key and uses no decrypt/re-encrypt step. The independent test oracle remains test-only and is never linked into production.

## Frozen context and state

```text
N=64, M=128, S=16, gap=2
multiplicative depth=7
scaling modulus size=50
first modulus size=55
digit size=0
max relinearization secret-key degree=2
FIXEDMANUAL / HYBRID / COMPLEX / EXEC_EVALUATION
FIXED_NOISE_DECRYPT (asserted; no polynomial-flooding perturbation in this narrow accuracy contract)
UNIFORM_TERNARY / HEStd_NotSet (diagnostic only)
supplied -DMATHBACKEND=4 configuration; compile-time BigInteger alias must be the inspected bigintdyn implementation
fresh exact scale=2^100
fresh OpenFHE metadata: level 0, noise-scale degree 2, two components
first RCB metadata: level 2, noise-scale degree 2, two components
q_div=1125899906843009
q_l=1125899906840833
q_div*q_l=1267650600226646386227681786497
output exact scale=2^200/1267650600226646386227681786497
cryptographic acceptance=absolute complex error <=2^-80
client numerical cross-precision guard <=2^-120
```

The test creates exactly one fresh `KeyGen()` keypair and calls `EvalMultKeyGen(secretKey)` once; it does not freeze secret material. Encryption and key generation remain randomized. The target must first compile-time-confirm that the selected `lbcrypto::BigInteger` alias is the inspected `bigintdyn` implementation used by the exact decimal bridge; a different backend is an unsupported-configuration failure, not a portability assumption. `FIXED_NOISE_DECRYPT` is asserted only so configured polynomial flooding does not alter this narrow `2^-80` accuracy contract—it does not make the cryptographic path deterministic. The test reads and compares the actual ordered moduli and roots from the context/ciphertext; it does not infer basis identity from level alone.

## Exact input vectors

The table defines mathematical decimal/dyadic values exactly. The test constructs them directly in multiprecision without a binary64 intermediate; the fixed `ClientReal` contract then rounds them at 100 decimal digits, far below the `2^-80` error budget.

| slot | left `(real, imag)` | right `(real, imag)` |
|---:|---|---|
| 0 | `(0.125, -0.0625)` | `(0.5, -0.25)` |
| 1 | `(0.125 + 2^-70, -0.0625 + 2^-73)` | `(0.5, -0.25)` |
| 2 | `(0.125, 0.25)` | `(-0.5, 0.125)` |
| 3 | `(0.123456789012345678901234567890, -0.234567890123456789012345678901)` | `(-0.271828182845904523536028747135, 0.314159265358979323846264338327)` |
| 4 | `(-0.314159265358979323846264338327, 0.271828182845904523536028747135)` | `(0.2, -0.1)` |
| 5 | `(-2^-60, 2^-65)` | `(-0.25, 0.375)` |
| 6 | `(1.234567890123456789e-19, -9.876543210987654321e-19)` | `(0.333333333333333333333333333333, -0.2)` |
| 7 | `(0, 0)` | `(0.411111111111111111111111111111, -0.377777777777777777777777777777)` |
| 8 | `(-0.499999999999999999999999999999, 0.333333333333333333333333333333)` | `(-0.125, -0.25)` |
| 9 | `(2^-40, -2^-45)` | `(2^-20, -2^-22)` |
| 10 | `(-0.125 - 2^-70, 0.0625 - 2^-73)` | `(0.5, 0.25)` |
| 11 | `(0.2, -0.142857142857142857142857142857)` | `(-0.4, 0.3)` |
| 12 | `(1.23456789e-28, -9.87654321e-28)` | `(0.25, -0.125)` |
| 13 | `(0.375, -0.4375)` | `(-0.125, 0.0625)` |
| 14 | `(-0.0009765625, 0.001953125)` | `(0.7, -0.2)` |
| 15 | `(0, 2^-80)` | `(0.5, 0)` |

## Frozen independent expected products

These are independently frozen host products, not values emitted by the proposed codec.

| slot | expected `(real, imag)` |
|---:|---|
| 0 | `(0.046875, -0.0625)` |
| 1 | `(0.046875000000000000000449986253228847055130046328486059792339801788330078125, -0.06250000000000000000015881867761018131357531046887743286788463592529296875)` |
| 2 | `(-0.09375, -0.109375)` |
| 3 | `(0.040132641420774808949887288570205500563303788511522569043477, 0.102547257465954082897938387693155713378359871980684644218665)` |
| 4 | `(-0.0356490347872054124156499929519, 0.0857815631050788370918321832597)` |
| 5 | `(0.00000000000000000020667603913004928273267069016583263874053955078125, -0.0000000000000000003320369153236857329147824202664196491241455078125)` |
| 6 | `(-0.0000000000000000001563786012156378601200000000000411522630041152263, -0.0000000000000000003539094648353909464799999999996707818929670781893)` |
| 7 | `(0, 0)` |
| 8 | `(0.145833333333333333333333333333125, 0.083333333333333333333333333333125)` |
| 9 | `(0.0000000000000000008605854744103691444934156606905162334442138671875, -0.00000000000000000024394548880923849765167688019573688507080078125)` |
| 10 | `(-0.078125000000000000000397046694025453283938276172193582169711589813232421875, -0.00000000000000000000026469779601696885595885078146238811314105987548828125)` |
| 11 | `(-0.0371428571428571428571428571429, 0.1171428571428571428571428571428)` |
| 12 | `(-0.000000000000000000000000000092592592875, -0.000000000000000000000000000262345678875)` |
| 13 | `(-0.01953125, 0.078125)` |
| 14 | `(-0.00029296875, 0.0015625)` |
| 15 | `(0, 0.000000000000000000000000413590306276513837435704346034981426782906055450439453125)` |

The exact adjacent product witness is:

```text
expected[1] - expected[0]
  = (2^-71 + 2^-75, -2^-72 + 2^-74)
  = (2^-71 + 2^-75, -3*2^-74)
```

Both components are below ordinary binary64 distinguishability at the surrounding base values.

## Required observations through the proposed public seam

### A. Fresh input receipts

For both operands, `HighPrecisionClientIO::Encrypt` must return a receipt whose actual ciphertext/state jointly satisfy:

- same exact context and crypto-parameter identity as the module/key;
- required feature mask `PKE | KEYSWITCH | LEVELEDSHE`, with the construction-time complete enabled mask containing that subset; the current complete mask still equals the recorded mask, and the client has not enabled or changed any feature;
- frozen scaling/key-switch/execution/decryption-noise/data modes;
- key tag copied from the public key and nonempty;
- exact full level-zero ordered modulus/root basis;
- CKKS packed encoding, context type `COMPLEX`, execution mode `EXEC_EVALUATION`;
- context crypto-parameters report configured batch size 16; `FreshEncodingSpec.slots`, vector length, ciphertext slots, and receipt slots all equal 16; gap 2, level 0, two evaluation-format components, and a present empty ciphertext metadata map;
- noise-scale degree 2;
- OpenFHE recorded factor exactly `2^100` in this frozen binary64-compatible metadata field;
- exact client logical scale `2^100/1`;
- origin `FreshClientEncoding`;
- input vectors unchanged after return;
- no `Plaintext` or packed-value cache visible in the API.

Before evaluation, public `Decrypt` on each fresh receipt must return exactly the corresponding frozen input vector with maximum absolute complex error `<=2^-80`. Independently, the test-only schoolbook secret/CRT path must recover each fresh decryption polynomial, apply the explicit stride projection, and direct-evaluate it at the frozen powers-of-five roots; that independent result must also match the corresponding input vector within `2^-80`. For the left input, both public and independent observations must preserve

```text
left[1] - left[0] = (2^-70, 2^-73)
```

with absolute complex delta error `<=2^-80`. This checks the sub-binary64 input witness without relying on a matching production encode/decode roundtrip.

### B. Retained evaluator behavior

The test calls only the retained public evaluator seams:

```text
leftEval  = leftBound.CloneForEvaluation()
rightEval = rightBound.CloneForEvaluation()
leftPair  = DoubleCKKS::DCP(leftEval)
rightPair = DoubleCKKS::DCP(rightEval)
product   = DoubleCKKS::Mult2(leftPair, rightPair)
rcb       = DoubleCKKS::RCB(product)
```

It verifies both private receipt states remain unchanged and that mutating/consuming evaluator clones cannot alter them; the supported clones and receipts must retain separate, empty metadata maps, and the test also retains existing pair immutability checks. It does not pass a private key into `DoubleCKKS`.

### C. First-operation scale binding

`BindFirstMult2Rcb` receives only the immediate `rcb` variable produced by the call chain in section B and the two fresh receipts. It has no transition/scale argument. The test therefore exercises the documented single-process orchestration contract, while making no claim that the binder cryptographically attests operand lineage against deliberate substitution of a different same-state ciphertext. It must derive `q_div` as the final parent-basis modulus and `q_l` as the penultimate parent-basis modulus, verify that the output is the exact ordered modulus/root prefix with both removed, and independently calculate:

```text
(2^100 * 2^100) / (1125899906843009 * 1125899906840833)
= 2^200 / 1267650600226646386227681786497
```

It must also derive the physical recorded factor in the pinned evaluator's exact operation order:

```text
base          = GetScalingFactorReal(0)
freshRecorded = base * base
readyForRS2   = freshRecorded * freshRecorded / base
qLIndex       = fullParentBasisSize - 2
expectedOut   = readyForRS2 / GetModReduceFactor(qLIndex)
```

Every intermediate must be finite/positive and actual output must satisfy exact `double == expectedOut`. The binder must reject a swapped/changed modulus, wrong root, output basis that is not the exact drop-two prefix, wrong level, wrong key/context/feature profile, wrong slots, wrong component count, nonempty/missing metadata map, wrong format/encoding/data type, wrong degree/integer factor, or wrong observed factor. The receipt must store derived factors `1125899906843009` and `1125899906840833`, clone the validated output into its private snapshot, and have origin `FirstMult2Rcb`.

### D. Public high-precision output

`HighPrecisionClientIO::Decrypt` must:

- assert the frozen context uses `FIXED_NOISE_DECRYPT`; the test does not change the configured mode or feature mask to satisfy precision;
- use public scheme `Decrypt(..., Poly*)`, not high-level Plaintext Decode and not `DecryptCore`;
- return a valid result only after `DecryptResult.isValid`;
- verify the returned coefficient `Poly` has the receipt cyclotomic order, ring dimension, and exact active composite modulus before centering;
- produce exactly 16 `OpenFhePackedStride` values;
- report the exact active composite modulus, actual maximum centered coefficient, nonnegative centered headroom, and cross-precision disagreement;
- have cross-precision disagreement `<=2^-120`;
- match a separate test-only direct evaluation of the independently decrypted **stride-projected coefficient polynomial** within `2^-120` componentwise;
- have the returned public value itself satisfy, for every slot `i`, `abs(decoded.values[i]-expected[i]) <= 2^-80` using complex magnitude;
- have the independent stride-projected result separately satisfy `abs(projected[i]-expected[i]) <= 2^-80` for every slot;
- satisfy both `abs((decoded.values[1]-decoded.values[0])-frozen_delta) <= 2^-80` and `abs((projected[1]-projected[0])-frozen_delta) <= 2^-80`;
- return owned values that remain valid after local raw Poly/coefficient buffers are destroyed.

No all-key/no-wrap theorem is asserted by the centered diagnostics.

## Independent transform/order controls

A codec roundtrip by itself is insufficient because matching wrong transforms can cancel. The first target therefore retains or adds test-only independent evidence without calling private production helpers:

1. **Public-encode / independent-decrypt control.** For each public `Encrypt`, the existing test-only schoolbook secret/CRT path recovers coefficients, applies the explicit packed-stride projection, and direct-evaluates it against the corresponding frozen input vector and left-input delta at `2^-80`. Public fresh `Decrypt` is checked separately at the same threshold. This oracle stays entirely in the test target.
2. **Independent public-decode expectation.** For the same randomized first-RCB ciphertext in the asserted `FIXED_NOISE_DECRYPT` mode, the test-only secret/CRT oracle recovers all centered coefficients, zeroes every coefficient outside indices `gap*j` and `gap*j+N/2`, and directly evaluates that projected polynomial at `xi^(5^j)`. This `O(NS)` small-N reference, not the production FFT, is compared to public `Decrypt` at `2^-120`.
3. **Literal `1`.** Independently generated canonical values for polynomial `1` must be all `(1,0)` in the powers-of-five order.
4. **Literal `X^(N/2)`.** Independently generated values must be the corresponding imaginary-order witness (the existing small-N table/check remains authoritative).
5. **Literal `X^2`.** Compare against the frozen independent small-N canonical table to catch exponent/order errors.
6. **Partial projection witness `X`.** At the pure coefficient/reference level for `N=64,S=16,gap=2`, stride projection removes the `X` message coefficient and gives zero, while full Horner gives nonzero `xi^(5^j)`. This validates that the two independent reference observations are deliberately different. It is not a claim that an encrypted decryption containing RLWE/flooding noise is bit-exact zero.
7. **Retained first-Mult2 all-coefficient oracle.** The existing test remains unchanged and independently verifies the current encrypted/evaluator result. The new public decoder is an additional packed-projection observation, not a replacement.

Production code never imports the test oracle or test fixture.

## Predicted genuine RED

Once this proposed test is authored after confirmation, the first expected failure is a compile failure because the public client header, types, and methods do not exist. A representative cause is:

```text
fatal error: openfhe_2023_1788/high_precision_client_io.h: No such file or directory
```

The exact compiler wording is platform-dependent. No runtime RED, cryptographic value, or CTest outcome is claimed in advance.

## Smallest future GREEN scope

Only after confirmed RED review:

- add `include/openfhe_2023_1788/high_precision_client_io.h`;
- add `src/high_precision_client_io.cpp`;
- add the one proposed test source and additive target/CTest registration;
- use only the declaration-shaped public seam: no test friendship/private-helper reach-through;
- link to existing OpenFHE/project targets;
- do not modify `double_ckks.cpp`, existing tests, thresholds, fixture literals, evaluator behavior, OpenFHE source, or workflow semantics except additive hosted execution of the new target.

No REAL path, serialization, context factory, arbitrary-level input, repeated-state binder, benchmark, or paper-run code belongs in this GREEN.

## Hosted acceptance commands after confirmation and reviewed code

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug   -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
cmake --build build --target precision_client_io_first_mult2_contract_test --parallel 2
ctest --test-dir build --verbose --output-on-failure
ctest --test-dir build --verbose --output-on-failure   -R '^precision_client_io_first_mult2_contract$'

cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
cmake --build build --target mult2_api_contract_test --parallel 2
cmake --build build --target add_api_contract_test --parallel 2
cmake --build build --target sub_api_contract_test --parallel 2
```

Acceptance additionally requires both hosted compiler families' warning-as-error settings already used by the project, all pre-existing 55 tests, and the five explicit API targets. Existing `<=2^-80` thresholds are not weakened.

## Ordered next slices, only after this one passes

1. **State/error closure:** malformed rational, nonfinite value, wrong context/key tag/root/basis/level/slot/component/format, invalid decryption, input immutability, and returned-value ownership.
2. **Full-slot codec/order gate:** `S=N/2` small/medium controls and then paper-shape `N=32768,S=16384` transform correctness plus measured resource/runtime evidence; no performance conclusion from complexity alone.
3. **Repeated-Mult2 reconciliation:** replace or extend the narrow first-operation binder only after the separate repeated design fixes exact scale/basis/context/key receipt ownership.
4. **Paper experiment gate:** paper modulus/key/security configuration, eight repeated squarings without section 6.2 refresh, and 1,000-run mean infinity-norm evidence.

A codec-only roundtrip is never sufficient to skip the independent controls or evaluator integration.
