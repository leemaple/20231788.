# Repeated-Mult2 probe: independent standards/replay review

Baseline `774fe2dcfca47d7a08cab9c04b29c430e354cf9f`; live HEAD
`8dcadb4544ed567c6de3a7d3825857f89470b29e`, branch `codex/repeated-mult2-01`.
Returned ZIP SHA256 `bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2`.
Below, `C`, `P`, `T` mean returned `complete/project/CMakeLists.txt`,
`tests/repeated_mult2_basis_family_probe.cpp`, and
`tests/repeated_mult2_second_contract_test.cpp` respectively.

## Result: REQUEST_CHANGES (static, not executed)

- **P1 — compiler-incompatible flags, C:214-216,226-228.** Both new targets
  unconditionally receive MSVC `/W4 /WX` AND GCC warning flags. GCC/MinGW
  receives inappropriate slash arguments; MSVC receives inappropriate GCC
  options. Restore the existing `if(MSVC)/else` selection for both targets.
  Verify each hosted compiler's generated command and warning-clean build
  after authorization; replay success is not compilation evidence.

- **P2 — incomplete CKKS context identity, P:117-118.**
  `GetContext(params, scheme)` defaults to `INVALID_SCHEME`
  (`official-full/.../cryptocontextfactory.h:74-76`); factory.cpp:66-74 stores
  that ID in new contexts. The standard CKKS generator explicitly sets
  `CKKSRNS_SCHEME` at `gen-cryptocontext-ckksrns-internal.h:145-146`.
  This probe never checks it, and relinearization can avoid the
  `VerifyCKKSScheme` check used by public CKKS encoding. Supply the scheme ID
  during construction and assert the returned context's ID, without mutating
  an already interned shared context. Verify this invariant in the authorized probe.

- **P2 — misleading certificate identity, T:43,1212-1216.** The new boundary
  executable still prints `precision_first_mult2_high_precision_contract`.
  Use a distinct identifier so retained evidence cannot conflate two tests.
  **Possible duplication smell:** its 59,250 bytes reproduce the first-Mult2
  oracle except a header and one rejection check (:1062-1077). Oracle
  independence from production does not require duplicating another test's
  entire implementation. Prefer a small boundary case using existing fixtures;
  keep the original precision regression unchanged. This is a KISS recommendation,
  not permission to refactor before seam confirmation.

## Observed proof and limits

Both patches replayed in order using isolated index AND object storage at
`/private/tmp/repeated-mult2-independent-replay.QMb7X1/`.
`stage1/` and `final/` contain actual replayed files; all three final files
equal returned complete bytes. Only CMake and two added tests change;
all old source/header/tests/workflow and all 55 CTest name/COMMAND pairs
remain intact and ordered (57 total).

Live index SHA256 before/after:
`c4d55264267c03985cd6e57ea4c7102b407cc91ca00e6baa5685814b07ec3529`.
Live active files remain unchanged. Constructor/precompute/key/ciphertext
symbols were inspected in the hash-verified original pristine-reference packet;
symbol presence does not establish compilation. No additional definite type
error is asserted; the packet lacks `ciphertext-fwd.h` for final alias checking.
The narrow rejection catch and executable-boundary failure catch do not suppress
unexpected success. Package Python was inspected, not executed.

Workflow/engineering constraints governed this audit. No build, crypto, CI,
live source/index mutation, or Git commit/push occurred. New seam remains
unconfirmed; only this note and dedicated replay artifacts were written.
