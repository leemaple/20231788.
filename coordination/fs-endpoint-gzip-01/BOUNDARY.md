# Bounded independent gzip byte seam

Owner: Codex root. This is a separate minimal transport slice after reader and
scalar replay. The attempted reuse of the earlier replay agent was rejected by
the agent thread limit; no additional worker was dispatched. Existing writer
and Linux conversion owners continue without interruption.

The user delegated routine implementation/test-seam decisions. Under adopted
ENDPOINT_SPEC section 8 and TEST_PLAN section 5, tests exercise only public
`encode_gzip(canonical)` and `verify_gzip(...)` byte interfaces. They use
synthetic bytes, not live sidecars or encryption. Verification takes external
expected canonical/gzip sizes and SHA-256 values and returns observed facts.

The input canonical bytes are assumed already validated by the independent
reader; this module does not claim to validate the TSV schema. It must enforce
nonempty/16 MiB bounds, exact fixed gzip header, one member, no trailing bytes,
CRC/ISIZE, finite decompression allocation and exact external identity. Encoding
uses DEFLATE level 9 and a manually fixed header/trailer. No cross-zlib identical
compressed-byte promise is made. SHA/size facts are external, not self-fields.

This slice performs no filesystem publication, status JSON, primary parsing,
wrapper/CI integration, full-slot replay, C++ build or cryptography. A passing
synthetic test cannot be reported as a complete evidence pipeline or E80 PASS.
