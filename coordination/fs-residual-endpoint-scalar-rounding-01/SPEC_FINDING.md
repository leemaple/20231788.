# Independent Spec finding before correction

Reviewer: `/root/routing_forward_test_0800`, requested `gpt-5.6-sol` / medium, backend unattested. First-pass source was the immutable Pro return at base `d02f5ffac31dd59e02c38d567efdef8302b938ec`. It did not read the Standards report or author's verdict before reviewing.

**P2 — Premature lower-exponent rejection.** Original `endpoint_evidence_primitives.py:213` rejects a floor-decimal exponent of -100000 before half-even rounding/carry. Some bounded exact dyadics below 10^-99999 round to the legal spelling `+1.` + 109 zeros + `e-99999`. The spec's final five-digit exponent limit must be applied after that carry.

The reviewer constructed `v = floor(2^333000 / 10^99999) / 2^333000` in a local python3 heredoc importing the original module with importlib.util. Its retained reported output was:

```text
dyadic True
bit_lengths 809 332999
below_target True
rounds_to_lower_boundary True
EvidenceError serialization exponent overflow
```

The original command's Python version and precise execution time were not captured; neither is inferred from a later command. Its 809-bit numerator witnesses the Python function's general bounded-Fraction scope, not a <=768-bit significand. Root's new regression uses a different 766-bit dyadic and a stricter exact carry-interval inequality, so it additionally applies to the intended binary768 represented-value seam.

No other semantic defect was established in the declared partial slice. The reviewer distinguished formulas/classifier/grammar/gzip/JSON envelope from the still-missing complete schema, Decimal replay, CTest/publication and C++ helpers. Its first pass did not rerun the 23 tests or perform C++/FHE/FFT/browser/CI work. This is independent reasoning within Codex, not attested provider diversity.
