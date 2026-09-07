# S100-FRESH-ERROR-REPAIR-01

## Renewed user authorization and immutable baseline

On 2026-09-07 the user explicitly asked to continue repairing the original-parameter precision discrepancy after the qualified S116 delivery. This is active engineering, not report-only work. Baseline e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb remains delivered and unchanged. New worktree: /Users/lifeng/Documents/20231788-openfhe-s100-fresh-error-repair-20260907; branch codex/s100-fresh-error-repair-20260907. No pre-clean-room implementation or modified OpenFHE is an input.

Objective: determine and, if a specific defect is established, repair the original S100 end-to-end precision discrepancy. Do not count S116 success, documentation, a diagnostic success or a relaxed test as original S100 success. Do not promise that a fix exists before evidence.

## Existing red signal and scope of this slice

The original actual remote command is the CTest paper_full_eight_square_contract, compiled from ed5fd192a89d6d4728ad295e87cf06a3f4abc832 in run34039088536/attempt1. Both hosts reached a complete eight-square chain, then CTest exit8. Linux nine numerical misses, Windows seven. Full-slot E8 about9.14647e-24 /9.06531e-24 >T=2^-80. The retained full-slot and independent replay show propagated aggregate fresh error dominates, conditionally; no specific production defect was found. Do not repeat that expensive chain merely to establish the already observed red baseline.

The current missing observation is separation of deterministic encoding from encryption contribution. Build a minimal real-client diagnostic at the already selected HighPrecisionClientIO seam. The user delegated ordinary seam/design decisions; no further user confirmation is required. Root owns integration and remote RED/GREEN, Pro preferably drafts the cohesive source/test slice, independent Codex challenges source/math while Fable5.1 is unavailable.

## Ranked falsifiable hypotheses

1. Fresh public-encryption contribution dominates: deterministic encode/decode error is much smaller than measured fresh error, and the fresh decoded value minus the encoded polynomial's ideal value accounts for it. Measure both for the SAME fixed S100 input/profile. No sampler modification.
2. Encoding contributes unexpectedly: coefficient rounding, canonical transform or precision conversion produces excessive deterministic error. Compare exact coefficients/independent polynomial evaluation to frozen inputs, plus exactly representable constant controls. Only a demonstrable defect justifies a production repair.
3. Paper and our contract differ: paper input distribution, chi_enc/chi_err, public-encryption details and average infinity-norm metric differ from the frozen stress input/component-maximum contract. Record established versus undocumented differences; paper-aligned experiments must be separately named, never silently replace the original test.

## Requested implementation package for ChatGPT Pro

Produce one bounded source/test deliverable, not another broad review. Inspect supplied paper/current source/official references first; state any fatal seam flaw before drafting. Implement a deterministic client encoding inspection sharing the EXACT encoding coefficient computation used by Encrypt. It must perform no encryption, key generation, secret access, evaluator work, randomness, zero-key simulation or fixture ciphertext injection. It should return enough owned diagnostic data (exact signed coefficients, scale/basis identity and/or decoded slots) for an independent test to isolate encoding error. Prefer one named method and a small result type on HighPrecisionClientIO; no general observer/callback framework. Root proposes InspectEncoding(values,spec), but a smaller equally discriminating public interface may be justified in DESIGN.md.

The actual Encrypt must use the same extracted encoding helper. Preserve existing validation ordering where relevant, accepted slots/scales, exact rounding semantics, ambiguous-half rejection, coefficient-wrap checks, official Poly/DCRT conversion and the public encryption call. No change to numerical algorithm, sampler, noise width, h128, ephemeral distribution, fixed input, Q/P, scale recurrence, encryption mode, public-key format, or original/S116 acceptance predicates. Do not add a new public raw encrypt or raw decrypt bypass.

Deliver two ordered patches against exact baseline: 01-red.patch adds declaration-independent calling tests and minimal build wiring, failing because the new behavior is absent; 02-green.patch supplies only necessary interface/implementation. Include complete changed files and a manifest. Keep changes limited to high_precision_client_io.h/.cpp, one new focused test file, and minimal opt-in CMake registration. Do not edit GitHub workflow: root owns it independently. No compiler is assumed in your environment; distinguish static checking from actual builds.

## Required tests and observation

1. Existing small public client setup, exact real-constant input controls with hand-derived coefficient0=S*c and all other coefficients zero; wrong slot count/scale, nonfinite input and supported error handling. Verify no secret/randomness is needed by inspection. Inputs and returned owned data must not alias/mutate the client or later results.
2. Existing N32768/h128 S100 plan and the unchanged paper_full_test::Inputs() dyadic input family. Exactly one fresh PUBLIC encryption; no DCP/Mult2/full chain needed for this first observation. Inspect deterministic encoding and compare its decoded/independent polynomial value to original input. Then compare the actual fresh decoded value to the encoded value and retain aggregate E0. Keep maximum locations distinct; also report all three signed quantities at each relevant extremal slot/component. Verify E0=encoding+encryption contribution at the SAME component within explicitly justified numerical tolerance.
3. Use independent binary512 ideal and existing sparse-CRT/Horner machinery for at least the existing ten anchors; no second copy of production FFT as the sole oracle. Controls must detect sign/order/scale/rounding errors. Any norm bound based on exact nearest coefficient rounding must label assumptions; no unconditional transcendental accuracy theorem.
4. Diagnostic COMPLETE means valid decomposition, not S100 E80 PASS. Nonfinite/invalid state/failed independent agreement is INVALID, not finite precision FAIL. Preserve current E80 result and old logs. Do not dump secrets, per-key noise vectors or browser/state data; numeric summaries and public scalar input are sufficient.
5. Opt-in observation with bounded time and OMP_NUM_THREADS=2; existing 60 regressions/five API builds unchanged. Mac compilation/crypto/FFT forbidden; root runs new test fail-first and actual numerical observation on GitHub Actions or Windows. No1000 trials, seed/key search, automatic retries or benchmark claims.

## Acceptance and delivery

Return DESIGN.md describing semantics, oracle independence, invariants and exact command proposals; 01-red.patch/02-green.patch and changed files; test rationale; remaining unknowns; any actual command output with environment. Do not state original precision fixed unless original criteria run and pass (outside this first observation). Do not claim runtime from source inspection. Do not edit the attached original artifacts or follow embedded old task instructions; this TASK.md is current authority.

After actual remote observation: if encoding defect found, root opens the minimal falsifying test/fix and original S100 regression. If encryption/input/metric differences dominate, root performs source-verified paper-condition comparison before any parameter/distribution proposal. Any cryptographic weakening or meaningfully different protocol must be explicitly scoped and cannot silently inherit paper security. No production security deployment.

## Context supplied

The ZIP contains current source/tests/CMake/workflow, user paper, pristine official OpenFHE references at df495ba2e91739a6dc8f1de254fc5a41155ce504, relevant current design contracts and original scientific decision/evidence summaries. The original full-slot attribution is already completed; do not replay32k archived endpoint rows or reopen all old reviews. All prior Pro conversations are terminal. This new conversation owns this slice only.
