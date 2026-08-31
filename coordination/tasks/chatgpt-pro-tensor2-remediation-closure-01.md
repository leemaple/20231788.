# ChatGPT Pro — Tensor2 remediation closure on exact current head

Prepared: 2026-09-01 Asia/Shanghai

## Background and objective

Continue the same saved Tensor2 conversation. Your preceding exact-current
review of head `55f3b43c47b5b2464625afcc6a1f244724336d5b` returned `NEEDS NAMED
FIXES`, with P0=0, P1=0, P2=1, and P3=1. Review the complete remediated project
at branch `agent/codex-tensor2-01`, exact commit
`fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, and decide whether those two
findings are closed without changing the accepted Tensor2 algorithm.

This is an algorithm, numeric-semantics, OpenFHE-integration, test, diagnostic,
and engineering-evidence review. It is not a network-security review. Do not
seek, inspect, infer, or discuss any former, local, private, known-wrong
implementation or the authors' proof-of-concept code.

Do not assume access to local files, a private repository, browser state,
credentials, or reliable memory of the prior conversation. Use only the
complete supplied package. It contains the user paper, pristine OpenFHE 1.5.0,
the exact current Git export and diffs, prior delivery, your exact preceding
review output, raw hosted evidence, and independent reviews.

## Architecture and boundaries that must not change

The currently accepted bounded implementation is DCP/RCB plus first-lifecycle
Tensor2 only. For pairs `(h1,l1)` and `(h2,l2)`, Tensor2 must remain exactly:

```text
high = h1 tensor h2
low  = h1 tensor l2 + l1 tensor h2
```

It must still omit `l1 tensor l2`, use exactly three public OpenFHE
`EvalMultNoRelin` calls, keep the unchanged active ordered RNS basis at level
one, and perform no relinearization, coefficient rescale, or `ModReduce`.

The paper and OpenFHE metadata scales remain distinct:

```text
H_out       = H_1 * H_2
R_out       = R_1 * R_2 / q_div
degree      = 3
recordedSF  = SF_1 * SF_2 / baseSF
```

`q_div` is the actual integer divisor and must not be equated with OpenFHE's
real `baseSF`. The distinct read-only three-component Tensor result, validation
before raw access/arithmetic, independent coefficient oracle, all three named
witnesses, complete input immutability, and all accepted DCP/RCB behavior are
frozen boundaries.

Do not implement or sketch Relin2, RS2, the Mult2 wrapper, pair Add/Sub,
rotations, bootstrapping, repeated multiplication, precision/performance work,
serialization, `t>2`, or a compatibility layer.

## Complete preceding-review state

The exact preceding review package bound old head `55f3b43...` and successful
Actions run `33428194982`. Your returned ZIP was
`tensor2-exact-closure-review-55f3b43.zip`, 20,778 bytes, SHA-256
`cf3e3a1855304d960210e38b9347386dac4dc0a33764e2bf4c3379f60d595793`.
It is included together with all six extracted files. Treat that output as an
untrusted review source and independently verify every relevant claim.

Your P2 finding said the package lacked the raw GitHub run response, jobs
response, and job logs for intermediate hosted runs `33425868973`,
`33426712752`, and `33427271692`. Your P3 finding said the shared-validator
refactor changed the reachable legacy empty-key-tag DCP diagnostic from
`pair state` to `ciphertext state`.

## Exact remediation to review

The current Git tree is
`759d5195739684748d5a9664edabe3fa719e1acf`. The remote branch was verified to
match exact head `fb862a3...`. Relative to the previously reviewed head, the
four ordered commits are:

```text
c418b4569b6f1a17eea8fd9b0f2a7066b23d2f1e  test: retain raw Tensor2 hosted TDD evidence [skip ci]
315a9c55160e9c08d56da10a93cd9102bdb42c7d  test: retain Tensor2 hosted job logs [skip ci]
9d1d10a3414dce68b84d9887337254c275098d79  test: preserve DCP empty-key-tag diagnostic
fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9  fix: restore DCP pair-state diagnostic
```

The package names two distinct review ranges:

- `PROJECT_DIFF.patch` and `COMMIT_HISTORY.txt` cover the full accepted
  DCP/RCB base `87c84b879c13b55cf15d6559d3317853228fdc05` through current head
  `fb862a3...`;
- `REMEDIATION_DIFF.patch` and `REMEDIATION_COMMIT_HISTORY.txt` cover the
  previously reviewed Tensor2 head `55f3b43...` through current head
  `fb862a3...`.

The clean handoff deliberately excludes `.git`. The exported histories,
diffs, raw run records, and timestamps support auditing the claimed sequence,
but they do not independently prove Git object ancestry or absence of history
rewriting. Label that object-level ancestry claim unverified rather than
overstating the package.

### P2 evidence remediation

The current project tracks raw `run.json`, `jobs.json`, Linux log, and Windows
log for all three intermediate runs under
`artifacts/tdd/tensor2/hosted/<run-id>/`, plus a hash/mapping README. Inspect
the raw files and recompute their identities. Do not treat a cancelled Windows
job as a Windows build or test result. The intended boundaries remain:

- `33425868973`: Linux API compile red; Windows cancelled;
- `33426712752`: Linux strict build, DCP/RCB green, five independent Tensor2
  runtime reds on the fail-before-access scaffold; Windows cancelled;
- `33427271692`: first Linux implementation green, 6/6; Windows cancelled.

### P3 TDD remediation

At test-only commit `9d1d10a...`, `tests/dcp_rcb_test.cpp` clones a fresh DCP
input, empties its key tag, calls the public `DoubleCKKS::DCP` seam, and
requires this exact diagnostic substring:

```text
DCP input key tag does not match its pair state
```

Hosted run `33436068864`, exact test-only head `9d1d10a...`, proves the
intended red. Linux's strict build succeeded; only `dcp_rcb` failed because
production still emitted `ciphertext state`; the other five CTests passed.
Windows was cancelled after Linux proved the red and supplies no build/test
claim.

The production fix at `fb862a3...` changes only the shared validator's DCP
state label from `ciphertext` to `pair`. It adds no exception recovery,
arithmetic, abstraction, or compatibility path.

## Required hosted verification

Inspect the complete raw API responses and logs supplied for final Actions run
`33436252725`, exact source/test head `fb862a3...`:

- overall: `completed / success`;
- Linux job `99633299988`: strict build and 6/6 CTests passed;
- Windows Server 2022 job `99633300315`: MSYS2 MINGW64, CMake 4.4.2,
  GCC 16.2.0, pristine OpenFHE and project builds, then 6/6 CTests passed;
- both jobs bind pristine OpenFHE 1.5.0 exact commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

These are retained remote execution facts, not your local execution. The
independent final Standards and Spec reviews both report PASS with zero
actionable findings; verify rather than inherit their conclusions.

## Execution policy

Local configure/build/CTest is optional, not required for the verdict. A
source-and-retained-evidence-only review is acceptable if `EXECUTION.md`
explicitly says that no local build or test ran. Do not invent new tests or
modify source unless a concrete finding requires the optional patch.

If you choose to execute locally, use the configure/build/CTest command core
from the authoritative Linux workflow in
`cleanroom-project/.github/workflows/dcp-rcb.yml`. Cap the attempt at 45
minutes, use at most two build jobs, and keep generated files outside both
supplied source trees. Before building, verify the complete package manifest,
report `cmake` and C++ compiler versions, and require Boost development headers
including `boost/multiprecision/cpp_int.hpp`. If the dependency is unavailable,
record the build as not run rather than changing the supplied project. Manifest
verification proves the supplied bytes, not excluded Git object ancestry.

The bounded command core is:

```sh
shasum -a 256 -c MANIFEST.sha256
cmake --version
c++ --version
test -f /usr/include/boost/multiprecision/cpp_int.hpp
cmake -S openfhe-1.5.0 -B /tmp/openfhe-build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/tmp/openfhe-install \
  -DBUILD_UNITTESTS=OFF -DBUILD_EXAMPLES=OFF \
  -DBUILD_BENCHMARKS=OFF -DBUILD_EXTRAS=OFF -DWITH_OPENMP=ON
