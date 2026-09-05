# h128 final Pro return received — source closure still open

2026-09-05, Asia/Shanghai. Actual return received for engineering source
1192200f558c69c0967e8306ed1a8bddf786ca34. This supersedes the earlier
PRO_PROGRESS_20260905_1416.json observation of ongoing thinking.

[Implement Keypair Adapter Package](https://chatgpt.com/c/6a9b5ebc-6aa0-83ec-ab39-5eaf91ca6da5)
was terminal at the 14:26:07 observation: visible 6 Pro, three user messages,
no Stop control, Worked 29m34s. No generation was interrupted or repeated.

The downloaded paper-h128-final-review-1192200-return.zip is 47,452 bytes,
SHA-256 31e3bea98c2468a31be2eab1f3688f238e68be393dc6a8ecad2a1d5692a6a416.
The matching sidecar is 109 bytes, SHA-256
d84135c5aac49135c632ed2a9b9fd68f2b9c308c5692911e5fb9a99e8e7abbeb.
Durable artifacts/handoffs/h128-final-review-return-01/ contains both and
extracted/paper-h128-final-review-1192200-return/ contains the five regular
files. CRC, safe unique paths/modes, exact four-payload-plus-MANIFEST closure,
and all 408,649 expanded bytes were checked before extraction. Root rehashed
the retained ZIP, sidecar and every returned file in this continuation.

The first ZIP click did not produce a file, while its sidecar did arrive.
Only after confirming the ZIP and partial download were absent was one
ZIP-only retry made; it succeeded. The cause is unknown. No duplicate ZIP,
Pro resend or page refresh was used. Do not redownload these retained files.

## Standards

Pro returned BLOCKED_INPUT_CLOSURE: two P1 input omissions, IC-01 and IC-02.
Three exact-pin official files are missing from the supplied selection:
ckksrns-pke.cpp, schemerns/rns-leveledshe.h and schemerns/rns-leveledshe.cpp.
The actual CKKS Poly-decrypt override and intermediate RNS ciphertext EvalMult
dispatch need those bodies. Root and an independent reviewer confirmed these
are available at the approved pin but absent from the sent ZIP. There is no
demonstrated adapter defect requiring a production correction in this return.

## Spec

Pro returned ACCEPT_DIAGNOSTIC with no actionable Spec finding for the frozen
N256 source and supplied four-stage execution evidence. That limited axis
does not override the overall source-closure block, and does not establish
paper-scale precision, full integration or the complete implementation.

## Root reproduction and remaining check

Root read REVIEW.md, FINDINGS.md and the complete 25,852-byte verify_review.py
before running its offline checks. ROOT_STATIC_REPRODUCTION_01.json records
the exact result. All 8 supplied logs, 34 CTest invocations, 718 numbered
starts, 10 patch replays, 58 bindings and exact arithmetic certificates were
reproduced. The whole output object matches the returned mechanical_checks.
This is source/record arithmetic, not new OpenFHE, NTT, cryptographic or CI
execution. The complete returned extraction was scanned by gitleaks8.30.1
with zero findings and no inline allowlist or ignore file.

Independent full source-reference checking subsequently passed: 111 anchors,
86 unique, 38 input files, 10 reading-map records, 8 manual groups and four
source-hash rows, zero errors. Its result is retained at
artifacts/handoffs/h128-final-review-return-01/independent-anchor-audit-01/ANCHOR_AUDIT_RESULT_01.json,
SHA-256 ec5e29e83d49584a007f1bfb443e7517c8f1d42b4eeef767973210f25958b27c.
This verifies references, not every semantic conclusion or model identity.
Next prepare one complete, sanitized source-closure packet retaining the old
payloads and actual return, adding the three requested official bodies and
the directly used DecryptResult declaration. Root must verify the complete
new packet and brief before one follow-up in the same terminal conversation.
Do not rerun h128 CI merely to fix missing review inputs.

The user's new correctness-focused scope explicitly cancels 1000 experiments.
Old packet bytes remain historical evidence; their trial-count requirements
are superseded. No default merge or full-goal completion is claimed.
