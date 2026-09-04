# Pro final Pair audit and tri-party disposition

Observed 2026-09-04 Asia/Shanghai. This closes the LIVE final Pair static review
recorded in PAIR_COMBINED_REVIEW_D73824C.md, not the full project goal.

## Completed task and verified return

Conversation title "实现 Pair 加减补丁":
https://chatgpt.com/c/6a9a4269-665c-83ec-b130-8e40fd86f2d7,
Ego space122, tabD8178F5CA273531DF5B0AC24424CFA0D.
Submitted once17:01:29 CST. Completion was observed by17:41 with Stop absent
and final/download controls present. No stop, refresh, reminder or repeated
prompt occurred. Visible UI was Pro; the return self-identifies GPT-5.6 Pro.
Provider-side routing is not independently proved by that self-report.

Source d73824c2d382013c3aadbd7cb29c57008e839714, documentation snapshot
f550eac1251f2005222e60aa4f07cc2e57380c46, official OpenFHE
df495ba2e91739a6dc8f1de254fc5a41155ce504.

Downloaded artifact:
 /Users/lifeng/Downloads/pair-combined-review-d73824c-static-review.zip
25242 bytes, SHA256
c90f58728495e64e2c229bbfc86d79dc1080ccbd25a5f98bf3c908b4b30b44b4.
The separately downloaded basename SHA sidecar matches this exact file.
An initial native click event timed out; the completed page and absence of
downloads were inspected before artifact-control recovery. The final directory
contains one ZIP and one sidecar. No Pro task was restarted or duplicated.

Exactly three nonempty safe regular ZIP members; no duplicates/symlinks/unsafe
paths and CRC passes. Required-file completeness is satisfied:
- REVIEW.md:536 lines,35487 bytes, SHA256
  62b609888736605dd6ca61aa4def0096322a173040470a551e182c85e5901c86;
- EXECUTION-LEDGER.md:90 lines,9395 bytes, SHA256
  afd81ce7747eef396688b349ece78a9d34ec8b1144d512369030a1731b2a99ce;
- MANIFEST.sha256:17085 bytes, SHA256
  1af0c333cf110bc3bd82d04443f6707c76753948b0c5b8f6266ea1380626b558.

Codex recomputed ALL140 non-comment manifest hashes:
1 input archive +71 extracted input files +66 complete original-return files
+2 output files. Each group's full path set closes exactly; output manifest
self-exclusion is explicit. Input/original entries were resolved from the exact
retained ZIPs, not guessed paths. No supplied script/binary was executed.
Gitleaks8.30.1 extracted-output scan: zero findings,61967 bytes.
All three files are retained byte-exact under
coordination/returns/pair-combined-pro-d73824c/.
Four original Markdown hard-break trailing spaces in REVIEW.md (lines3,4,5,71)
are preserved as delivered; diff-check excludes only that byte-verified original
artifact. Newly authored receipt and other archived files pass whitespace checks.
Manifest paths are logical input-archive/input/original-return/output roots,
not relative filesystem paths under this archival directory.

Codex read the full536-line review and90-line execution ledger.
The reviewer explicitly did not configure/compile/run OpenFHE or the project,
perform CTest/crypto/CI, install dependencies or mutate the input repository.
Its stated environment and static commands are reviewer reports; hosted
53/53 results are quoted supplied evidence, not its own runtime.

## Finding-by-finding disposition

Verdict PASS_WITH_GAPS; reviewer found P0=0,P1=0,P2=0 for the Pair boundary.

L-1 confirmed: official Clone has a fresh outer metadata map but shared entry
pointers and parameter provenance. Existing tests promise enumerated per-call
nonmutation, NOT arbitrary future mutation isolation. No source change needed;
Codex keeps that exact boundary in later documentation.

L-2 confirmed: native values, selected context facts and EvalMultKey A/B data
are deeply snapshotted where applicable; hidden state, other key caches,
unknown metadata subclasses and concurrency are not exhaustively covered.
No blanket state or thread-safety claim is adopted. Additional instrumentation
is deferred to an actual future contract, owner Codex, not required here.

L-3 independently resolved for recorded source/run association: Codex freshly
queried GitHub run33854419062 (attempt1, completed/success), exact branch
codex/integration-01 and head d73824c2d382013c3aadbd7cb29c57008e839714.
Earlier read-only job verification retained both exact job IDs and every
project warning/API/CTest step: Linux100964299802, Windows100964299593.
Stored totals53/53,0.68s and53/53,2.27s match both full project log sections.
Current production/tests/CMake/workflow remain byte-identical to d73824c.
No extra rerun is needed merely because the offline reviewer lacked GitHub.

N-1 confirmed and made precise from exact historical Git:
e22a2e1fb343731ca89cc0ea2e6444e7988bdc5e:src/double_ckks.cpp:648-650 throws
std::logic_error("DoubleCKKS: Add is not implemented"). The runtime test's
inner handler catches std::invalid_argument and compares the longer
"Add arithmetic is not implemented" text. Thus BOTH exception TYPE and text
differ from the dedicated scaffold path; the actual red reached main's
std::exception handler, as red-linux.txt:87-88 records. This refines the prior
ZCode cosmetic finding: wrong string alone is not the full historical cause.
The exact missing-feature source, sole failing case and fatal exit remain
authentic. No false pass and no production defect; do not rewrite history.
For any future genuine scaffold cycle, Codex owns checking expected type/text
before recording red evidence.

Other source/test conclusions agree with Codex and the independent ZCode
audit: componentwise high/high-low/low arithmetic, correct Sub order,
left-right-compatibility validation, exact CRT/RCB oracles, lifecycle/alias/
keyless/provenance matrices and the six narrow integration corrections.
The original HasNonzeroValue issue was helper availability/order across
patches, not an actual duplicated helper in the final original source;
the current file has exactly one helper. No duplicate-defect claim is added.

## Scoped tri-party closure and next regression

Codex integration/source review, ZCode "Static final Pair Add/Sub six-question
audit", and this completed Pro audit are substantively reconciled. No mandatory
Pair source fix remains in the audited53-test boundary. ZCode's additional
F5/R1 gap still applies: no retained test feeds an Add/Sub result into first
Mult2. Pro's no-defect conclusion does not establish coverage of that path.

Owner Codex: continue the smallest test-only Add->Mult2 then Sub->Mult2
regression with complete context and frozen independent expectations.
That is additional coverage of already-real red/green behavior, not a new
missing-feature TDD claim. Do not reduce the full goal to Pair acceptance.

True high-precision I/O, first-Mult2 precision, repeated lifecycle,
conservative BV bounds and paper-scale/security/performance evidence remain
open. The independent precision repair and BV Pro tasks were not interrupted.
The newly committed Codex centered-lift source note is a separate derivation,
not silently adopted by this Pair review.