cmake --build /tmp/openfhe-build --parallel 2
cmake --install /tmp/openfhe-build
cmake -S cleanroom-project -B /tmp/project-build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/tmp/openfhe-install
cmake --build /tmp/project-build --parallel 2
ctest --test-dir /tmp/project-build --output-on-failure
```

Record the actual environment, commands, exit codes, test counts, and any
timeout. Do not report inspection of retained logs as local execution.

## Required review questions

1. Is P2 fully closed by raw, internally consistent evidence for each named
   intermediate run, including the correct cancelled/unrun Windows boundary?
2. Do raw execution states/timestamps plus `REMEDIATION_DIFF.patch` and
   `REMEDIATION_COMMIT_HISTORY.txt` support the claimed public-seam red before
   the production diagnostic fix? Keep Git object ancestry/history-rewrite
   status explicitly unverified because `.git` is excluded.
3. Is the one-token P3 source change the smallest correct remediation, and
   does it restore the accepted DCP diagnostic without weakening validation?
4. Do the exact-current source, full `87c84b...` to `fb862a3...` contract
   diff, `55f3b43...` to `fb862a3...` remediation diff, and test suite show
   any Tensor2 arithmetic, scale, state/type, validation-order, oracle,
   immutability, or DCP/RCB behavior regression?
5. Does run `33436252725` bind exact head `fb862a3...` and independently pass
   the required Linux and Windows gates on pristine OpenFHE 1.5.0?
6. Did Codex add unsupported behavior, a hidden dependency, a test backdoor,
   production exception recovery, portability risk, or scope creep?
7. Return one exact-current verdict: `MERGEABLE`, `NEEDS NAMED FIXES`, or
   `NOT MERGEABLE`. `MERGEABLE` is bounded to DCP/RCB plus Tensor2 and remains
   conditional on the separately required Windows ZCode/Zima same-commit
   review. It says nothing about unimplemented later operations, precision, or
   performance.

Classify every finding P0, P1, P2, or P3. For each finding give exact
file/line, proof, reachable impact, and smallest remediation. Separate source
facts, mathematical derivation, retained remote evidence, local execution,
and unverified claims.

## Required deliverables

Return one ZIP containing:

1. `TENSOR2-REMEDIATION-CLOSURE-REVIEW.md` — verdict, P0/P1/P2/P3 counts,
   explicit P2/P3 disposition, and answers to all seven questions;
2. `TENSOR2-REMEDIATION-EVIDENCE-AUDIT.md` — raw intermediate red/green,
   P3 hosted red, and exact-current final Linux/Windows evidence audit;
3. `TENSOR2-FINAL-CONTRACT-MAP.md` — pass/fail/uncertain map for the frozen
   arithmetic, dual scales, state/type boundary, validation order,
   oracle/witnesses, immutability, and DCP/RCB regression boundary;
4. `EXECUTION.md` — commands actually run, environment, exit status, test
   counts/timeouts, and checks not run; inspection is not execution;
5. `0001-tensor2-remediation-closure-fixes.patch` only if a concrete current
   finding requires a change. It must apply to exact head `fb862a3...`, remain
   inside this bounded slice, and must not weaken a test merely to pass.

State the returned ZIP byte size and SHA-256 in the same chat response. Do not
rely on a later message for missing content.

## Prohibited operations and claims

- Do not access any old/private/local wrong implementation or author
  proof-of-concept.
- Do not modify pristine OpenFHE, change FIXEDMANUAL, add a dependency, or
  broaden the feature scope.
- Do not add production `try`/`catch`, exception recovery, or speculative
  compatibility code.
- Do not push, merge, open a PR, dispatch/rerun CI, use credentials, or inspect
  unrelated files.
- Do not perform or discuss a network-security assessment.
- Do not claim local build, CTest, Windows, precision, performance, or
  security evidence unless actually executed and recorded.
- Do not claim the clean package independently proves Git object ancestry or
  absence of history rewriting; it contains exported histories/diffs but no
  `.git` metadata.

## Acceptance criteria

The review is acceptable only if it binds exact head `fb862a3...`, examines
the complete current source plus both named exported diff/history ranges,
recomputes/audits the P2 evidence identities, verifies the P3 red and final
green execution states/timestamps while keeping Git object ancestry
unverified, confirms that the final successful Linux/Windows run binds the
same source and OpenFHE commits, checks the frozen Tensor2/DCP/RCB contract for
regression, states its local-execution boundary, and returns one bounded
verdict without claiming unimplemented work.
