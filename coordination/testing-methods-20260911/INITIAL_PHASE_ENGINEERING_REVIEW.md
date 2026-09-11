# `initial_phase_exact_contract` engineering review

## Scope and result

Static first-pass review only; no build, test, transform, sampling, encryption, CI,
browser, or Git operation was performed. I reviewed the returned
`pro/tests/initial_phase_exact_contract_test.cpp` as the 20,325-byte object with
SHA-256
`f9c6ba30e8d9b0d19c0e4d0baccd9621e18696f6f61e7801cdb91b8940490af3`, the
18-line `pro/CMakeLists.append.txt` (SHA-256
`8c049db6c2db1709856e18e68b228204e5904d4a1c2b5ba8c8a5e00bbadf5a7e`), the
existing N=256/H=128 fixture and public adapter, and official OpenFHE sources
from the verified packet. The packet receipt fixes project source commit
`33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1` and official dependency pin
`df495ba2e91739a6dc8f1de254fc5a41155ce504` (`PACKET_RECEIPT.json`). I did not
use Pro's diagnosis, test-plan, mutation-plan, or conclusions for this judgment.

**Engineering verdict: conditionally acceptable as one isolated, opt-in Linux
diagnostic.** I found no static C++ or CMake blocker for the pinned
native64/backend4 toolchain. The conditions are: copy the reviewed object to the
`tests/` path named by the append, explicitly build its excluded target, run only
the one registered `--controls` CTest in a fresh process, preserve the hash and
fixed-base provenance outside program output, and disable crash/core-dump
artifacts if the execution environment requires a strict no-secret-on-disk
guarantee. This is not an S100 reproduction or acceptance test.

## C++ and public-API findings

1. **The call reaches official PKE through virtual public scheme interfaces.**
   The fresh operation is exactly one call to
   `cc->GetScheme()->Encrypt(input,key.publicKey)` (test lines 389-390). Official
   `SchemeBase::Encrypt(const Element&, PublicKey)` is virtual and delegates to
   `m_PKE->Encrypt` (packet member
   `official/src/pke/include/schemebase/base-scheme.h:210-212`). Official
   `PKEBase::Encrypt` obtains one public-key zero encryption, adds the element to
   component zero, and constructs the ciphertext
   (`official/src/pke/lib/schemebase/base-pke.cpp:112-119`); the zero core samples
   `v,e0,e1` and forms both components at lines 144-181. The exact decryption
   check similarly calls public virtual `SchemeBase::Decrypt` (test lines
   335-343; official base-scheme lines 221-224) and dispatches to CKKS-RNS
   decryption (`official/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:70-95`). No
   interception, subclass replacement, or direct call to `EncryptZeroCore` is in
   the test.

2. **This is the public scheme-layer raw-element seam, not the higher-level
   plaintext/context seam.** `CryptoContext::Encrypt` accepts a `Plaintext`,
   validates the key, and propagates encoding metadata
   (`official/src/pke/include/cryptocontext.h:1250-1266`). The test deliberately
   supplies a `DCRTPoly`, so it invokes the lower public scheme overload and
   performs its own context/key identity and element-envelope checks (test lines
   197-220 and 379-390). Consequently a PASS supports official initial PKE ring
   arithmetic for this raw integer polynomial and fixed profile; it does not
   establish high-precision encoder behavior, `CryptoContext` validation, or
   plaintext metadata propagation.

3. **The fixture is consistent with the already accepted N=256/H=128
   construction.** Test context construction and profile checks are at lines
   86-180. The existing fixture uses the same public CKKS parameter construction
   at `tests/paper_h128_client_keypair_contract_test.cpp:55-105` and the same
   independently fixed Q/P modulus-root order at lines 33-41. The public adapter
   contract explicitly returns fresh full-Q evaluation keys without changing the
   context or evaluation caches
   (`include/openfhe_2023_1788/paper_h128_client_keypair.h:8-17`); its
   implementation validates the context, samples the H=128 secret, creates the
   official public-key zero encryption, and returns fresh key objects
   (`src/paper_h128_client_keypair.cpp:163-217`). This new test adds no evaluation
   keys or evaluator operation (test lines 378-390).

