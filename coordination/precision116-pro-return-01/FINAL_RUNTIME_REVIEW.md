# Final bounded runtime/semantic audit

## Disposition

**Scoped one-operation runtime acceptance; no blocker found.** GitHub Actions run `34051183115` supplies actual Linux and Windows evidence that tested source `2759fa90840946ef42957c7ba71ebea47e0e4995` builds warning-clean against pristine OpenFHE `df495ba2e91739a6dc8f1de254fc5a41155ce504`, preserves the selected prerequisite suite, and passes the exact experimental S116 one-operation seam. This is not acceptance of eight-square numerical correctness, the original `2^-80` gate, endpoint evidence, terminal RCB/rebinding, security, or the whole project.

I reached this disposition from the task, current source/tests, and raw job logs before consulting any root-authored hosted-execution verdict. This is an extra Codex adversarial review context, not a Pro semantic review and not provider-diverse evidence; the inference backend identity is not separately attested here.

## Reviewed identity

- Task: `coordination/precision116-pro-handoff-01/TASK.md`, SHA-256 `e55068b18b1bdf7e3714d085598d80ec62ff6ca558e90a225c018709f8e4f26f`.
- RED run/status: run `34050734415`, source `70c37679f4760c6dc2bebc35bdfd741c238f315b`; raw Linux log SHA-256 `f246a0a13f2422313d120602074e4372abc2f73cda27bceee122965cb3496a0a`, Windows `f68d34853bd442d5df363e5a42c6e1c967dacd6b84a58de3ff77f031ff4236d6`, status `3436834802f34b64fee9edee2efab5c6e217023669e05de3f4e30f6c99da665b`.
- GREEN run/status: run `34051183115`, tested source `2759fa90840946ef42957c7ba71ebea47e0e4995`; raw Linux log SHA-256 `9fcb0c7c8edfb9147c9885895990ddd698458c27281d111cd97781c23b2ff630`, Windows `30b3b31b0245dfddfdb164d04d1d85647163a4b82ebf1baaefa3b3f678924d4e`, status `3d5b9a9325529576fb9f3f68e3e38f207251806e57359e90cad1a96887fecc99`.
- Current docs-only HEAD during review: `3b3b0f653dc7be3e572f90c34a81cbb749451dd9`. The four tested production files are byte-identical to the return's reviewed GREEN complete files:
  - `include/openfhe_2023_1788/repeated_mult2.h`: SHA-256 `f95acc03b0feecd78a4763c9f776cbb3e0f6429860d71e81cc6ceb75e47f7cf3`.
  - `src/repeated_mult2.cpp`: SHA-256 `6781d01ff9b4012e7c6d14d16f482def0821c54f4e81e2de80bd8db9d1aea41b`.
  - `src/high_precision_client_io.cpp`: SHA-256 `57bbd6a7e0312dca4eb6adb90e236d3ff0334938c1da14c142213b2e871a8933`.
  - `src/double_ckks.cpp`: SHA-256 `c530b53d600aeb0c295477aebb84bddff2c25b4a139004cdc888edd5dba425eb`.
