# First-Mult2 high-precision candidate — c9ee28d

## Delivery status

This is a complete **test-only proposal**, not an integrated or hosted result.

- Precursor verdict: **PASS with bounded claim**; P0=0, P1=0, P2=4 nonblocking claim-boundary risks.
- New candidate: one additive CTest at public `DCP → Mult2 → RCB` behavior.
- Production changes: **none**.
- Existing accepted tests changed: **none**.
- Existing workflow changed: **none**.
- OpenFHE candidate build/runtime in this environment: **NOT EXECUTED**.

## Apply

From the exact supplied project root at `c9ee28d0370eeee1ec7a1965402ed0b5e91f425e`:

```bash
git apply --check /path/to/patches/0001-add-first-mult2-high-precision-contract.patch
git apply /path/to/patches/0001-add-first-mult2-high-precision-contract.patch
```

The patch changes only:

```text
CMakeLists.txt
tests/precision_first_mult2_contract_test.cpp
```

Patch SHA-256:

```text
acf043d3a04876e71ddbf17c14f8f579c1a7cb86a032d881243e27e010b6a105
```

## Frozen contract

```text
test:       precision_first_mult2_high_precision_contract
context:    N64, batch16, depth7, p50, first55, FIXEDMANUAL,
            HYBRID, COMPLEX, UNIFORM_TERNARY, HEStd_NotSet
input:      exact test-owned 2^100 DCRT embedding
output:     exact logical scale 2^200/(q_div*q_l)
trials:     four fresh unseeded keys per host
acceptance: all 16 complex slot errors <=2^-80 and
            designated sub-binary64 product-delta error <=2^-80
lifecycle:  RefreshRequired
```

Canonical contract bytes are in `evidence/FROZEN_CONTRACT.json`, SHA-256 `7d6b7f5e1c820cf49641dc1606fa64984a52267e2f04305c2ad5bd5981d2036b`.

## Read order

1. `PRECURSOR_REVIEW.md`
2. `FIRST_MULT2_TEST_DESIGN.md`
3. `SOURCE_CLAIM_TEST_LEDGER.md`
4. `HOSTED_COMMANDS_AND_EXPECTED_REGISTRATION.md`
5. `NEXT_PAPER_PRECISION_GATES.md`
6. `EXECUTION_LEDGER.md`

## Evidence and code

- `patches/` — applicable candidate patch.
- `final-changed-files/project/` — complete final copies of every changed/new file.
- `evidence/` — input integrity, retained precursor audit, CTest continuity, patch replay, source guards, contract bytes, hashes, and non-cryptographic arithmetic output.
- `tools/` — source-only standalone arithmetic and delivery verifier. No binaries or caches are included.

## Claim boundary

A future hosted pass is a first-observed green for one N=64 HYBRID first-Mult2 diagnostic. It is not evidence for repeated multiplication, refresh, production lossless I/O, Table 3, security, performance, or a universal no-wrap/relinearization theorem. A hosted failure must retain the frozen threshold, vector, expected table, and exact scale normalization.
