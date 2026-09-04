# Precision fixture CI safeguard: proposed boundary, not implementation

2026-09-04, follows final review P2-4. Existing precision fixture returns a
Plaintext whose DCRT element is high-precision but binary64 packed-value cache
is a zero placeholder (tests/precision_dcp_rcb_fixture.cpp:258-261).
Current manual source guards are not automated CI enforcement.

Proposed minimal safeguard: a build-before-compile read-only lexical checker,
scoped to fixture/header, its two precision contract consumers, their current
project dependencies and consumer/target admission. Reject forbidden ordinary
Decrypt, packed-value getter and explicit serialization identifiers; distinguish
the legitimate IndependentDecrypt/IndependentDecryptPair names, comments and
strings. Missing/empty files and unexpected new consumers must fail closed.
Preserve ordinary non-precision tests using standard Decode; no production API
change, no broad dependency scan and no Mac build or cryptographic execution.

This proposal is finite lexical enforcement, not complete C++ dataflow proof.
Macros, indirect wrappers and unknown APIs need explicit limitations. Avoid
overclaiming safety from substring/regex matching. Any checker should report
exact file/line/rule. Keep scanner dependency and Windows interpreter explicit.

An independent Codex static reviewer analyzed this scope without writing any
test/checker or running proposed mutation cases. The main agent read the
proposal. To satisfy the tdd skill's confirmed-seam rule, a NONBLOCKING question
was sent to the user around21:35CST asking confirmation of this build-before-
compile read-only validation boundary. No answer was observed when this note
was written; no new test/checker has been authored. This is not authorization
to implement at an unconfirmed seam. Existing TEST_SEAMS.md confirms module
public-operation seams, not this separate checker CLI.

If confirmed: record the chosen boundary, then one vertical RED/GREEN slice
at a time using tiny temporary source samples and exact checker diagnostics;
missing-script failure is not a valid misuse-rejection result. Prove allowed
IndependentDecrypt and comments stay accepted. Actual hosted integration
follows without modifying production or weakening existing55 tests.
If not confirmed: retain explicit bounded deferral, not a fabricated guard.
OwnerCodex. This optional test-infrastructure question must not block the
live Repeated Mult2 Pro design task, Pair external review, or other already
confirmed public-operation work. No full-project completion is claimed.
