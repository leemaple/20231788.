# Tensor2 remediation closure review

Reviewed: 2026-09-01 (+08:00)

## Bounded verdict

**MERGEABLE**

This verdict is bound only to the supplied clean-room DCP/RCB + first-lifecycle Tensor2 slice at:

- branch label: `agent/codex-tensor2-01`;
- exact source/test head: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`;
- recomputed current source tree: `759d5195739684748d5a9664edabe3fa719e1acf`;
- pristine OpenFHE 1.5.0: `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- exact-current retained Actions run: `33436252725`.

Finding counts:

| Severity | Count |
|---|---:|
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

The preceding P2 evidence-closure finding is **CLOSED**. The preceding P3 diagnostic-compatibility finding is **CLOSED**. I found no new current-head defect that requires a patch.

`MERGEABLE` is deliberately narrow. It says only that the supplied DCP/RCB + Tensor2 slice is ready for the clean-room integration line on the evidence reviewed here. It remains conditional on the separately required **Windows ZCode/Zima same-commit review**. It makes no claim about Relin2, RS2, Mult2, pair Add/Sub, repeated multiplication, precision, performance, serialization, `t>2`, bootstrapping, or network security.

## Input-package gate

**PASS.** No source, algorithm, or CI conclusion was drawn until the attachment identities were independently checked.

Independently recomputed from the supplied attachments:

- ZIP filename: `20231788-cleanroom-tensor2-remediation-closure-fb862a3-ci33436252725.zip`;
- ZIP size: `9,334,115` bytes;
- ZIP SHA-256: `54c4ab7ad202bfe8cb9c95a58816bd9d30af2132978b75ea3a074e1c4f561832`;
- ZIP central-directory entries: `2,310`;
- external binding SHA-256: `3572ed940fda807c20c19bd57ada8cfff08e3a6da0cc8dba95f8c2bc849954fc`;
- standalone task SHA-256: `71537ff615292b56a701866c22ecc722020d4175f2ba81b4f504364b833680b2`;
- internal `HANDOFF_CONTENTS.md` SHA-256: `ba01c3fd5c8537d75223ae40f28684f85c22ce76f6abd0deade8ae3ebc10f5e3`;
- internal `MANIFEST.sha256` SHA-256: `4fdc1087cf28fa058d013e2ff25f4b20bdb197bbf80a2eaa250bb3e1a2c4f95e`;
- `MANIFEST.sha256`: `2,037/2,037` entries verified;
- internal `FINAL_REVIEW_TASK.md` is byte-identical to the standalone task;
- `unzip -t` passed;
- no absolute or `..` traversal ZIP entry was found.

The current exported project was independently indexed in a temporary Git repository. `git write-tree` recomputed exactly:

```text
759d5195739684748d5a9664edabe3fa719e1acf
```

Reversing the supplied `REMEDIATION_DIFF.patch` against that exact tree passed `git apply --reverse --check` and reconstructed exactly the previously reviewed tree:

```text
2269bee6bac5e7cd1124ab78c49a750af9a38942
```

The full `PROJECT_DIFF.patch` likewise reverse-applied cleanly to the current exported tree. These checks strongly bind the supplied byte trees and exported diffs. They **do not** prove Git commit-object ancestry or absence of history rewriting because the package deliberately contains no `.git` object database. That object-level claim remains explicitly **unverified**, as required by the task; it is not converted into an acceptance claim.

The handoff's Gitleaks results are retained packaging evidence rather than a local scan claim. I independently reran the byte/hash/archive/path checks above; I did not claim a new Gitleaks execution.

## Evidence-class separation

### Observed current source facts

The exact current source implements and validates the bounded Tensor2 seam as follows:

- `src/double_ckks.cpp:508-511`: `ValidatePair(left)`, `ValidatePair(right)`, then `ValidateTensorCompatibility(left,right)` occur before member-handle acquisition or OpenFHE arithmetic.
- `src/double_ckks.cpp:513-516`: raw read-only member handles are taken only after those validations.
- `src/double_ckks.cpp:518-520`: exactly three `EvalMultNoRelin` calls exist in Tensor2 production code.
- `src/double_ckks.cpp:523`: the two cross products are combined with one `EvalAdd`.
- There is no low-low multiplication in Tensor2.
- `src/double_ckks.cpp:525-539`: scale normalization changes only the two result ciphertext metadata fields (`noiseScaleDeg` and recorded scaling factor); it performs no coefficient operation and drops no tower.
- `src/double_ckks.cpp:541-546`: the Tensor paper-scale descriptor is computed from both input pair manifests and the actual integer divisor.
- `src/double_ckks.cpp:461-505`: result validation enforces the three-component shape, level/basis, OpenFHE metadata schedule, key tag, slots, and both paper-scale values.
- `include/openfhe_2023_1788/double_ckks.h:86-133`: `TensorCiphertextPair` is a distinct final type with a private constructor and read-only ciphertext getters; it has no lifecycle field.
- `src/double_ckks.cpp:324-325`: the remediated DCP call now passes state label `"pair"`, restoring the legacy deep diagnostic without changing a predicate.
- `tests/dcp_rcb_test.cpp:571-575`: the public DCP regression requires `DCP input key tag does not match its pair state`.

