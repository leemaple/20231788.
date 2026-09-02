# Relin2 malformed-HYBRID-B-vector-length runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the valid HYBRID key with only a malformed B-vector length contract
is red on Linux**. The warning-clean project build and compile-only Relin2 API
contract pass; exactly the newly registered runtime case remains red against
the old scaffold. This is not a whole-algorithm result or a Windows test
result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `0efab27ea1e2946db94b8621be53c2ecaaa282e8`;
- parent: `3645c4e02d3fb1eca4496b1de9be1ebb5517bac9`;
- tree: `c5b446c13357231b61faa1ec2539444c8f4404ef`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed; production
`include/` and `src/` are byte-unchanged. The sixteenth fixture proves the
bound parameters use HYBRID key switching with exact `GetNumPartQ()==2`, then
generates one real `EvalKeyRelinImpl` whose A and B vectors both begin at
length two. It copies B, removes only the final B entry, and installs that
length-one B while proving A, context, tag, subtype, map row, and pointer stay
valid. The production-call snapshots are all taken after this deliberate
mutation. A test-owned RAII guard exists before key generation and restores the
initially empty process cache on every exit; no claim is made for restoring a
pre-existing nonempty cache's mutated pointee. Tensor/deep-metadata and full
map/vector/key/A/B invariance checks immediately follow the diagnostic helper.
Spec, TDD, and Delivery/API read-only reviews each returned `PASS` before
commit.

The inherited wrong-subtype test's real-key positive control remains the
anti-false-green witness: an unconditional HYBRID B-length rejection would
make that inherited test fail even if the new negative test passed. The
inherited malformed-A test also remains green and proves the accepted A guard
still has priority for its own malformed input.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33574363738`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33574363738`;
- exact head SHA: `0efab27ea1e2946db94b8621be53c2ecaaa282e8`;
- terminal run state: `completed/cancelled` after Linux evidence capture.

Linux job `100074968553` ran against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The warning-clean default build
and compile-only Relin2 API contract succeeded. CTest reported exactly `15/16`
passing in 0.15 seconds. Tests 1 through 15 passed; only
`relin2_key_hybrid_b_length` failed, with CTest exit `8` and the exact
observation:

`Relin2 test failure: Relin2 HYBRID B-vector length threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The red stops inside the new diagnostic helper, so it does not claim the new
negative case's later post-call checks executed; the paired green must prove
them. Passing test 14 proves the inherited valid-key positive control remained
green.

After complete Linux log capture, the intermediate run was cancelled. Windows
job `100074968813` completed toolchain installation and stopped during
`Verify OpenFHE provenance`; it supplies no OpenFHE build, project build,
API-contract, or test claim.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both job
records/logs, complete logs ZIP, exact workflow and source identity. The ZIP
passes `unzip -t`; uploaded artifact count is zero. Before commit, both the
retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The malformed-HYBRID-B-length contract is accepted red on Linux. The green
change may only compare the already cast key's B-vector length with the bound
context's `GetNumPartQ()` when the technique is HYBRID and reject mismatch with
the exact project diagnostic. It must not read any A/B entry; validate
basis/format; clone or raise a ciphertext; call key switching; or begin Relin2
arithmetic.
