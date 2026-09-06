# Hosted complete-finalizer test boundary

Status: seam frozen before authoring the test on 2026-09-06.

The sole public seam is one external invocation of the actual Python 3.12
`paper_endpoint_finalizer.py finalize` CLI. The test supplies one actual
16,384-row canonical synthetic TSV plus its matching CTest61 primary stream.
It does not call `finalize_endpoint` directly, replace an internal collaborator,
or invoke the `select` CLI. After finalization, it uses the accepted public
`select_endpoint_uploads`, status decoder, and gzip verifier to inspect the
committed result.

The fixture is the existing reader-integrated zero residual construction:

- explicit `scope=synthetic`;
- the exact current `GITHUB_SHA`, `GITHUB_RUN_ID`, `GITHUB_RUN_ATTEMPT`, and
  `ENDPOINT_TEST_HOST` identity;
- numeric failure count 2, E80 `FAIL`, CTest shell status 8, and Boost 108300;
- `fresh768=terminal768=2^-854`, matching the sidecar's actual C=3 allowance
  model;
- exact 16,384 ordered zero rows;
- ASCII-LF canonical TSV on both hosts;
- LF primary transport on Linux and CRLF primary transport on Windows.

Before any full fixture allocation, the test requires all of:

```text
GITHUB_ACTIONS=true
RUNNER_ENVIRONMENT=github-hosted
RUN_ENDPOINT_COMPLETE_GATE=1
ENDPOINT_TEST_HOST=linux|windows
```

It also requires CPython 3.12, a lowercase 40-hex `GITHUB_SHA`, positive-decimal
run/attempt values, and agreement between the requested host and `sys.platform`.
Missing or invalid gates fail explicitly; there is no skip, expected failure,
or local-host substitution.

Acceptance requires CLI status 8 together with a COMPLETE committed status,
the exact two-path gzip-then-status allowlist, the retained count/E80/Boost
facts, exact canonical/gzip sizes and hashes, and a verified gzip round trip to
the original TSV bytes. A traceback, `NotImplementedError`, status-only
fallback, zero exit, extra publication path, or altered canonical input fails.

This file is intentionally named `hosted_paper_endpoint_finalizer_complete.py`,
so routine `test*.py` discovery on the Mac does not select it. It must not be
run locally, even with fabricated hosted variables. The initial actual RED and
all full-replay evidence are owned by a later authorized hosted workflow.

Root intake limits the CLI child to 600 seconds, within the separate 15-minute
hosted job limit, so a child timeout can be reported before the runner kills the
job. This changes no paper CTest timeout. The workflow records the actual Python,
Decimal/libmpdec and zlib environment in its job log before starting the test;
the test requires the finalizer CLI itself to remain silent on a normal return.
The recorded default Decimal context is environment information, not a claim
that replay uses that default: replay's required local context is independently
fixed at precision 256 and ROUND_HALF_EVEN.

Before activation, independent review identified Windows path-length risk from
the default temporary directory and long prefix. Root selects the actual hosted
RUNNER_TEMP explicitly with the shorter fs-endpoint-synthetic- prefix. This is
a fresh temporary child, not the runner root or an existing project directory;
no Windows long-path policy is assumed. Actual hosted availability is still a
runtime observation, and an earlier filesystem error is not the intended RED.

The zero fixture does not by itself prove that the finalizer called the full
replay rather than manufacturing equivalent output. Source review must verify
the required primary binding, `replay_sidecar`, and metric reconciliation calls
on the same parsed sidecar before accepting GREEN.
