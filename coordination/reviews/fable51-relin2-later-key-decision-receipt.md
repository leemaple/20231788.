# Fable 5.1 receipt — Relin2 later evaluation-key semantics

Recorded: 2026-09-02 Asia/Shanghai

Status: **accepted controlling decision from a verified, restricted,
fresh-extraction `claude-fable-5-1` run**.

The complete request ZIP, byte-exact packet task/manifest, raw stream JSONL,
extracted answer, stderr, tool-call path list, CLI provenance, scan reports,
rejected-attempt records, and verification instructions are retained in
[`../handoffs/fable51-relin2-later-key-decision-01/`](../handoffs/fable51-relin2-later-key-decision-01/).

## Clean-room and model identity

- Implementation source commit/tree:
  `1e59e8b36d5119ceb2b463922f1053e03a029bd4` /
  `4e3a8b4857aeb8f5f7ef07dd2f01b5f74079ba77`.
- Official OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Request ZIP bytes/SHA-256: 105,324 /
  `4454f15e34bc48f73f23af8321ae7e26e315a42637447ccfb066b31d98fb2dcf`.
- Retained request `TASK.md` SHA-256:
  `a95239ab21f5ee1d81e99c9624e18f12e3b7d25814fb1c6f11683d05d81550aa`.
- Packet manifest SHA-256:
  `748def3010a589683e8de38dd2d52c79a163a9b33b86541200bc44ac50665371`.
- Claude Code: official registry `2.1.258`, verified native binary SHA-256
  `b63136194160791c27cfa7b0403060d85eb0752991625fde8c09f9acacb17c78`,
  valid Anthropic PBC Developer ID signature.
- Exact requested and emitted canonical model: `claude-fable-5-1`.
- Provider/session: `firstParty` /
  `5c5a027a-9b5e-46e4-b315-1b0ef9333eda`.
- Result: exit 0, `is_error=false`, `end_turn`, `completed`, 604,058 ms,
  17 turns, no fallback argument, no permission denial, no subagent, no web
  search or fetch.
- Working directory: the fresh extraction only.
- Tools: exactly 16 `Read` calls; every retained path is under that fresh
  directory. No MCP server or plugin was loaded.
- Raw JSONL bytes/SHA-256: 1,541,999 /
  `5cf7fdf279cc6eda4306b7bb53511d368e1955390b58df0af022e954c19c4e03`.
- Answer SHA-256:
  `56fc4e76deff61b0d58864ba29ba1f74cb877c354b0b1c9a7b6d23210baba98f`.
- Retained evidence: 39 files total; the 38-row `FILES.sha256` manifest has
  SHA-256
  `cfe466ea966e413b5b88fc1b959d2cb9f3ae8a78aa7358aa76d25ede0ce2ef56`
  and verifies every non-manifest file.
- Stderr: empty.
- The retained, mechanically replayable stage, fresh-extraction, request-ZIP,
  and raw-output Gitleaks scans are clean. ZIP integrity, manifest, path safety,
  uniqueness, and non-encryption checks passed. Two supplementary exact-token
  and high-specificity checks were observed clean before the ephemeral settings
  file was deleted, but their target hash/predicate/raw exit were not retained;
  they are explicitly non-replayable and are not used as acceptance evidence.

The session ID is a run-correlation identifier, not a resumable conversation:
the accepted invocation deliberately used `--no-session-persistence`.

Three earlier attempts are rejected and retained under `rejected/`: the first
emitted old `claude-fable-5`; the second used the right model but an unconfined
working directory containing a former implementation; the third enabled
restriction without explicit auth-only settings and failed 403 before model
work. None of their prose participates in this decision.

## Subsequent 37-test matrix adjudication

Two independent reviewers then disagreed about whether the result-state oracle
was a separate registration and which legacy lifecycle assertions occupied the
frozen 37 slots. That concrete disagreement was escalated to a second clean-room
Fable 5.1 run rather than guessed. Its complete request, raw stream, answer,
tool-path inventory, scans, and verification are retained under
[`../handoffs/fable51-relin2-test-matrix-01/`](../handoffs/fable51-relin2-test-matrix-01/).

- Request ZIP: 113,283 bytes / SHA-256
  `42ddcbe09df14c52fd7e38522da5e2dd259a2989c23b1b7a75ebac963da3808d`.
- Exact task SHA-256:
  `92958ac83c9f9899b4c578c7218621192d9fbdc65bb75881e21cb1c9851bfd14`.
- Raw JSONL: 1,150,987 bytes / SHA-256
  `cecf245ba36ad685b58d0f8d25eaffadc27bd73b3807b8bbd4d12aa7b7abd166`.
- Answer SHA-256:
  `dc3e173bd82cd27fb9fdeda41fd9af4693111c294f0327b5b6b3cd1e79aedc19`.
