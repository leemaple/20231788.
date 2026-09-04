# RS2 collaboration and integration receipt

Recorded: 2026-09-04, Asia/Shanghai. This records observations, not final acceptance.

## ChatGPT Pro

- Conversation: [完成 RS2 工程审计](https://chatgpt.com/c/6a99fcad-7c08-83ec-98f6-8230f6277b8c).
- Ego task space: `122`; model selector showed `Pro`. Submitted once and inspected read-only until `Stop answering` disappeared and the final answer was visible.
- Input source commit: `7629f446517413a3ae65551e7efe51b74fd70f00`.
- Input packet: `/private/tmp/rs2-chatgpt-pro.K1EpRJ/chatgpt-pro-rs2-7629f44.zip`, 150,501 bytes, SHA-256 `e4c04228ac8b3fadc4f4ffd9f7049ebd55dcaf0839647c5ba250772a4958a1e7`.
- Input task SHA-256: `5ff87f284fd30132011b1f049e154117797c3909716dc033efefad2d26cfcdff`; input manifest SHA-256: `846639cef978a0893f6e8ae32f30782546ee23698e0dff2aee8158c8580660ac`.
- Previous handoff preparation retained stage/archive/fresh Gitleaks checks and targeted exclusion checks, all with zero findings. The paper, exact selected clean-room source, and official references were supplied; no old implementation or credentials were supplied.
- Returned verdict: `READY_FOR_CODEX_INTEGRATION`, explicitly a static candidate, not a build/test/CI pass.
- Return ZIP was fetched through the authenticated, observed browser download request and verified in memory: 51,922 bytes, SHA-256 `d0a2dc6f8a0fc1c998af55354ab62b28245a56f0b37bcf1a60b4419449107e65`. A ZIP saved in the local download directory was not observed and is not claimed.
- Selected exact text artifacts, final response, and complete return-entry hash manifest are retained under `coordination/returns/rs2-pro-7629f44/`.
- G2 patch SHA-256: `1558f5ebfabf0d35a2709cfe3ca9e27d1cbc877af405c24320218c72e0893161`.

## Fable 5.1 fallback consultation

- Input source commit: `75cd77f57890eebfb49b9cc30e61b1a666bdd9f3`.
- Input packet: `/private/tmp/rs2-fable51-design.Ha3cXO/rs2-fable51-design-75cd77f.zip`, 126,768 bytes, SHA-256 `4562a51112fe3f2a8a4e3f1fa36217c567cf4f607e5e3ac33a80e0050cc9f35c`.
- Terminal-only invocation selected `claude-fable-5-1`, with no fallback, Read-only tool access, no MCP servers/plugins, and no session persistence.
- Emitted session: `c18231c6-ecae-46b4-8f93-adbcf0a70bec`; terminal result confirms canonical model `claude-fable-5-1`.
- Result is explicitly `is_error: true`: output exceeded the 64,000-token maximum. The partial response contains an `ACCEPT_RS2_DESIGN` conclusion and a useful implementation sketch, but is truncated. This is **not** a completed review or final acceptance.
- Partial visible response and machine terminal result are retained under `coordination/returns/rs2-fable51-75cd77f/`. The long process was not stopped, restarted, or resubmitted.

## Codex reconciliation and selective integration

1. Confirmed the paper's two centered divisions and non-linear low correction. Both external responses agree on public Rescale plus exact native multiplication/direct DCRT subtraction; no ciphertext EvalSub or production try/catch is introduced.
2. Independently inspected the pinned official `rns-cryptoparameters.h`: under FIXEDMANUAL, `GetModReduceFactor` returns `m_approxSF` (2^p), not the native q_l prime. Logical scales divide by native q_l; the recorded factor follows the official double-valued getter. The test explicitly distinguishes these values.
3. Pro's packet did not include the output-returning scheme ModReduce wrapper or native SwitchModulus/precomputation definitions. Its candidate clones the high path before Rescale; exact rounding is to be established by the independent CRT runtime test, not assumed from the model answer.
4. Existing Codex R1/G1/R2 commits are retained, not replaced by Pro's alternative test series. Real R2 red is commit `01d131714cd384733e00c513e299eda6dc6a4bdd`, Actions run `33817976696`: both warning builds passed; only test 39 failed with `DoubleCKKS: RS2 is not implemented`; the other 38 tests passed.
5. Only Pro's G2 source patch was applied. Its expected parent source blob `21d82dfb4948250ad0ee2e27fbaf28e61eff6571` exactly matched the live source. Read-only `git apply --check` succeeded before application; the resulting source blob is exactly Pro's `6a870126445face795cdf9078ec7dd3058d3ba6c`.
6. Gitleaks 8.30.1 scanned both retained return directories with `--redact --exit-code 1`; both exited zero with no findings. `git diff --check` passed. These are static checks only.

## Remaining gates

- G2 commit `ed00f3518d65223a482e1e9db54111eb24573f2c` passed both Linux/GCC and Windows/MinGW64 warning builds and all 39 tests in Actions run `33831920036`; complete CTest result sections are retained in `artifacts/tdd/rs2-valid-arithmetic/green.txt`. The remote branch matched that exact commit.
- Add/verify remaining public-pipeline, mutation, fail-fast terminal-lifecycle, and deeper immutability coverage. Pro's full alternative R2 test has not been applied or executed.
- Obtain/reconcile exact-current review after hardening. The truncated Fable result is not treated as full review closure; shared ZCode quota must be rechecked before any new ZCode dispatch.
- Mult2, required pair Add/Sub, end-to-end accuracy/theorem evidence, and final integration remain separate unfinished requirements.

## Additional return retrieval

On 2026-09-04, retrieved Pro's already-completed ZIP again through its observed authenticated browser download, without resubmitting a message or changing the response. The same 51,922-byte ZIP hash was verified. The exact alternative test file is now retained at `coordination/returns/rs2-pro-7629f44/final-tree/tests/rs2_test.cpp` (47,161 bytes; SHA-256 `6d7ed9d97851e84e393a2d5e328dc30b2f34bf687288cae3b01ccf53de93d117`). Gitleaks scanned this retained file with zero findings. It is a review input, not compiled production-test coverage. Its polynomial snapshots retain pointer identities but still need independent parameter-value snapshots to prove pointed-to parameter immutability.
