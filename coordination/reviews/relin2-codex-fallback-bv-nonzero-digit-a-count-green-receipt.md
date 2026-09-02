# Relin2 Codex fallback BV nonzero-digit A-count TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV `digitSize>0` evaluation-key A decomposition-count guard is
green on Linux and Windows; the corresponding B count, BV entry basis/format,
and all Relin2 arithmetic remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `36d88dd731ec1e8c48cb90ca36e6ddc6f91c6486` /
  `42e3a9f056db4b30106137e90d1497e6c568b436`;
- parent: `3411d65e752272a70d6dc147e8e7239014221196`;
- source diff: only `CMakeLists.txt` and `tests/relin2_test.cpp`, with one and
  172 insertions respectively;
- run/jobs: `33592368406` / Linux `100128793337` / Windows
  `100128793139`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33592368406`;
- result: both warning-clean project builds and Relin2 public-API builds
  passed; exactly the new twenty-first test failed, producing `20/21` on each
  platform with the old scaffold exception;
- evidence branch/commit/tree: `evidence/relin2-hosted-36d88dd` /
  `6bb025e5a5901ff6cf0295ab63b49348c840cd82` /
  `0e2f553ce8896d47c2e2a0b5658c0c68fa92fc3b`;
- 19-entry manifest SHA-256:
  `17a324b74e796849d18360e785e263179ce1eeffc93a3759f27b6f2970a15505`.

The fixture used the complete-`Q` tower MSB values `{35, 31, 30, 31}` and
`digitSize=10`. The expected decomposition count is the integer-ceiling sum
`4 + 4 + 3 + 4 = 15`. The test derived that value independently from the bound
complete-`Q` parameters and pinned both the parameter manifest and the count.

A durable valid-key positive control first reached the existing scaffold with
A and B lengths 15. The negative control then changed only A from 15 entries to
14 while leaving B at 15. The exact required diagnostic was
`DoubleCKKS: Relin2 evaluation key BV A vector length mismatch`.

Both platforms observed the same wrong-exception red:
`DoubleCKKS: Relin2 is not implemented`. Tests 1 through 20 remained green.

## Green boundary

- source commit/parent/tree: `86263e6d87899309981a10a0d108a07fdefb2251` /
  `36d88dd731ec1e8c48cb90ca36e6ddc6f91c6486` /
  `70230784f1a1aa8ec54b9956f4af4b2c6a1df56e`;
- source diff: only `src/double_ckks.cpp`, 11 insertions and no deletions;
- run/jobs: `33593842748` / Linux `100133145890` / Windows
  `100133145698`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33593842748`;
- result: warning-clean project build, Relin2 public-API build, and exactly
  `21/21` runtime tests passed on both platforms in 0.19/0.45 seconds;
- evidence branch/commit/tree: `evidence/relin2-hosted-86263e6` /
  `d92695f59136ade6fdf84fd2f040459f3c9846f0` /
  `6c464a025a07ce4cc9f1e6919d57819da66e3262`;
- 19-entry manifest SHA-256:
  `cc50a101fed8fac32bebe4d11680b5a9086771c91a3539c25066e2a130b33abb`.

For BV with nonzero digit size, the production change sums
`(towerBits + digitSize - 1) / digitSize` over the bound complete-`Q`
parameters and compares only the concrete key's A-vector length. It reuses the
accepted exact diagnostic. It does not validate B, basis, format, or perform
Relin2 arithmetic, and it adds no catch block or generic abstraction.

The green run executed both the full generated-key positive control and the
malformed-A negative control. Immediately after each production `Relin2` call,
the test completed Tensor, deep-metadata, A/B polynomial, cache-map/vector/key-
pointer, context, and tag invariance checks. The final RAII check restored the
initially empty global evaluation-key cache.

## Integrity, external-model handling, and decision

For both red and green, the implementation branch, local remote-tracking ref,
and live remote ref were verified equal after non-force pushes. Each evidence
directory contains 20 files: exactly 19 non-manifest files plus one
`MANIFEST.sha256`; every row verifies and the manifest file set is exact.

The red complete-logs ZIP has 30 unique, unencrypted, path-safe members; the
green ZIP has 31. Both pass `unzip -t`, and their aggregate Linux/Windows logs
are byte-identical to the separately retained logs. Both evidence sets report
zero uploaded GitHub artifacts and passed Gitleaks 8.30.1 plus independent
targeted credential filename/content scans without retaining matching secret
text.

Three independent read-only source reviews passed before the green commit.
After the hosted run, independent Spec, TDD, and Delivery/evidence reviews each
returned `PASS` against the frozen green evidence bytes.

A difficult 13-versus-15 count disagreement was escalated through the terminal
provider. The alias reported model `claude-fable-5`, not a verifiable Fable
5.1 identity, and its answer cited nonexistent paths and an incorrect exception
type. The entire answer was rejected and is not part of this proof. The invalid
receipt is recorded in
`coordination/reviews/fable51-relin2-bv-nonzero-digit-oracle-invalid-receipt.md`.
The accepted count of 15 instead comes from the pinned parameters, independent
integer calculation, official OpenFHE BV decomposition rule, and passing
cross-platform tests.

The BV nonzero-digit A decomposition-count red/green boundary is accepted and
closed. The next isolated TDD boundary is the corresponding BV nonzero-digit
B-vector count. It must begin from
`86263e6d87899309981a10a0d108a07fdefb2251`, run a durable full-key positive
control before changing only B from 15 to 14, and must not combine BV entry
basis/format, ciphertext raising, or Relin2 arithmetic.
