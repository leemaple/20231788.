# Precision delivery repair — reconstructed complete candidate

## Status

**DELIVERY REPAIR COMPLETE — RECONSTRUCTED ARTIFACT, PROPOSED ONLY.**

The prior delivered ZIP was internally consistent but incomplete: it contained no
patches and no final changed source files. Those absent bytes were not recoverable
from the reattached sanitized return or the currently mounted prior artifacts.
This package therefore does **not** claim byte identity with the missing candidate.
It supplies newly drafted replacement bytes, explicitly marked `RECONSTRUCTED`,
from the exact selected project baseline
`bda879104c8a8b1ba6ac9301385b5b1919bef440` and pristine OpenFHE 1.5.0 contract
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The frozen public-behavior contract was not weakened:

- CTest: `precision_dcp_rcb_high_precision_contract`;
- public seam: Encrypt → existing `DCP` → existing `RCB`;
- 16 lossless multiprecision complex slots;
- expected delta `(2^-70, 2^-73)`;
- all-slot and delta tolerance `2^-80`;
- four fresh-key trials, crypto randomness deliberately unseeded;
- exact N64 / batch16 / depth7 / p50 / first55 / FIXEDMANUAL / HYBRID /
  COMPLEX / degree2 / recorded scale `2^100` context;
- level, scale, lifecycle, ordered-basis and actual 128-bit non-wrap checks;
- independent secret/CRT polynomial recovery and direct canonical evaluation;
- constant, `X^32`, and hard-coded `X^2` monomial witnesses.

No production source or public header is changed. Patch 0002 changes only
`tests/precision_dcp_rcb_fixture.cpp`.

## Required candidate files

The package includes and mechanically checks:

```text
patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
patches/0002-green-replace-only-precision-fixture.patch
patches/candidate-final.patch
final-changed-files/project/CMakeLists.txt
final-changed-files/project/tests/precision_dcp_rcb_contract_test.cpp
final-changed-files/project/tests/precision_dcp_rcb_fixture.h
final-changed-files/project/tests/precision_dcp_rcb_fixture.cpp
PATCH_SERIES_AND_CONTINUITY.md
SOURCE-AND-DELIVERY-RECORD.json
tools/verify_contract_continuity.py
tools/standalone_precision_contract_math.cpp
OUTPUT_TREE.txt
MANIFEST.sha256
```

Every required member is nonempty. `MANIFEST.sha256` covers every regular file
except itself. `MANIFEST.sizes-sha256.tsv` records size and SHA-256 for payload
members; `OUTPUT_TREE.txt` names the complete ZIP tree.

## What was reverified here

- outer and both complete nested archive identities and manifests;
- original 51-payload input closure and prior 25-entry return closure;
- exact 30-file selected project baseline hash set;
- red patch application on that exact baseline;
- green patch application after red;
- red-to-green changed path: only the fixture implementation;
- byte-identical CMake, fixture header, complete contract test, test name,
  vectors, thresholds and assertions across red and green;
- aggregate patch equals sequential red+green state;
- final changed-file copies equal the sequential green state;
- no packed-value getter, production `Decrypt`, matching forward-transform
  expected oracle, broad exception catch, public API, or production change;
- warning-clean Boost-only standalone arithmetic prerequisite;
- clean ZIP extraction followed by required-member and manifest closure checks.

Pristine OpenFHE was not installed in this review container. Candidate CMake
configuration stopped at `find_package(OpenFHE 1.5.0)`. OpenFHE compilation,
Encrypt/DCP/RCB execution, focused red, focused green, full CTest and CI are all
**NOT EXECUTED**.

## Integration order

From the exact selected project root:

```bash
# Alternative A: preserve the hosted red/green sequence.
git apply --check <delivery>/patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
git apply <delivery>/patches/0001-red-freeze-dcp-rcb-high-precision-contract.patch
# Build and retain the genuine hosted RED without editing the contract.

git apply --check <delivery>/patches/0002-green-replace-only-precision-fixture.patch
git apply <delivery>/patches/0002-green-replace-only-precision-fixture.patch
# Build and retain the hosted GREEN or the unchanged-contract failure.

# Alternative B: apply the aggregate patch to a fresh exact baseline.
git apply --check <delivery>/patches/candidate-final.patch
git apply <delivery>/patches/candidate-final.patch
```

Do not apply `candidate-final.patch` after the numbered patches.

## Claim boundary

A future two-host focused green would establish only that the existing public
DCP→RCB path preserved this losslessly supplied diagnostic vector within the
predeclared `2^-80` gate for four fresh-key trials on each executed host. It
would not establish a production codec, Mult2 precision, repeated use, refresh,
Table 3 reproduction, security, performance, or “100-bit slots.” The separately
reported current 53/53 functional suite is not precision evidence and was not
reproduced or imported here.
