# Root independent review of fixed512 widening

The root read the actual Linux GCC/Boost diagnostic, the complete change to
diagnostics h/cpp/test.h, its source ledger, and the official Boost 1.83
cross-backend assignment/copy_and_round/right_shift_generic source retained in
the clean-room handoff. This is independent of the bridge's author, not an
additional model-provider attestation or a C++ execution result.

No blocking source finding in this bounded correction: a finite source has at
most 512 represented significand bits. `frexp` and exact power-of-two scaling
expose them as a signed integer. Same-source reconstruction checks extraction;
that integer fits the destination's 768-bit precision, and checked `ldexp`
restores the source exponent. No rounded decimal spelling of the original
noninteger value or direct fixed512-to-dynamic768 assignment is used. Negative
values and both admitted exponent extremes are explicitly covered in authored
fixtures. The original Horner `R(Int)` calculation remains unchanged and occurs
before the bridge.

The new source is not yet compiled or run. Actual Windows synthetic PASS at
03:04:03 UTC belongs only to 8d7e6f0, not this correction. Linux's current
8d7e6f0 failure remains the active compile baseline. No warning is disabled and
neither Boost nor the frozen oracle/production files is edited.

This needs one new-source hosted compile/self-test check. The previous run is
terminal and must not be rerun. The next exact draft branch will retire the
prior trigger, keep all old60/API gates and the same resource limits, and skip
the live paper CTest. Only after actual result inspection may the new bridge be
called compiled/passing. No full paper chain, quota retry, random trial or E80
claim is authorized by this source review.
