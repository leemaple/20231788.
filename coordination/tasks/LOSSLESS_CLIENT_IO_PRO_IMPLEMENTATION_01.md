# Production lossless client I/O implementation 01

## Authority, source identity, and objective

Implement the first production, test-driven high-precision client I/O slice for
the clean-room OpenFHE 2023/1788 project.  This is an independently isolated
track whose result will later be reconciled with the repeated-`Mult2` family
receipt.  It is not permission to claim the paper experiment complete.

The implementation source base is exactly commit
`4ccc8fd2e7617625d27e58a53eb3489e99466ed4` on branch
`codex/lossless-io-implementation-01`.  Its production source, tests, CMake and
workflow are byte-identical to tested commit
`4ecbd972429884489918d9f82dfc3fe9f702ef4a`; retained GitHub Actions run
`33892550947` passed the existing 57/57 tests on Linux and Windows.  Verify
these facts from the supplied Git snapshot and evidence; do not infer them from
this paragraph alone.

The pristine dependency is OpenFHE 1.5.0 commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.  Use only the supplied clean-room
project, user paper, and exact official-source excerpts.  Do not fetch or use a
different OpenFHE version, any locally modified OpenFHE, or any earlier local
implementation of 2023/1788.

The confirmed public behavior in `coordination/TEST_SEAMS.md` is client-owned
high-precision input construction/encryption and final decryption, with the
evaluator receiving no secret.  Codex accepts the narrow v1 interface described
in the design return after applying the two independent review corrections
below.  Do not ask for the old invented confirmation token.

Deliver a genuine end-to-end tracer RED/GREEN, including the malformed-key
safety correction needed before calling upstream primitives, followed by one
small clone/parameter-safety RED/GREEN pair for this public path:

```text
ClientComplex[16]
  -> HighPrecisionClientIO::Encrypt
  -> BoundCiphertext::CloneForEvaluation
  -> DoubleCKKS::DCP
  -> DoubleCKKS::Mult2
  -> DoubleCKKS::RCB
  -> HighPrecisionClientIO::BindFirstMult2Rcb
  -> HighPrecisionClientIO::Decrypt
  -> ClientComplex[16]
```

The evaluator path between the two client boundaries must contain no private
key, decryption, re-encryption, bootstrap, or Section 6.2 refresh.

## Required reading and precedence

Read these packet paths completely before designing or editing:

- `TASK.md` (this assignment; it wins over older prose on conflict);
- `project/.agents/skills/openfhe-2023-1788-workflow/SKILL.md` and its
  `references/engineering.md`;
- `project/coordination/TEST_SEAMS.md`;
- `project/coordination/LOSSLESS_CLIENT_IO_ACTUAL_RETURN.md`;
- `project/coordination/LOSSLESS_CLIENT_IO_RETURN_SPEC_REVIEW.md`;
- `project/coordination/LOSSLESS_REPEATED_H128_HANDSHAKE_REVIEW.md`;
- every file under
  `project/coordination/returns/lossless-client-io-pro-b64a980/`;
- current `project/include/`, `project/src/`, `project/tests/`,
  `project/CMakeLists.txt`, and `project/.github/workflows/dcp-rcb.yml`;
- the exact paper PDF/text and every supplied official source named by the
  source-claim ledger.

Treat the prior Pro return as reviewed design evidence, not implementation
authority.  Recheck all cited official lines.  Stop with an exact missing-file
or hash mismatch if a required input is absent; do not reconstruct it from chat
memory.

## Existing architecture and boundaries that must survive

- `DoubleCKKS` already implements the bounded DCP/RCB/Tensor2/Relin2/RS2/
  first-`Mult2` and pair Add/Sub public seams.  Do not modify its algorithm in
  this slice.
- Preserve every existing test, assertion, threshold, target, CTest name,
  command, and ordering.  Current CMake has exactly 57 CTest name/command
  bindings.  Append the new binding as number 58.
- The test-only multiprecision DCRT/secret/CRT fixture may remain an independent
  oracle.  It is not production I/O; its stale binary64 plaintext cache must
  never be read, serialized, or exported through the new public API.
- Build on official public OpenFHE primitives.  Do not copy or reimplement
  OpenFHE encryption, RLWE decryption, key switching, or FHE arithmetic.
- KISS and YAGNI apply.  No generic codec framework, alternate FHE backend,
  serialization system, context/key factory, plugin layer, rotations,
  bootstrapping, multiparty support, or REAL-mode emulation.
