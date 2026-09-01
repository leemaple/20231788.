# Relin2 remediation 04 receipt and review

Recorded: 2026-09-01 14:34 Asia/Shanghai

Decision: **CHANGES NEEDED — do not apply any remediation-04 patch to the real implementation branch.**

This receipt distinguishes mechanical observations from review conclusions. It
does not treat ChatGPT Pro local Linux evidence as Codex, hosted, or Windows
evidence, and it makes no new runtime claim.

## External response and recovered archive

- Conversation: <https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9>
- The final response began with `ready to apply` and supplied one archive named
  `chatgpt-pro-relin2-01-remediation-04-delivery.zip`.
- Page-declared and locally observed size: `206292` bytes.
- Page-declared and locally observed SHA-256:
  `61fa2b9ab16c79faf247338faeedf123d1329f0b905f75313709ff5154f28de8`.
- The first and only UI download-control click did not create a completed local
  file. Codex did not click that control again. It resumed the page's existing
  attachment query once and performed one direct transfer of that same signed
  attachment into a fresh quarantine directory. Exactly one completed file was
  retained.

## Mechanical archive and input gates

Observed pass:

- `unzip -t` returned success.
- The central directory contained exactly ten unencrypted regular `100644`
  root entries, in the required order, with no duplicate, directory, link,
  slash, backslash, or unsafe path:
  `01-red-relin2-api.patch`, `02-api-scaffold.patch`,
  `03-red-relin2-contract.patch`, `04-green-relin2-core.patch`,
  `05-red-tensor2-lifecycle.patch`, `06-green-tensor2-lifecycle.patch`,
  `07-final-docs.patch`, `REVIEW.md`, `TESTS.md`, and `PATCHES.sha256`.
- `PATCHES.sha256` contained exactly nine ordered records for the other nine
  files. All nine recomputed hashes matched.
- All nine inputs submitted to ChatGPT Pro matched the sizes and SHA-256 values
  frozen in the pre-receipt checklist. In particular, the base source archive,
  binding, original task, remediation tasks 01-04, remediation-03 archive, and
  remediation-03 receipt were byte-identical to their recorded identities.
- Pinned Gitleaks `8.30.1` initially reported 30 `generic-api-key` findings.
  Every finding was reviewed. They were Git tree/SHA-256 evidence identities or
  the deliberately synthetic Relin2 key-cache mutation tag. A remediation-04
  exact-fingerprint ignore file covering only those 30 reviewed findings was
  used; the default-rules rescan returned exit `0` and `no leaks found`.
- A separate targeted filename/content scan found no `.env`, API key, token,
  private key, cookie, browser-state, credential, or authorization material.

The archive and extracted files remain under ignored quarantine paths in
`artifacts/incoming/chatgpt-pro-relin2-remediation-04/`; no incoming binary or
extraction was added to Git.

## Exact-base replay

