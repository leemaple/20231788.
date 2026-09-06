# Independent gzip byte transport — actual bounded Python tests

Codex root authored the public test before the missing module, then the smallest
roundtrip implementation. After real roundtrip GREEN, stricter container tests
exposed missing header/end-of-stream gates; after that correction, input-type
tests exposed empty/mutable inputs and bool/float counts. All raw returns are
retained in `evidence/`, without replacing earlier failures.

| Stage | Actual result |
| --- | --- |
| Missing module | 1 discovered ERROR, exit 1 (missing implementation RED) |
| Synthetic nonzero roundtrip | 1 PASS, 0.000 s, exit 0 |
| Header/trailer/member boundaries | 3 discovered, 8 failing subtests, exit 1 |
| Strict header/end-of-stream fix | 3 PASS, 0.001 s, exit 0 |
| Empty/type/external-count boundaries | 7 discovered, 4 failing subtests and 1 error, exit 1 |
| Strict input contract fix | 7 PASS, 0.005 s, exit 0 |
| Independent-review boundary additions | 10 PASS, 0.091 s, exit 0; unchanged implementation |

Final observed environment: bundled CPython 3.12.14, zlib build/runtime 1.2.12.
Each command is one local Python process; no C++/FHE/FFT/NTT or hosted dispatch.
The initial largest input fixture was 16 MiB + 1 byte, rejected before compression. The
decompression-overflow fixture compresses 1 MiB once and asks verification for
32 bytes; its decompressor can return at most 33 bytes. No repeated trials or
full 16,384-row numerical replay was performed.

Current source identities after the review-requested tests:

- `paper_endpoint_gzip.py`: `d5a6825f5a1092b52f07da70f2a09d45065c283b1fe6e6c6d3d6dcf4f199839e`
- `test_paper_endpoint_gzip.py`: `29e7a184e77b86110be75bab1b09bf5ed356b47b3e68f946ad99438b5b1fb32c`

An independent Codex source review found no functional defect in the two byte
seams and requested three missing boundary checks. The added tests successfully
roundtrip exactly 16 MiB, reject a real 32 MiB + 1025-byte compressed input for
the envelope cause before payload parsing, and reject mutable verifier input.
No internal constant is patched and no decompressor is mocked. The unchanged
implementation passed their first execution, so this is coverage closure, not
a claimed new behavioral RED/GREEN correction. `evidence/07-...` retains that
actual output and new test hash. Review of the added assertions was requested.

## Resource and protocol boundary

The compressor uses level 9 raw DEFLATE, one Z_FINISH, a fixed ten-byte header,
and explicit little-endian CRC32/ISIZE. The 32 MiB + 1024 compressed-byte limit
is a conservative transport envelope, not a claim that all possible redundant
DEFLATE encodings of a valid canonical file must fit. The module also checks
its own emitted gzip against that envelope. The intended level-9 output is
small enough; an unsupported larger output fails rather than bypassing limits.

Verification checks external lengths/hashes and the exact header, then uses
`decompressobj(wbits=31).decompress(data, canonical_size+1)`. The expected size
is first restricted to 1..16 MiB. It never calls unbounded decompress or flush.
Success requires exact decoded size, EOF, no unused data and no unconsumed
tail, followed by canonical SHA-256 agreement. zlib checks the gzip CRC/ISIZE;
mutating either trailer field with an updated external gzip hash is rejected.

The primary references used for this API boundary were the official
[Python zlib documentation](https://docs.python.org/3/library/zlib.html)
(max_length, eof, unused_data/unconsumed_tail and gzip window mode) and the
[zlib manual](https://zlib.net/manual.html). The actual tested runtime above,
not the current documentation's version label, is the execution environment.

This byte module does not validate TSV semantics, prove the compression level
from arbitrary DEFLATE input, require cross-zlib byte identity, authenticate
untrusted external expected hashes, or implement status/publication/CI. Those
remain caller/integration boundaries. Its final independent review is pending;
do not label this a complete evidence-packer or original E80 result.