- Let unexpected failures propagate.  Do not add broad `try`/`catch`.  A test
  may catch a specific expected exception solely to bind an explicit rejection
  contract.

## Public production surface

Add only the narrow declarations needed under namespace
`openfhe_2023_1788::client_io`, following
`PROPOSED_PUBLIC_INTERFACE.md` unless this task narrows or corrects it:

- `ClientReal` fixed exactly to
  `boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>>`
  (the numeric wrapper, not the backend type alone);
- `ClientComplex { real, imag }` with no binary64 overload;
- gcd-reduced positive `PositiveRationalScale` using `cpp_int`;
- ordered modulus/root basis and immutable context/state receipt values;
- `BoundCiphertext`, exposing only `State()` and a separate
  `CloneForEvaluation()`;
- `HighPrecisionClientIO(context)`;
- `Encrypt(publicKey, values, FreshEncodingSpec)`;
- the operation-specific `BindFirstMult2Rcb(recombined, leftFresh,
  rightFresh)` bridge; and
- `Decrypt(privateKey, boundCiphertext)` returning owned multiprecision slots
  and centered-coefficient diagnostics.

The header and source should normally be
`include/openfhe_2023_1788/high_precision_client_io.h` and
`src/high_precision_client_io.cpp`.  A second small private implementation file
is allowed only if it produces a materially clearer ownership boundary.

Do not expose raw private snapshots, mutable receipts, `Plaintext`, raw
`Poly`, arbitrary caller-provided level/basis/scale transitions, or private
transform helpers.  Slot-value input and output accepts and returns no
`double`, `long double`, or `std::complex<double>` values.  The read-only state
receipt may expose OpenFHE's `double recordedScalingFactor` solely as an
explicitly non-authoritative compatibility observation; it is never a slot
value or the source of exact normalization.

### Required correction 1: fail closed on actual key shape

Before calling an official primitive, validate the actual key object rather
than only its pointer/context/tag:

- the public key has exactly two elements;
- both public-key elements have nonnull, mutually consistent parameter objects,
  Evaluation format, and the exact full ordered Q modulus/root/cyclotomic basis
  required by the bound context;
- the private key contains a valid element with nonnull parameters, Evaluation
  format, the expected full-Q identity from which the official decrypt path may
  take the exact active prefix, and matching context/nonempty tag; and
- all key and ciphertext structural checks occur before an unchecked upstream
  index/dereference.

Reject malformed objects with a stable project-owned diagnostic.  This checks
shape, basis, context and tag; it must not claim to prove the secret/public-key
RLWE relation.

### Required correction 2: precise clone/parameter ownership

OpenFHE `DCRTPoly` clones may share mutable `Params`.  Therefore do not claim
arbitrary transitive clone isolation.  V1 supports:

- separate coefficient element vectors and scalar ciphertext state;
- a present, empty ciphertext metadata map so no mutable metadata value is
  shared; and
- an explicit shared-context/shared-basis nonmutation contract.

Snapshot the complete public structural basis identity in the receipt and
revalidate it at every operation boundary that returns or uses a ciphertext:
`CloneForEvaluation`, `BindFirstMult2Rcb`, and `Decrypt`.  The nonthrowing
`State()` accessor only returns the already immutable value receipt and does
not perform live revalidation.  If shared parameters drift, the next operation
fails closed.  The test must prove coefficient/scalar/map isolation and
separately prove basis-drift detection; do not add a deep-copy
context/parameter subsystem.

## Frozen v1 profile and state

The test constructs and validates the actual context returned by the factory,
not only the locally requested parameter object.  Explicitly set every field
for which the pinned CKKS `CCParams` exposes a supported setter.  OpenFHE 1.5.0
deliberately rejects CKKS `SetEncryptionTechnique` and
`SetMultiplicationTechnique`; do not call those disabled setters.  Instead,
require the actual factory-returned CKKS profile to read back `STANDARD` and
`HPS` and fail closed if it does not.  Freeze:

```text
OpenFHE pin              df495ba2e91739a6dc8f1de254fc5a41155ce504
scheme ID                CKKSRNS_SCHEME
N / M                    64 / 128
batch S / gap            16 / 2
multiplicative depth     7
scaling / first bits     50 / 55
key switching            HYBRID
scaling                  FIXEDMANUAL
digit size               0
encryption readback      STANDARD
multiplication readback  HPS
PRE                      NOT_SET
CKKS data                COMPLEX
execution                EXEC_EVALUATION
decrypt noise            FIXED_NOISE_DECRYPT
noise scale              1
secret distribution      UNIFORM_TERNARY
security                 HEStd_NotSet (diagnostic only)
max relin secret degree  2
fresh exact scale        2^100 / 1
fresh state              full Q, level 0, degree 2, two components
projection               OpenFhePackedStride
```