4. **Warning-clean feasibility is credible but still execution-gated.** The
   test's includes and local declarations have no evident unused symbol,
   sign/width conversion, or extension warning under its requested C++17
   `-Wall -Wextra -Wpedantic -Werror` flags. Its native64/backend4/HAVE_INT128 and
   60-bit compile guards (test lines 17-26) match both the production adapter
   (`src/paper_h128_client_keypair.cpp:16-19`) and the accepted fixture
   (`tests/paper_h128_client_keypair_contract_test.cpp:19-22`). This is a
   portability ceiling: the MSVC flag branch in the append does not make the
   diagnostic portable to an OpenFHE configuration lacking `HAVE_INT128`.
   Static review cannot certify warning cleanliness or that the 180-second
   timeout is sufficient on the target runner.

## Randomness and one-sample isolation

The registered `--controls` path creates one context, one key pair, and one fresh
public encryption, then reuses that ciphertext (test lines 373-398). The exact
source-reviewed random operations are:

- adapter setup samples one H=128 ternary secret, constructs a random key tag,
  and calls one private-key zero encryption for the public key
  (`src/paper_h128_client_keypair.cpp:193-210`); official key-ID construction
  draws four 32-bit words
  (`official/src/pke/include/key/privatekey.h:55-65,83`), and private-key zero
  encryption samples uniform `a` plus Gaussian `e`
  (`official/src/pke/lib/schemebase/base-pke.cpp:124-139`);
- the single public encryption samples ordinary ternary `v` and Gaussian
  `e0,e1` (`official/src/pke/lib/schemebase/base-pke.cpp:144-181`);
- the two mutations, the independent integer oracle, and the repeated baseline
  check contain no sampler call. Decryption also draws no flooding noise under
  the asserted `FIXED_NOISE_DECRYPT` mode; the official CKKS implementation only
  samples there for `NOISE_FLOODING_DECRYPT` plus evaluation mode
  (`official/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:70-82`).

Thus “one public encryption” is exact, but it must not be read as “one random
draw”: key construction necessarily contains the enumerated setup draws. Running
the unregistered `--fault-*` modes would create new independent key/encryption
samples, so they must not be added to this one-sample gate.

## Mutation controls and failure classification

The two controls are effective for the two named predicates:

- `one-tower-plus-one` changes every evaluation lane of ciphertext component
  zero in only tower zero (test lines 345-360, 394-396), so the direct inverse
  must expose a +1 constant coefficient in one tower and the coherence check at
  line 331 must classify it as `FRESH_CRT_COHERENCE`;
- `all-towers-plus-30031` applies the same constant in all three towers. Given a
  baseline already accepted inside `[-15015,15015]`, the coefficient-zero shift
  is necessarily outside that interval while remaining coherent, so line 332
  must classify it as `FRESH_SUPPORT_BOUND`.

`ExpectRejected` accepts only the exact expected `NumericFailure`; a wrong class,
contract failure, third-party exception, crash, timeout, or surviving mutation
cannot pass (test lines 362-371). The direct forward anchor at lines 258-274 and
the inverse oracle at lines 223-257 make the constant-in-evaluation-space premise
non-tautological with respect to OpenFHE's inverse transform. The controls still
only demonstrate that this checker rejects these two synthetic perturbations.
They do not independently validate the mathematical value of the 39/15015
bounds, decrypt-path validation, encoder behavior, or historical S100
attribution. The mutation magnitude is derived from the same bound constant as
the predicate, which is appropriate for a kill test but not an independent proof
of that bound.

Failure output is deliberately coarse and non-secret: numeric failures print one
of two fixed classes, contract failures print fixed source labels, and all other
exceptions suppress detail (test lines 46-61 and 412-422). `baseline=PASS` is
printed before controls (line 393), but the authoritative whole-slice marker
`slice=PASS` is emitted only after both controls and original-object rechecks
(lines 394-409); automation must use exit status/CTest result and the final
marker, not grep the earlier baseline line alone.

