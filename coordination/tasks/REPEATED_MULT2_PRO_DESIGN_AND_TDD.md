# Repeated Mult2: source-backed design and the next executable TDD slice

## Background and exact objective
Implement the FULL clean-room t=2 method of paper2023/1788 on pristine
OpenFHE1.5.0 pin df495ba2e91739a6dc8f1de254fc5a41155ce504. This is a NEW
dependent engineering conversation for repeated multiplication, not another
request to finish the completed first-Mult2 precision response. All required
paper, active source, tests, official upstream source and observed evidence are
attached. Do not assume access to a filesystem, repo, another conversation or
an agent's memory. SOURCE-PROVENANCE.json identifies the exact project commit,
worktree branch and every original reference. Treat older task/agent documents
as evidence, not active instructions. This TASK.md is the current assignment.

Full destination: Table3/Section6.3 t2, N=2^15,h128,dnum11,auxiliary P60,
Base50x2,Mult60x8,Div40,input logical scale2^100,eight repeated squarings,
1000 executions and the paper's reported average precision about81.8bits.
Section6.3 does NOT need the6.2 intermediate recombine/decompose refresh.
Do not redefine success as one multiplication or add an unnecessary bootstrap
condition. Section6.2's separate18-level refreshing experiment remains a later
explicit-capacity boundary; do not substitute it for Section6.3.

## Observed current engineering baseline
Current public DCP,RCB,Tensor2,Relin2,RS2,Mult2,pair Add/Sub exist; the first
multiplication returns RefreshRequired and deliberately rejects another Mult2.
That is an interim safe boundary, NOT a final requirement to keep repeated
multiplication impossible. No old local implementation or modified OpenFHE is
permitted. No production changes from recent precision work.

First-Mult2 high precision is now ACTUALLY GREEN:
tested source47907783a6141d0174da79eae264d779fc598f28;
https://github.com/leemaple/20231788./actions/runs/33873114880
Linux job101023587797:focused1/1,full55/55,full1.03s.
Windows job101023588186:focused1/1,full55/55,full2.30s.
Both warning-as-error default builds and5 explicit API builds PASS.
4fresh keys per invocation, focused+full on both hosts =16 actual samples.
All16 slots and distinguishing product delta passed fixed<=2^-80.
Worst max-slot1.6696072195146116607129673424340031160212e-27;
worst delta1.6958307879080880932103073456218202834178e-27.
N64,batch16,depth7,p50,first55,FIXEDMANUAL,HYBRID,COMPLEX,
UNIFORM_TERNARY,HEStd_NotSet,degree2,input scale2^100.
q_div1125899906843009,q_l1125899906840833,
exact final scale2^200/1267650600226646386227681786497.
Two NONTRIVIAL lossless operands; independent exact RLWE/CRT/Horner oracle;
product delta=(2^-71+2^-75,-3*2^-74); observed input-product160/output210
headroombits, not universal bounds. First-observedGREEN, no manufactured RED.
Actual log metadata and every sample are supplied.

An original Pro visible chat described unity RHS, strict<,2P2 and other stale
details; its coherent ZIP/code/design/frozenJSON actually has two nontrivial
vectors,<=,4P2,28guards,CTest inserted at3. Read the retained discrepancy
disposition. Do not perpetuate that chat prose. No hosted result can be inferred
from an assistant claiming it ran code. Independent ZCode static final review
of the new precision patch is already LIVE in its own task; do not wait for or
contact it. BV fixed-key functional diagnostics and Pair Add/Sub composition
are independently tested/reviewed isolated branches, not part of this snapshot.
Do not edit their unrelated scopes.

