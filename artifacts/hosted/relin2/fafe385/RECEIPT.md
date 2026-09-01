# Relin2 wrong-concrete-evaluation-key-subtype runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the valid-row/nonnull/same-context/same-tag but wrong concrete key
subtype contract is red on Linux**. The warning-clean project build and
compile-only Relin2 API contract pass; exactly the newly registered runtime
case remains red against the old scaffold. This is not a whole-algorithm
result or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `fafe3850d7de4b6e725eed707f32865ff0d2de7d`;
- parent: `143b62489a3bf1dc29075a3ce16071b822bce350`;
- previous accepted green: `4bfe4fcf5d2c095eb99cdfd9f1df59f261b0b72d`;
- tree: `51168b6f4759326cbf66f75c949b54f742da7a43`;
- local, upstream, and remote implementation SHA matched after non-force push.

The cumulative boundary since accepted green `4bfe4fc` changes only
`CMakeLists.txt` and `tests/relin2_test.cpp`; production `include/` and `src/`
are byte-unchanged. The fourteenth CTest first generates a real, bound-context,
same-tag `EvalKeyRelinImpl` positive control. It accepts the exact current base
`std::logic_error("DoubleCKKS: Relin2 is not implemented")` or a future normal
return, rejects derived logic errors and all other diagnostics, then performs
the complete immediate Tensor/deep-metadata and map/vector/key context/tag/A/B
invariance checks and restores the empty cache.

The same CTest then inserts one exact public base
`EvalKeyImpl<DCRTPoly>` under the Tensor-tag row. The pointer is nonnull and its
context and actual tag are valid, but an `EvalKeyRelinImpl` dynamic cast is
null. The fixture deliberately never calls the base object's throwing A/B
getters. A second RAII guard exists before construction/insertion and restores
the initially empty cache on all exits. Spec, TDD, and Delivery/CI read-only
reviews each returned `PASS` before the accepted red was recorded.

The initial test-introduction commit `143b624` produced an unintended compile
red in run `33569890990`, Linux job `100061330826`: the new helper compared
`exception.what()` directly with a string literal, and `-Werror=address`
rejected that unspecified pointer comparison. Its run/job/log are retained.
That checkpoint is not the accepted behavioral red and makes no CTest-result
claim.
Commit `fafe385` changed only that comparison to construct `std::string`; it is
the accepted behavioral-red identity. Neither checkpoint changed production.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33570184376`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33570184376`;
- exact head SHA: `fafe3850d7de4b6e725eed707f32865ff0d2de7d`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100062228391` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest reported exactly `13/14`
passing in 0.20 seconds. Tests 1 through 13 passed; only
`relin2_key_wrong_subtype` failed, with CTest exit `8` and the exact
observation:

`Relin2 test failure: Relin2 wrong-subtype first evaluation key threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

That negative-control label proves the correct-subtype positive control and
all of its immediate post-call checks completed first. The red stops inside
the negative control's diagnostic helper, so it does not claim that the later
negative-control post-call checks executed; the paired green must prove them.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100062228486` stopped during `Install official Windows toolchain` and
supplies no OpenFHE build, project build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity, plus the
pre-red compile-failure record. The ZIP passes `unzip -t`; uploaded artifact
count is zero. Before commit, both the retained directory and expanded ZIP must
pass Gitleaks 8.30.1 and targeted credential scans, then every retained file
except the manifest itself is bound by `MANIFEST.sha256`.

The wrong-concrete-subtype contract is accepted red on Linux. The green change
may only dynamically cast the already validated first key to
`EvalKeyRelinImpl<DCRTPoly>` and reject a null cast with the exact project-owned
diagnostic before any A/B getter. It must not validate A/B shape, technique,
basis, or format; clone or raise a ciphertext; call key switching; or begin
Relin2 arithmetic.
