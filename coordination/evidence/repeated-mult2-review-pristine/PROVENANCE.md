# Pinned official forward declaration — supplemental source check

Retrieved read-only from GitHub's official `openfheorg/openfhe-development`
contents API with exact ref `df495ba2e91739a6dc8f1de254fc5a41155ce504`
on 2026-09-04. This is pristine upstream input, not a local OpenFHE checkout.

- Upstream path: `src/pke/include/ciphertext-fwd.h`.
- URL: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/ciphertext-fwd.h
- GitHub reported size: 3890 bytes; decoded and retained size: 3890 bytes.
- Reported and locally recomputed Git blob SHA1:
  `35dcbcd41327e6966c2d0c0ac7518db3cabee508`.
- Retained complete-file SHA256:
  `deb7550169a8b673105609313562e036bd48827a065eec41a27c414ebe710a10`.
- Local verification: `wc -c`, `shasum -a 256`, `git hash-object` (without -w).

Full license/comments are retained. Root decoded the API's base64 content,
read the full header and checked exact byte count/Git blob identity. Relevant
fact: ConstCiphertext aliases a const shared_ptr whose pointee is const; its
reference can bind the corresponding conversion temporary. This does not
assert candidate compilation or runtime success. The file was not included
in the earlier 53-source packet; this is an explicitly supplemental source.

## Supplemental declaration closure — 2026-09-05 Asia/Shanghai

After the ZCode review reported missing declaration evidence, root retrieved
the following complete files read-only from the same official GitHub contents
API at the exact same pin. The key-directory listing confirmed the concrete
`evalkeyrelin.h` path. No local OpenFHE tree was consulted. Root read the
declarations, preserved the full license/source bytes, and recomputed the Git
blob IDs; all three match the API responses exactly.

| Retained file | Exact upstream path | Bytes | Git blob SHA1 | SHA256 |
| --- | --- | ---: | --- | --- |
| `evalkey.h` | `src/pke/include/key/evalkey.h` | 5060 | `c2ff1b17cb0752a5cb65a53b4e1c4cbfe39c32c9` | `f01f5049cc9eeb1a9401f1d5e73951ee30e1367255206752b429ee6eec424aad` |
| `scheme-id.h` | `src/pke/include/scheme/scheme-id.h` | 2945 | `e4cb4fef2b33d3ea3092c018afbff3f60a05ef2d` | `98139eed774b4e834203a70149061f98f318e0fde70c33bf769e777a94341b78` |
| `evalkeyrelin.h` | `src/pke/include/key/evalkeyrelin.h` | 6595 | `8ea9462d3d575a7c02905d42202e98e7eee14d5b` | `8289c28cfdaec2de15c7eb1a15fdc33ffe19f7ba23f1f66d4c08d62b2bd01358` |

Primary links: [abstract evaluation key](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/evalkey.h),
[concrete relinearization key](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/evalkeyrelin.h),
[scheme enum](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/scheme-id.h).

`evalkey.h:97–99,130–132` declares public virtual const-reference getters
(the abstract defaults throw); `evalkeyrelin.h:141–143,171–173` publicly
overrides them and returns the owned A/B vectors. `scheme-id.h:45–50`
defines `SCHEME` and `CKKSRNS_SCHEME`. This closes the specific declaration
gap, not compilation, key correctness or the pending repeated-operation gate.
