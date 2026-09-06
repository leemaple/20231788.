import hashlib
import struct
import unittest
import zlib

import paper_endpoint_gzip as transport


class GzipBoundaryTests(unittest.TestCase):
    def verify(self, compressed, canonical, **overrides):
        expected = dict(canonical_size=len(canonical),
                        canonical_sha256=hashlib.sha256(canonical).hexdigest(),
                        gzip_size=len(compressed),
                        gzip_sha256=hashlib.sha256(compressed).hexdigest())
        expected.update(overrides)
        return transport.verify_gzip(compressed, **expected)

    def test_nonzero_synthetic_bytes_roundtrip_with_exact_header_and_external_facts(self):
        canonical = b"synthetic endpoint bytes\n+1.25\t-0.125\n"
        compressed = transport.encode_gzip(canonical)
        self.assertEqual(compressed[:10], bytes.fromhex("1f8b08000000000002ff"))
        receipt = transport.verify_gzip(
            compressed, canonical_size=len(canonical),
            canonical_sha256=hashlib.sha256(canonical).hexdigest(),
            gzip_size=len(compressed),
            gzip_sha256=hashlib.sha256(compressed).hexdigest())
        self.assertEqual(receipt.canonical, canonical)
        self.assertEqual(receipt.canonical_size, len(canonical))
        self.assertEqual(receipt.gzip_size, len(compressed))

    def test_rejects_optional_header_fields_mtime_xfl_os_and_wrong_method(self):
        canonical = b"synthetic header boundary\n"
        compressed = transport.encode_gzip(canonical)
        for position, value in ((2, 7), (3, 4), (3, 8), (3, 16),
                                (4, 1), (8, 0), (9, 3)):
            with self.subTest(position=position, value=value):
                malformed = bytearray(compressed)
                malformed[position] = value
                with self.assertRaises(transport.GzipError):
                    self.verify(bytes(malformed), canonical)

    def test_rejects_extra_member_trailing_data_and_missing_trailer(self):
        canonical = b"synthetic stream boundary\n"
        compressed = transport.encode_gzip(canonical)
        for suffix in (b"foreign", compressed):
            with self.subTest(suffix_bytes=len(suffix)):
                with self.assertRaises(transport.GzipError):
                    self.verify(compressed + suffix, canonical)
        for missing in (1, 4, 8):
            with self.subTest(missing_trailer_bytes=missing):
                with self.assertRaises(transport.GzipError):
                    self.verify(compressed[:-missing], canonical)

    def test_rejects_empty_mutable_and_oversized_canonical_input(self):
        for canonical in (b"", bytearray(b"synthetic\n"),
                          b"x" * (transport.MAX_CANONICAL_BYTES + 1)):
            with self.subTest(type=type(canonical).__name__, size=len(canonical)):
                with self.assertRaises(transport.GzipError):
                    transport.encode_gzip(canonical)

    def test_rejects_boolean_float_and_false_external_facts(self):
        canonical = b"x"
        compressed = transport.encode_gzip(canonical)
        for overrides in ({"canonical_size": True}, {"canonical_size": 1.0},
                          {"gzip_size": float(len(compressed))},
                          {"canonical_size": 2}, {"gzip_size": len(compressed) + 1},
                          {"canonical_sha256": "0" * 64},
                          {"gzip_sha256": "0" * 64}):
            with self.subTest(overrides=overrides):
                with self.assertRaises(transport.GzipError):
                    self.verify(compressed, canonical, **overrides)

    def test_rejects_crc_and_isize_corruption_even_with_updated_external_gzip_hash(self):
        canonical = b"synthetic checksum boundary\n"
        compressed = transport.encode_gzip(canonical)
        crc, size = struct.unpack("<II", compressed[-8:])
        self.assertEqual(crc, zlib.crc32(canonical))
        self.assertEqual(size, len(canonical))
        for offset in (-8, -4):
            with self.subTest(offset=offset):
                malformed = bytearray(compressed)
                malformed[offset] ^= 1
                with self.assertRaisesRegex(transport.GzipError, "checksum"):
                    self.verify(bytes(malformed), canonical)

    def test_decompression_is_bounded_by_expected_size_without_accepting_a_prefix(self):
        # A 1 MiB body is compressed once; validation may emit at most 33 bytes.
        compressed = transport.encode_gzip(b"a" * (1024 * 1024))
        with self.assertRaisesRegex(transport.GzipError, "overflow"):
            self.verify(compressed, b"a" * 32)

    def test_accepts_inclusive_16_mib_canonical_limit(self):
        canonical = b"x" * (16 * 1024 * 1024)
        compressed = transport.encode_gzip(canonical)
        receipt = self.verify(compressed, canonical)
        self.assertEqual(receipt.canonical, canonical)
        self.assertEqual(receipt.canonical_size, 16 * 1024 * 1024)

    def test_rejects_compressed_envelope_before_parsing_payload(self):
        # Real bounded input: no internal constant patch or decompressor mock.
        compressed = bytes.fromhex("1f8b08000000000002ff") + b"x" * (
            32 * 1024 * 1024 + 1025 - 10)
        with self.assertRaisesRegex(transport.GzipError,
                                    "gzip size outside transport envelope"):
            self.verify(compressed, b"x")

    def test_verifier_rejects_mutable_gzip_input(self):
        canonical = b"synthetic mutable boundary\n"
        compressed = bytearray(transport.encode_gzip(canonical))
        with self.assertRaisesRegex(transport.GzipError, "immutable bytes"):
            self.verify(compressed, canonical)


if __name__ == "__main__":
    unittest.main(verbosity=2)
