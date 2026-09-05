# H128 source-closure return acceptance

Date: 2026-09-05 (Asia/Shanghai).
Disposition: ACCEPT_DIAGNOSTIC_SOURCE_CLOSURE.

Accept only the source-input closure for the existing N=256, 128-packed-slot
h128 key-pair diagnostic. The previous IC-01 and IC-02 input gaps are closed;
the frozen engineering source, test oracle, profile and hosted evidence are
unchanged. This is not a new cryptographic test, fresh CI result, full-project
acceptance, paper-size eight-operation result, or security/performance claim.

## Frozen identities and actual return

The acceptance working tree was `codex/paper-h128-keypair-01`, clean at
`ca44094da2b329117a88089740aa74de34e4b21b` before these two receipt files.
The tested diagnostic source remains
`1192200f558c69c0967e8306ed1a8bddf786ca34`; supplied evidence remains
`a5d6f933e26fbf58ab06ee679faa444f441567b8`. Official source is pristine commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| Input build-02 `paper-h128-final-review-source-closure-1192200.zip` | 2328140 | `8882265a6344f519b9a00adfab0e64436882b65c2b1f40a26d18c34cec0cf18f` |
| Actual new return `paper-h128-source-closure-1192200-review.zip` | 71210 | `8010ae05c8ffa66e3c83d4156fd70353afbe73272782b55944f2fbcc6af51373` |
| Return `.zip.sha256` sidecar | 111 | `e88ca1f9bc0043fa53b867b5a7fb9e05a5e606d1f99d646af1d94bab8e5baa8e` |
| Return `MANIFEST.json` | 1330 | `dbcfdc41f7f4f4f0333b3853489c170cb42dc5669e973cb7cce2e8ae2e3da4ed` |
| Returned `verify_source_closure.py` | 29779 | `ade6fb6e13805c482a2d9c199efc38fa2d4124d77a5c0865a6662991853559c0` |

The input is retained under
`artifacts/handoffs/h128-final-review-source-closure-01/build-02/`.
The new return, matching sidecar, extracted files and mechanical replay output
are under `artifacts/handoffs/h128-source-closure-return-01/`.

Observed archive integrity: CRC passed; exactly five regular 0644 members,
589130 expanded bytes; four manifest payloads with `MANIFEST.json` as the sole
self-exclusion. Every payload byte count and SHA-256 matches, with no missing
or extra member. The sidecar names and hashes this exact return ZIP. Root's
completed path/type/NFC-casefold safety audit and gitleaks scan passed with
zero findings. The acceptance author independently reconfirmed CRC, member
names/types/counts, expanded size, sidecar and all artifact identities above.

## Actual Pro state and review disposition

