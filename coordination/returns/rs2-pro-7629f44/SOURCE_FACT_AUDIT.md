# RS2 source-fact audit

## 1. Provenance and packet integrity

### Verified facts

- Supplied archive: `chatgpt-pro-rs2-7629f44.zip`.
- Archive SHA-256:
  `e4c04228ac8b3fadc4f4ffd9f7049ebd55dcaf0839647c5ba250772a4958a1e7`.
- `MANIFEST.sha256` verified every one of its 20 listed files.
- The archive contained no path traversal entry and no symbolic link.
- `TASK.md:19-32` declares the project start commit
  `7629f446517413a3ae65551e7efe51b74fd70f00`, parent compile-red commit
  `75cd77f57890eebfb49b9cc30e61b1a666bdd9f3`, and official OpenFHE commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- The packet contains no `.git` directory or Git objects.

### Consequence / unverified boundary

The project files can be byte-bound to this packet, but the declared upstream
commit ID cannot be independently recomputed from the packet. Integration must
first verify that the actual checkout at `7629f446...` has the same parent-file
bytes expected by Patch R1.

## 2. Paper contract

### Verified facts

- `PAPER-2023-1788.txt:785-807` defines pair rescale as
  `(RS_q_l(high), RS_q_l(q_div*high+low) - q_div*RS_q_l(high))` over the retained
  basis.
- `PAPER-2023-1788.txt:809-815` states the exact recombination identity
  `RCB_q_div(RS2_q_l(CT)) = RS_q_l(RCB_q_div(CT))` and that RS2 consumes only
  `q_l`, not `q_div`.
- `TASK.md:43-62` restates the same required executable mapping and identity.

### Engineering inference

The low output cannot generally be replaced by `RS_q_l(low)`: centered
rounding is not linear. Therefore two independent rescale calls are semantically
material, not an implementation detail.

## 3. Starting project architecture

### Verified facts

- `include/openfhe_2023_1788/double_ckks.h:18-22` defines
  `ReadyForFirstMult`, `ReadyForRS2`, and `RefreshRequired`.
- `double_ckks.h:38-88` makes `CiphertextPair` read-only to callers and makes
  `DoubleCKKS` its sole constructor.
- `double_ckks.h:139-147` exposes public DCP, Tensor2, Relin2, RS2, and RCB.
- `src/double_ckks.cpp:240-278` binds `DoubleCKKS` to the exact context,
  requires CKKS-RNS/FIXEDMANUAL, records the full ordered Q basis, assigns the
  final full-basis tower to `q_div`, and keeps the exact prefix as the first pair
  basis.
- `src/double_ckks.cpp:280-341` validates context identity, encoding, level,
  component count, degree, recorded scale, tag, slots, ordered moduli, domain,
  roots, and cyclotomic order.
- Starting `ValidatePair`, `src/double_ckks.cpp:428-504`, accepts only level-1
  earlier lifecycles and validates both pair members.
- `src/double_ckks.cpp:640-815` returns a two-component, level-1,
  `ReadyForRS2` pair from Relin2.
- Starting RS2 is the explicit scaffold at `src/double_ckks.cpp:817-819`:
  `logic_error("DoubleCKKS: RS2 is not implemented")`.
- `src/double_ckks.cpp:821-831` implements RCB as exact componentwise
  `q_div*high+low` after `ValidatePair`.

## 4. Official OpenFHE 1.5.0 mapping

### 4.1 Public `Rescale`

**Verified fact:** `official-openfhe/cryptocontext.h:2507-2510` validates the
ciphertext and delegates to `GetScheme()->ModReduce(...)`, returning that
result.

**Not verified from the supplied files:** the scheme-level output-returning
`ModReduce` declaration/definition that would prove where cloning occurs is not
included. The supplied `cryptocontext.h` itself does not clone before the
call.

**Risk mitigation in G2:** RS2 first clones the pair's high ciphertext and
constructs recombination into another independent clone before calling public
`Rescale`. Thus pair-input immutability does not rely on the absent wrapper
implementation. The actual official wrapper signature/behavior remains an
acceptance-time source check.

### 4.2 FIXEDMANUAL state transition and factor index

`official-openfhe/ckksrns-leveledshe.cpp:172-190` shows that
`ModReduceInternalInPlace`:

- obtains current active tower count as `sizeQl`;
- decreases noise-scale degree;
- increases level;
- calls `DropLastElementAndScale` on every ciphertext component;
- divides the recorded scaling factor by
  `GetModReduceFactor(sizeQl - 1 - i)`.

For one consumed tower, the required recorded-factor index is therefore
`activeTowerCount - 1`. In the R2 fixture this is index `2` because the
pre-RS2 pair has three active towers. This is an index fact; the supplied files
do not define how the corresponding `double` is precomputed, so no unsupported
claim that it is numerically identical to `q_l` is made.

### 4.3 Exact native multiplication

`official-openfhe/cryptocontext.h:2073-2079` shows that
`EvalMultNoCheck(ConstCiphertext&, NativeInteger)` clones the ciphertext and
multiplies each DCRT component by the exact native integer without changing
ciphertext metadata. G2 uses it only after project-side structural validation.

### 4.4 Subtraction

- `official-openfhe/cryptocontext.h:1639-1642` shows public ciphertext `EvalSub`
  performs type checking and delegates to the scheme. The supplied CKKS source
  later contains level/depth adjustment paths, so using the public subtraction
  would make absence of hidden adjustment harder to establish.
- `official-openfhe/dcrtpoly-impl.h:402-407` shows DCRT `operator-=` subtracts
  corresponding tower vectors and does not itself validate vector length or
  basis.

G2 therefore validates both post-rescale ciphertexts to the same complete
state, checks component counts and aggregate DCRT basis equality locally, and
only then performs element subtraction. No ciphertext-level `EvalSub` is used.

### 4.5 Centered divide-and-round

**Verified mechanics:** `official-openfhe/dcrtpoly-impl.h:693-712` copies the
last tower, converts it to coefficient format, drops that tower, switches the
last-tower residue to every retained modulus, applies the precomputed factors,
and updates every retained tower.

**Inference requiring execution/source completion:** this is intended to be the
centered CKKS quotient map specified by `TASK.md:186-204`; however the supplied
files omit the underlying native-polynomial `SwitchModulus` implementation and
the definitions of the CKKS precomputation tables. Exact tie/sign behavior is
therefore not independently derivable from only these three excerpts. R2's
first-principles CRT oracle and signed boundary witnesses are the required
falsifying probe against the actual pinned source.

## 5. Patch-level facts

- R1 changes only `CMakeLists.txt` and adds `tests/rs2_test.cpp`.
- G1 changes only `src/double_ckks.cpp` and adds validation plus the exact
  lifecycle guard before retaining the scaffold.
- R2 changes only `CMakeLists.txt` and `tests/rs2_test.cpp`.
- G2 changes only `src/double_ckks.cpp`.
- G2 RS2 contains exactly two `context_->Rescale(...)` call sites, exactly one
  `EvalMultNoCheck(..., qDiv)` call site, no `RescaleInPlace`, no ciphertext
  `EvalSub`, and no production `try/catch`.
- `ValidatePair` is extended only for the terminal level-2
  `RefreshRequired` state; the earlier level-1 branches retain their exact
  state formulas and diagnostics.
