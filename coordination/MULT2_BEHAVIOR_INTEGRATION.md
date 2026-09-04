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
