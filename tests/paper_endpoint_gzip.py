"""Independent bounded gzip byte transport, not a TSV/status validator."""
from dataclasses import dataclass
import hashlib
import struct
import zlib


MAX_CANONICAL_BYTES = 16 * 1024 * 1024
MAX_GZIP_BYTES = 2 * MAX_CANONICAL_BYTES + 1024
HEADER = bytes.fromhex("1f8b08000000000002ff")


class GzipError(ValueError):
    pass


@dataclass(frozen=True)
class GzipReceipt:
    canonical: bytes
    canonical_size: int
    canonical_sha256: str
    gzip_size: int
    gzip_sha256: str


def encode_gzip(canonical):
    if type(canonical) is not bytes or not 0 < len(canonical) <= MAX_CANONICAL_BYTES:
        raise GzipError("canonical input must be nonempty bytes within 16 MiB")
    compressor = zlib.compressobj(level=9, wbits=-15)
    payload = compressor.compress(canonical) + compressor.flush(zlib.Z_FINISH)
    result = HEADER + payload + struct.pack("<II", zlib.crc32(canonical), len(canonical))
    if len(result) > MAX_GZIP_BYTES:
        raise GzipError("compressed output exceeds transport envelope")
    return result


def verify_gzip(data, *, canonical_size, canonical_sha256, gzip_size, gzip_sha256):
    if type(data) is not bytes:
        raise GzipError("gzip input must be immutable bytes")
    if type(canonical_size) is not int or not 0 < canonical_size <= MAX_CANONICAL_BYTES:
        raise GzipError("canonical size outside 16 MiB envelope")
    if type(gzip_size) is not int or not 0 < gzip_size <= MAX_GZIP_BYTES:
        raise GzipError("gzip size outside transport envelope")
    if len(data) != gzip_size:
        raise GzipError("gzip byte count mismatch or envelope excess")
    for digest in (canonical_sha256, gzip_sha256):
        if (type(digest) is not str or len(digest) != 64 or
                any(character not in "0123456789abcdef" for character in digest)):
            raise GzipError("external SHA-256 must be lowercase hexadecimal")
    if hashlib.sha256(data).hexdigest() != gzip_sha256:
        raise GzipError("gzip hash mismatch")
    if len(data) < 18 or data[:10] != HEADER:
        raise GzipError("gzip header is not the exact canonical header")
    decoder = zlib.decompressobj(wbits=31)
    try:
        canonical = decoder.decompress(data, canonical_size + 1)
    except zlib.error as error:
        raise GzipError("invalid gzip checksum or compressed stream") from error
    if len(canonical) != canonical_size:
        raise GzipError("decompressed byte count mismatch or overflow")
    if not decoder.eof or decoder.unconsumed_tail or decoder.unused_data:
        raise GzipError("gzip is truncated, has trailing bytes or additional members")
    actual_hash = hashlib.sha256(canonical).hexdigest()
    if actual_hash != canonical_sha256:
        raise GzipError("canonical hash mismatch")
    return GzipReceipt(canonical, len(canonical), actual_hash,
                       len(data), hashlib.sha256(data).hexdigest())
