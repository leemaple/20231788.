# Relin2 Codex fallback API-scaffold coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **minimal API scaffold green accepted; Relin2 algorithm remains
unimplemented**.

## Implementation boundary

- branch: `agent/codex-relin2-01`;
- commit: `6f1645b97ce5b2175530cde5bfd0929370997634`;
- parent: `557d2a331658a2cf16d47de36415c2d968e62b5f`;
- tree: `353a6f551deb186e6a01bb2fa6348b7ebb0021b6`;
- local HEAD, upstream, and remote branch SHA were all verified equal to the
  commit above after a non-force push.

The commit changes only `include/openfhe_2023_1788/double_ckks.h` and
`src/double_ckks.cpp`, adding eight lines. It appends
`PairLifecycle::ReadyForRS2`, appends the exact `long double` recombined scale
field, declares the public const `Relin2` seam, initializes the new DCP field
to the fresh recombined recorded scale, and adds only the exact immediate-throw
scaffold. It adds no Relin2 validation, key access, arithmetic, runtime test,
CMake change, or workflow change. Spec, TDD, and Delivery/CI read-only reviews
all returned `PASS` before commit.

## Hosted observation

- workflow run: `33529249978`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33529249978`;
- exact head SHA: `6f1645b97ce5b2175530cde5bfd0929370997634`;
- Linux job `99927872443`: terminal `success` on Ubuntu 24.04.4, CMake
  3.31.6, and GCC 13.3.0;
- exact pristine OpenFHE source:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- warning-clean default build succeeded;
- compile-only `relin2_api_contract_test` compiled and linked;
- the six inherited DCP/RCB/Tensor2 CTests passed exactly `6/6` in 0.12
  seconds; no runtime Relin2 test existed, so this is not Relin2 algorithm
  green;
- after the complete Linux job log was downloaded, the non-final whole run
  was cancelled once; Windows job `99927872804` is terminal `cancelled` during
  pristine OpenFHE build and supplies no project-build, API-contract, or test
  claim.

## Remote evidence binding

- evidence branch: `evidence/relin2-hosted-6f1645b`;
- evidence commit/local upstream/remote SHA:
  `ddc15d436723e3bdca536b36b590a699d7499098`;
- evidence tree: `95efc57eb0192883a37665c4658f0554589776f9`;
- `MANIFEST.sha256`: 23 entries, SHA-256
  `5a99235127f1642bf17ebfd48cb9930250301adf6cae6bea83fedea120e9e8e9`;
- evidence-side `RECEIPT.md` SHA-256:
  `ea200d580cba8073218f6f126f6ea5975284c97a229b2108bec6aae66c57db60`;
- terminal complete-logs ZIP SHA-256:
  `49f0bce3490bdbbb1def9605b25e5163a6068272c9cf2694f4450560670a5f57`;
- Gitleaks 8.30.1 and the targeted credential scan reported no findings;
- the terminal complete-logs ZIP passed `unzip -t`; the artifacts API reported
  exactly zero uploaded artifacts.

The evidence branch contains raw records and its manifest but does not bind
its own commit SHA. This coordination-side receipt supplies that non-circular
remote binding.

## Next authorized boundary

The next implementation work must begin with one independently observable
runtime Relin2 contract red against the current immediate-throw scaffold. Only
the minimum production change needed to make that behavior green may follow.
Core arithmetic, later validation branches, and later lifecycle behavior must
not be implemented ahead of their own red tests.
