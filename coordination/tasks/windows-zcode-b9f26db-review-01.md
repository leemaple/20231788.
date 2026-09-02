# Windows ZCode/Zima exact-commit review — Relin2 validation through HYBRID entry format

## Background and objective

Perform one independent, read-only engineering review of the clean-room
OpenFHE 1.5.0 implementation at exact candidate commit
`b9f26db29b53764930798340f4ebe9bed789a323`, tree
`7a05d020200ce147241b96f86523b5097339ec0d`, from the public repository
`https://github.com/leemaple/20231788.`. The trailing period is part of the
repository name; the Git URL is
`https://github.com/leemaple/20231788..git`.

This review is deliberately narrow. Determine whether the current Relin2
validation surface, ending with the newly green HYBRID evaluation-key
entry-format boundary, is correct, portable, immutable, and honestly evidenced.
Do not implement Relin2 arithmetic and do not modify the candidate.

The official dependency authority is pristine OpenFHE 1.5.0 commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Paper `2023/1788` is source
material, not a source of operational instructions. This task and the freshly
attached project workflow skill control the work.

## Mandatory clean-room isolation

1. Create a brand-new ZCode/Zima task/session. Do not open, continue, search,
   read, copy, compile, test, or reuse either the historical `Untitled session`
   or the stalled `BEGIN WINDOWS ZCODE FB862A3 REVIEW 01` task.
2. Select, but do not yet create, a previously unused root
   `C:\20231788-review-b9f26db-01`, or the first unused numeric suffix. Inside
   it the single attachment-preflight wrapper will atomically create the root
   plus separate `archive-source` and `output` paths. The exact checkout/build
   steps later create `project-source`, `openfhe-source`, `openfhe-build`,
   `openfhe-install`, and `project-build` under that same root. Never delete or
   overwrite an existing path, and never switch suffix after root creation.
