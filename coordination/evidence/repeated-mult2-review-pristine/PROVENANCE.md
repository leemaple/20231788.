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
