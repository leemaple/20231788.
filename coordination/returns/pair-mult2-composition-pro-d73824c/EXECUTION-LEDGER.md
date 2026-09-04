# Execution ledger

## 1. Identity and disposition

- Actual model: **GPT-5.6 Pro**.
- Scope: bounded **TEST-ONLY** candidate for Pair Add/Sub result into the first
  public `Mult2`.
- Selected tested baseline source:
  `d73824c2d382013c3aadbd7cb29c57008e839714`.
- Documentation head:
  `1610ee522a39949a3f50a08e08ef3a9a8bcc126c`.
- Official OpenFHE pin:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Candidate status: **READY FOR CODEX HOSTED EXECUTION**, not compiled green.

The categories below deliberately separate work performed in this drafting
seat from source facts, inferences, supplied hosted evidence and pending work.

## 2. EXECUTED in this drafting seat

### 2.1 Input integrity and safe extraction

1. Recomputed the outer input byte count and SHA-256:
   `1,361,787` bytes and
   `5eda78d7d49f0eb3ed565c0d18577608027ce31db1c5a165906487b1d79150d6`.
2. Checked the outer ZIP central directory, duplicate names, absolute/parent
   paths, symlink mode bits and CRC. It contains 11 safe regular files.
3. Verified all 10 payload size/hash records in the outer
   `SOURCE-MANIFEST.json`; no missing, altered or undeclared regular payload was
   found. The manifest explicitly excludes itself.
4. Recomputed the nested context archive identity: `1,305,833` bytes and
   `e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce`.
5. Checked the nested ZIP for duplicate/unsafe/symlink entries and CRC.
6. Verified all 70 payload size/hash records in its manifest; no mismatch,
   missing path or undeclared regular payload was found.
7. Rehashed the untouched outer input after all review/drafting work; its byte
   count and SHA-256 remained unchanged.

### 2.2 Source, paper and evidence inspection

1. Read the outer `TASK.md` before treating the older embedded static-review
   task as historical material.
2. Inspected the supplied paper text for Section 2.1 Add/Sub equations and the
   Section 4 componentwise Pair and `Mult2` composition statements.
3. Inspected `TEST_SEAMS.md`, the current Pair/Mult2 header and source, the full
   `tests/mult2_e2e_oracle_test.cpp`, CMake, workflow and relevant integration
   ledgers/logs.
4. Confirmed statically that the existing source already exposes the required
   public `Add`, `Sub`, `Tensor2`, `Relin2`, `RS2` and first `Mult2` seams; no
   production correction was drafted.

### 2.3 Independent host-vector checks

1. Parsed `FROZEN-HOST-VECTORS.json` using JSON binary64 round-trips.
2. Converted each real and imaginary component with Python
   `Fraction.from_float` and independently evaluated exact complex addition,
   subtraction and multiplication.
3. Checked 32 complex identities, i.e. 64 scalar real/imaginary equalities,
   across eight slots. Every identity was exact.
4. Independently parsed all seven final C++ frozen-vector functions and checked
   every C++ value against the corresponding JSON value as an exact rational.
5. Confirmed the required L1 maxima: A `3/8`, B `3/16`, C `3/4`, A+B `7/32`,
   A-B `9/16`. No fixture requires relaxing the existing unit envelope.

### 2.4 Patch and source closure

1. Generated two raw sequential patches and one aggregate patch.
2. Ran whitespace/error checking while generating the patches.
3. Applied patches 0001 then 0002 to an isolated byte copy of the exact supplied
   54-file project snapshot.
4. Applied the aggregate patch independently to a second isolated byte copy.
5. Compared every project regular file: both replay trees are byte-for-byte
   equal to the proposed final tree.
6. Parsed patch paths and confirmed that all three patches touch only:
   `CMakeLists.txt` and `tests/mult2_e2e_oracle_test.cpp`.
7. Checked that no production/header/workflow/other-test path changed.
8. Checked patch deletions: only the prior usage-string line is replaced at
   each stage; no old assertion, case body, vector, threshold or command binding
   is deleted.
9. Performed a lexical delimiter/state scan of the final C++ source. This is a
   static sanity check, not compilation.

### 2.5 CMake closure

