# Root execution ledger — documentation checks only

2026-09-08 Asia/Shanghai. Fixed project runtime source `a4b815a733efe81897325e2a8e4c826a4ebfa439`; OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`. Authoring and checking documentation does not change that runtime identity.

## Actually executed

1. Git source-state checks and incremental documentation commit/push, recorded in Git history. `git diff a4b815a -- src include tests CMakeLists.txt .github/workflows` remained empty. No merge or default-branch modification.
2. Official-source and handoff integrity checks, bounded secret scans and ZIP verification: exact commands, file counts, bytes and hashes are in `PACKET_RECEIPT.json`. These are packaging checks, not numerical tests.
3. Source-only lexical inventory of base parameter setters and independent source readings; see `ROOT_COVERAGE_CHECKLIST.md`, `ROOT_RANDOMNESS_MAP.md` and the other review maps. No compiler/runtime claim follows from a source read.
4. Poppler rendered user-supplied paper pages 4, 5 and 13 at 115 dpi; root visually inspected all three. After Pro's 21:54 CST progress mentioned a Theorem 4.8 normalization inconsistency, root additionally rendered page 8 at 130 dpi and visually checked that entire page. This follow-up is not claimed as a blind independent discovery. User PDF and packet PDF SHA-256 matched. See `ROOT_PAPER_CROSSCHECK.md`. No new PDF report was generated or sent.
5. Root read and then ran the new standard-library script exactly once:

   ```sh
   /Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -B -I coordination/parameter-atlas-20260908/check_frozen_parameters.py
   ```

   Exit code 0; result `ROOT_STATIC_PARAMETER_CHECK.json`, `check=PASS`. Script first verified the complete input ZIP hash and current `repeated_mult2.cpp` bytes against that packet, then extracted literal constants. It checked all frozen Q/P root congruences and root powers, pairwise coprimality, exact factorized scale recurrences for eight rounds, and 32 base / 8 CKKS-disabled setters. It did not perform a primality certificate check. Exact scalar values are represented by power-of-two numerators and integer denominator factors; a 50-digit Decimal calculation is used only for a human-readable scale ratio.

   S100: Q bit length 620, QP bit length 680. After eight rounds, exact scale / recorded scale − 1 is approximately `0.0003648503896039228505883078640327851759075371099`.

   S116: Q bit length 652, QP bit length 712. Corresponding ratio − 1 approximately `0.0000152026932966911198087405725575424662676971042`.

   These nonzero ratios demonstrate why the actual rational scale and the power-of-two compatibility metadata must be distinguished. They are not measured ciphertext errors, evidence that the current decoder uses the wrong scale, or proof of precision/security. The current project has an exact-receipt decoding path; the final atlas must explain it.

6. Browser Pro was submitted once and inspected read-only while live. A later optional `captureScreenshot` call timed out with `Page.captureScreenshot`; no screenshot was produced by that call. Subsequent `pageInfo` and semantic snapshots succeeded at the same saved conversation and showed continuing document/source work with `Stop answering`. Root did not refresh, stop, resend, or switch the active model. This observation failure does not establish a failed Pro computation.
7. After extended waiting, root opened the same saved conversation in a separate read-only status tab (URL fragment `#atlas-status-readback`, target `4B28A08EE33551937EDF8FDEF604D81F`) while preserving the original target. Initial loading UI was visible, but a subsequent `pageInfo` timed out in `Runtime.evaluate`; root closed only that temporary tab and switched back to original `21FBD0E06002EFFCAEBD2147941359A0`. Its semantic snapshot immediately succeeded and still showed `Stop answering`. No original-page refresh, task resubmission, stop, or model switch occurred. This readback attempt did not yield a terminal server-status confirmation or deliverable.
8. Read-only `gh run list --repo leemaple/20231788. --branch codex/parameter-atlas-20260908 --limit 10 --json databaseId,headSha,name,status,conclusion,url` returned `[]`: no Actions runs were listed for this documentation branch at this checkpoint. This is a query result, not a test result.
9. A second isolated status-readback tab (`FE3A86665293AFC7EBB698DC9C45A280`) was allowed to load for55seconds before inspection. Both Runtime/pageInfo and subsequent semantic snapshot checks timed out; explicitly selecting that temporary tab did not resolve it. Root closed only that tab and returned to the original, whose DOM read again succeeded immediately and still showed the live-generation control. No refreshed original page or new Pro prompt was used.
10. Root authored `docs/parameter-atlas/README.zh-CN.md` as an explicitly labeled **working introduction**, based on the retained source maps, paper checks and static results. It is not presented as the Pro main atlas. A bounded standard-library Markdown-link check verified16 local links and found0 missing targets; `git diff --check` passed. No FHE result follows from document/link validation.

## Explicitly not executed

No OpenFHE build, compiler check, PRNG sample, random-prime/root search, parameter precomputation, key generation, encryption, decryption, FFT/NTT, FHE circuit, new precision/performance/security experiment, workflow dispatch/rerun, author contact, production patch, automation change or Telegram delivery. Historical PASS/FAIL results remain historical, with their original source/profile/input/run identities.

## Recovery and repeatability

The scalar script opens its output exclusively and will refuse to overwrite it. Review the retained result rather than rerunning it automatically. If a separately justified rerun becomes necessary, use a new explicitly named output path through a reviewed script change, preserving the first result. Root source maps were completed independently of the active Pro draft; reconcile the returned main document against them after its terminal response.
