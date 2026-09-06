# FS-RESIDUAL-ENDPOINT-01 — partial return, not GREEN

## Decision

**PARTIAL_NOT_GREEN. Do not push this as the diagnostic GREEN or schedule the next chain from it.**
The complete requested implementation has not been delivered. This is a bounded,
usable standard-library primitive slice plus test-first evidence. It does not
satisfy the existing seven C++ declarations, and does not remove the hosted link
failure. No missing source/API prerequisite or contradiction in v1-r1 was
established. The blocking boundary is unfinished implementation/integration,
not a claimed specification defect, service dependency, or numerical result.

The original full-chain E80 failure remains unchanged and separate. The new
observer has no live numerical disposition. A primitive unit-test PASS, when
recorded in the ledger, is not endpoint observer GREEN or paper acceptance.

## Actual delivered interfaces

`files/tests/endpoint_evidence_primitives.py` contains no project imports and no
command-line finalizer. `files/tests/test_endpoint_evidence_primitives.py` is a
standalone `unittest` suite. They add two repository-relative paths only. No
production, public include, existing oracle, old test body/inventory, CMake or
workflow path is changed. All 39 input engineering paths remain byte-identical;
`PATH_IDENTITIES.json` gives every identity and the two ordered patches.

| Adopted requirement fragment | Actual function | Exact delivered boundary |
| --- | --- | --- |
| Exact C/S power-of-two ceiling | `scaled_one_norm_exponent` | Exact integer bit-length/cross-comparison, zero sentinel, negative exponents, positive reduced scale. It does not calculate polynomial C. |
| Classification precedence | `classify` | Exact Fraction comparisons; invalid, raw disagreement, unsupported/excess budget, two-path contradiction, then threshold overlap. It does not ascertain model support. |
| Conditional endpoint formulas | `endpoint_allowances` | D/H/P/Q/R and E0/E8/I8/A8/identity formulas. No library proof or live applicability checks. |
| Disk replay formula | `replay_allowances` | Exact Lipschitz inequality and specified replay budgets including supplied transport quanta. No Decimal replay is performed. |
| Canonical decimal text | `canonical_decimal`, `is_canonical_decimal`, `parse_canonical_decimal` | Exact integer half-even rounding from supplied Fraction, strict independent scanner/parser, quantum, unique zero and exponent range. No C++ represented-binary extraction or C++ independent validator. |
| Bounded scalar records | `parse_uint`, `integer_text`, `reduced_nonnegative` | Bounded canonical integers without altering Python's global integer-string limit; malformed unreduced receipts rejected. |
| Byte envelope | `validate_ascii_lines` | ASCII/LF/no BOM/CR/NUL, 16 MiB and 32768-byte lines. **Not the ordered metadata/check/header/16384-row schema.** |
| Deterministic gzip member | `gzip_bytes`, `validate_gzip` | Fixed header, raw level-9 deflate, one member, CRC/ISIZE, decompression cap, external byte/hash verification. **Not filesystem publication or schema attestation.** |
| Strict status envelope | `strict_status_json` | Duplicate/float/nonfinite/boolean rejection, exact caller-provided keys, sorted compact ASCII LF, no recognized self-size/hash fields. **Not complete status-schema semantics or CTest/E80 consistency.** |

## Mathematical and transport qualifications

All computations in this slice are exact Python integer/Fraction arithmetic.
There are no FFT, NTT, crypto, codec, polynomial decrypt, roots, pi, sin/cos or
complex-power computations. Test code never treats the arithmetic allowance
formulas as proof of the conditional Boost premises. The module accepts exact
Fractions at its formatting boundary; it does not assert those inputs came from
binary512 or binary768. The loss-of-low-bit test concerns Python Fraction only,
not the missing C++ extraction implementation.

The formatter uses an integer approximation only to initialize the decimal
exponent search. Exact rational comparisons determine the final exponent, and
integer quotient/remainder and parity decide rounding. No nonzero input is
silently replaced by zero. Parsing returns the printed value and its individual
rounding quantum. Full tuple replay and complex-transport quantum aggregation
are intentionally not claimed.

`gzip_bytes` deliberately can encode empty bytes; gzip validity alone is not
complete canonical evidence. The test suite explicitly demonstrates that empty
gzip decodes but is rejected by the canonical envelope validator. Similarly,
JSON envelope validity is not status semantic validity. These lower-level APIs
must never be used as upload authorization without the missing outer validators.
Canonical equivalence is not a claim of identical compressed bytes across zlib
versions. The actual interpreter and zlib versions are in the execution ledger.

Integer records are capped at 10000 digits, canonical exponent grammar at five
digits, and formatter input components at 400000 bits. These are explicit bounds
of this partial module. Their integration with complete runtime range checks
has not been adjudicated. No unsupported underflow or decimal conversion premise
has been silently adopted.

## Missing implementation boundary

1. All seven C++ endpoint helper implementations remain absent; the accepted missing-link baseline is not repaired.
2. No C++ full-slot DFT/direct768 observer, exact represented-binary extraction, or all-slot control execution is delivered.
3. No actual Horner R(Int) conversion applicability checks, endpoint radius checks, or 24 comparison receipts are integrated.
4. No full E0/E8/I8/A8 signed maxima/argmax/tuple capture and no owner-cleanup-safe RunPaper handoff/publication are delivered.
5. No complete ordered sidecar schema reader/writer, all-nine-scale primary CTest parser, or Decimal-256 signed-tuple replay is delivered.
6. No filesystem ownership/atomic no-overwrite publication, complete status semantics, finalizer, exact upload selection, or failure-preserving single-CTest shell/workflow wiring is delivered.
7. No C++ compilation, observer self-test, hosted synthetic evidence tests, Linux chain, Windows chain, or independent review was performed.

## Integration and patch use

Apply `patches/01-test-first.patch`, then `patches/02-implementation.patch` relative
to the exact `project/` root, not the archive root. Alternatively copy both
complete files under `files/`; do not both copy and apply the same patches.
The test-first stage fails because the new Python primitive module is absent.
It is not a numerical RED and is unrelated to the accepted hosted C++ API/link
RED. The implementation stage only makes this primitive suite runnable.

This return contains no speculative C++ interface declarations, stubs,
placeholder DFT, broad framework, historical source copy, dependencies,
credentials, raw polynomials, keys, ciphertexts or live canonical evidence.
The earlier incomplete evidence-reader work is not misrepresented as a complete
reader here; only the independently testable primitive boundary is packaged.

## No-extra-trial rule

No CI push, dispatch, repeat, build or chain is authorized by this partial
package. The prescribed future full-task order and all resource limits remain
unchanged, but require a complete independently reviewed implementation first.
No compile/integrity/unsupported/timeout result should trigger an automatic
second chain to obtain a sidecar. The historical E80 failure is not reclassified.
