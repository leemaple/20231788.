# Relin2 Codex fallback BV zero-digit B-length TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV `digitSize==0` evaluation-key B-vector-length guard is green
on Linux and Windows; nonzero-digit BV key shape, BV entry basis/format, and all
Relin2 arithmetic remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `c9ba4e4068608d9929de62d2d010636e4d11b45f` /
  `ed724ee6983e2b79112e18dd718a6ec184320652`;
- parent: `fd847ae5005c8eb179e9d5799b78b2d555f4981c`;
- run/jobs: `33588981664` / Linux `100118896875` / Windows
  `100118897067`;
- result: both warning-clean project builds and Relin2 API-contract builds
  passed; exactly the new twentieth test failed, producing `19/20` on each
  platform with the old scaffold exception;
- evidence branch/commit/tree: `evidence/relin2-hosted-c9ba4e4` /
  `16a7fcff755051eb10013a2850d879d94dcd90c5` /
  `aa77078d2cca369a5ca630a429f3b316719090d9`;
- 19-entry manifest SHA-256:
  `8e5e99e439ef196892eeda415c3eb12d2fd282111d24eb7aec1c78a488e7f6bf`.

The red fixture created a real public BV, `digitSize==0` context with four
complete-`Q` towers. Public `EvalMultKeyGen` produced one concrete
`EvalKeyRelinImpl` with A and B lengths four. A durable positive control first
accepted the full generated key and reached the existing scaffold, then the
negative control changed only B from four entries to three. The exact required
diagnostic was
`DoubleCKKS: Relin2 evaluation key BV B vector length mismatch`.

Both platforms observed the same wrong-exception red:
`DoubleCKKS: Relin2 is not implemented`. Tests 1 through 19 remained green.
Linux reported 0.20 seconds and Windows 0.42 seconds for the full CTest run.

## Green boundary

- source commit/parent/tree: `3411d65e752272a70d6dc147e8e7239014221196` /
  `c9ba4e4068608d9929de62d2d010636e4d11b45f` /
  `572925d4819a32eefb258af1ed37b79deb551cc0`;
- source diff: only `src/double_ckks.cpp`, four insertions and no deletions;
- run/jobs: `33590046548` / Linux `100122018402` / Windows
  `100122018678`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33590046548`;
- result: warning-clean project build, Relin2 API-contract build, and exactly
  `20/20` runtime tests passed on both platforms in 0.18/0.37 seconds;
- evidence branch/commit/tree: `evidence/relin2-hosted-3411d65` /
  `555bcb909629991e75d4c7792142942aa578958f` /
  `830fcc2ae006d040b5976be6d6b92320b8b2e0ff`;
- 19-entry manifest SHA-256:
  `cf763d47c5dfea78123d88640df48565473266128c9c8c4a041825ff5adcfb5b`.

The four-line production change runs after concrete-subtype and BV
digit-size-zero A-length validation. It compares the already-cast key's
B-vector length with the bound complete-`Q` parameter count and emits the exact
project diagnostic on mismatch. It does not add a generic helper, catch an
exception, or touch the existing scaffold.

The green run executed both the valid-key positive control and the malformed-B
negative control. Immediately after each production `Relin2` call, the test
completed Tensor, deep-metadata, A/B polynomial, cache-map/vector/key-pointer,
context, and tag invariance checks. The final RAII check restored the initially
empty global evaluation-key cache.

## Integrity and decision

For both red and green, the implementation branch, local remote-tracking ref,
and live remote ref were verified equal after non-force pushes. Each evidence
directory contains 20 files: exactly 19 non-manifest files plus one
`MANIFEST.sha256`; every row verifies and the manifest file set is exact.

The green complete-logs ZIP has 31 unique, unencrypted, path-safe members,
passes `unzip -t`, and contains aggregate Linux/Windows logs byte-identical to
the separately retained logs. The red ZIP has 30 members and passed the same
checks. Both evidence sets report zero uploaded GitHub artifacts and passed
Gitleaks 8.30.1 plus independent targeted credential filename/content scans
without retaining any matching secret text.

Three independent read-only source reviews passed before the green commit.
After the green hosted run, independent Spec, TDD, and Delivery/evidence reviews
each returned `PASS` against the frozen evidence bytes.

The BV digit-size-zero B-vector-length red/green boundary is accepted and
closed. The next isolated TDD boundary is the BV nonzero-digit A-vector
decomposition count. It must begin from
`3411d65e752272a70d6dc147e8e7239014221196`, add a durable valid-key positive
control before changing only the A decomposition count, and must not combine
the nonzero-digit B length, entry basis/format, ciphertext raising, or
arithmetic.
