# First Mult2 high-precision test design

## Status

**DRAFTED AND STATICALLY VERIFIED; OpenFHE build/runtime and hosted CI are NOT EXECUTED in this review environment.**

Frozen contract: `evidence/FROZEN_CONTRACT.json`  
SHA-256: `7d6b7f5e1c820cf49641dc1606fa64984a52267e2f04305c2ad5bd5981d2036b`

This is one additive test-only slice. It changes only:

```text
CMakeLists.txt
tests/precision_first_mult2_contract_test.cpp   (new)
```

It reuses the accepted `tests/precision_dcp_rcb_fixture.{h,cpp}` byte-for-byte. It does not change production code, public headers, the existing 54 test bindings, the workflow, the BV certificate test, pair Add/Sub, RS2, parameters, security policy, or lifecycle rules.

## Public seam and claim

The observed production path is:

```text
lossless test-owned plaintext fixture
  → Encrypt(left), Encrypt(right)
  → public DCP(left), public DCP(right)
  → public Mult2(leftPair, rightPair)
  → public RCB(result) as an observed endpoint only
```

`Mult2` is the exact production composition `RS2(Relin2(Tensor2(left,right)))` (`project/src/double_ckks.cpp:L1115-L1117`). The staged path is also invoked and compared byte-exactly to direct `Mult2`, but that comparison is only a wiring check. Expected plaintext arithmetic comes from frozen multiprecision product literals and independent secret/CRT/canonical evaluation.

A hosted pass would establish only this statement:

> In the frozen N=64 HYBRID diagnostic context, four fresh keys on that host produced first-Mult2 outputs whose independently decoded 16 complex slots and designated sub-binary64 product delta were each within absolute `2^-80`, while the frozen state, basis, lifecycle, scale, non-mutation, and actual-centered-headroom assertions held.

It would not establish repeated multiplication, refresh, Table 3, a production codec, security, performance, or a universal theorem.

## Frozen context

| Field | Value |
|---|---:|
| Ring dimension | 64 |
| Batch size | 16 |
| Multiplicative depth | 7 |
| Scaling modulus size | 50 bits |
| First modulus size | 55 bits |
| Scaling | FIXEDMANUAL |
| Key switch | HYBRID |
| Digit size | 0 |
| Max relin secret-key degree | 2 |
| Secret distribution | UNIFORM_TERNARY |
| CKKS data type | COMPLEX |
| Security level | HEStd_NotSet, diagnostic only |
| Input noise-scale degree | 2 |
| Exact input logical scale | `S=2^100` |
| Fresh key trials | 4 per host |
| Crypto RNG | fresh and deliberately unseeded |
| Primary absolute threshold | `2^-80` |
| Actual centered-representative headroom gate | at least 128 bits |

The host vectors and expected values are deterministic. Only cryptographic keys/noise remain fresh and unseeded.

## Frozen vectors and products

Every entry below is parsed as `cpp_dec_float_100` or constructed as an exact power of two. No expected value passes through `double`.

