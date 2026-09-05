# h128 final-review packet preparation — 2026-09-05

Status: PREPARED, NOT UPLOADED, NOT SENT. Root owns independent acceptance,
commit/push and Pro dispatch. No source/test/build-file changes or new CI were
made for this packet. Initial branch `codex/paper-h128-keypair-01` was clean at
`a5d6f933e26fbf58ab06ee679faa444f441567b8`; that is the evidence snapshot, not
the tested engineering SHA `1192200f558c69c0967e8306ed1a8bddf786ca34`.

## Frozen final artifact

Only build-03 is eligible for Root's pre-dispatch gate:

`/Users/lifeng/Documents/20231788-openfhe-paper-h128-keypair-20260905/artifacts/handoffs/h128-final-review-01/build-03/paper-h128-final-review-1192200.zip`

- ZIP: 2,202,028 bytes; SHA-256
  `c4b8012a1f690d40c5571b24ae7828f414a5d05c6715a45fec0f3cb3e6710305`.
- Adjacent `.zip.sha256`: 102 bytes; SHA-256
  `7d385961bd17e28761778d0ec10ba429c0c44082b73bb7939a740150b7fdfd9e`.
- Root directory: `paper-h128-final-review-1192200/`.
- 315 regular ZIP members, 314 manifest payloads, 7,760,741 expanded bytes.
- MANIFEST.json SHA-256:
  `4fe6548df021c0fae06519aa99fc9db2d0078b35f218be5c87fdbd159eebd15c`.
- Root TASK.md equals
  `coordination/tasks/PAPER_H128_KEYPAIR_FINAL_REVIEW_01.md`: 14,524 bytes,
  SHA-256 `bfd000b4719089910c010977f630510bff105a4d4a7a857e00ec580fc29b3518`.
- Requested output is `paper-h128-final-review-1192200-return.zip` and sidecar,
  deliberately distinct from the immutable incoming ZIP basename.

Builder: `build_final_review_01.py`, SHA-256
`9afbe001c24e4a9febafd98b5fbf7b75e916d4ce06d03403f2961042069972bc`.
Run from this worktree with `python3` and `--output` followed by the absolute
build-03 directory above (without the ZIP basename). The builder requires a
fresh output directory and exact evidence HEAD; it must not overwrite an
existing build. A future reproduction needs another fresh leaf and preserved
input refs. It performs Git-object/byte, archive and source-selection checks,
mechanical packaging and gitleaks; no compile, crypto, CI or upload.

## Source closure and exact provenance

- `current/project/`: 29 exact tested engineering Git blobs, all local source,
  tests, actual CMake/workflow, README and .gitignore. No unrelated worktree.
- `current/evidence/`: 44 exact evidence-HEAD Git blobs, including all four
  actual acceptances, eight complete logs, eight host verification records,
  all four final run metadata documents and preparation/dispatch records.
- `stages/`: 22 exact Git blobs covering the six changed engineering paths at
  A RED/A GREEN/B RED/B GREEN; absent A RED header/source recorded explicitly.
- `official/`: 74 exact pristine-pin Git blobs: all 51 original official
  inputs plus 23 targeted supplements. They cover actual RTTI component
  definitions and key-switch inheritance, SK/PK/evaluation/cache APIs, public
  parameter generation, ordinary CKKS leveled operations, packed plaintext /
  ciphertext getters, encoding and FFT source, and upstream LICENSE.
- `historical/input/`: all 98 original regular members, byte-for-byte, with
  only the outer directory stripped. Includes all 39 clean-room project
  inputs, 51 official objects, original task/design/seams/paper/metadata.
- `historical/pro-return/`: all 38 original return members, likewise preserved,
  including four patches, complete final changed files, frozen contract and
  arithmetic evidence. Historical NOT RUN claims are not silently rewritten.
- Six exact/byte-directed diffs: four adjacent actual engineering stages,
  original-input-to-final, and original-Pro-to-tested two-build-file overlay.

The original input ZIP remains 1,185,384 bytes with SHA-256
`52f0dec88ac9ee854b2a863a60f382f35a6bf7117aada1b39d45637f8e367e8b`;
the original return remains 96,840 bytes with SHA-256
`ccf2ecad2b6db7d0c6306dcedcd64b21e6e57aa677fa33e0eab83b964aa5df5a`.
Both durable source ZIPs and sidecar digest fields were checked. Compressed
original byte streams are not nested into the final bundle; every decompressed
member is preserved and mapped by PROVENANCE.json. Original project/official
payloads were again compared to exact Git objects, not just their manifests.

No .git, installed dependency tree, builds/binaries, databases, runtime/browser
state, credentials or unrelated documents are included. Full CI logs are
deliberately selected review evidence, scanned before packaging; no secret key
coefficients are printed. The official selection is a targeted review closure,
not a transitive build kit. Reviewer must report a concrete missing path/symbol
if any further object is essential, rather than guess or fetch private state.

## Validation actually performed

Builder exit0: safe original archive CRC/member/path/mode checks; exact archive
identities; 39 input-project / 51 input-official Git comparisons; every current,
stage, evidence and unified-official Git byte comparison; recomputed Git blob
SHA1; exact four stage source/run/attempt mappings; eight complete retained log
identities/provenance; final production/header/test/profile equal original
return; only reviewed build-file overlay differs. Root independent acceptance
and external-review conclusions are still pending.

Separate fresh build-03 ZIP read: CRC, regular modes, encryption/traversal/
NFC-casefold gates; all 314 manifest payload byte sizes and hashes; 259 declared
Git-origin blob SHA1 identities; exact root task and sidecar; expected315 member
closure. All passed. A build-02 validation initially assumed Windows's CTest
summary spelling for Linux; adjusting the read-only parser to allow Linux's
`, 0 tests failed` text passed all eight recorded summaries. No evidence/test
was edited. Build-03 differs build-02 only in TASK's requested return filename
and its updated manifest entry; that byte-difference set was checked directly.

Gitleaks 8.30.1, 2026-09-05 13:39 Asia/Shanghai:

- `gitleaks dir <build-03/root> --redact --no-banner`: exit0, 7,001,366 scanned
  text bytes, no leaks.
- `gitleaks dir <build-03> --max-archive-depth 1 --redact --no-banner`: exit0,
  14,002,732 scanned text bytes (staging plus actual archive), no leaks.
- Targeted filename exclusions additionally enforced by builder; PDF is
  preserved as user-authorized source material. Zero scanner findings are not
  an absolute guarantee of no secrets.

build-01 and build-02 are retained only as ignored, UNSHIPPED preparation
artifacts. build-02 added eight smoke-source objects before acceptance;
build-03 adopted Root's non-colliding output-basename correction. Do not use
either prior archive for upload. No artifact was uploaded or sent in this task.

## Scope of requested decision

Offline source/record review only, with separate Standards/Spec axes and real
review ZIP/sidecar. Reviewer may conclude ACCEPT_DIAGNOSTIC, CHANGES_REQUIRED
or BLOCKED_INPUT_CLOSURE. No N32768, precision80, eight squarings, 1000 trials,
security/performance, forced collision, generic concurrency, or full paper
completion claim is requested. New compilation or cryptography is prohibited.
