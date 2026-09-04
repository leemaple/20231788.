# Production high-precision client I/O — design decision

**Status:** proposed design only; no source, test, build, repository, or CI change is authorized by this document.  
**User-confirmation gate:** the public seam in `PROPOSED_PUBLIC_INTERFACE.md` must be explicitly confirmed before any test or implementation is authored.  
**Project source identity:** `b64a98041c0ca639ef47318f122273f5969caac2` on `codex/lossless-io-01`; active tested bytes remain `47907783a6141d0174da79eae264d779fc598f28`.  
**Pristine upstream pin:** OpenFHE 1.5.0 commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`.  
**Input archive:** `lossless-client-io-design-b64a980.zip`, 1,531,734 bytes, SHA-256 `efd18ebf2f753624251b1ad60da08d8e31c431ef91495fe93e8951d6cd3f24cc`.

## Decision in one paragraph

Adopt a small in-process C++ client module whose public numeric value is a fixed 100-decimal-digit complex type and whose exact logical scale is a reduced positive rational of arbitrary-size integers. The module directly performs a multiprecision version of OpenFHE CKKS's special inverse transform, paper-specified integer rounding, exact integer-to-DCRT conversion, and **public scheme-level** encryption. It returns an immutable ciphertext-and-state receipt, never an ordinary `Plaintext`. For output, it validates the receipt and key/context, calls **public scheme-level `Decrypt(..., Poly*)`**, checks the decryption result, then performs a separate multiprecision **strided packed-slot projection** and special forward transform. It never calls `DecryptCore`, never disables configured polynomial flooding, never gives a secret key to the evaluator, never decrypts/re-encrypts inside an evaluator operation, and never exposes the stale binary64 `Plaintext` cache used by the current test-only fixture.

## Scope and non-goals

This increment decides the client boundary required to preserve roughly 100-bit inputs and observe roughly 80-bit post-evaluation results without first passing through binary64. It does not implement the module, add tests, modify the evaluator, introduce serialization, construct contexts or keys, perform rotations/bootstrapping/multiparty operations, establish paper parameters, prove no wrap for all keys, or satisfy the paper's repeated-squaring experiment. It uses only the pinned public OpenFHE surface: no fork, hidden API, `const_cast`, or shared-context mutation is part of the design. The accepted first-Mult2 measurements remain diagnostic evidence only.

The evaluator boundary remains strict:

- `DCP`, `RCB`, `Add`, `Sub`, `Tensor2`, `Relin2`, `RS2`, and `Mult2` stay homomorphic.
- The evaluator receives ciphertext/state, never a secret key or a plaintext oracle.
- Client-side decryption is permitted only at the output boundary.
- No production implementation may copy the tests' schoolbook RLWE secret/CRT oracle.

### Evidence disposition applied

The original official-API audit was treated as source research, not an adopted interface. Its fresh recheck was read as a same-author recheck—not an independent third-party endorsement—and the main disposition's qualification is applied: on the CKKS path, ordinary context decryption invokes Decode only **after** a valid scheme result; invalid results return early. The direct scheme Poly route is selected only after independently tracing the supplied pinned source, and the stale-Plaintext internal alternative is rejected as a public seam.

The supplied hosted evidence at active source `47907783a6141d0174da79eae264d779fc598f28` reports Linux and Windows full `55/55`, focused `1/1`, all five explicit API targets, and 16 fresh-key first-Mult2 observations within `2^-80`. The supplied worst values are `1.6696072195146116607129673424340031160212e-27` for slots and `1.6958307879080880932103073456218202834178e-27` for the delta. Those runs were not repeated here, are not statistical/all-key evidence, and their centered headroom is not a universal no-wrap theorem; decrypted low coefficients can reflect secret convolution.

---

## Decision 1 — public input/output representation

### Selected representation

The proposed public numeric types are:

```text
ClientReal    := boost::multiprecision::number<cpp_dec_float<100>>
ClientComplex := { ClientReal real; ClientReal imag; }
ExactInteger  := boost::multiprecision::cpp_int
```

`ClientReal` provides 100 decimal digits, approximately 332 binary bits. That margin is intentionally well above the 100-bit fresh scale and the current 80-bit acceptance threshold. It is an **in-memory numeric contract**, not a persistence format. Callers must construct values from decimal strings, exact integers, rational/power-of-two operations, or an already-multiprecision value. The public API contains no `double`, `long double`, `std::complex<double>`, or implicit binary64 entry point.

All input real and imaginary components must be finite. NaN and infinity are rejected. A caller may explicitly convert a binary64 value before calling the module, but that is a caller-owned loss and is not represented as a precision-preserving path.

The returned values are rounded once to `ClientReal`'s 100 decimal digits after an internal precision-stability check. This finite-precision rounding is distinct from CKKS approximation/encryption/evaluation error. The module is therefore described as **precision-preserving client I/O**, not mathematically lossless encryption or exact approximate-number multiplication.

### Internal numerical policy

The transform is evaluated twice, at two independent fixed working precisions:

- primary work precision: at least 160 decimal digits;
- verification precision: at least 220 decimal digits.

At each precision, π and every root are generated natively at that precision; no binary64 FFT/root table is imported. For each scaled real or imaginary encoding coordinate, let `x160` and `x220` be the independently computed values, promote `x160` to the 220-digit type, and define:

```text
d = abs(x220 - x160)
h = distance(x220, {k + 1/2 : k is an integer})
```

Before applying the tie guard, the primary value must also satisfy

```text
u160 = max(1, abs(x160)) * numeric_limits<WorkReal160>::epsilon()
u160 < 2^-410
```

for each real and imaginary scaled coordinate. This explicit range check prevents a very large integer part from consuming the primary type's fractional precision while the API still pretends to resolve a `2^-400` tie margin. It is separate from the exact CRT headroom check. Both runs must then produce the same `round_paper` integer and satisfy `h > max(16*d, 2^-400)`. A range failure, disagreement, or value inside that guard is a precision-ambiguity `range_error`; the implementation does not guess. This deterministic conservative rule is not an interval proof of transcendental rounding. For decoding, independently generated 160- and 220-digit results must agree in every real and imaginary component within `2^-120` before the 220-digit value is rounded once to `ClientReal`. The `2^-120` guard bounds implementation roundoff only; it is not the cryptographic accuracy threshold.

### Paper rounding policy

For a real number `x`, let `f = floor(x)` and `r = x - f`. The selected nearest-integer rule is:

```text
round_paper(x) = f      when r <= 1/2
                 f + 1  when r >  1/2
