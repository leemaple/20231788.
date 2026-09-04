# First-Mult2 precision final ZCode review: reconciled acceptance

## Exact evidence and identity
Final original return: coordination/returns/first-mult2-precision-zcode-final-e482362/REVIEW.md,
24765 bytes, SHA256 a1316c93cbf2c2475de815e725c1d12782299dea51a6eddb306da6ad66238572.
The original REVIEW.md and its MANIFEST.sha256 are preserved byte-exact, including
the prose mistakes corrected below. The sidecar's original output/ prefix
belongs to the dedicated review workspace, not this archive directory.

Native /Applications/ZCode.app, task First Mult2 High-Precision Final Independent
Review, dedicated /Users/lifeng/Documents/20231788-openfhe-zcode-first-mult2-final-review-20260904.
Sent once 2026-09-04 20:56:43 CST; completed UI observed around21:19 CST:
Worked20m41s, Review complete, ACCEPT, P0=0/P1=0/P2=5, empty composer.
UI model GLM-5.3 Max; return self-identifies builtin:bigmodel-coding-plan/GLM-5.3.
This is the user-authorized LOCAL STATIC fallback, NOT Windows execution and
NOT Fable5.1. No local OpenFHE/project builds, crypto or benchmarks were run.

Input archive source e48236231f32651ae3c7a3f07ef0b7d67a7560b2;
active tested source47907783a6141d0174da79eae264d779fc598f28;
OpenFHE1.5.0 pin df495ba2e91739a6dc8f1de254fc5a41155ce504.
ZIP1304902bytes SHA25650660206ef1657d19c4ca2cd7540aa61ba301a7fe946f5734e3d3861f179f281.
Codex independently rechecked ZIP safety/CRC, unique119 regular members,
118-entry non-self manifest closure, and all119 extracted inputs byte-exact.
Root LOCAL-REVIEW-TASK.md matched the sent wrapper; output contained only the
two specified files. Review's two temporary name-list files were reported
removed from its output directory; no project source or inputs changed.

The main agent read all416 original lines and reconciled the report against
the frozen source, prior source audit and actual two-platform hosted evidence.
The original Pro draft/patch, Codex checks, and actual independent ZCode return
now exist. No source, vector, oracle or threshold was altered to accommodate
review prose.

## Corrections to the original review, not code fixes
1. Section2 scanner version is 8.30.1, NOT8.3.1. The successful pre-dispatch scan
   covered2848390bytes with zero findings after a separately disclosed
   premature missing-directory failure. ZCode did not rerun that scanner;
   its general 'no tool runs' wording cannot describe section10's actual
   bounded read-only/arithmetic commands.
2. Section3's 'four untracked notes excluded' is inaccurate. The dirty-state
   list had three future paper notes plus the newly authored review TASK.
   Only that TASK was included explicitly; the three future notes were not.
   Exact119-member immutability/manifest verification is unchanged.
3. Sections5/8/9 misstate the numerical margin and error bits. Static decimal
   recomputation from the unchanged retained logs gives:
   tolerance2^-80 =8.2718061255302767487140869206996285356581211090087890625e-25;
   worst slot=1.6696072195146116607129673424340031160212e-27,
   tolerance/worst slot=495.43425716229574413395270788565494;
   worst delta=1.6958307879080880932103073456218202834178e-27,
   tolerance/worst delta=487.77308352409735178059315766122514.
   Approximate absolute-error bits are88.95255(slot),88.93007(delta), not89.07.
   The ~49.5x claim is off by a factor10; ~9.04 excess bits is also wrong.
   These are observed sample margins, not a statistical guarantee.
4. Section7 says RightValues has16 real slots. It has16 complex-valued entries,
   many with nonzero imaginary parts (first0.5-0.25i). Both operands are
   nontrivial; no unity-RHS test or binary64-only precision oracle was accepted.