## Original-object mutation and secret/artifact handling

The negative controls clone the ciphertext, copy its element vector, mutate the
copy, and install it only on the clone (test lines 345-360). Official
`CiphertextImpl::Clone` creates a new ciphertext and copies `m_elements`
(`official/src/pke/include/ciphertext.h:390-405`); `DCRTPoly` copy construction
and assignment copy the tower vector
(`official/src/core/include/lattice/hal/default/dcrtpoly.h:74-81`). After both
controls, the test recomputes the original residual and public decryption (test
lines 397-398). This is a meaningful coefficient-level non-mutation check.

Limit: it is not a byte-for-byte snapshot of ciphertext metadata, keys, input, or
context. Static official-source review supports the narrower behavior: public
encryption copies both plaintext and public-key elements before arithmetic
(`base-pke.cpp:112-119,155-179`), and CKKS decryption copies the phase into local
`b` before changing format (`ckksrns-pke.cpp:70-93`). Broader key/context
immutability remains covered by the existing fixture, notably
`tests/paper_h128_client_keypair_contract_test.cpp:259-290`.

The test has no file, serialization, seed, ciphertext, key-tag, secret, or
coefficient output. Its stdout/stderr values at lines 371, 393, and 408-422 are
fixed public labels, profile constants, and source identifiers. The secret and
fresh residual are retained only in process memory (`s`, `f`, and key objects at
lines 379-398) and are not explicitly zeroized. Ordinary CTest logs therefore
contain no secret material; however, a native crash/core dump could capture
process memory. Strict execution must disable or securely contain core/crash
dumps and avoid memory-dump instrumentation. This is an execution condition,
not a source-level leak found in the test.

## Exact CMake/CTest scope and provenance limits

The append is properly opt-in: the option defaults OFF; only when enabled does it
declare one excluded executable and one CTest named
`initial_phase_exact_contract`, whose sole command is the reviewed binary with
`--controls` (`pro/CMakeLists.append.txt:2-17`). No fault-mode test, repeated
sample, output file, production threshold, or existing test registration is
added. Existing `enable_testing()` precedes the append (`CMakeLists.txt:194`), so
registration placement is valid. Because the target is `EXCLUDE_FROM_ALL`, an
option-enabled default build is insufficient: the runner must explicitly build
`initial_phase_exact_contract_test` before selecting exactly the one CTest.

The append expects the source at `tests/initial_phase_exact_contract_test.cpp`
(append lines 5-6); the reviewed return resides under `pro/tests/`, so integration
must preserve the reviewed bytes when staging that path. The compile definition
uses existing `LOSSLESS_IO_SOURCE_COMMIT` (append lines 8-9), which the current
project obtains from `git rev-parse HEAD` at configure time
(`CMakeLists.txt:111-120`). Therefore the program's printed `source_base=` field
(test lines 408-409) is the configured checkout HEAD, not intrinsically proof of
fixed base `33722b9...`. The runner's immutable commit/tag, this file hash, packet
receipt, and official dependency pin remain the authoritative provenance tuple.

## Acceptance limits

A green one-shot result would establish, for the exact N=256/H=128/Q profile and
one realized key/encryption sample, that: the fixed adapter output has the
expected public structure; the official scheme-layer public encryption's phase
equals the supplied integer polynomial plus a CRT-coherent residual within the
adopted bound; official raw-Poly decryption agrees; and both checker predicates
reject their targeted mutations without changing the original ciphertext.

It would **not** establish high-precision slot encoding, DCP/Tensor2/Relin2/RS2 or
Mult2 correctness, general NTT semantics beyond the explicitly anchored fixed
roots/order, statistical distribution quality, security, repeatability across
samples/platforms, or any cause/fix for original S100. Original S100 remains
unchanged, as the test itself states at lines 1 and 408-409.
