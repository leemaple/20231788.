# Repeated Mult2 hosted RED result 01

Observed and independently adjudicated 2026-09-05 10:53:21
Asia/Shanghai. This receipt is limited to the exact hosted RED source commit.
It does not inspect or accept any subsequently prepared GREEN production bytes.

## Verdict

**PASS — genuine dual-host missing-API compile RED. GREEN implementation may
now be applied, corrected, and tested, but no GREEN result is established.**

Both hosted jobs checked out the same exact RED commit, completed every legacy
checkpoint and all five public-API builds, then failed only when explicitly
compiling the new repeated-Mult2 semantic target. Both compilers reported the
same absent public header:

```text
openfhe_2023_1788/repeated_mult2.h: No such file or directory
```

This is the missing-API RED authorized by the frozen task. It is not a runtime
semantic failure, a CTest missing-executable result, an infrastructure failure,
or a deliberate production defect. The new focused test and the unfiltered
58-test suite did not run.

## Exact hosted identity

- Event: `push`
- Branch: `codex/repeated-mult2-semantic-01`
- Source commit:
  `7399db55b799a166aee9b72b8f89bcded373b540`
- Run: [33938285334](https://github.com/leemaple/20231788./actions/runs/33938285334)
- Attempt: 1
- Created: `2026-09-05T02:09:49Z`
- Final status/conclusion: `completed` / `failure`
- OpenFHE pin verified by both jobs:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Linux job:
  [101230431194](https://github.com/leemaple/20231788./actions/runs/33938285334/job/101230431194),
  `2026-09-05T02:09:51Z` to `2026-09-05T02:12:12Z`, failure.
- Windows MinGW64 job:
  [101230431312](https://github.com/leemaple/20231788./actions/runs/33938285334/job/101230431312),
  `2026-09-05T02:09:51Z` to `2026-09-05T02:18:35Z`, failure.

Each job's provenance step printed the exact source commit, run ID, and attempt:

```text
PROJECT_SOURCE_COMMIT=7399db55b799a166aee9b72b8f89bcded373b540 GITHUB_RUN_ID=33938285334 GITHUB_RUN_ATTEMPT=1
```

Later documentation-only branch HEADs are not the tested source identity.

## Gate-by-gate result

| Hosted gate | Linux | Windows | Accepted observation |
| --- | --- | --- | --- |
| Project configure and provenance | PASS | PASS | Exact RED SHA and pinned OpenFHE commit |
| Warning-as-error default project build | PASS | PASS | New semantic target remained excluded from default build |
| Relin2 API target | PASS | PASS | Explicit target built |
| RS2 API target | PASS | PASS | Explicit target built |
| Existing first-Mult2 focus | 1/1 PASS | 1/1 PASS | Existing precision contract preserved |
| Existing Pair Add/Sub focus | 2/2 PASS | 2/2 PASS | Both pair-to-first-Mult2 tests preserved |
| Legacy suite excluding new binding | 57/57 PASS | 57/57 PASS | No old regression observed |
| Mult2 API target | PASS | PASS | Explicit target built |
| Add API target | PASS | PASS | Explicit target built |
| Sub API target | PASS | PASS | Explicit target built; five-API checkpoint complete |
| CTest source inventory | PASS | PASS | 58 names; exact original 57 prefix retained |
| Explicit new semantic target build | EXPECTED FAIL | EXPECTED FAIL | Missing `repeated_mult2.h` from the semantic test include chain |
| Focused new semantic CTest | SKIPPED | SKIPPED | Not a runtime RED result |
| Unfiltered full 58-test suite | SKIPPED | SKIPPED | No 58/58 claim |

Direct parsing of each hosted `ctest --show-only=json-v1` log found 58 test
objects. On both hosts, the original 57 names, commands, and order normalize to
SHA-256
`3527832e2d46591c46a93d3cb96d5469a9362ec4ca1ba39c8ed0587964e77f8b`.
The last object is named exactly
`repeated_mult2_semantic_two_square_contract`. Because its executable had not
yet been built, CTest omitted the last object's resolved `command` field; this
step is therefore inventory evidence only. The exact source binding at the RED
commit remains:

```text
repeated_mult2_semantic_two_square_contract -> repeated_mult2_semantic_two_square_test
```

## Exact failure boundary

Linux explicitly ran:

```text
cmake --build build --target repeated_mult2_semantic_two_square_test --parallel 2
```

The library target was already built. Compilation of
`tests/repeated_mult2_semantic_two_square_test.cpp`, through
`tests/repeated_mult2_semantic_oracle.h:6`, failed because
`openfhe_2023_1788/repeated_mult2.h` did not exist. GNU Make returned exit 2.

Windows explicitly ran the same target against its isolated project-build
directory. MinGW compilation of the same test/include chain produced the same
missing-header diagnostic; Ninja stopped and the step returned exit 1.

No earlier legacy or API step failed. In both jobs the subsequent focused new
test and unfiltered suite have step conclusion `skipped`. Consequently this
evidence proves only that the frozen test requests a public API absent from the
RED tree; it says nothing yet about repeated-Mult2 arithmetic at runtime.

## RED source freeze

The exact Git blobs at tested commit `7399db5` have these SHA-256 identities:

| Frozen RED path | SHA-256 |
| --- | --- |
| `.github/workflows/dcp-rcb.yml` | `e35b2866499d5bcb183706a0bba62b9f1d54922fae1c57aae538d37fb3ca0480` |
| `CMakeLists.txt` | `c0f03945d54715639f655fe31acc82994047381b6b484e4ac70b137b45ee7f4a` |
| `tests/repeated_mult2_exact_vectors.h` | `e065a934ff9104d1deaa19874b0fa86f3e9ee573693b6fb96a747dde1aad3d43` |
| `tests/repeated_mult2_semantic_oracle.h` | `204d1704ad2b164691acc98bc354a80b8ff3278ade53907c1d2bec5a9126d17d` |
| `tests/repeated_mult2_semantic_two_square_test.cpp` | `b3eb681401bade58cb1a81091c33c4307a4595e226531d5a79580f9a5a7d86a3` |

The three test hashes and integrated workflow/CMake hashes still match the
recorded RED freeze. Engineering paths at the later documentation HEAD were
also byte-identical to this tested RED before GREEN preparation began. No
current or future mutable working-tree production bytes are used as RED
evidence.

## Retained evidence

Independent adjudication used the complete GitHub job logs and run metadata,
then cross-checked the already retained originals:

- `REPEATED_RED_RUN_7399db5.json`: 14,182 bytes, SHA-256
  `2138d457e57c3bfdf91dfeccc76221683ab5ac3f00e94c3a2d97436ee87b0ac2`;
- `REPEATED_RED_FAILURE_7399db5.txt`: 7,924 bytes, SHA-256
  `0ae0e332e8aef21db5cfa897b29e18ca47ffd7240c48ba6e2c7081b21dcb6aae`;
- `REPEATED_RED_CHECKPOINT_7399db5.txt`: 2,533 bytes, SHA-256
  `b834e71b842a1e9098e5bfb741ad2fd5f17acb97d756c5ce7576901147faf69a`.

The cross-track originals were read from the lossless-I/O handoff directory;
their later copied locations do not change these identities.

## GREEN entry decision and claim boundary

The genuine RED prerequisite is closed, so entering the GREEN implementation
stage is allowed. The original GREEN must still receive the separately recorded
production-only correction for the four disabled CKKS high-level setters before
it is eligible for hosted acceptance. That correction must not change the three
frozen tests, their vectors/oracle, the integrated workflow, the 2^-80 threshold,
or the CTest name/command.

This receipt establishes **no runtime GREEN**, no warning-clean GREEN build, no
focused 1/1, no full 58/58, no Z/W stage records, no precision measurement, and
no paper reproduction. Those claims remain pending a new exact-SHA dual-host
GREEN run and subsequent review.
