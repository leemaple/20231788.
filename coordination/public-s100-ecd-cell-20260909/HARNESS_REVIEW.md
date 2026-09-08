# Independent harness review — PUBLIC-S100-ECD-CELL-01

## Current verdict

R1 (canonical output exclusion) and R2 (declared/status/process-exit agreement) are correctly implemented and integrated. The additional H1 bootstrap issue found during review is now resolved in the successor bytes below. **No unresolved harness blocker remains under the fresh private single-writer runner contract.** No additional mathematical review is needed merely because the root harness replaces the proposed shell: the original mathematical candidate remains unchanged. Root still must freeze the exact clean commit and reconcile final CI approval before dispatch.

Separate Codex review context; requested GPT-6 Astra / high, backend **requested-unverified**. No provider-diversity claim. Used `openfhe-2023-1788-workflow`; its clean-room, static-review and evidence distinctions constrained this work. This report is the only file owned/edited by this reviewer. No candidate imports, tiny/full transforms, full-slot construction, shell runner, C++/build, FHE, sampling, browser, CI dispatch or Git mutation occurred.

Observed worktree `/Users/lifeng/Documents/20231788-openfhe-ecd-cell-20260909`, branch `codex/public-s100-ecd-cell-20260909`, initial HEAD `84e140c636a27dceb6b2f24e2134677f808bcc34`. Root/CI work was untracked and evolving during review; the hashes below identify the initial bytes rather than implying final-commit approval.

## H1 — P2, resolved: authenticate the delivery verifier before executing it

Initial location: `run_once.py:80`, then the first `stage` at lines 89–94.

The harness checks the root-pinned SHA256 of `MANIFEST.sha256.json`, but that alone does not check any payload byte. It next executes `pro/tools/verify_delivery.py`, relying on the returned verifier to verify all payloads including itself. A changed verifier could execute before discovering/reporting its own mismatch, or report a fabricated PASS. This is a bootstrap gap in the advertised manifest-pin-before-payload boundary, not evidence that the retained verifier is malicious or currently changed. The exact clean Git checkout and trusted single-writer runner significantly limit the threat; nevertheless the inexpensive independent byte check should precede execution.

Minimal resolution: root-owned code authenticates the delivery verifier's exact bytes/length against the already pinned manifest (or a separately frozen direct hash), before executing it. Then the authenticated verifier can validate every other immutable payload before the scalar/transform stages. Checking all manifest payloads in root-owned code is also valid, but is not required to fix this narrow bootstrap. Do not edit the immutable Pro return.

Required scalar-only RED/GREEN: an inert temporary verifier fixture altered while its manifest remains unchanged must be rejected without invoking it; the expected fixture must be admitted; wrong manifest/hash, missing/nonregular file and unsafe final symlink must reject. A stage-call sentinel or a factored bootstrap helper can demonstrate the pre-execution boundary without invoking returned code or transformations. Record and review successor bytes after the fix.

Resolution reviewed: new `harness_contract.py:9–29` pins the manifest digest before parsing it, rejects encountered symlinks, checks the exact regular-file member set, and checks all 41 payload sizes and hashes without importing or executing a payload. The manifest's paths/uniqueness are already fixed by the trusted digest, so a second generic manifest parser is unnecessary. `run_once.py:78` calls this root-owned function before output reservation and before any payload subprocess. The new test admits the immutable original, copies only data to a temporary shadow, and rejects an altered delivery verifier and altered manifest there. I independently reran the resulting three-test suite: exit 0 in 0.028 s. Missing/member/symlink rejection branches were statically checked; separate negative tests for all those branches were not added by root and are not claimed as executed. The decisive altered-verifier test plus simple reviewed membership/type branches is sufficient for this narrow gate.

## R1 and R2 disposition