- The RED test slice did not drift between RED and GREEN: Git blob IDs are `018748de9c68b2b3db86c567ef5f3f12919b58a3` for `CMakeLists.txt`, `fee9294e11f0424e8514501e1b4118aa210ca1a3` for the main paper test, and `b9f1452cfbd20ef0601c07e6ea56aaf07504a912` for `tests/experimental_precision116_profile_seam.h` (the latter's SHA-256 is the frozen `a9e1ab95b5d0e85b8ca635ec0aae7fe5b927a9972046f2fc1f995067f1f86fa5`).

## Evidence chain

The TDD transition is real. Both RED jobs first diagnose that `CreateExperimentalPrecision116Setup` is absent at the frozen test's line 285 (`RED_LINUX_JOB.log:5642-5645`; `RED_WINDOWS_JOB.log:5942-5945`) after their 60-test prerequisite selections passed (`RED_LINUX_JOB.log:5622`; `RED_WINDOWS_JOB.log:5916`). Later messages about `savedLow` and initializer-list deduction are compiler cascades from the missing setup type, not separately established RED defects; the unchanged test compiling and running under GREEN resolves that ambiguity.

The GREEN jobs use fresh exact checkouts. Linux fetched and checked out project SHA `2759fa...` (`GREEN_LINUX_JOB.log:96-110`), checked OpenFHE HEAD `df495ba...` and an empty status (`:287-294`), then rechecked project HEAD equals `GITHUB_SHA` and the project status is empty (`:1075-1090`). Windows initialized separate clean-room project and OpenFHE repositories, fetched those same exact commits, and rejected dirty states (`GREEN_WINDOWS_JOB.log:35-78`); it repeated the OpenFHE HEAD/status check (`:650-675`) and project HEAD/status check (`:1294-1314`). This supports clean-room execution identity; it does not independently re-audit every upstream source line.

Both hosts built the normal project and explicit API targets, then executed the same prerequisite selections:

- focused first-Mult2 precision: 1/1 PASS (Linux `GREEN_LINUX_JOB.log:1206-1248`; Windows `GREEN_WINDOWS_JOB.log:1444-1488`);
- Pair Add/Sub inputs: 2/2 PASS (Linux `:1249-1297`; Windows `:1493-1543`);
- legacy checkpoint: 57/57 PASS (Linux `:1298-3298`; Windows `:1548-3550`);
- production client-I/O: 1/1 PASS (Linux `:3402-3449`; Windows `:3639-3688`);
- repeated-Mult2 plus h128: 2/2 PASS (Linux `:3450-3505`; Windows `:3736-3793`);
- complete selected suite, explicitly excluding both paper no-argument and experimental tests: 60/60 PASS (Linux `:3506-5624`; Windows `:3798-5918`).

After explicitly building `paper_full_eight_square_contract_test`, the workflow selected only `^experimental_precision116_profile_seam$` with `--no-tests=error`, `OMP_NUM_THREADS=2`, and CTest timeout 1200. Linux executed the experimental flag and passed 1/1 in 10.85 seconds (`GREEN_LINUX_JOB.log:5647-5687`); Windows executed the same flag and passed 1/1 in 11.75 seconds (`GREEN_WINDOWS_JOB.log:5949-5992`). The overall status records both jobs and the run as success.

## What the passing seam establishes

Because the test bytes are unchanged from RED, one PASS means all its fail-fast assertions ran to completion. In each of two hosted environments and for that run's randomized key/noise draw, it establishes:

- the exact named Q/root/P candidate, N32768/full-slot geometry, native limits, all eight second-last-deletion families, alpha-one HYBRID tables, metadata exponent 58, and distinct family contexts/tags/evaluation rows (`tests/experimental_precision116_profile_seam.h:35-197`);
- a client-owned sparse ternary root secret with exactly 128 nonzero coefficients and identical signed coefficients across the root Q towers, plus public/root key context and tag consistency (`:198-224`);
- plan-bound S116 frozen-input public encryption, rejection of S100 for this candidate, DCP, and exactly one successful `Mult2(pair,pair)` (`:283-337`);
- physical S116 reset, the nonterminal family-1 Reentry state, and the exact public receipt ancestry and rational `S1=S0^2/(d*m7)`; terminal-only `RCBWithReceipt` still rejects that nonterminal result (`:234-254, 338-351`);
- input/key/evaluation-row preservation, release of the candidate plan/root keys/representative ciphertexts/receipts/evaluation rows, removal of candidate cache tags, and preservation then cleanup of an independently live N64 diagnostic setup (`:256-281, 352-414`). Passing those checks is runtime evidence for the defined ownership boundary. It is not proof that OpenFHE retains no other process-global context allocation.

The implementation supporting that run remains narrow in source: immutable original and experimental descriptors are the only paper-geometry profiles (`src/repeated_mult2.cpp:22-51, 257-278`); family validation derives exact bases and metadata from the issuing descriptor (`:111-205`); only the client setup factory returns the root secret, while each projected family secret is scoped locally through evaluation-key generation (`:353-379, 462-494`). The client-I/O plan path derives its exact scale/metadata from the issuing plan while the context-only path stays at 50/100 (`src/high_precision_client_io.cpp:176-244, 455-509, 557-576`). Evaluator arithmetic was not changed by this slice; the `double_ckks.cpp` delta only derives the terminal wrapper's expected recorded factor from its plan, a path not exercised by this one-operation test (`src/double_ckks.cpp:1246-1285`).

The original profile's literal Q/P values and metadata 50 remain in the descriptor, and the selected legacy suites pass. However, because the original no-argument eight-square test was deliberately excluded, this run does not constitute a fresh runtime result for the original full chain.

## What remains unproved

- The experimental result is not decrypted or compared to a plaintext one-square oracle. Completion proves the public arithmetic path and structural invariants execute, not first-square numerical accuracy.
- Operations 2 through 8, terminal RCB, `BindRepeatedRcb`, full-slot E0/E8/I8/A8 diagnostics, endpoint writing/finalization, and the original `2^-80` predicate were not run. The argument dispatch returns before normal `Run()` (`tests/paper_full_eight_square_contract_test.cpp:439-491`), and the GREEN status marks endpoint observer, endpoint finalization, selection, and upload steps skipped. Building the executable also compiled endpoint translation units, but did not execute them.
- No full-eight intermediate nonwrap/capacity premise, Gaussian tail statement, cross-key guarantee, or performance claim follows from two successful one-operation draws.
- Security remains unresolved for the approximately 712-bit QP exposure at N32768/h128. There is no estimator result and no basis for inheriting the paper's 128-bit security claim.
- The plan-derived terminal metadata changes compile and pass surrounding regression selections, but the experimental terminal path itself remains unexecuted. It belongs to the later eight-square/numerical slice.

Accordingly, run `34051183115` closes the bounded RED/GREEN **experimental one-operation integration seam**. It does not close the paper-scale correctness goal; the finite next discriminating slice remains the separate experimental eight-square numerical/endpoint test under the unchanged original E80 criterion and explicit unresolved-security label.
