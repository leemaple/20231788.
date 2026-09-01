# Relin2 Codex fallback wrong-context-key-red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **public-API-reachable nonnull-first-key/wrong-context red accepted;
production remains byte-identical to the preceding null-first-key green**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `0a8f84039e61c36548987df2b68153754c519442`;
- parent/tree: `37d1758b515a77e3a6f880f182462e223b2d2bd5` /
  `9402cc6b28a7f1f3f342444749e890f9a5d850bc`;
- run/job: `33564908322` / Linux `100045745583`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33564908322`.

Only CMake and the Relin2 test changed. The twelfth fixture creates a distinct
public OpenFHE context with identical element parameters, generates one real
nonnull relinearization key there, and sets its actual tag to the Tensor tag
before generation. Thus the sole validation-prefix mismatch is key context.
The cache guard exists before key generation and restores the initially empty
global map on every exit. Linux warning-clean build and API compilation
succeeded; inherited tests passed `11/11`; only the new test failed, giving
`11/12` and CTest exit `8`. It received the exact old
`DoubleCKKS: Relin2 is not implemented` exception rather than the requested
wrong-context `std::invalid_argument`. Three read-only reviews returned
`PASS`. Windows was cancelled during pristine OpenFHE build and makes no
project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-0a8f840` /
  `58e41401b02a97ffb883da7c5c23d0c5c7917c9f` /
  `e003965fae1902cb4730b9cf46c12b891fbdccbe`;
- 19-entry manifest SHA-256:
  `47819b18cc89c10441d32a19beaccbcc654c50c05cab8a7a5988f5bc2ec98fe3`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `04baabfcc0119364b0d58a27d1247ee18f24e22cf64e000b5d210268401f5331` /
  `04ac31c96ce45c19571c41f7fea6f88430e033ed134b0734cb9fa519fb2b87d7` /
  `8b2532f32644406565aa15eee32106eafc5356434c3400e9074d44468f804740`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans.
