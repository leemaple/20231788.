# Pair composition: main disposition of final ZCode review

## Observed return and integrity
The native /Applications/ZCode.app task
`Pair composition final independent local review` in the dedicated
20231788-openfhe-zcode-pair-composition-final-review-20260904 folder is
terminal: UI says Worked12m56s, Review complete, 21:47, both deliverables;
composer is empty and Send disabled. No Stop/refresh/retry was used.
This is user-authorized LOCAL STATIC macOS review, not Windows execution.
UI model is GLM-5.3. Dispatch recorded Max; the completion observation shows
Low. The current setting does not establish the entire run's reasoning
configuration. Do not label it a proven all-Max run or a Fable5.1 review.

Original REVIEW.md and MANIFEST.sha256 are preserved byte-for-byte under
coordination/returns/pair-composition-zcode-final-da50a7e/.
REVIEW.md:25464bytes,
SHA256dba324a7c8bbc986c9aa5a2bee7a70b8f76b937e1c942b7411f932ab680383c8.
The original sidecar names output/REVIEW.md relative to the review root;
keep its original path text, do not silently rewrite the evidence.
Main read the complete original. It verified all111 extracted files against
the safe/CRC-valid original ZIP, its exact archive hash, the sidecar digest,
and the wrapper against the originally stored full text: all unchanged.
The ZIP remains1293653bytes,
SHA2564417299f03ab04277aa479cfa8a761d03b854c957c7b17d6174ed90f2610e64b.

Source da50a7e1af05eaaff40d7ce82068e65da02815a2,
tested b48b54e22f14bbfe988a6890f1b03eac9efb11a3,
run33871723090: Linux101019032191 and Windows101019032537.
The review's original verdict is narrow ACCEPT, P0/P1=0, P2=3. Actual
source-branch focused2/2 and full55/55 are retained evidence, not a new
post-integration run. Its review does not replace future merged-source CI.

## Main corrections to original prose, not changes to original evidence
1. The printed finite decimal1.00000023655603276 is an approximation to
   2^60/(1073741953*1073741441), not exactly the same rational number.
   Actual code validates the descriptor with a specified floating tolerance.
2. The P2-2 statement that a future non-dyadic fixture would "never pass
   falsely" is too strong. Binary64 operations can agree after rounding
   without equaling the intended exact-number product. The current frozen
   dyadic values/products were independently exactly checked; that specific
   proof, not an assumed universal fail-closed property, justifies equality.
3. P2-1 says functional corruption would still be caught by the coefficient
   certificate. That is not an all-corruptions guarantee: the certificate
   permits nonzero error inside its conditional bound. Row presence and
   numerical acceptance do not prove deep key-material immutability.
4. "Full nonmutation" must be read as the enumerated input/pair snapshot
   fields only, not every shared context/cache/key field. Fresh KeyGen calls
   are source-observed; varying Hamming weights corroborate them, but do not
   independently prove key uniqueness or a statistical distribution.
5. The original claims no broad catch while describing main's std::exception
   catch. That catch is a broad exception class at the test executable
   boundary: it reports failure/nonzero, not swallowed success or recovery in
   the production algorithm. No new exception handling was introduced here.
6. The separately retained PAIR_COMPOSITION_ZCODE_RECONCILIATION_CHECK.md
   also corrects three source descriptions: the scale tolerance is
   32*epsilon*max(abs(ratios)), not exactly32ULPs; the integer assertion is
   2*errorNumerator<=boundNumerator, equivalent to comparing the two printed
   fractions, not doubling the printed error; newly added composition helpers
   are new, while reused old helpers are unchanged. Main read that complete
   independent note and agrees with these corrections.

## P2 disposition and ownership
- P2-1: documented scope limitation, not user-accepted risk. The frozen
  regression promises row presence plus enumerated input/pair snapshots.
  A deep evaluation-key digest check is deferred to the actual key-family
  lifecycle increment, ownerCodex, if needed for its confirmed contract.
  No existing check or requirement is weakened and no broader proof is claimed.
- P2-2: no defect in the exactly verified frozen dyadic fixture. Retain the
  current literals and comparisons unchanged. OwnerCodex must re-derive
  independent exact expectations/representability before any future fixture
  change; do not generalize the equality test to arbitrary real inputs.
- P2-3: no current mismatch among the three slot-0 imaginary witnesses and
  their frozen arrays. OwnerCodex keeps future fixture/witness revisions
  together and rechecks the contract before execution. No speculative helper
  or configurable fixture framework is warranted for these fixed literals.
- O-1: workflow additions belong to Codex's documented CI integration, not
  an unauthorized expansion of the Pro two-file patch. Preserve both focused
  checks and the full suite/API/warning/pinned-source steps when merging.

The reviewer's word "accepted" is its own assessment, not evidence of a
user risk acceptance. The three items above are exact-scope limitations or
future maintenance obligations, with no unresolved current P0/P1.

## Reviewer incident and resulting boundary
The reviewer discloses an outside-folder typo write at the20260804 sibling,
followed by deletion. The UI called it empty, while the report says the word
placeholder; neither description is independently proven by the present
absence of a file. Record this as a reviewer-reported scope violation, not
as a compliant action or a deletion performed by Codex. Main did not repeat
the write or delete anything. All authorized input bytes remain verified.

## Narrow acceptance and next gate
The main source spot-checks confirm original-pair expected recombinations,
the public Add/Sub-to-Mult2 path, fixed vector and state assertions,
presence-only eval-key checks, and conditional error inequalities.
Together with the earlier full source/Codex audits, Pro candidate and actual
two-platform runs, this accepts the exact functional N64/fixed1e-3 slice.
No source/test/threshold is changed by this disposition. It is not paper
precision, repeated multiplication, security, performance or full completion.
Merge must preserve the newer BV certificate helper, adapt only the new
HYBRID caller's non-BV arguments, retain both precision tests and produce
the exact57-test union. Require fresh dual-platform evidence for that merge.
