# Writer implementation intake after actual dual-host linkage RED

Root base56440558bc89afc20b597e4b1b6b798e2aa59972; that checkpoint and its
actual RED logs are pushed. Source9f12e1759055a0c4c0f9a794278768a6101f277e,
run34009619514/attempt1, failed on both hosts only at the expected three
undefined writer public functions. Each host retained old60/123 invocations
and five explicit API successes; no self-test or normal paper chain executed.
Read RED_DUAL_HOST_RESULT.json and lossless logs for the exact observed facts.

Root now imports the independently reviewed source-only writer implementation
from the dedicated worktree, byte-identical SHA-256
`3fc99b052868b252aa925cb7c350e24036eaf4f54be8e9340c3bdd9a502e1ac9`,
and adds this one source to the existing EXCLUDE_FROM_ALL paper target. Header
`c251fab8d526b6734036f97e47d67f36fcaecfd692f9cc9f22dcf0e32efc42b9`
and test `0621395297f16bc6fbfb85e17724741158563d0978baa7f65d8f743317adb1b0`
are unchanged from the actual linkage RED. Existing main/normal body and all
frozen production/oracle/test-entry properties remain unchanged.

The one-shot draft trigger is now `codex/endpoint-writer-selftest-20260906`.
It permits hosted compile and one synthetic self-test per host, and still
excludes the normal encrypted paper CTest. The old writer-link-red trigger is
retired. No dispatch/rerun/cancel or Mac compilation is performed. Actual
behavioral GREEN remains pending a new exact-source result; source review is
not substituted for execution. Conditional symlink fixture coverage is only
attempted when the host permits creation, and generic self-test PASS does not
by itself prove that conditional fixture was exercised on both hosts.

This intake adds no live publisher call. Full post-cleanup main wiring must
wait for the independent primary reader/reconciler and failure-preserving
status/transaction/wrapper/upload pipeline. The next normal one-chain-per-host
observation is not consumed by this draft run. E80 remains the prior FAIL;
original thresholds and A NOT_ADOPTED remain fixed.