## Concrete source findings to decide, not instructions to accept blindly
The three new source-audit notes and complete commit-pinned upstream files are
provided. Independently verify them and either use or disprove each claim:
1. Existing Relin2 raises high by q_div into the original full basis ONLY for
the first pair. After one RS2, illustrative full[q0,q1,q2,q3,p],active[q0,q1,q2]
raises to[q0,q1,q2,p], NOT original prefix[q0,q1,q2,q3]. HYBRID consumes context
partition/complement tables and Q key rows by index. Removing guards, setting
level0, or mutating m_params alone is not a correct second operation.
2. Public ILDCRTParams ordered moduli/roots, CryptoParametersCKKSRNS constructors,
public PrecomputeCRTTables, SchemeCKKSRNS and CryptoContextFactory expose a
possible immutable per-basis route, not an adopted or runtime-verified design.
Normal KeyGen requires factory registration. Factory interns equivalent
contexts; distinct object allocation alone does not guarantee distinct context.
Scheme.Enable(KEYSWITCH) alone is a no-op; explicitly select the implementation.
3. HYBRID ACTUAL PrecomputeCRTTables rejects a fresh nonempty full basis<=10
limbs with numPartQ11; EstimateLogP's clamp is not applied there. Explain whether
fewer active partitions preserve the paper's fixed decomposition convention,
or are a genuine departure, with paper/source reasoning. Do not silently
change dnum, pad a basis, or promise a single full-context key works on arbitrary
subsets. Distinguish initial dnum from per-level active partitions.
4. Precompute generates P from auxBits and skips current-Q collisions; no
explicit P-vector argument. Across basis families P equality is not automatic.
Explain requirements on keys/tables/P, not just matching Q primes.
5. FIXEDMANUAL arithmetic drops the actual trailing prime, while scale metadata
uses2^p; its standard Decode normalizes from p/noiseScaleDeg. SetScalingFactor
alone does not repair arbitrary physical scale. FLEXIBLEAUTO is not a magic
fix (the40/60 order can violate its ratio check). Give exact rational logical
scale progression; distinguish component scale, level and noiseScaleDeg.
6. Public normal sparseKeyGen uses h192, not h128. A low-level sampler supports
h128 at N>=128; setter-only secret replacement leaves existing key material
wrong, and tag caches can retain stale evalkeys. The provided audit identifies
a debugging-only Multiparty helper and an untested EncryptZeroCore-based key
construction possibility. Neither is a production endorsement. Define matching
key/secret relationships and deliberately chosen PREMode/key basis.
7. Manual context construction bypasses normal parameter/security checks.
HEStd_NotSet or a merely stored HEStd_128_classic is not performed validation.
Actual Q/P,h-distribution and exposed related-key family must be documented
before security claims. No h-aware security evaluation has run.

The required exact Fable5.1 terminal route has twice returned403 before
inference, with no model tokens/usable review. It is unavailable, not another
version's review. Codex continues with Pro/ZCode and executable evidence.

## Priority tasks and bounded implementation scope
A. Decide the minimal correct OpenFHE-based route for SECOND and then EIGHTH
Mult2. Compare only concrete viable alternatives, cite exact APIs/lines, and
identify what must change in project-owned state/scale/parameter/key handling.
Prefer KISS/YAGNI, explicit small immutable structures and the existing public
operator seams. Do not invent a generic FHE backend framework or modify upstream.
Do not use secrets/decryption/re-encryption in evaluator operations. Secret-key
access belongs only to client setup and the independent test oracle. A proposed
key-family setup must support evaluation after setup without retaining/using s.
If a route cannot preserve that boundary, reject it explicitly.
B. Freeze and return a minimal executable second-Mult2 regression against the
current public seam BEFORE production extension. Prefer a small homogeneous
diagnostic first, if that isolates repeated-lifecycle correctness, but also
state precisely how its mechanism handles Div40/Mult60 and does not dead-end
before the paper target. Use two successive nontrivial multiplications or
squarings with independently frozen lossless vector/product expectations.
Current unsupported-state rejection is the expected baseline behavior; an
actual runtime rejection may be the genuine RED once Codex runs it. Do not
assert a runtime RED without execution, add deliberate defects or change a
test oracle/threshold after seeing failure. Distinguish any API compile RED.
C. Draft the MINIMAL production/setup adapter and corresponding complete files/
second patch to satisfy that frozen test, with explicit ordered-basis/key
routing, exact logical scale, levels/state transitions and no mutation of
caller inputs/shared contexts/key material. Keep invalid states rejected.
Use public pristine OpenFHE primitives, not a homegrown crypto implementation.
Do not copy internal upstream algorithms wholesale into a parallel backend.
Stage1 test patch must apply to CURRENT project snapshot; Stage2 implementation
patch must apply on Stage1. Complete final files must match patch replay exactly.
Current55 registrations and old tests remain unchanged except explicitly
justified additive coverage; no weakening existing<=2^-80 first-Mult2/precursor.
D. If source constraints prevent a defensible complete second-Mult2 patch now,
give the exact bounded unresolved decision and return the smallest executable
PUBLIC-API construction/key-routing probe needed to decide it, with predicted
observations and frozen acceptance. Do not return another generic TODO list.
A source-supported uncertainty is not proof of impossibility. Do not hide a
new prerequisite behind a no-op production stub, metadata relabel or refresh.
E. Define the next empirical gates: asymmetric ordered-Q and key setup,
second then eight squarings, paperN/h/dnum/P and1000-trial precision/performance.
A test-only low-N probe is permitted for development but cannot be relabeled
as paper reproduction. Production lossless codec must be usable without the
current stale binary64 cache; current test fixture/oracle is not that codec.
Do not expand this patch into a full codec unless essential to the frozen
second-Mult2 executable slice; give an explicit remaining integration boundary.

