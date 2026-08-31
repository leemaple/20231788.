# ChatGPT Pro bounded DCP/RCB output evidence

## Identity and provenance

- Conversation: `https://chatgpt.com/c/6a952b95-0ed0-83ec-a38b-e415758ef2a5`
- Task brief: `coordination/tasks/chatgpt-pro-dcp-rcb-01.md`
- Submitted input archive SHA-256: `82b40b8084f35d2d0785f2ca05aa8e77842fd00a1c575d23cd7c49ddb8408243`
- Submitted clean-room commit: `7db43014a9c80d64e5f03f6666a710a3d7ca7e55`
- Submission observed: 2026-08-31 17:43 CST
- Response duration shown by ChatGPT Pro: 21m50s
- Download observed: approximately 2026-08-31 18:26 CST
- Local ignored archive: `artifacts/handoffs/chatgpt-pro-dcp-rcb-01/output/20231788-dcp-rcb-chatgpt-pro-bounded-recovery.zip`
- Archive size: 36,865 bytes
- Archive SHA-256: `c0d868b3f615b5288c8ed2790a034bdcfc66cfa8156fe9873a0b74e7ed04a108`

The locally computed archive SHA-256 exactly matches the value stated by ChatGPT Pro. `unzip -t` completed successfully. A Gitleaks 8.30.1 scan of the extracted 123.85 KB tree reported no leaks.

## Delivered contents

- `0001-red-tests.patch`
- `0002-dcp-rcb-implementation.patch`
- `DESIGN-DCP-RCB.md`
- `REVIEW-DCP-RCB.md`
- audit copies of CMake, public header, implementation, and independent-oracle test source
- patch-apply, red-attempt, green-attempt, and test-source-hash evidence

## Verification boundary

The candidate is review input, not accepted production output. ChatGPT Pro explicitly reports:

- both patches applied in order;
- a syntax-only missing-interface red was observed;
- its supplied OpenFHE source archive had an empty `third-party/cereal` dependency tree;
- both full builds stopped in upstream OpenFHE before project-owned compilation;
- CTest was not run;
- no project compile, warning-clean, passing-test, Windows, precision, performance, or security claim is made.

Codex therefore records the delivery as **candidate unverified**. Accepted green evidence must come from the exact official OpenFHE 1.5.0 commit with populated dependencies, strict warnings on project-owned code, and the independent oracle running unchanged in GitHub Actions or Windows.

## Candidate review points to reconcile

The output independently suggests several details that require source review and test evidence before adoption:

- validate that the paper divisor is odd;
- validate the sizes of OpenFHE's index-zero DCP precomputation vectors;
- represent paper/logical scale separately from OpenFHE's recorded FIXEDMANUAL scale;
- construct `low` by centered-switching the last source tower directly into each retained modulus;
- validate the exact fresh degree-two scale expected by the bound context;
- add adversarial manifest and metadata mutation tests without exposing a mutable public pair API.

No candidate patch is applied wholesale, and no previous or quarantined implementation is reused.