- Exact emitted/canonical model: `claude-fable-5-1`; first-party provider;
  success, `end_turn`, `completed`, 53 turns, 831,434 ms, session
  `29b94085-4612-4706-857b-c557496b82da`; no fallback, web, MCP, plugin,
  subagent, or permission denial.
- All 52 tool calls were `Read` and remained inside the fresh extraction.
- The 16-row evidence manifest SHA-256 is
  `6b476ee5df2c6137c7883bc6963240ac884a481d125898826871a1b249c05e15`
  and verifies every non-manifest file. Request/output Gitleaks are retained and
  clean. The exact-token/high-specificity check was contemporaneously observed
  clean with a bound target hash and process exit, but deletion of the ephemeral
  target makes it non-replayable; it is supplementary and not acceptance
  evidence.

The controlling verdict is matrix **B** below. The rejected matrix A could fit
37 only by moving two required independent scaffold-red legacy-field cases into
the six inherited registrations, which would make inherited tests red or alter
the frozen six-plus-thirty-one split.

## Accepted source/API facts

1. Current project `Relin2` rejects only an absent tag, empty vector, or bad
   index-zero key, then reads and validates only `front()` before the terminal
   scaffold. It has no `size()==1` restriction and never reads index one.
2. OpenFHE plural `EvalMultKeysGen` with `maxRelinSkDeg=3` emits the real ordered
   vector `[s^2 -> s, s^3 -> s]`. A duplicate index-zero pointer is not a
   semantically valid later key and cannot prove slot-role correctness.
3. Public OpenFHE `Relinearize` checks that key count is at least
   `components-2`; its three-component loop consumes exactly index zero.
   Therefore a null index one is a valid sharp probe that later entries are
   neither inspected nor consumed.
4. The public plural and singular generation calls insert only when the tag is
   absent. Each fixture must create its RAII cache guard before generation and
   call plural generation exactly once into an empty row.
5. `MakeContext` may gain only a fifth test-private defaulted
   `maxRelinSkDeg=2` parameter and set it unconditionally. The new fixtures use
   3. Independent pristine-source review confirmed the CKKS/RLWE default is 2
   and CKKS does not disable the setter.

## Controlling TDD decision

The two frozen configurations are full `success=true` Relin2 requirements,
not preflight-only requirements. The accepted Fable 5.1 decision therefore
rejects the earlier proposal to commit scaffold-through green tests. A helper
that accepts the terminal scaffold would not prove normal completion or that a
later key remains unused during arithmetic.

The current 26-test tree is only the completed pre-arithmetic validation
baseline. The two later-key cases are the last key-vector boundary in the core
Relin2 red stack; they do not by themselves authorize production arithmetic.
The exact eleven registrations that complete the frozen 37 are:

1. `relin2_valid_arithmetic_state_immutability` — after one normal Relin2 call,
   run every independently labelled fail-closed block without early exit: the
   Boost `cpp_int` exact `(u, v+w)` coefficient/tower oracle, the complete
   `ReadyForRS2` state/scale/metadata oracle, and public RCB
   exactness/non-mutation. Arithmetic and result state are independent blocks
   in this one frozen registration, not optional checks. Deterministic nonzero
   key-switch and `v/w` witnesses belong exclusively to item 2; this ordinary
   generated-key row must not discover them probabilistically.
2. `relin2_controlled_witnesses_and_boundaries` — deterministic key-switch,
   shared nonzero `v/w`, centered half-boundary, and quotient-carry witnesses.
3. `relin2_representative_public_input` — representative encrypted inputs
   through public DCP→Tensor2→Relin2 with the independent residue/RCB oracle and
   no unsupported plaintext-precision claim.
4. `relin2_key_extra_later_valid` — the real two-key HYBRID row permits an
   additional valid later key.
5. `relin2_key_malformed_later_ignored` — valid index zero plus null index one
   proves later entries are not inspected or consumed.
6. `relin2_hybrid_valid_shapes` — full-basis and active-prefix public
   relinearization plus exact Relin2 identity under HYBRID.
7. `relin2_bv_zero_digit_valid_shapes` — the same success contract for BV
   `digitSize==0`.
8. `relin2_bv_nonzero_digit_valid_shapes` — the same success contract for BV
   `digitSize>0`.
9. `relin2_first_recombined_rcb_validation` — corrupt only the new recombined
   field on `ReadyForFirstMult`; public RCB must issue the exact project
   diagnostic and leave the pair unchanged.
10. `relin2_first_recombined_tensor2_validation` — the same corruption on a
    valid DCP input must be rejected before Tensor2 multiplication with both
    inputs unchanged.
11. `relin2_tensor2_requires_first_lifecycle` — after core Relin2 is green, a
    real `ReadyForRS2` value must be rejected by Tensor2 with the exact
    `ReadyForFirstMult` lifecycle diagnostic and remain unchanged.

This yields six inherited DCP/Tensor2 registrations plus 31 distinct
`relin2_test` selectors. Existing DCP/Tensor2 registrations receive only
green-preserving propagation/snapshot assertions; the independent corruption
reds remain items 9 and 10 above.

