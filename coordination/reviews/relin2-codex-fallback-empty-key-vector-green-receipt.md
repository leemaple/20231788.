# Relin2 Codex fallback empty-key-vector-green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **present-but-empty evaluation-key-vector guard green on Linux; first
key validation and Relin2 arithmetic remain unimplemented**.

## Implementation and hosted boundary

- implementation branch/commit: `agent/codex-relin2-01` /
  `342686ae1badbcfd700eff0ec473cac4168e491a`;
- parent/tree: `e976626ebabf0aa40858ee97c830611e9c47c5f6` /
  `4aeb255ee5b78b8b3f9f8c113c47cc09f1ba67d8`;
- run/job: `33560502555` / Linux `100031552242`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33560502555`.

Only `src/double_ckks.cpp` changed, with five additions and one deletion. It
stores the existing map `find` result, preserves the accepted missing-row
diagnostic, then rejects `iterator->second.empty()` with the stable project
diagnostic. There is no `operator[]`, throwing OpenFHE getter, key-element
access, mutation, clone, basis change, arithmetic, or production `try`/`catch`;
a nonempty vector still reaches the exact old scaffold. Three read-only reviews
returned `PASS`. Linux built warning-clean, compiled the public API contract,
and passed exactly `10/10` tests. This green run also executes the new immediate
Tensor/cache/metadata invariance assertions. Windows was cancelled during
toolchain installation and makes no project-test claim.

## Evidence binding

- branch/commit/tree: `evidence/relin2-hosted-342686a` /
  `9839e0e759963e460d756b379a0c28dc1edf62b7` /
  `4982cdc820434f5472622765d08571c658f2486a`;
- 19-entry manifest SHA-256:
  `413230952a5271082274907f7381a83e9a961fbade0aac429f6f6509c95d01a5`;
- receipt / complete-logs ZIP / Linux log SHA-256:
  `8aff113c01e39080c3fee6397de75f45f91949567646291acb902bfe7a177925` /
  `54c151d85afb0d06c7947a7c7a84651dc7e61455e78a05b7490cdf5e280e26e3` /
  `4bc8d00f5cc41da3a72b9a0492226efceeee94df8389af04742e07b52721a35f`.

The ZIP passed integrity testing, the artifacts API count was zero, and both
retained and expanded evidence passed Gitleaks 8.30.1 plus targeted scans. The
next boundary must begin with a present vector whose first evaluation-key
element is null.
