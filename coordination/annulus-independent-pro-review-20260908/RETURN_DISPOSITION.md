# Independent Pro terminal return — root disposition

Recorded 2026-09-08 13:08 Asia/Shanghai. Root baseline branch `codex/s100-annulus125-20260908`, HEAD `932d92263f71d6b89ac34ec714b93282c34023f9`. The following is root acceptance of a bounded review, not acceptance of full paper reproduction or proof of absence of bugs.

## Terminal response and immutable intake

- Conversation: [独立复现审查](https://chatgpt.com/c/6a9f8aa6-eb78-83ec-aeb9-879aa5bf1b2e). Submitted once 12:10:14.731; terminal observed 13:00:00.747. UI reported `Worked for 48m44s`, `6 / Pro`; highest visible Power 5 of 5 was checked before submission. Backend identity is unattested. No interruption, refresh, repeated submission, or premature reassignment.
- One download click at `2026-09-08T05:00:39.662Z`; original ZIP 139,928 bytes, SHA-256 `badeeff902d1bdb36ae89e17ef33d845895eaebb17fd7d81cb87157cc97fd289`. All 44 regular files / 361,832 expanded bytes retained unchanged in `pro/`. The original ZIP remains under the ignored task-specific `artifacts/handoffs/annulus-independent-review-return-20260908/` directory.
- [RETURN_INTAKE.json](RETURN_INTAKE.json): safe paths, CRC, source/task/input binding, manifest closure and hashes PASS; targeted and Gitleaks 8.30.1 scans found zero findings. Its `external_code_executed: false` records the intake checkpoint, before the subsequent reviewed execution below; it is not a claim about all later work.
- [ROOT_SCALAR_REVIEW.md](ROOT_SCALAR_REVIEW.md) records a separate Codex first-pass mathematical/code review before reading the Pro verdict. Requested model identity is `requested-unverified`, not Fable. Fable's prior definitive balance failure has no observed recovery; no repeated probe or ZCode dispatch was performed.
- After terminal evidence and the archive had been retained, Ego taskspace 204 was closed at approximately 13:08 with `done: true`. The conversation URL is preserved; no browser agent remains thinking or waiting.

## Root execution, distinct from Pro-reported execution

Root read the entire returned `checks/independent_scalar.py` and its reproduction instructions before running it. It uses only standard-library scalar arithmetic and the committed raw TSV/end receipt. No returned package setup, shell hook, network, FFT, OpenFHE build, decryption, or new encryption was executed.

One new root scalar replay used Python 3.12.14, a single process, 230 Decimal digits and a 30-second subprocess timeout. Output existence was checked first; the original returned results were not overwritten. Working directory was the active clean-room worktree.

```bash
/Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -B -I \
  coordination/annulus-independent-pro-review-20260908/pro/checks/independent_scalar.py \
  --tsv coordination/s100-annulus125-20260908/experiment-evidence/sample/raw.tsv \
  --end coordination/s100-annulus125-20260908/experiment-evidence/sample/program-end.json \
  --precision 230 \
  --output coordination/annulus-independent-pro-review-20260908/ROOT_REPLAY_230.json
```

Actual child process exit 0; elapsed scalar time **1.2316961670294404 seconds**. Root separately asserted JSON `status == "PASS"` and all five gates true: checker exit 0 alone means analysis completed and does **not** imply numerical PASS. Result [ROOT_REPLAY_230.json](ROOT_REPLAY_230.json), SHA-256 `21984b7c87ee1552d61fdc55cd256319312b65b5accc88c53f9f58f2d149244a`. Root compared every JSON field with `pro/results/independent_230.json` after removing only the locally measured elapsed-time field: exact equality PASS.

All 16,384 rows replayed; raw SHA-256 `b0d17e0c2c819b1b59921fd7f8020a5265ae6c70e3b5fa87152a63abfbe4b5fd`. Maximum final complex error is `1.46231410388433711339e-25`, slot 56; absolute precision approximately **82.49995 bits**. Maximum same-slot relative error corresponds to approximately **73.30263 relative bits**, also slot 56. No second encrypted sample was generated.

Pro separately reports two-precision new/old scalar replays, 13 boundary groups, and historical/current receiver regression challenges in [EXECUTION_LEDGER.md](pro/EXECUTION_LEDGER.md). Root retained these logs and inspected the mathematical findings, but did not rerun those complete groups in this intake. Pro's old replay exit 0 means successful analysis of retained numerical **FAIL**, not a new PASS. The literal-paper counterexample and historical receiver regression deliberately report RED; neither is a new production C++ failure. First-pass freezing/read ordering is supported by the returned local ledger, not an independently trusted timestamp or a claim of perfectly blind context.

## Findings and disposition

| Finding | Root disposition |
| --- | --- |
| F01: new annulus PASS repairs original S100 | Reject that claim. Accept only the one predeclared Linux vector/key/noise observation at source `03f37b6`. Preserve both original near-unit FAIL results. |
| F02–F03: historical receiver agreement / LF / CRLF false acceptance | Already fixed in the integrated receiver with fail-first controls; Pro independently reproduces the historical/current distinction. No new production precision repair. |
| F04–F05: stdout synchronization / observer slot-order controls | Previously fixed and backed by retained remote RED/GREEN evidence. Do not reopen or repeat the encrypted sample. |
| F06: printed normalization/sign defects justify a production patch | Unsupported. Existing production uses the normalized `S²/(d q)` path; paper-expression counterexamples are not a new production RED. |
| F07: Lemma 4.4 intermediate carry claim | Preserve the counterexample and unresolved general proof boundary. It does not disprove the entire final bound. Owner: Codex, pending a concrete stronger proof/counterexample rather than a speculative patch. |
| F08: cross-precision checks constitute formal certification | Reject. Numerical consistency is conditional evidence, not certified transform/error bounds. |
| F09–F11: missing serialized observations, global one-shot history, cache/runtime attestation | Retain explicit evidence limits. Some properties are inspected-source/runtime assertions, not independently replayable from the TSV. Binary SHA is a recorded receipt; the binary itself is not retained. Current tag/event checks are not a universal history theorem. |
| F12: 16,384 slots imply independent repeated trials | Reject. One input vector and one actual encryption/key/noise sample. No 1,000-run requirement. |
| F13–F14: exact Table 3 / deployment security | Unfinished. No same-source performance/statistical reproduction or deployment-security certification is established. Do not label full goal complete. |
| F15: all observed improvement is causally due to radius | Reject the causal overclaim. The derivative supports reduced absolute amplification; old and new samples use different keys/noise. Relative condition number of nonzero `x^256` remains 256. |
| F16: trimmed review packet lacks `check_receiver_agreement.py` referenced by compile workflow | Verified packet limitation, not repository defect: `git ls-files` confirms the file is tracked in the active repository. Pro supplied independent three-case receiver checks but did not reproduce the entire CI workflow from its trimmed packet. No redundant resubmission is needed for this bounded acceptance. |
| F17: receiver docstring says `UNEXECUTED` | Verified stale descriptive comment. Deferred as a nonfunctional documentation cleanup; actual dated execution evidence takes precedence. Do not rewrite immutable Pro originals or trigger CI solely to change this comment. Owner: Codex at the next warranted receiver edit. |
| Root review: analysis CLI exit 0 can accompany recorded numerical FAIL | Addressed at root caller boundary by explicitly asserting JSON status/gates. Immutable returned checker unchanged. |

The new scalar checker uses an independently written factor identity for propagated error, not the supplied receiver's recurrence. Nevertheless TSV rows contain recorded errors, not raw decrypted endpoints/ciphertexts: independence of scalar calculations must not be described as independent re-decryption or a formal observer proof.

## Scientific decision and remaining boundary

Accept the Pro recommendation **not to invent a production repair from these results**. No new concrete production arithmetic defect or admissible repair mechanism has been demonstrated. Root rechecked that `git diff 223667e82b67c4758a56bd745f264110dd3b9619 HEAD -- src include` is empty.

For the two retained original near-unit S100 samples, preserving the observed fresh phase while setting subsequent residual `A8=0` still leaves propagated error `I8` above the fixed threshold. That is a valid sample-specific counterfactual, not a theorem excluding every future method. The new smaller-radius vector reduces absolute error; the old FAIL cannot be relabeled or erased. S116 stays a separate changed-parameter two-platform PASS.

This completes the requested renewed Pro-led comprehensive reassessment and its bounded intake/review, **not the full reproduction objective**. Exact Table 3 experiment provenance (version-bound input generation, noise/API/scales/measurement/statistical protocol) is still missing from the already investigated material. The user has declined author contact; do not ask again or contact authors. Current evidence does not support an honest remaining-time estimate for exact numerical/performance matching. More random repetitions do not supply that missing provenance.

Next checkpoint owner: Codex. Preserve/push this completed review and user-facing explanation; leave full-goal status incomplete. Any further engineering must name a new concrete testable defect or genuinely new primary-source provenance. Do not manufacture a follow-up Pro prompt, generic search, new ciphertext run, recurring timer, or a claim of ongoing external thought merely to keep a task active. This turn made concrete progress and is not a repeated blocked-state audit.

No production/test/CI changes, dispatch/rerun, default-branch merge, author contact, heavy Mac computation, automation change, or PDF/Telegram repeat was performed for this intake. Required future heavy experiments, if justified, remain on Windows or GitHub runners.

Pre-commit checks: all 44 returned files are present in the Git index (including the 12 intentionally retained `.log` files) and local bytes equal the original ZIP members; 45 local links in four root-facing documents exist; four Bash fences pass syntax-only `bash -n` without dispatching their commands. `git diff --cached --check` passed. Gitleaks scanned the staged change (~432,852 bytes before this summary sentence), exit 0 / no findings. Only evidence, intake tooling and documentation are staged.
