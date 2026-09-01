# Relin2 Codex fallback Tensor-validation-green coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **first runtime Relin2 behavior green on Linux; Relin2 arithmetic
remains unimplemented**.

## Implementation boundary

- branch: `agent/codex-relin2-01`;
- commit: `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`;
- parent/red commit: `f2deacbb9b1f1a291d91b6d9ac9eec5f363f0082`;
- tree: `028033ad13b90710eaa98e6f1436bdb2d8f49b86`;
- local HEAD, upstream, and remote branch SHA were all verified equal after a
  non-force push.

The commit changes only `src/double_ckks.cpp`: it names the `Relin2` parameter,
calls the existing complete `ValidateTensorResult(tensor)` first, and retains
the exact not-implemented `std::logic_error` for valid input. It adds no public
API, test, CMake, workflow, key-cache access, basis check, tower arithmetic,
relinearization, DCP/addition, or result construction. Spec, TDD, and
Delivery/CI read-only reviews each returned `PASS` before commit.

## Hosted observation

- workflow run: `33532645418`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33532645418`;
- exact head SHA: `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`;
- Linux job `99939294149`: terminal `success` on Ubuntu 24.04.4 with CMake
  3.31.6 and GCC 13.3.0 against pristine OpenFHE
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- warning-clean default build and compile-only Relin2 API contract succeeded;
- exact CTest result: `7/7`, including the red-to-green
  `relin2_tensor_validation_order` and all six inherited tests;
- after the complete Linux job log was downloaded, the non-final whole run was
  cancelled once; Windows job `99939294452` is terminal `cancelled` during the
  pristine OpenFHE build near 38% and supplies no project-build or test claim.

## Remote evidence binding

- evidence branch: `evidence/relin2-hosted-84df651`;
- evidence commit/local upstream/remote SHA:
  `a155990e6f95266701c7c271b1eea76d2424ecde`;
- evidence tree: `03f0fe46a3ebc7791977d39afcec5520465499c0`;
- `MANIFEST.sha256`: 23 entries, SHA-256
  `d0095027be7d0b38cc20d63ca0e307e23b8aaf1e78d531556416e2ebd49e206d`;
- evidence-side `RECEIPT.md` SHA-256:
  `b371656beb8f2888e607b33138bf7131294613f14835cd96d40e31cbc028c136`;
- terminal complete-logs ZIP SHA-256:
  `c3fc4f927779b71c6fbd524a83716181bfb6fb2ca8a3c75e2ba1599f11ac2f36`;
- Gitleaks 8.30.1 and the targeted credential scan reported no findings;
- the terminal complete-logs ZIP passed `unzip -t`; the artifacts API reported
  exactly zero uploaded artifacts.

The evidence branch contains raw records and its manifest but does not bind
its own commit SHA. This coordination-side receipt supplies that non-circular
remote binding.

## Next authorized boundary

The next behavior must start from a separately registered red against a valid
Tensor input that now passes complete Tensor validation and reaches the exact
not-implemented seam. Only the minimum behavior selected by that new test may
then be implemented.
