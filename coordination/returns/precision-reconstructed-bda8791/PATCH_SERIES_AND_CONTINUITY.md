# Reconstructed patch series and contract continuity

## Reconstruction disclosure

The original proposed code and patch bytes were absent from the defective prior
delivery and were not present elsewhere in the supplied/current workspace. The
files below are **RECONSTRUCTED** replacements with new hashes. They are not
claimed byte-identical to any missing prior file.

Selected source baseline:

```text
bda879104c8a8b1ba6ac9301385b5b1919bef440
```

Pinned upstream:

```text
OpenFHE 1.5.0 / df495ba2e91739a6dc8f1de254fc5a41155ce504
```

## Patch records

| Patch | Bytes | SHA-256 | Resulting change |
|---|---:|---|---|
| `0001-red-freeze-dcp-rcb-high-precision-contract.patch` | 38,671 | `1306127c68b5701d7391d8eaabef2233769a01e62f5d9d9afd59670ec3aa7810` | Adds CMake registration, complete frozen contract/header, and explicitly incomplete binary64 fixture |
| `0002-green-replace-only-precision-fixture.patch` | 11,678 | `7ddb9921c7c516bb2153134742fd2ccbb3e1c7e29ea19a17232d598003d6442b` | Changes only `tests/precision_dcp_rcb_fixture.cpp` to the test-owned fresh-`2^100` public-DCRT adapter |
| `candidate-final.patch` | 47,846 | `5626d938bb6d979057f73be7ab698f02c507ffb1d0ce9ea66eb9baec771f7bbe` | Aggregate baseline→green representation; alternative to the numbered sequence |

The table’s byte counts are checked again by the package manifest; the manifest
is authoritative if transport changes line-ending assumptions.

## Exact changed paths

Patch 0001 changes exactly:

```text
CMakeLists.txt
tests/precision_dcp_rcb_contract_test.cpp
tests/precision_dcp_rcb_fixture.cpp
tests/precision_dcp_rcb_fixture.h
```

Patch 0002 changes exactly:

```text
tests/precision_dcp_rcb_fixture.cpp
```

The aggregate patch changes exactly the same four final paths as patch 0001.
No file under `src/` or `include/openfhe_2023_1788/` changes.

## Frozen red/green hashes

These files are byte-identical after patch 0001 and after patch 0002:

```text
7daa3e85267dd1c52bc8c0ead56fc4cba58717968d6d66f9c2ab980a576d5cfc  CMakeLists.txt
137612719a57f36316ee4a89d5c971300524ac9c6924c1513b1e57baeefcef01  tests/precision_dcp_rcb_contract_test.cpp
4b7b1c4f2670f5dc93e8d28f1ad585a47bb9cf4b81130bb45200a3af82e6b554  tests/precision_dcp_rcb_fixture.h
```

The intentionally changing fixture hashes are:

```text
f47a2b2f0446a97b62e77f41c1a1b36d759e07c7eb1b003e2b2b842383e23017  RED tests/precision_dcp_rcb_fixture.cpp
09f1ff5c8f165a5296f3ad62e1bb9a8f41e3450fecaf371d5109c5d96b0f4907  GREEN tests/precision_dcp_rcb_fixture.cpp
```

## Unchanged acceptance contract

Both states compile the same test source and register the same name:

```text
precision_dcp_rcb_high_precision_contract
```

The following are frozen in patch 0001 and untouched by patch 0002:

- all 16 exact decimal/power-of-two source values;
- exact `(2^-70, 2^-73)` expected difference;
- `2^-80` delta and all-slot maximum-error assertions;
- four fresh-key trials;
- DCP and RCB public calls;
- N64, batch16, depth7, p50, first55, degree2, scale `2^100`;
- FIXEDMANUAL, HYBRID, COMPLEX, UNIFORM_TERNARY, HEStd_NotSet;
- level, scale, lifecycle, component, key-tag and ordered-basis assertions;
- actual 128-bit non-wrap headroom;
- independent secret/CRT recovery and direct canonical evaluator;
- constant, `X^32`, and hard-coded ordered `X^2` witnesses;
- explicit binary64 collapse and standard-encoding negative controls.

The future red diagnoses the incomplete fixture because it removes the sub-ULP
source information before Encrypt. It is not an upstream encoder, DCP, or RCB
defect finding. The proposed green changes only how the same lossless values are
inserted into the test plaintext.

## Mechanical verification result

`tools/verify_contract_continuity.py` was run against the exact supplied
30-file baseline and returned:

```text
RED_PATCH_APPLY=PASS
GREEN_PATCH_APPLY=PASS
AGGREGATE_PATCH_APPLY=PASS
GREEN_CHANGED_FROM_RED=tests/precision_dcp_rcb_fixture.cpp
AGGREGATE_EQUALS_SEQUENTIAL_GREEN=PASS
FINAL_COPIES_EQUAL_SEQUENTIAL_GREEN=PASS
CONTRACT_CONTINUITY=PASS
```

Full output is in `evidence/PATCH_APPLY_AND_CONTRACT_CHECK.txt`. This is static
patch/source evidence only, not OpenFHE compilation or precision execution.
