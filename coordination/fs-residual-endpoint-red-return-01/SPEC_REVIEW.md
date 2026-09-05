# Independent Spec review — FS-RESIDUAL-ENDPOINT-01

Disposition: **ACCEPT FOR DIAGNOSTIC API/LINK RED ONLY.** No substantive specification blocker or unauthorized implementation was found in this bounded independent pass. Numerical GREEN, a library accuracy certificate, and original E80/project acceptance remain unestablished.

The active workflow and engineering/external/model-routing references, root TASK, frozen contracts, current scope/seams, both complete current paper test/helper files, both original proposals, and returned specification/test plan/patch were read. This pass did not read the parallel Standards review. Requested Astra/high is not backend attestation or provider diversity. Source baseline is `9f6c8eae`; current `308d4f5` is the stated documentation-only successor. Package-wide intake/provenance and complete paper/source audit remain the root review's separate responsibility.

- **SPEC-I01 — Conditional mathematics is suitable for this diagnostic boundary.** TASK:47 says “Cross precision alone is not an interval certificate.” `pro/ENDPOINT_SPEC.md:98–117` preserves that limitation, requires actual conditioning and Horner string-conversion checks, and distinguishes missing model support from failure. Independent exact scalar checks below confirm the stated D/H/P majorants, slot mapping, scale recurrence and terminal conditioning cap. Residual and replay expressions (`:124–137,175–189`) consistently propagate endpoint, power, subtraction and serialization error. They remain conditional on the declared arithmetic/trigonometric premises; these checks do not prove Boost satisfies them.
- **SPEC-I02 — Original failure and evidence boundaries are preserved.** TASK:50 requires finite E80 failure to “propagate nonzero CTest status.” The specification's independent status hashes, strict rows/compression, incomplete states, pre-Require publication and preserved CTest return (`:195–325`) address that requirement without self-hash recursion. `:167,241–243,331–333` keeps original E80 and A=`NOT_ADOPTED` separate. Replay uses a justified smaller fixed disk, not an unbudgeted assumption that 110 digits are exact.
- **SPEC-I03 — RED is minimal and discriminating.** TASK:55–57 permits declarations/assertions and pre-setup dispatch while deferring helpers. `pro/RED.patch:1–211` changes only that dispatch and a new 173-line test contract: seven undefined functions, no implementation/stub/CTest/CMake/workflow change. Fixtures discriminate normalization, sign/map, exact comparison, allowance precedence and canonical decimal errors. Missing malformed-helper execution coverage is explicitly deferred in `pro/TEST_PLAN.md:80`; it must be supplied before claiming observer GREEN. Actual linkage remains NOT RUN.

No reviewer-requested amendment is necessary before the bounded hosted RED. Conditional prerequisites and unexecuted future evidence tests remain gates, not completed work.

## Actual bounded checks

Executed independent inline `python3` standard-library `Fraction`/integer arithmetic, exit 0, without importing project code or running the returned checker. The computation used `u=2^-p` and the inequality `(1+x)^n <= 1/(1-n*x)` for nonnegative `nx<1`:

| Check | Actual result |
|---|---|
| `(1+3u/(1-3u))/(1-16*128u)-1 < 2^12*u` | true, p=512 and 768 |
| `(32768+4)*128u/(1-(32768+4)*128u) < 2^24*u` | true, both precisions |
| `2^256*255*16u/(1-255*16u) < 2^270*u` | true, both precisions |
| Independent rational recurrence S8 equals closed product | true |
| `32768*(Qbase-1)/(2*S8) < 2^14` | true; formal cap, not observed fresh/terminal C |
| `{((5^s mod 65536)-1)/2}` equals all even bins | true for s=0..16383 |
| Selected roots unique and disjoint from negatives | true |
| `256*(3/2)^255 < 2^158` | true |
| `2^158/10^109 < 2^-128`, whereas `2^263/10^109 > 2^-128` | both true; conservative serialization-only illustration |
| Sparse fixture allowances equal D512+J768 and D768+J768 | true |
| Butterfly / real-multiply / real-add counts | 245760 / 983040 / 1474560 |
| Canonical field byte count | 119 |

The smaller-disk serialization check is illustrative, not a substitute for the prescribed actual per-field quantum, model and total-budget guards. Analytically, Q adds the radius-2 power Lipschitz term to P; E/I/A and identity bounds are triangle sums. Replay E0 reconstruction adds the fresh endpoint, residual, spelling and Decimal-addition errors before applying the radius-3/2 Lipschitz constant; replay I/A additionally count their two powers and subtractions. No omitted cancellation-dependent relative-error assumption was needed.

Executed `git -C <clean-root> apply --check <return>/pro/RED.patch`, exit 0, followed by read-only `git apply --stat`: two paths, 193 insertions and one deletion. No patch was applied by this reviewer. No C++ configuration/build, numerical codec, transform, FHE, external call, CI, historical checker, Git mutation or engineering write was performed.
