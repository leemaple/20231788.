# Primary/sidecar binding acceptance — 2026-09-06

Root owns the slice at base 30054c4fc02217b209c1b17e7701db3b5a292d38.
The documented public seam is validate_primary_binding; reconcile_replay is
still NOT IMPLEMENTED. Inputs must be actual validated reader returns; this
does not authenticate arbitrary Python objects or duplicated external summaries.

## Actual TDD and review evidence

- 01_import_red.json: absent module, one import error, exit 1.
- 02_identity_green.json: first identity tracer, 1 PASS.
- 03_binding_red.json: 4 tests, 141 actual failing assertions; tool output
  truncated. 03b_binding_red_structured.json retains a bounded rerun of the
  same unchanged source/tests solely to capture all failing case identifiers
  without output truncation. This is not a cryptographic retry or CI rerun.
- 04_binding_green.json: 4 PASS, 0.319s.
- 05_binding_review_coverage_green.json: 5 PASS, 0.377s; further identity and
  shape coverage first passed. No fabricated RED is claimed.
- 06_real_reader_integration.json: 6 PASS, 0.408s. Actual primary reader and
  sidecar reader dataclasses bind a 16,384-row synthetic TSV, then reject
  individually valid mismatched producer distance and alternate-K primary
  records. No scalar full replay ran.
- 07_test_locality_green.json: moved the existing shape assertions back into
  their named shape test in review stage. Same production code, 6 PASS,
  0.798s, exit 0. This is not a new TDD RED/GREEN loop.

Runtime: bundled Python 3.12.14, -B, one bounded process at a time.
Source SHA-256:
72683da272f423da95b7f8c079c3397e7954016b2528348802e1a0a5888e6f13.
Final test SHA-256:
eeef1a4819506ee0fe91324f3cabdfcd501fe120330a50b6bccc81fccb53b4d9.

Independent source-first review of the binding implementation found no blocker
under its parsed-input precondition. Review scope and limits are recorded in
INDEPENDENT_MATH_REVIEW.md; later real-reader integration closes the initial
lack of parser-interoperability coverage, not full pipeline verification.

## Scope and remaining work

Exact identity/scope/Boost/count/E80, all nine scales, all 24 five-field check
receipts and all four sidecar-C-derived maximum allowances now bind. Primary
does not contain exact C; matching allowances is not proof of runtime exact-C
provenance. Writer/capture and exact hosted source provide that separately.

No metric/signed-tuple replay reconciliation, finalizer, publication adoption,
shell wrapper, live C++ writer call or hosted full 16K replay ran in this slice.
E80 remains FAIL at the last actual full-chain observation; A NOT_ADOPTED and
the observer assurance CONDITIONAL are unchanged. No compiler, crypto/FFT/NTT,
extra encrypted chain or CI dispatch was used on the Mac.
