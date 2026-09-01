# Relin2 Codex fallback wrong-actual-key-tag red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **bound-context, nonnull-first-key/wrong-actual-tag red accepted;
production remains byte-identical to the preceding wrong-context green**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `82c9fa972e528a181a38fda4f3d71b9210547357`;
- parent/tree: `ba4ca7b4c93fc4d750618418aeeaf1db6b1dd32d` /
  `7185316cadd532dc358fea24f8ea2be9d8c321e6`;
- run/job: `33567295705` / Linux `100053280505`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33567295705`.

Only CMake and the Relin2 test changed. The thirteenth fixture uses the bound
context to generate one real, nonnull relinearization key under the Tensor-tag
map row, then changes only the key pointee's public actual tag. Context,
concrete subtype, and generated A/B material remain valid. The cache guard
exists before key generation and restores the initially empty global map on
every exit. Linux warning-clean build and API compilation succeeded; inherited
tests passed `12/12`; only the new test failed, giving `12/13` and CTest exit
`8`. It received the exact old `DoubleCKKS: Relin2 is not implemented`
exception rather than the requested wrong-tag `std::invalid_argument`. Three
read-only reviews returned `PASS`. Windows was cancelled while installing the
official toolchain and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-82c9fa9` /
  `ca48c70798b4b192154081fd1455ed2a2d43d29d` /
  `a1ab3e7f039234a2470b042303dc53a8dcf3393b`;
- 19-entry manifest SHA-256:
  `6465e7339643ee5c7f21cb6c5092cbdbe9638b5a6e6ffcf9dd8a6d9205ca1273`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `12a4cc2acc90e0b23b388b0efe6a26cb14e8f4ce2838c0658798e6babc6c0d99` /
  `6ff2051592b126606c9d56acbf0682d2031e80e6f84a71e1c96a5f2d48ba0c1d` /
  `d70786d0df9f73b3c69f3d37f0ef66d4dd10a67272e6453cd732af8593cc9f51`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
