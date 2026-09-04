# Revised precision TDD design: one frozen public DCP→RCB behavior

> **Artifact status: RECONSTRUCTED.** The exact prior code/patch bytes were absent from the defective return and current workspace. This file accompanies newly drafted replacement bytes based on the exact supplied baseline and frozen contract. No byte-identity claim to the missing prior candidate is made.

## 1. Decision

**Recommended bounded sequence: two patches, one CTest, one unchanged positive contract.**

1. `0001-red-freeze-dcp-rcb-high-precision-contract.patch` adds the complete
   test contract and an explicitly incomplete `complex<double>` fixture.
2. `0002-green-replace-only-precision-fixture.patch` changes only
   `tests/precision_dcp_rcb_fixture.cpp`; it replaces the lossy fixture with a
   test-owned multiprecision plaintext/DCRT adapter.

The shipping implementation, public header, DCP, RCB, pair representation,
parameter generator, security safeguards and lifecycle are unchanged. The exact
CTest name is:

```text
precision_dcp_rcb_high_precision_contract
```

The green patch does not delete, rename, invert, skip or relax the red
acceptance. `evidence/TEST_CONTRACT_HASH_LEDGER.txt` and
`tools/verify_contract_continuity.py` independently reconstruct and check this.

## 2. Status labels

- **OBSERVED — supplied baseline:** public DCP and RCB exist. DCP accepts a
  level-0, scale-degree-2 ciphertext and removes the final `q_div` tower; RCB
  forms `q_div·high + low` at the retained basis. The selected source is
  `bda879104c8a8b1ba6ac9301385b5b1919bef440`.
- **OBSERVED — pinned upstream source:** Native64 packed encoding first executes
  the special inverse DFT in binary64, rounds after multiplication by the
  depth-one scale, and only afterwards applies extra CRT scale powers for
  `noiseScaleDeg > 1` (`official-openfhe/ckkspackedencoding.cpp:115-133,
  191-309, 331-332`).
- **OBSERVED — pinned source contract:** `Plaintext::GetElement<T>()` has public
  const and mutable accessors (`official-openfhe/plaintext.h:258-269`), and
  `CryptoContext::Encrypt(Plaintext, PublicKey)` extracts the plaintext element
  rather than its packed `complex<double>` cache
  (`official-openfhe/cryptocontext.h:1248-1262`).
- **INFERRED:** a test can therefore inject a fresh high-precision DCRT element
  without an upstream fork. This does not make the stale packed cache valid.
- **PROPOSED, NOT EXECUTED:** on pinned hosted OpenFHE, the red state should fail
  the frozen positive precision assertions; the green state should pass them.
- **NOT CLAIMED:** Mult2 precision, refresh, repeated multiplication, Table 3
  reproduction, security, performance or a production user-facing
  multiprecision codec.

## 3. Exact public seam

The tracer bullet uses only the confirmed shipping seam:

```text
test-owned plaintext fixture
    → CryptoContext::Encrypt
    → DoubleCKKS::DCP
    → DoubleCKKS::RCB
    → test-only independent secret/CRT coefficient recovery
    → direct canonical evaluation
```

Public declarations are pinned at
`project/include/openfhe_2023_1788/double_ckks.h:143-149`; implementations are
at `project/src/double_ckks.cpp:406-425` for DCP and `1001-1011` for RCB.
Production RCB is the object under test. It is not used to manufacture the
expected coefficient result.

## 4. Frozen context and state contract

The single test fixes the following diagnostic context:

| Property | Frozen value | Meaning |
|---|---:|---|
| Ring dimension | `N = 64` | Small hosted tracer bullet, not a security parameter claim |
| Cyclotomic order | `M = 128` | Power-of-two CKKS ring |
| Batch size | `16` | All configured slots are nontrivially checked |
| Multiplicative depth | `7` | Retains the supplied DCP-compatible tower shape |
| Scaling modulus size | `50` | Homogeneous diagnostic primes |
| First modulus size | `55` | Diagnostic first prime |
| Scaling technique | `FIXEDMANUAL` | Matches the supplied implementation contract |
| Key switch | `HYBRID`, digit size `0` | Matches the green functional path; no BV adjudication |
| Maximum relin degree | `2` | t=2 boundary |
| Secret distribution | `UNIFORM_TERNARY` | Supplied baseline choice |
| CKKS data type | `COMPLEX` | Exercises real and imaginary values |
| Security level | `HEStd_NotSet` | Explicitly no security claim |
| Input scale degree | `2` | Required by public DCP |
| Recorded/logical input scale | exact `2^100` | Fresh full-scale fixture in green |
| Fresh key trials | `4` | Literal host vector; crypto PRNG intentionally unseeded |

The contract asserts, without adjustment between red and green:

- plaintext and encrypted input are level 0, scale degree 2, 16 slots, recorded
  scale `2^100`;
- DCP returns `ReadyForFirstMult`, level 1, degree 2, recorded scale `2^100`, and
  recombined logical scale `2^100`;
- DCP removes exactly one final tower, exposes that removed modulus as
  `pair.GetDivisor()`, and preserves the ordered prefix basis;
