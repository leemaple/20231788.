# ZCode combined Pair audit: received and reconciled

Observed 2026-09-04 Asia/Shanghai. This updates the ZCode LIVE state in
PAIR_COMBINED_REVIEW_D73824C.md; Pro's separate final audit remains LIVE.

## Seat, input and return integrity

Exact native app: /Applications/ZCode.app. Actual completed task title:
"Static final Pair Add/Sub six-question audit".
Dedicated local static-only fallback folder:
 /Users/lifeng/Documents/20231788-openfhe-zcode-pair-review-20260904.
The correct project and task heading were selected before reading the result.
UI showed Worked for 12m 50s and GLM-5.3; prior dispatch also verified Max.
Returned model self-identification is builtin:bigmodel-coding-plan/GLM-5.3,
explicitly limited to provider-exposed identity. This is not Windows execution
and not Fable 5.1. No unrelated visible task content was used or transferred.

Reviewed source d73824c2d382013c3aadbd7cb29c57008e839714;
documentation snapshot f550eac1251f2005222e60aa4f07cc2e57380c46;
pristine OpenFHE df495ba2e91739a6dc8f1de254fc5a41155ce504.
Input ZIP 1305833 bytes, SHA256
e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce.
Codex independently rechecked all 70 input payload sizes/hashes and the exact
71-file extracted closure, in addition to the original packet checks.
All three returned MANIFEST.sha256 entries verify against their original
paths (review, input ZIP, extracted input manifest).

Return retained byte-exact:
coordination/returns/pair-combined-zcode-d73824c/REVIEW.md
503 lines, 31907 bytes, SHA256
c12b82cf42b20906168baa5b0c29ad395a8040eb59858056fc92c1daef2f7945.
MANIFEST.sha256: 3 lines, 284 bytes, SHA256
431a7ea2a22bb4231c9fd1c6045f21bb40999a5329a48855359e99e11e62724b.
Its relative paths apply to the original output directory, not this archive.
Gitleaks 8.30.1 output scan: zero findings, ~32.19 KB.
Codex read the full 503-line review. No supplied script was executed.
An initial continuation read used output/REVIEW.md while already in output/;
it failed harmlessly and was corrected to REVIEW.md before completing the read.

## Findings reconciled against source and exact hosted evidence

Verdict is PASS_WITH_GAPS for Pair Add/Sub static scope, not project acceptance.

- F1/F2/F3 confirmed for the stated boundary: Add/Sub clone corresponding left
  members, perform only componentwise DCRT modular arithmetic, validate left
  then right then compatibility, preserve metadata/lifecycle, and revalidate.
  The Pair-specific production delta is 4 header +106 source added lines.
  Do not generalize this to "the entire integration history changes nothing
  else": the combined branch also contains separately reviewed RS2 validation
  fixes and Mult2 composition.
- F4 confirmed with a precision in wording: after both operands validate AND
  the lifecycle compatibility check passes, context-derived divisor/basis/
  level/scale/degree/format/shape mismatches cannot be reached by valid pairs
  of this module. Lifecycle, genuine key tag and genuine slots are the
  reachable mutual mismatches; foreign-context input fails individual
  validation earlier. Keep these defensive checks; do not fabricate valid
  malformed objects to cover unreachable branches.
- F5 confirmed: retained tests cover Add/Sub on all three prepared lifecycles
  but do not feed a ReadyForFirstMult Add/Sub RESULT into Tensor2/Mult2.
  Owner Codex: schedule a minimal regression slice with an independent oracle
  after the final Pro audit; do not mislabel future coverage as a missing-
  feature red-green or as high-precision/repeated-multiplication evidence.
- F6 confirmed: pair_add_test.cpp compares a dead scaffold diagnostic
  "Add arithmetic is not implemented", whereas the retained historical red
  says "Add is not implemented". Both exception arms fail loudly. No false
  pass or production defect; defer cosmetic cleanup to the next legitimate
  edit of that file (owner Codex), no standalone source change now.
- F7 confirmed from pinned ciphertext.h:390-406: fresh outer metadata map and
  copied DCRT values, shared metadata entry pointers and parameter provenance.
  Tests establish enumerated per-call nonmutation on exercised cases, not
  arbitrary future mutation isolation or an exhaustive hidden-state theorem.
  Likewise, snapshots alone cannot rule out an invisible read-only key
  lookup; the inspected no-key-access Add/Sub source and removed fixture row
  are separate supporting evidence. Preserve these bounded claims.
- F8: reviewer was offline and quoted run identities. Codex freshly queried
  GitHub's run and both jobs: run33854419062 is completed/success, branch
  codex/integration-01, exact head d73824c2d382013c3aadbd7cb29c57008e839714.
  Linux100964299802 and Windows100964299593 project warning build, CTest and
  all five Relin2/RS2/Mult2/Add/Sub API build steps succeeded. Linux reused
  the pinned dependency cache (dependency configure/build skipped); Windows
  built pristine OpenFHE. This is not a claim that Linux rebuilt upstream.
  Prior retained CTest totals remain 53/53,0.68s and 53/53,2.27s.

## Correct the review's prose, preserve original bytes

Q6 says the Pair family has six tests in addition to one dcp_rcb. Actual CMake
has FIVE Pair-prefixed registrations, plus the separate dcp_rcb test.
The exact family counts are dcp_rcb1 + Pair5 + Tensor2 5 + Relin2 31 + RS2 6
+ Mult2 5 =53. Section7's six Add/Sub-relevant entries intentionally include
dcp_rcb and are consistent with this corrected reading.
Codex recomputed 53 unique CMake names and exact 53-name closure in EACH
retained Linux/Windows combined log. There are15 executable targets plus
one library (16 total). No missing/duplicate/disabled test was found.

The closing statement "no files outside output/ were created" is too broad:
the explicitly allowed extract-d73824c/ tree exists. Treat it as a claim of
no additional engineering/source mutation beyond authorized extraction and
outputs, not literal filesystem non-creation. Input hashes/closure are
independently unchanged. No old implementation or modified OpenFHE was used.

Source-correct arithmetic plus finite passing witnesses supports the audited
boundary; phrases such as "any key-switch defect caught" or "nonmutation
proven" must remain scoped to inspected code and enumerated observations,
not a universal claim about unobserved state or hypothetical implementations.
No production correction is requested by this review.

## Recommendations and next owners

R1 (composition coverage): open bounded follow-up after Pro review; Codex owns
dispatch/integration/hosted CI. R2 (cosmetic) deferred as above. R3 (defensive
compatibility rationale) is documented here; avoid a source-only comment edit
and unnecessary CI solely for it. No user decision is required for these.

Pro's independent audit at
https://chatgpt.com/c/6a9a4269-665c-83ec-b130-8e40fd86f2d7
continues unmodified. The independent precision delivery repair and fixed-key
BV proof audit also remain live. No Mac build/crypto, source/threshold change,
CI dispatch/rerun or external-agent interruption was performed to reconcile
this static result. High precision, repeated multiplication, paper parameter
regime, security/performance and conservative BV theorem remain open.
