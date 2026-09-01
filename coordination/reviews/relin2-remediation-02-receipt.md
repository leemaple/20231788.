# Relin2 remediation 02 receipt and review gate

Recorded: 2026-09-01 11:18 CST

Status: **archive, checksum, secret-scan, exact-base replay, and static
production gates passed; candidate acceptance is `changes needed`**. None of
the returned patches has been applied to the real `agent/codex-relin2-01`
branch, compiled on the Mac, committed, pushed, or sent to hosted CI/Windows.

## Conversation and received artifact

- Saved conversation:
  `https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9`
- Ego Lite task space: `85`
- Page-reported natural response duration: approximately `40m`
- Page verdict: `ready to apply`
- Claimed exact base commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- Claimed exact base tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`
- Claimed pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Claimed final tree:
  `f3611ffb38be0975a891834995c8a3081bf09ddc`
- File:
  `artifacts/incoming/chatgpt-pro-relin2-remediation-02/chatgpt-pro-relin2-01-remediation-02-delivery.zip`
- Local size: `46,234` bytes
- Local SHA-256:
  `9b50e746e05425afd6ef6a9d56a141ea19d13ec446c7464e8f614ac923608528`
- The first UI click attempt produced no browser download event, history row,
  or local file. After that zero-event state was proved, one recovery click on
  the scrolled-into-view control produced the sole completed download event.
  The page-reported size/hash and local recomputation matched exactly.
- Pro reported local Linux `37/37`; Windows, hosted Linux/Windows, and Fable5
  were explicitly `pending`. The local result is an untrusted delivery claim,
  not Codex project test evidence.

## Archive, checksum, and secret gate

The central directory contains exactly ten regular `100644` root entries in
the required order, with no directories, links, duplicates, unsafe paths, or
encryption:

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
records, and all nine hashes matched in a new extraction. The verified files
are retained under
`artifacts/incoming/chatgpt-pro-relin2-remediation-02/output/`.

Pinned Gitleaks 8.30.1 initially reported seven `generic-api-key` findings.
Every finding was the same deterministic evidence line containing
`patch=01-red-relin2-api.patch sha256=6296f34d...d9f64`; the 64-hex value equals
the actual patch-01 SHA-256 and the first manifest record. It is not a secret.
The seven exact fingerprints were recorded in
`verification/gitleaks-false-positive-ignore.txt`; a default scan with only
those fingerprints ignored returned `no leaks found` and an empty resolved
report. Targeted credential filename/content scans also returned no hit. The
initial redacted report and resolved report are retained under `verification/`.

## Exact-base isolated replay

A fresh `git clone --no-hardlinks` was detached at exact base `fb862a3...`,
tree `759d519...`. For every patch, Codex independently ran an index-aware
sequence: `git apply --check --whitespace=error-all`,
`git apply --index --whitespace=error-all`, `git diff --check`,
`git diff --cached --check`, and `git write-tree`.

| Boundary | Patch SHA-256 | Independently reproduced cumulative tree |
|---|---|---|
| 01 | `6296f34d9aafc62ea501c638189118393a5196f2e5f0f7ef552a9b94811d9f64` | `28d0294bce8844fa831951879020b353568c1c13` |
| 02 | `364a1689fa93cf98a65a69a70814d40e49650477668a0bf13d5fcaf63db5c57c` | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| 03 | `8d354d75831a918fbeca64a18de571816bd196a4dcbbbd0c848fa62544d1ea42` | `81441392a9ac33e7458fa0d8a177d77babc811b1` |
| 04 | `a6ad402d8d8dc38661916b2d1049ec7e034cea74948241922b9a95dbefff10e1` | `e09ea3cee26b96e2d6125e3e084dcaf494ca0485` |
| 05 | `0c0951c6627f5aeb93aa30bf6164efd5bd97baa8707da04d2139fe905cc4052c` | `057cc9357baba384f50432f1dfef278778220798` |
| 06 | `60bdfb21b007816ba626c5186c92ae4a909eae3360f85aecee80234a28ff396f` | `2f71134d71234a470669281493d481ab124d8b70` |
| 07 | `c48488c2384133c2e3b3526f27cef84c588861dd49b146728475b974ab513255` | `f3611ffb38be0975a891834995c8a3081bf09ddc` |

The final tree exactly matches the Pro response and outer `TESTS.md`. One
permitted disposable, never-pushed review commit
`36780e80b5f37f148ec9264a101835f2f5df499e` binds that tree over fixed point
`fb862a3...`; its temporary review worktree was clean.

## Passed static gates

- The seven patch scopes and order are intact. Patch 01 is API/workflow/CMake
  only; patch 02 is declarations/scaffold; patch 03 is core tests; patch 04 is
  core production plus minimum accepted-suite changes; patch 05 is the
  test-only directed lifecycle red; patch 06 is the minimum guard; patch 07 is
  documentation/retained evidence only.
- Production implements exact `(u, v+w)` without consuming a working tower,
  has exactly two output-returning public `context_->Relinearize` calls, and
  retains read-only key lookup with `find(tag)` and index zero.
- No forbidden production key-switch, rescale/mod-reduction, cache-mutation,
  `try`/`catch`, legacy route, dependency, OpenFHE modification, or out-of-scope
  RS2/Mult2/Add/Sub implementation was found.
- The previous six remediation blockers are materially corrected: complete
  metadata map/value identities and provenance, context-aware deep cache
  restoration with an actual context mutation, every NativePoly tower format,
  the exact recombined diagnostic, and the patch-05 Tensor/cache snapshot.
- Retained patch-boundary logs now visibly contain an actual index update and
  `git write-tree` command/output. Codex's independent replay reproduced every
  stated tree.

## Formal review axes

Fixed point: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.

Review-only candidate: `36780e80b5f37f148ec9264a101835f2f5df499e`.

Diff command:

```text
git diff fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9...36780e80b5f37f148ec9264a101835f2f5df499e
```

No reviewer built or tested the candidate.

### Spec

`CHANGES NEEDED`:

- `tests/relin2_test.cpp:1274-1282,1369-1372` represents five successful
  Relin2 executions. They read `result.GetLifecycle()` before comparing the
  source Tensor and deep key cache, so a failed result assertion can mask the
  required immediate immutability proof.
- External `REVIEW.md` omits the original required lifecycle/scale table and
  the technique-specific HYBRID/BV key-shape/basis proof.
- `coordination/RELIN2_DESIGN.md:23` and external `REVIEW.md:51` make a false
  universal immediate-order claim.

No Spec finding was raised against the static production arithmetic, basis
restoration, public relinearization count, lifecycle/scale state, or scope.

### Standards

The delegated Standards axis returned `CHANGES NEEDED` with four observations.
They are recorded separately and not silently merged into the Spec result:

- It requested committing the external patches and mutation-injection scripts
  into the candidate tree. This is not adopted: the exact ten-file delivery
  already contains all seven patches, while patch 07 is explicitly required to
  bind only patches 01-06/evidence without creating a self-cycle. The governing
  task requires mutation evidence to identify each temporary change, failure,
  restoration, and final green; it does not require shipping `/tmp` scripts.
- It objected to registering all 30 contract cases in patch 03. This is not
  adopted: the fixed seven-boundary task explicitly places the complete core
  contract in patch 03 and requires independent registration so one failure
  cannot mask later cases.
- It objected to a non-throwing restoration destructor's catch-all. This is not
  adopted as a new blocker: remediation 02 explicitly requires a non-throwing
  destructor during assertion unwinding and a post-scope positive deep proof;
  the candidate provides that proof at `tests/relin2_test.cpp:706-752`.
- It suggested replacing the private decomposition helper's `.first/.second`
  with a named type. This judgement-call smell is not adopted because the
  production shape already passed Spec and this remediation forbids unrelated
  production redesign; KISS/YAGNI favor leaving it unchanged.

### Delivery/evidence and adversarial TDD

Both axes independently returned `CHANGES NEEDED` for the immediate-order
defect. Delivery/evidence additionally found that the final static-scan record
shows labels and counts only, not the actual commands, patterns, scopes, and
exit codes. Therefore its zero forbidden/try-catch/legacy counts and exact-two
Relinearize claim are not mechanically reproducible from retained evidence.

## Accepted blockers

### P1 — five successful executions do not prove immutability first

In patch 03, the successful branch of `RunKeyMutation` and
`TestValidTechnique` inspect result lifecycle before calling both source-Tensor
and deep-cache comparisons. Because those helpers execute five cases, the
universal claim is false five times. Move both comparisons to the first two
statements after each production return, before any result inspection,
reference call, destruction, or other later operation. Audit every production
Relin2 call mechanically and retain evidence of the source-order check.

### P1 — final static scans are not replayable

`TESTS.md:319-327` and the retained final log record only human labels and
totals. They do not record the actual command, exact pattern set, Relin2
function/production scope, or exit status for the forbidden-symbol,
production-try/catch, exact-two-Relinearize, and `legacy_*` scans. Regenerate
the final evidence with the full commands and outputs, and make the scan fail
closed on a nonconforming count rather than printing a desired number.

### P2 — required review proof is missing

The original output contract requires `REVIEW.md` to contain a lifecycle/scale
table and a key-cache plus HYBRID/BV proof. The current narrative has neither a
table nor technique-specific expected A/B lengths and exact bases. Add those
from the frozen contract and verified pristine OpenFHE observations; do not
change production to match documentation.

## Decision and downstream state

The candidate remains quarantined. The next request will ask ChatGPT Pro in the
same saved conversation, with all necessary authority and candidate files
reattached, for a fresh complete replacement seven-patch series from exact base
`fb862a3...`. The repair is limited to patch-03 test ordering and patch-07/
external documentation/evidence; accepted production code must remain
byte-equivalent unless an independently demonstrated blocker requires change.

No Mac build occurred. No real source commit/push, hosted run, Windows run, or
Fable5 call is claimed. The one authorized terminal Fable5 review remains
reserved for the first exact Relin2 commit that passes the same SHA on hosted
Linux and Windows.
