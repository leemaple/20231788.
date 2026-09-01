# Relin2 wrong-context-evaluation-key runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the nonnull-first-key/wrong-context contract is red on Linux**. The
warning-clean project build and compile-only Relin2 API contract pass; exactly
the newly registered runtime case remains red against the old scaffold. This
is not a whole-algorithm result or a Windows test result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `0a8f84039e61c36548987df2b68153754c519442`;
- parent: `37d1758b515a77e3a6f880f182462e223b2d2bd5`;
- tree: `9402cc6b28a7f1f3f342444749e890f9a5d850bc`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed; production
`include/` and `src/` are byte-unchanged. The twelfth public fixture creates a
second public OpenFHE context with the same element parameters and a different
batch size, generates one real nonnull relinearization key in that context,
and gives the foreign secret key the Tensor tag before generation. The global
cache therefore contains exactly one row and one concrete
`EvalKeyRelinImpl<DCRTPoly>` whose map tag, actual tag, subtype, and generated
A/B material are valid while its context identity alone differs from the
Tensor/module context. A test-owned RAII guard exists before key generation and
restores the initially empty process cache on all exits. Tensor, map/vector/key
pointer identities, key context/tag, and full A/B snapshots are checked
immediately after the diagnostic helper. This red stops inside the helper on
the wrong exception type, so it does not prove those post-call invariance
checks executed; the future paired green checkpoint must prove that. Spec,
TDD, and Delivery/CI read-only reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33564908322`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33564908322`;
- exact head SHA: `0a8f84039e61c36548987df2b68153754c519442`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100045745583` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest reported exactly `11/12`
passing in 0.18 seconds. Tests 1 through 11 passed; only
`relin2_key_wrong_context` failed, with the exact observation:

`Relin2 test failure: Relin2 wrong-context first evaluation key threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100045745807` stopped during pristine OpenFHE build and supplies no
project-build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The wrong-context-key contract is accepted red on Linux. The green change may
only compare the first nonnull key's public context identity with the bound
module context and reject mismatch with the exact diagnostic. It must not
inspect actual key tag, subtype, digit vectors, basis, format, clone or raise a
ciphertext, call key switching, or begin Relin2 arithmetic.
