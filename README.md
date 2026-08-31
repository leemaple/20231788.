# OpenFHE 2023/1788 clean-room implementation

This branch is a greenfield implementation of the `t=2` Double-CKKS multiplication method from IACR ePrint 2023/1788 for official pristine OpenFHE 1.5.0.

The destination repository's previous implementation and every related local code tree are quarantined and are not inputs. Development begins with paper-derived specifications and red-first independent-oracle tests.

## Live project state

- Active/default branch: [`cleanroom/reimplement-mult2-20260831`](https://github.com/leemaple/20231788./tree/cleanroom/reimplement-mult2-20260831)
- DCP/RCB implementation: isolated branch [`agent/codex-dcp-rcb-01`](https://github.com/leemaple/20231788./tree/agent/codex-dcp-rcb-01), exact head `87c84b879c13b55cf15d6559d3317853228fdc05`, has independent coefficient-oracle TDD evidence; its immediately preceding source-identical commit passed the strict build plus 1/1 CTest on Linux/GCC and Windows 2022/MSYS2 MinGW64 in [Actions run 33406650125](https://github.com/leemaple/20231788./actions/runs/33406650125), while exact-current-commit run `33411494861` is still pending. It remains unmerged until its exact-current-commit ChatGPT Pro remediation review and Windows ZCode/Zima review gates close.
- Tensor2 next slice: isolated branch [`agent/codex-tensor2-01`](https://github.com/leemaple/20231788./tree/agent/codex-tensor2-01) is fast-forwarded to the same green base; the bounded task and Codex paper/OpenFHE preflight are recorded, but no Tensor2 production implementation or test result is claimed yet.
- External work: ChatGPT Pro completed the exact-current DCP/RCB remediation review, approved retaining slot validation, found no production defect, and returned one remaining deep-metadata-snapshot test gap. The equivalent test-only commit `87c84b879c13b55cf15d6559d3317853228fdc05` is now in its exact Linux/Windows Actions gate. The existing Windows ZCode/Zima task is preserved rather than duplicated while the shared weekly quota is critically low.
- Continuity: an active long-running Codex Goal advances the project across turns; the daily 07:00 Asia/Shanghai automation independently produces and delivers the PDF status report.
- Progress evidence: [`coordination/CONVERSATIONS.md`](coordination/CONVERSATIONS.md)
- Paper/OpenFHE API review: [`coordination/CODEX_API_REVIEW.md`](coordination/CODEX_API_REVIEW.md)
- Confirmed TDD seams: [`coordination/TEST_SEAMS.md`](coordination/TEST_SEAMS.md)
- Independent oracle and red/green evidence plan: [`coordination/INDEPENDENT_ORACLE_PLAN.md`](coordination/INDEPENDENT_ORACLE_PLAN.md)
- Shared ZCode quota and allocation log: [`coordination/ZCODE_QUOTA.md`](coordination/ZCODE_QUOTA.md)
- Integration gates: [`coordination/INTEGRATION_REVIEW_CHECKLIST.md`](coordination/INTEGRATION_REVIEW_CHECKLIST.md)
- Git checkpoint policy: [`coordination/GIT_CHECKPOINT_POLICY.md`](coordination/GIT_CHECKPOINT_POLICY.md)

All coherent project changes are committed in small checkpoints and pushed immediately. Agent work stays on isolated branches until reviewed; shared history is never force-pushed.
