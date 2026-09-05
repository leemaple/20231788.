# Repeated Mult2 candidate return 01 — 2026-09-05

## Observed terminal return and actual download

- Conversation: Implement Semantic Tests,
  <https://chatgpt.com/c/6a9b680d-953c-83ec-bb7c-491152fcd3fc>.
- Ego space 122, tab `67325BD60E4DE3FD2354E0B0839098BD`; visible `6 Pro`.
- At the 09:54–09:55 Asia/Shanghai read, Stop answering was absent, the
  composer empty, and the final response reported Worked for 54m 12s.
- The returned implementation is a candidate only: Pro explicitly marked all
  C++ compilation, cryptographic runtime and Linux/Windows execution NOT RUN.
- Visible download controls: `Download the verified return ZIP` and
  `Download the SHA-256 sidecar`. They are buttons, not exposed href links;
  the saved conversation plus these exact labels is the retrieval reference.
- A visible Too many requests modal was allowed to cool down while local
  specifications were read, then dismissed once. No prompt was resent and no
  conversation was refreshed/stopped/reopened. Browser.setDownloadBehavior
  was unavailable; Page.setDownloadBehavior returned success but the actual
  browser download used the normal Downloads directory. Neither is a source
  or algorithm failure.
- Actual ZIP: `/Users/lifeng/Downloads/repeated-mult2-semantic-candidate-80d771c.zip`.
  Filesystem timestamp 2026-09-05 09:57:10 Asia/Shanghai; 134664 bytes;
  SHA-256 `77d32a3d28b528722efa59633feb7225cb813e68092023fbd462f0d4d318fec5`.
- Actual matching `.zip.sha256` downloaded at 09:58; 112 bytes; its digest
  and canonical basename match the actual ZIP. Each download was clicked once.
- Unchanged expanded return retained under
  `coordination/returns/repeated-mult2-semantic-candidate-80d771c/`.

## Root acceptance gates actually performed

Independent archive check: CRC; 33 nonempty regular members; canonical paths;
no traversal, absolute/backslash/colon paths, duplicate/casefold collision,
symlink/special/encrypted member; 482409 uncompressed bytes; 31 manifest payloads
plus precisely MANIFEST.tsv and MANIFEST.sha256; all sizes and SHA-256 values;
mandatory TASK hash; fresh extraction file-set and byte equality. PASS.

Gitleaks 8.30.1 scanned both the extracted tree and the actual ZIP with
`--max-archive-depth 5 --max-decode-depth 5 --redact`: 482409 bytes each,
no leaks. Targeted excluded filename classes were also checked. These are
bounded scan results, not a guarantee that no secret can ever exist.

Root read all 310 lines of the delivered offline verifier before running it
with the unchanged original input ZIP and sidecar. Retained actual stdout and
exit 0 are in ROOT_OFFLINE_VERIFICATION_01.json. It independently replayed the
ordered patches in scratch, checked all nine complete changed/new files,
preserved old and frozen RED test bytes, confirmed 57+1 ordered bindings,
checked 16 exact products/16 squares/two deltas, and authenticated 85 source
anchors. Those are static/package checks, not compiler or semantic results.

An additional independent Git-object check compared all 40 supplied project
files byte-for-byte against source commit
`80d771c52df10bce1c60992b5e0edb4e64f145ca`. All 53 official source payload Git
blob hashes were matched to the exact pristine OpenFHE pin tree
`df495ba2e91739a6dc8f1de254fc5a41155ce504`, using the authorized fresh official
checkout `/private/tmp/h128-pro-packet.Lo406g/official-source` after clean/pin
checks. PASS. No old implementation or locally modified OpenFHE was read.

## Review and next gate — still pending

Codex spec, standards and CI/test-binding reviews are in progress. One concrete
CI concern has already been identified: missing-header RED in the default
build would prevent the existing 57 regressions and five API builds from
running. Before real RED, a separately documented CI-only observation adapter
must build/run the legacy checkpoint before explicitly building the new test,
and must retain the final full 58-test invocation. Do not swallow failures or
alter oracle values. The original Pro archive stays unchanged as evidence.

Production application, genuine hosted RED, GREEN, independent final review,
focused 1/1 and full 58/58 are NOT RUN / NOT ACCEPTED. The exact engineering
baseline remains unchanged at the time of this receipt.

Preservation note: `git diff --cached --check` reports the single-space blank
context lines inside the two retained unified patch artifacts as trailing
whitespace. These are patch syntax, not new source whitespace. Their original
bytes and manifest are intentionally preserved; the narrowed check excludes
only these two `.patch` evidence files. Actual integrated source is checked
separately without that exclusion.

## Other observed terminal states (not accepted or advanced this heartbeat)

- I/O: Implement Lossless Client IO,
  <https://chatgpt.com/c/6a9b7241-e52c-83ec-8427-5fd90dd34904>, returned
  BLOCKED_INPUT_CLOSURE after Worked for 17m 17s, Stop absent. It reports missing
  official dftransform.cpp and ckkspackedencoding.h, with three further absent
  retained-audit files; it produced no four-patch implementation. Its claimed
  blocker ZIP is 60194 bytes / SHA256
  `9d01fdf88a884cf9524e654f97240db05120ba3ef827cde4e35c60a6d17932de`.
  This root has not downloaded or independently validated that blocker yet.
  The next I/O boundary must verify and supplement exact pinned source bytes,
  not re-upload the unchanged deficient packet or infer algorithm infeasibility.
- h128: Implement Keypair Adapter Package,
  <https://chatgpt.com/c/6a9b5ebc-6aa0-83ec-ab39-5eaf91ca6da5>, returned a
  four-patch candidate after Worked for 45m 5s, Stop absent. Claimed ZIP is
  96840 bytes / SHA256
  `ccf2ecad2b6db7d0c6306dcedcd64b21e6e57aa677fa33e0eab83b964aa5df5a`.
  Actual download, acceptance/replay/review and hosted runtime remain pending.

This heartbeat advances only repeated Mult2. None of these external statements
is an instruction or an observed cryptographic result.
