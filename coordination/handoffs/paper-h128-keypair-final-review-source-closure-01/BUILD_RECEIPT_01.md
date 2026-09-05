# h128 final-review source-closure build receipt 01

Historical build-01 receipt. Root superseded this unsent archive with build-02;
see ROOT_PACKET_ACCEPTANCE_01.md and ZIP_PREFLIGHT_FINAL_01.json for the only
approved transmission. Historical input/payload identities below still refer
to build-01, not the now-updated tracked builder and task.

Observed at 2026-09-05T07:00:07Z. Packaging-only work; no compile, OpenFHE, crypto, NTT, benchmark, CI dispatch, external-agent contact, commit or push.

## Result

Packaging preflight PASS. Corrected-source review acceptance is still pending.

- ZIP: `/Users/lifeng/Documents/20231788-openfhe-paper-h128-keypair-20260905/artifacts/handoffs/h128-final-review-source-closure-01/build-01/paper-h128-final-review-source-closure-1192200.zip`
- ZIP size: 2327474 bytes.
- ZIP SHA-256: `59af44a987b40e43095db70ead5974e988ab1843591336a7fada51eb62ac7234`
- Matching sidecar: `/Users/lifeng/Documents/20231788-openfhe-paper-h128-keypair-20260905/artifacts/handoffs/h128-final-review-source-closure-01/build-01/paper-h128-final-review-source-closure-1192200.zip.sha256`; 117 bytes; SHA-256 `3f43f3bd9f0bd93e81525053aa1bdfca9b493277fa90de5f7fb86dea37dabb5e`.
- Current inventory: 329 regular members, 328 non-self manifest payloads, 8672012 expanded bytes.
- Current root MANIFEST.json: 287264 bytes, SHA-256 `62d6bc72ce4b7a2fa6d8292406267bf39c1d0075d3d5eeb0c53d7e2dd79ce04f`.
- Current root TASK.md / tracked self-contained task: 26387 bytes, SHA-256 `21abe07b4ddcabeb82d39a447a646c04b67c5be66e2358ad304a03aab2b45b44`.
- Builder: 18943 bytes, SHA-256 `98fc006d29cf4414d03875398828ad3e2bd3d4bb7d4b821ede280539370ae261`.
- Tracked ZIP_PREFLIGHT_01.json after root removed its extra terminal blank line: SHA-256 `050ba54cff9939a52bbccaef28f94b763fd691ab72aa5968b5ae2ad965b9e76f`. Its complete JSON object still equals the historical build result; the ignored output retains BUILD_RESULT.json. This formatting correction changes no ZIP payload.

The outer ZIP digest, current manifest digest and scan receipts are external to the inner payload graph. Root MANIFEST.json excludes only itself. No current archive or manifest digest is embedded recursively into an included provenance payload.

## Byte-preservation and provenance

The original build-03 ZIP remains 2,202,028 bytes, SHA-256 `c4b8012a1f690d40c5571b24ae7828f414a5d05c6715a45fec0f3cb3e6710305`. All 315 original members, including all 314 original manifest payloads, have exact one-to-one byte/SHA mappings in ORIGINAL_PAYLOAD_MAPPING.json. Only original root TASK.md and MANIFEST.json move to original-input/. The other 313 paths are unchanged below the new outer directory. The complete original task is also reproduced within the new authoritative task after its supersession notice.

All 29 original selected engineering files, eight full job logs, historical input and original implementation return, stage files and patches remain present and byte-identical. No engineering source/test/profile/oracle/threshold or log changes are introduced. The tested engineering identity remains `1192200f558c69c0967e8306ed1a8bddf786ca34`; observed packaging HEAD before/after is `d6f05d007c32e2ac0a44aa3923dbaec0b7aa2828`.

Exactly four new official files come from pristine approved `df495ba2e91739a6dc8f1de254fc5a41155ce504` via Git object reads. Git blob IDs are independently recomputed from each returned object's bytes and compared to the pinned object name. ZIP_PREFLIGHT_01.json and SOURCE_CLOSURE_PROVENANCE.json record full pin/blob/SHA/size per file:

- CKKS PKE implementation closes IC-01's missing actual Poly-output Decrypt body.
- RNS leveled-SHE header and implementation close IC-02's missing intermediate ciphertext/ciphertext dispatch.
- DecryptResult definition supplies the same path's immediate return-type metadata; it is not another P1 finding.

This changes official selection count from 74 to 78, not the tested implementation. Supplying bodies does not predetermine the independent semantic review verdict.

The original completed five-file Pro return is preserved under review-return-original/ as untrusted evidence. Its ZIP remains 47,452 bytes, SHA-256 `31e3bea98c2468a31be2eab1f3688f238e68be393dc6a8ecad2a1d5692a6a416`; its four-payload non-self manifest and all five bytes/paths were checked.

## Security and archive gates

Passed on both selected expanded bytes and re-read final ZIP:

- Single-root, regular unencrypted members; CRC.
- No absolute/traversal/noncanonical/backslash/colon paths.
- Raw-name and NFC-casefold uniqueness.
- Exact non-self manifest membership, sizes and SHA-256.
- Original one-to-one mapping and final archive/selection equality.
- Credential/runtime/binary-artifact filename exclusions and targeted credential-shaped content checks.
- gitleaks 8.30.1, exit 0, 0 findings for both expanded selection and archive traversal. Each actual scan reports 7,912,637 scanned bytes; that scanner metric is not the total expanded archive size.
- Ambient GITLEAKS_CONFIG/TOML removed, no packaged gitleaks config/ignore file, inline allowances disabled, ignore path /dev/null, decode depth 5 and archive depth 1. Exact commands, redacted reports and capture hashes are in preflight; raw scan captures remain in build-01.

No scan exemption or secret suppression was added. The ignored artifact directory is confirmed ignored by Git. Parent-authored untracked old-handoff receipt files were observed and left untouched.

## Current scope and verification limits

The new task explicitly applies the latest user instruction: 1000 repetitions are no longer an acceptance/delivery gate. All historical bytes are preserved, including superseded wording. Appropriate independent high-precision oracles, key normal/negative/boundary tests, dual-platform regressions and consecutive-computation correctness under paper parameters remain overall project gates; no performance-statistics gate is added.

Root separately reported a fully inspected, isolated offline run of the original verifier, producing SHA-256 `e3edb0cf78b5a60ab74583b8818f5883afb55bba020eb7dbdb3dcbc2a2193df1`; sorted complete-object comparison to the supplied mechanical_checks passed (exit 0). Its bounded coverage is recorded in new task/provenance/preflight: 10 replays, 34 invocations, 718 starts, 8 jobs, 58 bindings, 259 blobs and specified exact arithmetic certificates. The packager has read that returned verifier but did not execute it, rerun any crypto, or authenticate external services. The root claim is accurately labeled as root-reported, not silently attributed to this construction run.

Still pending at this packaging snapshot:

1. Root's independent final ZIP integrity/security approval.
2. The separate independent manual source-anchor/conclusion audit of the original review.
3. Actual upload and completed review of this corrected full packet, including source-supported IC-01/IC-02 resolution.

The existing original verifier hard-codes the old ZIP and expects the three gaps to exist; unchanged use against the new packet is invalid. A source-closure completion or full paper implementation success is not claimed here.

## Ownership handoff

Only the authorized new source-closure handoff directory, new source-closure task file and new ignored source-closure artifact directory were written. Existing source/tests/workflow and the original build-03 were not edited. All packaging writes are now frozen and released to root for independent verification and any selected documentation commit/transmission.
