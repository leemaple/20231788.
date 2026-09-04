# Pair composition — independent Spec/oracle audit

Source: `codex/pair-mult2-composition-01`, initially clean; frozen
`09d5c722a1089937edc16ed3e5311119ca347949` against
`8a465764044d8b1e1578f462ea4916f7123428a4` (`git diff baseline...HEAD`).
Commits: `cdc4711`, `2b4bfa8`, `2661ae3`, `b48b54e`, `09d5c72`.
Hosted source: `b48b54e22f14bbfe988a6890f1b03eac9efb11a3`;
[run 33871723090](https://github.com/leemaple/20231788./actions/runs/33871723090),
Linux `101019032191`, Windows `101019032537`.

Here, **S** means `coordination/tasks/chatgpt-pro-pair-mult2-composition-d73824c.md`;
**T** means `tests/mult2_e2e_oracle_test.cpp`.

## Findings

No actionable missing, incorrect, or unrequested functional behavior found in
this bounded test-only slice.

One documented integration scope exception: S:59 says “Do not change any
production/header, workflow, upstream or other test file.” The active workflow
adds branch admission and focused runs at `.github/workflows/dcp-rcb.yml:13`,
`:98`, `:242`. These are Codex-owned hosted-verification additions, explicitly
separated from the unchanged Pro candidate in
`coordination/PAIR_MULT2_COMPOSITION_RETURN_AND_HOSTED.md:79` and `:171`.
They preserve the original full-suite/API steps; no production/header change
or functional scope expansion was found.

## Observed checks

- S:104–116: T:222–305 reconstructs exact secret/CRT coefficients;
  T:1193–1228 materializes centered BigInt sum/subtraction of ORIGINAL A/B
  recombinations captured before composition (T:1416). No result-derived
  expectation. T:1438–1452 separates staged/direct wiring from the independent
  integer negacyclic, high/low Relin2 and public-RCB literal checks
  (T:397–476, :801–1024).
- S:72–85: independent bounded BigInt arithmetic on a common `2^48` denominator
  verified all 112 C++/JSON scalar literals and 32 complex identities.
  Maximum L1 envelopes: A `3/8`, B `3/16`, C `3/4`, sum `7/32`, difference `9/16`.
  Slot-zero products distinguish Add/Sub and dropping B well beyond `1e-3`.
- S:95–124: fresh key generation and three COMPLEX imaginary witnesses
  (T:1369–1395); state/basis/level/scale checks (T:1293–1346); all requested
  snapshots and key-row presence (T:1406–1470). Coverage is enumerated values,
  not deep key/cache/parameter immutability.
- CMake retains all 53 old bindings plus exactly two additions (:188–191).
  Active test/CMake equal Pro final bytes; active source/workflow equal hosted
  source. Retained `add-sub-{linux,windows}.txt` independently contain focused
  2/2 and full 55/55 matching actual names. All eight certificates satisfy
  their printed integer non-wrap, per-path and coefficient inequalities;
  maximum logical error is approximately `8.527e-10`.

## Boundaries

This audit executed only read-only/static checks, not OpenFHE or CTest.
T:49 retains `1e-3`; T:716 requires `RefreshRequired`; existing terminal
rejections remain unchanged (`tests/rs2_test.cpp:912`). Conditional N64
functional evidence is not high-precision, paper-parameter, all-key/BV theorem,
security, repeated-multiplication or full-project acceptance.
