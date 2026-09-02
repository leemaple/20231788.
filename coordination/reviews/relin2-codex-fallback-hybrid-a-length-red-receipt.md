# Relin2 Codex fallback HYBRID A-vector-length red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **valid HYBRID relinearization key with only its A-vector length
malformed red accepted; production remains byte-identical to the preceding
wrong-subtype green**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `b50448f561ba62f9a6271779f5c1eeb4903cac4e`;
- parent/previous accepted green:
  `331dd7d64d53e6a87d94555a00aab29b1eaa998e`;
- tree: `1732a4bc99676723732b47507549390f9669a0b8`;
- run/job: `33572538385` / Linux `100069389984`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33572538385`.

Only CMake and `tests/relin2_test.cpp` changed. The fifteenth test proves the
bound configuration is HYBRID with exact `GetNumPartQ()==2`, generates one real
`EvalKeyRelinImpl` whose A and B vectors both start at length two, then copies
and shortens only A to length one. B, context, actual tag, subtype, cache row,
and pointer remain valid. All production-call snapshots are taken after the
mutation. A test-owned RAII guard exists before key generation and restores the
initially empty cache on every exit. Immediate Tensor/deep-metadata and complete
cache/vector/key/A/B invariance checks follow the diagnostic helper.

Hosted Linux built warning-clean, compiled the public API contract, and
reported exact `14/15` in 0.15 seconds: inherited tests 1 through 14 passed and
only `relin2_key_hybrid_a_length` failed because it observed the old
not-implemented scaffold. The red stops inside the diagnostic helper, so no
later negative-case postcheck is claimed at this checkpoint. The inherited
real-key positive control first accepted that same scaffold and remained green,
preventing an unconditional HYBRID-key rejection from being accepted. Three
read-only reviews returned `PASS`. Windows was cancelled during official
toolchain installation and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-b50448f` /
  `2a3aacf532096a7f53d644c0b65bbd95f21221c7` /
  `608cbb98d7fadb2ecae92489bbd81e8ae1f2fc2a`;
- 19-entry manifest SHA-256:
  `9a44f7105691c0c8b6009f7d0cdd279bc9a6b899e0f3d8dbd1bd0aa4ed96a8fa`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `03b1d138864bae981fb8b323535c9744863f872182b7aab531681bc36c68cb43` /
  `3413aeb5b4be671cd996d714250560808c2bf57fbb2657dc3fe92bbf0472f68e` /
  `9f530d6aaddbc4d11c3de221078637db43833a125a199860cd86df84797e22c0`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
