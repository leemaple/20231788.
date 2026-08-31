# ChatGPT Pro bounded recovery — DCP/RCB vertical slice

## Background and exact objective

Continue the saved clean-room OpenFHE 1.5.0 task after two response-delivery timeouts, but deliver only the first independently testable vertical slice. Write from scratch the minimal C++17 project files for the paper 2023/1788 `t=2` decomposition and recombination operations:

- `DCP(ciphertext) -> (high, low)` with centered last-prime division;
- `RCB(high, low) -> q_div * high + low` on the retained prefix basis;
- only the validation, immutable pair metadata, and build/test plumbing required by those two operations.

Do not implement or sketch Tensor2, Relin2, RS2, Mult2, refresh, serialization, `t>2`, compatibility layers, or performance work in this response.

The destination is the public repository `https://github.com/leemaple/20231788.`; the trailing period is part of the repository name. The current clean-room commit and every supplied input identity are in the package manifest. No implementation has been accepted yet.

## Authoritative inputs and provenance boundary

Use only:

1. the supplied paper PDF/text;
2. official pristine OpenFHE 1.5.0 at commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
3. the supplied clean-room project documents and this task.

The repository's previous implementation and all related local code are known wrong and quarantined. Do not search for, request, infer, reproduce, or adapt them. Do not assume access to the user's filesystem, private repositories, earlier chats, browser state, credentials, build machines, or any file not in this package.

## Current architecture and non-breakable boundaries

- One project-owned `openfhe_2023_1788::DoubleCKKS` instance binds one exact `CryptoContext<DCRTPoly>`.
- `CiphertextPair` has private construction/mutation and exposes read-only high/low ciphertexts plus the minimum facts callers/tests need: exact context identity, divisor `q_div`, ordered active moduli, level, paper-scale descriptor, recorded OpenFHE scaling factor, noise-scale degree, key tag, component count, format, and lifecycle.
- The first accepted input is a fresh two-component evaluation-format level-0 ciphertext on exact ordered basis `[q0, ..., q_l, q_div]` under `FIXEDMANUAL` scaling. DCP returns exact prefix `[q0, ..., q_l]` and lifecycle `ReadyForFirstMult`.
- RCB accepts a valid two-component pair and returns an ordinary ciphertext on that exact prefix.
- Public methods must validate context pointer, ordered moduli, level, component count, format, key tag, and scale metadata before any raw tower access.
- Keep upstream OpenFHE pristine. Prefer its public APIs. If a required primitive is unavailable, prove the exact gap with source citations and isolate the smallest proposed adapter; do not silently modify upstream.
- KISS, YAGNI, fail fast, no catch-all `try`/`catch`, no hidden fallback or metadata-only correction.

The repository's proposed names are `include/openfhe_2023_1788/double_ckks.h`, `src/double_ckks.cpp`, and `tests/dcp_rcb_test.cpp`; change them only when a concrete OpenFHE/CMake constraint requires it and explain that in the delivered design note.

## Paper and OpenFHE facts to verify, not merely repeat

For each ciphertext polynomial coefficient `x` in centered representation and odd last prime `q_div`, DCP must produce the exact centered remainder `r` in `(-q_div/2, q_div/2]` and quotient `h = (x-r)/q_div`, so `x = q_div*h + r`. RCB computes `q_div*high + low` modulo the retained composite modulus.

The clean-room review found these candidate upstream primitives:

- `DCRTPolyImpl::DropLastElementAndScale` in `src/core/include/lattice/hal/default/dcrtpoly-impl.h`;
- centered modulus switching in `NativeVector::SwitchModulus` in `src/core/lib/math/hal/intnat/mubintvecnat.cpp`;
- exact native-integer ciphertext multiplication through `CryptoContextImpl::EvalMultNoCheck(ciphertext, NativeInteger)` in `src/pke/include/cryptocontext.h`.

Independently verify their actual signatures, preconditions, centered-rounding semantics, parameter vectors, and metadata behavior in the supplied source. Do not call an internal helper just because its name resembles the paper operation.

