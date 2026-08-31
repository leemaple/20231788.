# ChatGPT Pro exact-commit closure — DCP/RCB

Prepared: 2026-09-01 Asia/Shanghai

## Background and bounded objective

Continue the same saved DCP/RCB review conversation. Review the complete
supplied clean-room source at branch `agent/codex-dcp-rcb-01`, exact commit
`87c84b879c13b55cf15d6559d3317853228fdc05`, and decide whether your sole
remaining test-only P1 has been implemented correctly and is now closed.

This is an algorithm, numeric-semantics, OpenFHE-integration, test, and
engineering-evidence review. It is not a network-security review. Do not inspect
or discuss the repository's known-wrong former implementation.

Use only the supplied paper, exact clean-room project, complete prior returned
review, retained CI logs, and pristine OpenFHE 1.5.0 at exact commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Do not assume access to local
files, a private repository, earlier conversation memory, browser state, or
anything outside this package.

## Complete prior-review state

Your immediately preceding review examined exact project commit
`02b34bac9cb87afc8acb9df275d5c0e137b554e7` and returned
`REMEDIATION-DCP-RCB-REVIEW-02b34ba.zip`, 16,697 bytes, SHA-256
`cd644f14902ce1cbce907379979c28712b224d1b93666471a7fc327394d3cdbd`.
The complete extracted result and original ZIP are supplied under
`prior-review/`.

Your verdict was `NEEDS NAMED FIXES`, P0 = 0, P1 = 1. You explicitly:

- approved keeping the slot-count manifest/validation;
- found no current production DCP/RCB arithmetic, centered-division, RCB,
  paper-scale, basis, validation-order, API, or portability defect;
- identified one test-only gap: OpenFHE ciphertext cloning shallow-copies
  metadata-map `shared_ptr<Metadata>` values and ciphertext equality compares
  map sizes/values but not keys, so ordinary clone/equality snapshots can
  false-pass key renaming or aliased in-place value mutation;
- returned a minimal test-only patch using a non-empty polymorphic metadata
  probe and independent deep key/value snapshots.

This task and package repeat that state so the answer must not depend on memory.

## Exact remediation to review

Current commit `87c84b879c13b55cf15d6559d3317853228fdc05` changes only
`tests/dcp_rcb_test.cpp` relative to `02b34bac...`. Codex did not blindly apply
the returned patch. It manually implemented the same behavior:

1. a test-only `ImmutabilityProbeMetadata` derived from OpenFHE `Metadata`,
   with polymorphic `Clone()` and value equality;
2. a non-empty `"immutability-probe" -> "unchanged"` entry on the DCP input;
3. `SnapshotMetadata`, which copies every ordered key and deep-clones every
   value before the operation;
4. `CheckMetadataUnchanged`, which verifies size, exact ordered keys, non-null
   values, and value equality after DCP and after RCB;
5. independent snapshots for the DCP input and both RCB pair members, in
   addition to the existing whole-ciphertext equality assertions.

The exact one-commit patch is supplied separately and the complete current
source is supplied, so inspect the implementation rather than accepting this
summary.

## Exact execution evidence

GitHub Actions run
`https://github.com/leemaple/20231788./actions/runs/33411494861` has exact
`headSha` `87c84b879c13b55cf15d6559d3317853228fdc05` and completed successfully:

- Linux/GCC built the project with the configured warning-as-error policy and
  passed 1/1 `dcp_rcb` CTest in 0.02 seconds.
- Windows Server 2022/MSYS2 MinGW64 checked out, built, and installed pristine
  OpenFHE commit `df495ba...`, built and linked the project, and passed 1/1
  `dcp_rcb` CTest in 0.31 seconds.

Complete retained logs and a concise evidence file are supplied. Treat them as
remote execution evidence, not as execution performed in your environment.

## Current architecture and boundaries

- `DoubleCKKS` binds one exact `CryptoContext<DCRTPoly>` under
  `FIXEDMANUAL`.
- DCP accepts a fresh, level-zero, degree-two, evaluation-format CKKS
  ciphertext over exact ordered basis `[q0, ..., q_l, q_div]`, including the
  minimum first-Mult2 basis `[q0, q_l, q_div]`.
- DCP uses locally derived `q_div^-1 mod q_i` and its negative with pristine
  `DropLastElementAndScale`, producing a read-only pair over the retained
  prefix at level one. It does not access absent OpenFHE precomputation rows.
- RCB validates the complete private pair manifest before raw element access
  and computes `q_div * high + low` without mutating either member.