- RCB returns level 1, degree 2, two RLWE components, 16 slots and recorded scale
  `2^100` at the exact retained prefix basis;
- actual independently decrypted coefficients have at least 128 bits of
  centered non-wrap headroom under the active modulus.

This is deliberately a homogeneous `p50/50` diagnostic. The removed generated
prime is approximately 50 bits, not the paper's ordered approximately 40-bit
`q_div`; the retained generated primes are approximately 50 bits, not the
paper's approximately 60-bit Mult primes. It must not be described as Table 3.

## 5. Frozen lossless source vector

The expected values originate as `cpp_dec_float_100` decimal strings and exact
powers of two. They never pass through `double` before expected-value use.

| Slot | Real source | Imaginary source | Purpose |
|---:|---|---|---|
| 0 | `0.125` | `-0.0625` | Exact dyadic base |
| 1 | `0.125 + 2^-70` | `-0.0625 + 2^-73` | Positive sub-binary64 delta |
| 2 | `0.123456789012345678901234567890` | `-0.234567890123456789012345678901` | Non-dyadic complex |
| 3 | `-0.314159265358979323846264338327` | `0.271828182845904523536028747135` | Signed non-dyadic complex |
| 4 | `-2^-60` | `2^-65` | Near-zero dyadic |
| 5 | `1.234567890123456789e-19` | `-9.876543210987654321e-19` | Near-zero non-dyadic |
| 6 | `0` | `0` | Zero control |
| 7 | `-0.499999999999999999999999999999` | `0.333333333333333333333333333333` | Boundary-like signed non-dyadic |
| 8 | `2^-40` | `-2^-45` | Exact dyadic control |
| 9 | `-0.125 - 2^-70` | `0.0625 - 2^-73` | Negative sub-ULP counterpart |
| 10 | `0.2` | `-0.142857142857142857142857142857` | Non-dyadic rational decimals |
| 11 | `1.23456789e-28` | `-9.87654321e-28` | Very small non-dyadic |
| 12 | `0.375` | `-0.4375` | Exact dyadic complex |
| 13 | `-0.0009765625` | `0.001953125` | Exact powers-of-two multiples |
| 14 | `0.411111111111111111111111111111` | `-0.377777777777777777777777777777` | Dense decimal complex |
| 15 | `0` | `2^-80` | Acceptance-scale imaginary control |

Important exact magnitudes are:

```text
2^-70 = 8.470329472543003390683225006796419620513916015625e-22
2^-73 = 1.058791184067875423835403125849552452564239501953125e-22
2^-80 = 8.2718061255302767487140869206996285356581211090087890625e-25
```

## 6. One unchanged positive acceptance contract

After independent decryption and direct canonical evaluation, every fresh-key
trial applies exactly these positive assertions:

```text
| (actual[1] - actual[0]) - (2^-70 + i·2^-73) | <= 2^-80
max_j | actual[j] - expected[j] |               <= 2^-80
```

The following test-source items are identical in red and green:

- `kAcceptanceBits = 80`;
- the exact real and imaginary sub-ULP deltas;
- all 16 source values;
- the direct evaluator and hard-coded exponent order;
- all monomial witnesses;
- DCP/RCB state and basis assertions;
- 128-bit non-wrap requirement;
- four fresh-key trials;
- error calculations and failure messages.

### Threshold justification

Fresh coefficient construction rounds only once at exact `2^100`. With 16
complex slots, the sparse packed representation has at most 32 populated real
integer coefficients. A coefficient-rounding-only triangle bound is therefore
at most

```text
32 · 0.5 / 2^100 = 2^-96
```

per canonical value before encryption. The `2^-80` gate leaves a fixed 16-bit
margin for encryption and exact DCP/RCB transport effects while remaining ten
bits below the real `2^-70` signal and seven bits below the imaginary `2^-73`
signal. This is a predeclared hosted acceptance boundary, not a universal noise
proof. A hosted green failure must trigger diagnosis; the threshold must not be
fitted after observing results.

## 7. Negative controls are not success criteria

The same red and green test first proves:

1. the multiprecision slot-0 and slot-1 values differ in both components;
2. converting them to `std::complex<double>` collapses them to equal values;
3. standard OpenFHE encoding of those already-equal binary64 values produces
   identical DCRT plaintext elements.

That is an explicit **negative control** for host-input collapse. It does not
assert an OpenFHE defect: the standard encoder correctly receives identical
binary64 values. It also does not count as DCP/RCB precision success.

## 8. What the red state proves and does not prove

### Red fixture

`tests/precision_dcp_rcb_fixture.cpp` converts each multiprecision value to
`std::complex<double>` and calls standard `MakeCKKSPackedPlaintext(..., 2, 0)`.
The source delta is therefore absent before Encrypt, DCP or RCB.

### Expected hosted red meaning

A failure of either unchanged positive assertion establishes only that this
**incomplete test fixture cannot supply the frozen >53-bit input contract**. It
must not be labelled a DCP, RCB or upstream standard-encoding defect. No hosted
red was executed in this environment.

### Red does not establish

