# First production run: retained FAILURE, not paper acceptance

Observed 2026-09-05. Source `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`,
documentation/inspection HEAD `1853701d8862dabef804021d2dea1899776f38e2`,
branch `codex/paper-scale-implementation-20260905`, pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4.
Tests and CMake remain byte-identical to frozen `448e9d3`; no production,
test, build or workflow change follows the first-run source in this record.

## Actual hosted execution

[Run33971779479](https://github.com/leemaple/20231788./actions/runs/33971779479),
push attempt1, completed FAILURE. The read-only watch ended with exit1;
session84826 is no longer live. Neither job was rerun or dispatched again.

| Host / job | Completed UTC | Pre-paper evidence | Paper outcome |
| --- | --- | --- | --- |
| Linux /101321455160 | 14:31:57 | Library, five API targets and final60/60 PASS | New test compilation FAIL; zero paper runtime |
| Windows /101321455226 | 14:36:38 | Library, five API targets and final60/60 PASS | Paper build PASS; one actual chain; round4 precision FAIL |

The independently selected GPT-5.6 Sol review context audited all old
123 Start/argv/Passed bindings per host, partitioned `1+2+57+1+2+60`.
Live inventories `[57,60]` on Linux and `[57,60,61]` on Windows match the
frozen names/order/argv/CMake backtraces. The root read all 479 lines of
`verify_first_run.py` and reran both host entries at HEAD1853701, exit0.
The ROOT_* JSON files are the actual rerun outputs, not passing FHE reruns.
The parser also verifies original source/run/attempt/pin, semantic numeric
records, build completion, terminal metadata and the distinct paper failures.

## Exact raw retention

Each terminal job log was fetched once. Original decoded connector captures
remain in ignored `artifacts/handoffs/paper-scale-production-first-run-01/`.
They are decoded connector text, not HTTP transport bytes. UTF-8 BOM and
original trailing whitespace are preserved in the tracked evidence logs.

- Linux: 455726 bytes, SHA256
  `75fd6a944d1b50b463e0ff934533e95d8f21cb9d3c7bb7a4cbf0b99f254ab4dd`;
  original connector content and retained LF bytes identical.
- Windows original connector UTF-8: 575557 bytes, 7649 CRLF pairs, SHA256
  `be4bf00801adef0eafddf3238e9f8a6cf231f0ccd2ff7170555a0996f78f6f54`.
  CRLF-only normalization produces 567908 tracked bytes, SHA256
  `c6e009afe774af39fee24e3687323414f3ce5f1d5069084f06fe13b181f985bf`.

The only whitespace-check exclusions for this evidence commit are the exact
two `evidence/LINUX_RAW.log` and `evidence/WINDOWS_LF.log` paths. No broad log
or repository exclusion is authorized. Secret scanning includes both logs.

## Linux: isolated test/toolchain issue

The exact paper-target build command failed on Boost1.83/GCC13.3 with two
`cpp_bin_float<512> -> <1536>` copy_and_round array-bounds diagnostics
(raw decoded lines5472/5485), followed by four make errors and exit2.
There is no additional production-source compilation error in this log.
The test did not start; old regression success is not paper correctness.

Independent Astra source diagnosis and root inspection of official Boost
1.83 sources identify trigonometric argument reduction's 3x precision
conversion. For a 512-bit source, the warned narrowing/carry branch requires
1536 < msb+1 although msb<=511; the widening branch is the applicable path.
This supports an unreachable-branch warning diagnosis, not an observed OOB.
Sources: [copy_and_round](https://github.com/boostorg/multiprecision/blob/boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp#L638-L672),
[reduction type](https://github.com/boostorg/multiprecision/blob/boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp#L1971-L1986),
[trig reduction](https://github.com/boostorg/multiprecision/blob/boost-1.83.0/include/boost/multiprecision/detail/functions/trig.hpp#L106-L151).

A narrow uncompiled proposal uses allocator-backed binary512 root-generation
temporaries with explicit `et_off`, returning unchanged binary512 roots.
[Allocator-dependent ET default](https://github.com/boostorg/multiprecision/blob/boost-1.83.0/include/boost/multiprecision/fwd.hpp#L148-L152)
must be accounted for. No warning suppression, precision reduction, compiler
flag change or Boost upgrade has been applied. Do not push a compiler-only
change merely to repeat the unexplained Windows precision experiment.

## Windows: actual numerical failure and bounded interpretation

There is one Start/argv/Failed process. CTest's later 129-line unprefixed
payload is byte-for-byte the same decoded payload as the live `61:` stream;
it is not a second experiment. Fresh full-slot maximum error is
`3.380628068541526514072840073085484952608435963e-25`.
Fresh codec disagreement is about `4.33e-165`, independent anchor/production
disagreement about `4.95e-102`. Round1–4 maximum anchor errors are about
`2.14850e-25`, `4.21784e-25`, `8.14497e-25`, `1.51983e-24`.
Round4 exceeds `2^-80` by a factor of about1.83736.

The test's Evaluate returns only after eight Mult2 calls and terminal RCB.
Client checks then stop at round4. Thus the eight evaluations returned, but
round5–8 numerical/receipt checks, final binder/decryption/full-slot/witness,
immutability/foreign rejection and cleanup acceptance are NOT REACHED.

An independent GPT-6 Astra context found strong evidence for inherited fresh
error amplification, and also nonzero additional arithmetic error. Root read
the entire audit and 120-line bounded Decimal script and reran it, exit0:
50 original anchors, no replay, no FHE/transform. For slot512, pure fresh-error
propagation to the fourth square has approximate interval
`[1.4940665002e-24,1.5382196576e-24]`, already wholly above the gate and
containing the actual value. This is an analytical explanation, not a proof
that every multiplication is correct. Counterfactual final propagation is
not an observed final result. See `evidence/FRESH_PROPAGATION_AUDIT.md`.

The paper reports a 1,000-run average, not a worst-case guarantee for this
exact input and official public-key noise profile. This distinction does not
silently relax the project gate. No specific faulty operation is proved yet.

## Concrete continuation and role allocation

Codex owns the diagnosis immediately; verified Fable5.1's prior definitive
insufficient-balance failure is not retried without a recovery signal.
Independent mathematical and evidence reviews are separate Codex contexts,
not separate model providers. The benchmark-informed routing skill remains
the authoritative allocation, published at688b3c4.

A complete, sanitized b1b024e-source ZIP and professional independent task
were submitted once to Pro as
[Diagnose Precision Failure](https://chatgpt.com/c/6a9c3081-536c-83ec-97c6-b82cd8295606).
See `../paper-scale-precision-diagnosis-01/TASK.md`, PREFLIGHT and SEND_RECEIPT.
Pro is asked for a minimal signed I_r/A_r/L_r diagnostic on the same chain,
retaining end-to-end errors and all failure gates, with a separate portability
proposal. Do not stop/refresh/resubmit its active response.

No new user decision is required at this stage. After the independent
diagnosis, adjudicate any real defect or disputed criterion before changing
it; do not select keys/inputs, replace truth by fresh-decrypted values, lower
noise/precision thresholds or invent a GREEN result. The next hosted run must
follow a meaningful reviewed change, not ordinary rerun/dispatch. No Mac
compilation, FHE, FFT/NTT, new chain or benchmark occurred in this work.