| Slot | Left `(Re, Im)` | Right `(Re, Im)` | Frozen product `(Re, Im)` |
|---:|---|---|---|
| 0 | `(0.125, -0.0625)` | `(0.5, -0.25)` | `(0.046875, -0.0625)` |
| 1 | `(0.125+2^-70, -0.0625+2^-73)` | `(0.5, -0.25)` | `(0.046875000000000000000449986253228847055130046328486059792339801788330078125, -0.06250000000000000000015881867761018131357531046887743286788463592529296875)` |
| 2 | `(0.125, 0.25)` | `(-0.5, 0.125)` | `(-0.09375, -0.109375)` |
| 3 | `(0.123456789012345678901234567890, -0.234567890123456789012345678901)` | `(-0.271828182845904523536028747135, 0.314159265358979323846264338327)` | `(0.040132641420774808949887288570205500563303788511522569043477, 0.102547257465954082897938387693155713378359871980684644218665)` |
| 4 | `(-0.314159265358979323846264338327, 0.271828182845904523536028747135)` | `(0.2, -0.1)` | `(-0.0356490347872054124156499929519, 0.0857815631050788370918321832597)` |
| 5 | `(-2^-60, 2^-65)` | `(-0.25, 0.375)` | `(2.0667603913004928273267069016583263874053955078125e-19, -3.320369153236857329147824202664196491241455078125e-19)` |
| 6 | `(1.234567890123456789e-19, -9.876543210987654321e-19)` | `(0.333333333333333333333333333333, -0.2)` | `(-1.563786012156378601200000000000411522630041152263e-19, -3.539094648353909464799999999996707818929670781893e-19)` |
| 7 | `(0, 0)` | `(0.411111111111111111111111111111, -0.377777777777777777777777777777)` | `(0, 0)` |
| 8 | `(-0.499999999999999999999999999999, 0.333333333333333333333333333333)` | `(-0.125, -0.25)` | `(0.145833333333333333333333333333125, 0.083333333333333333333333333333125)` |
| 9 | `(2^-40, -2^-45)` | `(2^-20, -2^-22)` | `(8.605854744103691444934156606905162334442138671875e-19, -2.4394548880923849765167688019573688507080078125e-19)` |
| 10 | `(-0.125-2^-70, 0.0625-2^-73)` | `(0.5, 0.25)` | `(-0.078125000000000000000397046694025453283938276172193582169711589813232421875, -2.6469779601696885595885078146238811314105987548828125e-22)` |
| 11 | `(0.2, -0.142857142857142857142857142857)` | `(-0.4, 0.3)` | `(-0.0371428571428571428571428571429, 0.1171428571428571428571428571428)` |
| 12 | `(1.23456789e-28, -9.87654321e-28)` | `(0.25, -0.125)` | `(-9.2592592875e-29, -2.62345678875e-28)` |
| 13 | `(0.375, -0.4375)` | `(-0.125, 0.0625)` | `(-0.01953125, 0.078125)` |
| 14 | `(-0.0009765625, 0.001953125)` | `(0.7, -0.2)` | `(-0.00029296875, 0.0015625)` |
| 15 | `(0, 2^-80)` | `(0.5, 0)` | `(0, 2^-81)` |

The complete decimal strings used by the test are authoritative in `evidence/FROZEN_CONTRACT.json` and `final-changed-files/project/tests/precision_first_mult2_contract_test.cpp:L492-L605`.

### Distinguishing witness

Slots 0 and 1 use the same right multiplier. Their exact product difference is

```text
real = 2^-71 + 2^-75
imag = -2^-72 + 2^-74 = -3·2^-74
```

or approximately

```text
(4.49986253228847055130046328486059792339801788330078125e-22,
 -1.5881867761018131357531046887743286788463592529296875e-22)
```

Its magnitude is approximately `4.7719073802074446e-22`, about 576.888 times the `2^-80` tolerance. Converting the corresponding left inputs to binary64 collapses slots 0 and 1 and therefore collapses their products. The test checks this as an explicit negative control (`new test:L662-L694`). It is fixture calibration, not a manufactured production regression.

## Exact scale derivation

Let each lossless input polynomial encode a canonical value at exact integer scale

```text
S = 2^100.
```

The unrescaled coefficient product has numerator scale `S²=2^200`. The paper defines `Mult2=RS2∘Relin2∘Tensor2` (`PAPER-2023-1788.txt:L895-L900`) and states that the plaintext scale is divided by `q_div` in Tensor2 and by `q_l` in RS2, so the overall scale is divided by `q_div·q_l` (`L949-L958`). Therefore the exact first-output logical scale is

```text
S_out = 2^200 / (q_div·q_l).
```

The test obtains `q_div` and `q_l` from the actual ordered integer basis after DCP and constructs the denominator as `cpp_int q_div*q_l` (`new test:L1075-L1080`). Direct canonical evaluation of the actual centered coefficients then multiplies by `(q_div*q_l)/2^200` (`L404-L418,L1121-L1123`).

This exact rational normalization is independent of:

- ciphertext `double GetScalingFactor()`;
- FIXEDMANUAL `GetScalingFactorReal()` and `GetModReduceFactor()` approximations;
- the long-double paper descriptor.

Recorded metadata follows the existing implementation: `2^100 → 2^150` at Tensor2 and back to `2^100` at RS2 because FIXEDMANUAL reports `2^50` for both metadata divisors (`official-openfhe/rns-cryptoparameters.h:L601-L649`; production `Tensor2` at `project/src/double_ckks.cpp:L784-L809`, `RS2` at `L1006-L1029`). That recorded `2^100` is checked as state, not used to infer high-precision correctness.

## Why `2^-80` is frozen—and what it is not

The fresh `2^100` fixture rounds at most 32 populated real coefficient coordinates. Each rounding error is at most `1/2` integer unit, so the direct canonical triangle bound per input slot is

```text
32·(1/2)/2^100 = 2^-96.
```

