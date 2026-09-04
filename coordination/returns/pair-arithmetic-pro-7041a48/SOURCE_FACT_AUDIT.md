# SOURCE FACT AUDIT — minimal pair Add/Sub

## 0. Status vocabulary

- **OBSERVED**: directly read from the hash-verified supplied packet.
- **SOURCE FACT**: directly established by the paper text or pinned source bytes.
- **IMPLEMENTED**: present in the returned candidate source.
- **INFERENCE**: design consequence that still needs compilation/runtime confirmation.
- **EXECUTED**: actually performed in this container.
- **PENDING / NOT EXECUTED**: not performed and not claimed.

## 1. Bound source record

**OBSERVED**

- Packet SHA-256: `50269f2a0f5198d5f4aee312808097370e6153783f7586cb1e9c0446da133c38`.
- Packet source commit: `7041a489ae1afa98b75322ec334543f29f10b738`.
- Target branch: `codex/pair-arithmetic-01`.
- Pinned pristine OpenFHE 1.5.0 commit: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- All 24 manifest-listed files matched both byte count and SHA-256; there were no unlisted regular files apart from the manifest itself.
- Baseline public header and production source had DCP, Tensor2, Relin2, RS2, and RCB, but no pair Add/Sub.
- The candidate does not add or assume Mult2.

## 2. Paper-to-operation mapping

### 2.1 Ordinary ciphertext arithmetic

**SOURCE FACT** — `PAPER-2023-1788.txt:252-254` defines ordinary ciphertext addition/subtraction for two ciphertexts in the same active ring as

- `ct_add = [ct + ct']_{Q_l}`;
- `ct_sub = [ct - ct']_{Q_l}`.

The operation is therefore modular ring addition/subtraction, component by component for the fixed two-component RLWE ciphertext representation used by this project.

### 2.2 Pair arithmetic

**SOURCE FACT** — `PAPER-2023-1788.txt:578` states that multi-precision addition and subtraction on pair ciphertext representations are performed componentwise.

For `CT_L = (high_L, low_L)` and `CT_R = (high_R, low_R)`, the implemented mapping is therefore:

```text
Add(CT_L, CT_R) = (high_L + high_R, low_L + low_R)
Sub(CT_L, CT_R) = (high_L - high_R, low_L - low_R)
```

No cross term, low-low omission rule, key switching, rescaling, or multiplication normalization belongs in these operations.

## 3. Pinned OpenFHE source mapping

### 3.1 Fresh-result provenance

**SOURCE FACT** — `official-openfhe/base-leveledshe.cpp:68-72` and `:102-106` implement ordinary ciphertext Add/Sub by cloning the left ciphertext and then performing the operation in place on that clone.

**IMPLEMENTED** — pair Add/Sub independently clone `left.high_` and `left.low_`. This gives each output member exactly one corresponding provenance source:

- output high derives only from left high metadata/parameters;
- output low derives only from left low metadata/parameters;
- high and low are never swapped or sourced from one another.

The existing project tests already treat OpenFHE ciphertext cloning as a fresh ciphertext and fresh outer metadata map with the cloned map's metadata entry pointers retaining the source entry identities. The new tests assert that same policy and also assert deep value equality. The exact runtime behavior remains **PENDING** until compiled against the pinned OpenFHE installation.

### 3.2 RLWE component arithmetic

**SOURCE FACT** — `official-openfhe/base-leveledshe.cpp:574-590` and `:601-617` add/subtract corresponding ciphertext components with `+=`/`-=` after cloning the left operand.

**SOURCE FACT** — `official-openfhe/dcrtpoly-impl.h:374-379` and `:402-407` apply those DCRT operations towerwise.

**IMPLEMENTED** — after validation proves that both pair members have exactly two RLWE components and the same ordered active basis, the candidate performs:

```cpp
highElements[index] += rightHighElements[index];
lowElements[index]  += rightLowElements[index];
```

or

```cpp
highElements[index] -= rightHighElements[index];
lowElements[index]  -= rightLowElements[index];
```

for every validated component index.

### 3.3 Why the candidate does not call `EvalAdd` / `EvalSub`

**SOURCE FACT** — `official-openfhe/cryptocontext.h:263-288` shows that ordinary `TypeCheck` covers nullness, context identity, key tag, and encoding type. It does not establish the pair-specific lifecycle, divisor, exact ordered basis, level, degree, recorded/paper/logical scale, slots, format, and fixed component-count contract.

**SOURCE FACT** — the public wrappers at `official-openfhe/cryptocontext.h:1420-1422` and `:1639-1641` delegate to scheme-level convenience operations after that ordinary type check.

**DESIGN DECISION** — direct arithmetic on already cloned and fully validated DCRT values avoids any convenience path whose level/scale adjustment policy is outside the pair contract. It also makes absence of hidden rescale/alignment auditable in the production diff.

## 4. Validation and compatibility contract

**IMPLEMENTED** — both operations use this strict order:

