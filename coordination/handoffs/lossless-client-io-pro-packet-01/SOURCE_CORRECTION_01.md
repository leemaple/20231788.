# Lossless client I/O packet source correction 01

## Scope and authority

This is a source-material closure correction for the existing lossless client
I/O implementation packet. It does not replace or amend `TASK.md`, authorize a
different implementation, or change any project source, test, CMake, workflow,
paper, frozen vector/oracle, branch, or source commit.

The original input archive remains identified by:

- filename: `lossless-client-io-implementation-01-4ccc8fd.zip`;
- bytes: `1294234`;
- SHA-256:
  `67cea2db1565550c7d96816d076a5d56be45e82f5175e9578e31ddbb50289f89`;
- exact original payload records: `111`;
- implementation base:
  `4ccc8fd2e7617625d27e58a53eb3489e99466ed4`;
- task overlay:
  `a6937904887d17dffcfcf8a2367b9b4244c52961`;
- tested engineering source:
  `4ecbd972429884489918d9f82dfc3fe9f702ef4a`; and
- pristine OpenFHE pin:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

Every one of the original 111 payload files is retained byte-for-byte. The
original `MANIFEST.tsv` and `MANIFEST.sha256` are also retained byte-for-byte at
`provenance/original-input/`; a new root manifest closes over the corrected
packet.

## Exact correction

The original packet copied all 59 entries under the earlier archive's
`official-full/` prefix, but five I/O-required upstream paths existed only under
legacy `official-openfhe/` short citation names. The historical statements of
`59` official-full files in the unchanged original `PACKET_README.md` and
`SOURCE_PROVENANCE.json` describe the original packet. This correction adds
exactly five unique canonical paths, making the effective corrected
`official-full/` count `64`; those historical files are intentionally not
rewritten.

| Legacy citation/source path | Added packet path (`official-full/` + canonical upstream path) | Bytes | SHA-256 | Git blob SHA-1 |
| --- | --- | ---: | --- | --- |
| `official-openfhe/dftransform.cpp` | `official-full/src/core/lib/math/dftransform.cpp` | 10368 | `091220ee6b29ca1b0efbe8afa41a90098507720f59abcd0db1ed0a6e70fc26f3` | `ea5b3b2dcc2b9024da0487d3de285c241a8ec8a4` |
| `official-openfhe/dftransform.h` | `official-full/src/core/include/math/dftransform.h` | 4719 | `7492b8d882a421aa3fa28c2dc2d24548bc914d83e04a719e89c0ff7176dde900` | `d4ed3000c11618102aa5415c8561809d3aad0273` |
| `official-openfhe/ckkspackedencoding.h` | `official-full/src/pke/include/encoding/ckkspackedencoding.h` | 11651 | `983981653104dc21680653f3b4e1108b51d0e8a39e40a0d8337f7d6502d0fe6a` | `956a97ff6f214abb2d690c77bceca0ec746f753a` |
| `official-openfhe/plaintext.h` | `official-full/src/pke/include/encoding/plaintext.h` | 13953 | `8736ff85fa9366cc06a30d43a40e8dcaa55a240c9f2c881c880eb79f3cc340cb` | `19d32771478cd5365248d5a6630e28b7501b47fb` |
| `official-openfhe/plaintextfactory.h` | `official-full/src/pke/include/encoding/plaintextfactory.h` | 6826 | `16f788d3e4cc32e8831bcbe7009f99a05e6ef6f0496c19e6e119e821f98b61a7` | `a28c475d70d786b0f63ced2bcbcd116642b9a703` |

No `base-leveledshe.h`, `keyswitch-bv.cpp`, or other legacy-only source is
added. They are outside the bounded I/O source closure.

## Why each file is in scope

- The retained design `SOURCE_CLAIM_LEDGER.md` directly cites
  `official-openfhe/ckkspackedencoding.h` in U-02 and
  `official-openfhe/dftransform.cpp` in U-06. The retained
  `DESIGN_DECISION.md` repeats those sources in Decisions 1 and 5.
- The retained `PRODUCTION_IO_OFFICIAL_API_AUDIT.md` identifies the five files
  as S05, S08, and S14-S16. Its exact references use `dftransform.h` for the
  public special-transform surface, `dftransform.cpp` for roots and butterfly
  order, `ckkspackedencoding.h` for cached binary64 values/slot checks, and
  `plaintext.h` plus `plaintextfactory.h` for the rejected internal Plaintext
  adapter alternative.
- The retained
  `coordination/evidence/production-io-api/main-source-verification-20260904.json`
  independently records their canonical paths, byte sizes, SHA-256 values and
  Git blob identities at the exact OpenFHE pin.

The builder obtains each byte sequence with `git show <pin>:<canonical-path>`
from a newly initialized, shallow, blob-filtered official repository. It then
requires byte equality with the matching legacy member in the previously
verified design input archive:

- archive bytes: `1531734`;
- archive SHA-256:
  `efd18ebf2f753624251b1ad60da08d8e31c431ef91495fe93e8951d6cd3f24cc`.

## Evidence boundary

This correction performs archive/source integrity work only. It does not run a
project or OpenFHE build, cryptographic operation, test, benchmark, GitHub
Action, browser task, external agent, upload, commit, or push. The existing
task and all prior execution claims remain unchanged. Independent secret
scanning and final delivery review remain Codex-owned gates.
