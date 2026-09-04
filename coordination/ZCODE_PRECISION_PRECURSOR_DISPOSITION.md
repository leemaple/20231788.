# ZCode final precursor review: receipt and Codex disposition

Observed 2026-09-04 19:32-19:36 Asia/Shanghai. Review input snapshot
c9ee28d0370eeee1ec7a1965402ed0b5e91f425e; active tested source
bd141806bd1e0b1dad80c7ad47bfd92fc334db55. Current clean precision branch before
this receipt: f6f8eb4c3c41c4ce993a804808131de384782d1d.
This receipt changes no source, test, frozen acceptance, workflow or CI run.

## Delivery and identity
Exact native app /Applications/ZCode.app, task title
OpenFHE Precision Review Q1-Q6 Audit; UI GLM-5.3, Max.
UI reports Worked for25m14s, final answer at07:26PM, no Stop button and an empty
disabled-Send composer when checked. This is the authorized LOCAL STATIC
fallback, not Windows execution, Fable or a newly verified routing guarantee.
The complete453-line REVIEW.md and its original output-relative MANIFEST.sha256
are archived unchanged under coordination/returns/precision-zcode-c9ee28d/.
Review bytes29063, SHA256
0b814e9b7b764df4e4fc26db10d6e8a87314fc9891bebacc29034bb100f34677.
The original manifest refers to output/REVIEW.md in the delivery folder;
that original path is deliberately not silently rewritten in the archive.

Codex read the entire review, independently verified its manifest, the outer
ZIP SHA e49e3fcb897ea7b9fa0cf31bc376a9289e0bd4fe8f4a1ff70a35622bd5fe0461,
and all61 original input manifest payloads by exact byte size and SHA256.
The permitted inner-archive extraction adds input/prior-pro-extracted/;
"no extra files" applies only to the original outer payload set, not the later
whole input directory. The review also reports an outer verification extraction
at /tmp/review-verify despite its folder-only rule. That process-scope deviation
is recorded, not presented as full compliance; no content from that temporary
path was adopted. Subsequent briefs will explicitly forbid outside-folder temp
paths. Input payloads are unchanged and no Mac compile/crypto run is observed.
An initial Codex Git status check in the non-repository delivery folder returned
"not a git repository"; it changed nothing. Repository state was then checked
in the exact precision worktree.

Review verdict PASS_WITH_GAPS; no P0/P1, five P2 findings and two informational
items. A reviewer verdict is not accepted wholesale: dispositions follow.

## F-1: DISPROVED with source and existing discriminating runtime coverage
The claimed non-centered DCP remainder is incorrect. It overlooks the actual
non-lazy SwitchModulus called by DropLastElementAndScale.

Production src/double_ckks.cpp:376-402 uses factors -q_div^-1 and q_div^-1.
Pinned dcrtpoly-impl.h:693-712 first converts the last tower to COEFFICIENT,
then tmp.SwitchModulus(targetModulus,...), multiplies by -q_div^-1, and adds
the retained source tower times q_div^-1. Therefore it is necessary to inspect
SwitchModulus itself, not treat the unsigned source residue as a signed lift.

Four additional pristine official files already verified in the independent
BV source investigation are retained at clean BV branch commit
38c28c2a6b39aa0cd6e40b0f1c2ebc381420093f under
coordination/official-references/bv-centered-lift/.
OFFICIAL-PROVENANCE.json binds bytes, SHA256 and Git blob hashes to upstream
df495ba2e91739a6dc8f1de254fc5a41155ce504. They are NOT old implementation code.
- poly-impl.h:400-406 forwards to m_values->SwitchModulus.
- mubintvecnat.cpp:109-122 sets halfQ=oldQ>>1, uses STRICT v>halfQ,
  then adds newQ-oldQ on the negative half when increasing modulus, or
  modularly subtracts oldQ-newQ when decreasing.
- ubintnat.h:891-902 reduces both modular-subtraction operands first.
- LazySwitchModulus at mubintvecnat.cpp:125-129 is a different function;
  DropLastElementAndScale does NOT call it.
Official source:
https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/lib/math/hal/intnat/mubintvecnat.cpp

