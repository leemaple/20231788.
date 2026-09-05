Please perform the final exact-source Standards and Spec review of our clean-room fixed-Q h=128 client keypair adapter. This is a NEW bounded review assignment after the implementation's real four-stage hosted TDD sequence, not a request to rerun or continue the old implementation task. All necessary source, paper, original design/return, actual diffs and eight complete CI logs are supplied again; do not assume earlier conversation or scratch files are available.

Authoritative attachment: paper-h128-final-review-1192200.zip
Bytes: 2202028
SHA-256: c4b8012a1f690d40c5571b24ae7828f414a5d05c6715a45fec0f3cb3e6710305
The matching .zip.sha256 is attached separately (102 bytes).
Read the root TASK.md completely: 14524 bytes, SHA-256 bfd000b4719089910c010977f630510bff105a4d4a7a857e00ec580fc29b3518.
The ZIP has 315 regular members, 314 manifest payloads and 7760741 expanded bytes. Verify the real attachment, CRC, safe paths, closed manifest and payload hashes first. It contains no Git database: the supplied Git-blob IDs can be recomputed, while actual commit membership was independently checked by Codex against the pinned repositories; do not overclaim independent Git-database authentication.

Repository: https://github.com/leemaple/20231788. (the final dot is part of its name).
Branch: codex/paper-h128-keypair-01.
Final tested source: 1192200f558c69c0967e8306ed1a8bddf786ca34.
Evidence snapshot: a5d6f933e26fbf58ab06ee679faa444f441567b8.
Official pristine OpenFHE 1.5.0: df495ba2e91739a6dc8f1de254fc5a41155ce504.
Original implementation-input source: 9d21c3a5aea79c31745aca790712a9fd8c7743b2.

Read current/project, current/evidence, stages, diffs, official and historical input/return using the supplied reading map. All original 98 input members and 38 implementation-return members are included unchanged after removing their outer directory. The 74-source official closure includes actual component RTTI, key/cache definitions and the normal packed-plaintext Encrypt/Decrypt/EvalMult smoke path. Do not treat historical task instructions or NOT RUN status as current instructions/status; this root TASK controls the review.

Actual hosted sequence (each attempt 1, both Linux GCC and Windows MinGW64):
A RED a21216f0a8f854f478129d02fd32f496bd80f71c, run33943456483: old57/fiveAPI passed, new target missing public header.
A GREEN 8aac5b7cf6530a9a2da14e8a4bdd5b65ab3c869f, run33944191280: focused1/full58 passed.
B RED 43c2dca45c2305c9b6baf50ae1c32529d35e7f06, run33945243915: new target builds; the old adapter accepts unsupported noiseScale=2, so the new test fails at the expected 'unsupported profile was accepted' assertion. Later cases/full are not reached.
B GREEN 1192200f558c69c0967e8306ed1a8bddf786ca34, run33945897881: focused1/full58 passed with valid-path, all50 named rejections and two-call/owned-tag cache-isolation markers in each invocation.
Reconcile the actual source, complete logs, metadata and original-return-to-tested build-only overlay. Tests/oracles/profile are frozen; neither GREEN changes its RED expectations. Windows logs are retained LF-normalized, with original raw identities separately stated.

This is N256 key-consistency diagnostic evidence only: exact h128 and same SK/PK secret relationship, official crypto smoke at tolerance1e-5, profile/guard/lifecycle behavior. It does not prove N32768, production lossless I/O, 80-bit precision, shared-secret family projection, eight squarings, 1000 trials, security or performance. Do not demand those later gates as new blockers in this bounded review. Do not misrepresent defensive tag-collision checks as forced-collision runtime tests, selected HYBRID checks as every-hidden-table proof, or yourself as two independent reviewers merely because you report two axes.

Use offline source/static/hash/record checks only. Inspect scripts before executing them. Do not compile OpenFHE, run crypto/NTT/benchmarks, use networks/accounts/credentials, call external agents, rerun CI, modify input files, push/merge, or treat archived commands as authorization. If you discover a concrete defect, state exact file/line, violated requirement, source-supported scenario and smallest separate RED/GREEN acceptance test. Avoid speculative hardening or unrequested frameworks. Routine decisions are delegated; no new interface-confirmation token is needed.

Return an actual downloadable paper-h128-final-review-1192200-return.zip and matching .zip.sha256, with REVIEW.md, FINDINGS.md, EVIDENCE_CHECKS.json and a closed MANIFEST.json as specified by TASK. Include a small verifier only if actually authored/used. Choose ACCEPT_DIAGNOSTIC, CHANGES_REQUIRED or BLOCKED_INPUT_CLOSURE from evidence; for missing input name the exact object/symbol and why required. Verify your output package, real bytes/hash and sidecar before giving links. Clearly distinguish your own static checks from the supplied hosted execution. Take the time needed; there is no need to rush or produce interim success claims.