```text
ValidatePair(left)
ValidatePair(right)
ValidatePairCompatibility(left, right, "Add" or "Sub")
clone/arithmetic only after all checks succeed
```

The narrow compatibility helper compares:

1. exact `CryptoContext` identity;
2. lifecycle;
3. `q_div`;
4. ordered active RNS basis;
5. level;
6. recorded scaling factor;
7. noise-scale degree;
8. paper recorded scaling factor;
9. paper divisor;
10. logical scaling factor;
11. recombined logical scaling factor;
12. key tag;
13. slots;
14. format;
15. component-count manifest.

Independent `ValidatePair` checks also verify each high/low ciphertext's actual context, basis, level, degree, recorded factor, key tag, slots, encoding, format, and exactly two RLWE components. This means malformed public-accessor corruption is rejected before compatibility or arithmetic.

## 5. Result-state policy

**IMPLEMENTED**

- The result is a fresh `CiphertextPair` with fresh cloned high/low ciphertext objects.
- The complete pair manifest is copied from the validated left pair.
- Because mutual compatibility has already established equality, this preserves the same lifecycle and all scale/basis/key/slot facts for both operands.
- The result is passed through `ValidatePair` before return.
- Same-object inputs are supported: both output clones are completed before either is modified.
- Caller operands are only read through const pair access; no caller ciphertext vector is mutated.

**INFERENCE, PENDING RUNTIME CONFIRMATION** — direct modular Add/Sub should preserve the valid pair lifecycle at `ReadyForFirstMult`, `ReadyForRS2`, and `RefreshRequired`, because it changes only ring values and none of the manifest/state facts. The supplied tests exercise all three states.

## 6. RCB algebra checked by the independent tests

For either operation `⊕ ∈ {+, -}`, public RCB of the result must satisfy, for every RLWE component and active-Q coefficient:

```text
RCB(Add/Sub(L,R))
= q_div * (high_L ⊕ high_R) + (low_L ⊕ low_R)  mod Q
```

The expected residues in `tests/pair_arithmetic_test.cpp` are formed with `boost::multiprecision::cpp_int`, textbook CRT reconstruction, centered representatives, and signed modular reduction. They are not formed with production Add/Sub, OpenFHE EvalAdd/EvalSub, production RCB on the operands, or DCRT arithmetic output.

## 7. Side-effect boundary

**IMPLEMENTED / STATICALLY INSPECTED**

The new production lines contain no calls or references to:

- `EvalAdd` / `EvalSub`;
- rescale or modulus reduction;
- relinearization;
- evaluation-key generation/insertion/removal;
- context-parameter mutation;
- automatic refresh or second multiplication;
- serialization or new dependencies.

The lifecycle test prepares DCP → Tensor2/Relin2 → RS2 using the public seam, deletes only the fixture key tag, verifies that an unrelated guard key row remains, then snapshots the remaining cache before Add/Sub/RCB.

## 8. Existing-code-fix audit

**OBSERVED / RESULT** — no necessary pre-existing production fix was identified for pair Add/Sub. No existing validation or operation body was silently changed. The production delta is insertion-only: four header lines and 106 source lines, with zero production deletions.

This does not certify all pre-existing code. It records only that the supplied baseline exposed the validation and state invariants needed for the scoped implementation without requiring a separate correction.

## 9. Test coverage map

The returned tests are designed to distinguish at least:

- Add versus Sub sign;
- Sub operand order;
- high/low swap;
- dropped high, low, or RLWE component;
- wrong modular wrap/carry near signed `±Q/2` boundaries;
- zero/self subtraction and self addition;
- left/right same-object aliasing;
- hidden level/scale alignment or lifecycle change;
- incorrect metadata provenance;
- input value/metadata/parameter mutation;
- evaluation-key cache mutation/dependence;
- mixed lifecycle;
- genuine key-tag, slot, and context differences;
- malformed divisor, basis, level, degree, all scale fields, key tag, slots, encoding, component count, and tower format.

Untouched encrypted public-pipeline fixtures include nonzero real and complex vectors; controlled coefficient replacement is isolated to deterministic boundary witnesses.

## 10. Remaining verification boundary

**EXECUTED**

- input SHA-256, ZIP integrity, manifest byte/hash coverage;
- full packet source/PDF inspection;
- 12-patch `git am` replay on exact supplied selected-project bytes;
- resulting tree/file equality;
- `git diff --check`;
- YAML parse and workflow target/branch inspection;
- static red/green state and forbidden-operation scans;
- CMake dependency-availability probe.

**NOT EXECUTED**

- successful CMake configure against pristine OpenFHE 1.5.0;
- any C++ compilation or warning build;
- API contract target execution;
- runtime red observation;
- focused or full CTest;
- Linux hosted CI or Windows/MinGW64 CI;
- decoding, accuracy, security, theorem, 53/106-bit, or performance claims.

The dependency probe stopped at `find_package(OpenFHE 1.5.0)` because this container has no OpenFHE installation. Therefore no compiler or test success is claimed.
