# Relin2 Codex fallback null-first-key-red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **public-API-reachable present-vector/null-first-evaluation-key red
accepted; production remains byte-identical to the preceding empty-vector
green**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `66d28156a698df78ecdfdd71ce8ddeb513308a7a`;
- parent/tree: `342686ae1badbcfd700eff0ec473cac4168e491a` /
  `e7dda469c03a325f7af87c2636ff164b68f03fa3`;
- run/job: `33562346015` / Linux `100037552108`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33562346015`.

Only CMake and the Relin2 test changed. The eleventh public fixture reaches an
otherwise-valid Tensor and directly inserts one matching cache row whose vector
contains exactly one null `EvalKey<DCRTPoly>` shared pointer. It uses no
`EvalMultKeyGen`, cache clear, throwing key lookup, `operator[]`, key
dereference, or Relin2 arithmetic. RAII restores the global map and the paired
green executes the immediate Tensor/cache/deep-metadata invariance assertions.
Linux warning-clean build and API compilation succeeded; inherited tests passed
`10/10`; only the new test failed, giving `10/11` and CTest exit `8`. It
received the exact old `DoubleCKKS: Relin2 is not implemented` exception rather
than the requested null-first-key `std::invalid_argument`. Three read-only
reviews returned `PASS`. Windows was cancelled during pristine OpenFHE build
and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-66d2815` /
  `1157986902a09496cd51097b72e9a52c03c9d4f1` /
  `70aecb6673d1374d32b7be46fe12e8721f111ab9`;
- 19-entry manifest SHA-256:
  `f7d1690b05caf01392727e2c77e372ab5d9f71e5ca5c2d461150bd5758c7a135`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `a7c8c0c57b6739940ae5053342dbb3de8eb77292884e9637d387f6ebc71acc4f` /
  `6b2f6067a2b27afe8e936951c202f32ce3bb3b72edb81c073533fff6707ac543` /
  `4095a1c1a2039475991e0ac1529672ecfda330b1485d211cf3e6d107a051998b`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
