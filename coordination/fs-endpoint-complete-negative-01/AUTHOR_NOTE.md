# Author note: hosted finalizer discriminating negatives

## Source and ownership

- Worktree: `/Users/lifeng/Documents/20231788-openfhe-endpoint-finalizer-negative-test-20260906`
- Branch: `codex/endpoint-finalizer-negative-test-20260906`
- Exact base: `18f4d32eaa70a0b182fc876e1589856fbbae8552`
- Base was confirmed present and the requested target path/branch were absent
  before the worktree was created.
- Only the four new files listed below are owned by this slice.  No existing
  source, test, workflow, parser or fixture was changed.

## Test-first authoring order

1. `BOUNDARY.md` fixed the public CLI seam, exact mutations, typed status
   outcomes, retained observations, and hosted-only execution boundary.
2. `tests/hosted_paper_endpoint_finalizer_negative.py` implemented exactly the
   three specified public negative cases.
3. `.github/workflows/endpoint-finalizer-negative-gate.yml` provided the
   isolated two-host execution envelope.
4. This note records the actual static-only author check.

These are regression cases for behavior already implemented and already
supported by the frozen complete positive.  No artificial new-behavior RED was
created.  The historical missing-complete-path RED was not rerun.

## Frozen dependencies observed

- `tests/hosted_paper_endpoint_finalizer_complete.py`:
  `d7784038f2bc2ee41b09bffd5665676d8f149a4f079055bb7c2c0447b7d30d89`
- `tests/paper_endpoint_finalizer.py`:
  `b3061c1b0283598e53e10d60241de77d5e4765743195e80782e4f2a7da507705`

The new test imports and reuses the frozen `_matching_full_inputs` fixture.  It
does not change that fixture or any reader/replay/reconciliation/publication
implementation.

## Authored files at static-check time

Final static check timestamp: `2026-09-06T06:59:41Z`.

- `coordination/fs-endpoint-complete-negative-01/BOUNDARY.md`:
  `800aedc7f45703a6fd854055d13dc7be8a55ecf87314ead5d8984a12a5cf4af9`
- `tests/hosted_paper_endpoint_finalizer_negative.py`:
  `0ca8000fb6cbb33f6d66100490d735f0bcdcdef35869d0f715f027332b0e084e`
- `.github/workflows/endpoint-finalizer-negative-gate.yml`:
  `6dc266a91d51ac3ea176a19308b02e58df2ebbe92b630920f2a380b5f613d01a`

The author note's own final hash is intentionally reported externally rather
than self-referentially embedded.

## Actual checks performed

- Bundled CPython 3.12 parsed the new test with `ast.parse`: `PYTHON_AST_OK`.
- Ruby Psych parsed the workflow syntax: `YAML_PARSE_OK`.
- Static enumeration found exactly three `test_*` methods.
- Static inspection confirmed the single exact trigger branch, two 15-minute
  jobs, pinned accepted action SHAs, 600-second subprocess timeout, and absence
  of upload/build/CTest/crypto/full-chain commands.
- Whitespace/error checks on the new Python and YAML files reported no errors.
- Root pre-review correctly found that an unanchored `b"0\t..."` slot token
  could also begin at the final digit of slots `10`, `20`, and so on.  Before
  any hosted execution, the conditioning mutation was corrected to include the
  preceding LF and full slot-zero line.  A tiny manually constructed byte
  sample containing only slot `0` and slot `10` reported
  `SLOT0_LINE_ANCHOR_OK`; it did not construct or parse the full fixture.

## Deliberately not executed

The hosted test, `_matching_full_inputs`, 16,384-row readers, scalar replay,
finalizer subprocess, C++, CTest, OpenFHE, crypto, build, workflow, CI dispatch
and any full suite were **NOT RUN** in this worktree.  Therefore this note makes
no RED, GREEN, cross-platform, runtime, or acceptance claim.  No commit, push,
dispatch, retry, artifact upload, secret access or main-worktree edit occurred.

Root still owns independent source review, secret scanning, the one allowed
hosted activation, evidence attribution and final acceptance.
