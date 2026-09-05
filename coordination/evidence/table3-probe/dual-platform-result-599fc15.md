# Dual-platform Table 3 parameter discovery: verified

Observed and reconciled 2026-09-05 09:30–09:40 Asia/Shanghai. Exact source
`599fc158b6b67d3e752c39cb53069c29dc60fc6f`, pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`, run
https://github.com/leemaple/20231788./actions/runs/33935796427,
workflow_dispatch attempt 1. Both jobs and the run completed successfully.
Later documentation HEADs do not inherit this run as their own source SHA.

| Observed result | Linux job 101223371509 | Windows job 101223371775 |
| --- | --- | --- |
| Focused first-Mult2 regression | 1/1, 0.22 s | 1/1, 0.24 s |
| Focused Pair Add/Sub regression | 2/2, 0.20 s | 2/2, 0.19 s |
| Full existing suite | 57/57, 1.32 s | 57/57, 2.54 s |
| Warning-clean default + five explicit API targets | pass | pass |
| Table 3 standalone discovery target | pass | pass |
| Ordinary A/B families | 18/18 exact | 18/18 exact |
| Separate reserved-P collision control | 1/1 exact | 1/1 exact |

Windows used Server 2022, MSYS2 MINGW64, GCC 16.2.0 and CMake 4.4.2;
Linux used Ubuntu GCC 13.3.0 and CMake 3.31.6. Both explicitly reported
NATIVEINT=64 and MATHBACKEND=4. Windows job ran 01:19:15–01:29:08 UTC
(593 seconds at API resolution); all 20 steps were success. The Windows
probe step's 12 seconds include its build: no cryptographic performance or
unbuffered program-runtime claim is derived from log timestamps.

## Independently reconciled numerical and regression evidence

Root and a separate Codex reviewer independently generated expected family
sequences from the pre-recorded candidate JSON and parsed actual job logs.
Each host has exactly 57 Q/P/QP basis records (19 cases x 3), 12 explicit
official RootOfUnity records, 19 family/partition pass records, and zero
`FAIL field=` records. Every size, ordered modulus/root pair and partition
value matches the frozen candidate and the other host.

- B0..B8 full L: 11..3; A0..A8 full L: 10..2.
- Every context uses family-local dnum=L, alpha=1, partition-count=L, and each
  PartQ has its exact one-limb basis.
- Every ordinary P is `1152921504606584833:4443670208963`; QP is Q||P.
- Replacing Mult0 in full Q with reserved P changes actual P to the removed
  Mult0 `1152921504598720513:100545759574150`; QP matches that control exactly.
- All 12 pre-recorded roots match the independent-from-context official
  RootOfUnity calls. No expected value was recovered from the tested object.
- Both hosts' actual 57 CTest names and order were compared to source CMake
  registration, not only the passing count. Each test name followed by LF has
  SHA-256 `41c97084eb889d520eb558eda0e175a8191a2077374b6de5e1844f8cddafe5e9`.
  Existing NAME/COMMAND/order preservation was already checked against base.

## Retained evidence and checks

| Connector-decoded log JSON wrapper | Bytes | SHA-256 |
| --- | --- | --- |
| linux-job-101223371509.json | 141936 | c610caed3d36df1afff7b549a67b3e7b4144323fd3af1d24b009a406e7143a29 |
| windows-job-101223371775.json | 229950 | c116888daf673e1b749a970e74dd1c607fbf0059cc4c47830b86755ba5ca904b |

These are structured wrappers preserving connector-decoded content, not raw
HTTP bytes. The independent review found no blocking P0/P1 for the discovery
scope. Gitleaks 8.30.1 scanned the evidence directory, approximately 380,248
bytes, with zero findings. Root rechecked source/test immutability and diff
whitespace. No Mac build, NTT or cryptographic execution occurred.

## Precisely completed boundary and next use

**The dual-platform parameter discovery gate is complete.** The pre-recorded
candidate is now backed by actual pinned public direct-context construction,
CRT/NTT precomputation and getter evidence on these two hosts. This evidence
can inform the next reviewed production profile TDD contract.

It is not a production GREEN, a factory/feature-state immutability proof,
h128 key generation, security estimate, client I/O or repeated semantic Mult2
result. No new CTest was added. No key, ciphertext, encryption, decryption,
eight-squaring or 1000-execution experiment was performed by this probe. Root
consistency is not a new primality certificate. Shorter-family dnum=L is an
adapter choice, not proof that every paper level literally used dnum=11.

Do not merge this exploratory branch into mainline, rerun the same discovery,
change existing expected vectors to observed values, or interrupt the three
Pro implementation tasks. Keep exact candidate constants and these logs for
the later production profile integration after the frozen low-N slices and
their independent RED/GREEN/review gates close.
