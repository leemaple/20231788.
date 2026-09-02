# Relin2 Codex fallback BV nonzero-digit B-count TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV `digitSize>0` evaluation-key B decomposition-count guard is
green on Linux and Windows; BV entry basis/format and all Relin2 arithmetic
remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `34678817fe863d2759e07b6358ffff2feaaf9d02` /
  `2f4edf16e8744bf482d4b1c4e146eee8f9e03c08`;
- parent: `86263e6d87899309981a10a0d108a07fdefb2251`;
- source diff: only `CMakeLists.txt` and `tests/relin2_test.cpp`, with one and
  172 insertions respectively;
- run/jobs: `33595207012` / Linux `100137152803` / Windows
  `100137152400`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33595207012`;
- result: both warning-clean project builds and Relin2 public-API builds
  passed; exactly the new twenty-second test failed, producing `21/22` on each
  platform with the old scaffold exception;
- evidence branch/commit/tree: `evidence/relin2-hosted-3467881` /
  `904a91c369bc8a9048e1b48800ffb8a8a13e0867` /
  `7f00b9807c655c5363d1af832b12a58b5d15b0be`;
- 19-entry manifest SHA-256:
  `0ba6c8d53afca16fb042bc13ac38d3b526b153885869a0bd4ca2ffd0d194d75a`.

The fixture used the complete-`Q` tower MSB values `{35, 31, 30, 31}` and
`digitSize=10`. It independently derived the expected A/B decomposition count
as the integer-ceiling sum `4 + 4 + 3 + 4 = 15` and pinned both the parameter
manifest and count.

A durable valid-key positive control first reached the existing scaffold with
A and B lengths 15. The negative control then changed only B from 15 entries to
14 while leaving A at 15. The exact required diagnostic was
`DoubleCKKS: Relin2 evaluation key BV B vector length mismatch`.

Both platforms observed the same wrong-exception red:
`DoubleCKKS: Relin2 is not implemented`. Tests 1 through 21 remained green;
Linux reported 0.18 seconds and Windows 0.37 seconds.

## Green boundary

- source commit/parent/tree: `3015f3f14e4bfc457bb702d5f1050a99b7697d90` /
  `34678817fe863d2759e07b6358ffff2feaaf9d02` /
  `51f150ca46d8ee068e8b5534710c330441ffc54a`;
- source diff: only `src/double_ckks.cpp`, six insertions and three deletions;
- run/jobs: `33596456570` / Linux `100140773937` / Windows
  `100140773851`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33596456570`;
- result: warning-clean project build, Relin2 public-API build, and exactly
  `22/22` runtime tests passed on both platforms in 0.19/0.40 seconds;
- evidence branch/commit/tree: `evidence/relin2-hosted-3015f3f` /
  `d429036e2614e4f77a25572a7feb9347993217f9` /
  `e55807f686e7df9526b1ea1507794a01afd10708`;
- 19-entry manifest SHA-256:
  `ecd93c0d25381e3855b9b3a9b98c01e37e95486e7a0946ebd300c4b5bfd7aba7`.

The production change renamed the existing A-specific local decomposition
count to a shared vector count. It retains the accepted complete-Q
integer-ceiling calculation, validates A first, then compares only B against
the same count and emits the existing exact B diagnostic. It does not introduce
a second formula, validate basis/format, perform arithmetic, add a catch block,
or change any test, public header, or CMake file.

The green run executed both the full generated-key positive control and the
malformed-B negative control. Immediately after each production `Relin2` call,
the test completed Tensor, deep-metadata, A/B polynomial, cache-map/vector/key-
pointer, context, and tag invariance checks. The final RAII check restored the
initially empty global evaluation-key cache.

## Integrity and decision

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
After each hosted run, independent Spec, TDD, and Delivery/evidence reviews
returned `PASS` against the frozen evidence bytes.

The BV nonzero-digit B decomposition-count red/green boundary is accepted and
closed. The next isolated TDD boundary is BV evaluation-key entry basis. It
must begin from `3015f3f14e4bfc457bb702d5f1050a99b7697d90`, add a durable
valid-key positive control before changing only one key entry's basis, and must
not combine entry format, ciphertext raising, or Relin2 arithmetic.
