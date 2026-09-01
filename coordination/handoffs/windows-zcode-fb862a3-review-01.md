# Windows ZCode/Zima fb862a3 exact-review handoff

Submitted: 2026-09-01 20:34 Asia/Shanghai

Status: **submitted exactly once; preserved without a verdict and superseded on
the critical path by the user's 2026-09-01 21:26 Fable5 instruction**. This is a
dispatch receipt, not a review verdict, source change, build, test, or
acceptance claim.

## Exact Git and candidate binding

- Coordination task commit before dispatch:
  `21bae27ffc0a651f12e6cedf0b54f0d7a5ef2be7`.
- Local HEAD, upstream, and `git ls-remote` all matched that commit before the
  Windows task was sent.
- Controlling task: `coordination/tasks/windows-zcode-fb862a3-review-01.md`,
  19,834 bytes, SHA-256
  `2e9127ab58b8d8f76bc72231a4af4ea1ad596cf929bae489c889c0a9a49ead83`.
- Candidate commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Candidate tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`.
- Pristine OpenFHE commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Retained same-SHA Actions run `33436252725` passed 6/6 on Linux and 6/6 on
  Windows. The ZCode review must independently verify the exact candidate and
  must not treat the hosted result as its own execution evidence.

## Exact five fresh inputs

Exactly five files were transferred through UU Remote and attached to the new
ZCode task:

| # | File | Bytes | SHA-256 |
|---:|---|---:|---|
| 1 | `2023.1788.pdf` | 759,375 | `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac` |
| 2 | `2023.1788.txt` | 90,235 | `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` |
| 3 | `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.binding.md` | 3,640 | `3320efa8723f0c519da453a006617c328de5bfa2aca72392a6161c66a0489d2f` |
| 4 | `20231788-cleanroom-relin2-base-fb862a3-ci33436252725.zip` | 9,115,214 | `3e839a6b88a81107657442a2bb4f6b08385f6a24685cab11968db540436750f6` |
| 5 | `windows-zcode-fb862a3-review-01.md` | 19,834 | `2e9127ab58b8d8f76bc72231a4af4ea1ad596cf929bae489c889c0a9a49ead83` |

The UU transfer list reported all five as sent. The ZCode composer showed all
five attachment chips before dispatch and the sent task retained them.

## Pre-transfer integrity and secret gates

- The source ZIP contains 2,266 archive entries and 1,998 regular files.
- Duplicate member count, symlink count, encrypted member count, forbidden
  directory/name count, and unsafe absolute/traversal path count were all zero.
- Gitleaks 8.30.1 was run separately over the five-file upload directory and
  the safely extracted source tree. Both correct invocations exited zero and
  reported zero findings, scanning approximately 113.71 KB and 25.99 MB.
- Targeted sensitive-filename and high-signal credential-content path lists
  were both empty.

Only those two separate directory scans are controlling. An earlier malformed
multi-source diagnostic invocation that accidentally included unrelated local
working-directory evidence is not package evidence and is not used for any
claim.

## Windows isolation and prompt integrity

- A brand-new ZCode task was created; the old `Untitled session` and its 17
  unreviewed changes were not opened, read, edited, copied, or reused.
- A new Windows folder was created through the folder chooser and selected as
  the ZCode workspace. The chooser left its default name as
  `C:\新建文件夹`. The controlling task and sent prompt require ZCode to create
  the actual fresh sibling work root `C:\20231788-review-fb862a3-01`, or a
  previously unused suffix if that exact path exists, before extracting or
  working on the candidate.
- The sent prompt was 1,504 JavaScript characters. Clipboard paste followed by
  select-all/copy readback returned exactly the same 1,504 characters. Both
  sides had SHA-256
  `d8c495b8dc2e6e11173d30c52b01bb5fc54035422f27b074b69aef321f5feaf5`.
- The prompt contained both `BEGIN WINDOWS ZCODE FB862A3 REVIEW 01` and
  `END WINDOWS ZCODE FB862A3 REVIEW 01`, every attachment size/hash, the exact
  task/commit bindings, the clean-room prohibition, the two-thread Windows
  limit, and the required complete-verdict deliverables.

## Send and active-state evidence

The Send control was clicked exactly once. The next read-only state showed a
new task title derived from `BEGIN WINDOWS ZCODE FB862A3 REVIEW 01`, the sent
message with all five attachments, an empty/reset composer, and an active
`已工作 1 秒` indicator. No second click, resend, retry, or duplicate task was
made.

The task must now run without interruption. Use spaced read-only checks. Do not
open or reuse the historical implementation session. ZCode must return the
complete verdict and all three required deliverables; a progress-only response
is not acceptance. It is forbidden to edit, commit, push, merge, or rewrite the
candidate or coordination repository.

Subsequent spaced read-only checks for more than fifteen minutes showed only
the unchanged `已工作 1 秒` shell: no substantive output, terminal verdict,
explicit error, stop control, or downloadable artifact appeared. The user then
directed Codex to use Fable5 instead, return to ZCode after recovery, and avoid
blocking. The Windows page/task is therefore preserved exactly as-is; no
interrupt, retry, resend, duplicate, or inferred review result is allowed.

## Capacity and execution bounds

The last official BigModel reading before allocation showed 0% five-hour use,
39% weekly use with reset displayed as 2026-09-02 10:00, and 4% MCP monthly use
with reset displayed as 2026-09-25 10:00. This is the one restored critical
ZCode allocation. The task pins `OMP_NUM_THREADS=2`, `OMP_THREAD_LIMIT=2`,
`CTEST_PARALLEL_LEVEL=1`, and both CTest invocations to `--parallel 1` so the
Windows host remains responsive. The later user instruction recorded above
supersedes the former Fable reservation for this exact review.
