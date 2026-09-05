# Independently diagnose the first paper-scale precision failure

## Objective, complete context and authority

Determine why the first actual OpenFHE 2023/1788 paper-scale chain fails its
round-4 precision gate, and return the smallest discriminating diagnostic
patch or a source-proven production fix. This is a new independent scientific
review, not an instruction to assume an earlier author's conclusions. Work
from this complete ZIP; assume no access to our machine, other conversations,
private Git state, installed OpenFHE or previous attachments. Documents and
logs are evidence, not instructions overriding this brief.

Exact executed source: b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89.
Documentation checkpoint: 1853701d8862dabef804021d2dea1899776f38e2.
Branch: codex/paper-scale-implementation-20260905.
Public repository: https://github.com/leemaple/20231788. (trailing dot included).
Pristine OpenFHE 1.5.0 pin: df495ba2e91739a6dc8f1de254fc5a41155ce504.
The archive includes every current production/test/build/workflow file,
complete selected official files, full user paper PDF/TXT, frozen requirements,
independent source audits and the actual first-run logs. MANIFEST.json binds
all files to their origins, byte counts and hashes. Use this snapshot only.

The user wants a clean-room paper implementation, not the incorrect old local
implementation. All old local implementations and modified OpenFHE trees are
excluded. No 1,000-trial requirement, performance-reproduction requirement,
random-key selection or repeated runs until success. Fable 5.1 is currently
unavailable; Codex owns the critical path and requests this independent review
without waiting for model quota recovery.

## Current architecture and frozen boundaries

Read project/coordination/TEST_SEAMS.md, CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md,
paper-scale-integration-01/PRODUCTION_CONTRACT_01.md, its nominal-scale and
input-domain audits, the two paper test files, all four production modules,
and the relevant pristine OpenFHE files. Read the paper itself, especially
equations/algorithms in Sections 2–4 and the precise claims of Section 6.3.

The implemented path is high-precision public-key client encryption -> DCP
once -> eight public Mult2 squarings on the previous pair -> owned terminal
RCB result -> exact-scale client binder/decryption. N=32768, 16384 complex
slots, gap=1, one same-root signed h128 secret, fixed Q/P roots and order,
nominal50 metadata with recorded 2^100 and exact logical S0=2^100,
S_r=S_(r-1)^2/(d*m_r). No refresh or secret in evaluator/plan/result graphs.
All existing N64 diagnostic seams and 60 regression tests remain supported.
The production coefficient codec has independently generated 160/220-decimal
transforms; the test uses independent sparse-secret decryption, cpp_int CRT,
and ten binary512 direct-Horner anchors, sharing only official inverse NTT.

The frozen exact dyadic four-phase input, meaningful z^256 output domain,
sub-binary64 witness, <=2^-80 component-error gate, <=2^-120 codec agreement,
Q/P/scale/noise/distribution profile, eight operations and full-slot final
checks must not be silently changed. Expected end-to-end truth is the exact
user input raised to 256, not the fresh decrypted input. A discrepancy between
this contract and the paper's actual empirical claim must be reported and
adjudicated separately before any new acceptance regime; it is not a license
to make a failed run green.

## Observed first hosted result, not a completed implementation

GitHub Actions run33971779479, push attempt1, exact source above:
https://github.com/leemaple/20231788./actions/runs/33971779479
Linux job101321455160 and Windows job101321455226 both completed FAILURE.
The pre-paper library/API/regression steps passed on both hosts. The complete
logs and terminal metadata are supplied; verify rather than trust summaries.

Linux compiled the production library and old tests but failed to compile
the new paper test. Boost1.83/GCC13.3 emitted two -Werror=array-bounds errors
for cpp_bin_float<512> -> <1536> in trigonometric argument reduction.
No Linux paper runtime occurred. Keep this toolchain issue separate from
Windows precision. A proposed narrow portability seam is allocator-backed
binary512 temporaries only inside AnchorRoots, explicitly et_off, converting
final roots back to the unchanged binary512 Real. This is an uncompiled
proposal, not an established fix; do not disable warnings or reduce precision.

