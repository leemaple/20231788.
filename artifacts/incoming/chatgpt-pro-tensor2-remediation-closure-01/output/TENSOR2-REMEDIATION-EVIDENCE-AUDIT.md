# Tensor2 remediation evidence audit

Reviewed scope: exact current clean-room source/test head `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.

This document separates retained provider evidence from local reviewer execution. GitHub Actions files in the package are inspected **remote evidence**; they are not local executions by this reviewer.

## 1. Raw intermediate P2 evidence identity

The three intermediate evidence directories contain exactly four raw files each: `run.json`, `jobs.json`, `linux-gcc.log`, and `windows-mingw64.log`.

All 12 SHA-256 values were independently recomputed and match `cleanroom-project/artifacts/tdd/tensor2/hosted/README.md`:

```text
383a723b036d4c42eba114ae2ecd9a68629521bcc89abd87952eff0fb3624408  33425868973/jobs.json
16075c494e1ab55f92b2b5039e41317edbee41efeba982cf73cde5d2f1c521f0  33425868973/linux-gcc.log
d68678cf9a0ace72d8fc72aefd948f2d2d0650e8b0f644302c2a127b34aca016  33425868973/run.json
97b021edbdf0a79d1ebf8c5c66d38bf0463ac183a8102980505fec680904e125  33425868973/windows-mingw64.log
ad3838350f4c0edaeb9ab36acbe249269bbd726a07deca27d5094f147b451096  33426712752/jobs.json
2aab913ab00ca5ff696cdd401bdd74737f32646dfbc7ccde2c6f1f337eee2eaa  33426712752/linux-gcc.log
0ddffa00b2202a7abe99fb1ede5ae763cb9b89cb5b57b57e816dd9c7367b3934  33426712752/run.json
8a217c3b5f6bc5523455eb79b8bb902c6de91c0a6aa5c1f8c08003477802afca  33426712752/windows-mingw64.log
da09b2d882b9010d32a9325c97e14f6a58273dea1365c661d391299ed60fa14f  33427271692/jobs.json
30a4472946941fcc342dda2242aee29397f47a6a5fd5d5a138a02dcf1d1b3c29  33427271692/linux-gcc.log
5ff25644275ddded22eb6153e5918d19885fb8bb866717cafb143c2baa133042  33427271692/run.json
aab5594a9d6bc72715d6ce566f2ce88fbd19e4d3683b5df14d5a9232a778ff54  33427271692/windows-mingw64.log
```

The 12 raw files total exactly `637,139` bytes, matching the retained mapping note.

### Run `33425868973` - API compile red

Provider records:

- attempt: 1;
- event: `push`;
- branch: `agent/codex-tensor2-01`;
- exact `head_sha`: `f3db12ef9fb0d13df0f779157eed168b8d582ea4`;
- overall: `completed / cancelled`;
- Linux job `99599155126`: `completed / failure` on `ubuntu-24.04`;
- Windows job `99599155483`: `completed / cancelled` on `windows-2022`.

Linux log facts:

- pristine OpenFHE checkout/provenance is bound to `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- project configuration succeeds;
- the production `openfhe_2023_1788` library target builds before the public API contract target fails;
- `tensor2_api_contract_test.cpp` fails because `TensorCiphertextPair` and `TensorScaleDescriptor` do not exist and `DoubleCKKS::Tensor2` is not a member;
- build exits nonzero; CTest is skipped.

Windows boundary:

- checkout/toolchain/OpenFHE provenance steps start successfully;
- the Windows job is cancelled while building OpenFHE;
- project configure/build/CTest steps are skipped;
- therefore **no Windows project build or test result is claimed**.

Disposition: this is the intended public API compile red.

### Run `33426712752` - complete runtime red

Provider records:

- attempt: 1;
- event: `push`;
- exact `head_sha`: `482d27d0c43c22779aa548e00955ed90175dee97`;
- overall: `completed / cancelled`;
- Linux job `99601946465`: `completed / failure`;
- Windows job `99601946625`: `completed / cancelled`.

Linux build succeeds under the strict warning settings. CTest executes all six entries independently:

```text
dcp_rcb                                      PASS
tensor2_valid_arithmetic_immutability        FAIL - logic_error scaffold
tensor2_result_scale_contract                FAIL - logic_error scaffold
tensor2_right_input_validation               FAIL - wrong exception type: logic_error
tensor2_mutual_compatibility                  FAIL - wrong exception type: logic_error
tensor2_prearithmetic_key_compatibility       FAIL - wrong exception type: logic_error
```

The log reports `17% tests passed, 5 tests failed out of 6`. Each Tensor2 case runs and reports its own failure; no first failure masks another case.

Windows is cancelled during OpenFHE build. Project configure/build/CTest are skipped, so **no Windows project result is claimed**.

Disposition: this is the intended complete fail-before-access scaffold runtime red.

### Run `33427271692` - first implementation green

Provider records:

- attempt: 1;
- event: `push`;
- exact `head_sha`: `1408d46217e97a1c14d43d49b64791da22f652da`;
- overall: `completed / cancelled` because the intermediate Windows job is cancelled;
- Linux job `99603779665`: `completed / success`;
- Windows job `99603779368`: `completed / cancelled`.

Linux strict build succeeds and CTest reports 6/6 passed, including all five Tensor2 cases.

Windows is cancelled during OpenFHE build; project configure/build/CTest remain skipped. No Windows result is inferred.

Disposition: intended first Linux implementation green is established; it is not misrepresented as cross-platform final evidence.

## 2. P3 public-seam diagnostic red

