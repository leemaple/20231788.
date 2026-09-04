# Mult2 behavior-red integration

Source baseline fbc649faf76144608558f1b402c59d0d3c0abdea, codex/mult2-01.
Candidate returned by ChatGPT Pro after81m35s:
https://chatgpt.com/c/6a9a3978-559c-83ec-9433-7f208af4fa05
Title: 实现 Mult2 工程. Stop answering absent; response actions visible.

Original ZIP mult2-pro-a3a6a17-mult2-delivery.zip:188514 bytes,
SHA-2564003c9103b3a4251e061072ff30e0365bcd80678019ce2471e80633ffdf72b28.
Downloaded via the existing browser conversation; verified locally against the
visible filename/size/hash. ZIP listing has no unsafe path or symlink; Gitleaks
8.30.1 archive-depth2 scan0 findings. Independent Node checks verified all58
manifest entries, exact closure and SHA256SUMS. No supplied verifier executed.

Only behavior-red patch3 is integrated now, adapted solely to current CMake
context. tests/mult2_test.cpp byte-matches patch3 and the returned final file.
The pre-existing const API/scaffold red-green was independently completed in
commits c4c98b6/40b6a78; do not replay or erase that history. Production Mult2
still deliberately throws logic_error("DoubleCKKS: Mult2 is not implemented").
Only after a genuine runtime red will the proposed one-line composition be added.
Expected suite:39 inherited cases plus mult2_composition_contract=40.

This first behavior slice has an independent deterministic host product oracle,
genuine Encrypt/DCP/Mult2/RCB/Decrypt and a frozen1e-3 functional tolerance,
explicitly normalizing decoded output with actual q_div*q_l/2^(2p).
It does not prove53/106-bit precision, a conservative non-wrap bound, or transparent
RCB->Decrypt semantics. The subsequent independent coefficient/certificate
oracle and negative tests have not yet been applied or run. Their eventual
integration must preserve independently visible uncorrected decoder bias and
the distinction between measured E_Relin and a conservative theorem bound.

Codex agrees with Pro's algebraic missing-factor inference, but does not accept
its q_div5/q_l7 example as an exact centered-RS result (5/7 is ideal unrounded
scaling). The exact integral fixture in MULT2_SCALE_ALGEBRA_CHECK.md remains the
stronger independent illustration. This is not an author-confirmed erratum.
The existing RS2 branch's later validation fixes/coverage must be reconciled
before main integration; they are not present in this Mult2 source baseline.

## Observed runtime red and minimal production change

Red commit30d6d0ecefadfd0d524411de39f5ed016ddc08bc, run33838194537,
Linux job100914915018: default warning build and Relin2/RS2 API targets succeeded;
39 inherited tests passed, only mult2_composition_contract failed at
DoubleCKKS: Mult2 is not implemented. Total0.47s, CTest exit8. Full relevant
sections captured in artifacts/tdd/mult2-behavior/red-linux.txt before writing
the implementation. The later Mult2 API step was skipped after the expected
runtime failure; its independent earlier API evidence remains applicable.
Windows red job100914915298 remains in progress; it was not restarted/cancelled.

After that verified runtime red, the scaffold was replaced only by the exact
Pro composition return RS2(Relin2(Tensor2(left, right))). No upstream algorithm,
test vector, tolerance or correction factor was changed. Candidate-green hosted
execution is pending; no successful multiplication result is claimed yet.

## Subsequent authoritative results

Windows red completed in run33838194537, job100914915298:39/40 passed,
only the intended Mult2 scaffold failure,0.78s. Both hosts now prove the same red.
Minimal production commit59143025e0b6a64c94ec099e43dabbd059ddeb4f triggered
run33838399740. Linux job100915499092 completed success:40/40,0.45s;
warning build and all configured explicit API steps passed. Windows job
100915498888 is live building pristine OpenFHE. Do not restart or cancel it.
Relevant raw sections are retained beside red-linux.txt.

Next owner Codex: collect Windows terminal green; then inspect/integrate the
independent cpp_int e2e certificate oracle, followed by scoped negative tests,
one reviewable coverage boundary at a time. Preserve host vectors and the frozen
tolerance; do not tune them to fit results. Add verbose focused hosted output
for numerical certificates before claiming measured error values. The returned
oracle has NOT yet been integrated or executed. Add/Sub Pro task and RS2 Pro
follow-up remain live; no duplicate task submission or interruption occurred.
