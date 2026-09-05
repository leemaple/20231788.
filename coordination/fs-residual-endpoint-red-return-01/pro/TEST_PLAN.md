# Test and integration plan — FS-RESIDUAL-ENDPOINT-01 v1-r1

## 1. What is delivered now

This is a specification plus one API/link RED candidate. It does not contain GREEN. The new test-local observer functions, direct reference, exact comparator/classifier, formatter and validator are deliberately declared but undefined. No live endpoint capture, packer/status writer, failure-preserving CTest wrapper, upload implementation or workflow change is delivered.

The exact source baseline is the supplied engineering tree attributed to documentation SHA `f1c33b7fdcc12741f40b96164c87a35f90090345`, with engineering bytes equal to tested source `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e`. BASE_HASHES.json records all44 input engineering files, the43 unchanged files and the two changed/new result identities. An actual hosted commit applying this patch will have its own SHA; never label that commit as the old tested source.

## 2. Ordered application and local static checks

These instructions describe Codex integration; the only operations already run by this author are recorded in EXECUTION_LEDGER.md. No configure/build command is authorized in this review environment.

1. Verify the return ZIP's externally reported size/SHA and its self-excluding manifest, safe relative regular paths and CRC. Verify the supplied input ZIP identity and both nested manifests. Extract the source nested ZIP separately from the outer packet; the patch paths are relative to its `project/` tree or an exact matching checkout, not to the outer packet root.
2. Verify each base entry in BASE_HASHES.json before application. In particular `tests/paper_full_eight_square_contract_test.cpp` must be25552 bytes, SHA `61f3b4b0f6e7c2e6a11a40bbca51b87df520c8190537caa39bdb738910e3f727`; `tests/paper_endpoint_observer_contract.h` must not already exist. The original oracle, workflow and CMake must match their recorded hashes. Reject source drift rather than fuzz-applying a patch.
3. From the exact project root run `git apply --check <absolute-return-directory>/RED.patch`, then `git apply <absolute-return-directory>/RED.patch`. There is one patch and no prerequisite or GREEN patch. Do not separately copy the complete files after applying; those files are for byte verification, not an additional change.
4. Compare the results against `files/tests/` and the result hashes in BASE_HASHES.json. Verify all43 other files are byte-identical and no extra path changed. The complete paper file must be26420 bytes/SHA `b2c966af71d41e198a85004329fd17fc4caa4ec2b3e9c0a9e7ffcfd1d49a230d`; the new header must be10324 bytes/SHA `c8fce07e3a9969d5109f85856b46ce5bc16657447e9d62726cb11b6e294c1d23`.
5. The optional own static/scalar audit is reproducible with Python3.10+ and Git installed:

```
python3 <return-directory>/validate_static_scalar.py <input-outer-zip> \
  --package <return-directory> --output <disposable-directory>/static-results.json
```

It checks bytes/manifests, exact integer slot mapping, rational majorants, historical primary log counts, synthetic grammar fixtures and disposable patch application. It executes only its own code plus `git apply`; it imports no project/returned-input code and performs no transform, codec, compiler or crypto operation. Running it is not C++ RED or GREEN. The JSON records actual absolute patch-command paths, so those path strings differ on another machine while arithmetic/source results should agree.

Codex owns specification adoption, integration and exact-SHA hosted execution. This package neither pushed a commit nor dispatched a workflow.

## 3. Hosted RED — expected sequence and retained result

Retain the existing workflow ordering and flags on Linux and Windows. The paper target is EXCLUDE_FROM_ALL, and only the paper source includes the new contract. The pre-paper checkpoint remains the accepted old60 runtime tests, with unchanged inventories/bodies, and the five explicitly built compile/link API targets:

```
relin2_api_contract_test
rs2_api_contract_test
mult2_api_contract_test
add_api_contract_test
sub_api_contract_test
```

The existing default-built `tensor2_api_contract_test` also remains unchanged; it is not a new sixth explicit API gate. Existing focused contracts and default builds stay where they already occur. [CM:43–69; WF:101–150,277–395]

The Linux paper build command already in the workflow is:

```
cmake --build build --target paper_full_eight_square_contract_test --parallel 2
```

The existing Windows MSYS step uses the unchanged `$PROJECT_BUILD` through `cygpath`:

```
build="$(cygpath -u "$PROJECT_BUILD")"
cmake --build "$build" --target paper_full_eight_square_contract_test --parallel 2
```

**Expected RED:** the paper target references undefined functions in `paper_endpoint_contract` and fails at linkage/API availability, after the old checkpoint. Typical GNU/MinGW diagnostics should identify unresolved `Observe`, `ScaledOneNormExponent`, `DirectSparseReference768`, `ExactAbsoluteDifference`, `AssessDifference`, `CanonicalDecimal` or `IsCanonicalDecimal` symbols. Exact compiler wording/order is not prescribed. No unsupported exception is expected, and no stub may satisfy the test.

No C++ build was run here. A syntax/warning/error before the intended undefined seam, an old60 failure, dependency failure or timeout is a different result and must be retained honestly; it is not the specified RED. Do not claim a numerical algorithm failed merely because linkage failed. Do not execute the normal paper chain or self-test after a failed build. No live endpoint artifact should exist from RED.

Preserve the actual integrated SHA, job/run/attempt identities, compiler and Boost versions, complete legacy/API checkpoint output, paper build failure and terminal job status. Do not rerun automatically, revise the test after an unexpected result, or fabricate a missing-header diagnostic. The candidate intentionally uses missing **definitions**, not a nonexistent include.

After Codex reviews the actual retained RED, a new complete handoff must provide that evidence, current exact source/manifest and this adopted specification before requesting any GREEN implementation. This task stops at authoring the RED candidate.

## 4. Future deterministic observer self-test (GREEN authorization required)

Once the missing functions have been implemented in the later authorized handoff, a single future invocation of the existing executable with `--endpoint-observer-self-test` must occur after paper build and before the paper CTest. It is not a CTest entry and does not change normal CTest61 argv. The current RED does not add this workflow invocation; the expected build failure prevents it from running in any case.

The self-test dispatch is before `Run()` and therefore before all context/key/encryption setup. It must report `namespace=synthetic chain_count=0`, never a live BEGIN or endpoint-evidence identity. Expected tests are already assertions in the delivered header:

| Area | Frozen cases |
|---|---|
| Exact conditioning | C=0; K with exponent-10; exact and upward power-of-two boundaries |
| Exact comparison bridge | |1-2^-1024| represented as (2^1024-1)/2^1024, not rounded first |
| Decisions | bounded equality pass, below-tolerance model contradiction fail, producer allowance pass, threshold overlap unresolved, raw excess fail, ceiling unresolved, unsupported model unresolved, raw failure not hidden |
| Decimal convention | unique zero, negative-zero normalization, +/-1,1/2, two exact integer half-even ties,3 valid spellings and15 invalid strings |
| Full-slot orientation | constant1, X, X^(N-1),3-2X+X^17-X^(N-1); both independent precisions vs direct768 at all16384 slots/components |
| Shape/ownership | both result lengths, exact C/K, finite values, unchanged coefficient vector/modulus/scale |

The flags and assertion labels identify synthetic diagnostics. These controls are not an interval proof, an independent NTT implementation, or a new encrypted trial. The declared root model remains conditional even when controls pass.

Later GREEN must add appropriate malformed-shape/scale/nonfinite tests around the implemented helpers as required by ENDPOINT_SPEC; it must not claim those absent negative execution results were already observed in RED. All file-based evidence tests below use separate disposable synthetic directories and never the live namespace.

## 5. Future evidence tests without additional encrypted trials

Before the next live chain, test the C++ writer validator and independent Python packer/reader against deterministic synthetic data in their own namespace. No production or user key material is needed. All such implementations and tests are deferred to the later GREEN handoff, not included in RED.patch.

The minimum discriminating evidence cases are:

- A complete16384-row synthetic candidate with exact identity/order/precision rules; malformed numeric spellings, missing/duplicate/reordered/extra rows, wrong column/header order, mismatched source/run/attempt/scale/check allowances, wrong row counts, nonfinite and oversized inputs must be rejected independently by C++ and Python. A four-row illustration must fail the live row-count rule and must never enter a live upload directory.
- External-hash and compression cases: canonical or gzip byte mutation, wrong byte count/SHA, nonzero gzip mtime, forbidden optional header fields, a second concatenated member, trailing data, CRC mismatch and decompression overflow. Cross-zlib byte differences are acceptable only when the canonical bytes are identical and the *actual* gzip is correctly externally hashed. Status cannot hash itself.
- Failure propagation cases in a disposable wrapper harness: a simulated finite E80 CTest nonzero code with complete evidence still closes/validates/packs and returns nonzero; simulated CTest zero plus packer failure returns nonzero; missing/fatal/timeout evidence gets truthful incomplete status, no fake sidecar and no PASS. These simulated shell outcomes must be labeled synthetic, not substituted for actual CTest or chain evidence. Test exact-path upload selection so no synthetic/temp/foreign-identity file is retained.

These tests discriminate lost-evidence and swallowed-status errors without encrypting additional inputs. No timing or repeated-random-trial project is created.

## 6. Future single-chain observer-specific GREEN acceptance

After retained hosted RED and an independently reviewed authorized GREEN patch, run the unchanged old60/API checkpoint and the deterministic observer self-test. Then run **the existing normal paper CTest exactly once per host**, with no extra args:

```
ctest --test-dir build --show-only=json-v1
ctest --test-dir build --verbose --output-on-failure -R '^paper_full_eight_square_contract$'
```

Windows keeps its existing `$build` and PATH setup. The actual single CTest invocation must be enclosed by the later failure-preserving wrapper, not followed by a second invocation to recover evidence. Keep name `paper_full_eight_square_contract`, entry61 order, normal executable argv,1200-second timeout, RUN_SERIAL and OMP_NUM_THREADS=2 unchanged. The show-only listing is not a chain.

That one fresh encryption and one eight-square chain supplies both independent endpoints, all exact C/scale conditioning,512/768 direct-root evaluations, unchanged ten-anchor Horner comparisons, all24 integrity comparison receipts, original finite E80 counts and final full-slot signed E0/E8 sidecar. Existing per-round observations, old witness/codec/original-E checks, foreign/state ownership and cleanup remain. No intermediate full-slot campaign is added.

Observer-specific completion requires applicable CONDITIONAL model/preconditions, every prescribed integrity comparison and estimator ceiling, complete canonical rows and independently validated replay, correct external hashes/status, and correct failure propagation. It does **not** require original CTest to become green by weakening E80. A complete observer implementation can pass its own deterministic controls and produce valid failing-chain evidence while the original paper test remains FAIL; the workflow must visibly remain nonzero in that case.

On the next actual observation, report separately:

| Record | Permitted conclusion |
|---|---|
| Observer/self-test and evidence integrity | PASS with CONDITIONAL assurance, FAIL or UNRESOLVED |
| Original E80 and actual CTest/job status | Actual unchanged result; no replacement by A |
| Full-slot E0/E8/I8/A8 | Diagnostic maxima, measured argmax/signed tuples and error allowances |
| A disposition | NOT_ADOPTED, even if numerically small |
| Scientific/project acceptance | Not completed by this diagnostic slice |

## 7. Exact no-extra-trial stopping rule

The authorized future endpoint observation is at most one next normal paper chain on Linux and one on Windows, each from its own exact-SHA job. Existing deterministic helper controls and legacy suites are not extra paper trials. CTest failure replay is not a new trial. Do not demand or perform1000 repetitions, rerandomization, favorable-key selection, additional endpoints, another encryption for capture, full-slot intermediate transforms or benchmark replication.

After the bounded results, stop and report. Complete observer integrity plus small A is diagnostic characterization, not E80 completion. A large discrepancy gives a concrete slot/component/path hypothesis for a separate bounded review; it does not authorize another chain automatically. A compile issue, integrity failure, unsupported model, timeout or missing artifact stops with its actual FAIL/UNRESOLVED result. No automatic rerun or scope expansion follows. Codex owns any later separately authorized task.