Require enabled features `PKE | KEYSWITCH | LEVELEDSHE`; record the complete
enabled mask and reject later drift.  The client must never call `Enable` or
mutate the context.  Verify actual Q moduli/roots/order, scheme ID, P and QP,
HYBRID partition profile, paramsPK=Q under PRE `NOT_SET`, encoding parameters,
ring dimension, data type, modes, and key profile before exposing a receipt.

The production code is initially supported only when the selected OpenFHE
build's `lbcrypto::BigInteger` is compile-time proven to be the inspected
`MATHBACKEND=4` dynamic implementation.  Use the exact official type aliases;
do not guess a type spelling or assert portability to other backends.

## Numerical and cryptographic contract

### Exact input construction

Use the 16 frozen complex vectors and independently frozen products from
`PROPOSED_FIRST_TDD_CONTRACT.md` byte-for-byte.  Construct all values directly
from decimal text or exact dyadic arithmetic.  No binary64 intermediate is
permitted.  Independently recompute every expected product and the sub-binary64
adjacent deltas before invoking production code.

`Encrypt` must:

1. build instance-owned multiprecision powers-of-five transform data without
   reading OpenFHE's binary64 root cache as numerical input;
2. execute the special inverse transform independently at 160 and 220 decimal
   digits;
3. multiply by the exact total scale `2^100` once and apply the paper's
   tie-down integer rounding;
4. enforce the proposal's supported-range and ambiguity checks, including
   `u160 < 2^-410` and `h > max(16*d, 2^-400)`;
5. require `2*abs(coefficient) < Q_full`;
6. bridge exact signed integers through decimal strings into official
   `BigInteger`, construct coefficient `Poly` then ordered `DCRTPoly`;
7. call public scheme-level `Encrypt(element, publicKey)`; and
8. set and then revalidate only the high-level CKKS metadata required by the
   retained evaluator: exact context/tag, packed encoding, slots 16, level 0,
   degree 2, recorded factor `base^2`, integer factor 1, two Evaluation-format
   components, exact full Q basis, and a present empty metadata map.

It must return no `Plaintext`, packed-value cache, raw mutable ciphertext, or
test fixture object.

### First evaluator result binding

`BindFirstMult2Rcb` is a deliberately narrow bridge for the immediate
same-process result of the exact public evaluator call chain.  It verifies
state but does not claim cryptographic parent-lineage attestation.  Both parent
receipts must be fresh and equal in context/profile/tag/slots/basis/scale.

Derive actual `qDiv` and `qL` from the parents' final and penultimate ordered Q
entries.  Require the output basis to be the exact modulus/root prefix after
those two entries are removed, not merely the same length.  Require matching
context/tag, level 2, degree 2, two Evaluation-format components, packed
COMPLEX encoding, 16 slots, integer factor 1, and present empty metadata map.

Derive the one authoritative receipt scale for this narrow bridge as an exact
reduced rational:

```text
S1 = Sleft * Sright / (qDiv*qL)
```

For the frozen diagnostic this is
`2^200 / 1267650600226646386227681786497`.  Recompute the physical OpenFHE
recorded factor in the exact current operation order from the actual returned
parameters and require exact finite positive `double` equality.  Do not accept
a caller-supplied transition or infer logical scale from `double`, level, or
nominal bit sizes.

This binder is not the repeated-operation architecture.  Do not generalize it
to second/eighth `Mult2`, family re-entry, arbitrary scale, or h=128.  The
parallel repeated implementation will become the unique exact-state issuer at
that integration boundary; record this explicit replacement/consumption seam
in `DESIGN_DECISION.md`.

### High-precision output

`Decrypt` must validate the receipt and actual private-key shape first, then use
public scheme-level `Decrypt(ciphertext, privateKey, Poly*)`.  Never call
`DecryptCore`, high-level `Plaintext` Decode, or disable/change decryption
protection.  Check `DecryptResult.isValid` before reading the polynomial.

