# Repeated Mult2: independent ZCode static review dispatched

## State and purpose

2026-09-04, Asia/Shanghai. Previous goal turn was PROGRESS: actual Pro return
bytes were recovered, independently reviewed, and preserved in pushed commit
`05c8cd873070144b0c74b0f0c5cde93420924d46`. This follow-up starts from that
clean branch, `codex/repeated-mult2-01`. Project source/tests/CMake/workflow
remain identical to the frozen `774fe2dcfca47d7a08cab9c04b29c430e354cf9f`.

The next review covers incoming candidate quality and R1–R6 reconciliation,
not semantic second-Mult2 success or implementation approval. New
client-setup/Mult2 test-seam confirmation remains pending. No candidate patch
was applied and no source/test/build/CI change was made in this dispatch.

## Complete input and independent preflight

Dedicated persistent project directory:
`/Users/lifeng/Documents/20231788-openfhe-zcode-repeated-probe-review-20260904`.
Source/evidence were copied there without modifying their bytes. The package
is prepared for Mac LOCAL STATIC fallback; it is not a Windows workspace.

Archive: `distribution/repeated-mult2-zcode-local-static-774fe2d-05c8cd8.zip`;
3084581 bytes, SHA256
`72320f25fc2b2e055cb9a0aaf761516667bdecd5cf5f7c124f968fcbf4f76ead`.
It holds complete expanded source156 and candidate24 inputs, both original
archives, six current evidence files, task, policy and security receipts.
No old local implementation or modified local OpenFHE is included.

Root fully read TASK.md, PACKAGE_INTEGRITY.md, SOURCE_SNAPSHOT.json and the
final external preparation receipt. Root independently checked archive CRC,
safe/non-link/non-encrypted paths, unique/case-fold-unique members, exact local
prepared-file equality, all 196 payload lengths/hashes plus the one manifest,
and all source156/candidate24 bytes against the two original ZIPs. Six copied
evidence files also equal the exact committed 05c8cd8 Git blobs. There are
197 total members, 5890518 payload bytes plus 129878 manifest bytes =
6020396 raw bytes. This is input integrity, not test execution.

TASK.md: 9445 bytes, SHA256
`184c24866e10a8af9f1a24907116cbb241dd996617515ea1370b32d090d49900`.
MANIFEST.json: 129878 bytes, SHA256
`187c0b6c74b4dc5acd94fc7272e0aede17990c2920f349d0fe8f12da7fa2f078`.
The packager's immutable preparation/scan originals are retained under
`returns/repeated-mult2-zcode-05c8cd8/`; their prepared-not-dispatched wording
is the author's historical scope, not the later UI outcome.

Root additionally scanned the final distribution directory with maintained
gitleaks8.30.1: ambient config overrides unset, GOMAXPROCS=2, redaction,
ignore-inline-allow, ignore path /dev/null, max archive depth3, decode depth5,
max target10MB. Actual 23:34 output: 7305207 bytes, zero findings, exit0.
The separate wrapper scan covered1701 bytes with zero findings. Earlier
source-selection and supplemental filename/content scans are attributed to
the packager in its original receipt. Clean scans are evidence, not a guarantee.

## Shared quota gate

Root reused the logged-in quota page read-only. Reload affected only
https://bigmodel.cn/coding-plan/personal/usage, never the Pro conversation.
Observation2026-09-04T15:34:33.261Z; page refresh label2026.09.04 23:32:

- Every5hours: used9%, remaining91%; reset01:56. Next Sep5 occurrence is an
  Asia/Shanghai inference because the page displays only the time.
- Weekly: used72%, remaining28%; reset2026-09-09 10:00.
- MCP monthly: used16%, remaining84%; reset2026-09-25 10:00.

All ZCode clients share these limits. Neither the exhausted-5h nor90%-weekly
gate was reached. No reset credit, purchase, authentication or quota-setting
change was made. One bounded review was dispatched, not a batch of experiments.

## Actual native UI dispatch

Application `/Applications/ZCode.app`. Root added the exact dedicated folder;
Choose workspace showed that folder selected (Value1), with the old project
unselected. The new-task model selection loaded the existing default
GLM-5.3 / Max. No other model is being represented as Fable5.1.

Wrapper: `tasks/REPEATED_MULT2_ZCODE_STATIC_REVIEW_05C8CD8.md`,1701 bytes
including file newline, SHA256
`fbdc75c697689c4e8054f85ad7667904d43936dd4c1e62d44932bdc3cdf71d38`.
All1700 prompt characters were read back exactly in the intended new-task
composer before Send. Root clicked Send exactly once at
**2026-09-04T15:37:14.229Z** (23:37:14.229 Asia/Shanghai).
Post-send UI showed a new task under the correct project, Working for1s,
Stop, empty follow-up composer, GLM-5.3 and Max.
Initial displayed task title:
`Repeated Mult2 independent incoming-candidate review. Wor...`.

These are direct root UI observations, not the packager's observations or a
completed review. Native task completion must be verified at this exact
project/task before collecting output/REVIEW.md and output/EXECUTION_LEDGER.md.
Do not send another task merely because review takes time. The reviewer must
preserve inputs and write only those two reports; unexpected writes or claims
require explicit reconciliation, not silent acceptance.

Read-only follow-up2026-09-04T15:43:41.149Z confirmed the actual task title
`Repeated Mult2 candidate review per TASK.md`, Working for6m25s, Stop and
GLM-5.3/Max. Visible activity had reached complete probe reading and pinned
public constructor/type checks. This confirms a live reviewer, not a verdict.

## Parallel work / next action

At2026-09-04T15:38:21.025Z the separate
[Design Client IO Seam](https://chatgpt.com/c/6a9ad753-af90-83ec-9062-0fc671f64197)
was authoritatively live with Stop. No final I/O package has been received;
its169-check progress statement remains an external claim. No Stop, refresh,
reminder, duplicate send or restart occurred.

Root also made read-only first-party conversation lookups at15:40–15:41Z,
using the existing login ephemerally without persisting credentials. They
returned HTTP200. Latest text was still the same commentary, with per-message
`finished_successfully` and numeric `async_status=3`; no final response was
present among the assistant text messages inspected. The enum is not decoded
here, and a finished commentary message is not evidence of overall completion.
The page still displayed Stop. No state-changing recovery action followed.

While these tasks run, a bounded source-only h128 Candidate-B decision is
being developed from the pinned public primitives. Root independently checked
the key-generation/EncryptZeroCore relation, PREMode-dependent GetParamsPK
and tag-cache bodies. This may inform setup/I/O integration, but is not h128
runtime acceptance. Actual second/eight-operation precision and the paper's
1000-trial experiment remain required; no Mac build or crypto was performed.