```

Thus exact ties go downward, toward negative infinity. This follows the paper, not the current fixture's half-away-from-zero helper. Existing first-Mult2 vectors are retained unchanged; the design does not claim their results depended on a tie case.

### Rejected alternatives

| Alternative | Decision | Exact reason |
|---|---|---|
| `std::complex<double>` / ordinary `MakeCKKSPackedPlaintext` | Reject | The value narrows before encoding and cannot preserve the supplied sub-binary64 witness. Official CKKS packed input storage and context helpers are binary64. |
| `long double` | Reject | Precision and ABI vary by platform and are still too close to a 100-bit contract; the current `PaperScaleDescriptor` is observational metadata, not an exact normalization mechanism. |
| Decimal strings as the primary public DTO | Reject for v1 | A string DTO requires parser grammar, canonical formatting, locale/error policy, and eventually a wire/persistence contract. It is broader than the requested in-process seam. Decimal strings remain a safe way to construct `ClientReal` without binary64. |
| Arbitrary runtime precision exposed as a template/plugin | Reject | It expands API and test combinations without a present use. Fixed public precision plus two fixed internal precisions is smaller and deterministic. |
| Exact rational complex values for every slot | Reject | Transcendental canonical transforms cannot remain rational; a high-precision floating representation is required at the slot boundary. |

**Primary sources:** paper lines 201 and 209–219; `official-full/src/pke/include/cryptocontext.h:363-445,1175-1205`; `official-openfhe/ckkspackedencoding.h:61-101`; `official-full/src/pke/lib/encoding/ckkspackedencoding.cpp:115-333`; current fixture `project/tests/precision_dcp_rcb_fixture.cpp:205-264`.

---

## Decision 2 — official encryption and undecoded decryption route

### Input path

The client module performs these steps without creating an OpenFHE `Plaintext`:

1. Validate the bound context, public key, CKKS-RNS configuration, level-zero full ordered basis, slot geometry, exact scale, and finite values.
2. Execute the multiprecision special inverse transform at cyclotomic order `2N`.
3. Multiply by the exact rational **total coefficient scale once** and apply `round_paper` coefficientwise. OpenFHE standard Encode instead receives a base factor and stages `noiseScaleDeg` through CRT multiplication before recording `pow(base, degree)`; an already-total `2^100` must not be fed through that staged degree-two path again.
4. Require the pre-reduction magnitude check `2*abs(c_i) < Q_active` for every signed coefficient. This rejects an encoding that would enter through modular wrap.
5. Convert each signed `cpp_int` to its nonnegative residue modulo `Q_active`, then to OpenFHE `BigInteger` using decimal-string construction—not a floating conversion. This bridge is supported in v1 only when the installed `lbcrypto::BigInteger` resolves to the inspected dynamic-integer implementation; the supplied hosted command line observes `-DMATHBACKEND=4`, but that macro observation is not generalized into a promise about every OpenFHE integer backend.
6. Build a coefficient-format large `Poly`, then construct an official `DCRTPoly` on the already-configured ordered DCRT parameters. OpenFHE's constructor performs the per-tower remainder reductions.
7. Call `context->GetScheme()->Encrypt(element, publicKey)`. This is a public scheme-level overload and keeps the official PKE implementation; it is not a parallel cryptographic backend.
8. Restore and verify metadata that the high-level `CryptoContext::Encrypt(Plaintext, publicKey)` wrapper normally copies from a `Plaintext`: slots, level, noise-scale degree, recorded binary64 scaling factor, integer scaling factor, and CKKS encoding type. For the first slice those are derived by the module—never caller-authoritative—and are level 0, degree 2, recorded `base*base = 2^100`, and integer factor 1.

Direct scheme encryption does **not** receive the high-level wrapper's key validation, and its returned ciphertext initially has only PKE-core defaults for some metadata. The module therefore repeats the relevant caller-boundary validation and metadata propagation; public visibility of the lower overload is not treated as permission to bypass validation.

For v1, input is deliberately limited to the full, level-zero ordered basis. OpenFHE public-key `EncryptZeroCore` handles a smaller plaintext basis by dropping trailing public-key towers by count. That positional-prefix behavior is not accepted as a generic raised/arbitrary-basis rule.

### Output path

The output route is:

1. Validate private key, ciphertext, immutable client receipt, context pointer identity, key tag, component count, encoding, data type, level, ordered modulus/root basis, format, slots, and exact scale transition.
2. Call `context->GetScheme()->Decrypt(ciphertext, privateKey, &poly)` using the public `Poly*` overload.
3. Check `DecryptResult.isValid`; invalid output returns/fails before any client decoding.
4. Convert the returned coefficient residues to exact integers, center them with respect to the actual active composite modulus, and execute the multiprecision packed projection and special forward transform.

The scheme-level CKKS `Decrypt(..., Poly*)` calls `DecryptCore`, then preserves configured polynomial flooding when the context uses `NOISE_FLOODING_DECRYPT` with `EXEC_EVALUATION`, changes to coefficient format, and performs official CRT interpolation. By contrast, the public `DecryptCore` wrapper calls only the core RLWE combination and does not apply that flooding. **`DecryptCore` is rejected.**

### What standard `Decode` would have done, and what this route omits

A successful ordinary `CryptoContext::Decrypt` sets CKKS metadata and invokes `CKKSPackedEncoding::Decode`. Standard Decode:

- selects strided coefficient positions;
- converts exact coefficient integers to `double`;
- applies a binary64 scale path;
- clears the stored `Poly`/`NativePoly` values;
- computes approximation-error statistics;
- executes the binary64 special forward transform; and
- for `REAL`, applies additional conjugate projection/Gaussian postprocessing and clears imaginary values.

The proposed client decoder intentionally omits that standard Decode and substitutes a multiprecision transform. It therefore does **not** claim equivalent decryption-output protection. It preserves configured **polynomial flooding** by using scheme `Decrypt(..., Poly*)`; it does not silently disable or override context policy. Initial support is restricted to CKKS `COMPLEX` plus `EXEC_EVALUATION`. `REAL`, `EXEC_NOISE_ESTIMATION`, and any configuration relying on standard REAL Decode postprocessing are explicitly rejected until separately designed and reviewed.

**Primary sources:** `official-full/src/pke/include/cryptocontext.h:319-345,1243-1266`; `official-full/src/pke/lib/cryptocontext.cpp:555-602`; `official-full/src/pke/include/schemebase/base-scheme.h:205-247`; virtual PKE declarations/overrides in `official-full/src/pke/include/schemebase/base-pke.h:91-141` and `official-full/src/pke/include/schemerns/rns-pke.h:53-119`; `official-full/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:44-95`; `official-full/src/pke/lib/schemerns/rns-pke.cpp:40-69,148-223`; `official-full/src/pke/lib/encoding/ckkspackedencoding.cpp:336-508`.

---

## Decision 3 — smallest coherent client seam

The public seam has three client transformations plus one clone-only evaluator accessor:

1. `Encrypt`: high-precision slots plus only `{slots, exact logical scale}` → immutable `BoundCiphertext`.
2. `BoundCiphertext::CloneForEvaluation`: a separate OpenFHE clone for the retained homomorphic evaluator under v1's empty-metadata-map rule; the receipt's private snapshot remains unchanged.
3. `BindFirstMult2Rcb`: an already-computed first-Mult2/RCB ciphertext plus the two immutable input receipts → immutable output `BoundCiphertext`; the method derives the logical scale, exact divisors, basis transition, and physical metadata expectations without a caller-supplied transition object.
4. `Decrypt`: private key plus `BoundCiphertext` → owned multiprecision slots and diagnostics.

No `Plaintext` enters or leaves the API. There is no value getter backed by a stale cache, no public raw `Poly`, no mutable metadata setter, no automatic context/key factory, and no general codec/serialization registry.

`BindFirstMult2Rcb` is intentionally operation-specific. It calculates, rather than accepts, the output logical scale:

```text
scale_out = scale_left * scale_right / (q_div * q_l)
```

It validates the physical output state before issuing the receipt. This avoids a generic API in which a caller can attach an unrelated arbitrary scale to a ciphertext. The binder is a structural state adoption boundary, not a cryptographic proof that the supplied ciphertext was computed from those exact operand objects: OpenFHE's returned raw ciphertext carries no unforgeable parent lineage. V1 therefore permits it only for immediate single-process adoption of the direct retained `RCB(Mult2(DCP(leftClone),DCP(rightClone)))` result. Because both accepted parents have the same frozen fresh scale and the accepted output has one exact first-operation state class, the derived normalization is state-correct; substituting another same-key/same-state ciphertext remains caller misuse that the API does not claim to detect. A repeated-operation/general state transition is pending the separate repeated-Mult2 result and is not predesigned here.

Every receipt binds at least:

- process-local context pointer identity and crypto-parameter identity;
- required feature mask plus the complete enabled-feature mask observed at module construction; the required subset must be present, the client never enables it, and later full-mask drift is rejected as shared-context mutation;
- scaling, key-switch, execution, decryption-noise, and CKKS data modes;
- key tag;
- cyclotomic order and ring dimension;
- ordered active moduli **and roots of unity**;
- level;
- packed slot count and projection kind;
- CKKS packed encoding and context CKKS data type;
- all component formats and exact component count;
- a present, empty OpenFHE ciphertext metadata map (the only metadata-map state supported by v1);
- noise-scale degree;
- OpenFHE recorded `double` scaling factor and integer scaling factor as compatibility observations;
- exact reduced rational logical scale as the client authority;
- state origin (`fresh-client-encoding` or `first-Mult2-RCB`);
- the exact `q_div`/`q_l` transition for the latter.

The receipt is immutable and process-local. It privately owns a validated ciphertext snapshot and exposes only `CloneForEvaluation()`, which returns a separate OpenFHE `Clone()` under the required empty-metadata-map rule. It exposes no raw shared handle to the snapshot. The receipt is not a persistence identity and is not serialized.

### Mandatory validation before cryptographic calls

| Invariant | v1 rule |
|---|---|
| Context | Non-null; exact `CryptoContext` pointer match for module, key, and ciphertext. |
| Scheme/features | CKKS-RNS; at construction require `(GetEnabled() & required) == required` for `PKE | KEYSWITCH | LEVELEDSHE`, record the complete enabled mask, and on every later use require exact equality with that recorded mask. Extra features already present at construction are accepted but never used; later mask drift is rejected. The client never calls `Enable`. |
| Integer bridge backend | Build-time `lbcrypto::BigInteger` must resolve to the inspected `bigintdyn` implementation under the supplied `-DMATHBACKEND=4` configuration. Another backend is unsupported until separately sourced and compiled. |
| Key | Non-null; context match; nonempty key tag; decrypt key tag equals receipt/ciphertext tag. |
| Basis | Exact ordered modulus/root/cyclotomic-order match for every tower and every ciphertext component. |
| Input level/basis | Level 0, full configured Q basis only. |
| Format | Constructed plaintext element starts `COEFFICIENT`; encrypted/evaluated ciphertext components must be `EVALUATION`. |
| Encoding/data type | `CKKS_PACKED_ENCODING`, `COMPLEX`, `EXEC_EVALUATION`. |
| Shape | Bind `S` from the context crypto-parameters' configured nonzero batch size; `N` and `S` powers of two; `1 <= S <= N/2`; `N % (2S) == 0`; `FreshEncodingSpec.slots`, value count, and every receipt/ciphertext slot field must equal that bound `S`. |
| Components | Exactly two for fresh encryption and first-RCB output. |
| Ciphertext metadata map | Present and empty. Nonempty entries are unsupported in v1 because OpenFHE `Clone()` copies the map entries while their `shared_ptr<Metadata>` values can remain aliased. |
| Scale metadata | Exact rational valid; the fresh exact scale is the total multiplier applied once. Physical degree/factor are derived, not caller supplied; observed OpenFHE factor must equal the expected compatibility value and `scalingFactorInt == 1`. |
| First-Mult2 state | Derive `q_div`/`q_l` and exact drop-two basis from parents; require level 2, two components, degree 2, and recompute the recorded factor in pinned evaluator operation order as `((base*base)*(base*base)/base)/GetModReduceFactor(fullBasisSize-2)` with exact `double ==`. |

**Primary sources:** feature observation in `official-full/src/pke/include/schemebase/base-scheme.h:93-163`; configured batch-size access in `official-full/src/pke/include/schemebase/base-cryptoparameters.h:76-102`; high-level validation and propagation in `official-full/src/pke/include/cryptocontext.h:319-345,1250-1266`; ciphertext metadata/defaults and cloning in `official-full/src/pke/include/ciphertext.h:72-82,205-316,385-405,499-515`; project validation and tracer feature setup in `project/src/double_ckks.cpp:240-374,440-568,991-1129` and `project/tests/precision_first_mult2_contract_test.cpp:640-659`.

---

## Decision 4 — exact scale ownership

### Value semantics

The client seam uses a local value object with these invariants:

```text
PositiveRationalScale = numerator / denominator
numerator   > 0
denominator > 0
gcd(numerator, denominator) = 1
```

Construction requires both arguments already positive and divides by their greatest common divisor. Zero, either negative argument, or a zero denominator is rejected; a negative denominator is not silently sign-flipped. Products are cross-cancelled before multiplication to limit intermediate growth and then reduced again.

For the frozen fresh input:

```text
scale_in = 2^100 / 1
```

For the frozen first-Mult2 output:

```text
q_div = 1125899906843009
q_l   = 1125899906840833
q_div * q_l = 1267650600226646386227681786497
scale_out = 2^200 / 1267650600226646386227681786497
```

The numerator and denominator above are coprime.

### Two distinct metadata domains

- The **exact rational logical scale** is the total coefficient multiplier applied once and determines coefficient-to-slot normalization in this client path.
- Standard OpenFHE Encode's input factor/degree staging is not reused: its base factor is multiplied through CRT according to `noiseScaleDeg` and its recorded factor becomes `pow(base, degree)`. The direct client already has the total `2^100`; degree 2 remains compatibility/evaluator state and must not multiply that total again.
- OpenFHE's `double` scaling factor, integer scaling factor, level, and noise-scale degree are retained and validated because existing evaluator code uses them as state/compatibility metadata. They do not override or approximate-normalize the exact rational.
- The current `PaperScaleDescriptor`'s `double`/`long double` fields remain observations. This design does not silently promote them to exact authority or replace them before the repeated-Mult2 decision returns.

The exact scale cannot be separately mutated. It is created with the fresh receipt or calculated by `BindFirstMult2Rcb` from the two parent receipts after deriving `q_div` and `q_l` from their exact ordered full basis and verifying that the output basis is the same ordered modulus/root prefix with those two final towers removed. The binder also derives the expected physical state: `base=GetScalingFactorReal(0)`, `fresh=base*base`, `ready=fresh*fresh/base`, and `expectedOut=ready/GetModReduceFactor(fullBasisSize-2)`, evaluated in that order and compared with exact `double ==`. This attaches both metadata domains to the verified first-operation **state class** without confusing the binary64 observation with exact normalization; it does not claim cryptographic operand-lineage attestation.

### Repeated-Mult2 dependency

The selected rational **semantics** can be adopted now; project-wide ownership cannot. After the repeated-Mult2 design returns, there must be one authority:

- if the evaluator introduces an exact state receipt, the client consumes/adapts that receipt and does not maintain a competing scale ledger;
- if no evaluator receipt is adopted, a reviewed operation-specific client transition must compute each scale from immutable parent receipts and the actual removed/introduced moduli.

No repeated operation may infer an exact denominator from the recorded `double`, nominal `2^p`, level alone, or a presumed original-prefix basis.

**Primary sources:** paper RNS relation at lines 1464–1470; standard staged degree scaling in `official-full/src/pke/lib/encoding/ckkspackedencoding.cpp:280-333`; current approximate scale fields and physical operation order in `project/include/openfhe_2023_1788/double_ckks.h:18-29` and `project/src/double_ckks.cpp:240-278,470-540,763-809,991-1110`; exact first-operation evidence in `project/tests/precision_first_mult2_contract_test.cpp:959-1181`, `project/tests/precision_dcp_rcb_fixture.cpp:215-255`, and `project/coordination/MULT2_SCALE_ALGEBRA_CHECK.md`.

---

## Decision 5 — transform order and partial/full packing

Let `N` be the ring dimension, `M = 2N` the cyclotomic order, `S` the declared packed slots, `Nh = N/2`, and:

```text
gap = Nh / S = N / (2S)
```

The slot order is `j = 0..S-1` with canonical exponents:

```text
e_j = 5^j mod 2N
```

### Encoding

1. Apply a multiprecision transcription of OpenFHE `FFTSpecialInv(values, 2N)` to exactly `S` complex values, including the same powers-of-five rotation group, butterfly indexing, bit reversal, and division by `S`.
2. Place transformed real and imaginary parts at:

```text
coefficient[gap*j]      = round_paper(scale * inverse[j].real)
coefficient[gap*j + Nh] = round_paper(scale * inverse[j].imag)
```

3. Set every other coefficient to zero.

### Decoding

1. From the exact centered `Poly`, read only:

```text
centered[gap*j]
centered[gap*j + Nh]
```

2. Multiply each by `scale.denominator / scale.numerator` at multiprecision.
3. Apply a multiprecision transcription of OpenFHE `FFTSpecial(values, 2N)`.
4. Return exactly `S` values in the powers-of-five order above.

This is explicitly named `OPENFHE_PACKED_STRIDE`. It does **not** claim to return the all-coefficient evaluations used by the existing independent Horner oracle when `gap > 1`.

### Why the distinction matters

At the current diagnostic `N=64`, `S=16`, `gap=2`. Standard OpenFHE Decode observes even-index strided coefficient pairs. Encryption and evaluation noise need not be confined to those indices. The current independent Horner oracle instead evaluates all 64 centered coefficients at each canonical root. These are different observations under partial packing and must remain separately labelled.

A useful independent mathematical witness is the monomial `X` at `N=64,S=16`: its **message contribution** to the packed-stride projection is zero because coefficient index 1 is off-stride, while a full Horner evaluation returns the nonzero canonical roots. An encrypted/decrypted instance can still contain projected RLWE or configured flooding noise, so no bit-exact zero cryptographic output is inferred. Conversely, `1`, `X^(N/2)`, and `X^2` provide literal/order witnesses for the two halves and powers-of-five order.

At the paper's full packing `S=N/2`, `gap=1`; the strided pairs cover all `N` coefficients, so the omission disappears and the packed transform has the paper's full-slot semantics. The current `N=64,S=16` diagnostic is not evidence for the paper's `N=32768,S=16384` performance or experiment.

### Complexity decision

The default production route is a multiprecision special FFT:

- precompute the `5^j mod 2N` rotation group and multiprecision roots for the bound `(N,S)` geometry;
- execute `O(S log S)` butterflies for the packed projection;
- perform `O(N)` coefficient placement or centered-coefficient selection.

Official DCRT construction and CRT interpolation remain library operations whose additional work depends on the active tower count `L`; this design accounts for them separately and does **not** collapse them into an `O(N)` claim. A full all-coefficient canonical route, where separately needed for evidence, should use an `O(N log N)` multiprecision transform rather than `N/2` Horner evaluations over `N` coefficients. The existing direct Horner oracle remains valuable only as an independent small-`N` test. Complexity is not a runtime benchmark and no timing claim is made.

**Primary sources:** paper lines 209–219; `official-openfhe/dftransform.cpp:50-69,209-268`; official encode stride placement in `official-full/src/pke/lib/encoding/ckkspackedencoding.cpp:288-333,516-533`; official decode extraction in `official-full/src/pke/lib/encoding/ckkspackedencoding.cpp:336-405,493-508`; current all-coefficient oracle and order in `project/tests/precision_first_mult2_contract_test.cpp:395-486`; fixture stride in `project/tests/precision_dcp_rcb_fixture.cpp:229-247`.

---

## Decision 6 — precision, centering, checks, failures, and ownership

### Exact coefficient conversion and centering

The module never converts a decrypted coefficient through `double`, `long double`, or a native-width integer. OpenFHE `BigInteger::ToString()` is parsed into `cpp_int`. Let `Q` be the exact product of the active ordered moduli and `r` a residue in `[0,Q)`:

```text
center(r) = r      if r <= floor(Q/2)
            r - Q  otherwise
