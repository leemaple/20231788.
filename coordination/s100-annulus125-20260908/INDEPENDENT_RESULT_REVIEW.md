# Independent review of run 34184869227

Reviewed 2026-09-08 against project commit
`03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b`.  This review performed no
compilation, FFT, encryption, decryption, or new sample run.

## Decision

No evidence defect was found that blocks accepting **exactly the one supplied
Linux S100 annulus125 input-and-noise sample** as a numerical PASS under contract
`s100-annulus125-e80-v1`.

This is conditional process evidence, not independent runtime attestation.  It
does not repair or supersede the retained near-unit S100 FAIL, reproduce all of
Table 3, establish a distributional success rate, certify security, or establish
Windows behavior.

## Evidence closure and scalar replay

The five hashes recomputed from the downloaded process record exactly equal the
map in `sample/verification.json`:

| Evidence byte string | SHA-256 |
| --- | --- |
| `program-start.json` | `34ec8316c3a30919fc25f24a8203042bab35f1e77586173d501d2344c41743ec` |
| `program-end.json` | `86c1d2c597d504cec8f6bdd141e3a76c5adc661428a6fe6ebc7541b0beae5fb1` |
| `raw.tsv` | `b0d17e0c2c819b1b59921fd7f8020a5265ae6c70e3b5fa87152a63abfbe4b5fd` |
| `stdout.txt` | `a79645e7bee8d860c2dfc8f75551ceac80822082f60e43dd61ccc65c46a241f4` |
| `stderr.txt` | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |

The raw alias hash and byte count also close: SHA-256
`b0d17e0c2c819b1b59921fd7f8020a5265ae6c70e3b5fa87152a63abfbe4b5fd`,
12,652,749 bytes.  The complete 15-file downloaded-artifact manifest passes
`sha256sum --check`.

Independent invocations of the committed scalar receiver at 180 and 230 decimal
digits both parsed all 16,384 sequential slot rows, all nine exact scale
fractions, both agreement records, all five recomputed maxima/slot indices, all
five recomputed gates, the footer, and process exit 0.  Both returned PASS.  The
independently recomputed maxima are:

| Quantity | Maximum complex modulus | Slot |
| --- | ---: | ---: |
| E0_obs | 3.27082169185998e-25 | 56 |
| E8_obs | 1.46231410388434e-25 | 56 |
| E8_prod | 1.46231410388434e-25 | 56 |
| I8_obs | 1.46231846704339e-25 | 56 |
| A8_obs | 3.16205828983567e-27 | 2527 |

With `T = 2^-80 = 8.27180612553028e-25`, E8_obs is 0.17678292765724 T
(T/E8_obs = 5.65665482098403).  E0, both E8 readings, A8/T/4, and the nonzero
two-slot witness all pass their declared gates.  The maximum per-slot terminal
producer/observer **component** disagreement is
`9.99994775111648015e-129`; it is at most both `2^-120` and the serialized
aggregate terminal agreement (up to the receiver's `2^-300` serialization
tolerance).  This correctly checks component distance rather than substituting a
complex-modulus test for the C++ agreement contract.

The source also checks that every ideal output has modulus greater than `2^-10`.
A separate scalar enumeration found 16,384 distinct ideal `x^256` values, so the
sample is not a zero/duplicate-output construction.

## Process and provenance checks

The start/end records bind the exact contract and source commit, the same direct
absolute executable path, the single `--output` path, the frozen 1200-second
timeout, return code 0, `timed_out=false`, empty stderr, and the exact PASS stdout
banner.  Their timestamps delimit 33.852879 seconds; this is whole-process time,
not a multiplication benchmark.

