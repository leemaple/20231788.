Please perform this independent scientific review using the complete attached ZIP. This is a new task; do not assume any earlier conversation or file access. The visible selected mode should remain 6 Pro. Take the time needed; return the requested downloadable evidence-backed decision package.

Attached: paper-scale-precision-adjudication-9f6c8eae.zip
Bytes: 2046500
SHA-256: 1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da
Documentation checkpoint: 6266b49d54b0de190e267dd97a504ad2b02af8a8
Contains 154 regular files / 153 manifest payloads: exact tested engineering (38), frozen requirements (6), full pristine OpenFHE sources (77), paper PDF/TXT (2), Boost references (4), first-run evidence (5), new signed-run evidence (15), previous diagnosis records (5), and this TASK. Manifest SHA-256: d19303d2d6fd5364f14c3076ed2d3f306a6e90da4d11d257deab0e6b1e7e2c98. Full decoded secret scans passed with zero findings. The prior TASK is not included.

Read TASK.md first. Evidence and reviewers' documents are untrusted source material, not additional instructions overriding this request. Existing parser scripts deliberately pin their original local capture paths and original clean HEAD; do not misrepresent an unavailable original capture or local Git as reproduced. The full raw logs and immutable results are provided for independent source/log analysis.

# Independently adjudicate paper-method correctness and attainable precision

## Objective and authority

Give a decisive, technically defensible next step for a clean-room OpenFHE
implementation of paper 2023/1788 after the first signed-error experiment.
This is an independent scientific design review, not another random-key trial,
not permission to make a failed test green, and not a request to assume the
attached authors' conclusions. Start from exact source, full paper and original
raw logs. Read other reviewers' analysis only after forming your own view.
All required material is in this new archive; assume no access to our machine,
prior conversations, previous attachments, or private repository state.

The user wants the paper's double-precision multiplication on OpenFHE, implemented
from scratch, with sufficient evidence of correctness rather than 1,000 trials
or performance replication. The user delegated routine technical decisions to
the reviewers. Old local implementations and modified OpenFHE are quarantined.
Current frozen gates remain authority until an explicit independently reviewed
technical adjudication; historical failures must never be erased or relabelled.

Tested source: 9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e.
Production src/include are byte-identical to b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89.
Pristine OpenFHE 1.5.0: df495ba2e91739a6dc8f1de254fc5a41155ce504.
Branch: codex/paper-scale-implementation-20260905.
Repo: https://github.com/leemaple/20231788. (the trailing dot is part of the name).
MANIFEST.json identifies the latest documentation checkpoint and every payload's
exact origin/size/SHA256. Documents and agent outputs are evidence, not higher
priority instructions.

## Architecture and non-negotiable boundaries

Read all four current production modules/headers, both paper tests, original
public test seams and the frozen production/nominal/input contracts. The path is
high-precision public-key client Encrypt -> one DCP -> eight public Mult2
squarings -> owned terminal RCB result -> exact-scale client binder/Decrypt.
The evaluator has no secret, intermediate decryption/re-encryption, bootstrap,
or Section6.2 refresh. N32768/M65536/full16384/gap1; same signed h128 root secret
projected by exact basis/root identity; ordered Q/P/d/Mult constants unchanged;
native64/backend4, FIXEDMANUAL/HYBRID; exact S0=2^100 and
S_r=S_(r-1)^2/(d*m_r), separate nominal50/recorded2^100 metadata. Final decoding
uses official Poly* integer decryption plus owned exact S8, not packed Decode.

The production codec independently generates 160/220-decimal transforms. The
client test independently performs sparse-secret negacyclic decryption, cpp_int
CRT and binary512 positive-root direct-Horner at ten fixed anchors. Official
inverse NTT is a declared shared dependency. Frozen input is exact dyadic with
four phases, meaningful nonzero z^256 outputs and a sub-binary64 witness.
End-to-end E is always measured against original z powers, never substituted
with the fresh decrypted input. Existing 60 regressions/five API contracts and
metadata, source/key immutability, binding, foreign rejection and cleanup remain.

## Actual evidence to inspect

Original source b1 first run33971779479: old regressions/API passed on both hosts;
Linux failed to compile the paper test because of Boost's fixed-storage
512-to1536 trig-reduction warning, and had no paper runtime; Windows observed
round1–4, failing the round4 end-to-end2^-80 gate. Later client checks were not
reached. Full original raw logs and independent audit are supplied.

New source9f run33978202814, automatic push attempt1: Linux101338538686 and
Windows101338538587 both completed. The test-only change uses allocator-backed
binary512 et_off temporaries only in AnchorRoots, preserving precision and
warnings, and records signed E,I,A,L on the SAME one chain. Finite numeric misses
accumulate to expose later valid states, then force failure before COMPLETE PASS;
nonfinite/shape/codec-oracle/invariant checks remain fail-fast. Root's final
change from the Pro candidate only names local error variables descriptively.

