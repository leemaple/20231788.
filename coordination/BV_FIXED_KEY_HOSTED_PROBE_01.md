# Fixed-key BV bound: first hosted diagnostic observation

## Frozen candidate before execution
Branch codex/bv-fixed-key-01, dedicated clean worktree
/Users/lifeng/Documents/20231788-openfhe-codex-bv-fixed-key-01.
Created from clean/pushed integration8a465764044d8b1e1578f462ea4916f7123428a4.
That baseline's active53-case source was tested atd73824c2d382013c3aadbd7cb29c57008e839714.
This branch is separate from codex/precision-01's54-case precursor experiment.

The complete Pro candidate and follow-up source review were inspected by Codex:
coordination/PRO_BV_FIXED_KEY_RETURN_C7CD790.md and
coordination/PRO_BV_CENTERED_LIFT_CLOSURE_A151FC6.md.
The former missing centered-lift premise is independently source-closed;
neither review claims hosted execution of this candidate.

Rechecked baseline oracle SHA256
b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26.
Original Pro patch SHA256
c70a5963909bb1fc1c4f5bdabbd5baba7d013dab7f5cdd9b7ebaf325a2545871.
git apply --check -p2 PASS; transferred through apply_patch.
Active resulting oracle is BYTE-IDENTICAL to Pro's archived full candidate:
SHA256 92a2f03c0301ba0e16d6d52f72e2fef129f069bbdee2b27e6a25b7c49030538d.
Exact oracle delta363 insertions/3 deletions. No candidate reconstruction,
coefficient/tolerance edits, bound tuning or claim-label escalation.
The only other non-document change admits codex/bv-fixed-key-01 to the existing
CI push allowlist. Jobs, pin, warning flags, dependency/build/test commands,
resource caps, all53 CTest names/bindings and API targets remain unchanged.

## Pre-execution interpretation
This is an additive test-only diagnostic at already implemented public
Mult2/Relin2 seams. It is not a manufactured missing-feature red/green.
First hosted result must be recorded as actually observed PASS/FAIL.
The exact fixed-key formula, source-lift witnesses, input vectors,1e-3 functional
tolerance and half-modulus inequalities are frozen before that observation.
Any compile/runtime failure must be retained and diagnosed; no fitting the bound
to observed ciphertext errors, weakening a threshold or removing a check.

BV REAL/COMPLEX must execute key-residual bound construction before any
plaintext/encryption or measured path error. Require four source-boundary
witnesses,8/7 digit counts,zero eighth high digit, seven active residual rows,
key immutability, nontrivial per-path/pair bounds, independent observed errors
below them, explicit pre-RS/final integer-lift checks and final coefficient bound.
The full mandatory53-suite includes those two BV cases; HYBRID and all prior
regressions plus warning/public API targets must also pass.
No additional focused rerun or unbounded fresh-key repeat has been launched.

Retain PER_PATH_CONDITIONAL,conservative_E_Relin_available=false and
universal_theorem_gate=UNPROVED. Fixed-key ciphertext-uniform key-switch evidence
is not an all-key Gaussian tail, unrestricted no-wrap, true precision, repeated
multiplication, paper-scale security or performance result.
No Mac compile or cryptographic run is authorized or performed.

## Next owner
Codex observes exact-SHA CI, retains full project logs and checks actual53-name
closure plus new certificate fields. ZCode must independently review the final
candidate/evidence at a subsequent boundary; its current precision precursor
review remains uninterrupted. Pro first-Mult2 precision work runs independently.

## First hosted observation: dual-platform PASS

Exact source5b5a4152076d43868a9dbad193807f2ede25e04d was pushed and its
remote branch SHA matched. Run33866620400 completed SUCCESS:
https://github.com/leemaple/20231788./actions/runs/33866620400
Linux job101002832323:53/53,1.00s total, completed19:12:00 Asia/Shanghai.
Windows job101002832196:53/53,2.13s total, completed19:17:28 Asia/Shanghai.
Both default warning-as-error builds and all five explicit public API build
steps succeeded. Linux used a verified pristine-install cache; Windows actually
configured/built pristine OpenFHE1.5.0. No Mac compile or crypto run occurred.

Retained full project configure/build/CTest/API log sections:
- artifacts/tdd/bv-fixed-key-bound/linux.txt
  SHA256 fbcf6bc8ab594853585efb17c18e7d3020162bc9231a0e9bba40e064d6604903
- artifacts/tdd/bv-fixed-key-bound/windows.txt
  SHA256 acfb4fb457182de8c6e923971bdc571bd6dacfa21f2c5a255019e37edf0c79e1
Only ANSI/CR/trailing whitespace normalized in these new retained logs.
Codex matched both actual53 unique PASS names to current CMake, exact closure.
An initial ad-hoc closure command had an extra closing brace and did not run;
its corrected read-only parser passed. No source/test/log was changed to fix
that command. This was not a compiler or CTest failure.

All four actually generated BV keys (REAL/COMPLEX per host) reported seven
active residual rows,zero final high digit,centered boundary probe PASS,
nontrivial fixed-key per-path/pair bounds, explicit integer-lift/non-wrap PASS,
independent coefficient PASS and unchanged1e-3 functional tolerance.
Per-path bounds / pair bounds, respectively:
- Linux REAL:4879082475520 /9758164951040
- Linux COMPLEX:3813930631168 /7627861262336
- Windows REAL:2714419113984 /5428838227968
- Windows COMPLEX:3813930610688 /7627861221376

Separately from the hosted tests, Codex recomputed the logged key bounds from
all seven exact moduli/radii and row residual norms with BigInt arithmetic,
rechecked every observed high/low/pair error, independently cross-multiplied
both no-wrap conditions and verified the conservative coefficient numerator/
denominator comparison. All four records matched exactly. These lightweight
log calculations are not new cryptographic executions or independent key draws.
Complete API-step metadata and exact per-case checks are retained in
coordination/evidence/bv-fixed-key-bound/dual-platform-5b5a415.json.

This is FIRST-OBSERVED GREEN for the additive diagnostic, not manufactured
feature TDD. The candidate oracle hash is unchanged from the frozen reviewed
92a2f03c...0538d. There was no threshold/vector/formula tuning after execution.
The fixed-key bound is source-backed and ciphertext-uniform for each specified
key/basis; downstream integer-lift checks remain input-envelope-specific.
PER_PATH_CONDITIONAL,conservative_E_Relin_available=false,
universal_theorem_gate=UNPROVED and the separate candidate status are retained.
No all-key Gaussian-tail, >53-bit precision, repeated multiplication,
paper-scale/security/performance or full completion claim follows.

Independent ZCode final review of this exact candidate/result remains pending;
do not confuse its previous theoretical lead with a final code review.
The branch remains isolated and unmerged. Pro first-Mult2 precision drafting
and ZCode precursor audit continue independently, without interruption.