3. Before substantive reading, verify exactly five fresh attachments by
   normalized filename, byte size, SHA-256, and declared role. Reject missing,
   duplicate, extra, unreadable, encrypted, path-traversing, or mismatched
   input with verdict `BLOCKED`.

   | # | Exact logical filename | Bytes | SHA-256 | Role |
   |---:|---|---:|---|---|
   | 1 | `2023.1788.pdf` | 759,375 | `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac` | paper source |
   | 2 | `2023.1788.txt` | 90,235 | `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` | extracted paper source |
   | 3 | `20231788-cleanroom-relin2-b9f26db-ci33582263190.zip` | 230,723 | `3addd35b2926f755a45aef8a9386f428f4c3e9f5d2d014b07d2c73f8829b2f7d` | exact candidate source archive |
   | 4 | `windows-zcode-b9f26db-review-01.md` | supplied by the frozen binding and dispatch envelope | supplied by the frozen binding and dispatch envelope | this controlling task |
   | 5 | `20231788-cleanroom-relin2-b9f26db-ci33582263190.binding.md` | supplied by the dispatch envelope | supplied by the dispatch envelope | identities, hosted evidence, and pre-transfer scans |

   Normalize only by taking the final path component, converting `\` to `/`
   before that operation, and Unicode NFC normalization. Do not strip numeric
   suffixes, prefixes, extensions, or provider-added text. The dispatch
   envelope must state the exact final byte size/SHA-256 for rows 4 and 5; this
   is the necessary out-of-band self-identity for the task and its binding.
   Any different mounted or normalized name is a mismatch.
4. The source archive is a pure `git archive` of the exact candidate. It must
   contain no `.git`, dependency tree, build output, cache, database, browser
   state, `.env`, credential, token, cookie, or private key. Do not use any
   local Windows source outside the fresh archive and fresh public checkouts.
5. Keep the Windows host responsive: set `OMP_NUM_THREADS=2`,
   `OMP_THREAD_LIMIT=2`, `CTEST_PARALLEL_LEVEL=1`; use build parallelism at
   most 2 and CTest `--parallel 1`.

## Architecture and boundaries that must remain intact

- `DoubleCKKS` is bound to one exact `CryptoContext<DCRTPoly>`.
- `TensorCiphertextPair` is private-state/read-only and must be fully validated
  before the evaluation-key cache is read.
- Relin2 currently validates only; it deliberately ends at
  `DoubleCKKS: Relin2 is not implemented` for an otherwise valid input.
- The accepted validation order is: Tensor state; active tower count greater
  than or equal to the Tensor noise-scale degree (currently degree three, hence
  at least three towers in the accepted fixture);
  evaluation-key row exists and is nonempty; first key non-null; exact context;
  exact tag; exact `EvalKeyRelinImpl<DCRTPoly>` subtype; HYBRID A length;
  HYBRID B length; every HYBRID A then B entry has the complete ordered public
  `ParamsQP` basis; every HYBRID A then B aggregate and NativePoly tower is in
  Evaluation format; only then the scaffold.
- Rejection must not mutate the Tensor ciphertext, deep metadata, evaluation
  key A/B polynomials, cache map/vector/key identity, key context/tag, or leave
  a test mutation installed after RAII restoration.
- DCP, RCB, and Tensor2 are inherited accepted functionality. Review only for
  regression from this Relin2 slice; do not redesign them.
- BV-specific evaluation-key shape, ciphertext basis raising, public
  relinearization/key switching, `(u, v+w)` recombination, RS2, Mult2, pair
  Add/Sub, serialization, performance, security estimates, and plaintext
  precision are outside this task and must remain explicitly pending.
- Do not expand the public API, add setters/friends, introduce catch-all or
  production `try`/`catch`, or add speculative abstraction. KISS and YAGNI
  apply.

## Required source and test review

Inspect at minimum:

- `.agents/skills/openfhe-2023-1788-workflow/SKILL.md`;
- `include/openfhe_2023_1788/double_ckks.h`;
- `src/double_ckks.cpp`;
- `tests/relin2_test.cpp`;
- `tests/relin2_api_contract_test.cpp`;
- `CMakeLists.txt`;
- `.github/workflows/dcp-rcb.yml`;
- `coordination/INTEGRATION_REVIEW_CHECKLIST.md`;
- `coordination/INDEPENDENT_ORACLE_PLAN.md`;
- `artifacts/hosted/relin2/b9f26db/RECEIPT.md` from the exact public green
  evidence branch identified below;
- the attached binding and the public red/green evidence branches named there.

Against the paper and pristine OpenFHE source, verify all of the following:

1. The public Relin2 signature and return type compile without exposing private
   construction or accepting mutable input.
2. The new helper uses OpenFHE 1.5.0's actual global `Format` type, not the
   invalid spelling `lbcrypto::Format`.
3. HYBRID basis rejection precedes format rejection. Basis checks cover the
   aggregate parameters and every same-index tower semantically, not pointer
   identity. Format checks cover the DCRTPoly aggregate and every NativePoly
   tower.
4. All A entries are checked before all B entries, and the exact diagnostic is
   `DoubleCKKS: Relin2 evaluation key HYBRID entry must be in evaluation format`.
5. The valid positive control still reaches the scaffold. The negative control
   changes only the first A entry to Coefficient format while lengths, basis,
   other A entries, all B entries, context, tag, and subtype remain valid.
6. Every production Relin2 call is followed immediately by the required
   Tensor/cache/key/metadata invariance checks; exception helpers compare full
   type and exact message rather than substrings.
7. No new undefined behavior, lifetime/alias bug, narrowing, nondeterminism,
   C++17/MinGW64 incompatibility, hidden coefficient transform, or validation
   side effect was introduced.
8. The test is a genuine red-green boundary: red commit
   `b1f4459d9e1d3009da5420954f26384b96ba3e57` changed only CMake/test and failed
   exactly test 18 on both platforms; green `b9f26db...` changed only
   `src/double_ckks.cpp` by 22 added lines while the test and CMake blobs stayed
   byte-identical.
9. Claims remain narrow. A green format guard is not proof of BV support or
   Relin2 arithmetic.

Use pristine OpenFHE definitions and call sites to support API conclusions; do
not infer behavior from project names alone. Separate observed source facts,
your reasoned conclusions, and anything you could not verify.

## Required Windows verification

### Attachment and archive preflight

Before extraction, retain the exact PowerShell/Python commands, tool versions,
working directories, complete output, and raw exits that prove:

- exactly the five inventory rows above, with row 4 and row 5 equal to the
  dispatch envelope;
- source ZIP SHA-256 equals the table, contains exactly 98 central-directory
  members and 75 regular files, and has zero duplicate normalized names,
  encrypted members, symlink/reparse members, absolute/drive/UNC/traversal
  paths, or forbidden names/directories;
- forbidden content includes `.git`, `node_modules`, build/dist/target output,
  caches, databases, runtime/browser/profile state, `.env`, cookies, login
  databases, API keys, tokens, private keys, and credential files;
- Gitleaks 8.30.1 with redaction scans both the five-file attachment directory
  and a fresh safe expansion of the ZIP with exit 0/no findings; retain the
  Gitleaks resolved path, version, binary SHA-256, reports, stderr, and raw
  exits;
- independent high-signal sensitive-filename and credential-content scans find
  no match. If a scan finds something, record only a sanitized path/count and
  never copy matching secret text into an output.

The retained PowerShell wrapper must first fail if the selected root exists,
then create that root plus empty `archive-source` and `output` directories
exactly once. Set `$ArchiveSource` to that new `archive-source`; the Python
script may extract only there. Later checkout commands must reuse this same
root and must not repeat the existence check or choose another suffix.

Use an inline, retained `python -` script based on `zipfile`, `pathlib`,
`unicodedata`, and `stat` to inspect the central directory before calling
`extractall`. It must check `flag_bits & 1`, Unix file type from
`external_attr >> 16`, DOS reparse bit `0x400`, both `/` and `\` separators,
NFC-normalized duplicates, empty/`.`/`..` components, leading `/` or `//`, and
drive-prefixed paths. Assert the exact 98/75 counts and call `extractall` only
after every assertion passes. Because this exact Git ZIP has
`ZipInfo.create_system == 0`, count a non-directory entry as regular when its
Unix mode/type bits are zero and its DOS reparse bit is clear. If Unix type
bits are nonzero, accept only a regular file or directory consistent with
`is_dir()`; reject every other type. After extraction, PowerShell must also
prove `Get-ChildItem -Recurse -Attributes ReparsePoint` returns zero. A
missing Gitleaks or Python gate is `BLOCKED`, not permission to skip it.

