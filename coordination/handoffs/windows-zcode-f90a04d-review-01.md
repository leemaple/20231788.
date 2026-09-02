# Windows ZCode f90a04d review handoff status

Recorded: 2026-09-02 22:33 Asia/Shanghai

Status: **submitted once in a new ZCode task and running asynchronously; no
review verdict is claimed**.

## Bound task and source

- ZCode task title: `Cleanroom Relin2 f90a04d Code Review`;
- controlling task:
  `coordination/tasks/windows-zcode-f90a04d-review-01.md`;
- task size/SHA-256: `14,477` bytes /
  `0e26f574ac615f3f7111a1698262968cdbf738793486cf55d6209c0745c1e214`;
- candidate commit/tree:
  `f90a04d199e96a3247a2607aa3e1f80ad55be8cc` /
  `7edbfad070201f68a60d1b53f6c72bbb99939eb3`;
- formal hosted run: `33638053832`, attempt 1;
- pristine OpenFHE commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The Windows launcher workspace is the newly created Downloads folder
`新建文件夹 (2)`. The controlling task requires ZCode itself to create the
first unused `C:\20231788-review-f90a04d-01` root before any extraction or
review. The historical `Untitled session`, the stalled fb862a3 task, the
b9f26db transfer attempt, and all old local implementations remain untouched
and forbidden as inputs.

## Five-file packet

The Mac staging directory contained exactly these five files:

| File | Bytes | SHA-256 |
|---|---:|---|
| `2023.1788.pdf` | 759,375 | `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac` |
| `2023.1788.txt` | 90,235 | `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` |
| `20231788-cleanroom-relin2-r1-f90a04d-ci33638053832.zip` | 248,965 | `a235258d178c716de9700d79d7d68dab4fa5d1c14493dfd85af4ff7cab399f1c` |
| `windows-zcode-f90a04d-review-01.md` | 14,477 | `0e26f574ac615f3f7111a1698262968cdbf738793486cf55d6209c0745c1e214` |
| `20231788-cleanroom-relin2-r1-f90a04d-ci33638053832.binding.md` | 5,051 | `8fb64b2e6aeddb584ab326dd53c478b21df42654e200e63797919d1544af6ef6` |

The source ZIP is an exact `git archive` with 98 members and 75 regular files.
It passed `unzip -t` and a fresh-extraction preflight for unique/path-safe,
unencrypted, non-symlink entries. The five-file directory and the fresh
extraction both passed Gitleaks 8.30.1 with redaction; the retained JSON result
for each is `[]`. Independent targeted sensitive-filename/content scans also
returned zero without retaining matching secret text.

## Windows transfer and attachment

The two paper files already existed in `C:\Users\cnlif\Downloads` from the
earlier exact paper transfer and displayed the expected byte sizes. UU Remote
was set to apply `跳过` to conflicts, so no existing file was overwritten.
The three f90a04d-specific files were then transferred and each displayed
`已发送`. ZCode must independently hash all five Windows files before use;
this receipt does not infer their hashes from the displayed sizes.

Exactly five attachments were added one at a time to the new task: the binding,
source ZIP, controlling task, paper PDF, and paper text. No b9f26db or fb862a3
artifact was attached.

## Submitted prompt and discovered envelope defect

The 1,983-character dispatch text was copied back from the ZCode composer
before Send. It matched the Mac source byte for byte and had SHA-256
`2a84ca9ca39cd0ccc458df6fb14af8cfa025a7d05c1d413a3fa653c6825467e8`.
Send was clicked exactly once. The resulting task title and the five attachment
chips were visible, and ZCode began working.

The post-send cross-check found that the dispatch text had copied two incorrect
paper hashes from an intermediate summary:

- PDF dispatch value:
  `61d9b94891b4cbf937a8a5be5230bf06bba438b85beb1e50d381e5b7ad5a9b49`;
- TXT dispatch value:
  `60dd871ac28f55e512c99f1f6e2f3842f6083e279ee02eb46bd9411714df61aa`.

The attached controlling task, attached binding, and actual staged files all
contain the correct values shown in the table above. The prompt explicitly
names the attached task as controlling and says to verify attachments against
the binding, but the conflict can still require a fail-closed `BLOCKED`
preflight. No second message, interruption, stop, resend, retry, duplicate
task, or correction was sent while ZCode was active. After its natural
preflight response, at most one correction may be sent in this same task with
the two correct values and an instruction to continue from the completed
identity gate. The task must never be restarted from scratch merely because of
this envelope defect.

## Operational decision

This asynchronous ordinary review is outside the critical path. GitHub Actions
remains authoritative for Windows build/CTest. No local Windows build, review
finding, PASS, code change, commit, push, or merge is claimed here. If ZCode
raises a genuine paper-algorithm, OpenFHE API, scale/lifecycle, or architecture
ambiguity, Codex will submit the precise question and complete evidence to
terminal `claude-fable-5-1` with fallback disabled rather than guessing.
