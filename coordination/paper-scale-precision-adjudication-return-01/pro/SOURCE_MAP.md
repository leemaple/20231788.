# Source map and citation convention

All paths below are relative to the verified input archive `paper-scale-precision-adjudication-9f6c8eae.zip`, not to a presumed local checkout. `ALIAS:123–145` means inclusive, one-based physical text lines of that payload. Raw-log references retain timestamps and count every physical line; Windows references use the supplied LF-normalized payload. PDF pages are physical, one-based pages. `CALC:/...` is a JSON pointer into this package's independently generated `CHECK_RESULTS.json`. A `*` in a CALC path is explicit shorthand for both `linux` and `windows`, not a literal JSON key. None of these references implies an independently fetched Git object.

| Alias | Exact input payload |
|---|---|
| TASK | `TASK.md` |
| IM | `MANIFEST.json` |
| PC | `project/coordination/paper-scale-integration-01/PRODUCTION_CONTRACT_01.md` |
| NOM | `project/coordination/paper-scale-integration-01/NOMINAL_SCALE_AUDIT_01.md` |
| INPUT | `project/coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md` |
| OA | `project/coordination/paper-scale-integration-01/ORACLE_ADVERSARIAL_AUDIT_01.md` |
| SCOPE | `project/coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md` |
| SEAMS | `project/coordination/TEST_SEAMS.md` |
| DCK | `project/src/double_ckks.cpp` |
| REP | `project/src/repeated_mult2.cpp` |
| IO | `project/src/high_precision_client_io.cpp` |
| KEY | `project/src/paper_h128_client_keypair.cpp` |
| TEST | `project/tests/paper_full_eight_square_contract_test.cpp` |
| ORACLE | `project/tests/paper_full_eight_square_oracle.h` |
| PKE | `official-full/src/pke/lib/schemerns/rns-pke.cpp` |
| DEC | `official-full/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp` |
| DCRTH | `official-full/src/core/include/lattice/hal/default/dcrtpoly.h` |
| DCRTI | `official-full/src/core/include/lattice/hal/default/dcrtpoly-impl.h` |
| TUG | `official-full/src/core/include/math/ternaryuniformgenerator-impl.h` |
| LEVELED | `official-full/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp` |
| CONTEXT | `official-full/src/pke/include/cryptocontext.h` |
| HYBRID | `official-full/src/pke/lib/keyswitch/keyswitch-hybrid.cpp` |
| TRANSFORM | `official-full/src/core/lib/math/dftransform.cpp` |
| BOOST | `boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp` |
| TRIG | `boost-1.83.0/include/boost/multiprecision/detail/functions/trig.hpp` |
| PAPER | `paper/PAPER-2023-1788.txt` |
| PDF | `paper/PAPER-2023-1788.pdf` |
| OL | `evidence/LINUX_RAW.log` |
| OW | `evidence/WINDOWS_LF.log` |
| OM | `evidence/RUN_TERMINAL_01.json` |
| SL | `evidence/signed-diagnostic-run/LINUX_RAW.log` |
| SW | `evidence/signed-diagnostic-run/WINDOWS_LF.log` |
| SM | `evidence/signed-diagnostic-run/RUN_TERMINAL_01.json` |
| LA | `evidence/signed-diagnostic-run/LINUX_VERIFICATION.json` |
| WA | `evidence/signed-diagnostic-run/WINDOWS_VERIFICATION.json` |
| LS | `evidence/signed-diagnostic-run/LINUX_SIGNED_ERROR.json` |
| WS | `evidence/signed-diagnostic-run/WINDOWS_SIGNED_ERROR.json` |
| SA | `evidence/signed-diagnostic-run/SIGNED_ERROR_AUDIT.md` |
| FPA | `evidence/FRESH_PROPAGATION_AUDIT.md` |
| PREVIOUS | `review/precision-diagnosis-return/PRO_DIAGNOSIS.md` |

The four corresponding current public headers under `project/include/openfhe_2023_1788/` were also read in full. The paper's public IACR version was consulted only as a supplementary reference; the archived PDF/TXT and their manifest identities govern this review. The public theorem display was not silently substituted for the archived display.

Input identity:

- Archive: 2,046,500 bytes; SHA-256 `1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da`.
- Manifest SHA-256: `d19303d2d6fd5364f14c3076ed2d3f306a6e90da4d11d257deab0e6b1e7e2c98`.
- Documentation checkpoint: `6266b49d54b0de190e267dd97a504ad2b02af8a8`.
- Tested source: `9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e`.
- Manifest's production source attribution: `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`.
- OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The manifest checks establish the supplied bytes and closure, not the authenticity of external hosting, the original local captures, or a fresh Git comparison between the two production commits.
