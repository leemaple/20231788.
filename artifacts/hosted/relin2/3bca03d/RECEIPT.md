# Relin2 malformed-HYBRID-entry-basis runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the complete HYBRID evaluation-key entry-basis contract is red on
Linux and Windows**. Both warning-clean project builds and both compile-only
Relin2 API-contract builds pass; exactly the newly registered runtime case is
red against the old scaffold. This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `3bca03d118625b636dffb4491481dcda5995dccd`;
- parent: `0e240f37ca22fd683f50a108c40e3e4bfa62f593`;
- tree: `5caebeaa9887111ff5c409eb5dfb54fa88e8da65`;
- local, upstream, and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed; production
`include/` and `src/` are byte-unchanged. The seventeenth fixture generates a
real public `EvalKeyRelinImpl` in an exact HYBRID/`GetNumPartQ()==2`
configuration, proves both A and B vectors have length two, and proves every
entry initially has Evaluation format and the full ordered public `ParamsQP`
basis.

The same test first installs a positive-control copy of `B.back()` whose
aggregate parameter object and every tower parameter object have independent
identities while their complete parameter values, tower coefficient values,
formats, and DCRT polynomial value remain equal. That positive control reaches
and accepts the old scaffold, then completes immediate Tensor, deep metadata,
A/B polynomial, cache-map/vector/key-pointer, context, and tag invariance
checks. It prevents a pointer-identity basis implementation from falsely
passing the negative test.

The negative control starts from the valid generated B vector, swaps only the
first two complete towers of `B.back()`, and reconstructs that entry. It proves
the A/B lengths, entry count, Evaluation format, cyclotomic order, context,
tag, subtype, other B entry, and every unswapped tower remain unchanged. The
new entry has the same complete tower count but a different ordered basis.
Snapshots are taken after the deliberate mutation and immediately before the
production call. The exact expected diagnostic is
`DoubleCKKS: Relin2 evaluation key HYBRID entry basis mismatch`. Spec, TDD, and
Delivery/API read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33577480146`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33577480146`;
- exact head SHA: `3bca03d118625b636dffb4491481dcda5995dccd`;
- terminal run state: `completed/failure`, caused only by the intended new red.

Linux job `100084514002` and Windows/MSYS2 MinGW64 job `100084513764` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default builds
and both compile-only Relin2 public API-contract builds succeeded. CTest then
reported exactly `16/17` passing on each platform: tests 1 through 16 passed and
only `relin2_key_hybrid_entry_basis` failed. The Linux total was 0.13 seconds;
the Windows total was 0.33 seconds. Both platforms produced the same exact
failing observation:

`Relin2 test failure: Relin2 HYBRID evaluation-key entry basis threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The failure is inside the new exact-diagnostic helper, so this red receipt does
not claim that the negative case's later post-call invariance checks executed;
the paired green must execute them. The positive control ran earlier in that
same test and passed, proving the fixture is not an unconditional rejection.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records/logs, complete logs ZIP, exact workflow, and source identity. The
ZIP passes `unzip -t`; the uploaded artifact count is zero. Before commit, both
the retained directory and expanded ZIP must pass Gitleaks 8.30.1 and targeted
credential scans, then every retained file except the manifest itself is bound
by `MANIFEST.sha256`.

The malformed-HYBRID-entry-basis contract is accepted red on Linux and
Windows. The green change may only compare every A entry and then every B entry
against the bound context's complete ordered public `ParamsQP` basis after the
accepted HYBRID A/B length checks. It must use semantic parameter equality,
accept independently allocated equivalent parameter objects, reject with the
single exact project diagnostic, and leave the old scaffold as the next
behavior. It must not validate format, inspect BV shape, raise or mutate a
ciphertext, call key switching/relinearization, or begin Relin2 arithmetic.
