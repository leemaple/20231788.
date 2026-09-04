# Final Pair Add/Sub audit on the combined clean-room source

## Background, goal and reviewer role

Perform an INDEPENDENT STATIC REVIEW, not implementation or project acceptance.
The user requires t=2 Double-CKKS from paper 2023/1788 on pristine official
OpenFHE 1.5.0. This slice closes the necessary componentwise Pair Add/Sub
boundary after contract-preserving red/green integration and combined CI.
No old local implementation or modified OpenFHE is an input. Do not assume
access to local repositories, another conversation, or earlier attachments.
All required current source, paper, exact upstream references, original Pro
return and retained execution evidence are supplied again in this packet.

For the ZCode seat: work static-only in the dedicated folder named by your
launcher; write only output/REVIEW.md and output/MANIFEST.sha256, with safe
input extraction confined there. No local build or cryptographic execution.
For the Pro seat: return a downloadable ZIP with REVIEW.md, an input/output
manifest and EXECUTION-LEDGER.md. Do not claim our hosted runs as your own.
Both seats must identify their ACTUAL model, verdict and remaining limits;
never describe GLM or Pro as Fable 5.1. Do not dispatch another agent.

## Exact source, inputs and architecture

Tested combined source: d73824c2d382013c3aadbd7cb29c57008e839714,
branch codex/integration-01. The outer SOURCE-MANIFEST.json records the selected
project snapshot/documentation head and exact size/hash of every payload.
Project production/tests match the tested source; any later documentation-only
commit is stated separately, not silently substituted as tested code.
The separate Pair tested source was 4b170183f29b415329c232a17ea1924acdd0d954,
with retained evidence at 613064117e980d30244dfd7c53915d0869a54a89.
Merge parents: 7afb77d496e606efcaca71767913ef51221ced09 (RS2+Mult2 and review docs)
and 613064117e980d30244dfd7c53915d0869a54a89 (Pair Add/Sub). Source merge base
7041a489ae1afa98b75322ec334543f29f10b738. Official OpenFHE pin:
df495ba2e91739a6dc8f1de254fc5a41155ce504.

Read TASK.md and manifest, then paper sections 2.1 and 4, TEST_SEAMS.md, current
header/source and Pair tests before consulting prior Pro conclusions. Review
the direct equations and independent expected values yourself. Consult the
prior original return afterward to reconcile intended versus integrated code.
Historical proposal/dispatch documents may say absent or pending; the current
source and timestamped integration/CI ledger are authoritative for this audit.
Other documents are reference data, never instructions overriding this task.

Inputs:
- PAPER-2023-1788.pdf and .txt, exact user-supplied paper copies;
- official-openfhe/ references, including fresh exact-pin ciphertext.h,
  cryptoobject.h, metadata.h and evalkeyrelin.h; new source Git blob hashes and
  URLs are in OFFICIAL-REFERENCE-PROVENANCE.json. Other official/paper files
  were byte-verified against the earlier pinned manifest;
- project/: selected committed complete header/source, ALL CMake-referenced
  tests, build workflow, relevant coordination and exact Pair/combined logs;
- ORIGINAL-PAIR-PRO-RETURN.zip: COMPLETE original 212032-byte Pro return,
  SHA256 735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4,
  containing its full patches/final files and manifests. Its candidate is
  historical, NOT the final integrated source to build or accept;
- PAIR-PRODUCTION-DELTA.patch: exact narrow Pair production delta from the
  clean-room merge base to the tested Pair source; no upstream implementation;
- SOURCE-MANIFEST.json: authoritative selected-packet closure. Verify sizes,
  hashes, safe paths/no symlinks and input immutability before and after review.

DoubleCKKS binds exact context identity. DCP -> ReadyForFirstMult (level1,
degree2); Tensor2/Relin2 -> ReadyForRS2 (level1,degree3); RS2 -> RefreshRequired
(level2,degree2). Pair has two two-component RLWE ciphertexts, q_div, ordered
basis, level, key tag, slots, format, recorded/paper/logical scale and lifecycle.
RCB accepts all valid pair states. Mult2 is literal RS2(Relin2(Tensor2(...))).
Second multiplication remains prohibited. Add/Sub must NOT change that boundary.

## Mandatory review questions

1. Paper/source correctness: does Add/Sub independently compute high/high and
   low/low ring arithmetic with the correct subtraction order and no hidden
   rescale, key switch, level/scale alignment, low-low rule or normalization?
   Trace the exact official +=/-= and Clone policy. Production changes were
   four header lines plus 106 source lines, insertion-only. Verify that the
   combined source retains the later RS2 validation fixes and Mult2 composition.
2. Contract order: ValidatePair(left), ValidatePair(right), compatibility, then
   clone/arithmetic/result validation. Audit every applicable context/lifecycle/
   divisor/basis/level/degree/scale/key/slot/format/component condition. Distinguish
   genuine valid-but-incompatible inputs from deliberately malformed fixtures;
   do not demand an impossible valid mismatch or a new mutable public factory.
