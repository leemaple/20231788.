# Lossless client I/O implementation Pro packet 01 — preparation and dispatch receipt

## Prepared packet

- Prepared: 2026-09-05 07:14 Asia/Shanghai.
- Branch: `codex/lossless-io-implementation-01`.
- Implementation source base:
  `4ccc8fd2e7617625d27e58a53eb3489e99466ed4`.
- Final reviewed task overlay commit:
  `a6937904887d17dffcfcf8a2367b9b4244c52961`.
- Packet-metadata source commit:
  `dacc8acd24a3a974b435e83ba525cd74195f5f75`.
- Last independently hosted engineering bytes:
  `4ecbd972429884489918d9f82dfc3fe9f702ef4a`.
- Retained hosted run: `33892550947` (Linux 57/57 and Windows 57/57 on
  unchanged engineering bytes).
- OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- ZIP:
  `/private/tmp/lossless-client-io-implementation-01-4ccc8fd.zip`.
- ZIP bytes: `1294234`.
- ZIP SHA-256:
  `67cea2db1565550c7d96816d076a5d56be45e82f5175e9578e31ddbb50289f89`.
- Sidecar:
  `/private/tmp/lossless-client-io-implementation-01-4ccc8fd.zip.sha256`
  (`115` bytes; SHA-256
  `84aa4276bca93b616dc839ee6c84caa579cd2cedf4575c62c939da2e632c060b`).

## Verified preparation evidence

- `project/` contains 47 selected files, each copied as an exact Git blob from
  implementation source base `4ccc8fd`.
- All engineering bytes at that source base match independently tested commit
  `4ecbd972`; the current engineering tree still registers 57 CTests.
- `TASK.md` is 23,771 bytes, SHA-256
  `707d366dcd4880450ac09ba4c1eb6195daf64def65333c305c20a099f8eadb1f`.
- `paper/` contains the two exact user-supplied paper files.
- `official-full/` contains 59 exact source files retained from the verified
  prior design packet at pristine OpenFHE pin `df495ba`.
- The outer manifest covers 111 regular payload files. Its declared
  self-exclusions are `MANIFEST.tsv` and `MANIFEST.sha256`; the latter verifies.
- Final ZIP has 113 regular files under one root. It has no absolute,
  traversal, backslash, duplicate, case-colliding, symlink, device, or other
  special entry. CRC, manifest closure, sidecar, mandatory hashes, and a fresh
  extraction all pass.
- Gitleaks 8.30.1 scanned both the complete final staging tree and final ZIP at
  archive/decode depth 5: zero findings in both scans.
- Targeted credential filename scan passes. The exact official OpenFHE source
  header `official-full/src/pke/include/key/privatekey.h` is allowlisted as
  public source code, not private-key material.
- Excluded: `.git`, dependencies, builds, caches, databases, runtime/browser
  state, `.env`, API keys, tokens, actual private keys, cookies, credentials,
  old local implementations, modified local OpenFHE, unrelated reports, and
  unrelated coordination history.

No OpenFHE/project build, cryptographic execution, benchmark, or CI dispatch
was performed on the Mac while preparing this packet.

## External dispatch

Status: `PREPARED_NOT_SENT`.

Target: existing ChatGPT Pro conversation `Design Client IO Seam`, URL
`https://chatgpt.com/c/6a9ad753-af90-83ec-9062-0fc671f64197`.

The exact frozen dispatch prompt is `DISPATCH_PROMPT.md`: 3,576 bytes,
SHA-256
`900f215cfdb6ea2e860600cba775dd78f9db7101ebed57fe022584cbeb4d9110`.
The single-send browser evidence will be recorded here after submission. Until
that evidence exists, this receipt does not claim that ChatGPT Pro received,
parsed, or started the assignment.
