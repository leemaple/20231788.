# Root independent integration checkpoint

## CI safety review (source only, not executed)

Current .github/workflows/dcp-rcb.yml does not include the repair branch in its push allowlist. Do not dispatch it unchanged: the legacy full-chain/finalizer/upload negative-ref conditions would include this new branch, unintentionally running the original eight-square endpoint again.

After Pro returns the exact opt-in target/option, root must first gate the repair RED/GREEN refs out of the legacy full-chain build/run/select/upload steps on BOTH hosts, then add dedicated opt-in steps. Preserve all other branches' behavior. Verify branch-condition coverage with a lightweight source-level truth table before a remote dispatch. Prefer one Linux fail-first contract build with matching missing-method diagnostic over repeating expensive baseline tests to prove an existing red. Green must run focused deterministic controls and exactly one fresh-only observation; subsequent cross-host regression must not silently invoke any old expensive chain. Preserve compiler/CTest exit codes and exact source provenance; neither arbitrary compile failure nor infrastructure failure establishes API RED. No workflow changes or dispatch have occurred yet.

## Paper-condition cross-check (reading only)

Read the supplied, packet-hashed paper Sections2 and6.1–6.3/Table3, plus current fixed Input() and Error() in tests/paper_full_eight_square_oracle.h. Paper experiments use HEaaN, whereas user explicitly requires OpenFHE. Section6.3 reports an average infinity norm over 1000 executions after eight repeated squarings; our retained gate uses fixed dyadic complex inputs near magnitude0.991 and a per-component maximum. The paper's chi_enc/chi_err setup is distinct from secret-key Hamming weight128; Table3 and the inspected experimental prose do not establish exact equivalence with OpenFHE's encryption sampling or our fixed input family. Do not invent the paper input distribution or claim this alone explains the observed discrepancy.

For complex errors, max(abs(real),abs(imag)) <= abs(complex) <= sqrt(2)*max(abs(real),abs(imag)). Consequently using complex magnitudes instead of component maxima cannot turn our same-sample failure into a pass. Averaging and input/distribution differences are separate questions. The user has removed the1000-trial requirement; do not reinstate it. A separately labelled, source-justified paper-aligned experiment may later be useful but must not overwrite the original frozen failure.

The current task measures m, p and decoder contributions first. No paper/sampler/API defect has yet been established, and no original-parameter fix is claimed.
