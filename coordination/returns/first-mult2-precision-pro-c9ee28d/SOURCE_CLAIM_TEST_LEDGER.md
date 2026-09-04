# Source / claim / test ledger

Status vocabulary: **OBSERVED** = supplied retained runtime or byte evidence; **STATICALLY VERIFIED** = checked in this isolated workspace without OpenFHE execution; **INFERRED** = mathematical/source inference; **PENDING HOSTED** = requires Linux/Windows OpenFHE execution; **OUT OF SCOPE** = deliberately not claimed.

| Claim | Primary source or evidence | Candidate assertion / record | Status |
|---|---|---|---|
| Exact input archive and selected 40-file snapshot | `SOURCE-MANIFEST.json`, `SOURCE-PROVENANCE.json` | `evidence/INPUT_INTEGRITY.txt` | OBSERVED locally |
| Accepted precursor runtime red was fixture loss, not a DCP/RCB defect | `red-linux.txt:L108-L122,L540-L545`; `red-windows.txt:L132-L139,L562-L567`; runtime contract claim boundary | binary64 negative control in new test `L662-L694` | OBSERVED precursor; control STATICALLY VERIFIED |
| Pre-red Windows event was a namespace compile failure | `compile-failure-windows.txt:L53-L70` | provenance discussion in `PRECURSOR_REVIEW.md` | OBSERVED |
| Accepted precursor passed 54/54 on both hosts | `green-linux.txt:L109-L119,L541-L543`; `green-windows.txt:L132-L142,L564-L566` | no alteration of accepted contract/fixture | OBSERVED |
| Test-only DCRT injection uses public interfaces | `plaintext.h:L460-L471`; `cryptocontext.h:L1250-L1263`; fixture `L178-L200,L238-L263` | source guard forbids stale-cache reads and production decrypt | OBSERVED source; STATICALLY VERIFIED guard |
| Native64 degree-two metadata does not create a fresh 100-bit embedding | `ckkspackedencoding.cpp:L190-L205,L279-L309,L331-L333` | candidate never uses standard binary64 encoding as high-precision input | OBSERVED source / design constraint |
| Native128 retains double-source precision rather than creating 100-bit slots | `ckkspackedencoding.cpp:L134-L145` | no Native128 precision claim | OBSERVED source / OUT OF SCOPE |
| Canonical slot order uses powers of five | `dftransform.cpp:L50-L69`; inverse ordering `L209-L238` | explicit exponents and constant/X^32/X^2 witnesses, new test `L395-L490` | STATICALLY VERIFIED; runtime self-check pending target execution |
| Pair recombination is `q_div·high+low` | production RCB `double_ckks.cpp:L1119-L1129` | independent recombination `L315-L344`; public RCB equality `L1106-L1119` | Source OBSERVED; PENDING HOSTED |
| Production first multiplication is `RS2∘Relin2∘Tensor2` | paper `L895-L900`; production `double_ckks.cpp:L1115-L1117` | direct/staged exact wiring check `L1049-L1067` | Source OBSERVED; PENDING HOSTED |
| Tensor2 omits low-low and combines high-high/cross terms | `double_ckks.cpp:L763-L812` | independently decoded input product and output slot comparison | Source OBSERVED; PENDING HOSTED |
| RS2 drops actual `q_l`, independently rescales high/recombined, and corrects low | `double_ckks.cpp:L991-L1112` | output prefix basis, level 2, degree 2, `RefreshRequired`, exact denominator | Source OBSERVED; PENDING HOSTED |
| Exact final logical scale is `2^200/(q_div·q_l)` | paper `L949-L958`; exact integer basis at runtime | `cpp_int` numerator/denominator and direct evaluation `L1009-L1013,L1075-L1080,L1121-L1123` | INFERRED from algorithm; PENDING HOSTED numeric observation |
| Recorded FIXEDMANUAL scale is not exact normalization | `rns-cryptoparameters.h:L601-L649`; production metadata `double_ckks.cpp:L784-L809,L1006-L1029` | recorded-state checks separated from exact rational evaluator | STATICALLY VERIFIED design; PENDING HOSTED |
| Frozen expected products are independent of production and double | `evidence/FROZEN_CONTRACT.json`; new test `L492-L638` | literal table plus `1e-75` host cross-check | STATICALLY VERIFIED; target self-check pending |
| All-slot and delta precision target is `2^-80` | frozen contract hash `7d6b…036b`; derivation in test design | new test `L1009,L1121-L1139` | FROZEN; PENDING HOSTED |
| Four keys are fresh and crypto RNG is not seeded | new test `L1015-L1021`; no seed calls in source | four loop iterations and source guard | STATICALLY VERIFIED; actual samples PENDING HOSTED |
| Current 54 registrations are preserved exactly and one test is added | baseline/candidate CMake parse | `CURRENT_54_CTEST_BINDINGS.tsv`, `CANDIDATE_55_CTEST_BINDINGS.tsv`, `CTEST_CONTINUITY.txt` | STATICALLY VERIFIED |
| Patch applies to exact `c9ee28d…` selected snapshot | supplied 40-file provenance; patch replay | `PATCH_APPLY_AND_FINAL_EQUALITY.txt` | STATICALLY VERIFIED |
| Candidate compiles warning-clean on pristine OpenFHE | none in this environment | hosted GCC/MinGW build | PENDING HOSTED |
| Candidate passes first Mult2 precision | none yet | `precision_first_mult2_high_precision_contract` | PENDING HOSTED |
| 128-bit headroom proves universal non-wrap | no such theorem/certificate | actual-centered representative checks only | NOT CLAIMED |
| Current diagnostic has 128-bit security | `HEStd_NotSet` | none | OUT OF SCOPE / NOT CLAIMED |
| Candidate reproduces Table 3 or eight squarings | paper `L1562-L1590` differs materially from candidate | later gates only | OUT OF SCOPE / NOT CLAIMED |
| BV universal theorem gate is closed | supplied project explicitly leaves it unproved | no BV file touched | OUT OF SCOPE / NOT CLAIMED |
