# Fable 5.1 decision task — exact Relin2 26-to-37 test matrix

Date: 2026-09-02 Asia/Shanghai

You must be the provider's exact `claude-fable-5-1` model. Begin the answer
with that exact identifier. This is a read-only clean-room specification gate.
Use only files under this fresh extraction. Do not edit, build, run tests,
access the network, invoke subagents, or inspect any path outside this folder.

## Background and objective

The clean-room implementation at commit
`1e59e8b36d5119ceb2b463922f1053e03a029bd4` currently registers 26 CTests:
six inherited DCP/Tensor2 cases and twenty Relin2 pre-arithmetic validation
cases. The frozen downstream contract has 37 total registrations, 31 of which
are `relin2_test` selectors. Your earlier accepted answer in `ANSWER.md` fixed
the two later-key fixtures and states at line 60 that the arithmetic green must
not merge without the preflight arithmetic-oracle and result-state reds.

Two independent reviewers now disagree on how the remaining eleven
registrations fit the frozen 37. We need one exact mechanically actionable
matrix before authoring the connected red stack.

## Conclusion A — separate result-state CTest

This reviewer says the exact eleven additions are:

1. `relin2_key_extra_later_valid`;
2. `relin2_key_malformed_later_ignored`;
3. `relin2_key_hybrid_valid`;
4. `relin2_key_bv_zero_digit_valid`;
5. `relin2_key_bv_nonzero_digit_valid`;
6. `relin2_valid_arithmetic_metadata_immutability`;
7. `relin2_result_state_scale`;
8. `relin2_controlled_witnesses_and_boundaries`;
9. `relin2_public_rcb_return`;
10. `relin2_representative_public_input`;
11. `relin2_tensor2_ready_for_rs2_rejection`.

It says `relin2-preflight.md:284-294` requires arithmetic and result-state as
two independently registered CTests. Old DCP-field propagation and
ReadyForFirstMult RCB/Tensor2 validation should extend inherited CTests rather
than add registrations.

## Conclusion B — frozen historical 10-plus-1 sequence

This reviewer says the exact eleven additions are:

1. `relin2_valid_arithmetic_state_immutability`;
2. `relin2_controlled_witnesses_and_boundaries`;
3. `relin2_representative_public_input`;
4. `relin2_key_extra_later_valid`;
5. `relin2_key_malformed_later_ignored`;
6. `relin2_hybrid_valid_shapes`;
7. `relin2_bv0_valid_shapes`;
8. `relin2_bvnz_valid_shapes`;
9. `relin2_first_recombined_rcb_validation`;
10. `relin2_first_recombined_tensor2_validation`;
11. `relin2_tensor2_requires_first_lifecycle`.

It says the first ten form a 26-to-36 core red/green, then item eleven forms a
36/37 lifecycle red followed by 37/37 green, matching
`chatgpt-pro-relin2-01.md:475-478,586-608` and the archived accepted 37-test
candidate. It treats arithmetic and result-state as two independent,
separately diagnosed oracle blocks inside item one rather than two CTests.

## Required decision

Read the complete current project files, `contract/relin2-preflight.md`,
`contract/chatgpt-pro-relin2-01.md`,
`contract/chatgpt-pro-relin2-remediation-06.md`, and the prior accepted
`ANSWER.md`. Resolve the apparent conflict by authority and semantics, not by
preference.

Return exactly:

1. one bounded verdict: `A`, `B`, or `NEITHER`;
2. the exact eleven CTest names and one-sentence responsibility of each;
3. the exact red/green sequence and expected total/pass/fail counts at every
   boundary;
4. whether arithmetic and result-state must be separate registrations or may
   be separate fail-closed blocks in one registration, with exact contract
   anchors;
5. which old DCP/RCB/Tensor2 regression assertions belong in inherited tests;
6. any exact naming adjustment needed to preserve the already accepted
   `key_` later-key names without silently changing behavior.

Reject any plan that makes 28/28 the complete arithmetic gate, exceeds or falls
short of the frozen 37 without proving the contract itself inconsistent,
weakens an oracle, accepts the Relin2 scaffold as success, or edits production
before red evidence exists. Distinguish observed facts, inference, and unknowns.
