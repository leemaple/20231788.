# First-modulus constructor boundary: actual dual-host RED accepted

2026-09-05, Asia/Shanghai. Codex accepts the actual missing-rejection RED at
engineering source `f1ea03f35a6a553d65db30c93e771738f6bc0e1d`, branch
`codex/lossless-io-implementation-01`, not a compile or infrastructure failure.
The firstModSize55 P2 is reproduced, not yet fixed. No GREEN was written before
this acceptance. Read FIRST_MODULUS_RED_DISPATCH_01.md for the frozen test.

[Automatic push run33952773643](https://github.com/leemaple/20231788./actions/runs/33952773643),
attempt1, created07:30:59Z. Linux101270511248 completed07:33:59Z;
Windows101270511113 completed07:40:53Z. Both failed only the new focused test.
FIRST_MODULUS_RED_RUN_FINAL_01.json retains the actual complete metadata.

| Actual gate | Linux | Windows |
| --- | --- | --- |
| Old precision focus | 1/1, 0.18s | 1/1, 0.25s |
| Pair focus | 2/2, 0.16s | 2/2, 0.20s |
| Legacy suite | 57/57, 1.28s | 57/57, 2.62s |
| New target and five explicit APIs | Built successfully | Built successfully |
| New focus | 0/1, 0.40s | 0/1, 0.29s total (test0.28s) |
| Full58 | Skipped after focus failure | Skipped after focus failure |

The project builds were warning-clean. Linux restored the exact-pin
native64/backend4 cache; OpenFHE configure/build were skipped due to that hit.
Windows actually configured/built/installed the pristine pinned OpenFHE.
Do not describe the Linux run as a fresh OpenFHE rebuild. Noncompiler
Node/action deprecation notices are not C++ warnings or fixture failures.

## Observed behavioral counterexample

The exact raw-log chain is:

- Linux line2657: unchanged A positive contract PASS; line2658: flushed
  actual_first_bits=56 fixture ready; line2660: required rejection was accepted.
- Windows line3547: unchanged A PASS; line3548: flushed actual56 ready;
  line3550: the same precise failure.
- In both, the exception diagnostic is
  `required rejection was accepted: HighPrecisionClientIO: unsupported diagnostic Q basis`.
- The later unnumbered copies of the A/ready/error output are CTest failure
  transcript replay, not another execution or keypair.

The original A marker reports one keypair, one evaluation-key generation,
32 malformed-key rejections and the frozen source/pin/N64/S16/gap2/profile.
Both independent auditors and root checked all fresh/product/delta errors
against 2^-80, Horner/cross-precision disagreements against 2^-120, exact
S1=2^200/(1125899906843009*1125899906840833) and positive centered headroom.
Max product error was 3.03286191286665493188920735452596709796007384e-28
on Linux and 6.00591028896424363954183824261814288694546093e-28 on Windows.
These are A positive observations before the deliberate constructor failure;
the entire focused test did not pass.

Root independently matched all 61 actual numbered starts, commands and results
per host: 60 old passes and one new failure, plus both live JSON legacy57
bindings against exact-source CMake. The original 57-entry normalized ledger
SHA-256 remains 3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b.
Initial read-only root parser attempts stopped on padded CTest numbers and
Windows backslash tokenization. Only the parser was corrected; the full
122-invocation check then passed. No source, test or log was changed for it.

## Retained evidence

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| FIRST_MODULUS_RED_LINUX_JOB_01.log | 205248 | ed65028835daf3a5cfdf02e5547c9f584aa990aa0e5249a1150b213df6e2262c |
| FIRST_MODULUS_RED_LINUX_VERIFICATION_01.json | 62268 | 9538c2c1f1e2f86941ea6f07f0d266ed9d39407065fef24d395c2d5d58348e47 |
| FIRST_MODULUS_RED_WINDOWS_JOB_01.log | 288685 | 8b9f334b013c4a4f572f55bb3c2d120ca345f1d8b61aa0af86b91e6cef84a7f6 |
| FIRST_MODULUS_RED_WINDOWS_VERIFICATION_01.json | 173575 | b8b7b9ecd64f228f1ff5340aa0593794a5b89b9d0e553c632670b9b7058dc389 |

FIRST_MODULUS_RED_ROOT_VERIFICATION_01.json records root's independent result.
The Linux 2731-line log exactly preserves the connector-decoded UTF-8/BOM and
209 trailing-whitespace lines. Windows's complete decoded UTF-8 raw log is
292251bytes SHA b4c91a34487eadd707e9355b40421beedd424fdff0710c5595328bfbd699b289;
only3566 CRLF pairs were normalized to LF for the checked-in file, retaining BOM.
The exact original string is retained in ignored
artifacts/handoffs/io-first-modulus-red-01/windows-api-capture.json
(323832bytes SHA c15df5d52846ba27777e0da20a0462710cacf1e7aca3ee4a448a65de4420cfc9).
Root independently verified that this sole transformation produces the saved LF
bytes. These hashes concern connector-decoded UTF-8, not HTTP transport bytes.

## Next authorized vertical step

Publish this actual RED evidence first. Then add only the actual first-Q
GetMSB()==55 condition to the existing safe Q-shape constructor guard,
preserving its project diagnostic. Keep the complete f1ea03f test, oracle,
thresholds, CMake, workflow and other production behavior unchanged. Review,
publish the engineering candidate and inspect its actual automatic dual-host
focus/full58 before closing the P2. The later B clone/shared-Params rejection
remains a distinct RED/GREEN slice. No default-branch merge is authorized here.

The 1000-run requirement remains cancelled. This bounded diagnostic is evidence
of one concrete profile-validation defect and its existing numerical path,
not complete paper-parameter/family/eight-operation implementation or security.
No local Mac C++ build, cryptographic run, NTT, benchmark, rerun or dispatch was used.
