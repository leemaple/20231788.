# Root verification of the lossless-client-I/O blocker return

Observed 2026-09-05 10:32:55 Asia/Shanghai. This is an independent, bounded,
offline artifact review. It did not run the returned verifier, compile source,
execute cryptography, dispatch CI, use a browser, or change project source,
tests, task requirements, or the frozen oracle.

## Verdict

**The return's `BLOCKED_INPUT_CLOSURE` verdict is accurate. It is not a
semantic RED.**

The exact original input archive is intact, but five of the 25 source objects
listed in its retained official-source audit are absent by canonical path,
basename, and content SHA-256. Twenty are present and independently matched by
size, SHA-256, and Git-blob SHA-1. Two of the five missing objects are cited
directly by U-02 and U-06, while `TASK.md:65-71` requires those cited official
lines to be reread and requires an exact stop when an input is missing.

The return contains no implementation patch, production file, semantic test,
or complete GREEN tree. Its own records keep compilation, cryptographic
runtime, hosted CI, and CTest execution at `NOT RUN`. The returned probe's
recorded exit 2 remains return evidence; this review did not execute it.

## Incoming return gate

- ZIP: `/Users/lifeng/Downloads/lossless-client-io-4ccc8fd-input-blocker.zip`
- Bytes: 60,194
- SHA-256:
  `9d01fdf88a884cf9524e654f97240db05120ba3ef827cde4e35c60a6d17932de`
- Sidecar: 111 bytes; exact digest and canonical basename matched.
- Archive: 24 regular entries under one root, 141,299 expanded bytes.
- CRC, absolute/traversal/backslash/colon paths, encrypted entries, normalized
  duplicates, case-fold collisions, symlinks/devices/special modes: pass.
- `MANIFEST.json` SHA-256:
  `b23f12a7ebecc6475de9744a2e9fda431416a74bafe8e095ff95632cc0a11328`
- Manifest: 22 payloads plus its two declared self-exclusions; every payload
  size/SHA passed and exact closure passed.

Only after this gate passed was the archive extracted to a new temporary
directory. `BLOCKER.md` was read completely: 4,023 bytes, 38 lines, SHA-256
`cfc3402a4671d343e5b247303f29ceace88865ff347d5a7bdc75dd388ce0102c`.
The returned probe was read completely but not executed: 9,596 bytes, 194
lines, SHA-256
`dbd6a1eb601fc16870dd680a84d225b426cd1562c90ccbf065c0fd8614f366f2`.
Seven input-evidence copies retained by the return were also compared
byte-for-byte with the original packet and matched.

## Independent original-input check

- Input: `/private/tmp/lossless-client-io-implementation-01-4ccc8fd.zip`
- Bytes/SHA-256: 1,294,234 /
  `67cea2db1565550c7d96816d076a5d56be45e82f5175e9578e31ddbb50289f89`
- Source base: `4ccc8fd2e7617625d27e58a53eb3489e99466ed4`
- OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Outer archive: 113 regular members, 111 manifest payloads, exact closure.
- Input manifest SHA-256:
  `d38aa03744754a4c28fe52815b46fdcda29c14134c66c54b2b8c134447a9bcc6`
- Selection: 47 project files and 59 `official-full/` files.
- Sole nested design archive: seven regular members, six manifest payloads;
  CRC, per-file hash, exact closure, and retained-copy equality pass.
- Task: 23,771 bytes, SHA-256
  `707d366dcd4880450ac09ba4c1eb6195daf64def65333c305c20a099f8eadb1f`.

The five absent audited sources are:

| Official pinned path | Bytes | SHA-256 | Direct authority |
| --- | ---: | --- | --- |
| `src/core/include/math/dftransform.h` | 4,719 | `7492b8d882a421aa3fa28c2dc2d24548bc914d83e04a719e89c0ff7176dde900` | additional audit closure |
| `src/core/lib/math/dftransform.cpp` | 10,368 | `091220ee6b29ca1b0efbe8afa41a90098507720f59abcd0db1ed0a6e70fc26f3` | U-06, ledger line 32 |
| `src/pke/include/encoding/ckkspackedencoding.h` | 11,651 | `983981653104dc21680653f3b4e1108b51d0e8a39e40a0d8337f7d6502d0fe6a` | U-02, ledger line 28 |
| `src/pke/include/encoding/plaintext.h` | 13,953 | `8736ff85fa9366cc06a30d43a40e8dcaa55a240c9f2c881c880eb79f3cc340cb` | additional audit closure |
| `src/pke/include/encoding/plaintextfactory.h` | 6,826 | `16f788d3e4cc32e8831bcbe7009f99a05e6ef6f0496c19e6e119e821f98b61a7` | additional audit closure |

No same-basename or same-digest substitute exists in either the outer packet
or its nested archive.

## Important limitations and deviations

1. The returned probe checks raw duplicate archive names, but it does not
   check normalized duplicates or case-fold collisions. The independent gate
   above supplied those checks and found no issue in either archive.
2. Its `sameBasenameMembers` search ranges over outer files only, although the
   blocker prose says nested basenames were also scanned. Independent review of
   all seven nested names and contents found no matching basename or digest, so
   the blocker conclusion is unchanged.
3. The expected identities for the absent bytes come from the authenticated
   retained `main-source-verification-20260904.json`. Neither the return nor
   this review refetched those absent bytes from the upstream pin. The return
   discloses that boundary correctly; a corrected supplement must bind the
   actual bytes to the exact pinned tree.

## Next gate

Supply all five exact pinned source objects with hash-bound provenance. Then
rerun the mandatory source reading and the unchanged four-patch TDD assignment.
There is no evidence-based reason to alter the accepted public seam, oracle,
thresholds, CTest target, or patch order, and no reason to classify this input
omission as a semantic RED.
