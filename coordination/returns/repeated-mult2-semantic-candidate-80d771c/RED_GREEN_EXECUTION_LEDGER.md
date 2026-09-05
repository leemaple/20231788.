# RED/GREEN execution ledger

## Identity and classification

Baseline `80d771c52df10bce1c60992b5e0edb4e64f145ca`; OpenFHE `df495ba2e91739a6dc8f1de254fc5a41155ce504`. New implementation bytes are **UNCOMMITTED candidate bytes**, not a new Git commit and not inherited hosted results. This recovery produced actual source edits, two ordered applicable patches and complete files. It did not compile or execute OpenFHE/project cryptography.

## Observed work in this recovery

| Check | Observed result | Evidence / limitation |
| --- | --- | --- |
| Tiny execution-channel check | PASS | Success rc=0 with `success-marker`; deliberate failure rc=7 with `deliberate-failure-marker`; file readback `execution-write-readback-ok\n` |
| Input archive and sidecar | PASS | Exact size/SHA, safe one-root ZIP, CRC, no traversal/duplicate/symlink/encrypted member |
| Input closure | PASS | 127 manifest payloads, all sizes/hashes; exactly the two declared manifest files outside the payload list |
| Mandatory identities | PASS against supplied provenance | Exact TASK bytes; implementation base and official pin; all 40 baseline origins |
| Official expanded source | PASS | 53 source files: sizes, SHA-256, Git blob SHA-1 and pin-bearing provenance paths |
| Prior expanded return | PASS | 24 files, authenticated 22-payload inner manifest plus its two declared self-exclusions |
| Frozen vector arithmetic | PASS | 16 exact Z products, 16 exact W squares, both exact complex deltas; canonical JSON digest and byte-identical source; C++ literal transcription |
| RED-only apply check and apply | PASS, both rc=0 | Actual fresh scratch copy of the supplied baseline |
| GREEN after RED check and apply | PASS, both rc=0 | Actual ordered scratch replay |
| GREEN directly against baseline | Rejected as required, rc=1 | `error: patch failed: CMakeLists.txt:105` / `error: CMakeLists.txt: patch does not apply` |
| Replay versus complete files | PASS | All nine changed/new final files byte-exact; selected final tree 45 files, no unexpected changes |
| Legacy / RED test preservation | PASS | All baseline test bytes unchanged; all RED test bytes unchanged through GREEN |
| CTest source binding comparison | PASS | Exact original 57 name/command/order pairs retained; only requested 58th appended; not generated CTest execution |
| Workflow / warning flags | PASS, source check | One exact push-branch addition; focused target/test added in both jobs; removing only those insertions reproduces original workflow; host-specific warning options |
| Bounded structural / kernel checks | PASS, static only | No private-key operations outside production setup; no private/client use in evaluator scope; no refresh calls in re-entry; original coefficient kernels text-preserved |
| C++/OpenFHE compilation | **NOT RUN** | No compiler invocation, no build directory or compiled artifact delivered |
| Cryptographic RED runtime | **NOT RUN** | No observed cryptographic failure is claimed |
| Cryptographic GREEN runtime | **NOT RUN** | No observed 2^-80 measurement, focused 1/1 or full 58/58 result |
| Linux / Windows hosted jobs | **NOT RUN** | No accounts, network, push, CI dispatch or remote execution |

The standalone compressed official/prior-return ZIPs are not embedded in this input. Their compressed ZIP hashes are producer provenance; they were not independently rehashed/re-CRC'd here. The expanded payload bytes and available inner manifests were independently verified. No external Git fetch/show was performed; the original producer's Git-byte comparison is retained as provenance, not presented as a newly performed command.

An optional `xxd` display command was unavailable (rc=127); it was not a source/build test and did not affect byte verification. Readback and hashes use Python bytes. An initial validation allowlist was corrected to recognize the packet's documented `user-attachment` origins; no input bytes were changed. Neither observation is an algorithmic blocker or a cryptographic result.

## Ordered RED freeze

The RED semantic test, exact literal header, independent oracle, CMake and workflow were finalized before GREEN production edits. `verification/RED_FREEZE.json` stores their final RED SHA-256 values. During pre-implementation source review, an extraneous assumption about every generated prime's exact integer bit length was removed: the frozen nominal 50/55 request, all exact vectors and the 2^-80 criteria were unchanged. No runtime failure had been observed and no GREEN production edit had begun. GREEN does not change the final RED oracle or expectations.

**Predicted RED:** the new test includes `openfhe_2023_1788/repeated_mult2.h`, absent in the baseline/RED tree. A focused/default build is expected to fail at that missing include/API. The compiler-specific diagnostic is unobserved; do not treat the following representative wording as a captured log: `fatal error: openfhe_2023_1788/repeated_mult2.h: No such file or directory`.

This is the permitted missing-API RED, not an intentional bug, expected runtime rejection, metadata facade or renamed old shape probe. GREEN supplies that header and implementation. Whether GREEN actually satisfies the test requires the unperformed authorized runs below.

## Authorized hosted commands — all NOT RUN here

Use pristine OpenFHE 1.5.0 at the exact pin, without DEBUG_KEY, in each independently authorized Linux/Windows environment. Apply RED first in a clean worktree to observe the missing-API RED. Preserve that log. Then apply GREEN to the same RED tree and run:

```text
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
cmake --build build --target relin2_api_contract_test --parallel 2
cmake --build build --target rs2_api_contract_test --parallel 2
cmake --build build --target mult2_api_contract_test --parallel 2
cmake --build build --target add_api_contract_test --parallel 2
cmake --build build --target sub_api_contract_test --parallel 2
ctest --test-dir build --verbose --output-on-failure -R '^repeated_mult2_semantic_two_square_contract$'
ctest --test-dir build --show-only=json-v1
ctest --test-dir build --verbose --output-on-failure
```

The workflow additionally explicitly builds `repeated_mult2_semantic_two_square_test` before its focused invocation, in both jobs. The existing first-precision focus, Pair Add/Sub focus, five API builds and full suite remain intact. Workflow text is delivered but was not dispatched.

## Acceptance remains pending

Both hosts must compile warning-clean, pass focused 1/1 and full 58/58, and retain the exact binding prefix. Each focused and full invocation must execute four fresh root trials with one parseable record for each trial/stage (eight records per invocation) and pass all slot/delta, receipt, profile, re-entry and immutability assertions. Records name only `repeated_mult2_semantic_two_square_contract` and scope `low-N-two-operation-diagnostic`. They report actual d/m1/m2 and rational normalizations rather than guessed prime values.

There are **no captured crypto stage records** in this return. A failed assertion, unexpected warning, malformed output record or anomalous native result blocks acceptance. No new tolerance or substitute probe is authorized by this ledger. Author self-review is not a second independent reviewer.