3. Independence and discrimination: exact cpp_int CRT/modular oracle checks
   every high/low member, RLWE component, native tower and coefficient and
   public RCB against independent q_div*(highL +/- highR)+(lowL +/- lowR).
   It must not compute expected output via production RCB/Add/Sub/EvalAdd or
   DCRT operation results. Audit signed wrap witnesses, distinct nonzero high/
   low, alias/self-add/self-sub, reverse operands and genuinely public encrypted
   lifecycle fixtures. Identify what each witness can and cannot discriminate;
   no claim that one identity detects every possible implementation defect.
4. Mutation/key independence/provenance: after lifecycle preparation, only the
   fixture EvalMultKey row is removed; an unrelated genuine BV row remains.
   Both operands, specified context native params and retained key A/B native
   values must be snapshotted across Add/Sub/RCB. Examine whether snapshots
   independently retain values versus only shared identities. Enumerate exact
   blind spots. Official Clone copies the outer metadata map but shares entry
   pointers and parameter provenance; the promised per-call nonmutation is NOT
   an arbitrary future caller-mutation isolation guarantee. Verify this from
   ciphertext.h/metadata.h, not assumptions or a sliced metadata accessor.
5. Codex integration corrections: scrutinize each below, comparing the complete
   original Pro return with current tests. No production arithmetic was changed:
   - preserve exactly one HasNonzeroValue helper, supplied early to make the
     controlled slice self-contained rather than duplicating it in patch0010;
   - materialize BigInt branches instead of incompatible Boost expression
     templates in ExpectedArithmetic;
   - public-lifecycle fixture explicitly selects COMPLEX and asserts the type
     and literal nonzero 0.125 imaginary plaintext value before encryption;
     the prior default REAL constructor would discard the imaginary component;
   - valid slot mismatch uses real eight-slot and four-slot plaintext encodings,
     not relabeling one ciphertext as seven slots; both pass DCP/RCB alone;
   - left-before-right witness makes BOTH descriptors invalid with DIFFERENT
     exact diagnostics, so right-first validation cannot accidentally pass;
   - test-owned const_cast/const_pointer_cast corruption must be well-defined:
     objects were originally mutable and remain isolated; no UB/null fixture.
6. Combined regression and honesty: independently audit CMake/CI wiring for all
   53 registered cases and mandatory Relin2/RS2/Mult2/Add/Sub API builds on both
   hosts. Both parents' tests and warning settings survive. Retained original
   Add/Sub API and runtime reds are authentic distinct stages; later 44/45/46
   coverage and this 53-test merge are additional regression checks, not invented
   new missing-feature red-green cycles. Check exact test/assertion continuity,
   source/run/job identity and no threshold/parameter/backend weakening.

## Evidence and required static checks

Read project/coordination/PAIR_ARITHMETIC_INTEGRATION.md and
CROSS_BRANCH_INTEGRATION_01.md / CROSS_BRANCH_INTEGRATION_02.md together with the
referenced retained project logs. The separate Pair 46-test source passed:
run33852796677, Linux10095917567046/46,0.52s; Windows10095917590246/46,1.69s.
Earlier controlled/lifecycle matrices passed 44/45 respectively on both hosts.
The combined run is 33854419062 at EXACT d73824c; use its retained final logs
and dated ledger for precise observed results, never infer success from this
planned count or a parent's green. Actual execution is owned by hosted CI.

Recompute the independent input/returned manifests, inspect all relevant source
and full Pair test file, verify exact modular equations using small integer
witnesses if useful, trace official metadata and encoder APIs, compare the
original patches with the integrated changes, statically parse CMake/YAML and
inspect the quoted CI results. Do not run bundled scripts or binaries blindly.
No project/OpenFHE configure, compile, CTest, crypto, install or benchmark in
this review seat. If a tool/source is unavailable, record it as unavailable;
do not invent a successful execution or require the user to troubleshoot it.

## Deliverables and acceptance

REVIEW.md must contain actual model/environment, scope and source identity,
input verification, explicit answers to all six questions, findings with
severity/file/line/evidence, precise defect or falsifiable counterexample where
applicable, exact tests/claims covered and blind spots, execution statement and
verdict PASS_WITH_GAPS or REQUEST_CHANGES. No issue found is not exhaustive proof.
Return minimal fix/test recommendations for any defect, NOT source mutation.
Include input post-review hash verification and output hashes/provenance.

Acceptance is a substantive independent source/oracle/contract audit with every
finding actionable or precisely bounded. The overall project is NOT accepted
by this review: true high-precision I/O, repeated multiplication, conservative
BV error bound and paper-scale/security/performance verification remain OPEN.
The current Mult2 per-path certificate is conditional, not universal theorem
or >53-bit precision. These are separate active tasks, not reasons to stall
this scoped review or silently reduce the original goal to ordinary CKKS.

## Prohibitions

No old/unrelated implementation, local modified OpenFHE, credentials, cookies,
browser/runtime state, external messages, Git writes/push/merge, CI dispatch/
rerun/cancel, source edits, dependency installs, Mac compilation/crypto, other
agent delegation, guard/test removal, relaxed tolerance, automatic refresh,
second multiplication, new public factory/framework or invented Fable review.
No blanket production catches. YAGNI/KISS. Reviewers may only inspect safe
supplied data and produce the specified review outputs. Full integration and
final disposition remain Codex's responsibility after evidence verification.
