Continue the production lossless client I/O implementation after the ended
BLOCKED_INPUT_CLOSURE response. Codex and an independent reviewer confirmed the
five missing official source files. The attached corrected packet fixes that
specific input omission. This is not a duplicate of a running turn, another
byte-identical upload of the incomplete packet, an interface change, or a request
to stop at another design/health-check answer. Complete the original four-patch
implementation if the now-complete sources support it.

Treat this as a complete-context assignment. Do not assume access to my local
files, Git repository, previous attachments, another conversation, or memory.
All original source, paper, task, reviewed design, fixed contract/vectors,
independent reviews, official source and evidence are reattached here:

lossless-client-io-implementation-01-4ccc8fd-source-correction-01.zip
3,132,684 bytes; SHA-256
4c8295d56ca59d39441adbdc2fe24e87bb2bafaab4128b19265ba159337f329c.
The second attachment is its .zip.sha256 sidecar.

First verify archive size/hash, sidecar, CRC, safe one-root paths, regular
members, duplicate/case-fold safety, exact MANIFEST.tsv closure and its
MANIFEST.sha256. The ZIP has 122 regular members, 120 manifest payloads, and
keeps the original root lossless-client-io-implementation-01-4ccc8fd/.
New MANIFEST.tsv: 14,893 bytes; SHA-256
3308504e4a6db8ead01bdc4da70810536eda3862758529f819005cde56b09b89.

Read SOURCE_CORRECTION_01.md and SOURCE_CORRECTION_01.json. Every original
111 payload is byte-identical, including all 47 project files, paper, task,
design return and original metadata. The old manifest/sidecar are preserved
under provenance/original-input/. Old PACKET_README.md and SOURCE_PROVENANCE.json
describe the historical 59-file source selection; the corrected selection is
64 official-full files. Do not mistake preserved historical counts for a new
manifest error or rewrite the original evidence.

Exactly these five canonical official source files were added from OpenFHE pin
df495ba2e91739a6dc8f1de254fc5a41155ce504, with size, SHA-256, Git blob and
fresh-upstream/legacy-source byte equality verified independently:

- official-full/src/core/lib/math/dftransform.cpp
- official-full/src/core/include/math/dftransform.h
- official-full/src/pke/include/encoding/ckkspackedencoding.h
- official-full/src/pke/include/encoding/plaintext.h
- official-full/src/pke/include/encoding/plaintextfactory.h

The correction documents map older official-openfhe/ short citations to these
exact canonical paths. Resolve those citations through that authenticated
mapping; the actual source bytes are now supplied, not reconstructed from prose.
The old incomplete archive remains historical identity 1,294,234 bytes /
67cea2db1565550c7d96816d076a5d56be45e82f5175e9578e31ddbb50289f89.
Do not run a checker hard-wired to that old outer ZIP identity against the new
packet and call the deliberate transport change an implementation blocker.
Preserve all substantive missing-source and source-integrity checks.

Mandatory unchanged task identity: TASK.md = 23,771 bytes; SHA-256
707d366dcd4880450ac09ba4c1eb6195daf64def65333c305c20a099f8eadb1f.
Implementation base: 4ccc8fd2e7617625d27e58a53eb3489e99466ed4.
Task overlay: a6937904887d17dffcfcf8a2367b9b4244c52961.
Pristine official OpenFHE pin: df495ba2e91739a6dc8f1de254fc5a41155ce504.
Read TASK.md completely, then every required-read source and review it names.
TASK.md remains the controlling engineering assignment, including its approved
seam, exact N64 diagnostic profile, acceptance thresholds and delivery order.
This correction changes input closure only. No new user confirmation is needed.

Implement the production HighPrecisionClientIO public seam with exact
number<cpp_dec_float<100>> input/output and no binary64 slot-value API:
Encrypt -> existing DCP/Mult2/RCB -> BindFirstMult2Rcb -> Decrypt.
The evaluator must never receive or use a secret, decrypt/re-encrypt, bootstrap,
or Section 6.2 refresh. Do not modify double_ckks.cpp or replace upstream
cryptographic primitives. Preserve the 57 existing CTest bindings, commands,
order, assertions and all five API targets; append only #58
precision_client_io_first_mult2_contract as specified by TASK.md.

Produce four ordered patches as two genuine vertical TDD cycles: tracer and
malformed-key RED, production GREEN, supported clone/shared-Params drift RED,
then immutable value snapshot and live drift-revalidation GREEN. Preserve the
frozen oracle and tolerances. A combined patch that erases RED, metadata-only
facade, plaintext shortcut, pseudocode or another design-only response is not
the requested deliverable. Use KISS/YAGNI; let unexpected failures propagate.

One source-supported API caution: at this exact pin, the CKKS CCParams
specialization disables SetEncryptionTechnique, SetMultiplicationTechnique,
SetMultipartyMode and SetThresholdNumOfParties. Do not call those throwing
high-level setters. Required DEFAULT values must instead be checked on actual
constructed parameters. Supported low-level CryptoParametersCKKSRNS constructor
and PrecomputeCRTTables arguments are a separate API and must not be confused
with these disabled setters. Recheck the supplied specialization/defaults and
TASK.md's parameter requirements rather than assuming generic Params behavior.

Return every ordered patch, complete final file, frozen-contract copy, replay
ledger, CTest inventory, manifest, receipt and final ZIP/sidecar required by
TASK.md. Verify each promised file is nonempty, all hashes and the final archive
agree, and the package can be freshly extracted and replayed. Provide working
downloadable links. If another source-level blocker genuinely prevents GREEN,
return its precise minimal counterexample and required blocked ledger; do not
invent results or silently relax a gate.

Do not use credentials, accounts or network, push, merge, dispatch CI, compile
or run cryptography on a Mac, or claim unperformed tests. Static source review,
exact arithmetic, patch replay and package validation are allowed. Clearly mark
all unperformed compilation, cryptographic/runtime and hosted work NOT RUN.
Codex will independently review, commit/push and execute the separate real RED
and GREEN stages on Linux and Windows GitHub Actions. Take the time needed to
finish the full assignment coherently.
