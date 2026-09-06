# Scalar canonical-rounding correction, one vertical slice

Base: d02f5ffac31dd59e02c38d567efdef8302b938ec. The byte-exact Pro return is immutable under coordination/fs-residual-endpoint-partial-return-01/pro/. This candidate is not integrated into active tests or the live observer.

The independent Spec review found premature lower-exponent rejection in the returned Python canonical formatter. The confirmed seam is canonical_decimal(Fraction), governed by ENDPOINT_SPEC v1-r1 section 6: round the exact represented dyadic with carry normalization, then require the final five-digit exponent to fit. Source input validation and nonzero preservation remain in force.

Write one regression before changing the copied formatter. Its expected minimum-exponent spelling is a literal from the grammar, not produced by the formatter. Use an exactly represented dyadic with at most 768 significant binary bits just below 10^-99999, prove its distance lies below half the previous-bin decimal step, and check both signs. It must round into the legal -99999 exponent; smaller non-carrying values must remain rejected, never flushed.

Retain actual focused RED against unchanged returned implementation, make the smallest bound correction, then execute focused GREEN and the unchanged original 23 tests together with this regression. These are bounded scalar/Python checks only, not C++ compilation, endpoint observation or E80 acceptance. Existing Standards findings are not silently closed by this correction.

No active engineering source, CMake, workflow, paper-chain parameters or historical evidence is modified. The separately missing C++ helpers remain Codex-owned; an analogous pre-rounding lower-bound check must not be copied into their future formatter.
