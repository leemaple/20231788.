# I/O source correction preparation 01

Prepared 2026-09-05 Asia/Shanghai, on `codex/lossless-io-implementation-01`.
The worktree was clean at `49c6e1303240408e2edae5651d120f4660ae9283` before
this slice. Earlier observations were committed and pushed as
`f75884f02a8a3ed5b3fea66c0105eed248a06952`. All changes in this slice are
coordination/materials; implementation base remains
`4ccc8fd2e7617625d27e58a53eb3489e99466ed4`. No source/test/CMake/workflow changed.

## Confirmed input failure and preserved return

Current terminal Pro conversation is `Implement Lossless Client IO`,
https://chatgpt.com/c/6a9b7241-e52c-83ec-8427-5fd90dd34904,
Ego task space 122, tab `6A78CA7CE5DB24A33D4DA65583A95E67`.
`INPUT_CLOSURE_TERMINAL_01.json` preserves DOM evidence: final response,
Stop answering absent, final Copy control present, empty composer.

The blocker ZIP and sidecar were successfully downloaded at approximately
10:27, after scrolling their controls into view. Earlier off-viewport click
attempts produced no located file and were not successful downloads. No chat
was refreshed, stopped or resubmitted to recover those controls.
Actual ZIP: 60,194 bytes, SHA-256
`9d01fdf88a884cf9524e654f97240db05120ba3ef827cde4e35c60a6d17932de`;
sidecar 111 bytes. Independent safe gate: 24 regular entries, 22 payloads,
141,299 expanded bytes; all hashes, paths, modes, CRC and closure passed.
See `ROOT_BLOCKER_VERIFICATION_01.md/.json` for exact findings and disclosed
probe limitations. Its reported exit 2 was not rerun by Codex; an independent
source scan confirmed 20 present / 5 missing sources directly instead.
Gitleaks 8.30.1 archive/decode-depth 5, redacted: zero findings, 141,299 bytes.
This is an input omission, not a semantic RED or impossibility result.

## Corrected full input, not a blind supplement

- New ZIP: `lossless-client-io-implementation-01-4ccc8fd-source-correction-01.zip`
- Bytes: 3,132,684
- SHA-256: `4c8295d56ca59d39441adbdc2fe24e87bb2bafaab4128b19265ba159337f329c`
- Sidecar: 136 bytes, exact new basename and digest.
- One root: `lossless-client-io-implementation-01-4ccc8fd/`
- 122 regular members; 120 manifest payloads.
- Root manifest: 14,893 bytes, SHA-256
  `3308504e4a6db8ead01bdc4da70810536eda3862758529f819005cde56b09b89`.
- Original 111 payloads preserved byte-for-byte; all 47 project files also
  independently matched exact implementation-base Git blobs.
- Exactly five new official source files, each matched against fresh official
  pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`, expected SHA/Git blob and
  the authenticated older clean-room design packet. Effective official count
  is 64; original metadata's count 59 remains explicitly historical.
- Original manifest and sidecar preserved at `provenance/original-input/`.
- TASK remains 23,771 bytes / SHA-256
  `707d366dcd4880450ac09ba4c1eb6195daf64def65333c305c20a099f8eadb1f`;
  no seam, test, threshold, vector, oracle, source commit or patch-order change.

See `SOURCE_CORRECTION_01.md/.json`, `SOURCE_SELECTION_BUILD_01.json`,
`SOURCE_CORRECTION_MANIFEST_01.tsv`, and `ROOT_CORRECTION_VERIFICATION_01.json`.
The independent root verifier also checked safe normalized/case-fold unique
paths, no encryption/special files/excluded filenames, bounded expansion,
CRC, full manifest/sidecar, original bytes, pinned added sources, and a fresh
extraction. Gitleaks 8.30.1 with archive/decode-depth 5 and redaction scanned
the exact staging (2,408,676 bytes) and archive (1,657,460 scanned bytes), both
zero findings. Scanned-byte counts are scanner output, not total ZIP size.
No unresolved secret finding remains. Source scanning is evidence, not a guarantee.

Build output is `/private/tmp/io-source-correction-20260905.oeqUa5/build-01/`.
Durable local copies of corrected input/sidecar, original input, authenticated
design input, and blocker/sidecar are under this worktree's
`artifacts/handoffs/io-source-correction-01/`; each ZIP copy was rehashed.
That directory is ignored by existing `.gitignore`, keeping input archives
locally recoverable without adding the user paper/archive bulk to public Git.
The builder accepts `--original-zip`, `--design-zip`, and a fresh empty
`--output-dir` for later recovery. Its temporary official source objects were
retained rather than deleted. No old implementation or modified local OpenFHE
was inspected or used.

## Dispatch boundary

`SOURCE_CORRECTION_PROMPT_01.md` supplies complete context and asks for the
unchanged four-patch production assignment, not another confirmation.
Pre-upload draft: 6,367 bytes / SHA-256
`c817d3f9d0fd8a2b91023c12ef532348a596c171f0cedeb5b5371ba19da43436`.
Any UI filename alias must be documented in the final sent prompt and receipt.
Preparation alone does not prove dispatch; see a subsequent dispatch receipt
for the actual once-only send and observed post-send state.

## Read-only cross-track observations

Repeated RED run https://github.com/leemaple/20231788./actions/runs/33938285334
is complete on source `7399db55b799a166aee9b72b8f89bcded373b540`, attempt 1.
Both hosts passed warning-clean default build, all five API steps, first-Mult2
1/1, Pair 2/2 and legacy 57/57, then failed explicit new-target compilation on
`openfhe_2023_1788/repeated_mult2.h` at oracle line 6. Linux legacy time was
0.68 s; Windows 2.52 s. Linux build exit 2; Windows build exit 1. New runtime
focus and full 58 were skipped, not passed. The run/job JSON, checkpoint
extract and failed-step logs are retained here for the next continuation.
One upstream log line retains trailing whitespace; this raw evidence is
intentionally preserved, not presented as whitespace-clean source code.

`REPEATED_PRE_GREEN_GUARD_01.md` records a newly confirmed P1 in the candidate:
four CKKS CCParams setters unconditionally throw at the exact pin. The required
values already are defaults; the narrow correction is production-only deletion
of those four calls while preserving actual-value validation and supported
low-level constructor/precompute arguments. This corrects an earlier static
review omission; no Green failure has been run. No unresolved source dispute
requires Fable arbitration for this finding. The next Repeated continuation
must retain the real RED and this guard before applying corrected GREEN.

Only I/O materials were advanced this slice. Repeated GREEN, h128 candidate
acceptance, any new CI, merge, Mac build/crypto/benchmark, and new ZCode work
were NOT RUN. The earlier repeated run was read, never redispatched or rerun.
