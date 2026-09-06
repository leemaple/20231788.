# Endpoint publication boundary

Status: sealed design before the first implementation test on 2026-09-06.

## Public seam

`paper_endpoint_publication.py` owns two operations:

```python
publish_endpoint_evidence(
    published_parent: pathlib.Path,
    identity: PublicationIdentity,
    status_record: dict[str, object],
    *,
    canonical_bytes: bytes | None,
    gzip_bytes: bytes | None,
) -> PublicationResult

select_endpoint_uploads(
    published_parent: pathlib.Path,
    identity: PublicationIdentity,
) -> tuple[pathlib.Path, ...]
```

`PublicationIdentity` contains the exact source commit, host, GitHub run ID, and
run attempt. `PublicationResult.required_exit_code` is zero only for a successful
COMPLETE/PASS record; a retained CTest exit (including the paper contract's 8)
is returned unchanged and every incomplete result requires a nonzero exit.
`PublicationError` carries a nonzero `required_exit_code` and, when a truthful
fallback was committed, its exact status path. A fallback is therefore not a
successful packaging result.

The status and gzip codecs remain unchanged. The publisher round-trips the
supplied status with `encode_status` and `decode_status`. For COMPLETE evidence
it also calls `verify_gzip` against the status's actual byte counts and hashes,
then requires the decompressed bytes to equal the explicitly supplied canonical
bytes. After the COMPLETE status itself validates, immutable-payload type and
gzip/hash/canonical binding checks occur inside the exclusively claimed private
publication tree. The caller cannot supply an arbitrary fallback record. A
recoverable failure in those checks or later publication derives the incomplete
record from that validated COMPLETE input: identity/model/assurance, Boost
version, CTest exit, numeric failure count and E80 observation remain unchanged;
only the schema-prescribed incomplete/null publication fields change. A
filesystem `OSError` maps to `IO_ERROR`; a closed-byte integrity mismatch maps to
`INTEGRITY`. Incomplete publication requires no payload bytes.

## Filesystem contract

The caller owns a fresh, exclusive scratch tree for one invocation. The C++
writer uses `scratch/canonical/<stem>/<stem>.tsv`; this module receives the
distinct, already-existing `scratch/published` path. That path must be absolute,
lexically normalized, a real directory, and contain no symlink component. The
module atomically claims a previously nonexistent `<stem>/` beneath it and uses
only that newly owned directory and its private `.staging/` child.

Within that cooperative caller-owned boundary, files are created exclusively,
closed, reopened, and fully revalidated before rename on the same filesystem.
For COMPLETE evidence the gzip is renamed first. The validated status moves to
a private candidate in the newly owned identity directory, staging is removed,
and only then is status renamed to its final name as the last mutation and commit
marker. Incomplete and fallback status use the same status-last ordering and
must carry null gzip facts. Preexisting identity directories, outputs, symlinks,
mixed identities, traversal, and overwrite attempts fail.

If a recoverable failure occurs after the directory is claimed but before the
status commit, the module removes only its named owned staging files and owned
orphan final gzip, commits the derived truthful incomplete status, then
raises `PublicationError` preserving the packaging failure disposition. Cleanup
or fallback failure also raises and never returns an empty/successful selection.
An incomplete-status failure cannot derive a second fallback; it removes its
tracked owned candidates and newly created empty directories before raising.
Fallback failure likewise attempts exact cleanup of every tracked owned path
while retaining the original publication cause. It does not delete a preexisting
or unrecognized entry and performs no broad recursive cleanup.

This is not a hostile-parent race-proof or crash-proof transaction. Exclusivity
of the scratch parent and process-level orchestration are explicit caller
obligations. A crash can leave an uncommitted directory; selection rejects it.

## Exact upload selection

Selection derives one exact stem from trusted expected identity, opens that
identity directory without recursion, and requires the exact committed shape:

- COMPLETE: `<stem>.tsv.gz`, then `<stem>.status.json`;
- incomplete: `<stem>.status.json` only.

It rereads and identity-validates status. COMPLETE selection rechecks gzip byte
size/hash and decompressed canonical size/hash with `verify_gzip`. Missing or
invalid status, orphan gzip, extra entries (including staging/temporary files),
wrong hashes, symlinks, or foreign/mixed identity fail. Returned paths are the
complete allowlist; callers must not glob.

## Deliberate exclusions

This module does not parse primary stdout, replay 16,384 rows, infer first cause,
prove that a tiny synthetic canonical payload has the scientific TSV schema,
capture commands or provenance, run CTest, compress upstream data, upload,
encrypt, or provide durable `fsync`/crash recovery. Those remain finalizer and
workflow responsibilities. Tests use only disposable
`fs-endpoint-synthetic-*` parents; schema-mandated production-shaped leaf names
inside them are never live upload candidates.