Conversation: [Implement Keypair Adapter Package](https://chatgpt.com/c/6a9b5ebc-6aa0-83ec-ab39-5eaf91ca6da5).
The original source-closure dispatch is retained in `DISPATCH_RECEIPT_01.md`
and its paired JSON.

Root observed the existing conversation terminal at
2026-09-05 07:42:06.208Z (15:42:06.208 Asia/Shanghai): four user messages,
no Stop control, visible UI label `6 Pro`, and `Worked 24m 47s`. The UI label
is a visible product label, not independent backend-model authentication.
This is the new source-closure return, not reuse of the historical blocked
return embedded in the input packet.

Root fully read the actual `REVIEW.md`, `FINDINGS.md` and returned verifier.
The return records overall `ACCEPT_DIAGNOSTIC`, Standards
`ACCEPT_DIAGNOSTIC`, Spec `ACCEPT_DIAGNOSTIC`, IC-01/IC-02 `CLOSED`, and
zero actionable P0/P1/P2 findings. It is **one reviewer with two review axes**,
not two independent reviewer seats. The disposition is retained as review
evidence; it is not accepted as authority without source and integrity checks.

## Independent mechanical and reference reconciliation

After full script inspection, root actually ran the current returned verifier
in isolated offline execution against the frozen source-closure input. No
OpenFHE build, crypto, NTT, network or CI execution was performed. Temporary
patch-application replay and exact-arithmetic checks are mechanical source
checks, not actual RED/GREEN runs. The entire parsed mechanical object equals
the new return's `EVIDENCE_CHECKS.json` `mechanical_checks` object, not merely
selected fields or a PASS marker. The acceptance author independently
reconfirmed this full parsed-object equality and the retained output identity:

- `artifacts/handoffs/h128-source-closure-return-01/root-mechanical-checks.json`
- 375691 bytes; SHA-256
  `4cb1069de014bf93d9b82b8f19c4be84b9f493340acd449c8396a5773369a9bf`.

Separately, an independent Codex agent used its own bounded read-only parser
and approved `git show`/`git rev-parse` reads, without importing or running
any returned or archived payload script. Its complete audit is copied
byte-for-byte into `RETURN_INDEPENDENT_ANCHOR_AUDIT_01.json`:

- 11510 bytes; SHA-256
  `6f5116807611d397ac8d34745b91a9ce1eaed204647bcf5109ad8ad73c48baab`.
- All 30 reading-map records: 23 file sizes/hashes and seven directory counts.
- Independently re-extracted 92 explicit narrative occurrences: REVIEW 67,
  FINDINGS 25. Report line numbers, multiplicity, paths and ranges match.
- All 55 structured references across nine groups. Their union with narrative
  references is exactly 83 unique anchors in 35 files. Every source range is
  in bounds; every file size/SHA-256 and original-byte inclusive line-range
  SHA-256 matches. This was a complete reference audit, not sampling.
- All 263 manifest-declared Git-origin entries match actual approved
  commit/path membership, blob IDs and exact bytes. This is stronger than
  merely recomputing a Git blob hash from packet bytes. The four added
  official files were also authenticated individually against the exact pin.
- Zero reference or bounded caller-closure findings.

The ignored original result was not modified. The independent audit script
is retained at
`artifacts/handoffs/h128-source-closure-return-01/independent-anchor-audit-01/audit_references.py`,
SHA-256 `09babb1f34486baaf78c159f96ab6371ddb4f8b1e4c0d8f159c34764dd28912f`.
Its source-reading conclusions are retained alongside it in
`SEMANTIC_CLOSURE.md`, SHA-256
`1c16c01be7f6f279c0e798a8d5591eaa93674ccb19cbadc756e957f630ff3cc8`.

## Source-observed closure and bounded inference

IC-01: the unchanged test's SK-first Decrypt caller reaches the actual DCRT
specialization and installed PKECKKSRNS Poly-output override. Both diagnostic
ciphertexts have three Q towers, so the added override selects full-Q CRT,
not the NativePoly/single-tower alternative. The added `decrypt-result.h`
resolves the called length constructor and validity/integer-scale metadata;
the context separately copies CKKS real scale, degree, level and slots before
Decode. The frozen FIXED_NOISE_DECRYPT profile does not enter the flooding
branch. This closes the actual caller, not an assumed generic base body.

IC-02: the unchanged ordinary two-ciphertext EvalMult caller reaches the
keyed wrapper, inherited keyed body and added intermediate RNS override.
FIXEDMANUAL adjusts only unequal tower counts; the same-level clones retain
Q without rescale. The existing core produces three convolution components;
official HYBRID switches the third component back to the same returned SK,
returns to current Q and leaves two components. The square consequently uses
IC-01's Poly/CRT path. No additional directly necessary missing definition or
concrete contradiction was identified in these selected chains.

These are source observations and branch-specific inferences. Numerical smoke
success remains supplied, previously accepted hosted evidence. Nominal square
scale 2^80 is not 80-bit accuracy; the h128 diagnostic's independent ordinary
API smoke tolerance is 1e-5. Acceptance does not assert arbitrary hidden-table
safety, concurrent mutation safety, production I/O completion, paper-size
eight-operation correctness, full mathematical proof or security strength.

The user's cancellation of 1000-trial experiments remains effective. This
source-only closure introduces no replacement repetition or performance
gate, new engineering change, new CI dispatch, or user decision. Remaining
project correctness gates retain their own scope and evidence requirements.

## Retrieval and publication note

The attempted Browser.setDownloadBehavior method was unsupported and stopped
before any download click. Page.setDownloadBehavior returned success but did
not relocate the actual downloads from the user's Downloads directory. At
07:43:16Z, the new ZIP and sidecar buttons were clicked; only the sidecar
arrived. Targeted read-only checks found neither a ZIP nor an in-progress ZIP.
At 07:44:29Z, the ZIP alone was retried once; its partial download and subsequent
completion were observed. The initial missing ZIP's cause is not established.
No Pro generation was stopped, refreshed or submitted again. Both final files
are now retained in the artifact directory above; no further download is due.

These two documentation receipts were drafted before an agent quota failure;
root recovered them from the actual worktree, fully read this receipt, and
confirmed the JSON is byte-identical to the previously completed independent
audit. The quota failure did not invalidate that earlier completed audit and
is not represented as any new review or test execution. Publication is a
documentation-only [skip ci] commit, separate from the tested source.
