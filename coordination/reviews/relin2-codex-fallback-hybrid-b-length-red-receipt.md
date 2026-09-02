# Relin2 Codex fallback HYBRID B-vector-length red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **valid HYBRID relinearization key with only its B-vector length
malformed red accepted; production remains byte-identical to the preceding
HYBRID-A green**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `0efab27ea1e2946db94b8621be53c2ecaaa282e8`;
- parent/previous accepted green:
  `3645c4e02d3fb1eca4496b1de9be1ebb5517bac9`;
- tree: `c5b446c13357231b61faa1ec2539444c8f4404ef`;
- run/job: `33574363738` / Linux `100074968553`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33574363738`.

Only CMake and `tests/relin2_test.cpp` changed. The sixteenth test proves the
bound configuration is HYBRID with exact `GetNumPartQ()==2`, generates one real
`EvalKeyRelinImpl` whose A and B vectors both start at length two, then copies
and shortens only B to length one. A, context, actual tag, subtype, cache row,
and pointer remain valid. All production-call snapshots are taken after the
mutation. A test-owned RAII guard exists before key generation and restores the
initially empty cache on every exit. Immediate Tensor/deep-metadata and complete
cache/vector/key/A/B invariance checks follow the diagnostic helper.

Hosted Linux built warning-clean, compiled the public API contract, and
reported exact `15/16` in 0.15 seconds: inherited tests 1 through 15 passed and
only `relin2_key_hybrid_b_length` failed because it observed the old
not-implemented scaffold. The red stops inside the diagnostic helper, so no
later negative-case postcheck is claimed. The inherited real-key positive
control accepted the same scaffold and remained green, while the inherited
malformed-A case continued to receive its accepted earlier diagnostic. Three
read-only reviews returned `PASS`. Windows completed toolchain installation
and was cancelled during provenance verification; it makes no OpenFHE build,
project-build, API-contract, or test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-0efab27` /
  `cb12d476fefc749b1f262c11ae174bac7b697a8b` /
  `d999b7557d887ce834cca12e8bb7f8d00886e2d1`;
- 19-entry manifest SHA-256:
  `6e1a0092fffa347d3ca40aaa2fd7492e3fb5b286cde0cae824558c1ce8a46575`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `4cc623b2437028e65e91e3fd667db1578aa1c1b49b58329b8517b79890caaa60` /
  `b896ebdf2e5d686df94fac53260b75ca11c4be6d3c5f4ab64e89f1e4f584ceec` /
  `0a88acfc8719f646550b4f5a347d8c4d5041f6b4a4e5ed7146e5cc13b39c614e`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
