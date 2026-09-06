# Implementation delivery — 2026-09-07

## Engineering acceptance

The requested constructive t=2 Double-CKKS algorithm implementation is delivered on pristine OpenFHE1.5.0, with bounded numerical correctness under the explicitly versioned `experimental-s116-d56-b58-v1` profile. All named operations and the client/evaluator end-to-end path have implementations and executed verification. This is **not** a claim of exact Table3 parameter reproduction, general all-key accuracy or deployment security.

The public repository's default branch is `cleanroom/reimplement-mult2-20260831`. The working implementation and evidence were advanced there by a normal fast-forward, preserving history. See[中文使用与复现说明](REPRODUCE.zh-CN.md) for commands, input/parameter limits, result interpretation and public API entry points.

## Requirement-by-requirement closure

| Required result | Evidence inspected | Disposition |
| --- | --- | --- |
| DCP, RCB, Tensor2, Relin2, RS2, Mult2, pair Add/Sub | Current public declarations/definitions, CMake test registrations, retained independent-oracle/red-green lineage and actual dual-host60-test logs | Implemented and verified at the stated boundaries |
| High-precision client encryption/decryption; evaluator without private key or intermediate refresh | Current `high_precision_client_io`, `repeated_mult2` APIs and tested `RunCandidate`/`Evaluate` call boundaries | Implemented; exact logical scales are distinct from compatibility metadata |
| Same-root h128, N32768,16384 slots, eight uninterrupted squarings | Scientific run34055816234/attempt1/source2b8b349; raw logs and independent numerical/runtime reviews | Both hosts PASS; no retry/key selection or1000-run batch |
| Frozen E80 error gate and independent plaintext/oracle checks | Full-slot endpoint component assertions, ten sparse-CRT/Horner anchors per stage, witness/codec/domain/headroom and wrong-scale checks | Linux final max2.5905e-26; Windows3.4806e-26; both below8.2718e-25 |
| Pristine dependency, clean-room isolation, TDD, independent review, Git history and Mac responsiveness | Pin`df495ba2e91739a6dc8f1de254fc5a41155ce504`; accepted source/review and fail-first records; exact hosted provenance; normal pushes | No quarantined implementation used; no Mac compilation/cryptographic replay in delivery |
| Final default-branch regression/wiring | Run34057018442/attempt1/source1552ebe; complete status, both job logs, root receipt and default-only CI review | Both60/60; five API builds each; all archived experiment/publisher steps skipped |
| Clear reproducible handoff | `REPRODUCE.zh-CN.md`, its bounded independent review, existing exact workflow and raw commands | Guide reviewed; shell block syntax and8 local links checked without another experiment |
| Daily07:30 PDF/Telegram reporting | Standing reporting workflow and existing recurring task | Continues as a reporting-only obligation; not permission to restart completed engineering or repeat experiments |

The separate[final requirements audit](coordination/precision116-eight-square-return-01/FINAL_REQUIREMENTS_AUDIT.md) found no missing named algorithm or client/evaluator seam. Root additionally checked the current production definitions and registered test names, read the final runtime and guide reviews, verified the completed default run, and reconciled the exact logged outcomes. The audit's formerly pending default-CI item is closed by[actual final-CI evidence](coordination/precision116-eight-square-return-01/DEFAULT_PROMOTION.md).

## Limitations that remain true

The original S100/Table3-profile test is still FAIL. The accepted experimental profile changes the initial scale and three primes; it demonstrates the paper's method in a different supported parameter regime, not the success of the original parameter experiment. The scope does not include proving all random keys/noise draws, arbitrary inputs/depths, all-slot intermediate nonwrap, Tensor/Relin universal lift safety, statistical failure rates, benchmarks or128-bit security. These are disclosed limits, not silent substitutes for the required operations or frozen numerical gate.

## Protected local state and continuity

The canonical local reporting worktree remains at8c06b48 with its existing report/skill edits because Git safely refused a local fast-forward. No stash/reset/staging of those edits was used;36 files were verified byte-identical. The remote default and clean engineering worktree carry the delivered code. Do not overwrite those local edits to make the reporting checkout look synchronized.

No code/test/CI task remains running or requires a new experiment. Subsequent scheduled runs should report the completed state and any genuinely new user-authorized work, preserve current evidence and daily-delivery idempotency, and avoid manufacturing further engineering tasks from historical pending entries.