### Exact checkout preflight

The attachment-preflight wrapper must already have selected and recorded the
first unused suffix before creating the root. The commands below assume it
selected `-01`; if it recorded another suffix, replace only the root literal
consistently. Never recheck availability or switch suffix here. Run from
PowerShell with the complete transcript retained:

```powershell
$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $false
$ProjectCommit = "b9f26db29b53764930798340f4ebe9bed789a323"
$ProjectTree = "7a05d020200ce147241b96f86523b5097339ec0d"
$OpenFHECommit = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
$ReviewRoot = "C:\20231788-review-b9f26db-01"
$ProjectSource = "$ReviewRoot\project-source"
$OpenFHESource = "$ReviewRoot\openfhe-source"
$OpenFHEBuild = "$ReviewRoot\openfhe-build"
$OpenFHEPrefix = "$ReviewRoot\openfhe-install"
$ProjectBuild = "$ReviewRoot\project-build"
$Output = "$ReviewRoot\output"
$ArchiveSource = "$ReviewRoot\archive-source"
if (-not (Test-Path -LiteralPath $ReviewRoot -PathType Container)) {
  throw "Attachment preflight did not create the selected review root"
}
if (-not (Test-Path -LiteralPath $ArchiveSource -PathType Container)) {
  throw "Attachment preflight did not create archive-source"
}
if (-not (Test-Path -LiteralPath $Output -PathType Container)) {
  throw "Attachment preflight did not create output"
}
if (Test-Path -LiteralPath $ProjectSource) { throw "project-source already exists" }
if (Test-Path -LiteralPath $OpenFHESource) { throw "openfhe-source already exists" }
New-Item -ItemType Directory -Path $ProjectSource,$OpenFHESource | Out-Null

function Assert-NativeSuccess([string]$Label) {
  $Code = $LASTEXITCODE
  Write-Output "$Label RAW_EXIT=$Code"
  if ($Code -ne 0) { throw "$Label failed with exit $Code" }
}

git -c core.autocrlf=false -c core.eol=lf init $ProjectSource
Assert-NativeSuccess "project git init"
git -C $ProjectSource remote add origin https://github.com/leemaple/20231788..git
Assert-NativeSuccess "project remote add"
git -C $ProjectSource -c core.autocrlf=false -c core.eol=lf fetch --depth 1 origin $ProjectCommit
Assert-NativeSuccess "project fetch"
git -C $ProjectSource -c core.autocrlf=false -c core.eol=lf checkout --detach FETCH_HEAD
Assert-NativeSuccess "project checkout"
$ProjectHeadBefore = git -C $ProjectSource rev-parse HEAD
Assert-NativeSuccess "project HEAD"
$ProjectTreeBefore = git -C $ProjectSource rev-parse 'HEAD^{tree}'
Assert-NativeSuccess "project tree"
$ProjectStatusBefore = @(git -C $ProjectSource status --porcelain=v1 --untracked-files=all)
Assert-NativeSuccess "project status"
if ($ProjectHeadBefore -ne $ProjectCommit) { throw "Project HEAD mismatch" }
if ($ProjectTreeBefore -ne $ProjectTree) { throw "Project tree mismatch" }
if ($ProjectStatusBefore.Count -ne 0) { throw "Project source is dirty" }

git -c core.autocrlf=false -c core.eol=lf init $OpenFHESource
Assert-NativeSuccess "OpenFHE git init"
git -C $OpenFHESource remote add origin https://github.com/openfheorg/openfhe-development.git
Assert-NativeSuccess "OpenFHE remote add"
git -C $OpenFHESource -c core.autocrlf=false -c core.eol=lf fetch --depth 1 origin $OpenFHECommit
Assert-NativeSuccess "OpenFHE fetch"
git -C $OpenFHESource -c core.autocrlf=false -c core.eol=lf checkout --detach FETCH_HEAD
Assert-NativeSuccess "OpenFHE checkout"
git -C $OpenFHESource -c core.autocrlf=false -c core.eol=lf submodule update --init --recursive --depth 1
Assert-NativeSuccess "OpenFHE submodule update"
$OpenFHEHeadBefore = git -C $OpenFHESource rev-parse HEAD
Assert-NativeSuccess "OpenFHE HEAD"
$OpenFHEStatusBefore = @(git -C $OpenFHESource status --porcelain=v1 --untracked-files=all)
Assert-NativeSuccess "OpenFHE status"
$OpenFHESubmodulesBefore = @(git -C $OpenFHESource submodule status --recursive)
Assert-NativeSuccess "OpenFHE submodule status"
if ($OpenFHEHeadBefore -ne $OpenFHECommit) { throw "OpenFHE HEAD mismatch" }
if ($OpenFHEStatusBefore.Count -ne 0) { throw "OpenFHE source is dirty" }
if ($OpenFHESubmodulesBefore | Where-Object { $_ -match '^[+\-U]' }) {
  throw "OpenFHE submodule mismatch"
}
```