For odd p=q_div and canonical0<=v<p, let d=v if v<=floor(p/2), else d=v-p.
For target r>p the source returns v or v+r-p, i.e. d mod r.
For r<=p it returns v mod r or (v-p+r) mod r, again d mod r.
Consequently high=(ct-d)/p mod each retained prime and low=ct-p*high=d.
The actual low coefficients represent the CENTERED remainder. No additional
+h term, non-centered-variant correction or production change follows from F-1.

This is independently discriminated by existing tests, not merely an RCB
round-trip identity: tests/dcp_rcb_test.cpp:185-193 computes a cpp_int
centered remainder and quotient; :274-300 includes both signs, p/2 boundaries,
p/2+1 and p/2+2, multiples of p and full-Q boundary values;
:367-406 compares actual high AND low coefficient-by-coefficient in every
tower against that independent centered oracle. The dcp_rcb case actually
passes on BOTH retained GREEN logs (Linux line107, Windows line130) at
bd141806. Those tests would reject the unsigned variant on the negative half.
No new crypto run is inferred; this uses existing verified hosted execution.
F-1's proposed adjusted constants and remainder deviation must NOT be propagated
into the live first-Mult2 design. Pro will receive the complete review/source
disposition at its next completed-response boundary, not mid-thought.

## F-2: current log/run binding independently revalidated; future enhancement
The reviewer accurately notes the retained Linux section has no self-contained
run-ID marker. It does not make the source/run binding unknowable: Codex
re-fetched exact GitHub jobs and their logs through the authenticated gh API.
The job metadata independently returned run_id and head_sha, and the normalized
project sections are BYTE-IDENTICAL to the retained logs:
- 19:35:10.164CST: job100991676478/run33863067661,
  e38764ab16bc638182d95ff259943eee0987d537, completed/failure,
  red-linux.txt50852bytes/SHA256
  56ae17d6cecafc1ce336acca3e3119ae88c2e0f02a1351ead64dec90dc0e19e7.
- 19:35:14.457CST: job100994829226/run33864080896,
  bd141806bd1e0b1dad80c7ad47bfd92fc334db55, completed/success,
  green-linux.txt54785bytes/SHA256
  2fb529a2893a639bc3acfafa6c958490ba1b062e79c8a2f65d6b36af437fa591.
Endpoints: repos/leemaple/20231788./actions/jobs/<jobId> and .../<jobId>/logs.
Selected from "##[group]Run cmake -S . -B build" to before Post job cleanup;
only ANSI/CR/trailing whitespace normalized, with one final newline.
No retained log was edited and no CI was rerun.
Future self-contained run/source markers: deferred to Codex's next justified
workflow edit, not inserted into historical evidence or the frozen precursor.

## F-3/F-4/F-5 and informational items
F-3: VALID diagnostic-label nit. The actual measured quantity is observed
centered-coefficient headroom, not a universal integer no-wrap proof.
The existing assertion/threshold stays frozen. Codex will adjust wording at
the next independently justified touch, preserving the archived runtime pair.
F-4: VALID style inconsistency only; both HEStd_NotSet labels disclaim security.
Codex defers uniform wording until those older tests legitimately change.
F-5: VALID future-misuse guard gap, not an observed stale-cache read.
Codex will add a scoped mechanically checked prohibition at the next
precision-test/CI integration boundary. A blanket grep for Decrypt is NOT a
sound proposed fix: IndependentDecrypt is the required test-owned oracle and
the explanatory comments name forbidden APIs. A guard must distinguish actual
production calls/getters/serialization from these permitted identifiers.
Current fixture remains test-only; no production I/O claim is admitted.
F-6/F-7: informational counts/wording, no behavior or acceptance action.
The review's "deterministic 2^-89 floor" wording is also not adopted: its
64*(1/2)*2^-100 calculation is an encode-rounding UPPER bound, not a lower
error floor or a bound including encryption/key-switch noise.

## Remaining gates and continuity
The source-supported transport/high-precision diagnostic remains accepted;
the erroneous F-1 interpretation is rejected. The independent Pro final
precursor review and first-Mult2 precision candidate are still live at
https://chatgpt.com/c/6a9a5891-6210-83ec-a24c-d6a5e0889fbd (Stop observed).
No stop, refresh, reminder, duplicate or follow-up was sent.
First-Mult2 accuracy, exact normalization, low-low/key-switch effects, repeated
lifecycle, paper40/60 parameters, production lossless I/O and paper-scale
security/performance remain required and unverified by this review.
