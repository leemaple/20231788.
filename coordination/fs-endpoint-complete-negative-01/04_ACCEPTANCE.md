# Accepted three-case negative regression gate

Source `0e3b82bdf71a49f21b497f4ad27a69792e18159a`, run [34018316389](https://github.com/leemaple/20231788./actions/runs/34018316389), attempt 1. Both jobs completed successfully. Full decoded logs and exact job metadata are adjacent.

Observed Linux job 101446077514: 3 tests, 3.826 seconds, OK; Ubuntu 24.04.4, runner 2.337.0, CPython 3.12.14, Decimal 1.70 / libmpdec 2.5.1, zlib 1.3.
Observed Windows job 101446077680: 3 tests, 3.462 seconds, OK; Server 2022 10.0.20348, runner 2.337.0, CPython 3.12.10, Decimal 1.70 / libmpdec 2.5.1, zlib 1.3.1.

The logs record the three named public-CLI cases as ok. Joint inference from those executed assertions and the frozen source order:
- count 2 -> 3 reaches primary/sidecar INTEGRITY binding rejection before scalar replay;
- E0.real=1 reaches fresh conditioning preflight, 255/128 > 3/2, then CONDITIONING;
- nonselected signed A8.imag=1e-80 reaches complete 16384-row replay and then signed-tuple INTEGRITY reconciliation rejection.
No internal stage trace was instrumented, so the stage attribution is not a separate runtime counter. Each test actually checked return 8, silent stdout/stderr, unchanged inputs, exactly one status and no gzip, retained count 2 / E80 FAIL / Boost 108300, and the expected incomplete cause.

Independent evidence attribution: /root/endpoint_writer_review read exact logs, metadata and source, found no conflict. This is a separate Codex context, not another provider; the reviewer ran no new test/remote operation.

This gate covers synthetic data rejection only. It does not prove a C++ writer -> Python end-to-end result, live chain, cryptographic improvement, q-only/near-boundary discriminator, arbitrary filesystem failure, crash safety or service upload. The last real paper E80 result remains FAIL. The accepted negative inputs, positive helper and numeric boundaries are unchanged.
