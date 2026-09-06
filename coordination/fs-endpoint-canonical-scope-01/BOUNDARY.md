# Scope-specific canonical input lookup

Root-owned vertical slice at 84edbec31b36cfa01481d3f775607e784a0017f7.
The user has delegated ordinary seam and implementation decisions. Tests use
the already agreed public finalizer CLI / real-filesystem seam, not internals.

Observed C++ writer contract: synthetic canonical directories and leaves use
fs-endpoint-synthetic-<sha>.<host>.<run>.<attempt>. Live canonical inputs use
fs-residual-endpoint-01.v1-r1.<sha>.<host>.<run>.<attempt>. The Python finalizer
currently accepts only the latter even when expected_scope is synthetic.

First RED: correct synthetic canonical directory containing a tiny malformed TSV
must reach the independent sidecar parser and publish FORMAT, not be rejected
as a foreign canonical directory. This is a path-selection test, not numerical
interoperability, full replay or C++ execution. Status/published naming stays the
existing schema-defined name for both scopes; no renaming of C++ output.

Then minimally derive canonical stem from the explicit validated scope and
identity. Reject the other scope's prefix and mixed/foreign inputs. Preserve
existing source identity, numerical, primary, reader, replay, reconciliation,
gzip, status and publication contracts. Update only synthetic test canonical
paths to match the actual writer. No legacy compatibility fallback is required
for a test-only incorrectly named fixture.

All local checks are small Python subprocess/filesystem tests. The actual C++
Capture/Write/Emit -> Python numerical interop fixture and full 16384-slot replay
are a separate hosted-only gate. Do not claim this tiny FORMAT test proves them.
