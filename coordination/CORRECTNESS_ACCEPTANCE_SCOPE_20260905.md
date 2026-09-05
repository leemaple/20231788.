# Current acceptance scope: correctness, not 1000 trials

Authority: on 2026-09-05 the user explicitly said: “不需要 1000 次实验，能判断实现的正确就行”.
This supersedes any older task, handoff, continuation prompt or roadmap that
requires 1000 experiments before completion. Historical evidence and packet
bytes remain unchanged; they are not current authority on trial count.

The full implementation objective remains. Do not replace it with only a
low-N diagnostic or call source review alone proof of correctness. Preserve:

1. Clean-room, exact pristine OpenFHE dependency and public algorithm/API
   correspondence to the paper, with substantive source findings resolved.
2. Production high-precision client input/output and evaluator-only repeated
   multiplication, independently checked against high-precision plaintext
   arithmetic, with exact scale/basis/key-family transitions.
3. Existing frozen vectors, error thresholds, legacy regression/API checks,
   and meaningful boundary/negative/ownership checks; actual RED before fixes.
4. A bounded representative paper-parameter end-to-end validation, including
   the required eight no-refresh squarings and the same-root h=128 secret
   across the implementation-owned ordered parameter/key family. Low-N tests
   or parameter-only probes do not substitute for this execution.
5. Linux and Windows regression/integration evidence tied to exact public
   commits, reproducible commands and retained raw results. Keep heavy work
   off the Mac. Choose additional cases by uncovered risk, not a trial quota.

Do not start or wait for a 1000-run batch. Statistical performance replication,
benchmark rankings and large-sample failure-rate estimates are not completion
gates under this correctness-focused scope. Record observed time/resource
use incidentally where useful, without adding a benchmark project.

Passing bounded tests is evidence for the stated supported domain, not a
universal mathematical proof or cryptographic-security certification. Retain
explicit parameter assumptions and do not infer production security from
HEStd_NotSet diagnostics. Final acceptance must state the tested input domain,
parameter families, operations, numerical bounds and remaining limitations.
