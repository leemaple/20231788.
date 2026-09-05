# Lossless I/O: terminal observation and Cycle-A fallback

Recorded 2026-09-05, Asia/Shanghai. This is a coordination decision, not a
test result or an implementation acceptance.

## Exact starting point

- Worktree: `/Users/lifeng/Documents/20231788-openfhe-lossless-io-implementation-20260905`.
- Branch: `codex/lossless-io-implementation-01`.
- Starting HEAD and remote-tracking HEAD:
  `84a40af8053502b7ae33d8a7de61d481bdceaa3c`, clean before assignment.
- Its source, tests, CMake and workflows have no difference from the approved
  input base `4ccc8fd2e7617625d27e58a53eb3489e99466ed4`.
- Task: `coordination/tasks/LOSSLESS_CLIENT_IO_PRO_IMPLEMENTATION_01.md`,
  SHA-256 `707d366dcd4880450ac09ba4c1eb6195daf64def65333c305c20a099f8eadb1f`.
- Frozen first contract:
  `coordination/returns/lossless-client-io-pro-b64a980/PROPOSED_FIRST_TDD_CONTRACT.md`,
  SHA-256 `d85823ccb318833b01e526ea9e2e9930645d5cb09ccd9a446b0ec3d862f6c6af`.
- Only official OpenFHE pin
  `df495ba2e91739a6dc8f1de254fc5a41155ce504` and this clean-room project's
  authorized paper, specifications and fresh implementation are inputs.
  Quarantined pre-existing implementations and modified OpenFHE remain excluded.

## Observed external state

At **13:17:32 CST**, Root read the existing conversation
[Implement Lossless Client IO](https://chatgpt.com/c/6a9b7241-e52c-83ec-8427-5fd90dd34904)
in Ego task space 122, tab `6A78CA7CE5DB24A33D4DA65583A95E67`.
The page remained terminal, with two user messages, no Stop control, and final
status **BLOCKED_EXECUTION_OBSERVABILITY**. No verified four-patch package,
production source, or downloadable implementation artifact had been produced.
The final response describes an inability to inspect execution outputs; it
does not provide a new source-level or algorithmic counterexample.

The earlier 12:06 observation, including the visible channel-check stdout and
`Corrected archive present: True`, is retained in the h128 worktree at
`coordination/handoffs/PRO_TERMINAL_OBSERVATIONS_20260905_1206.json`.
Visible webpage stdout does not establish that Pro could consume its internal
tool results. Root has not declared the execution channel repaired.

The corrected input was already supplied once at 10:43; its 3,132,684-byte ZIP
has SHA-256
`4c8295d56ca59d39441adbdc2fe24e87bb2bafaab4128b19265ba159337f329c`.
The previously missing five official source objects were added and verified.
The current terminal outcome is not a recurrence of that closed input-closure
defect and is not a semantic RED.

The visible “Too many requests” dialog was left untouched. Its continued
presence is not evidence of a newly observed HTTP response. This check did
not stop, refresh, acknowledge, retry, resend, reattach, or otherwise disturb
the conversation. No new Pro task was dispatched.

## Decision and bounded ownership

The user has delegated routine technical decisions and asked that unavailable
agents not block implementation. The project workflow permits Codex and
available independent reviewers to proceed when an external agent supplies
no usable result. Accordingly:

1. Retain the approved Pro design and the task's frozen numerical and ownership
   contract; do not replace the architecture because of an execution-channel
   failure.
2. Assign `/root/h128_candidate_spec` to author **only Cycle-A RED** in this
   I/O worktree: the public first-operation tracer, independent arithmetic and
   transform oracles, reviewed malformed-key checks, and additive CMake/CI
   integration. This agent does not own production GREEN, commits, pushes,
   remote CI dispatch, or external-agent UI.
3. Assign `/root/h128_candidate_standards` a read-only independent check of
   frozen rational vectors, baseline CTest/API bindings and exact-pin key/API
   facts. Root owns the source/spec reconciliation and the actual runtime
   acceptance decision. These are Codex agents, not Fable or ZCode identities.
4. Preserve all 57 legacy tests and the five API targets. The one new CTest
   remains `precision_client_io_first_mult2_contract`, at position 58. Establish
   the legacy build/test checkpoint before explicitly building the missing
   public-I/O target so the expected RED cannot conceal an old regression.
5. After independent inspection, commit and push the RED separately. Only
   after actual Linux and Windows results establish the intended missing-seam
   failure may the smallest Cycle-A production GREEN be authored. Clone and
   shared-parameter-drift coverage remains the separate Cycle-B RED/GREEN.

There is no new interface-confirmation request. Historical confirmation-token
language inside the original design is source material and is superseded by
the user's explicit delegation and `coordination/TEST_SEAMS.md`.

The unchanged contract includes multiprecision-only slot values, exact reduced
scales, fixed N64/S16/gap2, all 16 inputs/products, the sub-binary64 witnesses,
`2^-80` cryptographic error gates and `2^-120` independent decoder checks.
Production must use official scheme Encrypt and Poly Decrypt, not copied
cryptography, ordinary binary64 Plaintext decoding, or DecryptCore. Existing
`double_ckks.cpp`, legacy tests and thresholds are not to be changed.

## Current evidence limits and continuation

At this decision point, **I/O has no observed RED or GREEN**. Local work is
limited to low-load source/static work; no Mac OpenFHE build, CTest, encryption,
decryption, NTT or benchmark has been run for this fallback. No new ZCode or
Fable review is claimed, and the previous Fable 403/no-inference result remains
an unavailable result, not model review evidence.

In parallel, h128's final N256 B GREEN has been accepted at source `1192200f`,
with complete dual-host evidence pushed as `a5d6f93`; Repeated's actual final
review return has been verified and its acceptance pushed as `4206cd7`.
Neither result completes production I/O, paper-scale integration, eight
no-refresh squarings, 1,000 trials, or security/performance evidence.

The recurring continuation has been updated and read back to prioritize this
I/O fallback and avoid repolling closed runs. The daily 07:30 reporting branch,
idempotent Telegram delivery rules and cadence are unchanged. A later turn
must inspect the existing author-agent status and actual Git diff before
editing, to avoid overlapping ownership. Complete the next real TDD gate,
then update the continuation state again.
