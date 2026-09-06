# Changelog and review checklist

## Changes

Two new test-local Python files only: exact arithmetic/serialization/compression
primitives and their standalone deterministic tests. Test-first and implementation
patches are nonoverlapping. All 39 existing engineering paths remain unchanged.
No C++ signature, production API, oracle arithmetic, precision gate, metadata,
input formula, scale rule, control inventory, CTest entry or workflow is changed.

## Independent review checklist

- Confirm `PARTIAL_NOT_GREEN` and that the missing seven helper symbols remain
  missing; reject any attempt to present this as a drop-in diagnostic GREEN.
- Verify input/output manifest closure, exact baseline identities and patch order.
- Inspect exact K and classification precedence separately from conditional-model
  applicability, which is not implemented.
- Inspect integer half-even rounding, carry, unique zero, grammar, quanta and range
  limits. The C++ validator and represented-value extraction remain missing.
- Inspect gzip member/header/CRC/ISIZE/decompression checks and external identities;
  do not confuse compression validity with endpoint schema or upload eligibility.
- Inspect strict JSON envelope scope; complete status values/relationships, source
  ownership, original CTest parsing and failure propagation remain missing.
- Read actual RED/implementation stdout, stderr and exit evidence. No hosted or
  numerical observer result may be inferred from these primitive tests.

## Source-reference map

`evidence/SOURCE_REFERENCE_MAP.json` records exact packet paths, sizes, SHA-256 and
line/text matches for the adopted v1-r1 spec/test plan, current declarations,
original oracle and paper-test seams, correctness scope and retained acceptance
records. `evidence/INPUT_VERIFICATION.json` closes all source payload identities.
Historical statements retain their historical authority only. No external
OpenFHE/Boost version or sparse-reference closure claim is introduced.
