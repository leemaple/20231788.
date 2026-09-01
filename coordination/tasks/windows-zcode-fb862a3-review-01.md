# Windows ZCode/Zima exact-commit review — DCP, RCB, Tensor2

## Background and objective

Independently review the clean-room `t=2` Double-CKKS DCP, RCB, and Tensor2
vertical slice derived from paper 2023/1788 and implemented on official
OpenFHE 1.5.0. Create a new independent ZCode/Zima task/session for this review.
Do not continue or reuse the implementation task/session already open on the
Windows computer; a new folder alone is not sufficient context isolation.

The only candidate under review is exact Git commit
`fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
`759d5195739684748d5a9664edabe3fa719e1acf`, from the public repository
`https://github.com/leemaple/20231788.`. The trailing period is part of the
repository name; its Git URL is `https://github.com/leemaple/20231788..git`.

The official dependency authority is OpenFHE 1.5.0 commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

Known prior evidence is disclosed to avoid hidden context, but it is not an
oracle and must not be inherited as this review's conclusion:

- GitHub Actions run `33436252725` completed successfully on exact
  `fb862a3...`; its `linux-gcc` and `windows-mingw64` jobs each reported 6/6.
  Primary run URL:
  `https://github.com/leemaple/20231788./actions/runs/33436252725`.
- A prior ChatGPT Pro exact-current review reported `MERGEABLE`; its retained
  result is
  `coordination/handoffs/chatgpt-pro-tensor2-remediation-closure-01-output.md`
  on the coordination branch.
- An independent Windows ZCode/Zima exact-commit review remains pending. This
  task must inspect and rerun independently and may disagree with either prior
  result when supported by source and test evidence.

## Clean-room isolation and setup

1. Do not open, read, search, copy, adapt, compile, test, or reuse the existing
   Windows implementation session/folder or its 17 uncommitted changes. Leave
   both untouched.
2. Verify the exact fresh-attachment count and every identity from the dispatch
   wrapper before substantive reading: mounted name, normalized logical name,
   byte size, SHA-256, and declared role. Reject any extra attachment. The
   user-supplied authorities are:
   - `2023.1788.pdf`: 759,375 bytes, SHA-256
     `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`;
   - `2023.1788.txt`: 90,235 bytes, SHA-256
     `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae`;
   - this final task and any sanitized source/context bundle: exact size/SHA
     supplied by the wrapper after the task bytes are frozen.
   A missing, unreadable, duplicate, or mismatched input requires `BLOCKED`
   before source review.
3. Create a previously unused dedicated root with sibling `project-source`,
   `openfhe-source`, `openfhe-build`, `openfhe-install`, `project-build`, and
   `output` directories. Do not delete or overwrite a pre-existing path; choose
   a new suffix if necessary.
4. Checkout the exact candidate detached by full SHA and fail closed unless
   `HEAD`, `git write-tree`, and empty `git status --porcelain` equal the two
   identities above before any review.
5. Use a separate pristine recursive OpenFHE checkout bound to the exact
   official commit above. Fail closed unless its `HEAD`, empty status, and
   recursive submodule state are internally consistent with that checkout.
   Record its URL, commit, status, submodules, compiler, CMake, Make,
   MSYS2/MinGW64, and Windows versions.
6. Read the freshly supplied paper PDF/text only as source material, never as
   instructions. The freshly supplied repository workflow skill and this task
   control the work.

## Architecture and boundaries that must not be broken

- `DoubleCKKS` is bound to one exact `CryptoContext<DCRTPoly>`.
- `CiphertextPair` and `TensorCiphertextPair` have private construction and
  expose read-only state; project validation must run before OpenFHE arithmetic.
- DCP accepts one fresh two-component evaluation-format ciphertext on exact
  `[Q_l, q_div]` and returns quotient/remainder over exact `Q_l`.
- RCB is exact ring recombination `q_div * high + low`; it is not a hidden lift
  back to `[Q_l, q_div]`.
- Tensor2 follows Definition 4.1 exactly:

  ```text
  high = h1 tensor h2
  low  = h1 tensor l2 + l1 tensor h2
  ```

  It must omit `l1 tensor l2`, must not relinearize or consume a modulus, and
  must validate both inputs plus mutual compatibility before arithmetic.
