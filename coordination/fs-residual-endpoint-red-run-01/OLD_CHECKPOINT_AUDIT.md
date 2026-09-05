# Independent old-checkpoint audit — run 33991083281

## Result

`PASS_OLD_CHECKPOINT_EVIDENCE_BOTH_HOSTS`. No discrepancy was found in the reached old checkpoints.

This audit is deliberately limited to the pre-paper legacy/API/accepted-contract evidence. It does not adjudicate the new paper link failure, the absence of the new self-test runtime, or any scientific claim.

The authority is tested source `2fe655d493dcde5f05aa1515f41ca6823bba30bd`, tree `3102379bdc2c6fc25402809fd4d4db08971c71b2`, not current documentation HEAD. The verifier binds:

- `CMakeLists.txt`: Git blob `82fc3cc581342d677f8b4cac157db8eee498913f`, 15,938 bytes, SHA-256 `386a23eb61f083b109a18ca1d7b54ad0481ecd9ed0ccf40accf8bb2ba7e0eae1`.
- `.github/workflows/dcp-rcb.yml`: Git blob `20a663e947715e38a5c25b77b4c7b6b03be23b80`, 17,161 bytes, SHA-256 `586fa02addebc175ec7e9266b2fa61b9084a50c3b311a799b20d0bf2775ef441`.
- Official OpenFHE pin `df495ba2e91739a6dc8f1de254fc5a41155ce504` in tested workflow, exact fetched ref, abbreviated checkout completion, and each job's environment.

The workflow selectors independently derive six reached groups `[1, 2, 57, 1, 2, 60]`. On each host, all 123 invocations have a one-to-one ordered `Start -> Test command -> Passed` binding to the tested CMake name/number/argv. These are repeated invocations of exactly 60 unique old tests, not 123 unique tests. The two reached live JSON inventories are exactly 57 then 60 names, in CMake order, with matching argv and `add_test` file/line backtraces.

All five explicit API targets completed on both hosts: `relin2_api_contract_test`, `rs2_api_contract_test`, `mult2_api_contract_test`, `add_api_contract_test`, and `sub_api_contract_test`. The other four old explicit targets, all 12 default executable targets, and `openfhe_2023_1788` library completion were also observed. Terminal metadata independently marks every corresponding old build/run step successful.

## Retained bytes and execution

- Linux LF log: 460,571 bytes, SHA-256 `c2fe6cc6167d1648798cbc0d2a8f416369897f6d1458211cc0b447c70fa06b6e`; capture content is byte-identical, UTF-8 BOM, 0 CRLF.
- Windows LF log: 471,229 bytes, SHA-256 `3317309b60eacdee6175dceee7a62ad3c85eab4b76df4d79965b59aa01390764`; original decoded capture is 477,042 bytes, SHA-256 `b49549088d34397f24dc9350b12f88c9b531a1bf6d65994bf5138f3e5f05733d`, UTF-8 BOM and exactly 5,813 CRLF pairs. CRLF-to-LF is the only transform.

Command actually run (exit 0):

```text
PYTHONDONTWRITEBYTECODE=1 python3 artifacts/handoffs/fs-residual-endpoint-red-run-01/verify_old_checkpoint.py
```

Stdout: `PASS old checkpoints: hosts=2 unique=60 invocations=123 groups=[1, 2, 57, 1, 2, 60] ...`

Verifier: 22,492 bytes, SHA-256 `066af22503d288212624b9168015892d7f38cdc8c71d107f45f026af77218273`. JSON: 183,611 bytes, SHA-256 `ded5d33cb69c76107b857a776bfcc04bd608b592d1f709dfc13a349de80525fe`; it retains all 246 actual argv bindings plus line evidence.

An initial authoring run stopped before output because the parser counted six selectors per host and correctly encountered the seventh, unreached paper selector. It was corrected to validate all seven while restricting old groups to the first six; the successful result above is the only emitted JSON.

NOT RUN: C++ compilation, CMake, OpenFHE/FHE, FFT/NTT, crypto, CI, browser, or network. Requested Sol/high is a selector only; backend identity is unattested.
