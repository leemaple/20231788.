# Minimal correction after actual dual-host compile failure

Run 34006424510, attempt 1, source
`70ba4d74c7231f4a48e78381f2504e3c04dbcff2`, completed FAILURE on both hosts.
Linux job 101414361281 failed its paper build at 02:31:19 UTC; Windows job
101414361066 failed its paper build at 02:36:10 UTC. Both self-test and live
paper steps were SKIPPED. No new numerical or full-chain result exists.

The exact log excerpts and complete lossless raw job logs are retained alongside
`DUAL_HOST_COMPILE_RED.json`. Each host actually passed the same 60 unique old
tests across 123 invocations in groups 1+2+57+1+2+60, and the five explicit API
build steps. CMake 3.31 and 4.4 use different success-summary wording; matching
accepts their actual forms, without changing raw line endings or escapes.
Linux used GCC 13.3.0 and Boost 1.83; Windows installation recorded GCC
16.2.0-3 and Boost 1.92.0-3.

## Reproduced cause and exact change

Both compilers identify the same three `-Werror=range-loop-construct` errors in
`paper_endpoint_transform_negative_contract.h`: Int modulus, Int coefficient
and Scale scale loop variables copy initializer-list elements. The actual
feedback loop is the existing paper-target build after the old checkpoint.
This deterministic compiler diagnostic directly identifies the call sites;
another local reproduction, speculative multi-hypothesis experiment or fuzz
loop adds no information. The user's no-heavy-Mac-build constraint is retained.

The minimal correction changes only those three variables to const references.
Test cases, values, body, failure conditions and all warnings remain unchanged.
An independent Codex source review confirmed initializer-list lifetime and
synchronous lambda invocation safety, as well as the exact CI conditions.
This is a proposed source fix, not yet a passing build.

## Bounded next source verification

The failed run is terminal; it will not be rerun or dispatched. A new source
commit with the three-line correction needs actual compile/self-test evidence.
Retire the old draft push trigger and use only the new exact branch
`codex/endpoint-diagnostic-compile-fix-20260906` for one normal push-triggered
verification. Its two self-test if conditions and inverse live conditions
are updated together. All commands, old60/API order, OMP2, 20-minute self-test
step limit, CTest registration and job limits stay unchanged. The full paper
CTest is skipped on this new exact ref; no next paper chain is consumed.

This is a necessary new-source regression check, not a retry of unchanged
numerical data or a statistical trial. Do not dispatch, force-push, cancel an
external agent, or push the unfinished live branch. Retire this new exact
trigger before another push to it. A further failure must be read and classified
before choosing another change; no automatic repeat or GREEN claim follows.

## Parallel implementation

The independent Python replay author is correcting two source-review findings:
full complex tuple retention and a shared actual scalar kernel with meaningful
nonzero-complex tests. Full 16,384-slot replay remains hosted NOT RUN.
The follow-up source review found another blocker: Decimal absolute values in
maximum selection and the quarter-disk shortcut used the ambient context after
the kernel extraction. Both the lead and independent reviewer identified that
this can round a represented value before comparison. The author is adding a
public-boundary high-significance near-tie RED test before a context-free
absolute-value correction. This unintegrated Python work is independent of the
three-line C++ compile fix and is not claimed accepted or GREEN here.

A separate newly created writer worktree is based on 70ba4d7:
`/Users/lifeng/Documents/20231788-openfhe-endpoint-writer-20260906`, branch
`codex/endpoint-writer-20260906`. Agent `/root/endpoint_canonical_writer` was
requested as GPT-5.6 Sol/high (backend requested-unverified) for a bounded C++
canonical writer/validator and primary-metrics draft with tests authored first.
It owns only new writer files and its coordination notes. It cannot compile,
run crypto/transforms, modify existing main/CMake/workflow, or push. This is the
critical-path gap released by the earlier terminal Pro partial return, not an
interruption or duplicated Pro task. Final independent review is still required.
