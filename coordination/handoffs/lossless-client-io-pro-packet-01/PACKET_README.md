# Lossless client I/O implementation packet 01

This packet asks ChatGPT Pro for the first production high-precision client-I/O
RED/GREEN slice.  It is a clean-room input bundle, not a test result or an
implementation-complete claim.

## Controlling identities

- Branch: `codex/lossless-io-implementation-01`.
- Implementation source base: `4ccc8fd2e7617625d27e58a53eb3489e99466ed4`.
- Final reviewed task overlay commit:
  `6fb991f4e850eaa3b389e33b871704186f20e2db`.
- Last independently hosted engineering bytes:
  `4ecbd972429884489918d9f82dfc3fe9f702ef4a`.
- Pristine OpenFHE pin:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Retained hosted run for the unchanged 57-test engineering tree:
  `33892550947` (Linux 57/57 and Windows 57/57).
- Task bytes: 23,380; SHA-256
  `95087208602ae132d793fd260e42eb4a57dc045cceceaf02015e726422cb5a1e`.

The task is placed at packet-root `TASK.md`.  It is the active assignment and
wins over all older design notes.  Files below `project/` are exact Git blobs
from the implementation base.  `paper/` and `official-full/` are byte-derived
from the previously verified design input archive whose SHA-256 is
`efd18ebf2f753624251b1ad60da08d8e31c431ef91495fe93e8951d6cd3f24cc`.

## Contents and exclusions

The packet contains the task, 47 selected current project files, the two exact
user paper files, and 59 exact official OpenFHE source files.  The project
selection includes all current production/header/test/CMake/workflow files,
the project workflow/engineering instructions, the confirmed seam, the prior
I/O design return, both independent reviews, relevant source audits and the
retained hosted evidence boundary.

It deliberately excludes `.git`, dependencies, builds, caches, databases,
runtime/browser state, logs not needed by the task, `.env`, API keys, tokens,
actual private keys, cookies, credentials, old local implementations, modified
local OpenFHE, unrelated reports and unrelated coordination history.  The
official source header named `privatekey.h` is source code at the exact public
pin, not private-key material.

`MANIFEST.tsv` closes over every regular payload file except itself and its
sidecar.  `MANIFEST.sha256` authenticates the exact `MANIFEST.tsv` bytes.  Read
`SOURCE_PROVENANCE.json` for source selection facts.  ChatGPT Pro must verify
the outer archive and manifest before relying on any content.

No project/OpenFHE compilation, cryptographic execution, benchmark or CI run
was performed on the Mac to prepare this packet.
