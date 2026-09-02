# Relin2 malformed-HYBRID-entry-format runtime-red hosted receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the HYBRID evaluation-key entry-format contract is red on Linux and
Windows**. Both warning-clean project builds and both compile-only Relin2
public API-contract builds pass; exactly the newly registered runtime case is
red against the old scaffold. This is not a whole-Relin2 arithmetic result.

## Source boundary

- branch/commit: `agent/codex-relin2-01` /
  `b1f4459d9e1d3009da5420954f26384b96ba3e57`;
- parent: `a90188d0a83c8f138c177ab5a0114eb66e735d8b`;
- tree: `30b926d2c06f39238310a714d94fbb70716247c4`;
- local and remote implementation SHA matched after non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed. Production headers
and source are byte-unchanged from the accepted HYBRID entry-basis green. The
eighteenth fixture generates a real public `EvalKeyRelinImpl` in an exact
HYBRID/`GetNumPartQ()==2` context, proves both A/B vectors have exact length,
and proves every aggregate entry and every NativePoly tower initially has
Evaluation format on the complete ordered public `ParamsQP` basis.

The valid-key positive control first reaches and accepts the old scaffold,
then completes immediate Tensor, deep metadata, A/B polynomial,
cache-map/vector/key-pointer, context, and tag invariance checks. The negative
control copies the generated A vector and calls public `SetFormat` only on its
first entry, so that entry and all its towers become Coefficient format while
the A/B lengths, shared complete basis, remaining A entries, and complete B
vector stay valid. Snapshots are taken after the deliberate mutation and
immediately before the production call. The exact expected diagnostic is
`DoubleCKKS: Relin2 evaluation key HYBRID entry must be in evaluation format`.
Spec, TDD, and Delivery/API read-only source reviews each returned `PASS`
before commit.

## Hosted observation

- workflow/run: `OpenFHE 2023/1788 TDD` / `33581151491`, attempt `1`, event
  `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33581151491`;
- exact head SHA: `b1f4459d9e1d3009da5420954f26384b96ba3e57`;
- terminal run state: `completed/failure`, caused only by the intended new red.

Linux job `100095528356` and Windows/MSYS2 MinGW64 job `100095528194` both ran
against pristine OpenFHE
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. Both warning-clean default project
builds and both compile-only Relin2 public API-contract builds succeeded. CTest
then reported exactly `17/18` passing on each platform: tests 1 through 17
passed and only `relin2_key_hybrid_entry_format` failed. Linux reported 0.30
seconds; Windows reported 0.39 seconds. Both platforms produced the same exact
failing observation:

`Relin2 test failure: Relin2 HYBRID evaluation-key entry format threw the wrong exception type: DoubleCKKS: Relin2 is not implemented`

The failure is inside the new exact-diagnostic helper, so this red receipt does
not claim that the negative case's later post-call invariance checks executed;
the paired green must execute them. The valid positive control ran earlier in
that same test and passed, proving the fixture is not an unconditional
rejection.

## Integrity and decision

This directory retains terminal run/jobs/check-runs/artifacts JSON, both full
job records/logs, complete logs ZIP, exact workflow, and source identity. The
ZIP passes `unzip -t`; the uploaded artifact count is zero. Before commit, both
the retained directory and a fresh expansion of the ZIP must pass Gitleaks
8.30.1 and independent targeted filename/content scans, then every retained
file except the manifest itself is bound by `MANIFEST.sha256`.

The malformed-HYBRID-entry-format contract is accepted red on Linux and
Windows. The green change may only scan the already length- and basis-validated
HYBRID A entries and then B entries for Evaluation format and emit the single
exact project diagnostic. It must leave BV shape, ciphertext raising, key
switching/relinearization, arithmetic, metadata, and the old scaffold
untouched.
