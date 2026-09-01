# Relin2 malformed-HYBRID-A-vector-length runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the valid HYBRID key with only a malformed A-vector length contract
is red on Linux**. The warning-clean project build and compile-only Relin2 API
contract pass; exactly the newly registered runtime case remains red against
the old scaffold. This is not a whole-algorithm result or a Windows test
result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `b50448f561ba62f9a6271779f5c1eeb4903cac4e`;
- parent: `331dd7d64d53e6a87d94555a00aab29b1eaa998e`;
- tree: `1732a4bc99676723732b47507549390f9669a0b8`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed; production
`include/` and `src/` are byte-unchanged. The fifteenth fixture proves the
bound parameters use HYBRID key switching with exact `GetNumPartQ()==2`, then
generates one real `EvalKeyRelinImpl` whose A and B vectors both begin at
length two. It copies A, removes only the final A entry, and installs that
length-one A while proving B, context, tag, subtype, map row, and pointer stay
valid. The production-call snapshots are all taken after this deliberate
mutation. A test-owned RAII guard exists before key generation and restores the
initially empty process cache on every exit; no claim is made for restoring a
pre-existing nonempty cache's mutated pointee. Tensor/deep-metadata and full
map/vector/key/A/B invariance checks immediately follow the diagnostic helper.
Spec, TDD, and Delivery/CI read-only reviews each returned `PASS` before
commit.

The inherited wrong-subtype test's real-key positive control remains the
anti-false-green witness: an unconditional HYBRID A-length rejection would
make that inherited test fail even if the new negative test passed.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33572538385`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33572538385`;
- exact head SHA: `b50448f561ba62f9a6271779f5c1eeb4903cac4e`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100069389984` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest reported exactly `14/15`
passing in 0.15 seconds. Tests 1 through 14 passed; only
`relin2_key_hybrid_a_length` failed, with CTest exit `8` and the exact
observation:

`Relin2 test failure: Relin2 HYBRID A-vector length threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The red stops inside the new diagnostic helper, so it does not claim the new
negative case's later post-call checks executed; the paired green must prove
them. Passing test 14 proves the inherited valid-key positive control remained
green.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100069390165` stopped during `Install official Windows toolchain` and
supplies no OpenFHE build, project build, API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The malformed-HYBRID-A-length contract is accepted red on Linux. The green
change may only compare the already cast key's A-vector length with the bound
context's `GetNumPartQ()` when the technique is HYBRID and reject mismatch with
the exact project diagnostic. It must not read B or any A entry; validate
basis/format; clone or raise a ciphertext; call key switching; or begin Relin2
arithmetic.