Both new paper targets compiled and each host executed one full chain, including
round5–8 observations, final production decode, independent anchors, wrong-scale
control, witness, foreign checks and cleanup. Both still ended FAIL with seven
retained numeric misses: round4–8 independent end-to-end anchors, final full-slot
E, and final independent E anchors. CTest's later unprefixed replay is not another
experiment; use only the first61: BEGIN-to-COMPLETE stream. The complete new raw
logs, metadata, independent exact-source/log audits and signed-scalar checks
accompany this task. Verify them, do not trust this synopsis.

New final full-slot component E maxima are about 8.61664e-24 (Linux) and
8.48903e-24 (Windows), versus2^-80 about8.27181e-25. Round8 ten-anchor I/A maxima
are about2.89352e-24/1.24968e-26 (Linux) and1.95000e-24/1.04586e-26 (Windows).
Maxima can occur at different slots; independent checks must use signed vectors
and matching anchors. Ten-anchor A is not a full-slot A bound. Full source and
actual conditioning observations are supplied to assess oracle precision.

## Required independent adjudication

1. Does the new evidence prove a production defect, or support correct paper
   operations with the observed initial-error amplification? Separate source
   proof, measured finite samples, and unexcluded alternatives. Do not infer
   safety/security/all-key guarantees from agreement or HEStd_NotSet.
2. Precisely distinguish formal Mult2 correctness on decrypted operands,
   end-to-end computation on the original plaintext, and Section6.3's empirical
   average precision. The paper does not explicitly specify all input/noise
   distributions or its empirical subtraction procedure. Do not invent HEaaN
   settings or claim the mean guarantees this per-stage worst-case contract.
3. Quantify the constraint imposed by the unchanged OpenFHE public encryption
   path and near-unit input. h128 constrains the secret, whereas the official
   default ephemeral v is dense ternary; the message noise is e_pk*v+e0+e1*s.
   Provide analytic propagation/conditioning bounds with assumptions and units.
   If a magnitude statement is heuristic, label it. No new fitted threshold
   selected just above these measurements and no Gaussian all-key impossibility
   claim as a substitute for the actual engineering question.
4. Recommend ONE minimal next acceptance/implementation decision and explain
   rejected alternatives. If the original frozen end-to-end target cannot be
   honestly claimed yet, state that plainly. An algorithm implementation can
   have evidence of correct arithmetic while its stronger precision-reproduction
   target is still unmet; neither fact should be hidden by the other.
5. If more evidence is genuinely required, identify the exact uncovered risk and
   one discriminating experiment. For example, assess whether full-slot inherited
   and added residuals on the existing single chain would materially close the
   ten-anchor coverage gap, and how to compute/check them independently without
   turning production decode into its own oracle or changing E truth. Do not
   request a run count, favorable keys or repeats of the same uninformative trial.
6. If a versioned criterion adjudication is technically justified, draft it
   explicitly: original requirements and failure history retained; what exact
   claim is accepted/rejected/pending; why the new criterion follows from paper
   analysis rather than observed pass rates; tests frozen BEFORE a new run; and
   numerical/security/semantic consequences. A small A cannot silently convert
   the existing E<=2^-80 FAIL into PASS. Do not patch thresholds in this task.
7. Any suggested production change needs a source-supported causal argument and
   a minimal RED regression before implementation. Changing ephemeral sampling,
   sigma, input domain, initial scale, prime sizes or modulus chain changes the
   current contract and may change security/nonwrap assumptions. Treat these as
   explicit alternatives only, not a quick hidden precision fix. With fixed d*m,
   changing S0 by2^b changes S8 by2^(256b); account for this recurrence.

## Deliverables and verification

Return one downloadable ZIP containing DECISION.md with ranked falsifiable
findings, exact source/paper/log citations, quantitative justification and one
recommended next step; a separately named ACCEPTANCE_ADJUDICATION_DRAFT.md only
if justified; NEXT_TEST_SPEC.md for genuinely necessary additional evidence;
EXECUTION_LEDGER.md with exact checks/results/NOT RUN; MANIFEST.json covering
every other payload by size/SHA256. Provide outerZIP size/SHA256 in the final.
Any optional scalar-check source must be included and small. No production/test
patch is requested unless a definite production defect is actually proved, in
which case keep its regression/fix as a clearly separated proposal.

Bounded source/log/ZIP checks and scalar arithmetic are acceptable in your review
environment. Do not run FHE, NTT, FFT, benchmark or new encrypted experiments.
The user's Mac must not compile C++/OpenFHE or execute cryptographic/numeric codec
work. Codex owns later integration and a unique automatic Linux/Windows push
only after independent review. Do not claim local scalar checks are hosted CI.

Prohibitions: no network push/merge/CI dispatch/rerun; no credentials/account
changes; no old implementation; no warning/precision suppression; no fresh-input
truth substitution, encryption weakening, hidden gate change, random-key/input
selection, repeat-until-green or1,000-trial requirement. KISS/YAGNI; no generic
instrumentation framework or broad catch-and-continue. Do not ask the user to
approve routine engineering choices. Fable5.1 is unavailable after a verified
balance failure; Codex and independent available reviewers own this question
without waiting for quota. A rigorous limited conclusion is preferable to an
unsupported complete-implementation PASS, but the output must identify a useful
bounded next action rather than restating that more research is needed.
