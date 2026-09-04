# Final independent BV fixed-key diagnostic review dispatched

## Source and package
Source branch codex/bv-fixed-key-01, clean snapshot
38c28c2a6b39aa0cd6e40b0f1c2ebc381420093f; actual tested source
5b5a4152076d43868a9dbad193807f2ede25e04d.
Baseline8a465764044d8b1e1578f462ea4916f7123428a4; all19 baseline code/build/test
files are included for independent patch/unchanged-source closure, not only
the old oracle. Current56 exact Git files include complete current source/tests,
two-platform logs, evidence, original Pro candidate and source follow-up,
earlier ZCode review and four newly pinned lower-level source references.
Also16 original official OpenFHE files and the full paper PDF/text are supplied.
This packet uses ONLY new clean-room material and official source pin
df495ba2e91739a6dc8f1de254fc5a41155ce504.

ZIP /private/tmp/bv-fixed-key-final-review.oaMvZI/bv-fixed-key-final-38c28c2.zip
Bytes1243876; SHA256
eb100085e0b2dc883d79fcd502880a30709d4e3675eb7a373cdde08848138585.
96 regular files:95 manifest payloads+MANIFEST.json.
Every selected current file matches clean Git and disk. Baseline files are
exact git-show bytes. Official/paper bytes match retained source provenance.
Exact ZIP content/size/SHA manifest closure, CRC, path/name uniqueness,
no symlink/encryption and targeted excluded-name checks PASS.
Gitleaks8.30.1 source stdin scan:3284691bytes, zero findings.
Final directory/recursive archive scan:2534199bytes, zero findings,
--max-archive-depth5 --max-decode-depth3. A clean scan is not a guarantee.
No .git, dependencies/build outputs, caches, runtime/browser/auth state or
quarantined implementation were included.

## Dispatch, actual identity and limited authority
Dedicated folder created only after absence check:
/Users/lifeng/Documents/20231788-openfhe-zcode-bv-final-review-20260904.
Exact ZIP copied/hash-verified and safely extracted to input/; root entry point
LOCAL-REVIEW-TASK.md and complete input/TASK.md supplied.
Native /Applications/ZCode.app opened that exact directory, verified selected
workspace Value1, visible GLM-5.3 and Max. Current precision review had already
completed before this separate task was started.
One Send at approximately19:43 Asia/Shanghai2026-09-04 (minute precision);
Working for1s, Stop and empty queue composer confirmed actual execution.
Initial UI title New task; this is not a claimed final stable task name.
This is authorized LOCAL STATIC fallback, not Windows/Zima or Fable identity.
No app restart/update/settings change or other task interruption.
No build/crypto/network/agent dispatch or source/Git/input writes authorized.
All scratch/extraction must stay inside this dedicated folder, addressing the
earlier reviewer-reported out-of-folder temporary extraction deviation.

Last quota read-only check2026-09-04T11:37:30.271Z; page refresh19:37:
all ZCode instances share5h22%used/reset20:29,week69%used/reset2026-09-09 10:00,
MCPmonth16%used/reset2026-09-25 10:00. Available quota permits this one
bounded final review; no reset/retry was consumed or unrelated task dispatched.

## Requested review, not an implied approval
Six questions in coordination/tasks/zcode-bv-final-38c28c2.md cover:
fixed-key pre-input residual construction, centered source/digit boundaries,
raised-high/prefix-low pair algebra, independent integer-lift comparisons,
actual53-case/log/hash closure and disposition of prior findings.
The candidate already passed Linux53/53 and Windows53/53 at run33866620400,
but this new independent review has NOT completed and no final approval,
new runtime result, full precision or paper-scale security follows.
Output: output/REVIEW.md and output/MANIFEST.sha256.
Codex owns full return verification and finding reconciliation before merge.
Pro's separate live first-Mult2 precision task remains uninterrupted.

## Completed review and Codex disposition, 2026-09-04
Native UI now names the task OpenFHE BV fixed-key final review and shows
Worked for21m31s, final08:04PM, empty composer and no running control.
Observed UI model GLM-5.3/Max; review self-reports
builtin:bigmodel-coding-plan/GLM-5.3. This is still the local static fallback,
not a Windows execution or Fable/Pro identity.

