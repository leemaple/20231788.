# Relin2 Codex fallback HYBRID entry-format TDD closure receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **the HYBRID evaluation-key entry-format guard is green on Linux and
Windows; BV key shape and all Relin2 arithmetic remain unimplemented**.

## Red boundary

- source branch/commit/tree: `agent/codex-relin2-01` /
  `b1f4459d9e1d3009da5420954f26384b96ba3e57` /
  `30b926d2c06f39238310a714d94fbb70716247c4`;
- run/jobs: `33581151491` / Linux `100095528356` / Windows
  `100095528194`;
- result: warning-clean project and Relin2 API-contract builds passed on both
  platforms; exactly the new test failed, producing `17/18` on each;
- evidence branch/commit/tree: `evidence/relin2-hosted-b1f4459` /
  `76f9c5eea659c19f0d7211f8c3f04430a6591d1d` /
  `c2fb81757ce24322ac905b6fb86c097901cf72bb`;
- 19-entry manifest SHA-256:
  `cc4091acd00209d3e02786fa3bd45e88fe29af7f29fa5aa9013a6e346a9c0219`.

The eighteenth fixture generated a real public two-partition HYBRID
`EvalKeyRelinImpl`. Its positive control proved every A/B aggregate and tower
started in Evaluation format on the complete ordered `ParamsQP` basis and
reached the old scaffold. Its negative control changed only the first A entry
to Coefficient format through public `SetFormat`; both platforms then observed
the old scaffold exception instead of the required exact project diagnostic.

## Green boundary

- source commit/parent/tree: `b9f26db29b53764930798340f4ebe9bed789a323` /
  `b1f4459d9e1d3009da5420954f26384b96ba3e57` /
  `7a05d020200ce147241b96f86523b5097339ec0d`;
- run/jobs: `33582263190` / Linux `100098827445` / Windows
  `100098827360`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33582263190`;
- result: warning-clean project build, Relin2 API-contract build, and exactly
  `18/18` runtime tests passed on both platforms in 0.18/0.42 seconds;
- evidence branch/commit/tree: `evidence/relin2-hosted-b9f26db` /
  `8b2a7dcb2157e8cdd72e0821fdc58553a42e8f42` /
  `a3752ed65ef72deb2d630e9f4349e66fd2ed61bd`;
- 19-entry manifest SHA-256:
  `b1961d4e60623686e957f94a9fbb5a1d51774c6a64d1981e477969d4be136de3`.

Only `src/double_ckks.cpp` changed from red. The 22-line minimal addition
keeps length and complete ordered basis validation first, then checks all A
entries followed by all B entries. It rejects either a DCRTPoly aggregate or
any NativePoly tower outside Evaluation format with exactly
`DoubleCKKS: Relin2 evaluation key HYBRID entry must be in evaluation format`.
Tests, CMake, public headers, BV validation, ciphertext raising, key switching,
relinearization, arithmetic, metadata, and the old scaffold are unchanged.

Three read-only source reviews and three frozen evidence reviews returned
`PASS`. A pre-commit static check caught and removed the unsupported
`lbcrypto::Format` spelling before the final reviewed source hash and before
the commit. Both red and green evidence sets passed ZIP integrity, Gitleaks
8.30.1, independent targeted scans, manifests, source identity, and remote-ref
verification. The green run executed all positive/negative Tensor,
deep-metadata, A/B polynomial, cache-map/vector/key-pointer, context, tag, and
RAII restoration postchecks.

The next isolated TDD boundary is BV-specific evaluation-key shape. It must be
introduced red-first and must not start Relin2 ciphertext raising or arithmetic
in the same commit.