All inputs satisfy the frozen unit L1 envelope. For exact inputs `x,y` and representation errors `e_x,e_y≤2^-96`, the deterministic input-representation contribution to a product is bounded by

```text
|x e_y| + |y e_x| + |e_x e_y| < 2·2^-96 + 2^-192 < 2^-94.999….
```

Thus `2^-80` leaves roughly 15 bits for encryption, Tensor2’s omitted low-low term, HYBRID relinearization, and two rounded rescale effects. The accepted DCP→RCB precursor’s worst observed error is about `2^-90.44`, so it also leaves about ten observed bits to this gate. These facts make the gate a useful and nontrivial first experiment.

They do **not** prove the gate will pass. Theorem 4.8 has explicit magnitude and relinearization assumptions (`PAPER-2023-1788.txt:L903-L947`), while the project correctly leaves `conservative_E_Relin` unavailable and its universal theorem gate unproved. Moreover, this generated chain has approximately equal 50-bit `q_div` and `q_l`, whereas the paper says `q_l` should be at least slightly larger to suppress the dominant omitted-low-low term (`L949-L958`) and uses an approximately 40/60-bit split for its high-precision experiment (`L1568-L1576`).

Therefore:

- `2^-80` is frozen **before** hosted observation;
- it must not be relaxed after a failure;
- a correct failure is evidence that the current p50/50 diagnostic context does not meet this gate, not permission to fit a tolerance;
- the next paper-directed parameter experiment is the separately reviewed ordered Div40/Mult60 chain.

## Independent expected and actual paths

### Expected values

`FrozenExpectedProducts()` contains all 16 products as literal multiprecision decimals (`new test:L554-L584`). An independent host multiplication of the two frozen input vectors must agree with those literals within `1e-75` (`L586-L598`). The runtime comparison uses the frozen table, not a double result and not a production helper.

### Actual ciphertext decoding

The test converts each RLWE component and the secret key to coefficient format, performs schoolbook negacyclic multiplication for `c0+c1·s+…` in each RNS tower, and reconstructs centered coefficients with independent `cpp_int` CRT (`L92-L155,L181-L313`). It then independently computes centered `q_div·high+low` (`L315-L344`). Production RCB is called only afterward as an observed public endpoint and must decrypt to the same independently recombined coefficients (`L1106-L1119`).

### Canonical evaluation

The recovered polynomial is evaluated directly by Horner’s rule at explicit powers-of-five exponents (`L395-L437`). This is not a port of the matching forward special FFT. Before crypto execution, constant, `X^32`, and hard-coded ordered `X^2` witnesses validate normalization, sign, phase, conjugation, and slot order (`L439-L490`). OpenFHE’s pinned rotation group uses powers of five (`official-openfhe/dftransform.cpp:L50-L69`).

## Separate assertion classes

The test deliberately keeps these categories distinct:

1. **Fixture calibration:** frozen literals, product table, binary64 negative control, canonical monomial witnesses.
2. **Public state:** context, encoding, level, degree, component count, key tag, slot count, format, exact ordered moduli, divisor selection, and `RefreshRequired` lifecycle (`new test:L696-L847`).
3. **Wiring:** exact equality of direct `Mult2` and staged `Tensor2→Relin2→RS2` (`L1049-L1067`). This is not the arithmetic oracle.
4. **Immutability:** encrypted inputs, both DCP pairs, and the `Mult2` result across RCB are snapshot-checked (`L849-L921,L1033-L1073,L1106-L1119`).
5. **Actual centered headroom:** observed input-product and output representatives must each have at least 128 bits to the centered modulus boundary (`L1081-L1104`). This remains diagnostic, not a universal history proof.
6. **Slot accuracy:** all 16 complex errors and the specific product delta must be at most `2^-80` (`L1121-L1139`).
7. **Coefficient diagnostic:** the exact rational residual numerator is reported separately (`L944-L957,L1141-L1180`); it is not silently converted into a slot threshold.

## First-observed outcome policy

Existing production already has retained functional Mult2 TDD. No artificial red is created here.

- A first hosted pass is recorded as **first-observed GREEN** for this new precision contract.
- A first hosted failure remains RED with the exact frozen vector, threshold, oracle, and source hash unchanged.
- Diagnosis begins by classifying the first divergence among fixture calibration, state/basis/lifecycle, staged/direct wiring, independent coefficient relation, and canonical slot error. Only one precise cause is addressed before another run.
- Thresholds, expected values, scale normalization, and claim wording are not modified to manufacture green.