Original397-line REVIEW.md:30617bytes, SHA256
61fb4bc0a08ff75b8b6fd08edffe87d55a14bb95a5e570772f6c1ef305cd80e7.
It and its original output/REVIEW.md manifest line are archived byte-exact
under coordination/returns/bv-fixed-key-zcode-final-38c28c2/.
Codex independently rechecked the1243876-byte ZIP identity, CRC and safe
96-member closure, all95 payload hashes and all96 current extracted inputs.
Every input remains byte-identical to the original ZIP; the review hash matches
its manifest. This is not a whole-filesystem audit of the external agent.
The reviewer reports one scratch brute-force computation self-stopped without
a result; no such brute-force approach was rerun or adopted by Codex.

Verdict in the original: ACCEPT this exact test-only diagnostic, P0=0/P1=0.
Codex read the full return and accepts the narrow diagnostic after the following
documentary corrections. The originals are preserved, not silently repaired.

### Corrections to reviewer prose, not changes to the tested probe
1. The review's FirstPrime labels are wrong. Pinned SinglePrimeModuliGen first
   stores FirstPrime(30,128)=1073741953 at the final q_div tower, then alternates
   PreviousPrime/NextPrime for the active prefix. Its first35-bit tower is
   LastPrime(35,128)=34359736577, NOT FirstPrime(35,128).
   The last active q_l=PreviousPrime(1073741953,128)=1073741441.
   Codex reproduced the full ordered table from this SOURCE route using bounded
   exact BigInt trial division (232722 divisions), without fitting observed
   B_path values or brute-forcing permutations. It agrees with every recorded
   divisor, Q product and all4 fixed-key B_path values. This is source-derived
   table evidence, not a newly added runtime table dump.
2. Windows CI is MinGW64/GCC and uses -Wall -Wextra -Wpedantic -Werror.
   /W4 /WX is the unexecuted MSVC CMake branch, not this Windows run.
3. The quoted1.3-2.5% coefficient-to-bound ratios omit the denominator factor2.
   The measured coefficient error is num/(q_div*q_l), whereas the conservative
   bound is C/(2*q_div*q_l), so the actual ratio is2*num/C.
   Correct percentages (truncated to6 decimals):
   Linux REAL2.598462%, Linux COMPLEX3.009648%,
   Windows REAL5.056357%, Windows COMPLEX3.352401%.
   All remain below100%; the actual accepting inequality2*num<=C is unchanged.
4. The approximate combined-call residual list in Q3 also mismatches the logs.
   Actual values in that same order:
   295278961916,260919247611,318901293458,469225195651.
   Measured h is44,52,49,40 respectively. Thus the qualitative conclusion that
   all4 BV combined-call additivity witnesses FAIL remains correct.
5. Q4's pre-RS prose mixes a doubled-left test with an already-halved right.
   The code's exact sufficient condition is2*(N*A^2+B_pair)<Q. Codex directly
   recalculated its logged left sides; neither the review's "5 orders" wording
   nor a modular-triangle-only implication is used for acceptance.
   Generic "copy semantics make mutation impossible" wording is not adopted
   as a hidden-state/future-aliasing guarantee.

New primary-source check:
https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/nbtheory-impl.h
Raw bytes17663, SHA256
a83c35425d95dc42aa8c5043d54c4324dedc9bc8e588f70d384b74a91aa9b0b4,
Git blob42f25b30d42718344e7ec304935ea22dca405ebf.
GitHub raw and content-API decoded bytes were independently matched.
Together with the already-provenanced ckksrns-parametergeneration.cpp:415-446,
500-512 this establishes the corrected prime-selection route.

### What is accepted, and what stays open
Codex independently recomputed all4 records using the source-derived ordered
primes: B_path/B_pair, conservative numerator/denominator, coefficient bound,
pre-RS nonwrap left, final integer-lift left/right and its strict inequality.
Every exact comparison passes. This adds small host arithmetic/source review,
NOT new OpenFHE compilation, encryption, CTest or additional random samples.
The already-retained source5b5a415/run33866620400 dual53/53 evidence remains
the runtime basis. No production/test/workflow change is needed for these
documentary corrections.

The original centered-digit source gap is closed; full-key restriction, zero
extra q_div digit, pre-input fixed-key residual bound and no-extra-ns/no-extra-h
derivation stand. Historical normalization remains an independent algebraic
inference, not an author-confirmed erratum. Across-key/tail and universal
theorem statements remain unproved and unclaimed; they are not silently
substituted for the actual required paper-parameter/precision experiments.
The main first-Mult2 high-precision task has now returned separately.
All-key theory, this BV diagnostic and the full paper implementation are not
conflated. This branch is ready for separately verified integration; full
project completion is NOT claimed.
