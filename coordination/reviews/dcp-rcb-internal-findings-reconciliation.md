# DCP/RCB internal-review findings reconciliation

Prepared: 2026-09-01 Asia/Shanghai

## Exact target

- Branch: `agent/codex-dcp-rcb-01`.
- Exact current commit:
  `87c84b879c13b55cf15d6559d3317853228fdc05`.
- Last production-code change:
  `4971d2292b5af0ddbbe0c7dbe5a2e87f45102ff1`.
- Official OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Exact-current-commit CI:
  `https://github.com/leemaple/20231788./actions/runs/33411494861`.

Two parallel internal reviews completed against an earlier DCP/RCB snapshot.
This record rechecks their findings against the exact current commit so stale
findings do not become current claims.

## Findings already corrected

1. **Four-tower minimum rejected a valid first-Mult2 basis.** Corrected by the
   retained red/green sequence ending at production commit `bcf50df`; the
   current constructor accepts the minimum `[q0, q_l, q_div]` three-tower
   basis, and the independent oracle exercises it.
2. **Unused future lifecycle states violated YAGNI.** Corrected by `d7412b8`;
   the current public enum contains only `ReadyForFirstMult`.
3. **Negative tests could accept any `std::exception`.** Corrected by
   `2b54664`; the current harness accepts only `std::invalid_argument` with a
   module-specific diagnostic and translates any other standard exception into
   a test failure. Production code has no catch-all recovery.
4. **Evidence files lacked commands.** The retained red/green records now state
   the exact configure, build, and CTest commands together with exact project
   and OpenFHE commits, runner, run URL, and observed result.
5. **Branch role did not use the required prefix.** The exact current branch is
   `agent/codex-dcp-rcb-01` and its remote SHA has been verified.

## Findings that are not product-code requirements

The missing `0001-red-tests.patch`, `0002-dcp-rcb-implementation.patch`, and
ChatGPT design/review ZIP were deliverables of a bounded external-agent response
contract, not files required in the built library. That candidate response is
retained and audited separately. The accepted implementation was developed and
tested directly on the clean-room branch and does not claim that those external
patches were applied. Their absence from the product tree is therefore not a
current algorithm or integration defect.

## Deliberately deferred whole-pipeline acceptance

- Real and complex plaintext-slot vectors, repeated multiplication, and
  paper-derived end-to-end precision bounds require Tensor2, Relin2, RS2, and
  Mult2. The current slice intentionally tests exact DCP/RCB coefficient
  semantics on deterministic ciphertext components with an independent
  `boost::multiprecision::cpp_int` CRT oracle. The end-to-end scenarios remain
  mandatory before the whole implementation can be accepted; no current
  precision or repeated-multiplication claim is made.
- `ValidatePair` currently accepts only the one constructible lifecycle,
  `ReadyForFirstMult`. Generalizing its recorded/logical-scale rules for RS2
  output before an RS2 acceptance test would violate YAGNI. The rule must be
  revisited when that lifecycle is introduced.

## Current KISS judgments

- `PaperScaleDescriptor` intentionally exposes the distinction required by the
  paper and OpenFHE integration: the input/recorded scale remains unchanged in
  OpenFHE while the logical pair scale is divided by `q_div`. The repeated
  divisor and recorded scale are immutable and cross-validated before RCB.
  Removing either view now would obscure a required invariant; no change is
  proposed without a failing consumer test.
- The small private validation function and test-only CRT navigation remain
  direct. Introducing new state objects or oracle-accessor classes solely to
  remove parameter grouping or message-chain smells would add indirection to a
  one-source-file slice without changing correctness.

## Retained evidence limitation requiring explicit disposition

`red-hardening-01.txt` honestly records that a newly introduced missing
`PaperScaleDescriptor` interface caused compilation to fail before other new
runtime hardening assertions could execute. It is valid red evidence for the
descriptor, but it is not isolated initial-red output for every runtime
assertion authored in that same test commit. Later isolated red/green sequences
cover the minimum three-tower basis, absent OpenFHE precomputation rows, slot
metadata, and other concrete defects, while the exact current test suite passes
on Linux and Windows.

History cannot be rewritten into a fabricated red. The final external review
must state whether this transparent process-evidence limitation is a current
P0/P1 code blocker, a lower-priority process finding, or needs a specific
mutation-test artifact. No claim should conflate CI success with historical
TDD evidence that was not observed.

