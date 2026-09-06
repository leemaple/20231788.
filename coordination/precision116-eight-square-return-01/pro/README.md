# EXPERIMENTAL-PRECISION116-EIGHT-SQUARE-01 — test-only draft

**Delivery status: complete source draft; NOT COMPILED / NOT RUN. Candidate numerical result: NOT ESTABLISHED. Security: UNRESOLVED.**

The patch adds one dedicated test source and appends one opt-in CMake block. It changes no production source/header, original test/oracle, existing mode, fixture, workflow, or recorded original-profile failure. It references the already implemented factory; it does not manufacture an API RED.

Apply `patches/precision116-eight-square.patch` at the repository root corresponding to the packet's `project/` directory. `files/` contains the complete resulting files, with repository-relative paths beneath it. Patch application and full-file equality were checked against the supplied 85-file project snapshot. All 84 existing files other than CMake remain byte-identical; CMake retains its entire original byte prefix.

The independent first-pass source review found **no concrete profile-specific production defect requiring a production patch**. This is not a numerical or security approval. The new test itself has only author self-checks and needs a separate reviewer.

The executable uses one candidate setup/encrypted chain, the unchanged 16,384-slot input, exact closed-product rational scales, endpoint all-component E80 gates, and client-only signed-h128/CRT/binary512 Horner observations at the original ten anchors. It checks terminal family7/level2 and root-wrapper level9 separately. It retains the exact witness, domain, codec, headroom and actual-polynomial wrong-normalization predicates.

The executable is excluded from the default build. Its CTest registration is absent unless `OPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE=ON`. No old endpoint protocol or publisher is invoked.

## Contents

- `INDEPENDENT_REVIEW.md`: source-based disposition and evidence boundaries.
- `DESIGN_AND_ORACLE.md`: exact predicates, independence, round mapping and limitations.
- `COMMANDS.md`: application and one-shot Linux/MinGW64 execution instructions.
- `EXECUTION_LEDGER.md` and `checks/`: actual verification and configuration failure, not runtime results.
- `MANIFEST.json`: size/SHA-256 for every other return member; excludes itself.

**The original profile's recorded FAIL remains FAIL.** Ten intermediate anchors and all-slot endpoints do not prove all-slot intermediate accuracy, nonwrap, or Tensor/Relin lift safety. A later numerical PASS would establish only the bounded observed chain, not exact Table 3 parameter replication, universal reliability, project completion, or cryptographic security.