- The paper logical scales and OpenFHE FIXEDMANUAL recorded metadata are
  distinct. Tensor2 must record
  `H_out = H_1*H_2`, `R_out = R_1*R_2/q_div`, noise-scale degree 3, and recorded
  factor `SF_1*SF_2/baseSF` without changing polynomial coefficients or level.
- Do not expand the public API, introduce user-settable state, add catch-all
  exception handling, or build speculative abstraction. KISS and YAGNI apply.
- Relin2, RS2, Mult2, pair Add/Sub, second multiplication, serialization,
  performance, security, and plaintext precision are outside this review and
  must remain explicitly pending.

## Required source and test review

Inspect at least these exact candidate files:

- `include/openfhe_2023_1788/double_ckks.h`
- `src/double_ckks.cpp`
- `tests/dcp_rcb_test.cpp`
- `tests/tensor2_test.cpp`
- `tests/tensor2_api_contract_test.cpp`
- `CMakeLists.txt`
- `.github/workflows/dcp-rcb.yml`
- `coordination/TENSOR2_DESIGN.md`
- `coordination/INTEGRATION_REVIEW_CHECKLIST.md`
- all retained DCP/RCB and Tensor2 red/green evidence in
  `artifacts/tdd/dcp-rcb/` and `artifacts/tdd/tensor2/`

Against the paper and pristine OpenFHE source, verify:

1. centered signed CRT quotient/remainder semantics and tie convention;
2. exact RCB coefficient arithmetic, basis, component, format, level, scale,
   key-tag, slots, context-identity, and metadata behavior;
3. Tensor2's three raw products, exact omission of low-low, negacyclic wrap,
   and absence of pre-validation arithmetic;
4. independent-oracle integrity: expected results must not come from the
   implementation under test or from decrypt-only smoke checks;
5. immutability and metadata provenance, including deep state and pointer/
   alias behavior where tested;
6. stable full diagnostics for malformed inputs and validation precedence;
7. undefined behavior, overflow, integer narrowing, format transitions,
   nondeterminism, C++17/MinGW64 portability, and warning cleanliness;
8. whether the retained red-green evidence genuinely proves each nontrivial
   behavior rather than merely describing it.

The independent expected-value path must be disjoint from production arithmetic:

- DCP/RCB expected coefficients must come from test-owned
  `boost::multiprecision::cpp_int` CRT, centered quotient/remainder, and modular
  recombination. It must not call or reuse
  `DropLastElementAndScale`, project DCP/RCB, OpenFHE rescale/tower-drop, exact
  scalar multiply, or EvalAdd output to generate expectations.
- Check every ciphertext component, active tower, and coefficient, including
  witnesses immediately on both sides of the centered `(-q_div/2,q_div/2]`
  boundary and signed values whose reconstruction crosses an active modulus.
- Tensor2 expected coefficients must use test-owned `cpp_int` schoolbook
  negacyclic convolution. It must not call or reuse `EvalMultNoRelin`,
  `EvalMultCore`, `EvalAdd`, Tensor2, or any result derived from them.
- Require the explicit `X^(N-1)*X = -1` wrap witness, signed cross-modulus
  products, all three output components over every active tower/coefficient,
  and an independently nonzero `l1 tensor l2` witness that would fail if the
  deliberately omitted low-low term were accidentally added.

### Red-green provenance gate

Fetch complete connected candidate history, not a depth-1 final snapshot. For
each row below, verify that every named commit/tree exists, inspect the exact
test/source/evidence diff, bind the retained raw command/output/exit to that
tree, and prove that the red is caused by the named missing behavior. Compare
the relevant test blobs between red and green; any change must be explained and
must not weaken the failing assertion. Later source changes through `fb862a3`
must be covered by the independent current 6/6 rerun.

