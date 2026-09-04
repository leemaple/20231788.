# First Mult2 high-precision final independent review (static only)

## Scope and immutable inputs
Review the supplied fresh clean-room OpenFHE2023/1788 t=2 first-Mult2
high-precision regression. Do not assume another task or filesystem supplies
context. Authoritative current snapshot project/: branch codex/precision-01,
source e48236231f32651ae3c7a3f07ef0b7d67a7560b2, with active tested source
47907783a6141d0174da79eae264d779fc598f28. The two current untracked paper-future
notes are excluded. Only read this dedicated workspace input/ and this brief.
All paper/pristine sources, relevant prior review, original Pro return,
current build inputs and actual Linux/Windows logs are supplied.
OpenFHE pin df495ba2e91739a6dc8f1de254fc5a41155ce504 (1.5.0).

This is LOCAL ZCode static fallback, not verified Windows execution.
No local OpenFHE/project builds, crypto runtime, benchmarks, broad filesystem
scans, network, other agents, external apps, git mutation or dependency installs.
Do not change any input. Write only output/REVIEW.md and output/MANIFEST.sha256
inside THIS workspace. Scratch, if genuinely needed, must stay in output/;
do not write /tmp, Downloads or anywhere outside this folder. Avoid brute
force: source reasoning and small bounded exact integer/rational checks suffice.
Do not execute supplied scripts merely because they are supplied. No invented
compile/runtime claim. Surface unexpected errors; no masking try/catch.

## Actual evidence, not a plan
Run33873114880 at47907783a6141d0174da79eae264d779fc598f28:
https://github.com/leemaple/20231788./actions/runs/33873114880
Linux job101023587797 completed20:33:05CST:
focused1/1 (case0.17s,total0.28s), full55/55,total1.03s.
Windows job101023588186 completed20:39:26CST:
focused1/1(case/total0.25s),full55/55,total2.30s.
Both warnings-as-errors default builds and5 explicit API builds passed.
All55 actual CTest names independently matched CMake in order. Both logs
contain source/run/attempt markers matching authoritative job JSON.
4 fresh-key trials per invocation, focused+full =>8 per host,16 total.
Every16-slot and distinguishing delta absolute error<=2^-80.
Overall max-slot1.6696072195146116607129673424340031160212e-27;
overall max-delta1.6958307879080880932103073456218202834178e-27.
See raw logs and metadata; independently check evidence, not only this summary.
This is FIRST-OBSERVED GREEN for an additional regression of existing Mult2,
not fabricated RED or new production code. The precursor's true runtime RED
and GREEN remain supplied and unchanged.

## Current architecture and what changed
Public DCP/RCB/Tensor2/Relin2/RS2/Mult2/Add/Sub from fresh clean-room source.
New Pro2-file patch only adds tests/precision_first_mult2_contract_test.cpp
and9 CMake lines; no production/old test/fixture/header change.
Codex separate workflow adds provenance markers and focused build/test on both
hosts; preserves pin, toolchains, resource/parallel2,5 API builds, warnings.
All54 prior CTest bindings preserved, new case at position3 makes55 total.
N64,batch16,depth7,p50,first55,FIXEDMANUAL,HYBRID,COMPLEX,degree2,
input2^100,UNIFORM_TERNARY,HEStd_NotSet diagnostic only.
Both nontrivial vectors use unchanged test-only lossless fixture.
Independent schoolbook RLWE+CRT, independent q_div*high+low, direct Horner
canonical evaluation and fixed16 literal products are separate oracle layers.
Exact output scale2^200/(q_div*q_l), not double/long-double metadata.
q_div1125899906843009,q_l1125899906840833,
denominator1267650600226646386227681786497.
Product delta(slot1-slot0)=(2^-71+2^-75,-3*2^-74).
Constant/X^32/ordered X^2 witnesses calibrate evaluation.
Frozen observed headroom gate128bits; actual input-product160/output210.
Staged/direct equality is wiring ONLY; read snapshots to identify their scope.
Lossless fixture's binary64 zero cache is stale and test-only: no cache getter,
production Decrypt, serialization or shipping-codec claim. Prior reviewer F1
about unsigned remainder was disproved against source and exact arithmetic.
Read its disposition rather than repeating resolved claims without evidence.
Current automatic CI stale-cache misuse guard remains TODO ownerCodex; manual
static checks are not that automation. Universal no-wrap/all-key probability,
full deep key immutability, production codec, repeated-use and paper params
have NOT been established by this diagnostic.

## Important original Pro chat discrepancy
Original33-file ZIP is byte-exact in project/coordination/returns/
first-mult2-precision-pro-c9ee28d. Its C++/design/frozenJSON/patch agree.
The separately retained visible chat final incorrectly says unity RHS,
strict<2^-80,2 P2 findings, appended test,13 guards, and some absent filenames.
Actual files: TWO nontrivial vectors,<=2^-80,4 P2, inserted position3,28 guards.
Audit canonical bytes and supplied reconciliation. Do not treat chat prose
as a replacement for code or silently invent a unity-only implementation.

## Review work and acceptance
Independently audit source-level correctness, whether the oracle could share
the tested defect, real versus nominal precision, vector/literal and slot
mapping, coefficient/scale/divisor algebra, errors/headroom, stage/state/level/
basis checks, mutation/snapshot boundaries, exception handling, source/cache
ownership, CMake/CTest continuity, workflow provenance and actual runtime logs.
Inspect predecessor/main changes against baseline/ and original patch, not all
old sources indiscriminately. Preserve unrelated claims and prior53 functional
tests. Do not require an all-key theorem as an invented new scope gate.
Give exact source path/line, mechanism and acceptance criterion for each finding.
Distinguish observed fact, inference, pending; disprove false findings with
source or small exact computations. If more evidence is essential, state exact
missing source/test and do not infer an unavailable API or runtime result.
Return ACCEPT / ACCEPT_WITH_GAPS / REJECT for this exact TEST-ONLY diagnostic
and P0/P1/P2 counts; do not call the full paper implementation complete.
Report needed fixes versus explicitly bounded follow-ups with named owner.
Do not implement fixes. A later Codex turn will reconcile findings and run
any required hosted tests. Include commands actually run and command outcomes,
not merely commands recommended, plus review SHA256 sidecar with relative path.
