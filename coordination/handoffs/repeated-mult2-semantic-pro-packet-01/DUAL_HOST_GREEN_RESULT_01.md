# Repeated Mult2: genuine dual-host GREEN

Observed 2026-09-05, Asia/Shanghai. **The frozen low-N two-operation diagnostic passes on Linux and Windows.** This is a tested slice, not completion of the paper implementation.

- Branch: codex/repeated-mult2-semantic-01.
- Tested source: d09f15f535f0dbf22ef89b33255e947166cc392a.
- Pristine OpenFHE 1.5.0: df495ba2e91739a6dc8f1de254fc5a41155ce504.
- [Run 33940418513](https://github.com/leemaple/20231788./actions/runs/33940418513), attempt 1, event push, completed/success. Only one source push; no dispatch/rerun.
- Linux job 101236605909: 02:55:11–02:57:53 UTC (162 seconds).
- Windows job 101236605855: 02:55:11–03:04:45 UTC (574 seconds).

## Actual executable results

| Gate | Linux GCC | Windows MinGW64 |
| --- | --- | --- |
| Exact project and OpenFHE provenance | PASS | PASS |
| Warning-clean project build | PASS | PASS |
| Five Relin2/RS2/Mult2/Add/Sub API builds | 5/5 PASS | 5/5 PASS |
| Existing first-Mult2 focus | 1/1, 0.23 s | 1/1, 0.30 s |
| Existing Pair Add/Sub focus | 2/2, 0.12 s | 2/2, 0.19 s |
| Legacy checkpoint before new target | 57/57, 1.19 s | 57/57, 2.68 s |
| New semantic target explicit build | PASS | PASS |
| Repeated two-operation focus | 1/1, 0.70 s | 1/1, 1.09 s |
| Unfiltered full suite | 58/58, 1.66 s | 58/58, 3.57 s |

Linux reused the exact-pin cached official install: its Configure/Build OpenFHE steps were skipped. Windows built and installed pristine OpenFHE during this job. Neither result implies a Mac build. Timings above describe this diagnostic execution only, not a performance benchmark or platform comparison.

The root read-only verifier independently checked both job metadata and complete official job logs. Each CTest inventory contained 58 entries; the original 57 names/normalized commands/order exactly matched tested-source CMake, which is unchanged from RED except the two production wiring lines. The new test is #58 and its real executable command appears in both focus and full logs. An independent Codex reviewer separately verified Linux log starts/pass counts, all API target markers, and numerical records.

## Numerical evidence

Each host emitted exactly eight JSON records in focus and eight in full: four trials times stages 1 and 2 per invocation. Thus 32/32 records were parsed and checked, not inferred from a green badge.

Every record has N=64, batch=16, depth=9, scaling/first/input bits=50/55/100, FIXEDMANUAL, HYBRID, COMPLEX, UNIFORM_TERNARY, HEStd_NotSet, and scope low-N-two-operation-diagnostic. Each invocation contains four distinct root-derived tags; each trial's stage pair shares the appropriate family-1 result tag. All exact scales match:

- d=1125899906843009, m1=1125899906840833, m2=1125899906844161.
- S1=2^200/(d*m1).
- S2=2^400/(d^3*m1^2*m2).

| Invocation | Maximum slot error | Maximum delta error |
| --- | --- | --- |
| Linux focus | 2.5750438251822196e-27 | 4.4468041896633945e-28 |
| Linux full | 8.6567109302555721e-28 | 8.5301073231695580e-28 |
| Windows focus | 3.0483958933624023e-27 | 8.6335021892918749e-28 |
| Windows full | 1.2420908838119371e-27 | 4.6744102256671585e-28 |

All max-slot and distinguishing-delta errors are <=2^-80 (approximately 8.271806125530277e-25). The retained JSON preserves full decimal strings; the table is abbreviated for readability. The independent oracle covers all 16 complex slots after each actual operation. Observed product headroom is positive (stage 1:258 bits, stage 2:211 bits); this is sample evidence, not a universal no-wrap proof or a security estimate.

## Retained and reproducible evidence

GREEN_RUN_d09f15f.json preserves the terminal run/step metadata. GREEN_LINUX_VERIFICATION_d09f15f.json and GREEN_WINDOWS_VERIFICATION_d09f15f.json preserve exact job identities, every numerical record, summaries, and assertions. verify_hosted_green_01.py retrieves completed logs read-only and performs the checks; it neither dispatches nor builds anything.

Original Linux job log: 274,709 bytes, SHA-256 d9ba75b4a80ee0786080309ea21126dc012c42206fd62e1b5ecd1625477901d6.
Original Windows job log: 363,280 bytes, SHA-256 5b11de72dd59438d04dc60cc27ce0ad8f32cc260b4084cd80bda95d6300817c9.
These are original remote-log hashes; local JSON is a selected/structured record, not a byte-identical full-log copy. A timestamped Linux numerical excerpt is also retained.

## Remaining boundaries

The real dual-host RED preceded GREEN; frozen vectors, independent oracle, 2^-80 threshold, and workflow were not weakened. Static Standards and Spec final dispositions contain no confirmed finding; their nonblocking observations and one withdrawn evidence hypothesis remain transparent in GREEN_REVIEW_DISPOSITION_01.md.

Next owner: Codex, for one bounded exact-source external review using complete sanitized context. Recheck shared ZCode quota before allocation; use verified terminal-only Fable 5.1 as the configured substitute when applicable, and record any unavailable route instead of inventing a review or blocking all work. ChatGPT Pro must receive every required file/context and must not be interrupted. External findings require source/test adjudication before integration.

No default-branch merge was performed. Lossless I/O, h=128, three-slice integration, exact receipt/client-I/O connection, Table3 N=32768, eight no-refresh squares, 1,000-trial accuracy, and applicable security/performance evidence remain pending. This run does not prove those outcomes, and does not by itself justify an overall completion time guarantee.