| Behavior | Test/red evidence anchors | First implementation/green anchors |
|---|---|---|
| Initial DCP/RCB oracle | `45239d419e12cc419764deba45066a78d50e0db3`, retained red `517633562b68c6adf1ce7afa0c955899963d5385` | implementation `de5995160fc9687531c9b47c25f8aca7f2eaaa70`, retained green `652d3d82cdcf59b171875fb6e9ae4e016301d4b2` |
| Hardened DCP/RCB contracts | `dbcacbbe31c95532012d3d854554f9ba64828e87`, retained red `bea4de48479bf90f1e74f22cc13640c07c535259` | implementation `e961022bc4e24fbcc4fbd29d2b6cf24f9c11d0e3`, retained green `e1153122be529ef21e9e5bce1ace877015410304` |
| Minimum first-multiplication basis | `61325ee41b94f0be355a357392cfae2c5bf5d0c7`, retained red `25c1ef258931c49d7f6c78e32c9eb20da81357d7` | implementation `bcf50df0b2236250c43bd80b653ede7bda5a51ff`, retained green `ff67408a87b0f561d4a4f5422313e49d2441cce7` |
| Precomputation access guard | test chain `0f08445eda4f641de014f2a265306b3f34d67fc8` through retained red `f15495216fc89184107955f2ca5c4120acd5fa04` | implementation `59bba42d386dd043bdcb4371014c42bb965befd9`, retained green `749c90d579602040127753a0f24da6d4aed33d63` |
| Pair encoding metadata | `dd3a254a2a0505f85267b55dc0e4fab083e69655`, retained red `7608650c4676d4f9ca1783789bf4abd5ab410ed0` | implementation `b0cd3adba690b71b6446ade8c038002efea4b6ec`, retained green `19dfd722599574b60b0396b187e30ce1832bf542` |
| Pair slot metadata | `d0cbc97190c9cc5be2164c7bcbff82109fd2ca55`, retained red `48d73e9d2c934f2afed91862de18ec7c7bde05c0` | implementation `4971d2292b5af0ddbbe0c7dbe5a2e87f45102ff1`, retained green `3521d6bbf6a7b773f57a25644c65e77c2e18f1fd` |
| Tensor2 API | `f3db12ef9fb0d13df0f779157eed168b8d582ea4`, retained red `b03cbae78594e5207d66c6299a12957968a51cbe` | scaffold `76bac1800553f79c1dbaff15ccebf6e50c65ad89`; final API/runtime green is bounded by the next row |
| Tensor2 runtime/precedence | test chain `6f17c55df7c94edbe390eff42d891522a0c86e17` and `482d27d0c43c22779aa548e00955ed90175dee97`, retained red `d2a42d72f3d2c74fb0ac0012e372fff198d679d3` | implementation `1408d46217e97a1c14d43d49b64791da22f652da`, retained green `b67016e8ccfd6042ff76e581cce892e985dbbde0` |
| Legacy DCP empty-key diagnostic | test-only `9d1d10a3414dce68b84d9887337254c275098d79`; hosted run `33436068864` has a strict Linux build and exactly `dcp_rcb` red because production emitted `ciphertext state` instead of required `pair state` (Windows cancelled and makes no test claim) | one-label production fix `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`; hosted run `33436252725` binds that exact head and passes Linux and Windows 6/6 |

Do not accept a summary line as red-green proof. If a retained transcript is
truncated, lacks the literal command/tree/raw exit, cannot be causally tied to
the missing behavior, or conflicts with the actual diff, record a finding. Do
not modify or recreate historical evidence in this review.

Use these paper anchors rather than relying on names alone:

- Definitions 3.1 and 3.3 plus their surrounding DCP/RCB construction and
  correctness discussion: extracted text lines 345–477;
- the `t=2` pair representation and scale setup preceding multiplication:
  extracted text lines 584–656;
- Definition 4.1 and its Tensor2 algebra/error discussion in the following
  paper section.

Inspect the corresponding pristine OpenFHE definitions and call paths, not only
project wrappers. At minimum locate and bind the exact official files/lines for
`DCRTPoly::DropLastElementAndScale`, ciphertext/element tower dropping,
`CryptoContextImpl::EvalMultNoRelin`, `LeveledSHEBase::EvalMultCore`,
FIXEDMANUAL level/depth alignment for EvalAdd/EvalMult, ciphertext `Clone`/
`CloneEmpty`, metadata-map copying, scale/noise-degree setters, and format
conversion. Explain which upstream behavior is relied upon and which behavior
is deliberately replaced by the project's independent integer/CRT oracle.

