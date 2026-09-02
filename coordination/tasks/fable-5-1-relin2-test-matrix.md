# Fable 5.1 decision task — Relin2 26-to-37 test matrix

Prepared and completed: 2026-09-02 Asia/Shanghai

Status: completed naturally by verified canonical model
`claude-fable-5-1`; verdict **B**. The byte-exact task, 113,283-byte sanitized
request ZIP, raw stream, extracted answer, tool-path list, scans, invocation,
and verification are retained under
[`../handoffs/fable51-relin2-test-matrix-01/`](../handoffs/fable51-relin2-test-matrix-01/).

This escalation resolved a concrete disagreement between independent reviews.
The controlling sequence is 26/26 baseline → 26/36 core red → 36/36 core green
→ 36/37 lifecycle red → 37/37 lifecycle green. Arithmetic and result state are
independent fail-closed blocks inside the frozen
`relin2_valid_arithmetic_state_immutability` registration; the two
ReadyForFirstMult recombined-field rejection cases remain separately registered.
The final BV success-case spellings are `relin2_bv_zero_digit_valid_shapes` and
`relin2_bv_nonzero_digit_valid_shapes`; deterministic nonzero/carry witnesses
belong only to the controlled-witness registration.
The complete matrix and exact evidence identities are recorded in
[`../reviews/fable51-relin2-later-key-decision-receipt.md`](../reviews/fable51-relin2-later-key-decision-receipt.md).