No production `try`/`catch`, relinearization, `ModReduce`, fourth multiplication, low-low term, mutable Tensor constructor, test-only friend/backdoor, later-operation implementation, or upstream OpenFHE modification was found.

### Mathematical derivation from the supplied paper

The rendered supplied paper, Definition 4.1, defines the pair tensor as:

```text
high = h1 tensor h2
low  = h1 tensor l2 + l1 tensor h2
```

and explicitly discards the low-low component. Lemma 4.2 relates the ordinary tensor to `q_div * RCB(Tensor2)` plus the deliberately omitted low-low term. For input logical scales `H_i` (high component) and `R_i` (recombined value), this gives:

```text
H_out = H_1 * H_2
R_out = R_1 * R_2 / q_div
```

The actual integer `q_div` is therefore the paper/logical divisor.

### Observed pristine OpenFHE 1.5.0 facts

From the supplied pristine OpenFHE source at the pinned commit:

- `src/pke/include/cryptocontext.h:1968-1972`: public `EvalMultNoRelin` performs `TypeCheck` then delegates to scheme multiplication.
- `src/pke/include/cryptocontext.h:263-288`: `TypeCheck` rejects different key tags, making the project's different-key-tag test genuinely order-sensitive.
- `src/pke/lib/schemerns/rns-leveledshe.cpp:182-191,479-485`: under FIXEDMANUAL, equal-level ciphertext multiplication does not perform a rescale; it aligns levels then reaches the core multiply.
- `src/pke/lib/schemebase/base-leveledshe.cpp:620-660`: two 2-component inputs produce three components, add the input noise-scale degrees, and multiply recorded scaling factors.
- `src/pke/include/schemerns/rns-cryptoparameters.h:601-620,636-649`: FIXEDMANUAL exposes its fixed approximate base scaling factor for both real scaling and the standard modulus-reduction bookkeeping factor.
- `src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:172-190`: a real `ModReduceInternalInPlace` changes level/coefficients/towers and reduces the metadata degree/factor.

### Module-design inference

The module deliberately keeps the paper's actual-prime scale and OpenFHE's FIXEDMANUAL metadata schedule distinct:

```text
paper:
  H_out = H_1 * H_2
  R_out = R_1 * R_2 / q_div

OpenFHE bookkeeping:
  degree     = 3
  recordedSF = SF_1 * SF_2 / baseSF
```

where `baseSF = CryptoParametersCKKSRNS::GetScalingFactorReal(0)`. This is a bounded integration contract for the current first lifecycle, not a statement that OpenFHE automatically performs this normalization and not an equation `q_div == baseSF`. The current test explicitly rejects that conflation at `tests/tensor2_test.cpp:553-554`.

### Retained remote execution evidence

The hosted run/API/job/log records are inspected retained evidence. They are not described as local execution. Details are audited in `TENSOR2-REMEDIATION-EVIDENCE-AUDIT.md`.

### Local execution

I independently configured and built the supplied pristine OpenFHE source on Linux, installed it outside the supplied source trees, then configured and strictly built the exact current clean-room project and ran CTest. The project passed **6/6** locally. Exact commands, environment, sandbox timeout interruptions during the long OpenFHE build, and exclusions are recorded in `EXECUTION.md`.

No local Windows run, hosted CI dispatch/rerun, ZCode/Zima run, precision benchmark, performance benchmark, or network-security work was performed.

## Preceding P2 disposition

**CLOSED. No residual finding.**

The former issue was that the three intermediate hosted runs lacked raw provider evidence. The current exact Git export contains `run.json`, `jobs.json`, Linux log, and Windows log for each run under `artifacts/tdd/tensor2/hosted/<run-id>/`.

All 12 raw-file SHA-256 values independently recompute to the values in the tracked hosted evidence README, and the 12 raw files total exactly `637,139` bytes.

The records establish:

1. `33425868973`, head `f3db12ef...`: Linux builds the pre-existing production library and then fails the compile-only public API contract because `TensorCiphertextPair`, `TensorScaleDescriptor`, and `DoubleCKKS::Tensor2` are absent. CTest is skipped. Windows is cancelled during OpenFHE build and supplies no project build/CTest claim.
2. `33426712752`, head `482d27d0...`: Linux strict build succeeds; `dcp_rcb` passes; all five Tensor2 CTests execute independently and fail on the immediate `DoubleCKKS: Tensor2 is not implemented` scaffold. Windows is cancelled during OpenFHE build and supplies no project build/CTest claim.
3. `33427271692`, head `1408d462...`: Linux strict build succeeds and 6/6 CTests pass. Windows is cancelled during OpenFHE build and supplies no project build/CTest claim.

The overall runs are correctly recorded as cancelled because the unneeded intermediate Windows jobs were cancelled. No cancelled Windows job is treated as a Windows result.

## Preceding P3 disposition

**CLOSED. No residual finding.**

The remediated sequence is supported by the supplied exported history/diff plus raw provider timestamps:

- test-only commit `9d1d10a3414dce68b84d9887337254c275098d79` has recorded commit time `2026-09-01 04:26:40 +0800` (`2026-08-31T20:26:40Z`);
- P3-red run `33436068864` was created at `2026-08-31T20:26:45Z`, binds exact head `9d1d10a...`, and on Linux fails only `dcp_rcb` because actual production says `ciphertext state` while the new public test requires `pair state`; the other five CTests pass;
- Windows job `99632689495` is cancelled; its project build/test steps are skipped and provide no Windows result;
- production-fix commit `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9` has recorded commit time `2026-09-01 04:28:45 +0800` (`2026-08-31T20:28:45Z`);
- final run `33436252725` was created at `2026-08-31T20:28:49Z`, binds exact head `fb862a3...`, and completes successfully.

The source change itself is the smallest correct remediation: the single shared-validator call argument changes from `"ciphertext"` to `"pair"` at `src/double_ckks.cpp:324-325`. The validator predicate is untouched; exception type/prefix and fail-fast behavior are untouched; Tensor2 arithmetic is untouched. The new regression lives at the public DCP seam and is invoked by the normal `dcp_rcb` test executable.

Again, the exported timestamps/history support the claimed sequence but cannot prove Git object ancestry without `.git`. That limitation remains explicitly unverified rather than being treated as a failure of the remediation.

## Answers to the seven required questions

### 1. Is P2 fully closed?

**YES.** Raw internally consistent provider evidence is present for all three named intermediate runs. Hashes, run IDs, exact head SHAs, Linux outcomes, and cancellation boundaries match. The cancelled Windows jobs are not converted into build/test results.

### 2. Do the raw states/timestamps and remediation diff/history support public-seam red before the production fix?

**YES, with the mandated ancestry boundary.** The test-only commit timestamp precedes the exact-head P3-red run; that run exposes the old `ciphertext state` diagnostic while all five Tensor2 tests pass; the production-fix commit is later, followed by the exact-current green run. `REMEDIATION_DIFF.patch` reverse-applies cleanly to the current tree and reconstructs the previously reviewed tree exactly. The package lacks `.git`, so commit-object ancestry and absence of history rewriting remain **unverified**.

### 3. Is the one-token P3 source change the smallest correct remediation?

**YES.** It changes only the state-label argument passed to the existing shared validator, from `"ciphertext"` back to `"pair"`. It restores the legacy diagnostic required by the public DCP regression without altering any validation predicate, arithmetic, exception recovery, API, or abstraction.

### 4. Does exact current source/diff/test show any frozen-contract regression?

**NO.** No Tensor2 arithmetic, dual-scale representation, state/type boundary, validation order, oracle/witness, immutability, or accepted DCP/RCB behavior regression was found.

The current Tensor2 path remains exactly three `EvalMultNoRelin` products plus one cross-term addition; low-low remains omitted; no tower is consumed; paper scales still use the actual `q_div`; OpenFHE metadata still uses `baseSF`; result type and validation remain unchanged from the previously reviewed Tensor2 tree. The remediation range changes only retained evidence, one DCP regression test, and the one diagnostic label argument.

### 5. Does final run `33436252725` bind exact current head and pass Linux/Windows on pristine OpenFHE 1.5.0?

**YES.** `ci/run.json` binds `head_sha` to `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9` with overall `completed / success`.

- Linux job `99633299988`: `ubuntu-24.04`, CMake `3.31.6`, GCC `13.3.0`; OpenFHE provenance verifies `df495ba...`; project strict build passes; 6/6 CTests pass.
- Windows job `99633300315`: Windows Server 2022 / MSYS2 MINGW64, CMake `4.4.2`, GCC `16.2.0`; exact project head and pristine OpenFHE `df495ba...` are checked out; OpenFHE and project builds pass; 6/6 CTests pass.

These are retained remote facts, not local Windows execution by this reviewer.

### 6. Did the remediation add unsupported behavior, dependency, backdoor, exception recovery, portability risk, or scope creep?

**NO.** The remediation range adds raw retained evidence files, one public-seam diagnostic regression, and one diagnostic label fix. It does not add production arithmetic, a fourth multiplication, low-low product, `ModReduce`, relinearization, public mutators, test-only production access, production `try`/`catch`, later operations, or an upstream OpenFHE change. Boost remains test-only and is the required independent-oracle dependency. Exact-current hosted Linux/Windows plus the local Linux build provide direct build portability evidence for the reviewed slice.

Historical status prose in retained TDD documentation is not used as current CI authority; the exact-current gate is bound by `CI_EVIDENCE.md` and the raw `ci/` provider records for run `33436252725`.

### 7. Exact-current verdict

**MERGEABLE.**

Both named preceding findings are closed and no new current-head finding was identified. This verdict is bounded to DCP/RCB + first-lifecycle Tensor2 and remains conditional on the separately required Windows ZCode/Zima same-commit review. No later operation, precision, performance, serialization, or security claim is implied.

## Patch disposition

No `0001-tensor2-remediation-closure-fixes.patch` is included because there is no concrete current-head finding requiring a source/test change.