Compare the safe expanded archive to the tracked public checkout while
excluding only the checkout's `.git` directory. Require byte equality and no
missing/extra file. Retain the literal comparison command and raw exit.

### Exact MSYS2 MinGW64 build and tests

Use an existing official MSYS2 MinGW64 installation, or packages only from its
configured official repositories. Report `pacman`, CMake, GCC, Ninja, and
Boost package identities. The generator is fixed to Ninja. A missing tool is
`BLOCKED`; do not silently change
compiler, architecture, OpenFHE version, or build flags. In the MinGW64 shell,
run this exact flow after substituting only the already recorded unused root
suffix:

```bash
set -euo pipefail
export OMP_NUM_THREADS=2
export OMP_THREAD_LIMIT=2
export CTEST_PARALLEL_LEVEL=1
project_source="$(cygpath -u 'C:\20231788-review-b9f26db-01\project-source')"
openfhe_source="$(cygpath -u 'C:\20231788-review-b9f26db-01\openfhe-source')"
openfhe_build="$(cygpath -u 'C:\20231788-review-b9f26db-01\openfhe-build')"
openfhe_prefix="$(cygpath -u 'C:\20231788-review-b9f26db-01\openfhe-install')"
project_build="$(cygpath -u 'C:\20231788-review-b9f26db-01\project-build')"

cmake --version
g++ --version
ninja --version
pacman -Q mingw-w64-x86_64-boost mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-gcc mingw-w64-x86_64-ninja

cmake -G Ninja -S "$openfhe_source" -B "$openfhe_build" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$openfhe_prefix" \
  -DBUILD_UNITTESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_BENCHMARKS=OFF \
  -DBUILD_EXTRAS=OFF \
  -DWITH_OPENMP=ON
cmake --build "$openfhe_build" --parallel 2
cmake --install "$openfhe_build"

cmake -G Ninja -S "$project_source" -B "$project_build" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$openfhe_prefix"
cmake --build "$project_build" --parallel 2
cmake --build "$project_build" --target relin2_api_contract_test --parallel 2
export PATH="$openfhe_prefix/bin:$openfhe_prefix/lib:$PATH"
ctest --test-dir "$project_build" --output-on-failure --parallel 1
ctest --test-dir "$project_build" \
  -R '^relin2_key_hybrid_entry_format$' --output-on-failure --parallel 1
```

