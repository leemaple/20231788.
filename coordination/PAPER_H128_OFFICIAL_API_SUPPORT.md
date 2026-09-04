# Official OpenFHE 1.5.0: h=128 secret/key API support audit

## Scope and status

Read-only source audit dated 2026-09-04 (Asia/Shanghai), for required upstream pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`. All new implementation facts below come from this exact official OpenFHE commit, not a local implementation or modified OpenFHE tree.

The provided paper target is Section 6.3/Table 3, t2: N=2^15, sparse ternary secret h=128, dnum=11, auxiliary P=60 bits, Base=50x2, Mult=60x8, Div=40, input scale 2^100, eight repeated squarings and 1000 trials. Those are delegated, previously verified paper facts, not newly revalidated by this audit. The provided N=64 / UNIFORM_TERNARY / HEStd_NotSet tracer is not a paper reproduction.

Outcome: at the paper's N=32768, ordinary public CKKS key generation does **not** offer an h=128 configuration at this pin. Its sparse distribution branch requests h=192. The public low-level sampler accepts h=128; composing it with key-object APIs is possible to reason about, but no h=128 keypair construction or cryptographic test was executed here. One public keypair helper is explicitly debugging-only. No security claim, production endorsement, or project-completion claim follows. [Normal entry points][cckeys], [actual sampler selection][keygen], [low-level sampler API][samplerapi], [debugging-only helper][debugapi].

Repository observed before research: `/Users/lifeng/Documents/20231788-openfhe-codex-precision-01`, branch `codex/precision-01`, HEAD `7a5b523acd93243bf3efa200f1c11f632380c4dd`, initially clean. Before writing this note the HEAD was unchanged; another agent's untracked `coordination/REPEATED_RELIN2_RAISED_BASIS_REQUIREMENTS.md` existed and was not read or touched. This subtask edits only this note, using apply_patch. No commit, push, local build, cryptographic run, brute force, external-agent/browser activity, or quarantined implementation inspection occurred. Build/test command discovery and execution were outside this source-only subtask.

## Observed: normal KeyGen / SparseKeyGen call chain

1. `CryptoContextImpl::KeyGen()` forwards `false`; `SparseKeyGen()` forwards `true`. The latter's API documentation says it is currently unsupported by the schemes and describes its historical special/ring-reduction purpose. Neither accepts a Hamming weight. [Exact wrappers][cckeys].
2. `SchemeCKKSRNS` inherits `SchemeRNS`, which inherits `SchemeBase<DCRTPoly>`; neither derived scheme overrides this key-generation wrapper. Enabling PKE installs `PKECKKSRNS`. The base wrapper checks PKE enablement and invokes `m_PKE->KeyGenInternal(cc, makeSparse)`. [CKKS scheme][ckksscheme], [RNS scheme][rnsscheme], [enablement][enable], [wrapper][schemegen].
3. `PKECKKSRNS` inherits `PKERNS`, which inherits `PKEBase<DCRTPoly>`. The two derived PKE class declarations contain no `KeyGenInternal` override. [CKKS PKE class][ckkspke], [RNS PKE class][rnspke].
4. `PKEBase::KeyGenInternal` does not use `makeSparse`. It switches on the stored `SecretKeyDist`: Gaussian sampler; ternary sampler with its default h; or, for **both** SPARSE_TERNARY and SPARSE_ENCAPSULATED, the ternary element constructor with literal h=192. Unknown enum values throw. [Complete key-generation body][keygen].
5. That body samples s on `GetParamsPK()`, forms public elements (b,a) with b=ns*e-a*s, drops extra private-key towers back to Q when necessary, stores the two key objects, and synchronizes the public tag with the secret tag. [Same complete body][keygen].

The following is source-derived behavior, **not executed test results**:

| Public configuration / entry point | Resulting ordinary secret distribution | h=128 at N=32768? |
| --- | --- | --- |
| Default CKKS parameters + KeyGen | UNIFORM_TERNARY, sampler h=0 | No fixed weight |
| Default CKKS parameters + SparseKeyGen | Same branch as KeyGen; boolean ignored | No |
| SetSecretKeyDist(SPARSE_TERNARY) + KeyGen | Sampler requested h=192 | No: h=192 |
| SetSecretKeyDist(SPARSE_TERNARY) + SparseKeyGen | Same h=192 branch; not a separate h selector | No |
| SPARSE_ENCAPSULATED + ordinary KeyGen | Same h=192 branch | No |
| Low-level DCRTPoly ternary constructor with h=128 | Explicit-weight polynomial, not by itself a matching keypair | Polynomial only |

Sources for the mapping: [CKKS defaults][defaults], [entry-point wrappers][cckeys], [key-generation switch][keygen], [constructor signature][polyapi], [sampler][sampler]. The statement about SPARSE_ENCAPSULATED is limited to this ordinary KeyGen path, not every auxiliary key used by bootstrapping.

## Observed: defaults and the actual h parameter

- `CCParams<CryptoContextCKKSRNS>` derives from `Params` and selects CKKS defaults. `Params` loads those defaults; they include UNIFORM_TERNARY, HEStd_128_classic, HYBRID, ringDim=0, STANDARD encryption and PREMode=NOT_SET. These high-level defaults differ from the generic RLWE members' GAUSSIAN / HEStd_NotSet initializers. [CCParams][ccparams], [Params constructor][paramctor], [default loading][defaultload], [CKKS defaults][defaults], [generic RLWE members][rlwedefault].
- The audited complete `Params` member/setter surface and CKKS specialization contain `SetSecretKeyDist`, but no Hamming-weight member or setter. The lower RLWE `SetSecretKeyDist` also only assigns an enum. Most decisively, the traced normal KeyGen consumes the literal 192 rather than any configurable weight. This is a statement about these audited CKKS configuration/keygen surfaces, not an assertion that no API anywhere accepts h. [Members][params], [setters][setters], [specialization][ccparams], [lower setter][rlwesksetter], [keygen][keygen].
- The public `TernaryUniformGeneratorImpl::GenerateIntVector(size,h=0)` and `GenerateVector(size,modulus,h=0)` do accept h. `DCRTPoly` exposes a public ternary constructor with h=0 by default and forwards h to `GenerateIntVector`. [Sampler API][samplerapi], [constructor][polyapi], [constructor implementation][polyconstruct].
- In the nonzero-h branch, the code clamps h to size and contains a distinct-position sampling loop plus a sign-balance rejection loop. For h=128 at N>=128, the source-derived property is exactly 128 nonzeros, with 63, 64 or 65 positive coefficients: the rejection test requires the +1 count to lie between h/2-1 and h/2+1 inclusive. It is therefore not an arbitrary-sign fixed-support sampler. The equality of this sign-distribution detail to the paper's intended sampling law remains unverified; no general claim about small h edge cases is made. [Integer sampler][sampler].
- h=0 instead draws each coefficient from the integer interval -1 through 1. The DCRT constructor draws one integer vector, maps each negative coefficient to its residue in each CRT modulus, and converts each tower to the requested format. Thus the same signed coefficient pattern is represented across towers. [Distribution initialization][uniform], [integer sampler][sampler], [DCRT construction][polyconstruct].

Derived consequences: at the paper's N=32768, the standard sparse branch is exactly h=192. At N=64, requesting h=128 through the low-level sampler would be clamped to 64; it cannot serve as an h=128 acceptance test. At N=128, the standard sparse request of 192 would clamp to h=128, but changing the ring dimension to exploit that clamp is not the paper configuration. Dropping the extra private-key towers in normal KeyGen does not turn h=192 into h=128, because those towers encode the same sampled coefficient vector. [Sampler clamp][sampler], [shared CRT coefficients][polyconstruct], [normal KeyGen tower drop][keygen].

## Observed: declared SecretKeyDist and security checks

The high-level CKKS context generator passes both `GetSecurityLevel()` and `GetSecretKeyDist()` into the crypto parameters and invokes CKKS parameter generation. Its HE-standard checks map GAUSSIAN to HEStd_error and **every other declared secret distribution** to HEStd_ternary. The selected security lookup has no h argument. [Parameter wiring][wire], [distribution selection/check][security], [lookup categories][stdtable], [FindRingDim signature/body][findring].

When the security level is not HEStd_NotSet, this path checks an estimated modulus-bit bound, including estimated P for HYBRID, and checks the actual QP (HYBRID) or Q bit length after precomputation. With HEStd_NotSet, those two checks are skipped; the first stage still requires an explicit nonzero ring dimension. [Estimated-bound check][security], [actual-bound check][securityactual].

Derived limits:

- Neither a successful lookup nor the label SPARSE_TERNARY independently verifies the security of a custom h=128 secret. The inspected lookup distinguishes error versus ternary, not h=128 versus h=192, and cannot inspect a secret that will be injected later.
- Declaring UNIFORM_TERNARY while replacing the actual polynomial with sparse h=128 is a declared/actual-distribution mismatch, not security evidence. Merely changing the label to SPARSE_TERNARY does not add h-aware security analysis.
- `SetStdLevel` and `SetSecretKeyDist` only assign values, `SetPrivateElement` only assigns a polynomial, and the context's `ValidateKey` checks null/context identity, not ternarity, Hamming weight, or public/secret algebra. A hand-built context must establish which parameter checks actually ran; stored labels do not establish that.
- HEStd_NotSet is a bypass of these checks, not proof of any security level. No numerical security estimate for the paper configuration was performed.

These limits follow directly from [the parameter-generation guards][security], [the second guard][securityactual], [security setter][stdsetter], [distribution setter][rlwesksetter], [private setter][privatesetter], and [key validation][validkey].

## Observed: key consistency and public custom-key surfaces

### Replacing an existing secret is insufficient

`PrivateKeyImpl(cc)` requests a new identifier through `GenerateUniqueKeyID()`. Its public `SetPrivateElement` mutates only its stored polynomial; the corresponding public-key vector has a separate setter. Consequently, replacing an existing secret's polynomial does not recompute the already generated b,a or any evaluation keys. Tags alone do not certify algebraic correspondence. [Private constructor][privatector], [private setter][privatesetter], [public setter][publicsetter], [normal matching pair construction][keygen].

There is an additional cache hazard: `CryptoContextImpl::EvalMultKeyGen` and `EvalMultKeysGen` generate only when the key tag is absent from the static map. Calling either again after mutating a secret under an old tag can leave the old evaluation key in place. [Cache implementation][evalcache]. A future experiment should use a fresh immutable key lifecycle; this audit did not clear any caches or modify keys.

For fresh generation, the source path is based on the **provided** secret: the leveled implementation forms s^2 and asks key switching to convert from s^2 to s; the scheme wrapper sets the resulting tag. The CKKS/RNS leveled classes inherit that implementation without overriding these generators. HYBRID reads the supplied old and new private elements, extends the new secret from Q to QP, and constructs evaluation-key components from them; it does not call ordinary KeyGen to resample s. This supplies algebraic support for evaluating a fresh custom secret, but is not a runtime validation of a custom route. [CKKS leveled class][evalinheritckks], [RNS leveled class][evalinheritrns], [squared-secret generator][evalmult], [key-switch wrapper][keyswrap], [HYBRID selection][hybridselect], [HYBRID construction][hybrid], [tag assignment][evaltags].

### Candidate A: the named public debugging helper

`CryptoContextImpl::MultipartyKeyGen(vector<PrivateKey>)` is a public API, explicitly documented for debugging and not production. Its CKKS implementation is inherited through MultipartyCKKSRNS and MultipartyRNS from MultipartyBase; MULTIPARTY must be enabled. The base helper sums the supplied private elements on the context's Q basis, creates a new private object and matching (ns*e-a*s,a) public elements, and the scheme wrapper aligns the new tags. [API caveat][debugapi], [enablement][enable], [CKKS class][multipartyckks], [RNS class][multipartyrns], [base helper][debuggen], [tag wrapper][debugtags].

Derived research proposal, untested: a singleton vector containing a correctly constructed h=128 secret would make that sum equal to the supplied secret, while creating a matching new pair. Evaluation keys would have to be generated from the **returned** private key (with its new tag), not accidentally cached under the initial input key's tag. This is a source-supported debugging candidate, not a supported production single-party h=128 KeyGen factory.

The helper uses `GetElementParams()`, whereas normal KeyGen uses `GetParamsPK()`; RNS paramsPK selection can include additional basis elements for some modes. A future design must explicitly constrain/verify the supported mode and basis rather than assume the two paths are interchangeable. [Debug helper basis][debuggen], [normal basis][keygen], [paramsPK selection][paramsPK].

### Candidate B: public primitive/object composition

The public context exposes its scheme; the scheme exposes private-key `EncryptZeroCore`; the key object exposes a public constructor and public-element setter. For the supplied s, the RNS primitive returns (a*s+ns*e,-a) on the selected encryption basis. [Scheme accessor][schemepublic], [primitive wrapper][zerowrap], [primitive body][zero], [public-key constructor][publicctor], [public-element setter][publicsetter].

Derived research proposal, untested: treating that pair as public elements gives p0+p1*s=ns*e, algebraically the ordinary public-key relation after renaming -a. Creating it under the same fresh key tag and then generating new evaluation keys from that same s is plausibly consistent at the inspected algebraic level. The public encryption routine consumes those public elements in the expected relation. [Public encryption][pkencrypt], [normal relation][keygen], [evaluation-key construction][evalmult].

However, this composition is **not** an identified upstream h=128 keypair-generation API or a tested project capability. It leaves mode/basis, metadata, key lifecycle and all executable correctness checks to a future design. In particular, the wrapper supplies no explicit extended paramsPK and the body defaults to Q; modes requiring other public-key bases need separate verification. Do not present public accessibility of the components as production support. Candidate A has an explicit debugging-only caveat; Candidate B has no endorsement as a key-generation workflow in the audited source.

## Minimal options and gates for the future Pro design task

1. **Stay entirely on normal KeyGen:** select SPARSE_TERNARY and record h=192 as an explicit experimental deviation. This does not satisfy the paper's h=128 target. No implementation is proposed here.
2. **Keep h=128 mandatory:** evaluate one narrowly scoped public-API research adapter, preferably deciding first whether the explicitly debugging-only singleton helper is acceptable for the experiment. Candidate B is an alternative requiring its own justification. Use a fresh secret and regenerate the full matching key family; never mutate a live existing pair and infer consistency from its tags.
3. **If production-supported ordinary h=128 KeyGen at N=32768 is mandatory at the exact pin:** this audit found no such normal configuration. Preserve the requirement as unresolved. An upstream enhancement/change of pin would be a separate authority/scope decision, not silently patching official OpenFHE in this task.

Before accepting either custom research path, a new design/test task should establish all of the following; none was executed here:

- Enforce N>=128 before sampling, then check the **final returned key** is at the paper N, contains only coefficient residues 0,1,q_i-1, has h=128, and has the same signed support across all Q towers. The public private-element and tower getters permit inspection; copy towers and convert the copies to COEFFICIENT as upstream itself does. Count coefficients, not evaluation/NTT entries. Keep secret coefficients/key material out of evidence logs. [Private getter][privatesetter], [tower getters][towerapi], [upstream copy/format example][towercoeff], [sampler/CRT representation][polyconstruct].
- Establish whether the sampler's 63–65 positive-sign condition matches the paper's intended sampling law; otherwise record the distribution deviation separately from h.
- Verify required public-key basis, matching pair/tag ownership, public-key encrypt/decrypt, multiplication/relinearization using newly generated keys, and any specifically required rotations/relin2 extensions on Windows or CI. The generic EvalMult trace above does not validate the project's specialized relin2 path.
- Record exact actual N, Q/P bases, secret-distribution declaration, measured h and validation route. Treat any manual security-check bypass as an experimental limitation and require separate h=128-aware security review before any security statement.
- Continue the independent paper-parameter and eight-squaring/1000-trial acceptance gates. An h=128 property test alone cannot establish paper reproduction.

## Retrieval manifest

Each file below was retrieved read-only from the official `raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/...` URL shown by its link. Byte count and SHA-256 were computed over the complete received raw bytes in memory with Node's SHA-256 implementation; no upstream source copies were written locally. Excerpts/line numbers were produced by splitting the received UTF-8 text on newline. Duplicate requests for a file produced the same recorded hash. The manifest includes ancillary files examined even when a targeted symbol search had no matches.

A preliminary official GitHub blob lookup of `src/core/lib/math/ternaryuniformgenerator.cpp` returned HTTP 404; it supplied no evidence and has no source-byte manifest entry. The actual template implementation was then retrieved from `src/core/include/math/ternaryuniformgenerator-impl.h`. A preliminary GitHub blob view of rns-pke.cpp was subsequently matched to its raw source and hashed below.

| Official raw source at exact pin | Bytes | SHA-256 |
| --- | ---: | --- |
| [src/core/include/lattice/constants-lattice.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/constants-lattice.h) | 2531 | `c28a0db5b7e8db578e0ffdc8042cbaadf4838d55e7bba29681f4353f7ee92b86` |
| [src/core/include/lattice/hal/dcrtpoly-interface.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/dcrtpoly-interface.h) | 62733 | `281a0d11ef4f92306547bc4f88c8ace17b3393fdb72aa949713d332103efa0a2` |
| [src/core/include/lattice/hal/default/dcrtpoly-impl.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/dcrtpoly-impl.h) | 90376 | `31a9cab8e31d5ad46921ec422ec3b4cfa1b013a5d64da1cae9e53cafe884ec41` |
| [src/core/include/lattice/hal/default/dcrtpoly.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/dcrtpoly.h) | 20712 | `41ac8f33d8f777f7db093c045aecc7c40432a89ace8df62963944cfd04432c97` |
| [src/core/include/lattice/stdlatticeparms.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/stdlatticeparms.h) | 6145 | `fb1a3595658d2ff3968ac34ba22f1c4449ca65aa6adf5e77b1988848dd628aa1` |
| [src/core/include/math/ternaryuniformgenerator-impl.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator-impl.h) | 5325 | `dfc13aaa9ac2d8f34c99fa9483e6cdcf7284ca4eb983237bc369e65a02fbebbf` |
| [src/core/include/math/ternaryuniformgenerator.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator.h) | 3510 | `483a1373b3c1371381db47dc7fc28ed9143e247a9be673c1185a7902ba741a2c` |
| [src/pke/include/constants.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/constants.h) | 5715 | `780ea95b55c0c593de522008fecc0d46a2a36406ed2825a624253e87f0898680` |
| [src/pke/include/cryptocontext.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h) | 176160 | `cb88d34595f87eb9c525279a2000b20d15b24c3d71f0d177bd10f1a933538cfb` |
| [src/pke/include/key/privatekey.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h) | 5593 | `45a9c659544bec8e81620703800c2a46ecb65e00ab71c29da8d9afb16abbdf58` |
| [src/pke/include/key/publickey.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/publickey.h) | 5256 | `fab81bb019d71a95ae3bfd6f465637d29b86e6705c5bff3645708c819fa698c3` |
| [src/pke/include/scheme/ckksrns/ckksrns-leveledshe.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-leveledshe.h) | 7729 | `f76b151d2eea047dcf6a19a2a61cb2530357bef5d7af09215792af6a07b44c0d` |
| [src/pke/include/scheme/ckksrns/ckksrns-multiparty.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-multiparty.h) | 4210 | `7453ac089951214109a73bbb155115a3018c79dba369fe239a3af65f94239a2e` |
| [src/pke/include/scheme/ckksrns/ckksrns-pke.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-pke.h) | 3495 | `7b5c02defc5887e9b7c6ada890768523387cb254af12f4e7a83705f7701fd449` |
| [src/pke/include/scheme/ckksrns/ckksrns-scheme.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-scheme.h) | 3222 | `b7c4802ca935ac6671a3a64bf25209f71d69becb715686d3f23d34533c08dc53` |
| [src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h) | 6677 | `803303b2c09df0c41ef143602a06065d9f3fa8fc1d7415f2bd8859755e2ff9cc` |
| [src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h) | 4045 | `aecf373f3cb4c488f19cc1630dcf183172c2116eece4159b1ff4ad8b0bd9d79a` |
| [src/pke/include/scheme/gen-cryptocontext-params-defaults.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params-defaults.h) | 9047 | `63f6dcf827b3bb31912539a1aef74eb9d6bb4be2648f59f778f866edbe09c17f` |
| [src/pke/include/scheme/gen-cryptocontext-params.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params.h) | 17933 | `ebbd6f4b1664d047d565ca99b776a487717172432126f036e9cb8f486416fec0` |
| [src/pke/include/schemebase/base-pke.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-pke.h) | 5637 | `c34bc5b4d615076f3f17bc3d7222be810a067d1a2863e1c30f171bc5dcd54a3d` |
| [src/pke/include/schemebase/base-scheme.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-scheme.h) | 74489 | `9140faee557d3aa71f1e9301ea850ebb97f4a0cdd571206b2f6f37ceffff40d0` |
| [src/pke/include/schemebase/rlwe-cryptoparameters.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h) | 21793 | `36a3231c6df0ddaafb066757c06b70f933136013d5421370257477b832b5245b` |
| [src/pke/include/schemerns/rns-cryptoparameters.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h) | 61640 | `29e3d0666c41a36a4a7627fc4dd26487999ed2d81f6e0dd68046624275cb2340` |
| [src/pke/include/schemerns/rns-leveledshe.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-leveledshe.h) | 14839 | `e11102dd9c25e32c6c644c2e84e86fd752ba372aff028c5f50dade4c769bfd17` |
| [src/pke/include/schemerns/rns-multiparty.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-multiparty.h) | 5223 | `23e7fe0057fd10741e006d8e7e3b06d2b1583d17f0e97747107ac5dd086d6d55` |
| [src/pke/include/schemerns/rns-pke.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-pke.h) | 5308 | `1ed5ceb82280ef49fc532635ea85cd49bc7316ade517bdaa09915feaf7b05132` |
| [src/pke/include/schemerns/rns-scheme.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-scheme.h) | 3415 | `d6a74e2a3b6b8f010a11be32fc7030a030c5044d99f769cd49bc629b035bc6e7` |
| [src/pke/lib/cryptocontext.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp) | 42570 | `de14bbf46686facd807485e3471f6293271cc0d2f03ae300d64f14f71aa217db` |
| [src/pke/lib/keyswitch/keyswitch-hybrid.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp) | 21228 | `872d5e594ac2343f9d055a4e628871987113f67f2142d3a664b2015f17e1d345` |
| [src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp) | 25684 | `7476e1be5f7f1e3dc3d5d91af282340bc0e7d218323b6b13509bfa4bda096d8e` |
| [src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp) | 3317 | `1ee7cc32dc116c67254a27f554eb380537a033f679b8a160b8342819f6dc2c23` |
| [src/pke/lib/scheme/gen-cryptocontext-params-impl.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/gen-cryptocontext-params-impl.cpp) | 10747 | `083bc47fd7240e95e3db3243711919813e219b2136b44c5521e019e1ab5df5c9` |
| [src/pke/lib/schemebase/base-leveledshe.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-leveledshe.cpp) | 29321 | `a15b0d8dec4bd27d4d2cb86f1d659bf58e97ba913b935af3d8db65285fa6b9f9` |
| [src/pke/lib/schemebase/base-multiparty.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-multiparty.cpp) | 16923 | `c3e000481480b7d7a255b14e665e4fbe2004dd110639f53b90a3a576f46ef66c` |
| [src/pke/lib/schemebase/base-pke.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-pke.cpp) | 7991 | `371a22ceaea7ed39a2ae164a1456966b130afbbcc5eafc9f323feda98a5d8e84` |
| [src/pke/lib/schemebase/base-scheme.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-scheme.cpp) | 14355 | `db7cf2642c2139e0126cf88d2f7237c0351b5c3f74a95d901d0455d99c3cbd6a` |
| [src/pke/lib/schemerns/rns-multiparty.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-multiparty.cpp) | 21766 | `e491beb7477f832df893b4cc8c06401487c52754e8c899e78587542a6f0a0426` |
| [src/pke/lib/schemerns/rns-pke.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp) | 8654 | `e1103468a406d16202fc66d918b28fdf93d436aeadb86efda4a01032fac3bf19` |

## Exact upstream line citations

All reference links in this note resolve to commit-pinned GitHub blob line anchors:

[cckeys]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L1224-L1241
[schemegen]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-scheme.h#L196-L203
[ckksscheme]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-scheme.h#L56-L88
[rnsscheme]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-scheme.h#L62-L96
[enable]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp#L42-L62
[ckkspke]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-pke.h#L45-L94
[rnspke]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-pke.h#L53-L138
[keygen]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-pke.cpp#L45-L98
[params]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params.h#L52-L179
[setters]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params.h#L366-L466
[ccparams]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h#L54-L97
[defaults]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params-defaults.h#L46-L86
[defaultload]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/gen-cryptocontext-params-impl.cpp#L43-L87
[paramctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/gen-cryptocontext-params.h#L203-L206
[rlwedefault]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h#L529-L538
[rlwesksetter]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h#L390-L396
[stdsetter]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h#L341-L347
[wire]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h#L97-L146
[samplerapi]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator.h#L64-L82
[sampler]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator-impl.h#L102-L143
[uniform]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/ternaryuniformgenerator-impl.h#L50-L65
[polyapi]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/dcrtpoly.h#L109-L112
[polyconstruct]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/dcrtpoly-impl.h#L177-L193
[security]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L114-L155
[securityactual]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L192-L205
[stdtable]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/stdlatticeparms.h#L62-L76
[findring]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/stdlatticeparms.h#L129-L144
[privatector]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h#L73-L90
[privatesetter]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h#L127-L149
[publicsetter]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/publickey.h#L117-L141
[publicctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/publickey.h#L62-L79
[validkey]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L323-L333
[evalcache]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp#L86-L116
[evalmult]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-leveledshe.cpp#L135-L162
[evaltags]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-scheme.cpp#L57-L71
[hybrid]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L46-L129
[hybridselect]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-scheme.h#L68-L77
[evalinheritckks]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-leveledshe.h#L48-L199
[evalinheritrns]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-leveledshe.h#L50-L376
[keyswrap]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-scheme.h#L254-L258
[debugapi]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L3080-L3092
[debuggen]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-multiparty.cpp#L50-L80
[debugtags]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-scheme.cpp#L148-L161
[multipartyckks]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-multiparty.h#L47-L92
[multipartyrns]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-multiparty.h#L73-L113
[paramsPK]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L263-L269
[schemepublic]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L969-L983
[zerowrap]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-scheme.h#L227-L232
[zero]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L111-L146
[pkencrypt]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L148-L196
[towerapi]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/dcrtpoly-interface.h#L282-L312
[towercoeff]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L59-L81
