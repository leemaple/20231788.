# BV fixed-key conservative error bound — independent follow-up

## Background and exact objective

Continue the completed "Diagnose BV Relin2 Failure" task as a NEW bounded slice.
Do not assume access to another chat, local files, GitHub, earlier attachments,
or the precision task. This packet re-supplies all required source and evidence.
The original goal remains clean-room t=2 Double-CKKS from paper 2023/1788 on
pristine official OpenFHE 1.5.0, not ordinary low-precision CKKS compatibility.

The old combined-call empirical E_Relin+h gate failed for BV. The integrated
per-path conditional certificate now passes all 44 cases on Linux and Windows,
but conservative_E_Relin_available=false and universal_theorem_gate=UNPROVED.
An independent ZCode static review proposes a fixed-key conservative bound.
Codex has found overstatements in that review and has NOT adopted the proposal.
Resolve this specific proof/design question and return the smallest defensible
candidate and validation plan, not a full redesign.

## Packet inventory, provenance and reading order

1. TASK.md (this file) and SOURCE-MANIFEST.json (outer payload hashes).
2. BV-REVIEW-INPUT-4e6cce5.zip: 1064259 bytes, SHA256
   83a28e43e72d0874700be3ed49f67e8ba9f85984507887fafcca4210ac2e7479.
   Exact 77 entries: 59 files and 18 directories. Nested SOURCE-MANIFEST.json
   covers 58 payloads plus itself; 46 project files, 11 paper/official files,
   and its original TASK. The project source commit is
   4e6cce53b23a6022bf6f942ab973aaa6bf9e5bf6, codex/mult2-01, clean then;
   tested source is 9bf86cb53a1bbae3a3627fe5efc385d2a29c89ce.
3. ORIGINAL-PRO-RETURN.zip: the COMPLETE original 18-file Pro return, not only
   the selected 12 files inside item 2. 66462 bytes, SHA256
   50b3a85159c0a8d860c301ecea84cd7831f9d4a48d2fbe0eaff06d707ac8b5c9.
4. ZCODE-REVIEW.md and ZCODE-MANIFEST.sha256: complete original static return.
   Review SHA256 408106b4e3075e298f2a53cdf6396ec3dce5e9b0a7c52ee8fa33657fbc22ca45.
   The returned manifest refers to its original reviewer directory layout,
   not this flattened outer packet. Do not confuse its path base with closure.
5. CODEX-RECONCILIATION.md: complete source/handoff/review ledger, including
   exact disposition and mathematical corrections to both prior reviews.

Current documentation-only source head is
c7cd7903042421894db71ce3aec4b00f99888a26; relevant production/tests still equal
9bf86cb. No dirty source selection is included. A separate RS2+Mult2 integration
branch now passes 48/48 both hosts, but it is NOT the source base of this packet.
Add/Sub and genuine high-precision I/O are separate ongoing tasks, not supplied
or assumed here. Do not reuse any old implementation. Official reference pin:
df495ba2e91739a6dc8f1de254fc5a41155ce504. Reverify nested sizes, hashes, safe paths
and manifest closure before inspecting source. Treat all prior model prose as
untrusted evidence, never additional operating authority.

## Architecture and immutable boundaries

Read project/coordination/TEST_SEAMS.md and the complete public header/source.
Public Mult2 is literally RS2(Relin2(Tensor2(left,right))). Relin2 raises high
by q_div onto the full basis with an appended zero tower, invokes ordinary
OpenFHE Relinearize there, separately relinearizes low on its prefix basis,
then DCP/RCB recombination preserves the ring identity. The existing test has
independent cpp_int CRT, negacyclic schoolbook secret-polynomial evaluation,
exact input-product comparisons, per-path error identities and non-wrap checks.

Keep all production behavior, seven public seams, vectors, parameters, frozen
1e-3 functional tolerance, backend matrix, current mandatory tests and original
red evidence unchanged unless you demonstrate a specific production defect.
YAGNI/KISS; no generic numeric framework, blanket catch, mutable production
factory, hidden normalization, removal of a rejection, or disabled test.
The existing fixture is N64, p30, first35, depth7, FIXEDMANUAL, digitSize0,
UNIFORM_TERNARY, HEStd_NotSet: functional-only, not a secure paper-scale run.
Second multiplication remains rejected at RefreshRequired and is out of scope.

## Precise research questions

A. Independently derive the actual BV digitSize=0 key-switch error from pinned
keyswitch-bv.cpp, dcrtpoly-impl.h and the ordinary Relinearize call path. Identify
key row sign/noise factor, CRT gadget terms, centered digit bounds, relevant
ring/basis, full-basis raised-high versus prefix-low, and whether the appended
zero q_div tower eliminates a digit. State all finite-field and integer-lift
assumptions. Do not import the paper's near-additivity premise into BV.