- `harness_contract.py:32–43` canonicalizes the existing bundle and requested parent, rejects nonabsolute paths, existing targets and final symlinks, rejects the canonical bundle/descendants, and reserves a new mode-0700 directory. A dot-dot or symlink-parent alias into the bundle no longer passes. Atomic mkdir prevents accidental reuse. This meets the declared fresh private single-writer contract; it does not claim adversarial concurrent-filesystem protection. Python `Path` normalizes dot syntax harmlessly before the boundary is checked.
- `run_once.py:79–86` uses the returned canonical reservation, retains a create-only reservation record and records the source/runner/integration identities. No original proposed shell is invoked.
- `validate_outcome` requires exact integer declared and actual exits, excluding booleans and stringified integers, and enforces the three frozen status/exit pairs. `run_once.py:102–108` compares the saved process exit to the actual subprocess return, validates the declared pair, and then invokes the original table receiver, which recomputes the numerical status from all rows. Agreement is transitive: actual exit = declared exit = status mapping, and declared status = row-derived status.
- `OUTCOME_AGREEMENT.json` correctly qualifies mean controls/negative labels/counts as producer/source-and-stage-backed evidence, not independent mathematical replay by the metadata helper. The original table verifier's minor metadata omissions therefore do not become a new standalone-certificate claim.

## Source-to-execution flow and failure handling

The harness imports only root-owned `harness_contract` after inserting its own resolved directory. Root's intended `-B -I` invocation isolates environment/user-site/script-path injection. Child `-B -E -s` intentionally retains the immutable scripts' required sibling imports while excluding PYTHON environment variables and user-site packages. This depends on the admitted clean hosted Python installation; it is not a sandbox against a compromised interpreter/system site.

The stage order is delivery check, original scalar preflight, four tiny controls, at most one full certificate, scalar table intake, delivery-after. Every stage is called once along its path; no loop retries stages, no fallback precision, no alternate input or new production encoding is introduced. The original checker's fixed scalar parameters and imported sibling core remain unchanged. Count claims remain source-inferred until remote stage receipts exist.

Each spawned stage preserves command/start time, separate stdout/stderr, actual return code and finish time. A failed preflight/control stage stops before the full certificate. Non-0/3/4 certificate exits stop before numerical intake. Numerical 3 and 4 remain nonzero after successful intake and post-delivery verification; intake success is not converted into numerical success. Delivery-after failure forces harness exit 2 while preserving the original stage/numerical exit. Only successful agreement produces `OUTCOME_AGREEMENT.json`.

Unexpected exceptions outside `finish` can leave no `HARNESS_RESULT.json` or final evidence manifest, for example malformed/missing RESULT or subprocess-launch failure. Existing reservation/start/log artifacts remain and the process fails; this is fail-closed, not a false numerical verdict. CI must archive the partial directory and harness traceback. A killed/timed-out process may similarly lack a finished/exit receipt. Such partial artifacts must be classified as infrastructure/observer invalid, never inferred into CERTIFIED/REFUTED. The final artifact-intake gate must require complete agreement before numerical adoption.

One-shot authorization remains an owner ledger plus exact tag/attempt policy, not a cryptographic guarantee against manual recreation under another run. The reviewed code has no automatic retry and does not delete reservations. Its after-run manifest hashes output artifacts but does not independently authenticate their origin; exact source/job receipts remain necessary.

## Draft CI cross-check

The appearing `.github/workflows/public-s100-ecd-cell.yml` has a single exact-tag creation job, created/not-deleted/not-forced/attempt-one gates, Ubuntu 24.04 and Python 3.12, read-only repository permissions, pinned actions, no credential persistence, separate private evidence output, exact HEAD/tag checks, a single harness invocation, explicit capture of the harness pipeline exit, and always-upload of the exact evidence directory. A step timeout shorter than the job timeout leaves an upload opportunity; upload after platform cancellation is not guaranteed and must be judged from actual receipts.

