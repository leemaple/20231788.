# Cycle-A RED preflight: production client I/O

2026-09-05, Asia/Shanghai. **STATIC ACCEPTANCE ONLY; HOSTED RED PENDING.**

Candidate `12d8fae78cc0d0fed5038cf21cdbd2173fe1f1ef` on
`codex/lossless-io-implementation-01`, compared with
`7b8a9d2164037f9d30e94e3b96d245d27f32c61b`. The latter is the pushed
fallback receipt; its engineering equals approved input `4ccc8fd` and
previously tested `4ecbd972429884489918d9f82dfc3fe9f702ef4a`.

Only four engineering files change. Production `high_precision_client_io.h`
and `.cpp` deliberately do not exist. No existing source or test changed.

| Path | SHA-256 |
| --- | --- |
| tests/precision_client_io_first_mult2_contract_test.cpp | 6b3260a35db8715fff2a3d3a4b24137b7d8c12b6101db422c476b0236ed0ea16 |
| tests/precision_client_io_oracle.h | 9f7d8222ef6520bc845ab1b81fe735f5f7a46a48d5de2c2b984c63148e2c42af |
| CMakeLists.txt | 0eb5f61e7d43e2134ed469c09241917ee8f1af9b68714d654ef6c97ba0688050 |
| .github/workflows/dcp-rcb.yml | 6ef6c424d7b412bc28f94510a1ba70143378b5a3976e1eebb4b68536033e03fa |

Main test: 581 lines, 38,913 bytes. Oracle: 503 lines, 21,211 bytes.
Independent exact rational checks match all 16 products, 32 complex inputs,
input delta `(2^-70,2^-73)`, product delta `(17/2^75,-3/2^74)`, and
`qDiv*qL=1267650600226646386227681786497`, coprime to `2^200`.
The runtime test's independent arithmetic check precedes production use.
Actual compiler/API compatibility and cryptographic accuracy remain unexecuted.

## Standards

Independent Codex Standards review of the exact four-file diff:
**0 documented-standard findings; 0 actionable heuristic findings**.
The approved public seam, narrow diagnostic-specific exception catches,
fail-fast behavior and A-only scope are preserved. The test-only
schoolbook/CRT/Horner adapter is independent expected-result machinery,
not a duplicated production crypto backend. One TU includes its helper header.

The old 57 names/normalized commands/order are unchanged; the no-header
name<TAB>command<LF> ledger is 3,869 bytes with SHA-256
`3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b`.
Exactly one last CTest is added: `precision_client_io_first_mult2_contract`.
Both jobs preserve warning-as-error, old focused tests and five explicit APIs.

## Spec

Independent Codex Spec review: **0 actionable findings** against
`coordination/tasks/LOSSLESS_CLIENT_IO_PRO_IMPLEMENTATION_01.md`.
The first-operation tracer covers public Encrypt/clone/DCP/Mult2/RCB/bind/
Decrypt, frozen multiprecision values/scales, independent stride-projected
Horner and literal controls, rational ownership, complete decoded state and
owned-value lifetime. There is no B clone-mutation/live-Params-drift coverage,
production code, repeated-family support or h128 change in this RED.

Static malformed-key count is **32**: 2 null keys, 3 PK element-count cases,
9 shapes in either PK component, one unset SK and 8 other SK element shapes.
Exact-pin DCRT constructors/copy/move semantics were checked to keep negative
fixtures safe; invalid keys are never compared through unsafe official
operator==. All catches bind the prescribed project diagnostic and nonmutation.
This is a static count, not 32 executed rejection assertions yet.

## Root reconciliation and build gates

Root read the complete new test and helper, authoritative task/design/correction
records, baseline CMake/workflow and relevant pristine source. Root independently
checked final file hashes, absent production files and exact old57/new58 bindings.
Before the RED was committed, review refinements bound floating snapshots with
hexfloat, exact ClientComplex field types, full decoded-state comparison,
finite observed values and exact input equality rather than NaN-vulnerable
maximum-error reduction. No precision threshold or frozen vector was weakened.
Official Format is a global enum, not the proposal's declaration-shaped
lbcrypto::Format spelling. The actual pinned enum is used. Four disabled CKKS
setters are avoided; official default values are validated after factory return.

CI provenance overlay explicitly selects native64/backend4 and qualifies the
Linux dependency cache key. The new test is EXCLUDE_FROM_ALL. Each hosted job
must first build the old project, run existing focuses and legacy57, and build
all five APIs; it then explicitly builds the new target, runs its focus and
finally full58. Legacy exclusion is anchored to only the new test name.
No step masks errors. The two new show-only JSON observations do not run tests.

Expected genuine RED: compilation of the new target stops at main test line2
because `openfhe_2023_1788/high_precision_client_io.h` is absent. On that
failure the new focused and full58 steps must be skipped; a legacy failure,
infrastructure fault or unrelated syntax problem is not an accepted RED.
Both host identities, exact pin/source, full log completeness and old57/API
results must be retained before any production GREEN is authored.

Root gitleaks8.30.1 test-tree scan (ambient config unset, inline allows ignored,
/dev/null ignore file, decode depth5, GOMAXPROCS2): 737,876 bytes, zero findings.
Git staged diff check passed before candidate commit. All Mac work is small
source/Git/static arithmetic; C++ build, crypto, NTT and CTest are **NOT RUN**.
The original Pro channel failure is documented in CODEX_CYCLE_A_FALLBACK_01.md;
these two reviews are independent Codex axes, not fabricated Pro/Fable/ZCode
implementation reviews. Final I/O review remains a later gate.