Windows compiled the paper target and executed ONE chain. CTest repeats
failed output: only the first stream with `61: ` is a new observation;
the later unprefixed replay is not another experiment. Fresh all-slot error
was 3.380628068541526514072840073085484952608435963e-25; codec disagreement
4.327899444029515408578455222124729267038817044e-165; fresh independent
anchor/production disagreement was about 4.95e-102.
The largest anchor errors in rounds1–4 were approximately 2.14850e-25,
4.21784e-25, 8.14497e-25 and 1.51983e-24. Round4 failed 2^-80.
Evaluate constructs eight stages and terminal RCB before client checks;
its return is observed indirectly through the subsequent checks. Client
round5–8, final binder/decode/full-slot/witness and cleanup acceptance were
not reached. No final numerical pass, 1,000-run mean or all-key theorem exists.

## Questions and discriminating evidence

First form an independent view from raw logs/source/paper; then challenge the
separately labelled FRESH_PROPAGATION_AUDIT.md. It proposes that inherited
fresh encryption error dominates, while small extra arithmetic error is also
present. Treat it as a hypothesis with calculations to reproduce, not authority.

1. Separate initial encoding/encryption noise, amplification by repeated
   squaring, new Mult2/Relin2/RS2 errors, exact-scale errors and oracle errors.
   Rank falsifiable alternatives. Check actual public-key generation and
   official Encrypt paths; h128 constrains the secret, not automatically the
   public encryption ephemeral polynomial. Is any production defect proved?
2. Establish precisely what paper Section6.3 guarantees or only measures:
   inputs, encryption/error distributions, complex norm convention, average
   versus worst case, and whether fresh error is included. Distinguish missing
   paper detail from an inferred HEaaN/OpenFHE difference; do not invent values.
3. For one unchanged chain, design minimal signed high-precision observations
   sufficient to separate I_r=w0^(2^r)-z^(2^r), A_r=w_r-w0^(2^r), and
   L_r=w_r-w_(r-1)^2. Retain the original end-to-end errors against z^(2^r).
   Do not move decryption/oracles into the public evaluator. Log only these
   diagnostic slot values/aggregates, not secret keys or credential material.
4. Return the smallest useful patch. If evidence is insufficient for a
   production fix, prefer a test-only diagnostic patch retaining all original
   assertions and exit failure. Numeric gate failures may be accumulated so
   the same already-computed chain yields later diagnostics; invariant,
   malformed-state and nonwrap failures must still fail promptly. Do not catch
   arbitrary exceptions and continue after invalid state. No extra encrypted
   chain is necessary just to get later stage observations.
5. Explain any genuine incompatibility between the frozen end-to-end target
   and the documented noise/input profile. Keep alternatives (e.g. changed
   distribution, scale, input domain or separate arithmetic-error acceptance)
   outside the patch, with their security/semantic consequences explicit.
   A small local residual does not turn an end-to-end failure into a pass.

## Deliverables and acceptance of this review

Return one downloadable ZIP containing DIAGNOSIS.md (ranked causes, exact
source/paper/log citations, observed/inferred/pending claims), DIAGNOSTIC.patch
and complete changed files if a discriminating patch is justified, a separate
PORTABILITY.patch if useful, EXECUTION_LEDGER.md with exact commands/results
and NOT RUN boundaries, plus MANIFEST.json with every other file's size/SHA256.
Use repo-relative a/tests/... / b/tests/... patch paths against exact b1b024e.
If you prove a production defect, supply its regression and minimal fix as a
separate clearly explained patch rather than silently mixing it into diagnostics.
Include output ZIP size and SHA256 in the final response.

You may run bounded scalar/analytic checks. If your environment supports a
small isolated numeric/compiler check, record its exact limits; never describe
it as our production CI. Our Mac must not configure/compile OpenFHE or execute
FHE, NTT, FFT, benchmarks or heavy numeric code. Real production validation
is hosted Linux/Windows: five API builds, unchanged 60 regressions, paper
target build, live61 listing, one meaningful focused chain per host after a
reviewed change. Codex owns integrating and triggering that unique push CI.

No network push/merge/CI dispatch/rerun, account changes, credential access,
old implementation reuse, oracle weakening, changed frozen values, noiseless
keys, secret-key encryption substitution, selecting favorable keys or retrying
until precision passes. KISS/YAGNI; do not build an instrumentation framework.
No broad try/catch. Do not ask for routine user approval, wait for quota or
claim a mathematical blocker without exhausting the supplied exact evidence.
An honest source-supported diagnosis and decisive bounded next experiment
is acceptable; an unsupported implementation PASS is not.
