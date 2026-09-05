# Table 3 parameter discovery probe 01

## Scope and pre-recorded oracle

This is an isolated, source-mapping **discovery experiment**, not a production
feature, new CTest, genuine production RED/GREEN, or a paper-result claim.
The existing 57 CTest bindings, five explicit API targets and production
source remain unchanged. The user has delegated ordinary technical/seam
decisions; no further user approval is needed for this read-only upstream
parameter-construction experiment.

The sole seam is pristine OpenFHE 1.5.0 public CKKS parameter construction and
public parameter getters at commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The exact numerical oracle was
recorded **before** the probe in commit `94dfee8`:
`coordination/PAPER_TABLE3_RESERVED_P_CANDIDATE_01.json` and its adjacent MD.
Returned values must not be reused as their own expected oracle. In
particular the upstream `RootOfUnity` result is compared against pre-recorded
literal roots. A disagreement is a finding, not permission to silently change
the expected values.

Inspect full B0..B8 and A0..A8 parameter families at N=32768, local dnum=L,
alpha=1, ordered Q/root pairs, reserved P and ordered QP. Keep the distinction
between full family basis and a shorter active prefix explicit. A separate
reserved-P-in-Q collision control checks that upstream skips the occupied P.
This is not an instruction to impose common P on the ongoing low-N repeated
Mult2 implementation task.

No secret is sampled; no key, ciphertext, encryption, decryption, DCRT
polynomial or multiplication is created. Parameter/CRT precomputation runs
only on GitHub-hosted runners, with bounded parallelism. The Mac is used only
for source editing/review, lightweight exact arithmetic, and Git operations.
The official precompute path **does initialize NTT tables** for Q/P and HYBRID
tables. This is not an assertion of zero NTT work; no encryption/evaluation
operation is invoked. The source audit identified the EncodingParams overload
as necessary for explicit PREMode=NOT_SET, and getters must be inspected from
the returned context, not merely the input parameter request.

The collision control replaces Mult0 in the full 11-limb Q with reserved P;
the pre-recorded expected replacement P is the removed Mult0 modulus
`1152921504598720513`, root `100545759574150`. Keeping Mult0 in Q as well would
cause a further skip and is not this control. Never equate a fresh shortened
family with a shorter active ciphertext prefix in the full 11-limb context.

## Isolation, execution, and gates

- Worktree: `/Users/lifeng/Documents/20231788-openfhe-paper-table3-probe-20260905`.
- Branch: `codex/paper-table3-profile-probe-01`.
- Source base: `cfe2fbc072bbd2154443ef71a641eabe449f34f5` (documentation after
  the actual tested code baseline `4ecbd972429884489918d9f82dfc3fe9f702ef4a`).
- New target: `table3_profile_probe`, `EXCLUDE_FROM_ALL`, no CTest entry and no
  link to the project evaluator implementation.
- Existing `dcp-rcb.yml` is manually dispatched at this branch's exact pushed
  source; it retains both full platform suites and explicit API builds, then
  explicitly builds/runs this diagnostic target with warning-as-error.
- Record source SHA, run URL, actual runner results, failed assertions and
  toolchain before adopting any candidate as a subsequent production oracle.
- No merge into default/mainline as part of discovery. Any later production
  implementation must still pass a genuine, separately recorded RED/GREEN
  cycle at its agreed public seam and independent review.

## Evidence status

Preparation only at creation. Compilation/runtime/CI for this probe: **NOT
RUN**. Three Pro implementation tasks remain independent and uninterrupted.
Success here would only support upstream parameter representability. It does
not establish h=128 keys, security, high-precision client I/O, repeated semantic
Mult2, eight squarings, a 1000-run error result, or performance.
