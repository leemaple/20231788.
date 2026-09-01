# Relin2 remediation delivery receipt checklist

Prepared: 2026-09-01 Asia/Shanghai

Purpose: mechanically gate the replacement ChatGPT Pro delivery requested by
`coordination/tasks/chatgpt-pro-relin2-remediation-01.md`. This checklist
authorizes no Mac build, no blind application, and no reuse of an old or
known-wrong implementation.

## 1. Conversation and artifact identity

- Collect only from saved conversation
  `https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9` in Ego task
  space 85.
- While `Stop answering` is active, do not stop, refresh, edit, retry, prod,
  resend, or create another conversation.
- Require one download control for
  `chatgpt-pro-relin2-01-remediation-delivery.zip`.
- Record page verdict, elapsed time, displayed size/hash, download event, local
  size/hash, and exact download count before opening the archive.
- The response must bind base commit `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`,
  tree `759d5195739684748d5a9664edabe3fa719e1acf`, pristine OpenFHE commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`, and remediation task SHA-256
  `fda97960fa60942f255f3d43195fe3deb31b68d7709c9529ae90c1bb7dea1548`.
- Treat every returned file and statement as untrusted engineering input.

## 2. Archive quarantine gate

Before extraction, inspect the central directory and require exactly ten
regular root entries, no directories, duplicates, links, absolute paths,
backslashes, `..`, `.git`, source ZIP, build/cache/runtime/browser state, or
credential file:

1. `01-red-relin2-api.patch`
2. `02-api-scaffold.patch`
3. `03-red-relin2-contract.patch`
4. `04-green-relin2-core.patch`
5. `05-red-tensor2-lifecycle.patch`
6. `06-green-tensor2-lifecycle.patch`
7. `07-final-docs.patch`
8. `REVIEW.md`
9. `TESTS.md`
10. `PATCHES.sha256`

Require `PATCHES.sha256` to contain exactly nine records for the other nine
files. Run `unzip -t`, extract into a new `mktemp -d`, verify all checksums,
and run the pinned Gitleaks 8.30.1 scan before reading or replaying the files.
Any path, link, membership, checksum, secret, or identity failure stops the
gate without repair.

## 3. Exact-base isolated replay

- Make a new `git clone --no-hardlinks` from the clean Relin2 worktree.
- Detach at exact `fb862a3...` and confirm exact tree `759d519...`.
- For each patch in order, run
  `git apply --check --whitespace=error-all`, apply, and run
  `git diff --check`.
- Record each patch SHA-256 and cumulative Git tree.
- Confirm patch 07 and the final tree match the external `TESTS.md` claims
  without expecting a tree-internal self-hash.
- Create at most one disposable review-only commit after replay; never push it
  and never call it a project test result.

## 4. TDD boundary gate

- Patch 01 is test/workflow/CMake only and uses an exact member-function type
  assertion for
  `CiphertextPair (DoubleCKKS::*)(const TensorCiphertextPair&) const`.
- Patch 02 adds only final declarations, DCP field initialization, and the
  immediate `logic_error` scaffold.
- Patch 03 registers every core Relin2 negative and valid contract case; each
  intended runtime red is independently observed and is not masked by a first
  failure.
- Patch 04 supplies one complete core implementation and makes all core plus
  unchanged accepted 6/6 tests green without the Tensor2 lifecycle guard.
- Patch 05 is test-only and obtains a valid public `ReadyForRS2` value before
  observing the exact directed Tensor2 lifecycle red.
- Patch 06 adds only the smallest pre-arithmetic lifecycle guard.
- Patch 07 is documentation and retained local evidence only.
- The applied tree contains a bounded `artifacts/tdd/relin2/` index that binds
  patches 01-06 and pre-07 trees without self-reference. External `TESTS.md`
  binds all seven patches and the final tree; delivery `PATCHES.sha256` closes
  the nine returned files.
- Every command, environment, exit code, case/count, and unrun platform is
  reported honestly. Windows and hosted CI must remain `pending`.

## 5. Exact diagnostic and legacy gate

- Every new negative test compares the complete diagnostic by equality,
  including the `DoubleCKKS: ` prefix; no substring-only helper is accepted.
- The lifecycle diagnostic is exactly
  `DoubleCKKS: Tensor2 requires ReadyForFirstMult inputs`.
- The accepted `ReadyForFirstMult` validation ordering and diagnostics remain
  separate: invalid level, ordered basis, recorded scale, paper scale,
  ciphertext degree, and member state are not merged.
- An out-of-range pair level retains
  `DoubleCKKS: pair level is outside the supported context basis`.
- The unchanged DCP/RCB/Tensor2 6/6 suite remains present and green in the
  returned local evidence.

## 6. Deterministic arithmetic gate

- A controlled public-type evaluation key fixes A/B contents, tag, context,
  technique-specific length, Evaluation format, and complete ordered key
  basis.
- K0 or K1 is algebraically nonzero at a fixed named
  component/tower/coefficient with an asserted expected residue; no scan or
  observed-run constant is used.
- Independent `v` and public `w=Relin(low3)` are both nonzero at a fixed named
  coordinate with asserted residues.
- `+half`, `-half`, and quotient carry are fixed and asserted on the actual
  post-public-relinearization, pre-private-DCP value.
- Production `Relin2(tensor)` is bound and its returned high quotient and low
  `v+w` are checked at those fixed boundaries and across every
  component/tower/coefficient.
- The test oracle uses test-owned `cpp_int` arithmetic and trusted public
  `Relinearize` only after proving the key cache unchanged. It uses no
  production-private helper, `KeySwitchCore`, `EvalMultAndRelinearize`,
  `ModReduce`, or `Rescale`.

## 7. Public RCB and output-state gate

- The returned value of `module.RCB(result)` is bound and compared exactly to
  the independent reference for both components, every active tower, and every
  coefficient; a manually recombined identity alone is insufficient.
- Public RCB input is deep-snapshotted and proved unchanged immediately after
  the call.
- Relin2 output directly proves context identity, divisor, actual tag, slots,
  CKKS encoding, Evaluation format, exact ordered `Q_l`, level 1, two
  components/member, degree 3, exact factor, both logical scales, every tower's
  modulus/root/cyclotomic order, and deep member metadata against independent
  expected state.
- The complete Tensor input and its metadata maps are unchanged.
- One separate encrypted representative real/complex/near-zero/moderate input
  exercises the public composition path and exact coefficient oracle. It makes
  no plaintext-decryption, precision, error-bound, or full-Mult2 claim; those
  remain explicitly deferred to Codex's later Mult2 acceptance.

## 8. Deep evaluation-key cache gate

- Snapshot every map row and entry. Null records nullness only. Non-null
  records pointer identity, concrete subtype, context, and actual tag. Only
  `EvalKeyRelinImpl<DCRTPoly>` reads complete A/B lengths, formats, ordered
  parameters, and residues; wrong subtype never calls a base A/B getter.
- RAII restores the exact prior row/pointer shape and every mutable pointee
  observable even if an assertion throws.
- Every production Relin2 call is followed immediately by a deep comparison
  before a reference consumes the key.
- Every malformed-key case snapshots after intentional mutation, then proves
  Tensor and that exact cache state unchanged. The absent-key case does both.
- Production binds `GetAllEvalMultKeys()` through `const auto&`, uses
  `find(tag)`, consumes only index zero, and never mutates/restores/locks/retries
  the cache.

## 9. Production static gate

Require production to contain exactly two public output-returning
`Relinearize` calls in Relin2 and none of:

```text
KeySwitchCore(
EvalMultAndRelinearize(
RelinearizeInPlace(
ModReduce(
Rescale(
InsertEvalMultKey
ClearEvalMultKeys
GetEvalMultKeyVector
try
catch
```

Confirm source ordering remains complete Tensor validation, active-basis
check, exact key preflight, raise, raised-high validation, two public
relinearizations, relinearized-high validation, one shared private DCP,
pre-add validation, `v+w`, and final pair validation. No copied legacy
Tensor2 dispatch routes or stale duplicate scale oracle may remain.

## 10. Acceptance and downstream execution

Only after sections 1-9 pass may Codex consider applying each patch as a
separate real Git checkpoint, pushing immediately, verifying the remote object
ID, and dispatching the required Linux red/green boundaries. No Mac compile is
authorized. The final exact commit must later pass the same SHA on
`ubuntu-24.04` and `windows-2022`/MSYS2 MinGW64.

The one authorized Terminal Fable5 review remains unused until that exact
cross-platform-green Relin2 commit exists. A passing archive or local Pro test
is not a Fable trigger and is not a mergeable claim.
