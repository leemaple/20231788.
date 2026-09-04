# Finite hosted experiment matrix

## Common rules

- Upstream: pristine OpenFHE 1.5.0, commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Project baseline: exact selected files from
  `bda879104c8a8b1ba6ac9301385b5b1919bef440`.
- Hosts: GitHub Actions Linux GCC and Windows MinGW64.
- Build: Debug, `-Wall -Wextra -Wpedantic -Werror`, maximum two threads.
- Host vectors: literal, deterministic, no host RNG.
- Crypto keys/ciphertexts: fresh and explicitly unseeded.
- No tolerance edits after any red or green result is observed.
- Preserve raw focused and full-suite logs, toolchain versions, source hashes and
  exact patch hashes.

## Matrix

| ID | State / operation | Parameters and repetitions | Independent expected value | Frozen acceptance | Valid claim |
|---|---|---|---|---|---|
| S0 | Static continuity verifier | Exact baseline; apply 0001 then 0002 | SHA-256 and file-set comparison | Green changes only fixture; CMake/header/contract hashes identical | Patch continuity only |
| S1 | Standalone arithmetic prerequisite | N=64, M=128, 16 literal slots, scale `2^100`; one deterministic run | Direct canonical evaluation and monomial table | binary64 collapse true; fresh-rounding max/delta errors ≤`2^-80` | Arithmetic construction is internally consistent; not OpenFHE evidence |
| H1 | Hosted RED focused DCP→RCB | Patch 0001; p50, first55, depth7, COMPLEX, HYBRID; 4 fresh-key trials in one CTest; each host | Exact decimal/power-of-two values through independent secret/CRT/direct evaluation | Same two positive assertions as H2; expected to fail | Incomplete binary64 fixture cannot supply frozen contract; no DCP defect claim |
| H2 | Hosted GREEN focused DCP→RCB | Apply 0002 only; otherwise byte-identical to H1; 4 fresh-key trials; each host | Same oracle and expected values as H1 | delta `(2^-70,2^-73)` error ≤`2^-80`; all-slot max error ≤`2^-80`; ≥128-bit actual non-wrap headroom | Existing public DCP→RCB carries the tested high-precision plaintext under this diagnostic context |
| H3 | Hosted complete suite after H2 | Exact green tree; each host | Existing test contracts plus H2 | Report every result; do not suppress two inherited BV certificate failures | Compatibility accounting only; no blanket green while inherited failures remain |
| F1 | Later first-Mult2 gate — not in patches | Separate approved task; begin only after H2 and BV resolution | Reuse lossless source and independent coefficient/canonical oracle; expected products formed in multiprecision | Pre-frozen operation-specific threshold and non-wrap state | First Mult2 precision only, if executed and passed |
| F2 | Later refresh/repeated lifecycle — not in patches | Separate API/parameter/lifecycle decision; ordered 40/60 regime if chosen | Independent per-stage oracle and scale ledger | Pre-registered stage thresholds, wrap and lifecycle gates | Repeated Double-CKKS evidence only, if executed and passed |

## Exact H1/H2 vector and contract

The 16-slot vector is frozen in
`tests/precision_dcp_rcb_contract_test.cpp` and reproduced in
`REVISED_TEST_DESIGN.md`. H1 and H2 use exactly the same compiled test source.
Only `tests/precision_dcp_rcb_fixture.cpp` differs.

## Repeat interpretation

Four fresh-key trials per host are a deterministic-contract robustness check,
not a statistical precision distribution or security experiment. A single
failure invalidates the hosted green for that host. Four successes do not imply
a universal theorem.

## Invalid and non-wrap boundaries

The test rejects before the precision claim when any of these is false:

- ring dimension, slot count or scale degree differs from the frozen context;
- DCP does not remove exactly the final tower;
- pair and RCB basis order differ;
- recorded/logical scale metadata differs from exact `2^100`;
- output is not level 1 with two components;
- independently recovered maximum coefficient leaves less than 128 bits of
  centered headroom below the active modulus.

These are separate from canonical-slot accuracy. Passing one category must not
be reported as passing another.

## Result reporting template

For each host/state, record:

```text
source baseline SHA / patch SHA / resulting tree
compiler + CMake + OpenFHE commit
focused CTest exit and complete log
per-trial active modulus bits, q_div, delta_error, max_slot_error
state/basis/non-wrap assertion status
full-suite counts, with inherited BV failures named separately
claim made / claim explicitly withheld
```
