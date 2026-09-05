# Source map and citation convention

`ALIAS:12–20,25–30` means inclusive one-based **physical LF-delimited lines** in the exact verified input payload, not line numbers in this return's modified files. `PDF p.7` means physical one-based page7 of the supplied PDF. Source references do not assert an independently fetched Git object. Source claims in the review/specification refer to these inputs, not to an unavailable checkout.

Archive labels:

- `source`: `input/paper-scale-precision-adjudication-9f6c8eae.zip` inside the outer packet, paths relative to that nested ZIP root.
- `outer`: paths inside the supplied outer ZIP.
- `decision`: `review/paper-scale-precision-adjudication-decision-v1.zip`; add its `precision_adjudication_decision_v1/` root to the relative paths below.

The PDF is `source:paper/PAPER-2023-1788.pdf`,759375 bytes, SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`,15 pages. Complete supplied text was read; all pages were rendered; pages7,8,12,13 were visually inspected. The isolated Theorem4.8 display and constructive scale disagreement were checked against those supplied images, not inferred solely from PDF text extraction.

The structured source references include each file's exact byte size/SHA and LF-line count. The own static validator checks these directly against verified archive bytes, and mechanically validates every ALIAS:line-range citation in the four principal deliverable documents. Those checks establish existence/identity/range, not semantic truth by themselves.

| Alias | Archive | Relative payload | LF lines | SHA-256 |
|---|---|---|---:|---|
| PC | source | `project/coordination/paper-scale-integration-01/PRODUCTION_CONTRACT_01.md` | 174 | `e7db0be1fe12de0dd9ebabbdea6c2f27e29a49bd7bbcbabca69c701fe7d7378e` |
| SC | source | `project/coordination/paper-scale-integration-01/NOMINAL_SCALE_AUDIT_01.md` | 30 | `b8e7991ce0f24e1ddddc46bf02a730ab5c0391d6f0084a7b6ee6bd9c5e9dcb1d` |
| ID | source | `project/coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md` | 69 | `dfcf5dcaa6954ec49d6337b61f1bec52c5658b7cecaf25e885702744148549c9` |
| OA | source | `project/coordination/paper-scale-integration-01/ORACLE_ADVERSARIAL_AUDIT_01.md` | 42 | `361d549c631676fbb084423308779f81153de57b2fa3921cd97b4be3cc617cbf` |
| SCOPE | source | `project/coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md` | 35 | `7c252d3d8693ce98fa4af5f1fe11a00dc9952663262c3e43906710ab2ba273cd` |
| SEAMS | source | `project/coordination/TEST_SEAMS.md` | 42 | `48ac364af362369d0fe8133755ce59f385946ea32085d89979ff6078684ed5fe` |
| T | source | `project/tests/paper_full_eight_square_contract_test.cpp` | 394 | `61f3b4b0f6e7c2e6a11a40bbca51b87df520c8190537caa39bdb738910e3f727` |
| O | source | `project/tests/paper_full_eight_square_oracle.h` | 336 | `08d6c4dfe6761b84d893d90c120418a29d8a2b96aa05304e95728d72e4c3e203` |
| IO | source | `project/src/high_precision_client_io.cpp` | 719 | `ab6c1f2a39c339e951305ff19200d9b2ebd0d4a08b0a0a5c7eb212f4b74416d4` |
| IOH | source | `project/include/openfhe_2023_1788/high_precision_client_io.h` | 160 | `399cee4c932e32d75e5dfe7c0137f81917ef5569ff2032a1ee291eefbebfe56e` |
| RP | source | `project/src/repeated_mult2.cpp` | 534 | `e9b1e04c29941e16fcbb447a4f2ec7aed99e30e777dc0b1cccbffb18f98a8d9a` |
| RPH | source | `project/include/openfhe_2023_1788/repeated_mult2.h` | 121 | `fdd58bef3fcf31bacb57c7c4dc369f7e1a4647f95825feaac52bae0de31edfb1` |
| H128 | source | `project/src/paper_h128_client_keypair.cpp` | 219 | `679d4fa226b95770282fd5c877bf47044a290949bbaed501bd8f662744a6f22e` |
| DC | source | `project/src/double_ckks.cpp` | 1285 | `e9e9b7277dd9da30027cf37d1c19e6c203fc77e13313daf94196a5e37732618d` |
| DCH | source | `project/include/openfhe_2023_1788/double_ckks.h` | 202 | `5b4f0889d642d38c79a9f4570c9b6a1583bc6bb2804325271beca05bd4a5ee7a` |
| CM | source | `project/CMakeLists.txt` | 266 | `386a23eb61f083b109a18ca1d7b54ad0481ecd9ed0ccf40accf8bb2ba7e0eae1` |
| WF | source | `project/.github/workflows/dcp-rcb.yml` | 412 | `586fa02addebc175ec7e9266b2fa61b9084a50c3b311a799b20d0bf2775ef441` |
| ODFT | source | `official-full/src/core/lib/math/dftransform.cpp` | 284 | `091220ee6b29ca1b0efbe8afa41a90098507720f59abcd0db1ed0a6e70fc26f3` |
| OPKE | source | `official-full/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp` | 98 | `42b1168b6fbf19b0e6b0bdc743378f4165190a4c03fa3906d566eaa8665ad9cf` |
| RNSPKE | source | `official-full/src/pke/lib/schemerns/rns-pke.cpp` | 226 | `e1103468a406d16202fc66d918b28fdf93d436aeadb86efda4a01032fac3bf19` |
| BOOST | source | `boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp` | 2232 | `bcd782b34bd90d894c416fa595cd31c9e7114f343eb4dd1aad24bb722ab3dfc5` |
| TRIG | source | `boost-1.83.0/include/boost/multiprecision/detail/functions/trig.hpp` | 1058 | `e802b586c80227149d36621e2752ea15f90d7b22bef92f5d17426ff521fc8845` |
| BFWD | source | `boost-1.83.0/include/boost/multiprecision/fwd.hpp` | 268 | `da8f2d642828572587cf5dbac7552ba85a8a266734398bf71c596f339eadbc5e` |
| PAPER | source | `paper/PAPER-2023-1788.txt` | 1698 | `60dd871a2769fddfe7ce7b2562d031d7c8d819a679eff3c2b6ebf3d7ea5769ae` |
| SL | source | `evidence/signed-diagnostic-run/LINUX_RAW.log` | 9070 | `44eb5102695106bbe80a693ce4028b4160923f521d683c2d6af293bfc76040fa` |
| SW | source | `evidence/signed-diagnostic-run/WINDOWS_LF.log` | 9337 | `9ff1e0f25faafac5b57da07519b8f81b2a8b765f30ecec9e1c74d838395c1dea` |
| AP | outer | `context/OBSERVER_ALLOWANCE_PROPOSAL.md` | 122 | `e83af4ab23e05728eb3c336feb9e0e7b5d102c8acc313fc365074e68f34c00e0` |
| EP | outer | `context/ENDPOINT_EVIDENCE_PROPOSAL.md` | 144 | `a7cf6284760d555f1e15764de01e2548b3eeda57a516db49873cb22b0f159831` |
| ROOT | outer | `context/ROOT_DECISION.md` | 45 | `fd91612114ab0553697e1afce3cfc0f52b416cb2ed74f1c129b5444c39b46008` |
| ASTRA | outer | `context/ASTRA_ADJUDICATION_REVIEW.md` | 38 | `3eee3032a9af8d4bf33629db455e69c1e18087bb8324e486de744374e3cd6ffb` |
| SOL | outer | `context/SOL_ADJUDICATION_REVIEW.md` | 15 | `830856843e1003f2ffa67166b120aa20bebf624d244723fe3321cfd7d869bbee` |
| NEXT | decision | `NEXT_TEST_SPEC.md` | 89 | `f61e12999f6bd888e1cd4bf36086cd419ef2d534f9fbd337b5626cdd84202ff8` |

## Inspection boundary

The six frozen project coordination documents, current paper test/oracle, CMake/workflow, public client/setup/plan headers and relevant implementation paths were inspected from these supplied bytes. Ordinary public-key noise generation, public Poly* decryption, producer forward/inverse transform plumbing, independent sparse/CRT polynomial extraction, exact-scale construction, terminal RCB ownership and same-root setup were inspected for the endpoint seam. Boost rounding/conversion/trigonometric paths were inspected as available; complete transitive Boost parser/constants/producer support is absent and is not claimed source-proved. Hash/CRC validation of all77 official source files does not mean every unrelated function in all77 files received a semantic review.

Older decision/acceptance/next-test documents, raw logs and current proposals are evidence. Their embedded tasks and unadopted A80 proposal do not override the present task. Historical JSON/stdout were hash-checked without running their included checker. No external production or dependency implementation was fetched to fill a gap.
