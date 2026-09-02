claude-fable-5-1

**1. Verdict: B.** Conclusion B is the only allocation that fits the frozen count without breaking the binding task contract. Conclusion A fails on one decisive point: it puts the two `ReadyForFirstMult` recombined-field rejection cases into the inherited CTests. The task requires every patch-03 case, explicitly including the legacy-field cases, to be independently observed red on the scaffold while the existing DCP/RCB/Tensor2 cases stay green, per `contract/chatgpt-pro-relin2-01.md:372-378`. A red assertion inside `dcp_rcb` or a `tensor2_*` selector would turn an inherited registration red, and a new inherited selector would break the six-plus-thirty-one split. A's arithmetic/result-state split is not forbidden in itself, but it is count-feasible only through that violation. B's structure follows from packet authority alone; its appeal to an archived 37-test candidate is unverifiable here and is not needed.

**2. The eleven registrations**, B's names, one responsibility each:

1. `relin2_valid_arithmetic_state_immutability`. With the ordinary generated HYBRID key and a small-ring deterministic-polynomial DCP/Tensor2 fixture whose third component is nonzero, requires Relin2 to return normally, checks deep Tensor and cache invariance immediately, then evaluates without early exit three labelled fail-closed blocks: exact per-residue equality of every output coefficient against the test-owned Boost `cpp_int` `(u, v+w)` reference plus the exact RCB recombination identity and named nonzero `K0`/`K1` and `v`/`w` positions; the complete `ReadyForRS2` result contract through `CheckReadyForRS2Result`; and public RCB acceptance, non-mutation, and exact recombination of the returned pair.
2. `relin2_controlled_witnesses_and_boundaries`. Installs test-controlled valid-shaped A/B contents under the RAII guard, records named component/coefficient witnesses for centered quotient/remainder sign boundaries and quotient carry on the actual full-basis relinearized raised high immediately before the test-owned DCP, and requires Relin2's normal return to equal the reference at those named positions and everywhere else, with invariance.
3. `relin2_representative_public_input`. Runs the full public path of packed encryption, then DCP, Tensor2 and Relin2 with the ordinary generated key on representative plaintexts, and requires the normal `ReadyForRS2` return, per-residue equality against the test-owned reference, the RCB identity, and deep invariance.
4. `relin2_key_extra_later_valid`. Exactly as accepted in `ANSWER.md:51`: HYBRID context with `maxRelinSkDeg` 3, one `EvalMultKeysGen` producing the real two-entry vector, and a normal fully validated `ReadyForRS2` return with both entries, the cache and the Tensor unchanged.
5. `relin2_key_malformed_later_ignored`. Exactly as accepted in `ANSWER.md:52`: same generation, index one set to null through the public map, and a normal return proving index one is neither inspected nor consumed.
6. `relin2_hybrid_valid_shapes`. Under HYBRID with the ordinary generated key, executes one full-basis public `Relinearize` on a test-raised clone of Tensor high and one level-one-prefix public `Relinearize` on Tensor low with the same key, asserts their exact stage metadata, then requires Relin2's normal fully validated `ReadyForRS2` return, deep invariance, and the exact per-residue identity that RCB of the result equals the full-basis reference with only the appended residue deleted plus the level-one reference.
7. `relin2_bv0_valid_shapes`. Item 6 under BV with digit size zero.
8. `relin2_bvnz_valid_shapes`. Item 6 under BV with digit size ten.
9. `relin2_first_recombined_rcb_validation`. DCP a fresh input, assert the propagated recombined field equals the recorded factor, corrupt only that field through the public const reference, and require RCB to throw the exact project-owned field-specific `std::invalid_argument` with the pair otherwise unchanged.
10. `relin2_first_recombined_tensor2_validation`. Same corruption on one valid DCP input, require Tensor2 to throw the same field-specific diagnostic before multiplication with both inputs unchanged; the explicit-field source of Tensor2's recombined scale is proved by source review, not by this test.
11. `relin2_tensor2_requires_first_lifecycle`. After G1, obtain a valid `ReadyForRS2` pair from public Relin2 and require Tensor2 to throw the exact project-owned `ReadyForFirstMult` lifecycle diagnostic for that pair in each operand position, including the pair with itself, with the pair unchanged.

Items 6 to 8 verify the technique-dependent key switch through the exact identity; the technique-independent DCP split is verified by items 1 to 3 under HYBRID, so the BV rows need no second DCP oracle.

**3. Sequence and counts.** Every boundary is a Linux observation before the next commit exists. R1 may be split into R1a and R1b; if so, both records are kept and R1a is never followed by production.

| Boundary | Content | Registered | Pass | Fail | Failing cases and mode |
|---|---|---:|---:|---:|---|
| C0 `1e59e8b` | current | 26 | 26 | 0 | none |
| R1a, test-only, optional | items 4, 5 | 28 | 26 | 2 | items 4, 5 with the scaffold text |
| R1, test-only | items 1 to 10, plus green-only inherited extensions of section 5 | 36 | 26 | 10 | items 1 to 8 with `DoubleCKKS: Relin2 is not implemented`; items 9, 10 with `did not fail fast` |
| G1, production only, no test edit | smallest complete Relin2, `ValidatePair` lifecycle branch, recombined-field predicate, Tensor2 explicit-field source, RCB `ReadyForRS2` acceptance, scaffold removed | 36 | 36 | 0 | none; Linux and Windows |
| R2, test-only | item 11 | 37 | 36 | 1 | item 11 on the wrong downstream diagnostic or exception |
| G2, production only | smallest pre-arithmetic Tensor2 lifecycle guard | 37 | 37 | 0 | none; one SHA, Linux and Windows |
| T1, test-only, after G2 | retire the tolerance in the helper at `project/tests/relin2_test.cpp:71-85` | 37 | 37 | 0 | none |

