# Repeated semantic RED integration gate 01

## Inputs and authorized delta

The original Pro return is preserved byte-for-byte at receipt commit `0ab6f89`.
Original RED patch SHA256:
`4fdcdeb9aa05641fbac8f7187127e4c350ad320c38cfe8eb289e71b5b6f7bc62`.
Original GREEN patch SHA256:
`0c9f118fb1034dbdcd3cb01f8da802595c5e0f0eb54f003622eb6d5514b87a27`.
Root applied only RED to the engineering-equivalent branch, preserving later
coordination documents. No new production header/source is present.

Independent CI review found that the candidate's default semantic build would
fail at its legitimately absent public header before the old tests and five
API targets could be observed. The pre-runtime integrator delta is restricted
to CMake and workflow observation order:

1. The new target is EXCLUDE_FROM_ALL but remains registered as CTest #58.
2. Both jobs build the unchanged default targets, Relin2/RS2 API targets, run
   old precision 1/1 and Pair Add/Sub 2/2, then run the legacy suite with an
   exact exclusion for #58 (expected 57/57).
3. Mult2/Add/Sub API targets build before the new semantic target, completing
   all five APIs at the RED checkpoint.
4. Then record CTest JSON, explicitly build the semantic target, run focused
   #58, and retain the final unfiltered full suite. Missing-header compilation
   must fail the job normally. No continue-on-error or swallowed error exists.

This does not reduce the full suite or change an oracle. The original package
and its original RED_FREEZE.json are not rewritten to pretend the delta came
from Pro. GREEN still applies after this integrated RED; its only permitted
CMake difference is the two-line production target_sources connection.

## Static preflight actually performed

At 2026-09-05 10:06 Asia/Shanghai, Root parsed the real YAML with Ruby/Psych and
checked the step order above in both jobs. Independently compared the original
57 normalized NAME/COMMAND bindings in exact order and one unique appended
binding. Three new test file bytes match the original RED freeze. All existing
include/src are unchanged; repeated_mult2.h and repeated_mult2.cpp are absent
from the engineering paths. `git diff --check` passed. Original GREEN
`git apply --check` passed without actually applying it.

Integrated RED file SHA256 values:

| File | SHA256 |
| --- | --- |
| CMakeLists.txt | c0f03945d54715639f655fe31acc82994047381b6b484e4ac70b137b45ee7f4a |
| .github/workflows/dcp-rcb.yml | e35b2866499d5bcb183706a0bba62b9f1d54922fae1c57aae538d37fb3ca0480 |
| tests/repeated_mult2_exact_vectors.h | e065a934ff9104d1deaa19874b0fa86f3e9ee573693b6fb96a747dde1aad3d43 |
| tests/repeated_mult2_semantic_oracle.h | 204d1704ad2b164691acc98bc354a80b8ff3278ade53907c1d2bec5a9126d17d |
| tests/repeated_mult2_semantic_two_square_test.cpp | b3eb681401bade58cb1a81091c33c4307a4595e226531d5a79580f9a5a7d86a3 |

## Hosted acceptance rules, not current results

No Mac C++ compile, cryptographic execution or benchmark is permitted.
Actual hosted run/job/commit identities belong in the subsequent receipt.
Do not call queued work or missing-executable CTest output valid RED.

For valid RED, both hosts must pass default warning-clean build, all five API
builds, old focus 1/1, Pair focus 2/2 and old full 57/57, then fail explicitly
building the new semantic target at the missing header/API. If compilation
unexpectedly succeeds, an actual new-seam failure must be diagnosed rather
than presumed. Configure/fixture/loader/infrastructure failure is not RED.
CTest show-only may not resolve the not-yet-built new executable; this is an
inventory checkpoint, never a test pass or missing-header compiler result.

Only after preserving both hosts' genuine RED evidence may GREEN be applied.
Do not change the three RED test hashes or workflow, lower 2^-80, change the
frozen dyadic vectors, or substitute a shape/staged-equality oracle.
GREEN requires exact-SHA dual-host warning-clean/API/focused/full58 evidence,
eight parseable {trial,stage} records per invocation and independent review.

This gate neither accepts the GREEN implementation nor proves the full paper.

## Actual RED dispatch

The exact five engineering paths were explicitly staged (two modified build/CI
files and three added test files). Staged diff/check passed; Gitleaks 8.30.1
scanned 54057 diff bytes with no leaks. They were committed separately as
`7399db55b799a166aee9b72b8f89bcded373b540` and pushed once at approximately
2026-09-05 10:09 Asia/Shanghai. The branch push filter triggered, without any
manual workflow_dispatch or rerun:

- Run <https://github.com/leemaple/20231788./actions/runs/33938285334>.
- Event push; created `2026-09-05T02:09:49Z`; head SHA exactly `7399db5...`.
- Linux job `101230431194`; Windows job `101230431312`.
- Initial read: both in_progress. Linux provenance/configure succeeded and the
  default project build was active; Windows checkout succeeded and toolchain
  installation was active. No new test result was yet observed.

The expected failure is not yet observed and must not be reported as RED
success. Later documentation commits are not the tested source SHA. Do not
rerun/dispatch this already-running run or apply GREEN until the exact evidence
gate above is closed on both hosts.
