# Independent reconciliation of final ZCode Pair review

## Exact evidence and scope

Initial branch `codex/pair-mult2-composition-01`, clean HEAD
`2a048f4e6c9aee0fee46a1ca2d37030ac5fbfbf3`. Active source/build/test/workflow
diffs against both tested `b48b54e22f14bbfe988a6890f1b03eac9efb11a3` and
packet source `da50a7e1af05eaaff40d7ce82068e65da02815a2` are empty.

Read the complete original `output/REVIEW.md` and sidecar in
`/Users/lifeng/Documents/20231788-openfhe-zcode-pair-composition-final-review-20260904`.
Review SHA256 `dba324a7c8bbc986c9aa5a2bee7a70b8f76b937e1c942b7411f932ab680383c8`;
sidecar verification PASS. Sidecar SHA256
`64a6993975e6e6bf33e6f99478ff4c33f70343b6ee8b403d11a3e3468a79eafc`.

Independent bounded checks: local ZIP equals the explicitly authorized handoff
ZIP; SHA256 `4417299f03ab04277aa479cfa8a761d03b854c957c7b17d6174ed90f2610e64b`.
All 111 unique safe regular unencrypted members pass CRC and equal `input/`
byte-for-byte; all 110 non-self manifest size/hash entries pass. All 86 Git
inputs equal the advertised commit/path blobs. The other 22 reference files
are covered by the manifest check, not freshly fetched. Wrapper is 2,832 bytes,
SHA256 `a3c18e48edfd8c5fcf041f44d7e9ba9e81d31d7df760a340491a1f293bc8bcd2`.
Main separately reports equality with its stored original wrapper; this audit
independently establishes its current hash/size, not that historical comparison.

Stage2 raw logs and job JSON agree: run `33871723090`, Linux `101019032191`,
Windows `101019032537`, tested HEAD above; each focused 2/2, full 55/55 with
exact CMake name order, warning build and five explicit API targets successful.
Four composition certificates per host satisfy checked per-path/coefficient
inequalities and 1e-3 error threshold. Existing Codex spec/standards notes agree.
These are retained hosted observations, not new execution.

## Required disposition corrections

1. **Authority:** REVIEW:11-12,319-345 calls P2 items “accepted risk/accepted.”
   These are reviewer recommendations, not evidence of user acceptance.
   Record P2-1 as an explicitly scoped key-row-presence limitation; P2-2/3 as
   frozen-fixture maintenance notes. Main owns the narrow integration decision;
   no user risk acceptance or new digest requirement is inferred.

2. **P2-1 overstatement:** REVIEW:324-325 cannot promise every functional
   corruption is caught. Test:918-939 accepts bounded nonzero coefficient
   error, and :1022 accepts bounded decoded error. Material/key/cache mutation
   outside the observed checks can escape. Preserve the existing enumerated
   scope; do not add deep-key mutation testing merely to satisfy this prose.

3. **P2-2 overstatement:** REVIEW:335-338's “never pass falsely” is invalid for
   changed fixtures. A read-only binary64/Fraction check showed
   `0.1 * 0.2 == 0.020000000000000004` is true, while exact multiplication of
   those binary64 inputs differs from the represented expected value by
   `1080863910568919/649037107316853453566312041152512`.
   Equality alone cannot certify exactness. The current independently checked
   frozen dyadic arrays remain valid; future changes need independent exact
   representability/arithmetic verification, not just this equality test.

4. **Numeric wording:** REVIEW:289's finite decimal is a rounded display, not
   exactly `1152921504606846976/1152921231876374273` (independently checked).
   REVIEW:189 should say `32*epsilon*max(abs(ratios))`, not exactly 32 ULPs
   (test:995-999). REVIEW:287 should identify numerator comparison
   `2*errorNumerator <= boundNumerator`, equivalently error <= bound with
   the printed denominators, rather than doubling the printed error fraction.

5. **Minor source description:** REVIEW:141-142 should say reused old helpers
   remain unchanged, not that every called helper pre-existed: the diff adds
   `ComposeFirstMultInput`, `ExpectedPairInputCoefficient`, and others at
   test:1180 onward. This does not change its correctly identified additive scope.

## Identity, conduct, and conclusion

Main reports real UI completion at 21:47, Worked 12m56s, GLM-5.3, LOCAL STATIC,
with current reasoning selector Low. Earlier dispatch Max is historical UI
state, not an attestation of final inference effort. This sub-audit did not
observe the UI itself. Raw model self-description remains separately labelled.

REVIEW:380-384 self-discloses one mistyped outside-directory write and cleanup.
Record a self-reported scope violation; cleanup is not independently verified
here, and preserved inputs do not erase the violation. Do not access the
forbidden typo directory to investigate. Original return stays byte-exact.

No newly demonstrated current code defect or CI mismatch emerged from this
reconciliation. The bounded acceptance recommendation can be considered after
these corrections; it is not paper precision, repeated multiplication,
all-key/security proof, user acceptance of risks, or full-project completion.

Only read-only source/log/hash/ZIP/Git-blob checks and tiny exact arithmetic ran.
No build, crypto, network/UI, source/Git mutation, or outside-scope access.
Main's untracked archive appeared concurrently and was untouched. This note is
the sole write by this sub-audit; no commit or push was made.
