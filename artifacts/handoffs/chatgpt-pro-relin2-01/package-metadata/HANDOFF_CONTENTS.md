# Relin2 clean-room implementation handoff

Prepared: 2026-09-01 Asia/Shanghai

## Binding identities

- Implementation branch: `agent/codex-relin2-01`.
- Exact source/test base:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Exact source tree: `759d5195739684748d5a9664edabe3fa719e1acf`.
- Accepted DCP/RCB base:
  `87c84b879c13b55cf15d6559d3317853228fdc05`.
- Official OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Exact final base CI: run `33436252725`, Linux and Windows each 6/6 CTests.
- ChatGPT Pro Tensor2 closure: `MERGEABLE`, P0/P1/P2/P3 = 0, no patch.
- Relin2 preflight commit:
  `3675db2db6e70577779261482648356004b6b912`.
- Audited task-definition commit:
  `fba47fd4b0786303bf2303e925c95b90e8f848b4`.
- Task SHA-256:
  `9323d631de4c6f6418ba1aa597332206a765846d0510a275df42a7a64b0a7513`.

## Complete implementation inputs

- `IMPLEMENTATION_TASK.md`: the complete bounded engineering/TDD contract.
- `cleanroom-project/`: exact current Git export, including accepted source,
  tests, workflow, project skill, and retained pre-current TDD records.
- `openfhe-1.5.0/`: pristine official implementation platform source.
- `references/`: user paper PDF and extracted text.
- `reviews/relin2-preflight.md`: paper/API/metadata design contract.
- `reviews/relin2-task-audit.md`: resolved audit findings and final PASS/PASS.
- `reviews/tensor2-exact-closure-fixes-two-axis-review.md`: exact-base
  Standards/Spec review.
- `accepted-tensor2/`: exact ChatGPT Pro closure result and independent Codex
  verification record.
- `ci/33436252725/` and `CI_EVIDENCE.md`: raw exact-current hosted evidence.
- `source-provenance/`: exported full diff/history from the accepted DCP/RCB
  base to the exact Relin2 base.
- `SOURCE_IDENTITY.txt` and `SOURCE_EXPORT_SCOPE.md`: identity and exclusion
  boundaries.
- `MANIFEST.sha256`: SHA-256 for every other package file.

## Important hashes

```text
9323d631de4c6f6418ba1aa597332206a765846d0510a275df42a7a64b0a7513  IMPLEMENTATION_TASK.md
4fea56326a1bae2d965be075ba74dea21961ed72bd44232790169d2e3055bf24  reviews/relin2-preflight.md
44347caba4088f781014e3c554c381f610b166f3bd045f11e0f8d34f6c78c30a  reviews/relin2-task-audit.md
61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac  references/2023.1788.pdf
60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae  references/2023.1788.txt
2ce3caea4f098f81ffdd6a36ec13fd07dace5f89b8a5f0b03be82dfcedf31503  cleanroom-project/include/openfhe_2023_1788/double_ckks.h
740ecf02e440ca7a85ba851733b6b2a9d4cb4762bac77ffd1efce644fc246e34  cleanroom-project/src/double_ckks.cpp
628a5478c15514e44d4835994a798b1a8f4d5e705877039e1ca8b32c4c3fa217  cleanroom-project/tests/dcp_rcb_test.cpp
0aa7847548a4b0d86dab434c996539a6ae7b057691eca2e2add08852569d542d  cleanroom-project/tests/tensor2_test.cpp
32a7d81c7f5b7eb59793de86878050a8bac12483f99fa9b0cc18304739e90299  source-provenance/PROJECT_DIFF.patch
29b95b2c3cdb9ced08c0e7f56584f565090baa7345ecff1dd8d00ffb1c0b022d  source-provenance/COMMIT_HISTORY.txt
40e1211eb8437189bf25e85ef2c7b5633a4d55bbfd217c3002d4ba44c443771d  accepted-tensor2/chatgpt-pro/tensor2-remediation-closure-review-fb862a3.zip
```

Every remaining file is covered by `MANIFEST.sha256`.

## Verification boundary

The source package excludes `.git`. It supports exact byte-tree, diff/history,
paper, OpenFHE, CI, and review inspection but does not independently prove Git
object ancestry or absence of history rewriting. The externally supplied final
binding is generated only after the immutable ZIP exists; it records the final
ZIP byte size/hash and internal handoff/manifest hashes without creating a
self-referential archive claim.

Final packaging must establish, and the external binding must record:

- Gitleaks 8.30.1 on both staging and a fresh extraction;
- ZIP integrity and safe entry paths;
- all manifest entries passing;
- no credential, browser-state, Git-metadata, build, cache, or runtime files;
- byte-identical staged and freshly extracted trees;
- byte-identical `cleanroom-project/` and a fresh archive of tree `759d5195...`;
- exact task equality and final package file/entry counts.