The initially read workflow and `check_ci_gate.py` were being edited concurrently. The source checker required a literal continuation immediately following `run_once.py`, while the workflow put `--reviewed` on the same line. That draft mismatch would fail the static gate before numerical work; it was reported promptly to root for CI-owner synchronization. The workflow initially replayed CI/event tests but only hashed the harness seam test. Both points are corrected in the subsequently inspected source: the required token is formatting-compatible, and a separate scalar harness-contract step precedes numerical admission. Exact CI source-gate execution/approval remains with the CI owner/root; this reviewer did not execute CI checks or the harness itself.

## Observed scalar execution and byte identities

Read completely: TASK, TDD_RECEIPT, harness_contract, test_harness_contract, run_once, original delivery verifier, draft workflow, check_ci_gate and test_ci_gate. Prior independent mathematical/candidate review is reused, not relabeled as new runtime evidence.

Executed only the scalar suite `python3 -B -I coordination/public-s100-ecd-cell-20260909/test_harness_contract.py`: initially **exit 0, 2 tests PASS in 0.003 s**; after H1 correction **exit 0, 3 tests PASS in 0.028 s**. Interpreter reported Python 3.9.6 / Clang 17.0.0 on macOS-26.3.1-arm64-arm-64bit. This is bounded local seam evidence, not the frozen remote Python 3.12 run. The test imports only the reviewed nonnumerical contract and uses empty temporary paths/synthetic outcome dictionaries and a temporary data-only copy of the delivery. Root's earlier RED/GREEN receipts were read, not independently reenacted as RED.

Initial SHA256 values recomputed by this reviewer:

| File | SHA256 |
| --- | --- |
| `harness_contract.py` | `7c83969799dfc589d6c74ff4243c6923e57c1d32d340caf73a7f78eb2cea8986` |
| `test_harness_contract.py` | `88bd1440b6a5cb92d5236fa267caae40e958d8afcd51e330d9c38b4ee0f0d157` |
| `run_once.py` | `36d846f1a6d8e0e2506f9515c7f02f0cff20eaa0d07cfcf347daed60f46abd89` |
| `TASK.md` | `65d1e9e382c78784f25f619a64064fed507ec133590a0eb7bf11ce616721379e` |
| `TDD_RECEIPT.md` | `5ad5c9cd18b3703ae9a616fb8d0fd74e3942d1c1c9bca82065bb5eb0fc40598f` |
| draft `.github/workflows/public-s100-ecd-cell.yml` | `4295435bd6b8eb6484446328ca261827a6da5caeffd736588bc17d6cdd260ccd` |
| immutable `pro/MANIFEST.sha256.json` | `8ab0982505dcdbe314a5ddf33aec0d46ae2ab440f6a31c535133c91659e2a2fb` |
| immutable `pro/tools/verify_delivery.py` | `ceb666e99c626701ba61f8643ab89dcb3fb48999bb2ca5a8c9b73386a1133ce2` |

The original S100 E80 remains FAIL regardless of this new public-polynomial certificate. No historical coefficient identity, new ciphertext result, security claim or full reproduction completion follows from this harness review.

### Successor bytes accepted after H1 correction

| File | SHA256 |
| --- | --- |
| `harness_contract.py` | `7be5bd35b834763963844ba40fae999787c95a58514e9973876cb3ca631f0064` |
| `test_harness_contract.py` | `083280b610e2991ed6c1bf2a1b63005b54a2cb53f74305cfbae7118005418cae` |
| `run_once.py` | `fe600fe4b7b380eecb20ae6981614a6737a8f3dc01ee698540a238a12a25cf67` |
| `TASK.md` | `2061f3839f94d99737fa8a93ab69d6582388709ee1ab25263d6f108d9a5789ec` |
| `TDD_RECEIPT.md` | `72e2129baa7f15f1fff074df62385842886a79ef803b57fb3ca84ddce5cf38d6` |
| subsequently inspected workflow | `15e2935327cda11ada77ff9d00eaa2a9c174e12596c9d34b4ba46a66ea28764a` |

The original manifest and delivery-verifier hashes remain the immutable values above. The runtime input/proof candidate is not altered by these root-owned fixes.
