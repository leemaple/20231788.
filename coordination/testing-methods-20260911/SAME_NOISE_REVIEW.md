# Blind review: same-in-memory-noise public `Encrypt` diagnostic

Date: 2026-09-11 (Asia/Shanghai). Clean-room project source reviewed at fixed
commit `33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1`. Official OpenFHE source is
pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`, read only from the already
verified 486-member packet identified by
`coordination/testing-methods-20260911/PACKET_RECEIPT.json:L1-L13`. Every source
line below is fixed to those commits. This judgment was formed without the
other reviewer's final answer. No code was drafted, compiled, or executed; no
transform, sampling, encryption, CI, browser, or Git action was performed.

## Verdict

A same-in-memory-noise test is sound **only if it observes the exact zero
ciphertext returned inside the one production public-encryption call, before
the official encryptor adds the message**. The cleanest existing seam is a
test-only virtual `EncryptZeroCore(publicKey, params)` observer that delegates
to the official `PKERNS` implementation, deep-copies its two returned elements,
and returns the original vector unchanged. The inherited official
`PKERNS::Encrypt` must still perform the message format conversion, addition,
and ciphertext construction.

Calling `EncryptZeroCore(publicKey)` separately and then calling
`HighPrecisionClientIO::Encrypt` is unsound for this purpose: those are two
different random draws. Deriving `Z` after the fact as `C-(m,0)` and then
checking `C=Z+(m,0)` is tautological. Resetting or fixing the process PRNG would
neither prove the same call trace nor be an acceptable production-randomness
test.

With the pre-add capture and the independent integer oracle below, the test can
establish two bounded facts:

1. for one N64/S16 call, the public client seam inserts the selected exact
   message once, into coordinate zero only, without changing coordinate one;
2. for that same captured zero-encryption sample, the independently recovered
   phase obeys the successful-source finite-support bound
   `39*(64+h+1)`, where `h` is the actual nonzero weight of the N64 ternary
   secret and hence the bound is at most `5,031`.

It cannot establish typical noise, distribution quality, security, a paper
N32768/h128 result, historical ciphertext identity, or the cause of run
34039088536.

## Why the observation seam is real rather than a replacement encryptor

The project public method validates the context/key, recomputes the exact
integer encoding, creates the official `DCRTPoly` element, and calls
`binding.context->GetScheme()->Encrypt(element, publicKey)`
(`src/high_precision_client_io.cpp:L607-L636`). That `SchemeBase::Encrypt`
overload is virtual and delegates to its installed virtual `m_PKE->Encrypt`
(`official/src/pke/include/schemebase/base-scheme.h:L196-L213,L1552-L1556`).

For DCRT CKKS the installed PKE object's operative method is
`PKERNS::Encrypt`. It obtains `ba` by a virtual
`EncryptZeroCore(publicKey, ptxtParams)`, changes the supplied plaintext to
evaluation format, adds it only to `(*ba)[0]`, and moves both elements into the
ciphertext (`official/src/pke/lib/schemerns/rns-pke.cpp:L56-L69`). The public
zero-core overload is itself virtual
(`official/src/pke/include/schemerns/rns-pke.h:L109-L119`). Its official body
samples dense ternary `v` and Gaussian `e0,e1`, then returns
`(pk0*v+e0, pk1*v+e1)`
(`official/src/pke/lib/schemerns/rns-pke.cpp:L148-L196`).

Therefore a test-only PKE subtype may override **only that exact public-key
zero-core overload**, call the base-qualified official implementation with the
unchanged `publicKey` and unchanged `params`, deep-copy the returned two
elements, and return the original vector. It must not override `Encrypt`,
reimplement its addition, call a second zero encryption, move from the captured
vector, or return a reconstructed clone. An exact `override` declaration is a
compile-time guard against accidentally hiding the overload. Override only the
public-key overload. Ordinary key generation constructs `(b,a)` directly in
`KeyGenInternal`, without traversing either zero-core overload
(`official/src/pke/lib/schemebase/base-pke.cpp:L47-L97`); the one expected
capture therefore remains the payload call.

The capture must be a deep value copy made before returning. Keeping only the
returned vector pointer is insufficient because `PKERNS::Encrypt` subsequently
mutates its first element in place. The observer should also retain, in memory,
the call count and the `params` identity; require exactly one captured public
zero call and the full-basis plaintext parameters. This is evidence that the
intended virtual hook was actually traversed.

## Context construction and virtual-dispatch gates

The ordinary CKKS scheme installs a concrete `PKECKKSRNS` when PKE is enabled
(`official/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp:L42-L47`), so an
ordinary `GenCryptoContext` cannot be modified afterward through a public
setter. A test-only `SchemeCKKSRNS` subtype can install the observing PKE through
the protected `m_PKE` member, while delegating all other feature behavior and
using the existing `SetKeySwitchingTechnique(HYBRID)` path. It must expose the
same enabled feature mask expected by the N64 client profile; the project checks
PKE, KEYSWITCH, LEVELEDSHE and the complete parameter/profile identity before
use (`src/high_precision_client_io.cpp:L180-L245,L266-L278`).

The custom context must be created through the public
`CryptoContextFactory<DCRTPoly>::GetContext(params, scheme, CKKSRNS_SCHEME)`
with already generated N64 parameters, not by a bare `make_shared` of
`CryptoContextImpl`. `KeyGen` asks the factory registry to recover the shared
context for `this` and throws if it is absent
(`official/src/pke/include/cryptocontext.h:L116-L125,L1225-L1241`); the factory
registers a newly created context
(`official/src/pke/lib/cryptocontextfactory.cpp:L41-L76`). Pass the CKKS scheme
identifier explicitly so the project's CKKS guard is not relying on a later
mutation. Create only one observing context. The standard scheme's equality
operator requires its exact dynamic type, so a test subtype does not alias the
ordinary context in the factory
(`official/src/pke/include/scheme/ckksrns/ckksrns-scheme.h:L56-L66`).

These are setup-validity gates, not numerical assertions. A context-registration,
feature-mask, overload, or capture-count failure invalidates the fixture; it is
not evidence of an encryption arithmetic defect.

## Non-tautological exact integer oracle

### 1. Fix the message independently of the production encoder

Reuse the already controlled N64/S16 polynomial, with `S=2^100`:

```text
M[0]  =  3S/8
M[2]  =   S/8
M[6]  =  -S/16
M[32] =  -S/8
M[34] =  2^25
all other M[j] = 0.
```

It occupies both halves of the legal `gap=2` projection, is evaluated into
slots by the independent Binary512 direct-Horner helper, and already has the
exact `InspectEncoding == M` control
(`tests/s100_fresh_error_diagnostic_test.cpp:L220-L245,L276-L280,L320-L330`).
The construction oracle must keep this literal `M` as its expected value; it
must not define expected coefficients by rereading the plaintext argument seen
by the capture, by subtracting the returned ciphertext, or solely by accepting
the second `ComputeEncoding` result.

Let the returned production components be `(C0,C1)` and the pre-add captured
components be `(Z0,Z1)`. Check exact element/basis/format identity first. Then:

- require exact `C1 == Z1`;
- for every tower `q_i` and evaluation index `k`, independently evaluate the literal `M` as
  `sum_j M[j]*r_i^((2*bitreverse_6(k)+1)*j) mod q_i`, where `r_i` is that
  tower's recorded primitive 128th root, and require the exact modular powers and
  accumulation with integers. Read the raw evaluation entries and require
  `C0_i[k] == (Z0_i[k] + Eval_i,k(M)) mod q_i`, computing the right side with
  `cpp_int`, not a DCRT addition or subtraction.

Do not construct the expected DCRT element with the same project
`Poly -> DCRTPoly -> SetFormat` path being tested. Comparing native residues to
the direct modular evaluation of the literal integer vector prevents a shared
constructor, forward/inverse transform pair, scale, sign, lane, or
message-coordinate error from producing a common-mode pass. `Poly::SwitchFormat`
uses a forward transform whose documented output is bit-reversed evaluation
order, and its root table stores powers at bit-reversed indices
(`official/src/core/include/lattice/hal/default/poly-impl.h:L419-L440`;
`official/src/core/include/math/hal/intnat/transformnat-impl.h:L303-L310,L714-L742`).
For N=64 this yields the exponent order stated above. A coefficient-converted
`C0-Z0` may be retained as a secondary diagnostic, but must not be the independent
acceptance oracle for the post-encoding claim.

This direct evaluation is narrowly necessary because the claim is that the
fixed integer `M` reached the observed post-encoding evaluation boundary. If
the intended claim were only the library-internal identity `C=Z+(D,0)`, then
official inverse `SetFormat` would be adequate but explicitly conditional on
the library's paired transform convention. The direct check does **not** prove
all NTT semantics, root generation, multiplication, or inverse conversion; it
checks one fixed N64 message, the actual ordered Q roots, and the evaluation
coordinates consumed by this addition.

### 2. Recover the same captured zero phase independently

Read the generated N64 secret only in the test process. Across every Q tower,
coefficient-convert a copy, require coherent residues in `{-1,0,1}`, form its
signed 64-entry vector, and count its actual weight `h`. For each tower,
coefficient-convert copies of `Z0,Z1` and compute

```text
F_i = Z0_i + Z1_i * s  in R_{q_i}
```

by an independent 64-term negacyclic schoolbook convolution with exact/modular
integer additions. CRT-reconstruct each coefficient with `cpp_int`, center it
in the full composite Q, and assert

```text
max_j |F[j]| <= 39*(64+h+1) <= 5031.
```

This must not call production `Decrypt`, official `DecryptCore`, a slot decoder,
or define `F` as production-decrypted `C-M`. The existing paper oracle shows the
required independent pattern—coherent signed-secret extraction, per-tower
coefficient conversion, negacyclic convolution, exact CRT, and centering—at
`tests/paper_full_eight_square_oracle.h:L198-L223,L225-L262`; the N64 version
must allow the actual uniform-ternary weight rather than assert h128.

The bound follows from the same successful official source support argument:
public encryption has `f=e_pk*v+e0+s*e1`, `||v||_1<=N`, Gaussian coefficient
support 39, and secret one-norm `h`, hence `||f||c<=39*(N+h+1)`
(`coordination/initial-lift-nonwrap-20260909/INDEPENDENT_INITIAL_MAP.md:L190-L217,L219-L242`).
This observes only aggregate `F`; it does not reveal or claim to recover
`e_pk`, `v`, `e0`, or `e1` individually.

Unlike the direct message-construction oracle, this phase observer still uses
official `SetFormat` to recover coefficient representatives for `Z0`, `Z1`, and
the secret. It is independent of project decode, official `DecryptCore`, DCRT
multiplication, and CRT interpolation, but **not** independent of the official
inverse NTT or its agreement with the forward NTT used when the small sampled
polynomials entered evaluation form. A paired forward/inverse convention defect
could cancel. Consequently the N64 support-bound result must be reported as
conditional on that ring/NTT representation dependency. A separately authored
direct inverse/interpolation oracle would be needed to remove it, but that would
expand this narrow construction test into an NTT-semantics project and is not
required for the selected post-encoding addition claim.

## Mutation requirements

At least these two controlled mutations are needed; neither mutates production
source or claims to be a faulty real encryption:

1. **Independent-message mutation:** change one copied expected legal-lane
   coefficient, for example `M[2] -> M[2]+1`, while leaving the slots,
   production call, returned ciphertext, and captured zero unchanged. The
   direct all-tower evaluation oracle must fail. Also run its wrong-order
   sentinel once: replace `bitreverse_6(k)` with `k` in a copied expectation and
   require at least one mismatch. These demonstrate that the expected value and
   ordering are not sourced from the production plaintext, `C-Z`, or official
   `SetFormat` itself.
2. **Coherent-zero mutation:** after the baseline observation, copy `Z0` and
   alter one coefficient coherently in every tower so that the independent
   centered phase at a selected index becomes exactly `B+1`, where
   `B=39*(64+h+1)`. Rerun the complete schoolbook/CRT observer on the copied
   pair; the support assertion must fail. This tests the observer and bound
   together, rather than merely feeding `B+1` directly to the final comparator.

An additional cheap construction sentinel may add the literal message a second
time to a copied `C0`; the residue oracle must reject it. Swapping two labels in
printed summaries, mutating only a postcomputed maximum, or checking that a
value equals itself is not a useful mutation.

Mutation PASS means the assertion rejected the controlled bad copy. A build
failure, timeout, missing capture, or exception before the targeted assertion
is not a killed numerical mutation.

## Secret and evidence boundary

The observing PKE state, `(Z0,Z1)`, private key, signed secret, actual `h`,
per-tower phase, aggregate `max|F|`, and coherent mutation remain process-local.
Do not print, serialize, hash for publication, attach, or retain them. Output
only fixed public parameters, capture count, assertion labels, the conservative
public bound `5,031`, and final PASS/FAIL.
Snapshot the public key and input before the call and verify they are unchanged;
destroy all diagnostic values on process exit. This is client-side diagnostic
secret use, not evaluator access or an acceptance-chain decrypt/refresh.

## Claim boundary for original S100

N64 uses different ring dimension, Q, ordinary uniform-ternary key generation,
and an actual `h<=64`; it does not traverse the project's paper h128 key factory
or the N32768 plan-owned client constructor. A baseline pass would rule out the
selected exact message-insertion defect only at this generic public seam and
would validate one N64 support-bound sample. It would not make the historical
S100 draw replayable or explain its E80 miss.

A baseline construction failure, after all dispatch/setup gates pass and both
mutations are effective, would be evidence of a current generic public-Encrypt
contract defect worth reproducing at the paper seam. A support-bound failure
would contradict the guarded successful-source sampler/profile premise for the
new N64 draw. Neither result may be attributed to run 34039088536 without its
unretained historical key, randomness, and ciphertext chain. No outcome from
this test alone authorizes changing S100 parameters, input, threshold, noise,
or production arithmetic.