- a production Double-CKKS precision failure;
- that DCP or RCB corrupts a correctly supplied `2^100` plaintext;
- a quantitative encryption-noise distribution;
- Mult2, refresh or repeated-lifecycle behavior;
- security or performance.

## 9. Green fixture and stale-cache boundary

The green patch changes only the test fixture implementation. It:

1. creates a normal scale-degree-2 packed plaintext solely to obtain pristine
   metadata and the generated DCRT parameters;
2. performs a `cpp_dec_float_100` port of pinned `FFTSpecialInv` using the full
   `M=128` geometry;
3. rounds fresh inverse coefficients at exact `2^100` with explicit positive
   and negative branches for half-away-from-zero materialization;
4. builds each DCRT tower through public `NativeVector`, `SetValues` and
   `DCRTPoly` interfaces;
5. replaces the plaintext's public mutable DCRT element and leaves all shipping
   code untouched.

The placeholder's private packed-value cache remains zeros. Consequently:

- the test never calls `GetCKKSPackedValue()` on an injected plaintext;
- expected values never come from standard double decoding;
- the test never invokes production `Decrypt`, whose REAL masking behavior is
  irrelevant to this COMPLEX, test-only coefficient oracle;
- the adapter is not proposed as a shipping codec or mutable pair factory.

The public accessor and Encrypt data path support the fixture mechanically, but
stale cache semantics make it unsuitable as a production API. A later
production decision still has to define lossless caller input and output types,
ownership, cache consistency, validation and serialization.

## 10. Independent oracle construction

### 10.1 Coefficient recovery

The output ciphertext is recovered with a test-only exact oracle:

- each DCRT component is copied to coefficient format;
- each tower computes `c0 + c1·s + ...` using independent exact negacyclic
  convolution with `boost::multiprecision::cpp_int`;
- residues are reconstructed with an independently implemented centered CRT;
- production RCB is never reused to generate an expected coefficient vector.

The test follows the supplied existing oracle pattern but owns its implementation
inside the new contract source.

### 10.2 Direct canonical evaluation

The oracle does **not** port OpenFHE's forward `FFTSpecial`. It directly evaluates
all recovered coefficients by Horner's rule at

```text
ζ^e,  ζ = exp(2πi / 128)
e = [1, 5, 25, 125, 113, 53, 9, 45,
     97, 101, 121, 93, 81, 21, 105, 13]
```

The order is the first 16 powers of 5 modulo 128. This is tied to the paper's
canonical embedding (`PAPER-2023-1788.txt:209-219`) and the pinned rotation group
and special transform (`official-openfhe/dftransform.cpp:50-60, 209-239`).

### 10.3 Worked oracle witnesses

Before any ciphertext operation, the direct evaluator must pass:

- coefficient `2^90` at `X^0` → every checked slot is exactly `1` within
  `1e-70` (scale and constant sign);
- coefficient `2^90` at `X^32` → every checked slot is `+i` within `1e-70`
  (phase direction, conjugation and sign);
- coefficient `2^90` at `X^2` → a hard-coded 16-value decimal table for
  `exp(iπe/32)` (slot order, bit reversal, phase, signs and conjugation).

Thus a matching inverse/forward transform mistake cannot silently cancel.

## 11. Hosted progression for this bounded slice

The exact first hosted progression is:

1. Apply patch 0001 to the exact packet baseline.
2. Configure/build with pristine pinned OpenFHE on Linux GCC and Windows
   MinGW64, maximum two build threads, warnings as errors.
3. Run only `precision_dcp_rcb_high_precision_contract` verbosely and retain the
   failing log. Record which unchanged positive assertion failed; do not call
   unrelated inherited failures part of this red.
4. Apply patch 0002 without changing any contract file.
5. Repeat the focused build/test. A green is valid only if the frozen assertions
   pass in all four fresh-key trials on both hosts.
6. Run the complete suite. Report the two inherited BV empirical certificate
   failures separately; they remain outside this patch and prevent a blanket
   full-suite green statement until their independent task is resolved.

This slice ends at public DCP→RCB. A later gate, under a separate approved task,
may reuse the same multiprecision source/oracle approach for first Mult2. Refresh
and repeated multiplication remain later lifecycle gates.

## 12. Scope exclusions and material decisions

| Item | This slice | Why |
|---|---|---|
| Shipping codec/API | No | Requires a caller-visible precision and cache contract |
| Production plaintext private hook | No | Violates pristine/public-interface boundary |
| Mutable pair constructor | No | Not required for DCP→RCB tracer bullet |
| REAL decryption masking changes | No | Security safeguard must remain intact |
| Parameter factory 40/60 split | No | Separate production parameter/lifecycle decision |
| Mult2/relinearization changes | No | BV certificate task is independent |
| Refresh/repeated multiplication | No | Explicitly outside bounded continuation |
| Table 3/security/performance claim | No | No reproduced evidence |

The smallest honest next decision after hosted DCP→RCB green is whether the
project will expose a production multiprecision input/output representation or
keep high-precision construction and observation as internal implementation
machinery. This patch does not prejudge that API decision.
