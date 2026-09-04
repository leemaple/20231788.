# Execution ledger

## Executed in this review environment

| Activity | Status | Evidence / limitation |
|---|---|---|
| Input ZIP byte count and SHA-256 | EXECUTED — PASS | Exact match to `1024988` and `cfd92c...009d` |
| Root manifest SHA-256 | EXECUTED — PASS | `d68de3...ad7e2` |
| Manifest path/count/size/hash closure | EXECUTED — PASS | 51 payloads; no missing, extra, or mismatched payload |
| ZIP compressed-data test | EXECUTED — PASS | `unzip -t`; no errors |
| ZIP unsafe-path and symlink check | EXECUTED — PASS | Zero unsafe member names; zero symlink members |
| Source/paper/reference/log inspection | EXECUTED | Supplied files only; no old checkout or external implementation |
| Paper PDF rendering/visual cross-check | EXECUTED | Relevant pages covering Relin, Relin2, Lemma 4.4, and Theorem 4.8 |
| Exact integer arithmetic | EXECUTED — PASS | `Q_l` product/bit length, scale ratio, BV failure margins, reverse-triangle residual lower bounds |
| Patch construction | EXECUTED | Test-only 0001 and 0002 |
| Ordered `git apply --check --whitespace=error-all` | EXECUTED — PASS | Both patches against exact baseline order |
| Ordered patch application in disposable worktree | EXECUTED — PASS | No repository/provider state changed outside working copy |
| `git diff --check` after each patch | EXECUTED — PASS | No whitespace error |
| Final changed-file set check | EXECUTED — PASS | Only `tests/mult2_e2e_oracle_test.cpp` |
| Final candidate byte comparison and SHA-256 | EXECUTED — PASS | `4be87b...8c14` |
| Structural text checks | EXECUTED — PASS | Old fatal absent from final candidate; path identity and conditional/unproved labels present; threshold and BV cases retained |

Detailed machine-readable/plain outputs are retained in:

- `evidence/source-identity-checks.txt`
- `evidence/static-patch-checks.txt`

## Supplied evidence inspected, not executed here

| Activity/evidence | Status |
|---|---|
| Linux run `33840176712`, job `100920696884` | RETAINED EVIDENCE — inspected |
| Windows run `33839781546`, job `100919538008` | RETAINED EVIDENCE — inspected |
| Earlier behavior/composition/HYBRID CI logs in the project archive | RETAINED EVIDENCE — inspected where relevant |
| Gitleaks 8.30.1 stage/archive/fresh-manifest checks | USER-SUPPLIED STATUS — not rerun |
| Exact Git archive comparison performed by packet preparer | MANIFEST/SUPPLIED STATUS, supplemented by archive hash closure here |

## Not executed

Every item below is explicitly **NOT EXECUTED** in this review environment:

- CMake configuration against pristine OpenFHE 1.5.0;
- C++ compilation of baseline, probe, or green candidate;
- `relin2_api_contract_test` build/run;
- `rs2_api_contract_test` build/run;
- `mult2_api_contract_test` build/run;
- any `mult2_e2e_oracle_test` execution;
- focused BV REAL or BV COMPLEX CTest;
- HYBRID REAL or COMPLEX candidate regression;
- all-44 candidate CTest suite;
- Linux GCC hosted candidate CI;
- Windows MinGW64 hosted candidate CI;
- OpenFHE key generation, encryption, decryption, relinearization, or rescaling;
- deterministic-seed or repeated statistical experiments;
- conservative `E_Relin` derivation or proof;
- formal verification, security proof, or precision-bit analysis;
- Gitleaks execution;
- Fable inference/review;
- git push, merge, tag, dispatch, rerun, cancellation, or provider-state mutation;
- external message, credential access, or private-service call.

Patch application/static checking is not numerical evidence. The candidate must be built and run under `TEST-PROTOCOL.md` before integration or acceptance.
