# Returned full pair arithmetic test: pre-integration findings

Observed 2026-09-04. Codex read all1389 lines of the returned
final-changed-files/tests/pair_arithmetic_test.cpp from the previously verified
Pro archive735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4.
This file has NOT been integrated or compiled. The following are not test results.

1. Real complex coverage needs an explicit data-type gate. MakeContext does not
   select COMPLEX, yet TestPublicLifecyclesAndKeyIndependence passes a complex
   vector and claims complex coverage. Pinned official ckkspackedencoding.h:89-102
   shows the constructor's REAL path zeroes imaginary values; cryptocontext.h
   forwards the context data type to it. Before claiming genuine complex coverage,
   select and assert the context type and verify a nonzero imaginary plaintext
   component survives construction. The current Mult2 oracle explicitly sets
   CKKSDataType; its matrix must not be confused with this returned pair test.
2. ExpectedArithmetic uses a conditional expression over two different Boost
   expression-template operators. This is a potential compile portability issue,
   not yet an observed compiler failure. Prefer straightforward if/return branches
   materializing BigInt when this coverage slice is integrated; do not change any
   mathematical expected values or record a fabricated TDD red for a static fix.
3. The label left validation precedes right validation and compatibility uses a
   valid right operand. It proves rejection of the malformed left operand, not
   left-before-right order. A real order witness needs independently malformed
   operands with distinguishable expected diagnostics, or a narrower label.
4. Immutability snapshots enumerate native modulus/root/order, all coefficient
   values and metadata, but do not snapshot every aggregate or hidden context
   field. As for RS2, keep this claim narrow or add explicit scalar snapshots;
   pointer identity is not a deep-value proof. No actual mutation demonstrated.

The textbook cpp_int CRT and signed modular reduction oracle reads public pair
members and compares each component/tower coefficient and public RCB. Controlled
mod-Q half-boundaries, signs, self-inputs and reverse subtraction are useful
separate evidence from the first literal decoded behavior. They must not be
described as already executed. Add/Sub production remains direct validated DCRT
arithmetic; no normalization, tolerance fitting, hidden key or rescale operation.

Next owner Codex: close Sub runtime red-green before integrating additional
coverage one boundary at a time, retain any actual failure, then seek substantive
independent review of the final candidate and reconcile it against runtime data.