## Required Windows execution

Use the following workflow-equivalent Windows/MSYS2 MinGW64 flow with
parallelism no greater than 2. Replace only the previously unused review-root
suffix; retain literal commands, working directories, full stdout/stderr, raw
exit codes, and durations.

In PowerShell, establish exact LF checkouts and identities:

```powershell
$ProjectCommit = "fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9"
$ProjectTree = "759d5195739684748d5a9664edabe3fa719e1acf"
$OpenFHECommit = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
$ReviewRoot = "C:\20231788-review-fb862a3-01" # choose unused suffix
$ProjectSource = "$ReviewRoot\project-source"
$OpenFHESource = "$ReviewRoot\openfhe-source"
$OpenFHEBuild = "$ReviewRoot\openfhe-build"
$OpenFHEPrefix = "$ReviewRoot\openfhe-install"
$ProjectBuild = "$ReviewRoot\project-build"
$Output = "$ReviewRoot\output"
New-Item -ItemType Directory -Path $ReviewRoot | Out-Null
New-Item -ItemType Directory -Path $Output | Out-Null
git -c core.autocrlf=false -c core.eol=lf init $ProjectSource
git -C $ProjectSource remote add origin https://github.com/leemaple/20231788..git
git -C $ProjectSource -c core.autocrlf=false -c core.eol=lf fetch --no-tags origin refs/heads/agent/codex-tensor2-01:refs/remotes/origin/agent/codex-tensor2-01
git -C $ProjectSource -c core.autocrlf=false -c core.eol=lf checkout --detach FETCH_HEAD
git -C $ProjectSource rev-parse HEAD
git -C $ProjectSource write-tree
git -C $ProjectSource status --porcelain

git -c core.autocrlf=false -c core.eol=lf init $OpenFHESource
git -C $OpenFHESource remote add origin https://github.com/openfheorg/openfhe-development.git
git -C $OpenFHESource -c core.autocrlf=false -c core.eol=lf fetch --depth 1 origin $OpenFHECommit
git -C $OpenFHESource -c core.autocrlf=false -c core.eol=lf checkout --detach FETCH_HEAD
git -C $OpenFHESource -c core.autocrlf=false -c core.eol=lf submodule update --init --recursive --depth 1
git -C $OpenFHESource rev-parse HEAD
git -C $OpenFHESource status --porcelain
git -C $OpenFHESource submodule status --recursive
```

Explicitly compare the project HEAD/tree/status and OpenFHE HEAD/status to the
required values and abort with `BLOCKED` on mismatch. Do not continue merely
because the commands printed plausible values.

In the MSYS2 MinGW64 shell, report versions and execute:

```bash
cmake --version
g++ --version
make --version

review_root='/c/20231788-review-fb862a3-01' # use the same unused suffix
openfhe_source="$review_root/openfhe-source"
openfhe_build="$review_root/openfhe-build"
openfhe_prefix="$review_root/openfhe-install"
project_source="$review_root/project-source"
project_build="$review_root/project-build"
export OMP_NUM_THREADS=2
export OMP_THREAD_LIMIT=2
export CTEST_PARALLEL_LEVEL=1

cmake -S "$openfhe_source" -B "$openfhe_build" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$openfhe_prefix" \
  -DBUILD_UNITTESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_BENCHMARKS=OFF \
  -DBUILD_EXTRAS=OFF \
  -DWITH_OPENMP=ON
cmake --build "$openfhe_build" --parallel 2
cmake --install "$openfhe_build"

cmake -S "$project_source" -B "$project_build" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$openfhe_prefix"
cmake --build "$project_build" --parallel 2
export PATH="$openfhe_prefix/bin:$openfhe_prefix/lib:$PATH"
ctest --test-dir "$project_build" --show-only=json-v1 --parallel 1
ctest --test-dir "$project_build" --output-on-failure --parallel 1
```

The required retained execution boundaries are:

1. clean configure with warning-enabled Debug settings;
2. full project build;
3. `ctest --test-dir <build> --show-only=json-v1`;
4. unfiltered `ctest --test-dir <build> --output-on-failure`.

The candidate must register exactly this frozen bidirectional mapping:

| CTest name | Exact executable/argument route |
|---|---|
| `dcp_rcb` | `dcp_rcb_test` with no test selector |
| `tensor2_valid_arithmetic_immutability` | `tensor2_test valid_arithmetic_immutability` |
| `tensor2_result_scale_contract` | `tensor2_test result_scale_contract` |
| `tensor2_right_input_validation` | `tensor2_test right_input_validation` |
| `tensor2_mutual_compatibility` | `tensor2_test mutual_compatibility` |
| `tensor2_prearithmetic_key_compatibility` | `tensor2_test prearithmetic_key_compatibility` |

Prove `CMakeLists.txt` → complete CTest JSON command tuple → actual built
executable path → exact selector dispatch/target function in the test source,
and prove the reverse mapping has no extra or unreachable route. Confirm six
unique names, six unique full command tuples, six distinct intended behaviors,
and no disabled, skipped, `WILL_FAIL`, filtered, alias, empty, or substitute
route. A successful unfiltered run must report 6/6; do not claim success if any
full registration or execution output is unavailable.

Do not modify expected values, weaken assertions, suppress warnings, or change
the candidate to obtain green. If the exact candidate does not build or pass,
preserve the failure and diagnose it.

## Deliverables

Return one complete response and create deliverables only below the sibling
`output` directory, never in `project-source`:

1. `WINDOWS_REVIEW_fb862a3.md` containing:
   - exact candidate and OpenFHE identities;
   - environment and commands;
   - verdict `MERGEABLE`, `CHANGES NEEDED`, or `BLOCKED`;
   - P0/P1/P2/P3 findings, each with exact file:line, observed evidence,
     consequence, and minimal fix plus regression test;
   - explicit disposition of every review item above;
   - observed/inferred/pending claims kept separate.
2. `WINDOWS_TESTS_fb862a3.txt` containing full raw configure, build,
   registration JSON, and unfiltered CTest output with raw exits.
3. `WINDOWS_MANIFEST_fb862a3.sha256`, an ordered manifest covering the two
   deliverables and any conditional patch, with path, byte size, and SHA-256.

If no issue is found, say so only after completing the full review and test
matrix. A green build alone is not a code-review verdict. If a fix is needed,
generate a separate minimal patch only in another disposable clone, apply-check
it against a fresh clean detached `fb862a3`, and include its path/size/SHA in the
ordered manifest. Do not apply it to the candidate and do not change report/test
evidence to make it appear green.

After all review/build activity, re-run and retain candidate `rev-parse HEAD`,
`write-tree`, `status --porcelain`, and `git diff --exit-code`; all must prove
the source remains exact and unmodified. Re-run OpenFHE HEAD, status, and
recursive submodule status. Any unexpected source/dependency modification makes
the verdict `BLOCKED` until a new clean review is performed.

## Forbidden operations and claims

- Do not inspect or reuse the existing Windows implementation or any old local
  2023/1788/OpenFHE modifications.
- Do not commit, push, merge, rebase, open a PR, edit GitHub settings, dispatch
  CI, or overwrite/discard any existing work.
- Do not access credentials, browser state, cookies, tokens, `.env`, private
  keys, or unrelated files.
- Do not claim Mac, ChatGPT Pro, Fable5, hosted same-SHA, Relin2, RS2, Mult2,
  Add/Sub, precision, performance, or security work was performed.
- Do not report planned or inferred commands as executed. Missing evidence is
  `pending` or `CHANGES NEEDED`, never an assumed pass.

## Acceptance criteria

The review is acceptable only if it is bound to the exact two commits, starts
from a clean dedicated folder, never reads the stale Windows implementation,
reviews both specification and code, retains complete Windows build/test
evidence, accounts for all six tests, and gives a finding-by-finding verdict
that Codex can independently verify. Do not send a progress-only response;
finish naturally and return the complete deliverables once.