1. Parsed the exact old/new CTest name-command bindings.
2. Confirmed 53 unique baseline registrations, 54 unique registrations after
   patch 0001 and 55 unique registrations after patch 0002.
3. Confirmed all 53 old bindings are unchanged and remain in the same order at
   both new stages.
4. Confirmed the two exact additions are:

   ```text
   mult2_pair_add_input_hybrid_complex
     -> mult2_e2e_oracle_test pair_add_input_hybrid_complex
   mult2_pair_sub_input_hybrid_complex
     -> mult2_e2e_oracle_test pair_sub_input_hybrid_complex
   ```

5. Confirmed every final command resolves to one of the 15 declared executable
   targets.

### 2.6 Delivery closure

After freezing the payloads, the delivery ZIP was extracted into a fresh empty
folder and checked for CRC errors, duplicate/unsafe/symlink members, required
member completeness, nonempty required files, `MANIFEST.sha256` agreement and
`FILE-INVENTORY.json` agreement. These packaging checks do not compile or run
project code.

Machine-readable results are in `STATIC_CHECKS.json`,
`HOST_VECTOR_VERIFICATION.json`, `CMAKE_TEST_CLOSURE.json` and
`INPUT_OUTPUT_MANIFEST.json`.

## 3. SOURCE / OBSERVED FACTS FROM SUPPLIED BYTES

- The baseline CMake file registers 53 tests.
- The baseline workflow retains warning-as-error builds and explicit Relin2,
  RS2, Mult2, Add and Sub API target builds for Linux and Windows paths.
- The supplied production `Mult2` body is the named public composition
  `RS2(Relin2(Tensor2(left,right)))`.
- The supplied Pair Add/Sub bodies preserve `ReadyForFirstMult` and perform
  corresponding-member ring arithmetic after validation.
- The retained baseline logs state 53/53 at exact source `d73824c` for Linux
  job `100964299802` (`0.68 s`) and Windows job `100964299593` (`2.27 s`) in
  run `33854419062`.

These statements describe supplied source/log bytes. They are not executions
performed by GPT-5.6 Pro.

## 4. INFERRED / PROPOSED

- Applying patch 0001 should produce a 54-test candidate and applying patch
  0002 cumulatively should produce a 55-test candidate, because the CMake
  registrations parse that way.
- The proposed source is intended to compile against the exact pinned OpenFHE
  APIs because it reuses already-present types and helpers, but only a compiler
  can establish that fact.
- The cases are expected to discriminate common composition defects using the
  independent coefficient and literal-product checks described in
  `REVIEW_AND_DESIGN.md`; no unexecuted defect mutation is reported as observed.

## 5. SUPPLIED HOSTED CI — NOT REEXECUTED

| Source | Run/job | Supplied result | Ownership |
|---|---|---|---|
| `d73824c` baseline | run `33854419062`, Linux `100964299802` | 53/53, 0.68 s | supplied GitHub Actions evidence |
| `d73824c` baseline | run `33854419062`, Windows `100964299593` | 53/53, 2.27 s | supplied GitHub Actions evidence |

No supplied result covers either proposed new test.

## 6. NOT EXECUTED

The following were deliberately not performed in this seat:

- OpenFHE or project CMake configure;
- C++ compilation or linking;
- warning build on GCC or MinGW64;
- focused CTest or full CTest;
- encryption, decryption, DCP, Add, Sub, Tensor2, Relin2, RS2 or Mult2 runtime;
- cryptographic key generation or cache mutation;
- precision, security, theorem or performance experiment;
- dependency installation;
- Git push, merge or repository write to the supplied source;
- CI dispatch, rerun, cancellation or external messaging;
- another-agent dispatch.

## 7. Pending hosted commands

After patch 0001, Codex should build the existing target and run the focused Add
case before the full suite. After patch 0002, it should run both focused cases
before the full suite on both existing host jobs. Representative commands are
recorded in `REVIEW_AND_DESIGN.md` and are **pending**, not results.

## 8. Claim boundary

This delivery is additional regression coverage only. It does not establish
more-than-53-bit precision, 106-bit precision, repeated multiplication, a
conservative BV theorem, paper Table 3 parameters, security, performance or
project completion. The retained `PER_PATH_CONDITIONAL`,
`conservative_E_Relin_available=false` and
`universal_theorem_gate=UNPROVED` labels remain intact.