For the two later-key cases in that commit:

- keep the frozen wrapper names `TestExtraLaterValid` and
  `TestMalformedLaterIgnored`;
- selectors: `key_extra_later_valid` and
  `key_malformed_later_ignored`;
- CTests: `relin2_key_extra_later_valid` and
  `relin2_key_malformed_later_ignored`;
- add a fail-closed normal-completion helper that returns the actual
  `CiphertextPair` and converts every exception into a labelled test failure;
- add a complete `ReadyForRS2` result oracle covering lifecycle, context,
  divisor, ordered basis, level, component count, format, tag, slots,
  noise-scale degree, recorded factor, both paper logical scales, metadata, and
  each ciphertext member;
- generate a real two-entry HYBRID/digit-zero vector using public
  `EvalMultKeysGen`; prove both entries are distinct valid relinearization keys
  with complete A/B lengths, ordered `ParamsQP` basis, Evaluation format,
  context, and tag;
- for `ExtraValid`, retain both real entries;
- for `MalformedLaterIgnored`, change only index one to null;
- after each production `Relin2` call, immediately prove complete Tensor,
  cache-map/vector/pointer, key context/tag, and A/B state invariance, then
  validate the returned pair;
- prove the cache is restored after each fixture scope.

The exact TDD sequence is:

1. **C0:** exact source `1e59e8b` is 26/26.
2. **R1:** one test-only connected core-red commit adds items 1–10 and the
   green-only inherited assertions: 36 registered, the existing 26 pass, items
   1–8 fail on the Relin2 scaffold, and items 9–10 fail because the missing
   recombined predicate did not reject. An optional focused earlier run may
   register only items 4–5 and show 26/28, but 28/28 is never a complete gate.
3. **G1:** production-only minimal complete Relin2, `ValidatePair` lifecycle and
   recombined-field handling, Tensor2 explicit-field use, and RCB
   `ReadyForRS2` acceptance; no test weakening: Linux and Windows 36/36. This
   commit remains non-mergeable until G2.
4. **R2:** test-only item 11: 37 registered, 36 pass, exactly item 11 fails on
   the wrong downstream behavior/diagnostic.
5. **G2:** production-only minimal Tensor2 lifecycle guard: one identical SHA
   passes warning-clean/API builds and 37/37 on Linux and Windows.
6. **T1:** remove the remaining scaffold-tolerant positive-control behavior;
   no production edit and 37/37 remains green.

Every red must retain its own fixture-before-failure evidence and exact
diagnostic. Do not edit either later-key test or weaken any other oracle in a
following green commit.

This is a deliberately short red-to-green series, not two long-lived unrelated
reds. The generic scaffold explains the initial failure; post-green dedicated
mutations establish that each later-key property is actually discriminating.
The controlling Fable decision resolves, and intentionally overrides, the
preliminary reviewer preference for scaffold characterization seams.

After the arithmetic green, remove the scaffold-tolerant behavior from all
remaining positive-control helpers so no valid path can continue accepting
`std::logic_error`.

## Dedicated mutation evidence

Run mutations only in isolated evidence worktrees/branches; never alter or
discard the accepted main worktree to restore them.

- M1 adds an exact-size-one restriction after the empty-vector check. On the
  37-test green tree, the exact target set is both later-key tests, because both
  intentionally contain two entries. All other tests remain green. This exact
  two-target set supersedes the packet task's preliminary “which test alone”
  wording for M1: isolating it further would require skipping or mutating the
  second required two-entry fixture and would make the evidence dishonest.
- M2 adds a loop that rejects null at index one or later, after the existing
  index-zero null guard. The exact target set is only
  `relin2_key_malformed_later_ignored`; all other tests remain green and the
  inherited null-first diagnostic is unchanged.
- M3 deletes the G2 Tensor2 lifecycle guard; exactly
  `relin2_tensor2_requires_first_lifecycle` fails.
- M4 deletes the recombined-field predicate from `ValidatePair`; exactly
  `relin2_first_recombined_rcb_validation` and
  `relin2_first_recombined_tensor2_validation` fail. Tensor2's use of the
  correct equal-valued recombined field is a source-review gate rather than a
  dishonest black-box mutation.

For every mutation retain its isolated base SHA/diff, exact command and raw
output, target/non-target matrix, and proof the accepted source SHA and tree
remain unchanged.

## Remaining implementation boundary

Together, the two verified Fable 5.1 decisions fix the later-key fixtures, the
exact eleven-registration matrix, and the 26→36→37 TDD placement. They do not
authorize a production setter, direct `KeySwitchCore`, OpenFHE modification,
compatibility layer, catch-based production recovery, or reuse of any former
implementation. The next production change remains the already specified
complete Relin2 arithmetic slice using public OpenFHE APIs, followed later by
RS2 and the remaining Mult2 pipeline.
