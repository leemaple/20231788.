# Independent RED source review

## Scope and identity

This is a source-first review of the unapplied RED test slice at engineering worktree HEAD `2745ab44f185b9743d670f8acdf668f69a061980`, against `coordination/precision116-pro-handoff-01/TASK.md` SHA-256 `e55068b18b1bdf7e3714d085598d80ec62ff6ca558e90a225c018709f8e4f26f`. I did not inspect the return author's verdict, checker, GREEN patch, or GREEN complete files before reaching this review. I did not build, compile, run cryptography, run CTest, or dispatch CI.

Reviewed RED artifacts:

- `coordination/precision116-pro-return-01/pro/RED.patch`: SHA-256 `293a9c8c35b7756c0fe746595d6c19dc7d4c56640a41abebbd010812ac9b8612`.
- `coordination/precision116-pro-return-01/pro/RED/CMakeLists.txt`: SHA-256 `8c9de16cdd4945c6450493ba25defd8bf7365777a2b3e301ae850a8c2458a7d8`.
- `coordination/precision116-pro-return-01/pro/RED/tests/paper_full_eight_square_contract_test.cpp`: SHA-256 `d3a852d1c2a2ef7db38b2303f52afffd0527ff886ed4655efbbb3ad5c0f4bd90`.
- `coordination/precision116-pro-return-01/pro/RED/tests/experimental_precision116_profile_seam.h`: SHA-256 `a9e1ab95b5d0e85b8ca635ec0aae7fe5b927a9972046f2fc1f995067f1f86fa5`.

`git apply --check` succeeded without changing the worktree. The patch touches exactly the three permitted paths with `8 + 419 + 10` added lines and no deletions. Each complete-file copy has the exact Git blob ID named as the patch postimage (`018748de...`, `b9f1452c...`, and `fee9294e...` respectively).

## Standards review

No actionable standards finding.

The RED separation is genuine: the test calls the absent `CreateExperimentalPrecision116Setup()` at `RED/tests/experimental_precision116_profile_seam.h:283-286`, while the current public header declares only the diagnostic and original-paper factories at `include/openfhe_2023_1788/repeated_mult2.h:58-60`. No placeholder declaration or production file is present in RED. Thus missing-factory name lookup is the intended source-level break; an actual hosted compiler receipt remains required to establish the observed first diagnostic.

The CMake addition reuses the already `EXCLUDE_FROM_ALL` paper executable (`CMakeLists.txt:133-144`), adds a separately named CTest with `RUN_SERIAL`, `OMP_NUM_THREADS=2`, and a bounded 1200-second timeout (`RED/CMakeLists.txt:273-279`), and leaves the original CTest and properties byte-for-byte intact (`RED/CMakeLists.txt:266-271`). The main-file delta is only the include and argument dispatch. The dispatch precedes all prior modes and the no-argument `Run()` path (`RED/tests/paper_full_eight_square_contract_test.cpp:439-448`); the pre-existing body after it is unchanged.

## Specification review

No actionable specification finding.

- Exact profile binding is explicit, not inferred from bit counts: all eleven ordered Q modulus/root pairs and the reserved P/root are literals (`RED/tests/experimental_precision116_profile_seam.h:35-49`). Every family is checked for exact Q/P/QP identity, native ceiling, order-65536 roots, alpha-one tables, metadata exponent 58, second-last deletion, and distinct context/parameter/scheme/basis/tag/key-row ownership (`:64-197`). The exact root-subset check uses modulus, root, and order through `ReadBasis`, rather than tower position alone (`:122-151`).
- The root client keys are checked against family 0 and its tag; coefficient conversion checks a signed ternary secret of Hamming weight exactly 128 with identical signs across every root-Q tower (`:198-224`). Family evaluation rows are checked through public context/tag and QP shapes (`:177-188`). The test adds no projected-secret accessor or retention hook; its only secret snapshots are of the explicitly client-owned root secret, while the production projection boundary remains the scoped local secret at `src/repeated_mult2.cpp:320-346`.
- The plan-bound client rejects the generic context route and the obsolete S100 request, then accepts the frozen full-slot input at exact S116 (`RED/tests/experimental_precision116_profile_seam.h:297-324`). In the current client order, the scale rejection precedes transforms and PKE (`src/high_precision_client_io.cpp:541-584`), so RED requests Encrypt twice but performs only one successful encoding/encryption operation if GREEN preserves that fail-fast order.
- The test performs one DCP and exactly one successful public `Mult2(pair, pair)` (`RED/tests/experimental_precision116_profile_seam.h:325-337`). Its expected pair states agree with DCP's level-one divisor removal (`src/double_ckks.cpp:370-462`) and with `Mult2`'s nonterminal `Reenter` return (`src/double_ckks.cpp:1213-1224`). The exact receipt chain is correctly asserted as family-1 nonterminal Reentry over family-0 Rescaled, Relinearized, Tensor, and Input, with `S1=S0^2/(d*m7)` (`RED/tests/experimental_precision116_profile_seam.h:338-351`; receipt construction at `src/repeated_mult2.cpp:254-285`). The rejected `RCBWithReceipt` call exercises the unchanged terminal-only guard (`src/double_ckks.cpp:1246-1254`); there is no `BindRepeatedRcb`, full-eight loop, endpoint capture, publication, or normal `Run()` call.
- Owner checks use weak references for the candidate plan, root keys, evaluation rows, representative ciphertexts, and receipts, and verify removal of every candidate tag without changing an independently live N64 diagnostic row (`RED/tests/experimental_precision116_profile_seam.h:256-281, 376-414`). This matches the plan-owned cleanup mechanism (`src/repeated_mult2.cpp:223-232`). The candidate's large evaluation rows are retained only as raw identities/weak references; only the deliberately small N64 diagnostic rows are copied for coefficient-preservation checks.
- PASS and FAIL output are separately named and truthfully state `squares=1`, `full_eight_square_E80=NOT_TESTED`, and `security=UNRESOLVED` (`RED/tests/experimental_precision116_profile_seam.h:415-416`; `RED/tests/paper_full_eight_square_contract_test.cpp:440-446`). They do not emit the live endpoint schema or alter the original E80 result.

## Limits and disposition

Source review found no blocker to applying this RED slice for the requested hosted compile RED. API and type use is consistent with the inspected public headers except for the intentionally missing factory. This is **NOT COMPILED / NOT RUN**: it does not establish compiler portability, actual missing-factory diagnostic ordering, OpenFHE runtime resource use, successful encryption/multiplication, cleanup behavior, numerical accuracy, or security. The appropriate next evidence is the exact hosted RED compile receipt before any GREEN production patch is applied.