5. Sections4.3/4.6's |low| approximately q_div/2 must NOT be applied directly to
   independently decrypted low plaintext coefficients. DCP source377-417
   constructs centered remainders COMPONENTWISE. Decryption sums low_i*s^i
   with secret convolution, which can amplify their coefficients. The
   current test measures independently decrypted coefficients and low-low
   error; it does not assume that decrypted-low bound. This distinction
   does not invalidate the measured slot/coefficient diagnostics.
6. Section4.2's blanket 'No' to shared oracle defects, its numerical evaluation
   error much-less-than2^-200, and section4.4's headroom language are not
   accepted as formal universal proofs. Accepted evidence is targeted
   binary64-loss discrimination, explicit integer/CRT reconstruction,
   calibrated slot evaluation and observed centered-representative headroom.
   No all-defect, all-key/noise-history or universal no-wrap claim follows.

## Finding dispositions
- P2-1 small-sample statistics: DEFERRED, ownerCodex. Four unseeded keys per
  invocation and16 actual samples are empirical first-Mult2 diagnostics.
  The later paper1000-trial experiment must state sampling/aggregation;
  it is not silently replaced by these samples.
- P2-2 right-input expected key tag: DISPROVED as a required correctness fix.
  Both inputs use the same generated public key; test1022-1032 intentionally
  checks right state against left tag and then explicit cross-input equality.
  Replacing the expected tag with its own tag adds no protection and makes
  that specific state check tautological. Keep the cross-input invariant;
  only its duplicate diagnostic wording is cosmetic.
- P2-3 no mutation RED for this particular test: DEFERRED optional hardening,
  ownerCodex, not a new release gate. The inherited missing-precision feature
  has genuine precursor RED/GREEN evidence. This additional existing-behavior
  regression is honestly FIRST-OBSERVED GREEN; no fabricated RED is claimed.
- P2-4 automatic stale-cache misuse guard: DEFERRED bounded implementation,
  ownerCodex, due at the next precision integration boundary. Current manual
  static guards are not CI automation. The check must distinguish forbidden
  production Decrypt/packed-value getters/serialization from the test-owned
  IndependentDecrypt oracle, preserve legitimate public-operation tests,
  and explicitly state lexical enforcement limits. No new production API
  or full codec may be invented merely to close this testing safeguard.
- P2-5 observed headroom vs universal integer-history theorem: DEFERRED only
  if such a universal claim becomes necessary, ownerCodex. Current observed
  qualifiers are retained. Do not invent an all-key theorem as an additional
  full-project completion prerequisite.

The printed Theorem4.8 scale reconciliation remains an inference/paper-side
question, NOT an author-confirmed erratum. It is already in the complete new
Repeated Mult2 Design and TDD task supplied to Pro; do not interrupt its live
thinking or send a duplicate reminder.

## Accepted hosted result and boundary
[Run33873114880](https://github.com/leemaple/20231788./actions/runs/33873114880),
source47907783a6141d0174da79eae264d779fc598f28:
Linux job101023587797 focused1/1, full55/55 in1.03s;
Windows job101023588186 focused1/1, full55/55 in2.30s.
Both five API targets and default warning-as-error builds passed.
Exact55 CTest names/order, job/source/run/attempt markers,8 records/host,
all16 slot/delta errors<=2^-80, divisor denominator1267650600226646386227681786497
and observed headroom160/210bits were verified. Detailed log hashes, durations
and outputs remain in FIRST_MULT2_PRECISION_RETURN_AND_HOSTED.md and its JSONs.
No new cryptographic run occurred during this return audit.

Verdict: accept this exact TEST-ONLY first-Mult2 high-precision regression,
with the above independently reconciled nonblocking dispositions. Production
lossless I/O, repeated multiplication, paper40/60 N32768 h128 parameters,
paper experiment statistics and performance/security remain unestablished.
This is neither full paper completion nor acceptance of every reviewer sentence.