Retain literal command, shell, working directory, complete stdout/stderr, raw
exit, and duration for:

1. toolchain versions and both source identities;
2. project configure;
3. warning-clean default project build;
4. compile-only `relin2_api_contract_test` target;
5. full CTest with `--output-on-failure --parallel 1`;
6. targeted `ctest -R '^relin2_key_hybrid_entry_format$' --output-on-failure --parallel 1`.

### Fail-closed postflight

After all reading, builds, tests, and output drafting—but before the manifest
and verdict—rerun the PowerShell identities. Require project `HEAD` and tree to
equal the constants, project status to be empty, both `git diff --exit-code`
and `git diff --cached --exit-code` to return 0, OpenFHE `HEAD` to equal the
constant, OpenFHE status to be empty, and recursive submodule output to equal
the exact preflight output with no `+`, `-`, or `U` prefix. Retain commands,
complete output, and raw exits in `TESTS.md`. Any mismatch is `BLOCKED`; never
restore or clean a source tree merely to make postflight pass.
Use the same `Assert-NativeSuccess` helper after every postflight `git` command
and comparison command so `TESTS.md` retains each native raw exit; PowerShell's
exception preference alone is not accepted as native-command evidence.

If an independent Windows build cannot be completed, do not infer or copy the
GitHub result. Record the exact command, exit, and blocker, continue the static
review where possible, and use verdict `BLOCKED` if the missing execution
prevents acceptance.

Known evidence is disclosed but is not your own execution: GitHub Actions run
`33582263190` at exact `b9f26db...` completed `18/18` on Linux job
`100098827445` and Windows job `100098827360`. Its accepted evidence commit is
`8b2a7dcb2157e8cdd72e0821fdc58553a42e8f42` on branch
`evidence/relin2-hosted-b9f26db`, with 19-entry manifest SHA-256
`b1961d4e60623686e957f94a9fbb5a1d51774c6a64d1981e477969d4be136de3`.
Independently inspect it and report any mismatch; never relabel it as ZCode's
own build.

The paired red is source `b1f4459d9e1d3009da5420954f26384b96ba3e57`,
tree `30b926d2c06f39238310a714d94fbb70716247c4`, run `33581151491`,
Linux job `100095528356`, and Windows job `100095528194`. Its evidence branch
is `evidence/relin2-hosted-b1f4459`, commit
`76f9c5eea659c19f0d7211f8c3f04430a6591d1d`, tree
`c2fb81757ce24322ac905b6fb86c097901cf72bb`, with 19-entry manifest SHA-256
`cc4091acd00209d3e02786fa3bd45e88fe29af7f29fa5aa9013a6e346a9c0219`.
Both platforms passed the build/API gates and reported exactly `17/18`, with
only `relin2_key_hybrid_entry_format` red against the old scaffold.

## Deliverables

Write only under the fresh root's `output` directory:

1. `REVIEW.md` — exact identities, reviewed files/OpenFHE anchors, findings in
   descending severity with `file:line` evidence, validation-order table,
   scope/non-scope statement, and final verdict `PASS`, `CHANGES NEEDED`, or
   `BLOCKED`.
2. `TESTS.md` — literal commands, working directories, complete raw exits,
   durations, attachment/archive/Gitleaks/targeted-scan gates, preflight and
   postflight identities, full versus targeted test counts, and clear
   separation of your execution from disclosed GitHub evidence.
3. `MANIFEST.sha256` — SHA-256 for every output file except the manifest itself.

Do not edit source, create a patch, commit, push, open a PR, rerun GitHub
Actions, send messages, or claim execution not present in retained raw output.
Do not stop at a progress summary: return the complete verdict and all three
deliverables. A slow task must continue naturally without asking Codex to
resend or restart it.

## Acceptance criteria

`PASS` is allowed only if exact identities and all five attachment rows match;
the central-directory, safe-extraction, Gitleaks, targeted-scan, archive/source
equality, initial identity, and final identity gates all pass; the review covers
every required source/API/test/evidence item; Windows verification completes
with warning-clean build, API target, and exact `18/18` plus targeted `1/1`;
no P0/P1/P2 finding remains; output hashes verify; and the report makes no
claim beyond the validation-through-HYBRID-format slice. Otherwise return
`CHANGES NEEDED` with actionable evidence, or `BLOCKED` with the exact failed
gate and completed partial work.