Raw evidence hashes independently match `CI_EVIDENCE.md`:

```text
a95452c531538488f5588e5868f96755ef88e8d1e74367ffd4d3938c715a7330  tdd/p3-diagnostic-red/run.json
0f3d58b38a9339396644b63b0259c949a2593dc6b1c97e42738a7ac8f04ca9a5  tdd/p3-diagnostic-red/jobs.json
ba3fbbeaa33a073d110ab2c3c597c687f694138cb6510b09ba5fe226c2a4a55f  tdd/p3-diagnostic-red/linux-gcc.log
9a0dfb389ee9b8051b00c329e2253d83e600e91e563380f7b0c94b855218e06c  tdd/p3-diagnostic-red/windows-mingw64.log
```

### Exported change sequence

`REMEDIATION_COMMIT_HISTORY.txt` records:

1. `c418b456...` - retain run/jobs JSON;
2. `315a9c55...` - retain intermediate job logs;
3. `9d1d10a3...` - add the DCP empty-key-tag public regression test;
4. `fb862a3d...` - change the production DCP state label from `ciphertext` to `pair`.

The test-only commit records commit time `2026-09-01 04:26:40 +0800`, equivalent to `2026-08-31T20:26:40Z`.

Run `33436068864` is created at `2026-08-31T20:26:45Z`, five seconds later, and its raw run/jobs records bind exact `head_sha` `9d1d10a3414dce68b84d9887337254c275098d79`.

### P3-red Linux outcome

Linux job `99632689793`:

- `completed / failure`;
- OpenFHE provenance step succeeds;
- CMake `3.31.6`, GCC `13.3.0`;
- strict project build succeeds;
- CTest executes all six tests;
- only `dcp_rcb` fails;
- actual diagnostic is:

```text
DoubleCKKS: DCP input key tag does not match its ciphertext state
```

while the new public-seam regression requires `pair state`;
- the five Tensor2 CTests all pass;
- result: 5/6 passed.

Windows job `99632689495` is `completed / cancelled`; relevant later steps are skipped. It supplies no Windows build/test result.

### Production fix timing

`REMEDIATION_COMMIT_HISTORY.txt` records the production fix commit `fb862a3...` at `2026-09-01 04:28:45 +0800`, equivalent to `2026-08-31T20:28:45Z`.

Final run `33436252725` is created at `2026-08-31T20:28:49Z`, four seconds later. Thus the retained provider timestamps plus the exported commit sequence support a public-seam red before the production fix.

Because `.git` is absent, this does **not** independently prove commit-object ancestry or absence of history rewriting. That remains unverified by design.

## 3. Exact-current final green

Raw evidence hashes independently match `CI_EVIDENCE.md`:

```text
d283cf83067bd969b2bcc23334c4a9274be989de789bfa2efc0550bfaaee8743  ci/run.json
4bd699605dc681c2d2d2693552c973dd6c349ec10a5bf0a904c85f2f004ac425  ci/jobs.json
26be856772c3c4a481aeab23d131858f006052b97400e50faf75cb2bead2d8cd  ci/linux-gcc.log
b3e0d6535ec5328d28a6bcefbe2b72aaa0ae872127622a39e7fe5d7aab471707  ci/windows-mingw64.log
```

`ci/run.json`:

- run `33436252725`;
- attempt 1;
- event `push`;
- branch `agent/codex-tensor2-01`;
- `head_sha = fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`;
- `completed / success`.

### Linux job `99633299988`

- runner label: `ubuntu-24.04`;
- `completed / success`;
- exact project checkout log shows `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`;
- OpenFHE checkout/provenance verifies `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- CMake `3.31.6`;
- C++ compiler `Ubuntu GCC 13.3.0`;
- pristine OpenFHE install is restored from a cache key explicitly bound to the exact OpenFHE commit;
- project configure succeeds;
- warning-clean project build succeeds;
- CTest: 6/6 passed.

### Windows job `99633300315`

- runner label: `windows-2022`;
- environment: MSYS2 `MINGW64`;
- `completed / success`;
- detached project checkout is exactly `fb862a3...`;
- detached pristine OpenFHE checkout is exactly `df495ba...`;
- CMake `4.4.2`;
- GCC `16.2.0`;
- OpenFHE configure/build/install succeeds;
- project configure/build succeeds;
- CTest: 6/6 passed.

Disposition: the required exact-current cross-platform final gate is satisfied by retained provider evidence.

## 4. Current-tree/diff consistency checks performed locally

These are local byte-tree checks, not hosted execution:

- temporary Git indexing of the supplied `cleanroom-project/` recomputed tree `759d5195739684748d5a9664edabe3fa719e1acf` exactly;
- `git apply --reverse --check REMEDIATION_DIFF.patch` passed against current source;
- reverse-applying that remediation diff and recomputing the tree produced exactly the previously reviewed tree `2269bee6bac5e7cd1124ab78c49a750af9a38942`;
- `git apply --reverse --check PROJECT_DIFF.patch` also passed against the exact current tree.

These checks prove the supplied byte-tree/diff relationship. They do not prove the original Git commit graph.

## 5. P2/P3 closure summary

| Item | Previous issue | Current evidence | Disposition |
|---|---|---|---|
| P2 | Missing raw intermediate provider records | 12 raw files, all hashes verified; heads/outcomes/cancellations match | **CLOSED** |
| P3 | DCP empty-key-tag diagnostic drifted to `ciphertext state` | Public regression red at `9d1d10a...`; one-label source fix; final 6/6 Linux+Windows | **CLOSED** |

No residual evidence blocker was found.
