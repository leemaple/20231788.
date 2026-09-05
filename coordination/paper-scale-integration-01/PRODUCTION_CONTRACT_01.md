# Paper-scale production contract — frozen before implementation

Engineering baseline: `9c4d83b5cde16e5c5af89886bd73fe5252a99002`.
This joins the accepted I/O, repeated-evaluator and fixed-Q h128 modules into the
actual paper §6.3/Table3 experiment. Existing 60 tests and all five compile-only
API targets remain unchanged. The current user removed 1,000-trial and statistical
performance gates, not the paper-scale correctness objective.

## Scope and minimal public interface

Use one N32768/M65536/full16384/gap1 input, one root secret of actual signed
ternary Hamming weight128, and eight sequential squarings without intermediate
decrypt/re-encrypt, bootstrap or §6.2 refresh in the evaluator. Client-only
fresh/final observations and an independent client oracle are allowed. Pass
only the immutable public plan and ciphertext into the evaluator.

Add these narrowly scoped interfaces (ordinary names live in
`openfhe_2023_1788`; I/O remains in its `client_io` namespace):

- `RepeatedMult2ClientSetup CreatePaperRepeatedMult2Setup();`
- `HighPrecisionClientIO(std::shared_ptr<const RepeatedMult2Plan> plan);`
- `RepeatedMult2Result DoubleCKKS::RCBWithReceipt(const CiphertextPair&) const;`
- `BoundCiphertext HighPrecisionClientIO::BindRepeatedRcb(const RepeatedMult2Result&) const;`

`RepeatedMult2Result` is a privately constructed immutable value issued only by
the evaluator. It exposes `GetCiphertext()` as `ReadOnlyCiphertext` and
`GetReceipt()` as a const reference to the owned
`shared_ptr<const RepeatedMult2Receipt>` for independent client-side checks.
It retains its issuing plan and owned ciphertext snapshot. No public constructor,
setter or caller-supplied scale may manufacture an accepted result. This is
normalization/state provenance, not cryptographic operand-lineage authentication.

RCBWithReceipt requires an issued terminal Rescaled receipt. Recombine using the
correct family's existing RCB, validate actual basis/root/order and same-root
ownership, then wrap unchanged elements into original B0/context/root tag,
absolute level9 for the paper profile and the two-Base prefix. Preserve
degree2, recorded2^100, integer factor1, slots16384 and empty metadata. Bind
validates the issuing plan, result/receipt/terminal state and live context
before taking its own snapshot and exact S8. Expose a distinct
`ClientCiphertextOrigin::RepeatedMult2Rcb`; first-operation factors remain absent.
Keep strong ownership through I/O/result/bound state as needed; cached State
values are not live-mutation authorization.

The original context-based I/O constructor and BindFirstMult2Rcb retain the
strict N64/Q8/UNIFORM first-operation contract. The original diagnostic setup
retains N64/two-family and its frozen semantics. Generalize only internal
geometry/transform/family plumbing needed by these two explicit profiles.
Do not add a generic parameter framework, arbitrary-result binder, refresh
adapter or alternative backend.

## Parameter and client setup contract

Use the exact ordered eleven Q modulus/root pairs in the included
`historical-probe/table3_profile_probe.cpp` (`kFullQ`) and reserved P
1152921504606584833/root4443670208963. These values came from a parameter-only
probe; they are not prior evaluator evidence.

B0..B7 have11..4 full-Q towers, always preserving the final Div and removing
each actual second-last Mult. Base primes are50-bit, eight Mult are60-bit,
Div is40-bit, P is60-bit. Use nominal50/initial recorded2^100 for FIXEDMANUAL
compatibility; use actual d and m for exact arithmetic. Never inherit the
probe's EncodingParams100. Require native64/backend4, HYBRID/alpha1,
STANDARD, PRE NOT_SET, COMPLEX, compositeDegree1, noiseScale1, SPARSE_TERNARY,
HEStd_NotSet and the existing supported noise/features. Validate actual
factory-returned Q/P/QP/partitions and modes, not only requested parameters.

Call CreateFixedQH128ClientKeyPair exactly once in B0. Check actual h128 signed
ternary coefficients client-side; project that same secret to each actual Bi
by complete modulus/root/phi identities and generate only its family-local
evaluation row. No fresh secret per family and no private key in plan Data.
Keep row ownership, immutable seals, unrelated-cache preservation and cleanup.
Do not set DEBUG_KEY or rewrite official OpenFHE.

## Scale and transform contract

Read NOMINAL_SCALE_AUDIT_01.md. Metadata follows
2^100 -> Tensor2 2^150 -> RS2 2^100 every round. Exact scale follows
S0=2^100 and S_i=S_(i-1)^2/(d*m_i); reentry preserves it. At round8 S8 differs
materially from2^100. Production decrypt must remain the public Poly* route
followed by high-precision full coefficient decoding with receipt S8.
Do not substitute standard Plaintext/CKKSPackedEncoding::Decode, DecryptCore,
binary64 slot data, or scalar/plaintext multiplication convenience APIs.