Twenty-eight of twenty-eight never occurs. The complete arithmetic gate is 36 of 36 after G1, and G1 is non-mergeable until G2 per `contract/chatgpt-pro-relin2-01.md:383-386`. Disposable mutations, each restored with an unchanged SHA and a green rerun: M1 and M2 from `ANSWER.md:62-70`; M3 on the G2 tree deletes the guard and exactly item 11 fails; M4 on the G1 tree deletes the recombined predicate from `ValidatePair` and exactly items 9 and 10 fail. Tensor2 reading the wrong equal-valued field has no black-box mutation and is closed by source review only, per `contract/chatgpt-pro-relin2-01.md:203-207`.

**4. Arithmetic and result-state: separate fail-closed blocks in one registration.** Anchors:

- The binding task lists coefficient oracle, witnesses, state/lifecycle/scale assertions, immutability and RCB acceptance as bullets of one named group, `contract/chatgpt-pro-relin2-01.md:419-455`.
- The preflight's two names at `contract/relin2-preflight.md:285-294` are a provisional minimum that defers to the eventual task, `contract/relin2-preflight.md:5-10` and `:274`.
- The frozen total is 37, `contract/chatgpt-pro-relin2-remediation-06.md:519`. The two success rows, the two legacy-field registrations, the three per-technique registrations and the separate lifecycle registration are each fixed by `contract/chatgpt-pro-relin2-01.md:502-505`, `:372-378` with `:523-527`, `:513-515`, and `:387-396`, leaving exactly one slot for the group.
- The masking clause at `contract/chatgpt-pro-relin2-01.md:376-377` is met inside the registration by running every block after the single Relin2 call, collecting each block's `TestFailure`, and throwing one failure that names every failed block. Test-side `try`/`catch` is permitted at `:416-417`. No block may use the tolerant helper.

**5. Inherited tests.** Green-only extensions, byte-stable diagnostics, no new inherited registration:

- `dcp_rcb`: every DCP output's `approximateRecombinedLogicalScalingFactor` equals its recorded factor and its `inputRecordedScalingFactor`; the field joins the pair deep snapshot compared after RCB; the existing three-field descriptor corruption texts stay exact. Anchors `contract/chatgpt-pro-relin2-01.md:521-522` and `:531-532`, `contract/relin2-preflight.md:333-337`.
- `tensor2_*`: input-pair snapshots include the recombined field and are unchanged after Tensor2; `tensor2_result_scale_contract` keeps asserting the Tensor recombined value.
- Not inherited: the corruption rejections of items 9 and 10, RCB acceptance of `ReadyForRS2` in item 1, and Tensor2 rejection of `ReadyForRS2` in item 11.

**6. Naming.** Keep `relin2_key_extra_later_valid` and `relin2_key_malformed_later_ignored`, their selectors and `TestExtraLaterValid`/`TestMalformedLaterIgnored` exactly as `ANSWER.md:51-53` and `contract/chatgpt-pro-relin2-remediation-06.md:258-259`. The `key_` prefix stays reserved for the nineteen frozen matrix rows, so A's `relin2_key_hybrid_valid`, `relin2_key_bv_zero_digit_valid` and `relin2_key_bv_nonzero_digit_valid` are rejected: those tests mutate nothing in the map, and `key_` names would present the frozen nineteen rows as twenty-two, contrary to `contract/chatgpt-pro-relin2-remediation-06.md:261-264`. Do not rename the seventeen existing selectors or refactor them into a `RunKeyMutation` wrapper; rows map by semantics, `ANSWER.md:17`. B's `bv0`/`bvnz` spelling differs from the existing `bv_zero_digit`/`bv_nonzero_digit` style; that is cosmetic, but it must be fixed before R1 so the red evidence binds final names.

Facts, inference, unknowns

Observed:
- `project/CMakeLists.txt:74-99` registers 26; `project/tests/relin2_test.cpp:3070-3131` dispatches the same twenty; eleven positive controls use the tolerant helper inside negative tests.
- `project/src/double_ckks.cpp:421-460` has no lifecycle branch and no recombined predicate; `:405-411` propagates the field; `:578-583` derives the Tensor recombined scale from `inputRecordedScalingFactor`; `:545-548` has no lifecycle guard; `:698` throws the scaffold. Items 9 and 10 are therefore honest reds today, independent of arithmetic.
- The packet holds only the four project files, three contracts and four OpenFHE files listed at `TASK.md:112-132`. The inherited test files, the workflow, and any archived 37-test candidate are absent.

Inferred: 31 relin2 selectors equals 37 minus the six inherited registrations frozen at `contract/chatgpt-pro-relin2-01.md:23-24` and `:531-532`. The fixture split among items 1 to 3 follows `contract/chatgpt-pro-relin2-01.md:415-416` and `:421-449`. R2's exact wrong diagnostic is an execution observation.

Unknown: whether the inherited test files already assert propagation; whether deterministic boundary and carry witnesses can be constructed, which gates the slice at `contract/chatgpt-pro-relin2-01.md:446-449`; the exact field-specific diagnostic text, which must be one stable project-owned string bound at R1; and the frozen downstream audits' ten-call-site and wrapper structure, which the clean-room already does not match and which this matrix does not change.