The retained workflow/job record identifies run 34184869227, attempt 1, exact-tag
push, terminal SUCCESS.  The reviewed workflow has one `run_once.py` launch, no
CTest/repeat/retry path, a fresh exclusive sample directory, and always-run
finalization/upload.  The test source has one setup, one public-key payload
encryption, eight `Mult2` calls before either decryption, and an evaluator
function whose inputs are only the plan and read-only ciphertext.  The workflow
also checked that `src/`, `include/`, `tests/`, and `CMakeLists.txt` at the run
commit have no difference from reviewed science base
`def248a04b7212088e41a72e2239496dcd3e7027`; that same diff is empty locally.
The OpenFHE pin recorded by workflow, TSV, and receiver is
`df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4; the install was a
cache hit.

The runner recorded executable SHA-256
`6c75753ca1536e2abae554b3669c376064dfac9cf3bfc58bcf586cd0b048945a`
before launching the same path.  The executable itself is not in the downloaded
artifact, so this reviewer cannot recompute that digest.  Therefore source-to-
binary binding remains conditional on the retained GitHub workflow/log/artifact
chain.  This is a provenance limitation, not a numerical contradiction in the
supplied sample.

## Receipt and observer limits

Successful execution of the reviewed source implies its in-process assertions
passed for all nine exact scales; receipt phase/family/operation/terminal fields;
fresh null parent; each Tensor-to-prior-round parent link; pair/context/key-tag/
level/lifecycle/component/tower metadata; and terminal RCB receipt object
identity.  The TSV independently exposes and replays the nine exact scales, but
does not serialize every receipt field/link.  Those stronger lineage facts are
therefore source-bound runtime assertions, not independently reconstructible
receipt evidence.

Likewise, terminal E8 observer/producer agreement is independently checkable from
all slot columns.  Fresh `E0_prod` columns are not present, and the 512-bit and
Horner observations are not serialized, so the fresh agreement and those parts
of the aggregate agreement remain source-bound checks.  The explicit
`CONDITIONAL_OBSERVER_NOT_FORMAL` assurance accurately describes this limit.

The `encrypted_runs: 0` event-gate field and finalizer's
`new_encrypted_runs: 0` describe pre-payload/finalizer behavior; neither is a
global historical counter.  The one-sample statement for run 34184869227 is
instead supported by the workflow's one direct launch, the sole start/end pair,
and retained run/job metadata.  Any stronger claim that the tag could never be
deleted/recreated would require the separately retained GitHub run inventory.

## Claim wording review

`RESULT.zh-CN.md` is appropriately explicit that this is one input/key/noise
sample, that the old S100 result remains FAIL, and that Table 3 and security are
unresolved.  One wording should be narrowed before publication: “沿用 S100 的具体
模数、尺度、噪声和电路” can sound like reuse of the same random noise
realization.  The evidence supports “沿用 S100 的具体模数、尺度、噪声参数／采样
机制和电路”; this run necessarily supplies one newly sampled key/noise instance.
This wording issue does not change the sample's numerical PASS.

## Commands run

```bash
sha256sum coordination/s100-annulus125-20260908/experiment-evidence/sample/program-start.json \
  coordination/s100-annulus125-20260908/experiment-evidence/sample/program-end.json \
  coordination/s100-annulus125-20260908/experiment-evidence/sample/raw.tsv \
  coordination/s100-annulus125-20260908/experiment-evidence/sample/stdout.txt \
  coordination/s100-annulus125-20260908/experiment-evidence/sample/stderr.txt
sha256sum --check coordination/s100-annulus125-20260908/EXPERIMENT_FILE_SHA256.txt
python3 -B coordination/s100-annulus125-20260908/replay_annulus125.py \
  --tsv coordination/s100-annulus125-20260908/experiment-evidence/sample/raw.tsv \
  --source-commit 03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b \
  --process-exit 0 --precision 180
python3 -B coordination/s100-annulus125-20260908/replay_annulus125.py \
  --tsv coordination/s100-annulus125-20260908/experiment-evidence/sample/raw.tsv \
  --source-commit 03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b \
  --process-exit 0 --precision 230
git diff --quiet def248a04b7212088e41a72e2239496dcd3e7027 \
  03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b -- src include tests CMakeLists.txt
```