The existing positive-forward / negative-inverse special-FFT convention and
conservative rounding remain. Use dynamically sized heap storage for the
paper geometry, independently generated160/220-decimal roots and the existing
ambiguity/nonwrap guards. Do not place paper-sized transform arrays on the
Windows stack or use an O(N^2) production transform.

## Frozen behavior test

Read INPUT_DOMAIN_AUDIT_01.md for the complete rational generator, actual
prime consumption order, numerical bounds and its four-phase blind spot.
Do not change its input generator or numerical gates after observing results.

A single paper test per host performs this full sequence:

1. Client setup, actual N/full basis/partitions/h128 checks, then production
   Encrypt of all16384 generated values at exact2^100.
2. Production fresh Decrypt checks every slot against independently generated
   rational input at max real/imag component error<=2^-80. Independently
   evaluate the encrypted polynomial at fixed ten anchors to check phase,
   order and the2^-75 sub-binary64 witness.
3. Evaluator receives no secret. DCP once; call the existing public Mult2 on
   the previous pair with itself eight times. Keep public stage receipts and
   check family/local level/component/metadata/exact rational scale against a
   test-owned integer recurrence, preserving original ciphertext/key state.
   Return read-only pair observations to the client test for the per-round
   oracle below; no staged duplicate eight-square chain is required.
4. RCBWithReceipt, BindRepeatedRcb, and final production Decrypt. All16384
   values must match independent z^256 componentwise within2^-80; codec
   cross-precision disagreement stays<=2^-120. Check meaningful nonzero
   values, positive headroom, the slot0/1 output-difference witness, and a
   wrong-2^100-normalization falsifier.
5. A client-only independent oracle uses sparse signed-h128 c0+c1*s,
   negacyclic arithmetic and exact CRT at fresh and final stages; evaluate
   via direct high-precision Horner at anchors
   {0,1,256,257,512,513,768,769,1023,16383}. It does not call production FFT,
   public Decrypt or current fixture's O(N^2) convolution to obtain its
   independent polynomial. Official inverse NTT is an explicit common
   dependency. Compare anchors to both expected values and production values
   using2^-80; keep independent numerical work well beyond120 bits.
   At each of the eight returned pair observations, independently evaluate
   the high and low ciphertext polynomials, combine d*high+low modulo its
   actual active Q, and apply direct Horner at the same anchors divided by
   independently computed S_i. Compare to independent z^(2^i) with2^-80.
   This is client-side per-round evidence after evaluation, not an evaluator
   decryption/refresh path or another encrypted chain.
6. Verify nonterminal output rejection, a foreign plan/result rejection
   without a second paper keygen (the existing small diagnostic plan is a
   foreign identity), and private-constructor/state ownership by actual
   supported interfaces. Preserve all existing numerical/negative tests;
   add further cycles only for a concrete uncovered failure.

Generate expected z^256 with a separate sufficiently precise plaintext
arithmetic implementation (for example binary multiprecision), not the
encrypted algorithm or production transform. Expected input is exact dyadic;
four-phase rotation uses component swaps/signs. Avoid full-slot giant-Fraction
expansion; the audit's exact scalar anchors provide independent controls.
All new errors/receipts must be emitted as parseable finite values with source
SHA, OpenFHE pin, actual profile and final completion markers.

The numeric2^-80 target is frozen pre-run; it is not an all-Gaussian-key theorem
or a reproduced1,000-run mean. Failure is evidence to diagnose, not permission
to relax the target or select inputs after the fact.

## TDD and hosted execution

The first patch adds only the new public-behavior test/helper plus CMake and
workflow wiring. Missing new public declarations/linkage is honestly API RED,
not an executed semantic RED. Do not use expected-unsupported exceptions,
static_assert-only shape checks or stub success as the GREEN proof.
The subsequent implementation patch is applied only after the first hosted
RED is retained; GREEN must execute the actual full sequence above.

Name the executable `paper_full_eight_square_contract_test` and CTest
`paper_full_eight_square_contract`, as new entry61. Preserve all60 old
name/command/order entries and source test bodies. EXCLUDE_FROM_ALL and explicit
build follow the existing project convention. In the existing workflow:

- add the new branch trigger `codex/paper-scale-implementation-20260905`;
- exclude only the new paper test from existing legacy57 and complete60 runs;
- after the complete60 checkpoint, explicitly build the paper target, show
  the full61 live listing and execute only the paper test once per host;
- keep exact upstream pin, native64/backend4, Linux cache and Windows no-dot
  build paths. Give the new CTest a1200-second bound, RUN_SERIAL, OMP_NUM_THREADS=2.

No local Mac configure/build/crypto. No normal CI dispatch/rerun or default
branch merge. Preserve failed first execution and diagnose exact source.
No1000-trial, performance-reproduction or extra security-certification gate.
