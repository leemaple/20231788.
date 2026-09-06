# Endpoint exact widening — source-only ledger

Base before this bounded edit:
`b44e17d8b8090d6fbb685c60bce5918658ab7c9f`.

The agreed public test-local seam is
`paper_endpoint_contract::internal::WidenRepresented512(const paper_full_test::Real&)`.
Tests and its declaration were authored before the definition. At that point the
new calls had no definition and were expected to fail linkage; this expected RED
was **NOT COMPILED and NOT RUN** under the task restriction and is not claimed as
observed evidence.

Source-authored boundary cases cover:

- an all-512-bits-set significand at supported positive and negative exponents;
- negative sign and zero;
- infinity and NaN as `NONFINITE`;
- both sides of the checked exponent envelope as `MODEL_UNSUPPORTED`;
- `HornerConversionsSupported` using an exact full-width result from the
  unchanged decimal-string `R(Int)` path.

The implementation then replaced only the two fixed-binary512-to-binary768
conversion sites in `paper_endpoint_diagnostics.cpp`. Allocator-backed observer
binary512 widening sites were left unchanged. No production file, frozen oracle,
workflow, compiler flag or Boost source was modified.

Verification status: **SOURCE ONLY — NOT COMPILED, NOT RUN, NOT DISPATCHED**.
The retained Windows success belongs to the earlier exact source and is not a
GREEN result for this change. A new exact-source hosted build is still required.

Retained source-only checks:

- `git diff --check`: exit 0.
- Direct-conversion inventory: no remaining `paper_full_test::R` or Horner-anchor
  fixed512 direct construction of `Binary768`. The remaining templated conversions
  in this file receive the project allocator-backed `Binary<512>` type, not the
  failing fixed `paper_full_test::Real` backend.
- `tests/paper_endpoint_diagnostics.h`:
  `a1e580cc86ad4e6d4778e5c015a95f8d76f7da18b0b6872cc7802f9cb64a7ebd`
- `tests/paper_endpoint_diagnostics.cpp`:
  `2a05653222d00dbe7a4c62a393256a20e13435e1585d2318e1d3c4060b36f669`
- `tests/paper_endpoint_diagnostics_test.h`:
  `9a1f3aac18f006a748145ccc9694c8a4a856de90a611a31a93522aeff4fd050b`

## Hosted verification observed 2026-09-06

The source-only status above is the retained pre-push checkpoint, not current
execution status. Exact source `2af8745b8f58898c10570110cf550200f108965c`
completed run `34008596955`, attempt 1, successfully on both hosts.

- Linux job `101420249949`: paper target build and synthetic observer self-test
  PASS. Raw self-test marker time `2026-09-06T03:24:13.6382854Z`.
- Windows job `101420249963`: paper target build and synthetic observer self-test
  PASS. Raw self-test marker time `2026-09-06T03:25:32.8082211Z` (CR retained).
- Each host recorded groups `[1,2,57,1,2,60]`, 123 passing test invocations and
  the same 60 unique old test names; five explicit API build steps succeeded.
- Each self-test marker states `namespace=synthetic chain_count=0`; the normal
  paper CTest was skipped by the exact draft-ref gate. No new E80 observation.

`DUAL_HOST_RESULT.json` contains derived identities/counts; adjacent
`LINUX_JOB_LOG.lossless.json` and `WINDOWS_JOB_LOG.lossless.json` retain the full
raw tool results. The actual compile blocker is resolved for this exact source.
The unintegrated writer and the later Python gzip commit are not covered by
this run. Original E80 FAIL and project incomplete remain unchanged.
