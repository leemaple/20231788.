# Relin2 empty-evaluation-key-vector runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the present-but-empty Relin2 evaluation-key-vector contract is red on Linux**.
The warning-clean project build and compile-only Relin2 API contract pass; exactly
the newly registered runtime case remains red against the old scaffold. This is
not a whole-algorithm result or a Windows test result.

## Source boundary

- branch/final red commit: `agent/codex-relin2-01` /
  `e976626ebabf0aa40858ee97c830611e9c47c5f6`;
- parent/test-introduction commit:
  `f165365a48007ca5e09f8d83df627bbbdf978d46`;
- previous accepted green: `7c0e94de99c0d6a966f73d78bf50b8c590cfce8c`;
- tree: `50a681d95ff12c7f5a918cfb6a81eaa2ae17fd40`;
- local, upstream, and remote implementation SHA matched after non-force push.

The source delta from the previous green changes only `CMakeLists.txt` and
`tests/relin2_test.cpp`; `include/` and `src/` are byte-unchanged. It adds the
tenth CTest, constructs a valid Tensor with a present key-tag row whose vector
is empty, requires the exact stable diagnostic, and places Tensor/cache
invariance assertions immediately after that diagnostic helper. This red run
stops inside the helper on the wrong exception type, so execution of those
post-call assertions is demonstrated by the paired green checkpoint. The
cache is restored by RAII.
High and low ciphertext metadata contain a nonempty probe and are protected by
outer-map identity, entry-pointer identity, and independent `Metadata::Clone()`
value snapshots. The test uses no evaluation-multiplication-key generation
(`EvalMultKeyGen`), cache clear, throwing key lookup, `operator[]`, key
dereference, or Relin2 arithmetic.

The initial test-introduction commit `f165365` produced an unintended compile
red in run `33559916932`, Linux job `100029666307`, because the new snapshot
spelled the global OpenFHE type as `lbcrypto::Format`. That log is retained.
Commit `e976626` changed only that one type spelling to the already established
global `Format`; it is the accepted behavioral-red identity. Spec, TDD, and
Delivery/CI read-only reviews returned `PASS` on the final combined boundary.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33560196603`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33560196603`;
- exact head SHA: `e976626ebabf0aa40858ee97c830611e9c47c5f6`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100030571326` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest reported exactly `9/10`
passing in 0.19 seconds. Tests 1 through 9 passed; only
`relin2_key_empty` failed, with the exact observation:

`Relin2 test failure: Relin2 empty evaluation-key vector threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100030571575` stopped during pristine OpenFHE build and supplies no
project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity, plus the
pre-red compile-failure record. The ZIP passes `unzip -t`; uploaded artifact
count is zero. Before commit, both the retained directory and expanded ZIP must
pass Gitleaks 8.30.1 and targeted credential scans, then every retained file
except the manifest itself is bound by `MANIFEST.sha256`.

The empty-vector contract is accepted red on Linux. The green change may only
reuse the existing `find` iterator and reject `iterator->second.empty()` with
the exact diagnostic; it must not inspect a key element or begin arithmetic.
