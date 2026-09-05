# Repeated Mult2 pre-GREEN guard 01 — additive review correction

## Status and scope

This is a cross-track, source-only guard for the next Repeated Mult2 continuation. It does not modify the retained Repeated candidate, its RED, production code, tests, workflow, or CI state. No C++ compilation, OpenFHE execution, cryptographic run, dispatch, rerun, GREEN application, commit, or push was performed for this finding.

Classification: **P1 / hard GREEN-acceptance blocker**, not a project-feasibility blocker. At the exact OpenFHE pin, the returned GREEN production source invokes CKKS `CCParams` functions whose bodies unconditionally throw. This is a conclusion implied by authenticated source bytes; it is not a claim that the failure has already been run or observed on either host.

## Exact candidate identity

- Candidate ZIP: `/Users/lifeng/Downloads/repeated-mult2-semantic-candidate-80d771c.zip`
- ZIP size: `134664` bytes
- ZIP SHA-256: `77d32a3d28b528722efa59633feb7225cb813e68092023fbd462f0d4d318fec5`
- Implementation base: `80d771c52df10bce1c60992b5e0edb4e64f145ca`
- Preserved GREEN patch: `patches/0002-green-repeated-mult2-semantic-two-square.patch`
- GREEN patch SHA-256: `0c9f118fb1034dbdcd3cb01f8da802595c5e0f0eb54f003622eb6d5514b87a27`
- Returned complete production file: `complete/project/src/repeated_mult2.cpp`
- Complete-file SHA-256: `e4360763ee8e8d000dcf663c48123d0c14739518b2b3deb764d011e024d223d7`

The immutable expanded return is retained under `coordination/returns/repeated-mult2-semantic-candidate-80d771c/` in the Repeated Mult2 worktree. The hashes above were independently re-read from the actual ZIP and preserved bytes; this document does not rewrite either artifact.

## Confirmed finding in the returned GREEN

In `complete/project/src/repeated_mult2.cpp`:

- line 265 constructs `CCParams<CryptoContextCKKSRNS>`;
- line 272 calls the allowed `SetPREMode(NOT_SET)` and then the disabled `SetEncryptionTechnique(STANDARD)`;
- line 273 calls the disabled `SetMultiplicationTechnique(HPS)` and `SetMultipartyMode(FIXED_NOISE_MULTIPARTY)`;
- line 275 calls the disabled `SetThresholdNumOfParties(1)` after two allowed setters;
- line 278 is the later `GenCryptoContext(seedParameters)` call.

If `CreateRepeatedMult2DiagnosticSetup()` reaches line 272, `SetEncryptionTechnique` throws synchronously before line 278. The other three disabled calls are not reached in that current order, but each has the same unconditional throwing body if reached. Deleting only the first two technique setters is insufficient: `SetMultipartyMode` and then `SetThresholdNumOfParties` would remain unconditional failure points.

## Exact OpenFHE 1.5.0 source chain

Official source pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

- `src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h:54-57` defines the CKKS specialization and constructs its base as `Params(CKKSRNS_SCHEME)`.
- The same file at lines 78-80, 82-84, 90-92, and 94-96 overrides the four calls above; every body is exactly `DISABLED_FOR_CKKSRNS`.
- `src/pke/include/scheme/gen-cryptocontext-params.h:199` expands that macro to `OPENFHE_THROW("This function is not available for CKKSRNS.")`.
- `src/core/include/utils/exception.h:163` expands `OPENFHE_THROW` to an unconditional throw of `lbcrypto::OpenFHEException`.

The requested values need no high-level setter. They are already the exact CKKS scheme defaults:

- `src/pke/include/scheme/gen-cryptocontext-params-defaults.h:71`: `STANDARD` encryption;
- line 72: `HPS` multiplication;
- line 75: `FIXED_NOISE_MULTIPARTY`;
- line 82: threshold party count `1`.

`src/pke/include/scheme/gen-cryptocontext-params.h:204-205` calls `SetToDefaults` from the base constructor. `src/pke/lib/scheme/gen-cryptocontext-params-impl.cpp:46-82` assigns all scheme defaults, and lines 83-87 select the CKKS defaults. Finally, `src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h:98-122` passes the resulting getters into the low-level CKKS crypto-parameter constructor.

## Minimal production-only correction after genuine RED

Only after both hosted jobs have preserved the genuine missing-header RED, derive corrected GREEN production bytes that remove these four calls from `CreateRepeatedMult2DiagnosticSetup()`:

1. `SetEncryptionTechnique(STANDARD)`;
2. `SetMultiplicationTechnique(HPS)`;
3. `SetMultipartyMode(FIXED_NOISE_MULTIPARTY)`;
4. `SetThresholdNumOfParties(1)`.

Do not remove other allowed seed setters. In particular, retain `SetPREMode(NOT_SET)` and the explicit `SetCKKSDataType(COMPLEX)`; CKKS's default data type is `REAL` at `gen-cryptocontext-params-defaults.h:86`, so that explicit override remains material.

Do not remove or weaken the existing fail-closed production validation. Preserve `ValidateProfile` in `repeated_mult2.cpp:74-116`, including its actual returned-parameter checks at lines 91-101 for `STANDARD`, `HPS`, `NOT_SET`, `COMPLEX`, `FIXED_NOISE_MULTIPARTY`, and threshold `1`, and preserve its call at line 169. Those checks verify the values actually installed by context construction instead of trying to set disabled high-level fields.

The separate low-level uses are legal and must remain:

- returned file lines 140-143 construct `CryptoParametersCKKSRNS` with explicit `STANDARD`, `HPS`, `FIXED_NOISE_MULTIPARTY`, threshold `1`, and `COMPLEX`;
- lines 146-147 call `PrecomputeCRTTables(..., STANDARD, HPS, ...)`;
- official `src/pke/include/scheme/ckksrns/ckksrns-cryptoparameters.h:70-88` exposes those constructor parameters;
- the same header at lines 92-94 exposes the precomputation arguments, and `src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp:46-49` forwards them normally.

This distinction is mandatory: the four scheme-specific high-level CKKS `CCParams` setters are disabled, while the low-level `CryptoParametersCKKSRNS` constructor and `PrecomputeCRTTables` inputs are supported at the exact pin.

## RED and evidence preservation gate

The original RED patch SHA-256 is `4fdcdeb9aa05641fbac8f7187127e4c350ad320c38cfe8eb289e71b5b6f7bc62`; the retained `verification/RED_FREEZE.json` SHA-256 is `ff58f135ad80bd0a00a9317b4280f020b3b15f1590b45282413046761727afc6`. Do not change the frozen RED tests, their exact vectors/oracle, CMake observation adapter, or workflow to accommodate this GREEN defect. Do not edit the original returned ZIP or patches in place; any corrected GREEN and regenerated integrity evidence must be a new derived artifact after the genuine RED gate closes.

At the time this guard was written, the existing record said the expected failure had not yet been observed. Therefore the next continuation must first obtain and preserve the already-dispatched dual-host genuine RED. It must not apply the correction, dispatch another run, or rerun the existing run merely because this guard exists.

## Correction to the earlier static review record

`CODEX_PRE_HOSTED_REVIEW_01.md` remains an immutable historical review record. Its earlier statement that no hard Standards P0/P1 finding had been established omitted this scheme-specialization setter check. This additive guard corrects only that omission; it does not erase the earlier review, revive its withdrawn concurrency hypotheses, or convert static/package checks into runtime evidence. The returned GREEN is not acceptable until the four-call production correction is made after genuine RED and the normal GREEN evidence gates are rerun on the corrected bytes.
