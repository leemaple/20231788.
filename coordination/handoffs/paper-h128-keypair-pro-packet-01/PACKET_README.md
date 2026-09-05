# Handoff packet: fixed-Q h=128 client keypair implementation 01

Final replacement prepared 2026-09-05 08:09 Asia/Shanghai from clean branch
`codex/paper-h128-keypair-01` at
`9d21c3a5aea79c31745aca790712a9fd8c7743b2`.

This supersedes the unsent 07:55 draft. See PREFLIGHT_REVIEW.md for the
independent findings, added source and corrected acceptance wording.

## Archive identity

- Local archive:
  `/private/tmp/paper-h128-keypair-implementation-01-9d21c3a-20260905T0812.zip`
- Bytes: `1,185,384`
- SHA-256:
  `52f0dec88ac9ee854b2a863a60f382f35a6bf7117aada1b39d45637f8e367e8b`
- Sidecar:
  `/private/tmp/paper-h128-keypair-implementation-01-9d21c3a-20260905T0812.zip.sha256`
- One archive root:
  `paper-h128-keypair-pro-implementation-01-9d21c3a/`

The archive contains 98 regular files and no directory entries.
`MANIFEST.tsv` declares and closes over exactly 96 non-manifest files;
its SHA-256 is
`136c369d95820c4907cb004bbd222d2b33b30eecb3b8cd5ad426d08c1b3cd8d5`.
`MANIFEST.sha256` records the same digest. `TASK.md` is 13,316 bytes with
SHA-256 `6187380d3031a4f13681ecb1292fd71abb8e8557d9aa39fc65f0cfc7575c7fe0`
and is byte-identical to the committed task copy.

## Safety and integrity gates

- ZIP CRC: pass.
- Relative one-root names, no traversal/absolute/drive paths: pass.
- No duplicate or Unicode-NFC/case-fold-colliding names: pass.
- No encrypted, symlink or special-file entries: pass.
- Manifest path/byte/SHA/origin closure: 96/96 pass.
- Gitleaks 8.30.1 source-tree scan: no leaks.
- Gitleaks 8.30.1 final-archive scan, archive depth 5: no leaks.
- Targeted sensitive filename scan: zero hits.
- Targeted private-key/API-token pattern scan: zero hits.

The packet contains no `.git`, dependency tree, build output, cache, database,
runtime/browser state, `.env`, credentials, cookies, private keys, old local
implementation, modified local OpenFHE or unrelated reports.

## Authorized selection

- current clean-room project engineering files and workflow;
- project workflow skill and relevant current h=128/paper boundary notes;
- user-supplied `2023.1788.pdf` and `2023.1788.txt`;
- 51 files obtained by sparse checkout from official OpenFHE commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- packet task, provenance, selection and integrity metadata.

The last exact hosted engineering candidate remains
`4ecbd972429884489918d9f82dfc3fe9f702ef4a`, GitHub Actions run
`33892550947`, Linux/Windows 57/57. This packet's engineering tree is
byte-identical to that candidate; the current branch adds only coordination.

No upload or ChatGPT Pro execution is claimed by this packet record. The
dispatch receipt records the actual conversation URL and submission state.