The real implementation branch was clean at exact local/remote commit
`fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
`759d5195739684748d5a9664edabe3fa719e1acf`, before inspection. No patch was
applied there.

First, a separate no-hardlinks clone replayed the rejected remediation-03
archive from the fixed base. Its final tree independently reproduced as
`342436fab92fa2daf005ec557ee300cbe330122e`.

Then a different fresh no-hardlinks clone replayed remediation 04 with, for
each patch, `git apply --check --whitespace=error-all`,
`git apply --index --whitespace=error-all`, `git diff --check`,
`git diff --cached --check`, and `git write-tree`. Every command returned
success and every cumulative tree matched the precomputed gate:

| Boundary | Observed tree |
|---|---|
| base | `759d5195739684748d5a9664edabe3fa719e1acf` |
| 01 | `28d0294bce8844fa831951879020b353568c1c13` |
| 02 | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| 03 | `ef19f3d7a55eee6767622c2b7210e00f7bdcfd54` |
| 04 | `358215176f75e96af3dc3a1baa58842e9d33c2b5` |
| 05 | `6ea2a0107e8cbc5239dcaafb6eb1d903ec6cac84` |
| 06 | `4f0d376c8be65ccfc3c9dded02921525bfec65b8` |
| 07 | `2b1a10ab6e4f776a620710e2c7dd2b214a237357` |

Patch 03 produced required test blob
`e422c61bfd1cc210a6a7d0f3dab8a18d66985ffe`; patch 05 produced required test
blob `7f04970cf56cce051336c63abc315c9a22261ff8`. All eight protected final source,
workflow, CMake, and test SHA-256 values matched the checklist. A complete
tree-to-tree comparison showed that, outside documentation/evidence, the
remediation-03-to-04 difference was exactly the six snapshot lines moved in
the three permitted Relin2 test functions. Production was unchanged.

## Independent static observations

Observed pass on the exact replay tree:

- frozen production/cache-path audit;
- exact two output-returning production `Relinearize` calls;
- zero production `try`/`catch` and frozen forbidden routes;
- 37 unique CTest name/command tuples and the 31-entry Relin2
  registration/`ResolveTest` bijection;
- exact remediation-03-to-04 protected-source and six-line reorder audit;
- Clang 17 raw-token discovery of ten direct Relin2 calls plus the single
  allowed unevaluated API-contract node, with the currently expected six/four
  classification and current statement ordering.

The reference-scope script passes with local Python 3.14, but the unversioned
retained command does not establish a portable or fail-closed proof. The three
review axes independently agreed on the following hard failures.

## P1 review failures

1. `artifacts/tdd/relin2/audit_reference_scope.py:54` embeds backslash-bearing
   regular expressions inside an f-string expression. It is a syntax error
   under the acceptance host's Python 3.9 and 3.10, while the retained command
   uses only unversioned `python3` and `14-environment.txt` omits the actual
   Python identity/version. Therefore the literal audit command and claimed
   raw exit cannot be independently reproduced as delivered.

2. `artifacts/tdd/relin2/audit_reference_scope.py:76-93` enumerates member
   calls, `std` calls, and only four preselected free/constructor names. It does
   not discover and reject every other non-member or qualified call. The
   current `lbcrypto::NativePoly(...)` construction is already unclassified,
   and an added unknown helper/qualified call could pass. This violates the
   remediation-04 requirement that any unclassified reference call fail the
   gate.

3. `artifacts/tdd/relin2/audit_relin2_calls.py:93-107,212-223` assigns an
   enclosing helper by the nearest hard-coded function start rather than an
   exact brace-matched function range. On failure paths it finds a prior
   `CheckThrowsExactInvalidArgument` token but does not assert that the Relin2
   token lies before that invocation's matching right parenthesis. A call in an
   intervening helper or after the diagnostic assertion could therefore be
   misattributed or falsely classified as contained. This does not prove the
   exact enclosing-helper and exception-containment contract.

These are evidence-integrity failures, not observed arithmetic failures. The
current production and executable test bytes remain frozen, but the
remediation-04 package is not acceptable under its own fail-closed criteria.

## Required next action

Issue one minimal remediation-05 in the same ChatGPT Pro conversation. Freeze
patches 01-06, all production/API/CMake/workflow/executable-test bytes, and all
runtime claims. Change only patch-07 audit programs and their directly affected
retained evidence/index/docs, plus external `REVIEW.md`, `TESTS.md`, and
`PATCHES.sha256`. Require:

- Python 3.9-compatible syntax and an explicit retained Python identity;
- token/AST-aware enumeration and exact allow-list classification of every
  call in the complete three-helper reference closure, with unknowns failing;
- brace-matched exact enclosing function ranges for all ten Relin2 calls;
- proof that every failure call lies inside the matching exact diagnostic
  invocation;
- fresh raw outputs/exits, patch-07 hash, final tree, archive hashes, and a
  fresh ten-file archive.

Until that replacement passes mechanical and three-axis review, do not apply,
commit, push, dispatch hosted CI for, or spend the one authorized Fable5 review
on this Relin2 candidate.
