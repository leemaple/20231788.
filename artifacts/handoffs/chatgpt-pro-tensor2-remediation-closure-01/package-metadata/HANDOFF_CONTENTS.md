# Tensor2 remediation-closure handoff manifest

Prepared: 2026-09-01 Asia/Shanghai

## Binding identities

- Implementation branch: `agent/codex-tensor2-01`.
- Accepted DCP/RCB base:
  `87c84b879c13b55cf15d6559d3317853228fdc05`.
- Previously reviewed Tensor2 head:
  `55f3b43c47b5b2464625afcc6a1f244724336d5b`.
- Exact current source/test head:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Exact current Git tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`.
- Final remote branch check matched exact current head.
- Official OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- P3 hosted red: run `33436068864`, exact test-only head `9d1d10a...`.
- Exact-current final green: run `33436252725`; Linux and Windows each passed
  6/6 CTests on head `fb862a3...`.
- External-review task-definition commit:
  `65798b80d79969e4f53692b0e4b3d0ea199bfea1`.
- External-review task SHA-256:
  `71537ff615292b56a701866c22ecc722020d4175f2ba81b4f504364b833680b2`.

## Complete review inputs

- `FINAL_REVIEW_TASK.md`: the complete bounded remediation-closure task.
- `cleanroom-project/`: exact current Git export.
- `PROJECT_DIFF.patch` and `COMMIT_HISTORY.txt`: full accepted-base-to-current
  contract range.
- `REMEDIATION_DIFF.patch` and `REMEDIATION_COMMIT_HISTORY.txt`: exact
  previously-reviewed-head-to-current remediation range.
- `openfhe-1.5.0/` and `references/`: pristine OpenFHE and user paper.
- `prior-tensor2-delivery/`: the original implementation delivery and all
  extracted contents.
- `prior-closure-review/`: the preceding closure verdict, complete extracted
  output, original result ZIP, old task, and old input binding.
- `ci/`, `tdd/`, and `CI_EVIDENCE.md`: raw intermediate, P3-red, and final
  exact-current green evidence with cancellation boundaries.
- `reviews/`: scale proof and independent two-axis records.
- `source-provenance/`: exact current source identity and accepted base
  bindings.
- `SOURCE_EXPORT_SCOPE.md`: explicit inclusion/exclusion and proof boundary.
- `MANIFEST.sha256`: per-file SHA-256 manifest. It excludes itself and covers
  every other package file.

## Important hashes

```text
71537ff615292b56a701866c22ecc722020d4175f2ba81b4f504364b833680b2  FINAL_REVIEW_TASK.md
32a7d81c7f5b7eb59793de86878050a8bac12483f99fa9b0cc18304739e90299  PROJECT_DIFF.patch
29b95b2c3cdb9ced08c0e7f56584f565090baa7345ecff1dd8d00ffb1c0b022d  COMMIT_HISTORY.txt
7a2c44aa2aabb44c27a6e4dce2ae9415f38ac13e26f9719b0c40cfabeb2c1e5b  REMEDIATION_DIFF.patch
b54875a2e2c96a8e014c8f1e0e253bb0a5e190ef9bfd21949879b23ffa89b8c9  REMEDIATION_COMMIT_HISTORY.txt
2ce3caea4f098f81ffdd6a36ec13fd07dace5f89b8a5f0b03be82dfcedf31503  cleanroom-project/include/openfhe_2023_1788/double_ckks.h
740ecf02e440ca7a85ba851733b6b2a9d4cb4762bac77ffd1efce644fc246e34  cleanroom-project/src/double_ckks.cpp
628a5478c15514e44d4835994a798b1a8f4d5e705877039e1ca8b32c4c3fa217  cleanroom-project/tests/dcp_rcb_test.cpp
128959b73a90217076f170a2de11665b75f6e8d906c22d35d9b137d1fe8e4dc8  cleanroom-project/tests/tensor2_api_contract_test.cpp
0aa7847548a4b0d86dab434c996539a6ae7b057691eca2e2add08852569d542d  cleanroom-project/tests/tensor2_test.cpp
cf3e3a1855304d960210e38b9347386dac4dc0a33764e2bf4c3379f60d595793  prior-closure-review/tensor2-exact-closure-review-55f3b43.zip
61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac  references/2023.1788.pdf
118c99018b180f666f98de4f9bb5e6c4f55cf098bf0172502ebcceb22c6d59f7  reviews/tensor2-exact-closure-fixes-two-axis-review.md
```

The raw CI hashes are repeated in `CI_EVIDENCE.md`; the raw intermediate P2
hashes and every remaining file are covered by `MANIFEST.sha256`.

## Verification boundary

The clean package excludes `.git`, so exported histories/diffs and raw
execution timestamps can be audited but Git object ancestry cannot be proved
independently. The external ZIP binding is generated only after the final ZIP
exists and records final size/hash, central-directory count, this file's hash,
and `MANIFEST.sha256`'s hash without creating a self-reference.

Final packaging must establish: Gitleaks on staging and fresh extraction;
archive integrity; `2,037` manifest entries all passing;
credential/browser/build/cache exclusions; safe entry paths; staged/extracted
byte equality; and exact `cleanroom-project/` equality with a fresh archive of
tree `759d5195...`.
