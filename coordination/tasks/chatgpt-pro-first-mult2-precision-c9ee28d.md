Your previous precursor delivery is complete; this is one new bounded continuation with FULL current context, not a reminder or duplicate. Attached precision-first-mult2-c9ee28d.zip: 1163390 bytes; SHA-256 e49e3fcb897ea7b9fa0cf31bc376a9289e0bd4fe8f4a1ff70a35622bd5fe0461. It has62 regular files,61 exact manifest payloads,40 current Git-verified project files,16 pinned official source references, full paper PDF/text, exact dual-platform runtime RED/GREEN logs, and your original complete precursor return. Recursive safety/CRC passes for both ZIPs (96 regular files); Gitleaks8.30.1 source and final archive scans pass. Authoritative source c9ee28d0370eeee1ec7a1965402ed0b5e91f425e; actual tested code bd141806bd1e0b1dad80c7ad47bfd92fc334db55 is now54/54 on BOTH hosts. Please first audit this precursor, then draft the NEXT independent first-Mult2 high-precision test-only slice. No additional context is assumed. Full task follows and is also TASK.md in the ZIP.

# High-precision first Mult2: independent precursor audit and next executable slice

## Background, exact state and goal
The user requires a clean-room implementation of paper2023/1788 t=2 on pristine
official OpenFHE1.5.0, commit df495ba2e91739a6dc8f1de254fc5a41155ce504.
The full destination includes true high-precision multiplication, repeated
multiplications/refresh as needed, lossless usable I/O, paper parameters and
evidence; do not redefine success as a smaller functional compatibility layer.
Do not assume this conversation, another agent, a private repository, or local
files supply any context. This complete ZIP supplies the paper,16 pinned
official source files, all current build inputs/tests, pertinent ledgers and
the original complete Pro precursor return.

Authoritative project/ snapshot: branch codex/precision-01,
commit c9ee28d0370eeee1ec7a1965402ed0b5e91f425e; clean, pushed and remote verified.
Its active code/tests/CMake/workflow equal tested bd141806bd1e0b1dad80c7ad47bfd92fc334db55.
Runtime GREEN: https://github.com/leemaple/20231788./actions/runs/33864080896
Linux job100994829226:54/54,1.19s; Windows job100994829374:54/54,1.98s.
Both warning-as-error builds and five explicit API targets succeeded. This is
actual retained evidence, not a proposal. No Mac build or cryptographic test.

## New observed precursor evidence and exact limitations
The test precision_dcp_rcb_high_precision_contract uses N64,batch16,depth7,
p50,first55,FIXEDMANUAL,HYBRID,COMPLEX,degree2,scale2^100,UNIFORM_TERNARY,
HEStd_NotSet diagnostic only. Four fresh-key trials on EACH host pass unchanged
delta and all16-slot absolute2^-80 assertions. Worst observed delta across
eight trials=5.9416098364710929682297021517222122255998e-28; worst max-slot
error=5.0925606891564857369051272102149462810231e-28.
Actual centered-coefficient headroom258bits is diagnostic, NOT a theorem
about unwrapped integer histories or universal key-switch/Gaussian errors.

True runtime RED: e38764ab16bc638182d95ff259943eee0987d537,
https://github.com/leemaple/20231788./actions/runs/33863067661:
53/54 on BOTH hosts, only the new fixed positive2^-80 delta gate failed at
trial0 with approx1.443717080129e-15 from a binary64 lossy input fixture.
Do not allege DCP/RCB or pristine encoding was defective. Before that, fe35a099
failed Windows compilation at four invalid lbcrypto::Format qualifications;
that is retained separately, NOT the intended runtime RED.
Codex mechanically corrected those four to global Format:: before runtime RED.
GREEN mechanically corrects the same issue at three original fixture locations.
Original Pro return bytes are archived unchanged; no claim that its raw patches
equal these namespace-corrected sources.

GREEN changed only tests/precision_dcp_rcb_fixture.cpp. Current frozen hashes:
contract ad677414499c3e98e7f798ed940d587cb35c6cc791c7b0f81166ca1e6917f854;
header4b7b1c4f2670f5dc93e8d28f1ad585a47bb9cf4b81130bb45200a3af82e6b554;
CMake ab5a9873f90f5ab7d292dca4e54684e1242102ac469e0810f71508b26e39c91b;
workflow e3e1d23250b73f70747a3681975e3da870d1f337a3e2f54e674bb05a8900f951;
GREEN fixture4bcd633c3fb4b6ad4fa5d2088908e7992ccd50dec0d3b826fe976e590a3aa596.
Fresh inverse special FFT in multiprecision rounds ONCE at2^100 and injects
through the public DCRT element of a pristine plaintext. Its binary64 zero
placeholder cache is stale: never packed-value-read, serialize, production
Decrypt or call this a shipping codec. Independent secret/CRT schoolbook
decryption plus direct canonical polynomial evaluation supply the oracle,
with constant/X^32/hardcoded X^2 witnesses. Audit these, not only PASS text.

