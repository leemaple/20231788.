# Relin2 Codex fallback Tensor-validation-red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **first runtime Relin2 red accepted; production remains the minimal
immediate-throw scaffold**.

## Implementation boundary

- branch: `agent/codex-relin2-01`;
- commit: `f2deacbb9b1f1a291d91b6d9ac9eec5f363f0082`;
- parent: `6f1645b97ce5b2175530cde5bfd0929370997634`;
- tree: `d1231cb5fb080a0eac65091fe1f5db539779d4b2`;
- local HEAD, upstream, and remote branch SHA were all verified equal after a
  non-force push.

The commit changes only `CMakeLists.txt` and new `tests/relin2_test.cpp`.
Production `include/` and `src/` bytes are unchanged. The independently
registered test creates a valid Tensor2 value without `EvalMultKeyGen`, corrupts
the high logical-scale manifest, and requires full-string equality with:

`DoubleCKKS: Tensor2 result paper-scale descriptor is inconsistent`

Spec, TDD, and Delivery/CI read-only reviews each returned `PASS` before
commit.

## Hosted observation

- workflow run: `33531734269`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33531734269`;
- exact head SHA: `f2deacbb9b1f1a291d91b6d9ac9eec5f363f0082`;
- Linux job `99936280030`: terminal `failure` after the exact strict project
  build and compile-only Relin2 API contract succeeded;
- the six inherited DCP/RCB/Tensor2 CTests passed exactly `6/6`;
- only `relin2_tensor_validation_order` failed, producing aggregate `6/7` and
  CTest exit `8`;
- exact red attribution: current production threw the scaffold's
  `std::logic_error("DoubleCKKS: Relin2 is not implemented")` instead of the
  required project-owned `std::invalid_argument`;
- after the complete Linux job log was downloaded, the non-final whole run was
  cancelled once; Windows job `99936279878` is terminal `cancelled` during the
  pristine OpenFHE build near 48% and supplies no project-build or test claim.

## Remote evidence binding

- evidence branch: `evidence/relin2-hosted-f2deacb`;
- evidence commit/local upstream/remote SHA:
  `2ba2b012733a9dba4b056993df92a56d21985f79`;
- evidence tree: `92edac05797eb06d4e9f297f1fbae6f729d13c69`;
- `MANIFEST.sha256`: 23 entries, SHA-256
  `9312ee550fc5c4cb15f444b47740131e81fca643ca7fb0a1f00891e523dd632e`;
- evidence-side `RECEIPT.md` SHA-256:
  `d90f145105f80ce7785d8a99f9c14ccfa7c46e845ff2afab913d51de00d18d01`;
- terminal complete-logs ZIP SHA-256:
  `c26ca79d6c4aab74c98264e4ae0dcb14345556ac808cd3a4bbf4354d3010f847`;
- Gitleaks 8.30.1 and the targeted credential scan reported no findings;
- the terminal complete-logs ZIP passed `unzip -t`; the artifacts API reported
  exactly zero uploaded artifacts.

The evidence branch contains raw records and its manifest but does not bind
its own commit SHA. This coordination-side receipt supplies that non-circular
remote binding.

## Next authorized boundary

The next production commit may only name the `Relin2` parameter and call the
existing complete `ValidateTensorResult(tensor)` before retaining the current
exact not-implemented `std::logic_error` for valid input. It may not access the
evaluation-key cache, add the insufficient-basis branch, raise towers, call
`Relinearize`, perform private DCP/addition, or construct a result.
