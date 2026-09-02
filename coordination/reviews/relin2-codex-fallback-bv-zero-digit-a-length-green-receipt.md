# Relin2 Codex fallback BV zero-digit A-length TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the BV `digitSize==0` evaluation-key A-vector-length guard is green
on Linux and Windows; the corresponding B-vector, nonzero-digit BV key shape,
BV entry basis/format, and all Relin2 arithmetic remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `a3901a6703d463d4ac52b089a4da39995bbde422` /
  `2cc8db125b2105d93409c8d771cc4d3bd355536d`;
- parent: `b9f26db29b53764930798340f4ebe9bed789a323`;
- run/jobs: `33586601051` / Linux `100111946283` / Windows
  `100111946109`;
- result: both warning-clean project builds and Relin2 API-contract builds
  passed; exactly the new nineteenth test failed, producing `18/19` on each
  platform with the old scaffold exception;
- evidence branch/commit/tree: `evidence/relin2-hosted-a3901a6` /
  `386f7755ae1ff5c5a0e779c65ae519b216c686c2` /
  `c91c1ccc0f2e0ad9a4d70d4ba0090495d12b078a`;
- 19-entry manifest SHA-256:
  `87f8e616790c1a9498ee0c2593f8cbdaad784556267db45a860cdb8046028b55`.

The red fixture created a real public BV, `digitSize==0` context with four
complete-`Q` towers. Public `EvalMultKeyGen` produced one concrete
`EvalKeyRelinImpl` with A and B lengths four. A durable positive control first
accepted the full generated key and reached the existing scaffold, then the
negative control changed only A from four entries to three. The exact required
diagnostic was
`DoubleCKKS: Relin2 evaluation key BV A vector length mismatch`.

Both platforms observed the same wrong-exception red:
`DoubleCKKS: Relin2 is not implemented`. Tests 1 through 18 remained green.
Linux reported 0.25 seconds and Windows 0.41 seconds for the full CTest run.

## Green boundary

- source commit/parent/tree: `fd847ae5005c8eb179e9d5799b78b2d555f4981c` /
  `a3901a6703d463d4ac52b089a4da39995bbde422` /
  `dfb46490f57bf4086652682db65f1a99ba7c2932`;
- source diff: only `src/double_ckks.cpp`, four insertions and no deletions;
- run/jobs: `33587707313` / Linux `100115198170` / Windows
  `100115197923`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33587707313`;
- result: warning-clean project build, Relin2 API-contract build, and exactly
  `19/19` runtime tests passed on both platforms in 0.22/0.37 seconds;
- evidence branch/commit/tree: `evidence/relin2-hosted-fd847ae` /
  `0e41525f3b2efb49340adba8ddf0705c644925de` /
  `b697b5f2b7eb9099612f19dd309c89fc700c6f39`;
- 19-entry manifest SHA-256:
  `c1ba1c2146a5b8c830d268b3718b2c53edecd224335a74be7189be91269301b5`.

The four-line production change runs after concrete-subtype validation and
only when the bound parameters select BV with digit size zero. It compares the
already-cast key's A-vector length with the bound complete-`Q` parameter count
and emits the exact project diagnostic on mismatch. It does not add a generic
helper, catch an exception, or touch the existing scaffold.

The green run executed both the valid-key positive control and the malformed-A
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
the separately retained logs. The red ZIP passed the same checks. Both evidence
sets report zero uploaded GitHub artifacts and passed Gitleaks 8.30.1 plus
independent targeted credential filename/content scans without retaining any
matching secret text.

Three independent read-only source reviews passed before the green commit.
After the green hosted run, independent Spec, TDD, and Delivery/evidence reviews
each returned `PASS` against the frozen evidence bytes.

The BV digit-size-zero A-vector-length red/green boundary is accepted and
closed. The next isolated TDD boundary is BV digit-size-zero B-vector length.
It must begin from `fd847ae5005c8eb179e9d5799b78b2d555f4981c`, add a durable
valid-key positive control before shortening only B, and must not combine
nonzero-digit BV, entry basis/format, ciphertext raising, or arithmetic.
