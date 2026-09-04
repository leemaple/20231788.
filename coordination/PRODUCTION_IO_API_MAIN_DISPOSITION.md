# Production I/O source investigation: main disposition

Source-only research is accepted as input to a DESIGN-ONLY Pro handoff, not
as an adopted interface or working codec. The complete original159-line audit
and the author's separate fresh recheck were read by the main agent.
The recheck is by the SAME author, not a third-party independent endorsement;
its filename must not be used to imply otherwise.

Main independently recomputed25source sizes, SHA256 and Git-blob SHA1 values
and51exact-pin citation-file closures; the six newly retrieved sources were
again fetched through GitHub's contents API at the exact pristine pin and
matched byte-for-byte. The complete per-source result is retained in
evidence/production-io-api/main-source-verification-20260904.json.
Earlier main source inspection checked the public scheme overloads, actual
CKKS Poly decryption, RNS prefix handling, context Decode call and current
test-only fixture. These checks do not execute the proposed client route.

One original wording is qualified: context Decrypt invokes CKKS Decode on
the CKKS-encoded path after a valid result; invalid results return early and
other encodings take their own Decode path. The relevant successful CKKS
coefficient-clearing conclusion still holds. Keep the original audit hash
intact and read it with this qualification and the author's recheck.

The candidate public scheme Poly-decryption path preserves configured CKKS
polynomial flooding; direct DecryptCore does not. Neither automatically
retains all standard REAL Decode output processing. Pro must decide and
document the intended client output policy, not silently weaken configured
protection to meet a precision number. No evaluator may use secret decryption.
Partial-slot stride extraction and all-coefficient canonical evaluation must
also remain distinct in the design and its independent acceptance oracle.

For standalone virtual-dispatch inspection, include the recheck's two
already-pinned declaration headers base-pke.h and rns-pke.h along with the
25listed sources. Their inclusion is source context, not a new dependency
or assertion that a reference excerpt set can build OpenFHE.
Any existing ordered-Q/key research necessary for the repeated-operation
handshake must be provided with its matching pinned sources too.

The serialization discussion identifies metadata that would otherwise be
lost IF transport is used. It does not add a general persistence subsystem
or make a broad serialization test suite a new project completion gate.
The small client interface and its tests remain unconfirmed with the user.
The separate pending stale-cache checker question authorizes neither this
interface nor implementation. Pro may propose concrete declarations and one
test contract, but no implementation/test patch at this stage.

No source/test/build/CI modification, Mac compilation, cryptographic run,
precision measurement or benchmark is performed by this research increment.
Eight repeated squarings, paper parameters and statistics remain required.