## Architecture and boundaries that must survive
Public DCP,RCB,Tensor2,Relin2,RS2,Mult2,pair Add/Sub exist from clean-room code.
DCP removes last q_div; Tensor2 omits low-low; Relin2 uses raised high and low
key-switch paths; RS2 rescales high/recombination independently and corrects
low. Mult2 composes these. Pair lifecycle after first multiplication is
RefreshRequired; second Mult2 is deliberately rejected. This is an INTERIM
tested boundary, not the full goal. Do not bypass or weaken validation, expose
private helpers, create a mutable pair factory, or mutate inputs/keys.
Current53 original functional tests and all API bindings must remain.
Prior Pro proposal's F1 dependency on BV resolution is NOT adopted: this next
HYBRID precision gate can proceed independently. BV fixed-key bound/source
premise is under separate Pro review; no parallel edits to that test or
universal_theorem_gate=UNPROVED/conservative_E_Relin labels.
Pair-composition extra coverage is another isolated task and not current source.

## Tasks, in priority order
1. Independently audit the ACTUAL current precursor, red/green continuity,
oracle independence, slot ordering/witnesses, test-only cache caveat,
namespace corrections, logs and claim boundaries. Return a source-cited
verdict with P0/P1/P2 findings and observable disposition criteria. Do not
silently "fix" immutable accepted tests as part of another slice.
2. Design and DRAFT a minimal test-only first-Mult2 high-precision executable
candidate at the agreed public Mult2/RCB seam, preferably reusing the existing
test-owned precision fixture without changing its file/header or old contract.
Supply COMPLETE files plus an applicable patch against exact project/ snapshot.
A new standalone test target is fine; no production changes before the new
behavior is actually observed on hosted CI.
3. Freeze deterministic representative multiprecision input vectors, independent
product expectations, sub-binary64 distinguishing witness, exact parameter
context, fresh-key trial count, and operation-specific acceptance BEFORE
execution. Primary desired first-Mult2 absolute target is2^-80. Derive its
plausibility from paper/source and actual scaling; do not simply copy the
precursor bound. If this context cannot justify/support it, expose the exact
reason and minimal paper-directed parameter change, not a silently lowered
threshold or a made-up successful result.
Use real/complex, signs, zero/near-zero and nontrivial fractional data; keep
within justified magnitude bounds. Use lossless literals/exact rationals and
independent small host arithmetic for expected products; no double-valued
expected answer, metadata-derived precision claim or production helper oracle.
A negative control may show binary64 inputs lose distinguishing information;
label it a control, not a fake production regression.
4. Independently decrypt/recombine/evaluate actual Mult2 outputs with exact
integer/rational scale arithmetic. Initial scale is2^100; derive rather than
assume final logical scale, expected2^200/(q_div*q_l) for the current first
Mult2 path. Neither double GetScalingFactor nor long-double approximate
descriptors may set the high-precision expected normalization. Public RCB
may be the observed endpoint but must not compute the coefficient oracle.
Keep stage-state/key/basis/level/lifecycle/immutability checks separate from
slot accuracy and universal no-wrap claims. Compare staged/direct paths only
as a wiring check, never the sole independent arithmetic oracle.
5. Give a bounded next-step path to the FULL paper6.3/Table3 goal:
Delta~2^100, Div40/Mult60, Base50x2,8 repeated squarings,1000 executions,
N2^15,h128,dnum11,auxiliary P60 and reported average error~2^-81.8.
The existing p50/50, N64 HEStd_NotSet experiment is not that reproduction.
Explain concrete minimal parameter/scale/lifecycle and production lossless
I/O work still required. Do not implement speculative frameworks or expand
the current patch to all those independent changes.
6. Treat first observed outcome honestly: existing production already implements
Mult2, so this newly added regression may initially PASS. If so call it first-
observed GREEN; do not manufacture RED by breaking production. If it fails,
retain the actual failure and diagnose a single precise cause before any fix.
The existing Mult2 feature already has retained original TDD evidence. Do not
change a frozen threshold/vector/oracle after seeing a failure to make it pass.

## Deliverables and acceptance
Downloadable ZIP and matching SHA-256 sidecar, every required payload nonempty:
PRECURSOR_REVIEW.md; FIRST_MULT2_TEST_DESIGN.md with frozen vectors/thresholds,
scale derivation and independent worked expectations; SOURCE_CLAIM_TEST_LEDGER.md;
HOSTED_COMMANDS_AND_EXPECTED_REGISTRATION.md; NEXT_PAPER_PRECISION_GATES.md;
complete final changed/new files; applicable test-only patch; manifests with
exact paths, sizes and SHA-256; explicit static/compiled/runtime execution ledger.
Check patch application against the supplied CURRENT snapshot, not old bda8791.
Show all54 current CTest names/command bindings preserved exactly, and any new
registration. Preserve current workflow and all default warnings/API targets.
Do not alter any original production/test file to relax or bypass acceptance.
Avoid omissions masked by a manifest listing; provide actual code and patch bytes.
Do not claim hosted execution from local static work. Source review, independent
small arithmetic, patch applicability and manifest checks can be done now.

## Required hosted commands and prohibitions
Codex will run GitHub Actions Linux GCC and Windows MinGW64, Debug and
-Wall -Wextra -Wpedantic -Werror, --parallel2:
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
explicit relin2_api_contract_test,rs2_api_contract_test,mult2_api_contract_test,
add_api_contract_test,sub_api_contract_test builds
ctest --test-dir build --verbose --output-on-failure
No Mac OpenFHE/project compilation or crypto runtime; no old implementations or
modified OpenFHE; no credentials/browser state; no git push/merge, CI dispatch,
external messages, security-noise guard removal, broad catch or tolerance tuning.
Only existing public seams and explicit test adapters; KISS/YAGNI/TDD.
Return inspected facts, inferences and pending items distinctly. Continue the
real paper destination, but make this one precision slice executable and auditable.
