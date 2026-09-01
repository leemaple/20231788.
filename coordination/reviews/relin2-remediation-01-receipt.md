# Relin2 remediation 01 receipt and review gate

Recorded: 2026-09-01 09:55 CST

Status: **archive/replay/production-spec gates passed; candidate acceptance is
`changes needed`**. None of the returned patches has been applied to the real
`agent/codex-relin2-01` branch, compiled on the Mac, committed, pushed, or sent
to hosted CI.

## Conversation and received artifact

- Saved conversation:
  `https://chatgpt.com/c/6a960223-f7d8-83ec-9ad1-ac404f614ba9`
- Ego Lite task space: `85`
- Natural response duration: `48m 02s`
- Page verdict: `ready to apply`
- Claimed exact base commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- Claimed exact base tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`
- File:
  `artifacts/incoming/chatgpt-pro-relin2-remediation-01/chatgpt-pro-relin2-01-remediation-delivery.zip`
- Local size: `45,632` bytes
- Local SHA-256:
  `910f7c248b82cdc6c1d6e1a290093b96881fee0bb9cdcc06e603008c3eb74d10`
- Browser download history showed exactly one completed download event. The
  displayed size/hash and the local recomputation matched exactly.
- Pro reported local Linux `37/37`; Windows and hosted Linux/Windows were
  explicitly `pending`. Those local results remain untrusted delivery claims,
  not Codex project test evidence.

## Quarantine and checksum gate

The archive contains exactly ten regular root files in the required order:

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

There are no directory entries, links, duplicates, unsafe paths, encryption,
credentials, source archives, build products, or browser state. `unzip -t`
passed. `PATCHES.sha256` has exactly nine correctly ordered records, and every
hash matched. Gitleaks 8.30.1 scanned about 213 KB with no leak.

The retained extracted files are under
`artifacts/incoming/chatgpt-pro-relin2-remediation-01/output/`.

## Exact-base isolated replay

A disposable `git clone --no-hardlinks` was detached at exact base
`fb862a3...`, tree `759d519...`. Every patch passed
`git apply --check --whitespace=error-all`, actual application, and
`git diff --check` in order. Codex independently materialized the cumulative
index after each patch and obtained:

| Boundary | Patch SHA-256 | Independently reproduced cumulative tree |
|---|---|---|
| 01 | `6296f34d9aafc62ea501c638189118393a5196f2e5f0f7ef552a9b94811d9f64` | `28d0294bce8844fa831951879020b353568c1c13` |
| 02 | `364a1689fa93cf98a65a69a70814d40e49650477668a0bf13d5fcaf63db5c57c` | `7ea2bf2db6f87ee357da2a68f198836c9fa30d4a` |
| 03 | `bac44bdedda4dd3cf42c2210cb7933e72a3f4fedf874b5c3fcaa5ea09aa251a0` | `97e6a179f4f52c579c085dc05fb8af48fc4a7133` |
| 04 | `a058adfcde1dec1338e7c03dc0a3d7d00c4cafd79b350b7c7c764603edffabe2` | `828a65672cd3575b7dd6f5a33db5157505480181` |
| 05 | `a67e8895757912f55447e401b47f855c6674bba1738e515bbb6ac50e8d2cc7f4` | `bbcd32a458173c590cbdcab5a830da255f3a101d` |
| 06 | `60bdfb21b007816ba626c5186c92ae4a909eae3360f85aecee80234a28ff396f` | `97291044906ff4a3b3ef3694f8388a03a42c188d` |
| 07 | `cac89f27df590e10adce3297f4ed38fa9aff55bacedd70b71a8f7ff3e485b163` | `c045eb4a3f252984e0b8a3b56563b510e6bf7123` |

The final tree exactly matches the Pro response and delivery `TESTS.md`. This
proves the patch bytes map to those trees when Codex stages them; it does not
repair the delivery's missing original index-update evidence described below.

## Passed static gates

- Independent paper/OpenFHE/production Spec review: `PASS`, no P0-P3 finding.
- The production order is Tensor validation, active-basis gate, read-only key
  preflight, high raise, raised-state validation, exactly two public
  output-returning `Relinearize` calls, relinearized-high validation, one
  shared private DCP, exact pre-add validation, only `v+w`, then complete
  `ReadyForRS2` validation.
- Relin2 consumes no working tower and implements exact `(u, v+w)`.
- Production key-map binding is `const auto&`, uses `find(tag)`, and consumes
  only index zero.
- Production contains none of the forbidden calls, cache mutation APIs, or
  `try`/`catch`.
- The API signature, deterministic K0/v/w and centered-boundary arithmetic,
  public RCB return oracle, representative encrypted public-input path, and
  seven semantic patch boundaries are present.
- No old/private/known-wrong implementation or modified OpenFHE was found.

## Acceptance blockers

### P1 — cumulative-tree execution evidence is not mechanically possible as recorded

Delivery `TESTS.md:65-74,257-260` says the replay used ordinary
`git apply`, `git diff --check`, then `git write-tree`. The retained boundary
logs, beginning at `artifacts/tdd/relin2/01-api-red.txt:13-21`, likewise record
no `git apply --index`, `git add`, or `git update-index`, and do not show the
`git write-tree` command itself. Ordinary `git apply` changes only the working
tree, while `git write-tree` reads the index. Therefore the recorded command
chain cannot produce the claimed changed trees. Either an index-update command
was omitted, violating exact-command evidence, or the claimed original tree
binding is not supported. A new fresh replay must record a real index-aware
sequence and regenerate the retained evidence, patch 07, outer documents, and
hashes.

### P1 — metadata identity is not part of the deep snapshot

`tests/relin2_test.cpp:80-113` keeps only each key plus a clone of the metadata
value. It discards the input ciphertext's outer `MetadataMap` identity and the
original per-entry `shared_ptr<Metadata>` identities. Replacing an input map or
value pointer with an equal-content clone passes both `CheckMetadataMatches`
and OpenFHE ciphertext value equality. This leaves false-pass holes in Tensor
immutability, public-RCB input immutability, and result metadata provenance.

The revised test must explicitly model the observed pristine OpenFHE 1.5.0
semantics: inputs retain the exact outer map and value-pointer identities;
output-returning clone operations produce a distinct outer map whose entries
shallow-alias the expected first-source metadata values; deep value contents,
keys, order, size, nullness, and identities are all checked.

### P1 — deep key-cache RAII does not restore context identity

`tests/relin2_test.cpp:568-615` saves key tag and Relin A/B vectors, while the
saved map is only a shared-pointer copy. Public `EvalKeyRelinImpl` assignment
can replace the inherited context identity. The guard records context in its
comparison snapshot but cannot restore it after an assertion. Its restoration
proof at `:619-656` does not mutate context. The revised guard must restore
every recorded mutable pointee observable, including context for Relin and
non-Relin entries, tag, and Relin A/B state, and the nested proof must actually
mutate context and demonstrate restoration.

### P1 — per-tower format is not directly asserted

`tests/relin2_test.cpp:452-474` snapshots only the aggregate `DCRTPoly` format;
`CheckMemberState` at `:711-738` also checks only that aggregate before
inspecting tower parameters. In OpenFHE, `DCRTPoly::m_format` and each contained
`NativePoly` format are separately observable, so one tower can disagree with
the aggregate. The original contract requires every component/tower format.
The revised snapshots and result/public-RCB checks must record and compare each
tower's `GetFormat()` in addition to the aggregate format, and a temporary
hardening mutation must show the assertion detects a single-tower format
change.

### P2 — one new negative test still accepts a diagnostic substring

The newly added recombined-scale RCB corruption case at
`tests/dcp_rcb_test.cpp:753-758` calls the legacy helper at `:106-123`, which
uses `message.find`. This violates the explicit rule that every newly added
negative case compare the complete `DoubleCKKS: ...` message by equality. It
also contradicts delivery `REVIEW.md`'s claim that substring acceptance is not
used. Preserve old tests, but make this new case require exactly
`DoubleCKKS: pair recombined logical scale is inconsistent`.

### P2 — patch-05 Relin2 call omits Tensor immutability proof

`tests/relin2_test.cpp:1294-1303` snapshots and checks the deep key cache around
the lifecycle fixture's production `Relin2(tensor)` call, but does not call
`SnapshotTensor` before or `CheckTensorUnchanged` immediately after it. This
violates the all-production-calls immutability rule and makes the corresponding
universal statement in `REVIEW.md` false.

## Decision and downstream state

The candidate is `changes needed`. It must stay quarantined and must not be
applied to the real branch. The next request will ask ChatGPT Pro once, in the
same saved conversation and with every authority attachment repeated, for a
fresh complete seven-patch replacement from exact base `fb862a3...`.

No Mac build occurred. No hosted run, Windows run, Fable5 call, project commit,
or source push is claimed. The one authorized terminal Fable5 review remains
reserved for the first exact Relin2 commit that passes both hosted Linux and
Windows.
