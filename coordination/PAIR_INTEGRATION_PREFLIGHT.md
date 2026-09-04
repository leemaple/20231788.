# Pair integration — bounded merge preflight

## Frozen inputs and actual Git result

- Integration: `9f0dcc653b8fcaf36cf69a2353325f7117214fd6`.
- Pair: `2a048f4e6c9aee0fee46a1ca2d37030ac5fbfbf3`.
- Actual automatic merge-base:
  `8a465764044d8b1e1578f462ea4916f7123428a4`.

Commands: `git merge-base <integration> <pair>` and
`git merge-tree --write-tree <integration> <pair>` using the exact IDs above.
The latter returned **exit 1**, candidate tree
`0cd3d91ee8e3eb317317df15d0b1af752bff9e8a`, and only one text-conflicted path:
`.github/workflows/dcp-rcb.yml`. CMake and the ordinary Mult2 oracle auto-merged.
No actual merge, index update, ref change, commit, push, CI or compilation ran.
Only the authorized Git internal tree objects and this note were written.

## Real blockers

1. **Workflow conflict, not a clean merge.** Candidate workflow lines 14–18
   conflict on precision/Pair branch admission; 108–116 conflict on Linux
   focused steps; 266–282 conflict on Windows focused name/commands. Both
   branches and BOTH focused tests must survive. Windows needs two complete
   step blocks, each retaining its msys2 shell, working directory, prefix/build
   variables and PATH setup. Choosing either side wholesale would discard
   required coverage. Both alternatives currently appear only inside conflict
   markers; their presence is not a valid runnable workflow.
2. **Semantic C++ conflict missed by textual merging.** Candidate
   `tests/mult2_e2e_oracle_test.cpp:1333–1341` defines the sole
   `PrintCertificate` with nine required arguments, including
   `FixedKeyBvBound*` and `FixedKeyBvApplication`. The new Pair runner at
   `:1832–1833` still supplies seven arguments. Thus the auto-merged source
   cannot compile as-is. This is a static signature finding, not an observed
   compiler failure. A narrow test-only adapter should obtain the existing
   unavailable-BV application via
   `CheckFixedKeyBvApplication(nullptr, composed, arithmetic)` and pass
   `nullptr`, that application, then `slots`. Do not revert the BV-aware
   signature or broaden the HYBRID-only Pair case. No adapter was implemented.

Line numbers above refer to the exact candidate tree, not the unchanged
integration working-tree files.

## Preservation checks on exact blobs

- CMake is the exact ordered union: integration 55 + two new Pair bindings =
  **57**. All integration name/COMMAND pairs remain the first 55 in order;
  common parent bindings have identical commands. New entries are
  `mult2_pair_add_input_hybrid_complex` and
  `mult2_pair_sub_input_hybrid_complex`, selecting the corresponding
  `mult2_e2e_oracle_test pair_{add,sub}_input_hybrid_complex` cases.
- The oracle's original 71,943-byte prefix, including every fixed-key BV
  helper and original RunCase body, remains byte-identical to integration.
  Original selector branches/vectors are unchanged. The only deleted old
  line is the usage string extended for new selectors; no old assertion or
  threshold is removed. New Pair helpers/literals are byte-identical to Pair.
- All other **18** integration files under `tests/` are byte-identical,
  including both precision contract tests and both shared fixture files.
- `src/` and `include/` subtree IDs equal both parents AND the merge-base:
  `cf3c6521440227352df702befea951c2000b9bd0` and
  `0e48eec8c57c42cb584d4881d8ebf77eca80ead0`, respectively. No path is deleted.
- Outside the conflict markers, both provenance steps, both full-suite steps
  and all ten platform-specific API build steps remain present. Focused
  preservation is still a resolution obligation, not an accomplished result.

Static checks used only `git show`, `git diff`, `git ls-tree`, `git rev-parse`
and bounded Node string/binding comparisons of these clean-room Git objects.
Candidate SHA256: CMake
`2a05a14ec0676657e4a4349426a8ec50ef9886a1952dd9c3096a94f863362248`;
oracle `7cc0a033377304df19df78c1c4d048df95519f86ffb4e2f2cbf9fdaf2e886284`.

## Integration checklist — pending owner action

1. Reconcile the returned final ZCode review separately. Its untracked Pair
   return directory was observed but not read or included in these exact
   committed inputs; this preflight is not review acceptance.
2. Resolve only the identified workflow regions, retaining precision and
   Pair focused runs on BOTH platforms and both branch-admission entries.
3. Apply the narrow HYBRID Pair certificate-call adapter; preserve the
   BV implementation, all existing assertions and frozen tolerances.
4. Recheck conflict-marker absence, exact 57-name/COMMAND closure, no deleted
   paths, all four precision blobs, production/header identity, and unchanged
   original oracle bodies. Revalidate exact parents if either branch advances.
5. Freeze the actual resolved source and obtain NEW hosted warning-clean
   builds, five API targets, precision focused 1/1, Pair focused 2/2 and full
   57/57 on both hosts, with exact in-band source/run/attempt, independent
   precision record checks and retained BV certificates. None is claimed here.

Both worktrees were initially source-clean; Pair had only the owner's
untracked final-review return directory. No old implementation or OpenFHE
tree was accessed. Full-project completion is not implied.
