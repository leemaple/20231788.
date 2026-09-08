# Read-only evidence validation

2026-09-08 23:12 CST, Codex root. No numerical experiment was performed.

- `collect_build_evidence.py` revision 2 completed with exit 0; five historical job logs retrieved, selected counts 59/52/57/49/32. Revision 1 receipt retained unchanged.
- Bounded Python stdlib assertions completed with exit 0: both receipts contain five jobs; IDs, raw CLI log byte lengths and SHA-256 values match across both retrievals; revision 1 selections are empty; revision 2 counts match; four configurations report OpenMP ON and found; annulus dependency configuration is skipped.
- `/opt/homebrew/bin/gitleaks dir coordination/build-provenance-20260908 --redact --no-banner` completed with exit 0, scanned approximately 94,241 bytes; no leaks found. This covers the collector, both JSON receipts and assessment as they existed before this validation note.
- Original receipt SHA-256: `228b3fa52a1d3df673ae23e9b7ebd6bb574a12f926ec8762d3e7a7b68d3f002a`.
- Revision 2 receipt SHA-256: `5817bb483db4209b4349fcfe812ec0911a0710332ea58a66ceedf9f87d232748`.
- Collector SHA-256: `fd8a911a3070ac017446083c22cd265d6224a547920c289fdc2436943e9e74a2`.
- Assessment SHA-256: `7ccd5a896ef334e5c8de7329a65b81d7f8ad49565e0bfd6f50120d774fce2b7f`.

These checks validate transport/selection consistency and recorded configuration facts, not an OpenFHE binary, sampler measurement, cryptographic correctness, or new precision PASS.