Keep two scale views explicit:

- paper/logical DCP scale: approximately `2^(2p) / q_div`;
- OpenFHE FIXEDMANUAL recorded metadata after DCP: retains the original recorded `2^(2p)` scaling factor and noise-scale degree 2 while the level advances to 1.

The difference is intentional and must not be “corrected” by writing the actual prime into OpenFHE metadata.

## Required deliverable format

Return one downloadable ZIP, not a long prose answer. Put all detail inside the ZIP and keep the chat response to a short file manifest plus any blocking fact.

The ZIP must contain:

1. `0001-red-tests.patch`: build files and independent tests only, intentionally failing because the production DCP/RCB interface or behavior is absent. It must apply cleanly to the packaged clean-room commit.
2. `0002-dcp-rcb-implementation.patch`: the smallest implementation that makes the same tests pass; it must apply after patch 0001.
3. `DESIGN-DCP-RCB.md`: equations, exact OpenFHE API/source mapping with paths and line/function names, invariants, ownership, basis/level/format/scale/component transitions, and open risks.
4. `REVIEW-DCP-RCB.md`: clearly separated observed, inferred, and unverified claims; list every build/test command actually run and its result, or state explicitly that none were run.
5. Any complete new source files needed to audit the patches, if the patch files do not already contain them.

Do not paste hundreds of lines of code into chat. Do not spend the response on the remaining multiplication pipeline.

## Independent tests required in patch 0001

Use a test-only `boost::multiprecision::cpp_int` CRT oracle that shares no production tower-copy, rounding, DCP, or metadata helper. It must independently:

- reconstruct coefficients from ordered residues by textbook CRT;
- center modulo the full composite modulus;
- calculate centered remainder and exact quotient using integer arithmetic;
- reduce signed expected coefficients independently into each retained tower.

Cover deterministic values `0`, `+/-1`, `+/-(q_div-1)/2`, `+/-(q_div+1)/2`, one step around these boundaries, and values near `+/-Q/2`. Because `q_div` is odd, do not invent an exact half-prime tie case.

Tests must check every component and retained tower for DCP, exact coefficient identity for RCB, last-tower removal, ordered basis, two RLWE components, evaluation format, exact context/key tag, level transition, paper/logical scale descriptor, recorded scaling factor, and noise-scale degree. Negative tests must reject wrong level, wrong last modulus/order, coefficient format, wrong component count, mismatched context/key tag, and malformed pair state before unsafe access.

The red patch must be genuinely red without weakening assertions. A compile failure from the absent first public interface is acceptable for this first slice. Do not use production code to generate expected values.

## Mandatory downstream verification

Codex—not ChatGPT Pro—will preserve the first red output, apply the implementation patch, and run warning-enabled builds/tests on GitHub Actions and/or Windows. Therefore:

- make the commands deterministic and document exact CMake/CTest invocations;
- pin OpenFHE to the exact commit above;
- do not require an already installed local OpenFHE;
- do not claim a build or test passed unless you actually executed it;
- never weaken a threshold or alter the oracle between red and green.

## Prohibited actions and claims

Do not push, merge, open a PR, run GitHub Actions, access credentials, change repository settings, inspect old code, silently patch OpenFHE, add broad exception handling, implement unrequested features, or claim precision/security/performance/build/test evidence not observed. Do not return another monolithic full-project solution.

## Acceptance criteria

- Both patches apply in order to the packaged clean-room commit without unrelated changes.
- Patch 0001 supplies a meaningful independent-oracle red test for the agreed public DCP/RCB seam.
- Patch 0002 is minimal, preserves upstream OpenFHE, validates before raw access, and makes no metadata-only mathematical correction.
- The design accounts exactly for ordered bases, centered rounding, format, component count, context/key identity, level, and both paper/OpenFHE scale views.
- Every uncertainty is explicit and convertible into a test; unexecuted work is not reported as complete.
