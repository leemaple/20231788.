# Tensor2 external-task audit

Recorded: 2026-09-01 Asia/Shanghai

## Scope and evidence boundary

Two existing review agents independently inspected only:

- `coordination/tasks/chatgpt-pro-tensor2-01.md`;
- `coordination/reviews/tensor2-preflight.md`;
- clean-room base `agent/codex-tensor2-01` at exact commit
  `87c84b879c13b55cf15d6559d3317853228fdc05`;
- the user-supplied paper and official OpenFHE 1.5.0 source at
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

They made no edit or build and did not inspect any previous/private/wrong
implementation. One audit focused on paper/OpenFHE/spec correctness; the other
focused on TDD, KISS/YAGNI, evidence, and executable acceptance.

## Blocking findings and dispositions

1. **One missing-API compile error would mask all runtime red assertions.**
   The task now separates a missing-API compile red from a temporary
   fail-before-access scaffold and a complete runtime contract red. Every named
   runtime case must execute and report independently before the one complete
   implementation patch. The scaffold is explicitly non-mergeable and never
   returns a partial/incorrect public result.
2. **Invalid lifecycle was not constructible through the public seam.** The
   base has only `ReadyForFirstMult`, returns lifecycle by value, and privately
   constructs pairs. That negative case is now forbidden and deferred. Tests
   may use only the existing public-getter adversarial mutation pattern and may
   not add production hooks, friends, setters, or test constructors.
3. **The candidate OpenFHE metadata normalization was prematurely mandatory.**
   The task now makes source/paper proof a precondition. If degree 3 and
   `SF1 * SF2 / baseSF` cannot be proved consistently, the reviewer must return
   `blocked` without scale or production patches.
4. **The paper-scale expectation was incomplete.** The task now requires two
   independently derived values, never read from the result:
   `H_out = H_1 * H_2` for the high member and
   `R_out = R_1 * R_2 / q_div` for the recombined Tensor2 value.
5. **The convolution fixtures could false-pass ordinary convolution or unsigned
   reduction.** The task now requires an explicit
   `X^(N-1) * X = -1` witness, a signed product crossing an active tower
   modulus, and a named nonzero low-low witness at an exact
   tower/component/coefficient.
6. **Input archive identity and downstream builds were underspecified.** The
   task now requires a manifest, final byte count/SHA-256, two secret scans,
   archive/tree checks, and fail-closed identity verification. It also records
   exact Linux and Windows project commands, runner/toolchain evidence fields,
   same-SHA final results, and the rule that unexecuted work remains `pending`.
7. **A staged design could label a knowingly wrong low member or missing input
   validation as green.** Removed. The only temporary production state throws a
   project-owned `logic_error` before all element access/arithmetic. The final
   implementation must atomically supply valid high/low members, full operand
   and mutual validation, and the complete result manifest.
8. **A downstream OpenFHE exception could false-pass validation tests.** Both
   negative cases now require project `std::invalid_argument`, the stable
   `DoubleCKKS: ` prefix, and field-specific diagnostics; scaffold, generic, and
   OpenFHE exceptions are explicit failures.
9. **The Windows command used an undefined build variable.** The command block
   now fails if `OPENFHE_PREFIX` is absent and defines
   `PROJECT_BUILD="$PWD/build"` locally before configure/build/CTest.
10. **The descriptor wording could reopen the accepted DCP API.** Both task and
    preflight now freeze the DCP descriptor/API and permit only a separate
    minimal Tensor-result descriptor. A proven need to alter DCP returns
    `blocked` pending approval.

## Lower-priority findings and dispositions

- `TensorCiphertextPair` itself represents the output state; no new lifecycle
  enum or mirrored flag is allowed.
- Existing RCB tests already exercise individual pair invariants. Tensor2 adds
  only proof that the right operand uses that validator plus a genuinely new
  cross-input slot-compatibility case.
- Patch files are the single authoritative deliverable; competing full-file
  replacements were removed.
- The first patch must add the isolated Tensor2 branch to the existing Actions
  trigger. Linux retains each red; the final exact SHA runs on both Linux/GCC
  and Windows/MSYS2 MinGW64.

## Result

After the first revisions, the paper/OpenFHE reviewer returned `PASS`. The
engineering reviewer identified and then rechecked four additional corrections:
no false-green partial result, module-attributed pre-arithmetic failures, a
self-contained Windows command block, and a frozen DCP descriptor/API. After
those corrections its final result was also `PASS`.

The task is materially stricter and independently reviewed, but no Tensor2
code, red result, build, test, or external-agent submission is claimed by this
audit.
