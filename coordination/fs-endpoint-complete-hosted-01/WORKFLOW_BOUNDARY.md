# Hosted complete-finalizer gate activation — 2026-09-06

Root owns this workflow, derived from the existing public exact-source checkout
and the accepted native Python finalizer boundary. The hosted test is authored
independently in the dedicated test worktree at b4d9ac5.

Only the new exact branch codex/endpoint-finalizer-complete-red-20260906 triggers
the new Python-only workflow. The existing dcp-rcb.yml is byte-unchanged and does
not include this branch. No existing paper CTest, CMake target, OpenFHE source,
compiler, encryption, wrapper, artifact upload or live-chain run is invoked.
There is no success-forcing failure policy, retry or second finalizer invocation.

GitHub documents that a new workflow_dispatch file must exist on the default
branch to be triggered initially, while push workflows may run before merging
to default. Therefore this gate uses an exact draft push trigger and does not
merge default to register a workflow. [Official trigger reference](https://docs.github.com/en/actions/reference/workflows-and-actions/events-that-trigger-workflows#workflow_dispatch)
and [push branch filters](https://docs.github.com/en/actions/reference/workflows-and-actions/events-that-trigger-workflows#push)
were opened on 2026-09-06.

The action refs were resolved read-only from their official Git repositories:
checkout v4 -> 11d5960a326750d5838078e36cf38b85af677262;
setup-python v5 -> a26af69be951a213d495a4c3e4e4022e16d87065.
The workflow pins those exact commits, explicitly selects Python3.12, no package
installation/cache, and contents:read. Setup's actual runtime patch version is
observed in the job, not assumed from the local Python runtime. See
[setup-python v5](https://github.com/actions/setup-python/tree/a26af69be951a213d495a4c3e4e4022e16d87065).

Linux uses exact-SHA checkout with persist-credentials:false and a clean-tree
check. Windows independently fetches this public repository's exact SHA into a
fresh named child of RUNNER_TEMP outside the trailing-dot workspace; checkout
uses core.autocrlf=false/core.eol=lf with each native Git exit checked. Windows
executes native Python via PowerShell; it does not claim MSYS behavior.

The test runtime contract is GITHUB_ACTIONS=true, RUNNER_ENVIRONMENT=github-hosted,
RUN_ENDPOINT_COMPLETE_GATE=1 and ENDPOINT_TEST_HOST matching the actual platform,
plus actual canonical GITHUB_SHA/run/attempt. Do not simulate these gates on Mac.
Its non-discovered module is executed explicitly once; the job has a bounded
15-minute limit. The true complete-path RED must be observed on both hosts
before any finalizer GREEN implementation is authored.

Pre-activation review correction: both jobs explicitly reject any runtime not
Python 3.12 and print actual Python/Decimal/libmpdec/zlib environment JSON. Windows
sets ErrorActionPreference=Stop in the test step and immediately captures the
native test exit before returning it. The child is bounded to 600 seconds within
the 15-minute job; the paper CTest's existing 1200-second timeout is unchanged.

A zero synthetic full-row fixture can establish transport/CLI/real pipeline
execution but cannot by itself prove every internal stage was used. Independent
source review and later discriminating negative fixtures retain that obligation.
The separate real-CTest prefix/timeout, MSYS path, once-wrapper/always-upload,
C++ writer interoperability and next single live-chain gates remain pending.

Activation is one authorized source push. Once its result is retained, retire
or deliberately change the exact trigger before any later push to the same
ref. A checkpoint ref outside all workflow filters does not rerun the gate.