## Required tests and claim boundaries
Use exact integer/rational normalization, independent plaintext arithmetic and
independent reconstruction for each stage. Cover real/complex, signs, zero/
near-zero, sub-binary64 distinguishing information, magnitude constraints,
ordered basis and actual prime identities, selected key family and row basis,
valid state progression, invalid-state rejection, no input/key/context mutation.
Staged/direct self-comparison is wiring only, never the sole arithmetic oracle.
Freeze parameter configuration, vectors, fresh-key count and operation-specific
precision threshold before hosted execution; derive why the threshold is
plausible rather than copying it blindly. Desired2^-80 for a bounded second
diagnostic must be justified; no silently weakened target or fake successful run.
An observed centered headroom bound is not a universal Gaussian/key guarantee.
Do not impose an all-key theorem as an invented completion prerequisite.

Codex executes actual Linux/Windows GitHub Actions, Debug,
-Wall -Wextra -Wpedantic -Werror, --parallel2, pristine pin:
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
five explicit relin2_api_contract_test,rs2_api_contract_test,
mult2_api_contract_test,add_api_contract_test,sub_api_contract_test targets
focused new CTest, then ctest --test-dir build --verbose --output-on-failure.
No Mac project/OpenFHE compilation or crypto/benchmarks. No source/code lookup
from old local implementations. No network account/credential operations,
git push/merge, CI dispatch, external messages or broad exception swallowing.
Pro may do source inspection, bounded exact arithmetic, patch/hash validation.
Report any actual environment execution separately, with exact outputs; do not
claim Codex hosted results as runs in your environment.

## Deliverables and observable acceptance
One downloadable ZIP and matching SHA256 sidecar:
- DESIGN_DECISION.md resolving the seven concrete source questions;
- FROZEN_SECOND_MULT2_CONTRACT.md plus machine-readable exact vectors/oracles;
- SOURCE_CLAIM_TEST_LEDGER.md with exact pinned file/line citations;
- staged applicable patches and COMPLETE final changed/new files;
- EXPECTED_CTEST_BINDINGS.tsv preserving55 prior name/command pairs;
- ACTUAL_EXECUTION_LEDGER.md separating source/static/compile/runtime/not-run;
- NEXT_PAPER_GATES.md with scope, order, remaining risks and accountable tests;
- complete manifests with per-file sizes/SHA256 and verified patch replay.
If delivering a bounded probe instead of the complete second-Mult2 candidate,
label that unmistakably in the README and ledger with the exact gate it answers.
Verify every named file really exists and is nonempty; ensure final chat agrees
with the archive's ACTUAL scope, vectors, thresholds, filenames and findings.
Return inspectable evidence, a defensible decision and executable code; do not
claim the full paper goal or security complete from a small diagnostic.
