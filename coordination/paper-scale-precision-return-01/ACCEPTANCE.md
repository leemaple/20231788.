# Precision diagnosis return and bounded diagnostic integration

Recorded 2026-09-06 Asia/Shanghai. Disposition: **ACCEPT TEST-ONLY DIAGNOSTIC
CANDIDATE FOR ONE HOSTED PUSH; NOT AN IMPLEMENTATION PASS.**

## Identity and retained evidence

Executed production baseline: `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`.
Integration parent: `797e86dc0ff625c1847543fcbab295b2a330de76` on
`codex/paper-scale-implementation-20260905`; clean before these edits.
Pristine OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
All intervening baseline-to-parent commits were documentation-only.

Pro conversation: [Diagnose Precision Failure](https://chatgpt.com/c/6a9c3081-536c-83ec-97c6-b82cd8295606).
Visible identity: `6 Pro`, not an API model attestation. The task was sent once
at 2026-09-05 23:08:47 Asia/Shanghai. At 2026-09-06 00:19:46 the page showed one
user message, a completed response, no Stop answering control, and the download
button. The ZIP was downloaded once; local file mtime was 00:20:36. Its size is
123,321 bytes and SHA-256 is
`1466662ee302dc89fe01d1e55621f5d197e64ddf1a184b545ec084d70c0ae6da`.
The exact archive is retained here. `RETURN_PREFLIGHT.json` records CRC, safe
unique regular paths, all 33 manifest payload hashes, and strict gitleaks 8.30.1
exit 0 / no findings. Returned reports are preserved as `PRO_DIAGNOSIS.md` and
`PRO_EXECUTION_LEDGER.md`; external claims remain attributed to that review.

## Diagnosis, limits and one discriminating change

The first hosted run remains failed: Linux did not compile the paper test;
Windows reached round-4 client checking and missed the frozen end-to-end gate.
Do not reinterpret its later CTest replay as another experiment or infer
round-5–8/final acceptance. See the closed
`../paper-scale-production-first-run-01/ACCEPTANCE.md` and its exact raw logs.

The direction-independent slot-512 inherited-error interval after four ideal
squarings, approximately [1.49406650e-24, 1.53821966e-24], exceeds 2^-80 and
contains the observed error. This supports fresh-error propagation as a cause,
but does not measure accumulated arithmetic error or prove a production defect:
signed contributions could reinforce or cancel. Dense ternary ephemeral public
encryption is source-verified; h128 describes the secret, not that ephemeral
polynomial. The paper does not specify enough experimental details to assert
that its empirical noise/input regime is identical or different.

Only the two paper test files change. The same single chain now records signed
E (original-input end-to-end), I (inherited fresh), A (accumulated added), and L
(one-step) residuals at ten independent anchors. Original z remains the truth;
w0 powers are diagnostics only. L1 also includes any DCP departure. No claim is
made to separately measure encoding/encryption or individual Mult2 primitives.
Coefficient/scale maxima support interpreting Horner conditioning, not a new
no-wrap proof or a replacement acceptance gate.

Finite numeric/witness misses accumulate so already-computed later stages and
cleanup can be observed. Every old inequality remains; the aggregate must be
zero before COMPLETE PASS. Structural, nonfinite, codec/oracle-agreement,
existing headroom and malformed-state checks still fail immediately. Production
sources/headers, inputs, Q/P/roots, distributions, scales, precision, anchors,
public evaluator boundaries, old tests, CMake and workflow remain unchanged.

The separate portability seam uses allocator-backed binary512 temporaries with
explicit et_off only in AnchorRoots, converting final roots back to unchanged
Real. It does not disable Werror or lower precision. Pro's isolated GCC14.2 /
Boost1.83 scalar/compiler probe is retained evidence, not a hosted GCC13.3/full
target result and not a compiler run on this Mac.

## Independent review and root verification

**Standards:** independent GPT-5.6 Sol review found zero hard violations and one
nonblocking Mysterious Name judgment. Root resolved it by replacing i/b/l with
inheritedError/addedError/localError and separate const declarations. No numeric
expression or control-flow meaning changed. Original findings are retained in
`STANDARDS_REVIEW.md` rather than rewritten as if none existed.

**Spec:** independent GPT-6 Astra high review found no blocking defect or new
false-PASS path; see `SPEC_REVIEW.md`. It read original/combined source before
Pro's diagnosis. These are independent task contexts, not a claim of different
model providers or final semantic approval of the whole production system.
Fable 5.1's recorded insufficient balance did not block this ownership transfer.

Root read both complete candidate test files, both patches, Pro's diagnosis and
execution ledger, the validator and independent scalar-analysis source. Root
re-executed the reviewed validation script against a fresh hash-verified copy of
the exact input packet: exit 0, all 640 synthetic signed components matched
exact-Fraction calculations within 2^-360, maximum discrepancy approximately
4.40945e-126. This checks retained synthetic scalar output, not a newly executed
C++ probe or FHE chain. Finite-failure/nonfinite-abort output assertions, 133
incoming payload hashes, unchanged source-function guards, and all four patch
application orders/complete-file byte matches passed. Exact results are in
`ROOT_VALIDATION.json`. Disposable patch checks did not modify production.

Root applied the reviewed changes with apply_patch, then the naming-only fix.
The final test is byte-identical to the returned combined candidate; the oracle
differs only by the explicitly documented local-variable rename/declaration
split. Subsequent source/hash and staging checks bind this disposition to the
actual commit. No Mac compiler, CMake, FHE, FFT, NTT or benchmark was run.

## Next evidence boundary

Push one engineering commit to the existing branch and bind the unique automatic
GitHub Actions run to its exact SHA. Do not dispatch/rerun, select keys, or retry
unchanged until green. Retain five API builds, unchanged 60 regressions, the live
61-test inventory, and one meaningful focused paper chain per host. Numeric FAIL
with complete signed diagnostics remains FAIL, but may resolve the dominant
cause. If invariant/oracle validity fails, stop interpretation at that point.

Hosted patched compilation, actual signed FHE observations, round-5–8 and final
full-slot/witness/binder/cleanup outcomes are **pending at this checkpoint**.
Any criterion adjudication or production fix requires separate recorded
reasoning before another acceptance run. No 1,000-trial requirement, security
certification, paper-mean reproduction, or complete-implementation claim is added.
