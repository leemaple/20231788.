# Execution ledger

## Environment and scope

This design task used the supplied mounted ZIP only. Work was limited to archive/provenance verification, static reading of the paper text and pinned source, bounded arbitrary-precision arithmetic, documentation generation, and delivery-package verification. No dependency was installed and no repository or remote was accessed.

Supplied Linux/Windows build and cryptographic results were treated only as archived evidence. They were not rerun or represented as this environment's results.

## Actual checks performed

| Check | Actual command/tool family | Outcome |
|---|---|---|
| Input size and SHA-256 | `stat`, Python streaming/hashlib, `sha256sum` cross-check | **PASS:** 1,531,734 bytes; SHA-256 `efd18ebf2f753624251b1ad60da08d8e31c431ef91495fe93e8951d6cd3f24cc`. |
| Input ZIP inventory and safety | Python `zipfile`, POSIX mode/path checks | **PASS:** 172 unique regular members; no directory, symlink, absolute path, parent traversal, backslash path, or duplicate name. |
| Input ZIP CRC | `ZipFile.testzip()` | **PASS:** no corrupt member. |
| Input manifest closure | Python JSON/path/byte/SHA-256 recomputation | **PASS:** exactly 171 actual non-self files equal the 171 declared entries; every size/hash matches; `MANIFEST.json` is explicitly self-excluded. |
| Root task identity | byte comparison, SHA-256, `wc -l` | **PASS:** root `TASK.md` is byte-identical to `project/coordination/tasks/LOSSLESS_CLIENT_IO_PRO_DESIGN.md`; 189 newline-terminated lines; SHA-256 `d51165e8b27ce9f41de031302673a47d4bab32109a21246ea34aa579b9f4621a`. |
| Current-project provenance | SHA-256 and canonical Git-blob SHA-1 recomputation | **PASS:** all 91 declared Git-origin project entries match their supplied bytes, sizes, SHA-256 values, and blob IDs. |
| Official-source provenance | SHA-256 and canonical Git-blob SHA-1 recomputation | **PASS:** all 75 official entries match; they represent 66 unique upstream paths. The 16 legacy short-name entries close as 9 byte-identical full-path duplicates plus 7 previously verified short-only paths, with no conflicting duplicate. |
| Required source coverage | exact upstream-path set comparison | **PASS:** all 27 declared client-I/O source paths are present; the supplied repeated-operation context paths are also present. |
| Source-ledger reference integrity | parser over every exact `path:line-range` reference in `SOURCE_CLAIM_LEDGER.md` | **PASS:** 62 exact references across 36 archive paths; every file exists and every line/range is in bounds. |
| Static source/signature tracing | bounded `nl`, `sed`, and `rg` inspection | **PASS for the ledgered claims:** public scheme Encrypt/Poly Decrypt dispatch, high-level validation/metadata copying, CKKS flooding placement, ordinary Decode behavior, DCRT/CRT conversion, feature observation, batch-size access, ciphertext clone/metadata behavior, current first-Mult2 state arithmetic, and repeated-task dependencies were inspected in the supplied pinned bytes. This is source review, not compilation. |
| Exact frozen vector arithmetic | Python `fractions.Fraction` | **PASS:** all 16 frozen complex products equal the documented independent products exactly; input delta is `(2^-70, 2^-73)` and product delta is `(2^-71 + 2^-75, -3*2^-74)`. |
| Exact scale arithmetic | Python arbitrary-size integers and `gcd` | **PASS:** `1125899906843009 * 1125899906840833 = 1267650600226646386227681786497`; its gcd with `2^200` is 1. |
| Transform/order sanity check | local non-cryptographic `mpmath` transcription at 140 decimal digits, `N=64,S=16,gap=2` | **PASS:** special forward transform agreed with direct Horner evaluation of an explicitly stride-supported deterministic polynomial; maximum complex difference `9.7651782052278941993322286952468849641734644063638675428120985385664884457775218993793622199678402556219414043e-137`, below `2^-400`. This is scratch arithmetic, not a project test or runtime result. |
| Frozen-input working-precision check | local non-cryptographic `mpmath` special inverse transforms at 160 and 220 decimal digits, followed by exact `2^100` scaling | **PASS:** left/right paper-round integers agreed at both precisions and passed the proposed half-integer guard. Maximum 160/220 coordinate differences were approximately `1.6373791033471652e-131` and `3.3246947843413903e-131`; minimum half-integer distances were approximately `0.03794602685329534` and `0.008706169069976532`. Using `1e-159` only as a nominal arithmetic proxy for a 160-decimal-digit relative epsilon, the resulting bounds were approximately `1.8196937143629876e-130` and `2.4098658646028695e-130`, both below `2^-410`. No claim is made about a compiled Boost `numeric_limits` value. |
| Conversation-file semantic retrieval | scoped Files search against the uploaded archive | No parsed archive result was returned. The developer-provided mounted raw ZIP was then inspected directly, which is the appropriate archive path in this environment. |
| Final documentation consistency | local Markdown/JSON/path validators | **PASS:** required files are nonempty; the public confirmation token is consistent; all eight decisions are present; no implementation/test source was generated; unsupported/pending boundaries are explicit. |
| Final delivery ZIP, manifest, and sidecar | deterministic Python `zipfile`, independent reopen/CRC/path/hash validation | **PASS:** exactly seven nonempty root regular files; no unsafe path, directory, symlink, or duplicate; CRC passes; `MANIFEST.json` self-excludes and closes over exactly six Markdown files with exact byte counts/SHA-256 values; the SHA-256 sidecar exactly matches the final archive bytes and filename. |

## Representative commands actually used

```bash
sha256sum /mnt/data/lossless-client-io-design-b64a980.zip
wc -l <extracted>/TASK.md <extracted>/project/coordination/tasks/LOSSLESS_CLIENT_IO_PRO_DESIGN.md
nl -ba <extracted-source> | sed -n '<bounded-range>p'
rg -n '<source signature or consistency term>' <extracted-tree-or-delivery>
python3 /tmp/verify_lossless_design.py
python3 /tmp/check_transform_precision.py
```

The Python checks recomputed ZIP safety/CRC, manifest closure, provenance SHA-256/Git-blob identities, source-reference bounds, exact rational/vector arithmetic, and the bounded transform calculations described above.

## Explicitly not executed

```text
CMake configure or build
compiler or warning-as-error build
OpenFHE encryption, decryption, key generation, or evaluator operation
project executable or CTest
Linux, Windows, or macOS hosted job
benchmark, paper-scale transform timing, or 1,000-run experiment
Git status, checkout, commit, push, fetch, or remote verification
CI action or workflow mutation
dependency installation
credential or browser-state access
Fable, ZCode, Codex, or other-agent dispatch
live repeated-Mult2 task polling, interruption, refresh, or contact
```

The supplied Fable 5.1 attempts were recorded only as archived 403-before-inference evidence; no Fable result or token output is claimed here.

## Design-only conclusion

The delivery contains documentation only. It creates no test source, production source, build change, cryptographic result, repository mutation, or implementation promise. The proposed seam remains gated on explicit user confirmation and coordinating-workflow recording.
