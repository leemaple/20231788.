# Adjudication return — receipt and bounded replay

2026-09-09 Asia/Shanghai. Scope: current clean-room source 31e24bec1eb2db5d13de3b442a9e909e26db7206, not quarantined implementations. Production baseline a4b815a733efe81897325e2a8e4c826a4ebfa439 is unchanged.

## Transport and identity (observed)

[执行复现裁决审查](https://chatgpt.com/c/6aa06117-a1b8-83ec-bb22-710339c3b865) completed without stop, refresh or duplicate submission. Highest visible selector was 6 / Pro, Pro 5 of 5 / Latest; backend identity is not attested. Terminal was observed at 2026-09-08T20:10:29.733Z; displayed duration 43m59s. One download click at 20:11:07.184Z. Dedicated Ego space214 was closed through completeTaskSpace only after the validated return was retained.

ZIP: 369089 bytes, SHA256 ca8aa74937bc425d91519ec032a2bf13a842dad37384b6e9172e9fd501871424. All 42 original files (952482 expanded bytes) are preserved under pro/. Manifest SHA256 8ab0982505dcdbe314a5ddf33aec0d46ae2ab440f6a31c535133c91659e2a2fb, 41 exact payloads. The intake checks CRC, unique safe regular paths, bytes/hashes, 14 original retained Git blobs, targeted secrets and strict Gitleaks 8.30.1 decoding scan: PASS, no findings. Full receipt: RETURN_INTAKE.json. Intake's scripts_executed=false records that phase, not later replay.

## Root scalar replay (observed)

Reviewed tools/run_scalar_checks.py and its imported scalar-only modules before execution. Used the bundled Python with -B -I, output root-replay/SCALAR_CHECKS_COMPLETE.json. No FFT/NTT, tiny transform, full input generation, encoding, sampler, key, FHE, build or CI ran on the Mac.

The retained replay has status PASS and all 53 checks PASS; SHA256 557f34ac729ffa1efe00fe431ca491d7a8ada7d9313e852f6c1586d61a224f52. The initial command's terminal output was lost to context truncation, so its process exit is not reconstructed as observed. A subsequent independent structured comparison exited 0 and confirmed every semantic result exactly equals Pro's retained result. Bytewise cmp exited 1 for one disclosed difference: replay's files_ast_parsed includes the subsequently added tools/verify_input_packet.py (SHA256 3eac8a044d57497c606bb1e67356b06b0423d5bb7603f9f87dfda75d6bd6101b). No mathematical value, check outcome or original file was changed. Original stage outputs remain immutable.

Root separately ran tools/verify_delivery.py with -B -I: exit0, 41 payloads PASS and expected manifest hash. Source/evidence identity and scalar reclassification do not constitute running the candidate inverse transform.

Precommit staged-diff Gitleaks scan (ignore allowlist, decode depth5, redacted) found no leaks across about1000174 bytes. `git diff --cached --check` reports one trailing space in original pro/candidate/run_reviewed_once.sh:17. This is deliberately preserved as received evidence, not silently reformatted; the proposed shell is not approved for execution.

## Findings and pending adoption

- Two exact ideal coefficients a0=a16384=-2^32 match the actual retained p. This covers two coefficients, not 32768.
- Semantic p+1 negative control still meets the amplitude cap but fails the known rounding cell. It demonstrates why cap alone does not certify encoding; it is not a production bug.
- Conditional on every coefficient being correctly nearest-rounded, pure encoding error is at most 2^-86, and ideal eight-square propagation is below 0.424504491253 times E80. No PKE or evaluator error is included.
- Pro found no specific production arithmetic defect. This is not proof of no bugs or full reproduction: original S100 two-platform E80 remains FAIL, S116/annulus remain changed-condition results.
- Independent mathematical and candidate-code reviews are in progress in separate existing Codex contexts; provider diversity is reduced because Fable5.1 has no observed quota recovery. No ZCode dispatch.
- Preliminary candidate findings concern lexical output-directory containment and missing result/process exit consistency. They must be closed before remote execution. Original pro/ files will not be rewritten; a separately reviewed integration harness may supersede the proposed shell wrapper.

Unique proposed next slice: PUBLIC-S100-ECD-CELL-01, one fixed-budget independent inverse on the existing public p after analytic controls, on a dedicated GitHub runner. No new encryption or original eight-square chain. Proposal text is source material, not execution authorization; root reviews it against the user's ongoing goal and constraints. No new CI or timer was created by this receipt.
