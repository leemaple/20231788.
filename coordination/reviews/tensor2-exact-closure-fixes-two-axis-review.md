# Tensor2 exact-closure fixes — final two-axis review

Prepared: 2026-09-01 Asia/Shanghai

Review boundary:

```text
git diff 55f3b43c47b5b2464625afcc6a1f244724336d5b...fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9
```

The fixed point is the previously reviewed Tensor2 source/test/workflow head.
The reviewed head adds retained raw intermediate hosted evidence, one public
DCP diagnostic regression test, and the one-line diagnostic compatibility
fix. Standards and Spec were reviewed by separate read-only agents in
parallel. Neither agent built, tested, edited, browsed, or pushed.

## Standards

**PASS.** No hard documented-standard violations and no actionable baseline
smells.

- `src/double_ckks.cpp:324-325`: the one-token `"ciphertext"` to `"pair"` change
  minimally restores the pre-refactor accepted diagnostic, preserves
  fail-fast behavior, and adds no exception handling or abstraction.
- `tests/dcp_rcb_test.cpp:571`: the regression uses the public `DCP` seam, a
  fixed expected diagnostic, and one behavioral assertion. Commit order
  records test-before-fix; at the test commit, production still emitted
  `"ciphertext state"`, establishing the red condition.
- Hosted evidence is internally consistent: all 12 README SHA-256 hashes
  match, their total is exactly 637,139 bytes, run/job IDs and head SHAs
  align, attempt/event/branch metadata match, Windows cancellation follows
  each Linux boundary, OpenFHE is pinned to the stated 1.5.0 commit, and the
  described compile-red/runtime-red/6-of-6-green outcomes match the logs.

## Spec

**PASS.** No actionable Spec findings.

- P2 is complete. The requirement to supply raw run JSON, jobs JSON, and
  relevant Linux/Windows job logs with hashes is met under
  `artifacts/tdd/tensor2/hosted/` on the implementation branch. The mappings
  in its `README.md` match the tracked JSON/logs: runs `33425868973`,
  `33426712752`, and `33427271692` bind the stated SHAs; job IDs/names match;
  Linux results are respectively compile failure, 1/6 with five intended
  Tensor2 failures, and 6/6 success; all three Windows jobs and overall runs
  are cancelled. All 12 hashes recompute exactly, totaling 637,139 bytes.
- P3 is complete. The regression at `tests/dcp_rcb_test.cpp:571-575` invokes
  public `DCP` and is called from `main`. Commit `9d1d10a` precedes
  `fb862a3`; at the test-only commit, `ValidateDcpInput` still passes
  `"ciphertext"`, so the exact legacy-diagnostic assertion fails. The
  subsequent one-line source fix passes `"pair"`, producing the required
  diagnostic without changing Tensor2 arithmetic or other accepted behavior.

The Spec reviewer kept new exact-head Linux/Windows CI and Windows ZCode/Zima
review separate as external acceptance gates. After the review was dispatched,
GitHub Actions run `33436252725` completed successfully on exact source/test
head `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`: Linux job `99633299988`
and Windows job `99633300315` each passed 6/6 CTests. Raw API records and logs
are retained in coordination commit `c5966f73dc72c3aeccd8c3348cb113889ef25737`.
The same-commit Windows ZCode/Zima review remains pending and is not claimed.

Summary: Standards 0 findings (PASS); Spec 0 findings (PASS); no worst issue
exists within either axis.
