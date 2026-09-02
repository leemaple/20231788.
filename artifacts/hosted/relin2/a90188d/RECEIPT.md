# Relin2 malformed-HYBRID-entry-basis runtime-green hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the complete HYBRID evaluation-key entry-basis contract is green on
Linux and Windows**. Both warning-clean project builds, both compile-only
Relin2 public API-contract builds, and all 17 registered runtime cases pass.
This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `a90188d0a83c8f138c177ab5a0114eb66e735d8b`;
- parent: `3bca03d118625b636dffb4491481dcda5995dccd`;
- tree: `891792982def9b3c908a1f9e4dcc5bc7868b0b4a`;
- local and remote implementation SHA matched after non-force push.

Only `src/double_ckks.cpp` changed from the accepted red source. CMake,
headers, workflow, and the complete test source are byte-unchanged. Production
adds one private source helper and one HYBRID-only guard after the accepted A/B
length checks. It obtains the bound context's public `ParamsQP` basis, visits
every A entry and then every B entry, and requires semantic equality of both
the aggregate DCRT parameters and every same-index tower parameter, including
an exact tower-count match. A null or incomplete parameter object is rejected
with the exact project diagnostic
`DoubleCKKS: Relin2 evaluation key HYBRID entry basis mismatch`.

The guard deliberately uses parameter-value equality rather than pointer
identity. The frozen test's independent-parameter positive control therefore
reaches and accepts the old scaffold. Its malformed negative control, which
only swaps the first two complete towers of the final B entry, reaches the new
guard and receives the exact diagnostic. In the green run, both paths complete
their immediate Tensor, deep metadata, A/B polynomial, cache-map/vector/key
pointer, context, and tag invariance checks. Spec, TDD, and Delivery/API
read-only source reviews each returned `PASS` before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33579691868`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33579691868`;
- exact head SHA: `a90188d0a83c8f138c177ab5a0114eb66e735d8b`;
- terminal run state: `completed/success`.

Linux job `100091129476` and Windows/MSYS2 MinGW64 job `100091129136` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Linux restored the already pinned
OpenFHE installation from its commit-keyed cache; Windows configured, built,
and installed pristine OpenFHE in this run. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded.

CTest reported exactly `17/17` passing on both platforms, including
`relin2_key_hybrid_entry_basis`. Linux reported `100% tests passed, 0 tests
failed out of 17` in 0.19 seconds; Windows reported `100% tests passed out of
17` in 0.34 seconds.

GitHub-hosted action infrastructure emitted Node-runtime deprecation notices,
including automatic Node 24 use for actions and cache-action Node module
deprecations. These are outside the C++ project/OpenFHE compilation and do not
change the successful warning-clean project-build step; they are retained in
the raw logs rather than suppressed.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records/logs, the complete logs ZIP, exact workflow, and source identity.
The ZIP passes `unzip -t`; the uploaded artifact count is zero. Before commit,
both the retained directory and a fresh expansion of the ZIP pass Gitleaks
8.30.1 and independent targeted filename/content scans. Every retained file
except the manifest itself is bound by `MANIFEST.sha256`.

The malformed-HYBRID-entry-basis contract is accepted green on Linux and
Windows. The next TDD slice remains unimplemented: this change does not
validate key-polynomial format, inspect BV shape, raise or mutate a ciphertext,
call key switching/relinearization, or begin Relin2 arithmetic. Valid input
still reaches the exact old `DoubleCKKS: Relin2 is not implemented` scaffold.