- The module intentionally distinguishes OpenFHE's retained recorded scale
  from the paper's logical pair scale divided by `q_div`.
- The only constructible lifecycle is `ReadyForFirstMult`; later Tensor2,
  Relin2, RS2, Mult2, refresh, pair Add/Sub, and `t>2` are outside this review.
- Keep KISS/YAGNI, upstream OpenFHE pristine, and production fail-fast. Do not
  broaden the API, add dependencies, or add production exception recovery.

## Internal-review findings to adjudicate

The package includes
`INTERNAL-FINDINGS-RECONCILIATION.md`, which transparently reconciles two
earlier parallel internal reviews against this exact commit. Verify its claims.

Most concrete findings are stale and have exact corrective commits: the
four-tower minimum is now three, unused future lifecycle values were removed,
the negative-test harness rejects wrong exception types, evidence records list
commands, and the branch uses `agent/*`. Real/complex slot vectors and repeated
multiplication remain mandatory whole-pipeline tests but cannot run before the
later operations exist. Current pair-scale validation deliberately covers only
the sole constructible lifecycle under YAGNI.

One evidence limitation remains explicit: an early hardening red commit failed
to compile on a missing scale descriptor before several runtime assertions in
the same commit could execute. Later isolated red/green records cover multiple
specific contracts, and the current suite is cross-platform green, but history
must not be rewritten into an unobserved per-assertion red. Classify this as a
current P0/P1 blocker, a P2/P3 process issue, or name the smallest concrete
additional evidence artifact required. Do not call it a code defect without a
reachable incorrect behavior or violated current acceptance contract.

## Required review questions

1. Is the current deep metadata snapshot semantically equivalent to or stronger
   than your returned patch, without coupling production code to the test?
2. Does it close the exact metadata key/value immutability P1 for DCP input and
   both RCB members? Name any observable metadata state still not covered.
3. Did commit `87c84b...` introduce any P0/P1 algorithm, test-oracle,
   portability, lifetime, or OpenFHE-integration defect?
4. Are any reconciled internal-review findings still P0/P1 for this bounded
   DCP/RCB slice? Give exact file/line and reachable impact for every yes.
5. For exact commit `87c84b...`, return one verdict: `MERGEABLE`,
   `NEEDS NAMED FIXES`, or `NOT MERGEABLE`. `MERGEABLE` is still conditional
   on the separately required Windows ZCode/Zima same-commit review and says
   nothing about later multiplication operations.

Classify every finding P0, P1, P2, or P3. Separate observed source facts,
derived conclusions, retained remote execution evidence, and unverified claims.

## Required deliverables

Return one ZIP containing:

1. `EXACT-CLOSURE-REVIEW.md` — exact verdict, P0/P1 counts, answers to all five
   questions, and exact file/line/proof/impact/minimal remediation for findings;
2. `EXACT-CLOSURE-CONTRACT-MAP.md` — pass/fail/uncertain map for metadata
   deep-snapshot coverage, input immutability, DCP/RCB arithmetic non-regression,
   validation ordering, and exact CI binding;
3. `INTERNAL-FINDINGS-DISPOSITION.md` — explicit disposition of every supplied
   reconciled internal finding;
4. `EXECUTION.md` — commands actually run, environment, exit status, test
   counts, timeouts, and checks not run; inspection is not execution;
5. `0001-exact-closure-fixes.patch` only if a concrete current-commit P0/P1
   defect requires a change. It must apply to exact `87c84b...` and stay inside
   the bounded DCP/RCB slice.

State the returned ZIP SHA-256 in chat. Do not rely on a later message for
missing content.

## Mandatory checks and prohibited actions

- Review complete current source and the exact `02b34bac...87c84b` patch.
- Verify the test does not share a mutable metadata object with its snapshot.
- Verify exact key comparison, deep value comparison, DCP input coverage, and
  independent high/low RCB coverage.
- Inspect retained Linux and Windows evidence identity and record any mismatch.
- Do not inspect old/private/local implementations or modified OpenFHE trees.
- Do not implement, sketch, or review later algorithms in this response.
- Do not perform or discuss a network-security assessment.
- Do not push, merge, open a PR, dispatch/rerun CI, or use credentials.
- Do not claim local build, CTest, Windows, precision, performance, or security
  evidence unless actually observed and recorded.

## Acceptance standard

The closure is accepted only if it is bound to exact commit `87c84b...`,
reviews the complete current source and exact test-only delta, explicitly
disposes of the metadata P1 and every supplied internal finding, separates
inspection from execution, and makes no claim about unimplemented later
operations or the pending Windows ZCode/Zima review.