```

The module records:

```text
max_centered_abs = max_i abs(center(r_i))
centered_headroom = floor(Q/2) - max_centered_abs
```

It may report the integer and its floor-log2 bit measure. This is a check of the **actual final centered representatives**. It is not a theorem about prior integer/noise history, not proof that no earlier modular wrap occurred, and not an all-key guarantee. In particular, low-component coefficients may be amplified by secret convolution before final decryption.

### Shape and magnitude checks

Reject before encryption when:

- a value is nonfinite;
- `N`/`S` geometry is unsupported;
- the exact scale is invalid;
- either precision run cannot make a stable rounding decision;
- any pre-reduction coefficient fails `2*abs(c_i) < Q_active`;
- a coefficient cannot be converted exactly through the integer/string route.

Reject before or after decryption when:

- context, key tag, basis, roots, level, components, format, slots, encoding, data type, or metadata differ from the receipt;
- decryption returns invalid;
- a residue or active modulus is malformed;
- the two decoding precisions differ by more than `2^-120`;
- an unsupported execution/decryption mode would omit required standard postprocessing.

### Failure classes

The eventual implementation should throw without catch-and-translate inside the module:

- `std::invalid_argument`: malformed caller value/specification, empty/null input, invalid rational, invalid slot geometry;
- `std::domain_error`: unsupported context/mode, context/key/state mismatch, invalid decryption result;
- `std::range_error`: nonfinite value, coefficient headroom failure, exact-conversion failure, precision-instability failure;
- OpenFHE exceptions propagate unchanged when raised by the official primitive.

No `try/catch` is added unless a future application boundary has a deliberate recovery/translation policy.

### Immutability and cache ownership

- Input value vectors are received by const reference and copied only into local transform buffers.
- Each `HighPrecisionClientIO` instance owns immutable multiprecision rotation/root/bit-reversal tables keyed by its validated `(context identity, N, S, projection)` profile. The module does not reuse or mutate OpenFHE's binary64 DFT cache, install a process-global mutable cache, or mutate the shared cryptographic context. A changed context profile is rejected rather than causing an in-place cache rebuild.
- `PKERNS::Encrypt` takes the DCRT plaintext element by value before changing it to evaluation format; caller values, key, and context are not mutated.
- `BoundCiphertext` privately owns a validated ciphertext snapshot. OpenFHE `Clone()` creates a separate ciphertext and element vector, but `CloneEmpty()` copies metadata-map entries whose `shared_ptr<Metadata>` values need not be deep-copied. V1 therefore requires the metadata map to be present and empty before snapshot creation, before evaluator export, and on return from the first RCB binder. Under that supported state, `CloneForEvaluation()` cannot alias a metadata value object back into the receipt. A generic metadata deep-copy policy is deliberately out of scope.
- Raw `Poly`, DCRT construction buffers, and exact coefficient arrays remain local and are destroyed after use. No claim of compiler-proof secure erasure is made.
- `DecodedSlots` owns its returned multiprecision vector. It has no relation to an OpenFHE `Plaintext` cache.
- No production source may depend on `tests/precision_dcp_rcb_fixture.*`.

**Primary sources:** backend-specific exact integer constructors/formatting in `official-full/src/core/include/math/hal/bigintdyn/ubintdyn.h:186-214,730-738,829-837` and the scope qualification in `project/coordination/PRODUCTION_IO_OFFICIAL_API_AUDIT.md:28-36`; official Poly/DCRT conversion in `official-full/src/core/include/lattice/hal/default/poly.h:81-91,166-219`, `dcrtpoly.h:84-88`, and `dcrtpoly-impl.h:59-68,767-796`; by-value encryption in `official-full/src/pke/lib/schemerns/rns-pke.cpp:40-69`; ciphertext metadata-map and cloning behavior in `official-full/src/pke/include/ciphertext.h:319-405`.

---

## Decision 7 — frozen proposed tracer bullet

No test is authored now. After explicit user confirmation, the first public-seam contract is exactly the contract in `PROPOSED_FIRST_TDD_CONTRACT.md`:

- one freshly generated keypair and multiplication key under the frozen `N=64,S=16`, `FIXEDMANUAL`, `HYBRID`, `COMPLEX`, `EXEC_EVALUATION`, `HEStd_NotSet` diagnostic context; required features are `PKE | KEYSWITCH | LEVELEDSHE`;
- exact `ClientComplex` input tables, including the sub-binary64 adjacent-slot witness;
- public `Encrypt` for both operands;
- retained public `DCP -> Mult2 -> RCB` evaluator seam;
- operation-specific `BindFirstMult2Rcb` and public `Decrypt`;
- exact output scale `2^200/(1125899906843009*1125899906840833)`;
- all 16 results compared with an independent host product at absolute complex error `<=2^-80`;
- adjacent product-delta compared independently at `<=2^-80`;
- independent small-`N` literal/monomial/order evidence and the explicit partial-projection witness;
- retained old 55 tests and existing independent all-coefficient first-Mult2 evidence unchanged.

The predicted genuine first RED is compile-time absence of the proposed public header/types/functions. No runtime failure is claimed before code exists.

The smallest future GREEN scope is one public header, one production source, additive CMake registration, and one new public-seam test source. It does not change the evaluator or old thresholds.

---

## Decision 8 — repeated-Mult2 handoff

The client decisions that are independent now are:

- fixed multiprecision public complex values;
- no `Plaintext`/cache exposure;
- official DCRT construction and scheme Encrypt path;
- scheme `Decrypt(..., Poly*)`, not `DecryptCore`;
- retained configured polynomial flooding and `COMPLEX`-only initial support;
- exact integer conversion, centering, packed-stride transform/order;
- immutable validated-snapshot context/key/feature/basis/shape receipts with empty metadata maps and clone-only evaluator export;
- positive reduced rational scale semantics;
- client/evaluator key ownership separation.

The following cannot be frozen generically until the separate repeated-Mult2 result returns:

- the exact scale transition at every repeated stage, including pair-member versus recombined scale;
- the identity and ownership of raised/permuted Q bases and exact ordered roots;
- whether basis-specific contexts/keys are introduced and how pointer identity is represented;
- HYBRID/BV key-switch table and digit-partition compatibility on the active basis;
- generated `P` basis identity, `dnum=11` behavior, and paper `h=128` key route;
- the correct source of level/noise-scale/recorded-factor metadata after refresh/re-entry;
- which evaluator operation issues the authoritative exact state receipt;
- the lifecycle admitted after `RefreshRequired` and the paper's eight repeated squarings.

`BindFirstMult2Rcb` is therefore a deliberately narrow bridge, not the repeated-state architecture. `REPEATED_MULT2_HANDSHAKE.md` contains the exact reconciliation checklist. The live separate task was not contacted, stopped, refreshed, or assumed.

---

## Supported initial configuration

The first implementation slice, if confirmed, supports only:

```text
OpenFHE pin             df495ba2e91739a6dc8f1de254fc5a41155ce504
Integer backend          supplied -DMATHBACKEND=4 configuration, with lbcrypto::BigInteger required to resolve to the inspected bigintdyn implementation
Scheme                  CKKS-RNS
Required features       PKE | KEYSWITCH | LEVELEDSHE (verified, never enabled)
Scaling                 FIXEDMANUAL
Key switching           HYBRID (integration context)
Execution/data type     EXEC_EVALUATION / COMPLEX
Tracer decrypt mode     FIXED_NOISE_DECRYPT (asserted; not changed by client)
Security level          HEStd_NotSet, diagnostic only
Ring / cyclotomic       N=64 / M=128
Slots / stride          S=16 / gap=2
Fresh level/components  0 / 2
Fresh scale degree      2
Fresh exact scale       2^100
First RCB level/components 2 / 2
First output scale degree  2
q_div                   1125899906843009
q_l                     1125899906840833
Projection              OPENFHE_PACKED_STRIDE
```

The active ordered moduli and roots are not reconstructed from level numbers; they are read from the actual context/ciphertext and matched element-by-element. A different integer backend is rejected until its exact string-in/string-out bridge is source-reviewed and compiled in the intended hosts. `HEStd_NotSet` remains unsuitable as a paper/security configuration. Full paper `N=32768,S=16384`, `h=128`, `dnum=11`, Table 3 modulus schedule, eight squarings, and 1,000-run mean infinity-norm evidence remain separate obligations.

## Final design disposition

**PROPOSED / READY FOR USER SEAM CONFIRMATION.** The design is concrete enough for a single tracer-bullet TDD slice, but it authorizes neither tests nor implementation. It does not waive repeated multiplication, paper parameters, security configuration, or empirical paper evidence.
