# Finalizer public seam and authoring sequence

Root owns the finalizer in the main clean-room worktree at base b0fae5dcfb70a71d462e6354d370fa385fdb5475.
The adopted endpoint specification sections 5–8 and wrapper INTEGRATION_CONTRACT
define the behavior. The user delegated ordinary internal seam choices.

The public data/filesystem operation is:

`finalize_endpoint(primary_log, identity: PublicationIdentity, *, ctest_exit_code,
capture_exit_code, expected_scope, canonical_parent, published_parent,
timed_out=False) -> PublicationResult`.

It consumes the actual bounded primary log bytes through the independent parser,
not externally supplied parser objects or replay summaries. Exact shell statuses
are integers 0..255. `timed_out` is an explicit observed Boolean at this seam;
the future CLI must obtain it from a verified CTest-owned transcript format, not
guess from exit code. Both child parents and the primary log share one normalized,
exclusive caller-owned scratch root; no broad working directory is accepted.

The staged TDD sequence first establishes incomplete status-only finalization,
then primary facts and capture first-cause retention, then complete canonical
binding/replay/compression, then the exact `finalize` and `select` CLI described
in the wrapper contract. Complete finalization must directly call replay_sidecar
on the same parsed sidecar before reconcile_replay and pack/publish. It must not
accept a caller-provided summary or silently waive that check.

Only concrete boundary exceptions are translated to truthful status. Unexpected
programming exceptions fail loudly with a traceback. Invalid invocation or an
unusable publication parent fails before file publication. No fabricated empty
success is permitted. Existing E80 count/disposition and exact CTest status are
retained when the primary reader establishes their complete record; no missing
fact is filled with zero. Capture failure prohibits COMPLETE regardless of a
complete-looking retained prefix.

Local fixtures exercise actual readers/status/gzip/publication through this
public operation with small disposable synthetic files. Do not mock internal
collaborators or run full16384 scalar replay on Mac. The complete replay fixture
is hosted-only and is a required gate, not removed for local convenience.
The complete pipeline and CLI remain pending until their own actual evidence.