Require Coefficient format and exact cyclotomic order/ring dimension/composite
modulus.  Convert every exact residue through the official decimal string
bridge, center in `(-Q/2,Q/2]` according to the frozen rule, read only packed
stride coefficients `gap*j` and `gap*j+N/2`, normalize by the receipt's exact
rational, and run independent 160/220-digit forward transforms.  Require every
real/imaginary cross-precision disagreement `<= 2^-120` before rounding once to
`ClientReal`.  Return exactly 16 owned values plus the actual composite modulus,
maximum centered absolute coefficient, centered headroom, and maximum
cross-precision disagreement.  Do not promote observed headroom into an
all-key/no-wrap theorem.

## Required ordered vertical RED/GREEN series

Return four independently applicable patches in this order.  Each pair is one
behavioral vertical slice; do not put all negative-state coverage in the first
RED or implement later pairs speculatively:

1. `0001-red-lossless-client-io-first-mult2-tracer.patch` adds only the positive
   end-to-end semantic tracer, independent arithmetic/transform oracles,
   and the directly reviewed malformed public/private key shape cases needed
   to prove official primitive calls are safe.  This slice also binds the
   positive-rational value semantics and decoded-value lifetime needed by that
   tracer.  It adds the
   CMake/CTest registration, exact branch trigger
   `codex/lossless-io-implementation-01`, and the same focused test step in both
   Linux and Windows jobs.  It contains no production implementation.  The
   honest expected RED is a missing public header/type/API compile failure.
2. `0002-green-lossless-client-io-first-mult2-tracer.patch` applies only after
   patch 1 and adds the smallest public header/source and build integration
   needed for that unchanged tracer and malformed-key cases to pass.  It
   includes the pre-call fail-closed checks required by correction 1.
3. `0003-red-lossless-client-io-shared-params-drift.patch` applies after patch 2
   and adds only the supported clone-isolation assertions plus one separate,
   disposable-fixture shared-`Params` drift rejection required by correction 2.
4. `0004-green-lossless-client-io-shared-params-drift.patch` applies after patch
   3 and adds only the immutable structural snapshot/revalidation needed for
   those unchanged cases.

Do not alter a RED expectation in its GREEN, modify `double_ckks.cpp`, or edit
existing tests.  Later rejection/state/ownership matrices remain ordered future
RED/GREEN work in `NEXT_GATES.md`; they are not silently folded into these four
patches.

Name the new executable `precision_client_io_first_mult2_contract_test` and the
new CTest exactly `precision_client_io_first_mult2_contract`.  Append it after
the existing 57 name/command pairs.  Add its warning flags under `/W4 /WX` only
for MSVC and `-Wall -Wextra -Wpedantic -Werror` only for non-MSVC.  Preserve all
five explicit API-target builds and existing focused steps.

## Test obligations

The new test executable must use only the public client/evaluator seams.  The
two ordered pairs above must cumulatively prove all of the following:

- positive-rational rejection and canonical gcd reduction;
- malformed public and private key shape/basis/format rejection before official
  primitive access;
- exact frozen 16-slot input and product arithmetic, including adjacent
  sub-binary64 input/product deltas;
- public fresh Encrypt/Decrypt and an independent test-only secret/CRT/Horner
  observation each agree with the frozen input at absolute complex error
  `<= 2^-80`;
- the immediate DCP→Mult2→RCB result binds and both public Decrypt and an
  independent stride-projected oracle agree with all 16 exact products at
  absolute complex error `<= 2^-80`;
- both public and independent output delta errors are `<= 2^-80`;
- production forward-transform output agrees with the separately authored
  direct stride-projected Horner result componentwise within `2^-120`;
- literal `1`, `X^(N/2)`, `X^2`, and the off-stride `X` projection witness catch
  transform/order or projection cancellation;
- exact actual context/profile/Q-root/P/QP/HYBRID/PK-key state, fresh/output
  receipts, `qDiv/qL`, exact rational scale, recorded factor, lifecycle/state,
  slots/format/components/degree/encoding and metadata policy;
- on ordinary positive and malformed-key paths, input vectors, keys,
  contexts/tables and client receipts remain unchanged;
- evaluator clones cannot mutate receipt coefficients/scalars/maps, while a
  deliberate shared-parameter drift is detected rather than falsely called
  isolated.  Run that drift case last in its own disposable context/key/
  ciphertext fixture; require receipt use to fail closed and do not include the
  deliberately mutated context in an `unchanged` assertion; and
- returned decoded values remain valid after raw local Poly/coefficient buffers
  are destroyed.

Use one freshly generated matching keypair and evaluation key for the frozen
positive diagnostic.  The final disposable shared-`Params` drift fixture may
use one additional isolated matching keypair; it needs an evaluation key only
if that fixture actually reaches evaluator arithmetic.  This explicit isolated
fixture is not a profile change requiring user confirmation.  Any other key
count change must be reported before claiming success.  Test catches must be
narrow, diagnostic-specific and verify the rest of observable state is
unchanged.