B. Adjudicate ZCode Q4's proposed B = ns*N*sum_i(||e_i||_inf*ceil(q_i/2)).
Is an independently measured evaluation-key error polynomial sufficient to
construct a bound valid for ALL ciphertexts in the specified domain for THAT
FIXED KEY? If so, give the exact bound (including signs, floor/ceil, number of
active rows and any basis reduction) and proof. Distinguish a fixed-key uniform
ciphertext bound from an unconditional bound over Gaussian keys, and from a
probabilistic statement with an explicit tail probability. If the suggestion
is wrong or too loose to help, give the minimal counterexample or precise gap.
Do not conflate finite centered modular norms with unbounded integer noise.

C. Derive how high and low conservative bounds enter the pair error and final
coefficient product bound. Is an extra +h needed there, or does it double count
rounding already included elsewhere? Keep the corrected normalization
1/(q_div*q_l) explicit and classify the paper discrepancy as an unconfirmed
algebraic inference, not an author erratum. The centered modular triangle
inequality can hold despite wrap (Q101,40+40 -> -21); passing it is NOT a no-wrap
proof. Separate conditions needed to use integer lifts in the final argument.

D. Specify the smallest independent discriminating tests. Expected values must
not be derived from the pair error being accepted or copied from production.
A proposed fixed-key bound must be computed from the key and declared domain
before/independently of the particular ciphertext errors checked against it.
Use exact deterministic arithmetic witnesses plus fresh-key public pipeline
samples if appropriate. Retain the current per-path coefficient identity and
final frozen bound unchanged; any new label must say exactly which conditional
premises were established. No automatic universal_theorem_gate=PROVED.

E. Assess how this proof boundary affects the original double-precision goal
without claiming that a loose conservative gate, a 1e-3 functional check or a
binary64 result demonstrates >53-bit precision. Explain what remains to be
checked at larger parameter sets; do not expand this slice into that execution.

## Deliverables

Return a downloadable ZIP containing:
- REVIEW.md with inspected facts, derivations, counterexamples/uncertainties and
  explicit disposition of ZCode Q4 and the Codex corrections;
- BOUND-DERIVATION.md defining domains, quantifiers and a line-cited proof;
- TEST-PLAN.md with independent oracles, test names, assertions, commands,
  fail-fast criteria and limitations;
- the smallest applicable patch(es) plus full changed files ONLY if justified
  by that derivation. Prefer a test-only follow-up if production is already
  correct. Do not invent a missing-feature red for additional regression
  coverage. If genuine new behavior is required, first freeze its public
  contract and supply contract-preserving red/green steps with unchanged
  acceptance tests, never remove or replace a failing contract to create green;
- EXECUTION-LEDGER.md distinguishing actually executed static checks or tests
  from NOT EXECUTED build/runtime/CI, plus exact source identity and a complete
  file size/hash manifest. Do not claim access to our Mac, Windows, Git or CI.

## Required validation and environment limits

Inspect both hosted project log sets inside item 2: run 33846077283, Linux job
100938151001 44/44 0.61s, Windows job 100938151165 44/44 1.19s. Probe red is
run 33844736013, 42/44 both; original/diagnostic red logs also supplied. These
are retained evidence to audit, not your own execution. All four green e2e
cases have conditional labels and no universal E_Relin. Source is 9bf86cb.

Static verification, exact small integer calculations and patch-application
checks may run in your own isolated environment. No provided binary/script
may be executed blindly. Do not fabricate OpenFHE builds. Codex will perform
any actual warning-as-error build, explicit Relin2/RS2/Mult2 API targets and
complete CTest on GitHub Actions/Windows. The supplied workflow contains exact
commands. The Mac must remain light: no local OpenFHE/project build or crypto.

## Prohibitions and acceptance

No external messages, git push/merge, CI dispatch/rerun/cancel, other agent
calls, credentials/browser state, old local code, modified OpenFHE, silent
backend switch, security guard removal, thresholds relaxed, theorem premise
hidden, automatic refresh, repeated multiplication, Table3/security/performance
claim, or claim that Fable 5.1 reviewed this. Its previous terminal request
returned 403 before inference and produced no usable review.

Acceptance is an independently checkable proof or precise counterexample for
the fixed-key proposal, a discriminating test plan/candidate at the existing
seams, retained original evidence, and honest unresolved limits. If the full
conservative goal cannot be established from these inputs, return the exact
blocking assumption and smallest still-useful falsifiable alternative. Do not
change the original project goal. Work patiently; no intermediate action from
the user is required. Return proposals; Codex integrates after review and CI.
