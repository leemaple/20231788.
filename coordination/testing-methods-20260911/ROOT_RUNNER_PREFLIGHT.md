# Root runner preflight — no experiment dispatched

2026-09-11, Asia/Shanghai. Reviewed against runtime base
`33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1` while the independently briefed
browser Pro task was still thinking. This is a preparation record, not a test
result or adoption of a test candidate.

## Execution scope

- The new documentation branch does not match the push allow-list in
  `.github/workflows/dcp-rcb.yml`. Its pushes therefore do not launch that suite.
- Do **not** dispatch that legacy workflow blindly on the new branch. Its
  fresh-diagnostic build/run steps are restricted to two historical branch
  names; a different branch skips those steps and can instead run the unrelated
  full eight-square test. A green workflow could consequently miss the intended
  diagnostic entirely.
- Select one reviewed target only after the Pro return and independent review.
  A narrowly gated runner path is required. Verify the exact CTest registration
  and selected count before execution; use `--no-tests=error`, preserve exit
  codes through logging, and do not convert a timeout/build failure into a
  numerical mutation kill.
- The existing opt-in fresh target is
  `s100_fresh_error_diagnostic_test`, enabled by
  `OPENFHE_2023_1788_ENABLE_S100_FRESH_ERROR_DIAGNOSTIC`. It registers
  `s100_encoding_inspection_contract` and `s100_fresh_error_diagnostic`.
  Those are possible seams, not a decision to run both.

## Build identity and load

The official source must remain
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Record the actual clean source,
submodule revisions, compiler/Boost versions, configuration and linked library
identity. Require native 64-bit/backend 4, OpenMP enabled, and reduced-noise
disabled. Do not infer the latter from a cache key that omits it.

The legacy install cache can skip compilation and does not by itself establish
the build flags of its contents. Prefer the already demonstrated fresh-source
provenance pattern in `public-s100-encoder-cap.yml` for a new diagnostic unless
a cache's complete configuration provenance is independently established.
Use two remote build workers and `OMP_NUM_THREADS=2`. No Mac compile, FFT/NTT,
sampler, encryption, or eight-square run is authorised by this preflight.

## Candidate review questions

1. A hard coefficient envelope on fresh `p-m` would test a real initialization
   premise, but the adopted `39*(32768+128+1)=1,282,983` envelope is deliberately
   conservative. Passing it does not show typical noise, E80 precision, or
   equivalence to the historical failing draw. A copied out-of-bound vector
   tests the assertion; it is not a faulty production encryption.
2. A generated encoder roundtrip is valid only on the encoder's actual image.
   With partial packing, do not assume an arbitrary dense polynomial is legal.
   A half-boundary offset must survive conversion to the public input type;
   otherwise rejection may be correct, not a production bug. Independent
   coverage review is checking these fixture conditions before adoption.
3. A runtime baseline with a correct result plus a caught known-bad mutation
   establishes a useful test, not a historical defect. Keep original S100 FAIL,
   changed-profile S116 PASS, and changed-input annulus PASS separate.

No source, test, CMake, workflow, external agent state, or automation was changed
by this preflight. No production fix has been identified.
