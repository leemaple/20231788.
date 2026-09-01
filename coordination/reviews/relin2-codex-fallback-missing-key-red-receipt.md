# Relin2 Codex fallback missing-key-red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **public-API-reachable missing-evaluation-key red accepted; production
still ends at the not-implemented Relin2 seam after the two prior validations**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `b0196dd5349e94a0df0cd0f0750fa7f6819a3498`;
- parent/tree: `791f634c7e29c9a4b9e465d7a092e94eb429a7ab` /
  `5dd4c479e77e60cee399befc89c5aca06271cf2d`;
- run/job: `33557301667` / Linux `100021153304`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33557301667`.

Only CMake and the Relin2 test changed. The ninth public fixture proves exact
four-to-three tower reachability, degree three, and an empty unchanged eval-key
cache without calling `EvalMultKeyGen` or clearing cache state. The warning-clean
build and API target succeeded; inherited tests passed `8/8`; only the new test
failed, giving `8/9` and CTest exit `8`. It received the exact old
`DoubleCKKS: Relin2 is not implemented` exception instead of the requested
stable missing-key `std::invalid_argument`. Three read-only reviews returned
`PASS`. Windows was cancelled during toolchain setup and makes no test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-b0196dd` /
  `8c1cf1e429264f2309779ad6e0246063a03f69a8` /
  `dcbcb6d8d926c1302114d86f44c32da03a83cf04`;
- 19-entry manifest SHA-256:
  `d2b53ee7020f3761c14a8183dd968bc354f10f21b809a2aec61d68631003b28c`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `628685152a9acb262d5004dcdeb5cbb2d6896d9b5f2edf4b7dcd5c9c7130ea5b` /
  `e14afd6b2f1ed675cdc52fed0dbb8357b0d43a8dce46ded8da01baebc783c31b` /
  `0704da847974baac9dba1aef8a6adf009773d3abb9075a51f3c81d8302f194d5`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
