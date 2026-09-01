# Relin2 Codex fallback empty-key-vector-red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **public-API-reachable present-but-empty evaluation-key-vector red
accepted; production remains byte-identical to the preceding missing-key
green**.

## Implementation and hosted boundary

- implementation branch/final red commit: `agent/codex-relin2-01` /
  `e976626ebabf0aa40858ee97c830611e9c47c5f6`;
- parent/test-introduction commit:
  `f165365a48007ca5e09f8d83df627bbbdf978d46`;
- final red tree: `50a681d95ff12c7f5a918cfb6a81eaa2ae17fd40`;
- previous accepted green: `7c0e94de99c0d6a966f73d78bf50b8c590cfce8c`;
- run/job: `33560196603` / Linux `100030571326`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33560196603`.

Only CMake and the Relin2 test changed from the previous green. The tenth
public fixture reaches an otherwise-valid Tensor with three active towers,
degree three, a matching nonempty tag, and exactly one empty evaluation-key
vector. It uses no `EvalMultKeyGen`, cache clear, throwing key lookup,
`operator[]`, key dereference, or Relin2 arithmetic. RAII restores the global
map; Tensor and cache post-call assertions include outer metadata-map identity,
entry-pointer identity, and independent metadata-value clones.

The initial `f165365` checkpoint was an unintended compile red because the new
snapshot used `lbcrypto::Format`; its complete Linux failure record is retained
and is not claimed as the behavioral red. Commit `e976626` changed only that
type spelling to global `Format`. Its Linux warning-clean build and API target
succeeded; inherited tests passed `9/9`; only the new test failed, giving
`9/10` and CTest exit `8`. The exact observation was the old
`DoubleCKKS: Relin2 is not implemented` exception instead of the requested
empty-vector `std::invalid_argument`. Three read-only reviews returned `PASS`.
Windows was cancelled during pristine OpenFHE build and makes no test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-e976626` /
  `72ae2a7b8c44a0f697296c15f4a5e99c89acacf9` /
  `3d036db2ade44923fa68fc691137e9b8e17fa072`;
- 22-entry manifest SHA-256:
  `6d532b4b2721f83c5880baf9b4d03ec483f11ad44ca676edaeb9e603808a1b2c`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `b046065435519d758e87aa6e23aced4a28765e829ab5dabd09f2f604b6de402f` /
  `1af5917f03afcd2af542c469199b510068a1ce03e3baf1aa2423a4a3caf87957` /
  `d63c8dc77ce02a87fdcc849ab68fef573ec56149bb1ad4a628f8723591aa4848`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
