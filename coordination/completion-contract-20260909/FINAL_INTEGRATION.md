# Final root integration gate

Checkpoint: 2026-09-09 05:54 Asia/Shanghai, before commit/push.
Parent HEAD: `72db6ef37bb23f480b25c3e4219e789344d20f04`.
Branch: `codex/public-s100-ecd-cell-20260909`.

The independent review's final addendum is PASS: A1/A2 resolved, no remaining
integration finding. Its SHA256 is
`d815e142575c787a2a245197dfb7a30a9e866b3c4a1e1b95d33d8c7370fef563`.

Final adopted document hashes:

| File | SHA256 |
| --- | --- |
| CHECK_AND_HANDOFF.zh-CN.md | c866c3aac040d49d0e6cce2c64ed1b22e6a6c38cc25dbb18be012b8969a60209 |
| REPRODUCE.zh-CN.md | ba684e4c2925bbc8a49e6b085c01bab1868867fe3f69282dec9c7ad562dbc790 |
| ADOPTED_RESULT.zh-CN.md | 295c1511cda4683790584a425485ab53c71c9f3429d161e71067d876b0599be5 |

Root's inspected-checker replay is retained in
[ROOT_REPLAY.json](root-replay/ROOT_REPLAY.json): 33 bounded scalar/text checks,
document RED exit1 then GREEN exit0, 12 rejected text mutations. These are not
new FHE tests. Exact Pro candidates were verified before root-owned link and
qualification additions; final documents are not claimed byte-identical to them.

The final `check_docs.py --root .` invocation returned PASS, 14 fields, no errors.
Earlier final-content checks resolved all 40 local Markdown links across the two
entry documents and adopted result, and verified all 30 Pro manifest payloads
unchanged. The final review addendum does not change those three documents or
the immutable Pro payloads. `git diff --check` passed. Read-only diff against
HEAD for src/include/tests/diagnostics/CMakeLists.txt/.github returned no paths.

[FINAL_SCAN_RECEIPT.json](FINAL_SCAN_RECEIPT.json) retains the 60-file targeted
and strict Gitleaks8.30.1 scan, with no findings. That receipt predates the final
review addendum and this checkpoint; the final staged diff must additionally
pass a strict scan before commit. No source ZIP is newly uploaded by this step.

Final staging exposed eight whitespace warnings inside the immutable returned
`pro/docs/01-completion-labels.patch`: blank unified-diff context lines retain
their required leading space. That evidence file is intentionally not rewritten.
The staged whitespace check excluding exactly that patch passed; the previously
reported unstaged check did not cover new files. The staged diff strict scan
(same Gitleaks8.30.1 flags as the retained scan receipt, sanitized environment,
single Go worker) returned exit0 with no findings. This explicit exception does
not concern either adopted root Markdown file or production code.

Publication scope: only two root entry documents and this completion-contract
coordination directory. No production, input, parameter, truth, threshold,
workflow or historical-result modification. No scientific rerun or new agent
submission. A successful push must be confirmed by exact remote branch identity;
this pre-publication record itself does not claim that push occurred.

This closes only the documentation/adjudication boundary once published.
Original S100 E80 remains FAIL on both platforms and the full reproduction goal
is not complete. No waiver or universal future-work prohibition is adopted.
