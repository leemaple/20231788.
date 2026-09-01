# Relin2 mismatched-actual-evaluation-key-tag runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the bound-context/nonmatching-actual-key-tag contract is red on
Linux**. The warning-clean project build and compile-only Relin2 API contract
pass; exactly the newly registered runtime case remains red against the old
scaffold. This is not a whole-algorithm result or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `82c9fa972e528a181a38fda4f3d71b9210547357`;
- parent: `ba4ca7b4c93fc4d750618418aeeaf1db6b1dd32d`;
- tree: `7185316cadd532dc358fea24f8ea2be9d8c321e6`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed; production
`include/` and `src/` are byte-unchanged. The thirteenth fixture uses the bound
context to generate one real nonnull relinearization key under the Tensor tag,
then changes only that cached key pointee's public actual tag. The map key,
context, concrete subtype, and generated A/B material remain valid. A test-owned
RAII guard exists before key generation and restores the initially empty
process cache on all exits. Tensor, map/vector/key pointer identities, key
context/tag, and full A/B snapshots are checked immediately after the
diagnostic helper. This red stops inside the helper on the wrong exception
type, so it does not prove those post-call checks executed; the future paired
green checkpoint must prove that. Spec, TDD, and Delivery/CI read-only reviews
each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33567295705`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33567295705`;
- exact head SHA: `82c9fa972e528a181a38fda4f3d71b9210547357`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100053280505` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest reported exactly `12/13`
passing. Tests 1 through 12 passed; only `relin2_key_wrong_tag` failed, with
the exact observation:

`Relin2 test failure: Relin2 wrong-tag first evaluation key threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100053280667` stopped while installing the official Windows toolchain and
supplies no project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The mismatched-actual-key-tag contract is accepted red on Linux. The green
change may only compare the first key's actual public tag with the Tensor tag
and reject mismatch with the exact diagnostic. It must not inspect subtype,
A/B vectors, key-switching technique, basis, or format; clone or raise a
ciphertext; call key switching; or begin Relin2 arithmetic.
