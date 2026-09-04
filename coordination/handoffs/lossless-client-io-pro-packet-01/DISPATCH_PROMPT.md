This is a fresh, complete-context implementation assignment within the existing
Design Client IO Seam conversation. Do not assume that you can access my
filesystem, Git repository, private repository state, previous attachments, or
any fact from earlier messages. Everything authorized for this assignment is in
the newly attached archive:

`lossless-client-io-implementation-01-4ccc8fd.zip`

Expected archive identity: 1,294,234 bytes; SHA-256
`67cea2db1565550c7d96816d076a5d56be45e82f5175e9578e31ddbb50289f89`.
The attached `.sha256` sidecar records the same archive identity.

First extract the archive in a fresh scratch directory. Verify ZIP integrity,
the one-root layout, `MANIFEST.sha256`, all sizes and hashes in `MANIFEST.tsv`,
and these mandatory identities before doing design or code work:

- `TASK.md`: 23,771 bytes, SHA-256
  `707d366dcd4880450ac09ba4c1eb6195daf64def65333c305c20a099f8eadb1f`;
- clean-room implementation source base:
  `4ccc8fd2e7617625d27e58a53eb3489e99466ed4`;
- pristine official OpenFHE pin:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

Then read `TASK.md` completely before editing. It is the sole current
assignment and overrides older prose and the previous design-only request.
Follow every required-read path named in it, including the project workflow and
engineering instructions, the paper, exact official source, the confirmed seam,
the retained design return, independent reviews, source audits, and hosted
evidence boundaries. Do not ask for another interface confirmation: the seam is
approved for this bounded implementation slice.

The immediate deliverable is real production high-precision client I/O using
the public `HighPrecisionClientIO` seam and exact
`number<cpp_dec_float<100>>` values, exercised through Encrypt, the existing
DCP/Mult2/RCB/BindFirstMult2Rcb pipeline, and Decrypt without a binary64
slot-value API. Implement the four ordered patches required by `TASK.md` as two
genuine vertical TDD cycles: positive tracer plus malformed-key safety RED then
GREEN, followed by shared-DCRT-Params drift RED then GREEN. Append exactly the
new CTest #58 named `precision_client_io_first_mult2_contract`; preserve the
existing 57 tests, five API compile targets, and exact hosted Linux/Windows
workflow contract. Do not modify `double_ckks.cpp`.

Return four separately applicable ordered patches, the complete final files,
and every replay ledger, manifest, receipt, and archive required by `TASK.md`.
A generic design response, pseudocode-only response, metadata facade, plaintext
shortcut, exception-swallowing workaround, or combined patch that erases the
required RED states is not the deliverable. Use KISS/YAGNI and let unexpected
failures surface.

Do not compile or run cryptography on a Mac, use credentials/accounts/network,
push, merge, dispatch CI, or claim any unperformed test. Static source review,
exact arithmetic, patch replay, and package validation are allowed. Clearly mark
all compile/runtime/hosted results as `NOT RUN` unless actually performed in an
authorized environment. Codex will independently replay, review, commit, push,
and run Linux/Windows GitHub Actions later.

Take the time needed to finish coherently. Before your final response, inspect
the actual return ZIP and sidecar, verify every promised file is nonempty and
consistent with the chat summary, and provide working downloadable links. If a
source-level blocker genuinely prevents GREEN, return the exact minimal
counterexample and blocked ledger required by `TASK.md`; do not substitute a
generic plan or false success.
