# Precision TDD continuity follow-up

## Result

**AMENDMENT COMPLETE — proposed two-patch hosted red/green sequence.**

The prior sequence did not preserve one public-behavior test across red and
green. This package corrects that defect:

- one exact CTest name;
- one complete contract source, byte-identical across red and green;
- one red fixture that is explicitly incomplete rather than accusing standard
  `complex<double>` encoding of a defect;
- one green patch that changes only that fixture implementation;
- one independent coefficient/canonical oracle with direct evaluation and
  worked monomial witnesses;
- no production source/header/API/parameter/security/lifecycle change.

Candidate OpenFHE build, crypto execution, CTest and CI remain **NOT EXECUTED** in
this environment. The return is code-drafting and source-backed test design for
Codex-hosted integration, not an observed green.

## Package index

| File / directory | Purpose |
|---|---|
| `REVISED_TEST_DESIGN.md` | Exact seam, vectors, frozen threshold, red/green meaning, state/scale and scope |
| `ORACLE_AND_CACHE_REVIEW.md` | Independent oracle review, worked witnesses, stale cache and rounding analysis |
| `HOSTED_EXPERIMENT_MATRIX.md` | Finite red/green/full-suite progression and later gates |
| `PATCH_SERIES_AND_CONTINUITY.md` | Patch and contract hashes, exact changed paths and TDD interpretation |
| `SOURCE_CLAIM_TEST_LEDGER.md` | Source-backed claim limits and pending hosted evidence |
| `COMMANDS_AND_EXECUTION_LEDGER.md` | Exact Linux/Windows commands and executed/not-executed ledger |
| `RISK_AND_DECISION_TABLE.md` | Proven, pending and deferred decisions with falsifying checks |
| `patches/0001-...patch` | Frozen-contract red |
| `patches/0002-...patch` | Green fixture-only implementation |
| `patches/candidate-final.patch` | Aggregate convenience patch |
| `final-changed-files/` | Proposed final copies of all changed files |
| `tools/verify_contract_continuity.py` | Reapply and mechanically check red/green continuity |
| `tools/standalone_precision_contract_math.cpp` | Boost-only arithmetic prerequisite, not crypto evidence |
| `evidence/` | Input hashes, patch checks, source hashes, environment probe and standalone output |
| `SOURCE-AND-DELIVERY-RECORD.json` | Machine-readable source, patch, contract and execution binding |
| `OUTPUT_TREE.txt` | Complete delivered member list |
| `MANIFEST.sha256` | Hashes for every package member except the manifest itself |

## Minimal integration order

```bash
# From the exact supplied project baseline:
git apply --check patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
git apply patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
# Observe and retain hosted focused red.

git apply --check patches/0002-green-replace-only-precision-fixture.patch
git apply patches/0002-green-replace-only-precision-fixture.patch
# Verify continuity, then observe hosted focused green or retain the failure.
```

Do not apply `candidate-final.patch` after the numbered patches; it is an
alternative aggregate representation.

## Bounded claim after a future hosted green

The strongest allowed claim is that the existing public DCP→RCB path preserved
the selected losslessly supplied 16 complex values, including sub-binary64
differences, within the frozen `2^-80` diagnostic gate for the named context and
fresh-key trials on the executed host.

It is not evidence for a production codec, Mult2 precision, repeated
multiplication, refresh, Table 3, security or performance.