Print one parseable success record containing the test name, exact source/pin,
N/S/gap/profile, actual `qDiv/qL`, exact scale numerator/denominator, maximum
fresh/product slot error, input/product delta error, cross-precision error and
centered headroom.  Label scope `low-N-first-operation-production-io-diagnostic`.

## Commands and evidence boundaries

Codex, not Pro, will execute the authoritative hosted gates against pristine
OpenFHE on Linux and Windows:

```text
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
cmake --build build --target mult2_api_contract_test --parallel 2
cmake --build build --target add_api_contract_test --parallel 2
cmake --build build --target sub_api_contract_test --parallel 2
ctest --test-dir build --verbose --output-on-failure -R '^precision_client_io_first_mult2_contract$'
ctest --test-dir build --show-only=json-v1
ctest --test-dir build --verbose --output-on-failure
```

Hosted GREEN acceptance is warning-clean builds, focused 1/1 and full 58/58 on
both jobs, with all original 57 exact bindings preserved.  Codex will apply and
push each RED before its matching GREEN and retain the actual failing run.  Pro
must mark compilation/runtime/CI `NOT RUN` unless it genuinely executed them;
the existence of a later returned GREEN patch is not itself a RED observation.

You may perform source inspection, exact arithmetic, patch replay and bounded
static checks.  Do not use credentials/accounts, send messages, push/merge,
change CI state, or claim Codex/GitHub/Windows runs as your own.  Mark compile,
runtime and hosted commands `NOT RUN` unless you actually ran them in an
authorized environment and provide exact raw evidence.  No Mac project/OpenFHE
compilation or cryptographic execution is part of this handoff.

## Forbidden success substitutions

The following are not acceptable evidence for this task:

- ordinary `MakeCKKSPackedPlaintext` or binary64 cached values;
- a codec roundtrip as the only oracle;
- test code calling production private helpers;
- metadata relabeling without exact coefficient construction/validation;
- plaintext multiplication or intermediate decryption/re-encryption;
- arbitrary caller-supplied scale/basis/state;
- changing precision thresholds after seeing a failure;
- using the first-Mult2 binder as repeated-family support;
- h=128, N32768, eight-operation, 1000-run, performance, security or full-paper
  claims from this N64 test; or
- a generic plan, TODO, source-only sketch, or deliberately broken RED.

## Required return archive

Return one downloadable ZIP and matching SHA-256 sidecar containing:

- `README.md` with exact source/pin, actual scope and honest status;
- `DESIGN_DECISION.md` resolving ownership, corrected key validation,
  shared-Params drift contract, numerical algorithms, first binder, and the
  later repeated-receipt handoff;
- all four ordered patches above;
- `complete/project/...` with every complete changed/new file after GREEN;
- `contracts/PROPOSED_FIRST_TDD_CONTRACT.md`, byte-identical to the supplied
  Markdown that contains the frozen input/product vectors; an optional derived
  machine-readable vector file must name that source and pass an independently
  documented exact-arithmetic check;
- `SOURCE_CLAIM_TEST_LEDGER.md` with exact official/project line citations;
- `EXPECTED_CTEST_BINDINGS.tsv` with all 58 exact name/command pairs in order;
- `RED_GREEN_EXECUTION_LEDGER.md`, separating static, replay, compile, runtime,
  hosted and not-run states;
- `NEXT_GATES.md` covering repeated receipt integration, malformed/state closure,
  full-slot codec, h=128, paper ordered Q/P, eight operations, then N32768/
  1000-run precision/performance; and
- a closed manifest with each payload path, byte size, SHA-256, origin and
  source commit.  The manifest must explicitly self-exclude itself and its own
  hash sidecar; the outer ZIP and outer sidecar are necessarily outside that
  inner payload manifest.

Verify in a fresh scratch copy that patch 1 applies to exact `4ccc8fd`, every
later patch applies only after its predecessor, replayed final files equal
`complete/project` byte-for-byte, and no unexpected file changes exist.
Exclude `.git`, dependencies, builds,
caches, databases, runtime/browser state, credentials and unrelated files.

If a source-level blocker prevents a defensible GREEN, return the smallest exact
counterexample, the narrow executable public-seam probe that would decide it,
and an honest blocked ledger.  Do not replace the requested implementation with
another design-only answer.
