# Relin2 remediation 03 receipt and review gate

Recorded: 2026-09-01 Asia/Shanghai

Status: **archive, checksum, secret scan, exact-base replay, production freeze,
and bounded static arithmetic review passed; candidate acceptance is `changes
needed`**. None of the returned patches has been applied to the real
`agent/codex-relin2-01` branch, compiled on the Mac, committed, pushed, or sent
to hosted CI/Windows.

## Conversation and received artifact

- Saved conversation:
  `https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9`
- Ego Lite task space: `85`
- Page-reported natural response duration: approximately `25m34s`
- Page verdict: `ready to apply`
- Claimed exact base commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- Claimed exact base tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`
- Claimed pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Claimed and independently reproduced final tree:
  `342436fab92fa2daf005ec557ee300cbe330122e`
- File:
  `artifacts/incoming/chatgpt-pro-relin2-remediation-03/chatgpt-pro-relin2-01-remediation-03-delivery.zip`
- Local size: `49,641` bytes
- Local SHA-256:
  `cd3c5c43214023c997ee2a1ab7802cc096c293314349c392b509fe92330ca2a0`
- The first visible-control click produced no download event or local file.
  After that zero-event state was checked, one recovery click produced the sole
  completed browser event, GUID
  `d4e1201f-48de-424f-971c-37cb230a56f3`, with exactly `49,641` bytes.
- Pro reported local Linux `37/37`; Windows, hosted Linux/Windows, project Git,
  and Fable5 were explicitly pending. This is an untrusted source-agent claim,
  not Codex build/test evidence.

## Archive, checksum, and secret gate

The central directory contains exactly ten regular unencrypted `100644` root
entries in the required order, with no directories, links, duplicates, or
unsafe paths:

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

`unzip -t` passed. `PATCHES.sha256` contains exactly nine correctly ordered
records, and all nine hashes matched in a fresh extraction. The verified files
are retained under
`artifacts/incoming/chatgpt-pro-relin2-remediation-03/output/`.

Pinned Gitleaks 8.30.1 initially reported 17 `generic-api-key` findings. Each
finding was an exact patch/tree identity appearing in the retained final-docs
evidence, not a credential. Their exact fingerprints are retained in
`verification/gitleaks-false-positive-ignore.txt`; the resolved scan returned
exit zero and `[]`. Targeted credential filename and content scans each found
zero hits. Initial redacted and resolved reports are retained beside the
archive.

## Exact-base isolated replay

A fresh `git clone --no-hardlinks` was detached at exact base `fb862a3...`,
tree `759d519...`. For each patch, Codex independently ran an index-aware
sequence: `git apply --check --whitespace=error-all`,
`git apply --index --whitespace=error-all`, `git diff --check`,
`git diff --cached --check`, and `git write-tree`.

| Boundary | Patch SHA-256 | Independently reproduced cumulative tree |
|---|---|---|
| 01 | `6296f34d9aafc62ea501c638189118393a5196f2e5f0f7ef552a9b94811d9f64` | `28d0294bce8844fa831951879020b353568c1c13` |
| 02 | `364a1689fa93cf98a65a69a70814d40e49650477668a0bf13d5fcaf63db5c57c` | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| 03 | `bd5cff9a595d19710639ab03eba8e741b97d9fc4d6e02a7dddefcabaae2c67dc` | `eb304e89ac4d68c29e468a301b6557a766a1be70` |
| 04 | `a6ad402d8d8dc38661916b2d1049ec7e034cea74948241922b9a95dbefff10e1` | `d2eaf3bfbf3e56d67f209bd2d34b2a39b52df69b` |
| 05 | `9428ca4c90fef5a44ef5f994f0bc49e43365ed9b95f9da5c429801faa75ccc12` | `1a225f784bf9c8ba9b1b0d0151f809297d455b48` |
| 06 | `60bdfb21b007816ba626c5186c92ae4a909eae3360f85aecee80234a28ff396f` | `9a335343069095ae76160c2bb4942fdc51c8734a` |
| 07 | `eff1878dff7798c57ede11eb9d4128e0421411d009539d8b7d6c029546ad88d4` | `342436fab92fa2daf005ec557ee300cbe330122e` |

All stated cumulative trees and both frozen production hashes reproduced. One
disposable, never-pushed review commit
`fd614f2e4e9fad115293352bafa24826ff54905d` binds the final tree over parent
`fb862a3...`; the replay worktree is clean.

## Passed bounded gates

- The seven semantic boundaries, file modes, and patch order are intact.
- `src/double_ckks.cpp` SHA-256 is the frozen
  `96db76b6911bea7f7c1481efdadbb051f1c7b303a694be878bbb1853a8639e3f`.
- `include/openfhe_2023_1788/double_ckks.h` SHA-256 is the frozen
  `0236736b8e34f71fdd871fb8fc5e338eb48d713eef5dd20258633a67f788d1f1`.
- Production retains exact `(u, v+w)` on unchanged `Q_l`, exactly two public
  output-returning `Relinearize` calls, one private DCP seam, and no production
  `try`/`catch`.
- The retained CMake candidate registers 37 tests in total, 31 of them Relin2
  cases. No reviewer in this gate built or executed them.
- Every six successful source-level `module.Relin2(tensor)` site has the Tensor
  and deep-cache comparisons as the first two post-return statements.
- The required lifecycle/scale table and technique-specific HYBRID/BV proof
  now exist in external `REVIEW.md`.

## Formal review axes

Fixed point: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.

Review-only candidate: `fd614f2e4e9fad115293352bafa24826ff54905d`.

Diff command:

```text
git diff fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9...fd614f2e4e9fad115293352bafa24826ff54905d
```

No delegated reviewer edited, built, or tested the candidate.

### Standards

`CHANGES NEEDED`:

- **P2 hard:** `README.md:10-14,27` describes Tensor2 as the current slice,
  Relin2 as both unimplemented and present, and final Tensor2 verification as
  pending. The final candidate's live-state claims contradict one another.
- **P2 hard:** `artifacts/tdd/relin2/07-hardening-test-mutations.txt:2` and
  `08-hardening-production-mutations.txt:2` invoke unretained
  `/mnt/data/relin2_r3/inject_*_r3.py` programs. The candidate retains neither
  their bytes nor an exact mutation diff, so an independent reviewer cannot
  determine what was changed before the claimed failures.
- **P3 judgement call — Mysterious Name:** the public field
  `approximateLogicalScalingFactor` is less explicit than the Tensor field
  name once a separate recombined scale exists.
- **P3 judgement call — Mysterious Name:** private decomposition results are
  consumed as `.first`/`.second` rather than named quotient/remainder values.

The two P3 observations are recorded but not adopted: both production files
are frozen, the names are already part of the accepted bounded API/production
shape, and changing them would violate the remediation scope without fixing a
demonstrated defect.

### Spec

`CHANGES NEEDED`:

- **P1:** remediation-03 lines 138-140 require a complete audit of every
  production Relin2 invocation. The retained audit at
  `artifacts/tdd/relin2/10-fail-closed-static-scans.txt:150-176` matches only
  exact successful assignments. It omits the four existing failure-call sites
  at `tests/relin2_test.cpp:1145,1165,1182,1280`, and an unclassified new call
  would not fail the asserted successful-site count.
- **P1:** remediation-03 line 167 requires zero forbidden primitives in the
  exact independent-reference scope. The retained extraction at
  `10-fail-closed-static-scans.txt:125-142` includes only
  `BuildReferenceRelinPaths`, while that function transitively uses
  `RaiseHighReference` and `RaiseElement` at `tests/relin2_test.cpp:763-786`.
  Forbidden production logic added to either helper would evade the scan.
- **P1:** remediation-03 lines 165-166 require a fail-closed proof that
  production does not mutate or restore the evaluation-key cache. The regex at
  `10-fail-closed-static-scans.txt:120-123` is a partial blacklist: operations
  such as `erase`, `insert`, `emplace`, `clear`, `swap`, mutable aliases,
  A/B-vector setters, or pointee/context assignment can evade it.
- **P2:** remediation-03 lines 126-131 require complete Tensor/cache snapshots
  immediately before a successful call. At `tests/relin2_test.cpp:988-993`,
  `1094-1099`, and `1118-1123`, two metadata snapshots intervene. The metadata
  snapshots must move earlier, or the complete Tensor snapshot must supply the
  needed metadata expectation.

### Adversarial TDD/evidence

`CHANGES NEEDED`:

- **P1:** the source-order audit is not closed over all ten lexical production
  call sites (six success, four failure), so removing failure-path
  immutability checks or adding a differently formatted call can leave the
  gate green.
- **P1:** the two mutation logs contain only calls to absent injection scripts.
  Hash, exit, and restoration lines alone do not prove which nine changes were
  actually injected; a script could manufacture the same expected error text.
- **P2:** the cache-mutation blacklist is incomplete and therefore cannot
  support a universal zero-mutation claim.

Summary: Standards has two hard P2 findings and two non-adopted P3 judgement
calls; Spec has three P1 and one P2 findings; adversarial TDD/evidence has two
P1 and one P2 findings. The worst Standards issue is non-reproducible mutation
evidence; the worst Spec issue is a fail-open verification surface.

## Accepted blockers and bounded corrections

1. Replace the successful-only call audit with a complete, fail-closed audit
   that enumerates and classifies every lexical `module.Relin2(` call. It must
   cover all six successful and four failure source sites, validate the
   required pre/post order for each class, and fail on any unclassified site.
2. Extract and scan the complete transitive independent-reference scope:
   `RaiseElement`, `RaiseHighReference`, and `BuildReferenceRelinPaths`.
3. Replace the cache-mutation token blacklist with a fail-closed proof. The
   frozen whole-production SHA-256 must be asserted, the complete
   `ValidateRelin2EvaluationKey` and `Relin2` bodies must be retained, and any
   semantic access audit must be allow-listed or otherwise reject an unknown
   operation rather than guess at a finite blacklist.
4. Make the complete Tensor and deep-cache snapshots the literal final two
   observations before each successful production call; move metadata-only
   snapshots earlier.
5. Retain the exact source mutation for every hardening run, for example a
   literal pre-run `git diff --no-ext-diff --binary` plus a non-empty-diff
   assertion and post-restore zero-diff/hash assertion. The injection scripts
   need not enter the applied project tree, but their effects must be
   independently auditable from retained evidence.
6. Correct README live-state facts: the bounded candidate extends accepted
   DCP/RCB/Tensor2 with Relin2; Relin2 remains a not-yet-integrated candidate;
   RS2, Mult2, and pair Add/Sub remain outside the slice; hosted Linux/Windows,
   Fable5, integration commits, and pushes remain pending.

## Decision and downstream state

The candidate remains quarantined. The next request is a narrow remediation in
the same saved ChatGPT Pro conversation. The two production files and all
accepted algorithm/state behavior remain byte-frozen. Only patch-03 test
ordering and auditability plus patch-07 evidence/documentation may change.

No Mac build occurred. No real implementation commit/push, hosted run, Windows
run, or Fable5 call is claimed. The one authorized terminal Fable5 review
remains reserved for the first exact Relin2 commit that passes the same SHA on
hosted Linux and Windows.
