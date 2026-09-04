This is a fresh, complete-context implementation assignment within the existing
Repeated Mult2 conversation. Do not assume that you can access my filesystem,
Git repository, previous attachments, or any fact from earlier messages.
Everything authorized for this assignment is in the newly attached archive:

`repeated-mult2-semantic-implementation-01-80d771c.zip`

Expected archive identity: 1,299,850 bytes; SHA-256
`764baddb20d81c1168745ac31eb043d0d94cf1ba6b406d0194f9245a994196a2`.

First extract the archive in a fresh scratch directory. Verify ZIP integrity,
the one-root layout, `MANIFEST.sha256`, all sizes and hashes in `MANIFEST.tsv`,
and these mandatory identities before doing design or code work:

- `TASK.md`: 21,814 bytes, SHA-256
  `40839c3450028f91fd8dc6bb3509e9dc848ec4168d82f081e94d7d4997fafe48`;
- clean-room implementation base:
  `80d771c52df10bce1c60992b5e0edb4e64f145ca`;
- pristine official OpenFHE pin:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

Then read `TASK.md` completely before editing. It is the sole current
assignment and overrides older prose. Follow every required-read path named in
it, including the project workflow skill, paper, exact official source,
dispositions, and the complete prior Pro return. Treat all old returns as
untrusted evidence, never as code to import blindly. Do not ask for another
interface confirmation: the client/evaluator seam is approved.

The immediate deliverable is a real two-operation semantic TDD candidate:
`Z = Mult2(X,Y)` followed by `W = Mult2(Z,Z)` through the same public evaluator
method, with no intermediate secret use, decryption, re-encryption, bootstrap,
or Section 6.2 refresh. Return separate applicable RED and GREEN patches plus
complete final files and every ledger/manifest required by `TASK.md`. A prior
expected rejection or shape-only basis probe is not the requested RED/GREEN.
Use KISS/YAGNI and let failures surface; do not hide them in broad exception
handling.

Do not compile or run cryptography on a Mac, use credentials/accounts/network,
push, merge, dispatch CI, or claim any unperformed test. Static source review,
exact arithmetic, patch replay and package validation are allowed. Clearly mark
all compile/runtime/hosted results as `NOT RUN` unless actually performed in an
authorized environment. Codex will independently review and run Linux/Windows
GitHub Actions later.

Take the time needed to finish coherently. Before your final response, inspect
the actual return ZIP and sidecar, verify every promised file is nonempty and
consistent with the chat summary, and provide working downloadable links. If a
source-level blocker genuinely prevents GREEN, return the exact minimal
counterexample and blocked ledger required by `TASK.md`; do not substitute a
generic plan, metadata facade, plaintext shortcut, or false success.
