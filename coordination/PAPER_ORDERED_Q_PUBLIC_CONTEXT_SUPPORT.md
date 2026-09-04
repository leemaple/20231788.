# Ordered Q: public CKKS context/table/key construction at the official pin

## Status and scope

Source-supported route/constraint inventory, 2026-09-04 (Asia/Shanghai). Required OpenFHE 1.5.0 source pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`. **Not an adopted design, implementation, executable proof, security assessment, or paper-reproduction claim.**

Delegated paper target: N=32768, ordered Q with Base50x2, Mult60x8, trailing Div40; dnum=11; HYBRID auxiliary P60; logical input scale 2^100; sparse ternary h=128. The target facts were provided by the parent; this audit did not reread the paper or select concrete primes.

The repository was `/Users/lifeng/Documents/20231788-openfhe-codex-precision-01`, branch `codex/precision-01`, HEAD `47907783a6141d0174da79eae264d779fc598f28` at entry. Main advanced HEAD to `8094f84c167ff8d19355c346dd7ea1137bb3056c` before this note was written. The two permitted existing notes, `coordination/PAPER_H128_OFFICIAL_API_SUPPORT.md` and `coordination/REPEATED_RELIN2_RAISED_BASIS_REQUIREMENTS.md`, were treated as data and left untouched. No other local implementation/OpenFHE source was read. Only this note is written by this subtask.

## Answer in brief

**Yes, the public interfaces expose the necessary construction pieces:** an explicitly ordered ILDCRTParams basis, a concrete CryptoParametersCKKSRNS object, its public PrecomputeCRTTables method, a configured SchemeCKKSRNS, and CryptoContextFactory registration. The same table builder consumes the supplied ordered Q rather than requiring one common prime-bit size. This is enough source evidence to investigate fresh basis-specific context/key families without changing upstream; it is **not** evidence that those families already implement the paper correctly. [Ordered-basis constructor][qctor], [CKKS constructors/table signature][ckksctor], [RNS table builder][precompute], [CKKS table builder][ckkstables], [factory][factory].

Five material constraints remain:

- Actual precomputation cannot use 11 partitions on a fresh full basis with 10 or fewer limbs.
- Precompute generates P itself; auxBits=60 is not an explicit P-prime input and does not promise the same P across differing Q families.
- FIXEDMANUAL computes actual-prime rescale arithmetic but uses a constant 2^p scaling-metadata factor; its decoder also uses p/noiseScaleDeg rather than an arbitrary ciphertext scale label.
- Factory registration and reuse, context identity, and global tag-keyed evaluation-key caches matter.
- Manual construction does not run normal parameter-generation/security checks, and does not solve the separate h=128 key-generation obligation.

The source facts and derivations supporting these points follow below.

## Observed public construction surface

| Layer | Public surface at the pin | What it supplies / does not supply |
| --- | --- | --- |
| Ordered RNS basis | ILDCRTParams(cyclOrder, moduli, rootsOfUnity) | Appends corresponding limbs in input order and multiplies their moduli; checks equal vector lengths. It does not replace them with a uniform-bit chain. |
| Encoding metadata | EncodingParamsImpl(p, batchSize, ...) | Stores p and slot/batch settings; in CKKS, p is used as an exponent for nominal scale, not an ordered Q specification. |
| Concrete crypto parameters | CryptoParametersCKKSRNS(params, EncodingParams, ..., ksTech, scalTech, ..., PREMode, ..., noiseScale, ..., compositeDegree, registerWordSize, ckksDataType) | Captures supplied basis/configuration through RNS/RLWE constructors; does not run the normal Q generator or standard-security lookup. |
| Tables | PrecomputeCRTTables(ksTech, scalTech, encTech, multTech, numPartQ, auxBits, extraBits) | Public CKKS override calls the RNS builder and then CKKS-specific rescale/scale/Barrett precomputations. |
| Scheme | SchemeCKKSRNS; inherited SetKeySwitchingTechnique | Creates the scheme and selects BV or HYBRID implementation explicitly. |
| Registered context | CryptoContextFactory<DCRTPoly>::GetContext(params, scheme, schemeId) | Registers a new context when no equivalent scheme/parameters exist; otherwise returns the existing one. |
| Required operations | CryptoContextImpl::Enable | Dispatches feature enablement to the scheme. PKE and LEVELEDSHE install their CKKS implementations; KEYSWITCH enablement alone does not instantiate its implementation. |

Table sources: [basis][qctor], [encoding][encctor], [concrete constructors][ckksctor], [RNS constructor assignments][rnsctor], [RLWE assignments][rlwector], [precompute declaration][rnsdecl], [CKKS precompute body][ckkstables], [scheme constructor][scheme], [key-switch selection][schemeks], [factory API][factoryapi], [factory body][factory], [feature dispatch][ccenable], [CKKS enablement][enable].

Constructor-overload difference: the CKKS overload taking EncodingParams defaults PREMode to NOT_SET; the simpler plaintext-modulus overload does not expose PREMode and its RNS constructor supplies INDCPA. For HYBRID, GetParamsPK returns QP when PREMode is not NOT_SET; otherwise STANDARD uses Q. Thus those overloads are not interchangeable if the intended public-key basis matters. A future design must specify the mode rather than depend on an accidental overload/default. [CKKS overloads][ckksctor], [RNS forwarding][rnsctor], [public-key basis selector][pkbasis].

The explicit moduli/roots constructor creates every limb with the same supplied cyclotomic order, but does not perform primality, root-order, pairwise-distinctness or security validation in that constructor chain. The leaf constructor stores the supplied modulus/root; the base computes the ring dimension from the order and stores the values. A second public ILDCRTParams overload can derive roots from supplied moduli, but that is not an input-validation certificate. [Vector constructor][qctor], [leaf constructors][limbctor], [base constructors][elemctor], [moduli-only/limb-pointer overloads][qauto]. The candidate must separately validate nonempty Q, arithmetic/backend limits and correct roots for order 65536; no primes or roots were generated here.

## Observed table dependencies and order

The RNS builder first saves the seven precompute arguments, then extracts Q moduli and roots in their supplied order. It initializes DFT/NTT data for N and Q. In HYBRID it constructs contiguous partitions, P, concatenated QP, P-to-Q constants, and level/partition-indexed complementary bases and inverse tables. The CKKS override then builds successive trailing-prime rescaling tables from the actual moduli; it does not assume all those moduli have equal bit lengths. [Initialization][precompute], [partitions][parts], [P/QP generation][pgen], [P/Q constants][ptables], [prefix/complement tables][complements], [CKKS actual-Q rescale tables][ckkstables].

Three similarly named inputs must remain distinct:

| Quantity | Location / effect | Not equivalent to |
| --- | --- | --- |
| Paper trailing Div40 prime | Explicit last member of the supplied Q vector | HYBRID P or a scale label |
| auxBits=60 | Requested auxiliary-prime width in P generation | A caller-supplied P prime or an extra Q limb |
| extraBits | Stored precompute configuration, used by the CKKS scale-factor branches | An instruction for this public table method to append the paper's Div40 prime |

These distinctions follow from Q being read as-is, P being generated separately, QP being formed as Q followed by P, and extraBits being used by the scaling-factor logic. The table method does not call the normal Q-prime generator. [RNS inputs][precompute], [P and QP][pgen], [extraBits-dependent scale tables][flex].

For the proposed full 11-limb vector, the indexed interpretation is Q[0..1]=Base50, Q[2..9]=Mult60, Q[10]=Div40. The following are **derived under that supplied target**, not observed runtime values:

- numPartQ=11 gives ceil(11/11)=1 limb per partition. If every specified Mult prime actually has bit length 60, the maximum partition bit length is 60, so auxBits=60 leads the table builder to choose one P limb. The actual selected prime, its exact width and QP product still need measurement.
- The code starts auxiliary-prime generation from auxBits and FindAuxPrimeStep; CKKS supplies step 2N. It skips collisions with the current Q. A different Q family can therefore obtain a different P if the collision set changes. There is no P-vector input in the inspected precompute signature.
- For a fresh nonempty full basis with 1<=L<=10 and numPartQ=11, ceil(L/11)=1 and the guard L<=1*(11-1) rejects it. A low 10-limb context or a later raised 10-limb context therefore cannot blindly reuse literal numPartQ=11.
- EstimateLogP is not a substitute for that admission check: it clamps numPartQ to sizeQ, while actual PrecomputeCRTTables does not. It also models first/common/optional-extra bit sizes, not an arbitrary supplied ordered vector. Deciding how the paper's dnum=11 relates to active partitions versus a newly created smaller full context is still a design obligation.

Sources: [actual partition guard][precompute], [partition bit lengths][parts], [P selection/collision loop][pgen], [CKKS auxiliary step][barrett], [public signature][rnsdecl], [different estimator behavior][estimate]. No choice to change dnum, pad a basis, or replace the paper's P requirement is adopted here.

## Observed scale, level and decoding behavior

In FIXEDMANUAL (also the fixed branch used by FIXEDAUTO), CKKS precompute sets m_approxSF=2^p. Both GetScalingFactorReal(level) and GetModReduceFactor(index) return that constant; they do not return the varying 40/60-bit prime selected by the index. FLEXIBLE/COMPOSITE modes instead use their precomputed arrays. [Scale-table initialization][flex], [public getters][sfget].

ModReduceInternalInPlace computes diffQl from full-context Q size minus actual ciphertext limb count. It applies the appropriate actual-Q trailing-prime tables, decreases noiseScaleDeg, increments level, and divides the scale metadata by GetModReduceFactor. LevelReduceInternalInPlace only drops trailing limbs and increments level. **Consequently, heterogeneous-Q table construction is not by itself heterogeneous-scale semantic correctness.** [Exact rescale/level implementation][modreduce].

Merely choosing FLEXIBLEAUTO is not an established remedy. For non-composite FLEXIBLEAUTO with extraBits=0 and the supplied 11-limb order, source arithmetic gives nominal factors D, D, then D^2/M, where D is the 40-bit tail and M is the adjacent 60-bit prime. At that third factor the comparison ratio is D/M<2^-19, below the permitted 0.5 threshold, so the source implies rejection at level 2. This is a bounded derivation from the prescribed bit widths and recurrence, **not a run**. No composite-scaling alternative was evaluated or selected. [Recurrence and rejection][flex].

The context's CKKS plaintext helper obtains its base scale from GetScalingFactorReal, supplies noiseScaleDeg, and chooses a prefix basis by popping the last parameter for each requested level unless explicit parameters are provided. Encoding applies additional scale powers according to noiseScaleDeg and updates the stored scale to its power. FIXEDMANUAL decoding uses p and noiseScaleDeg for normalization; it does not use an arbitrary ciphertext scalingFactor in the flexible-mode normalization branch. [Plaintext construction][makescale], [encoding scale powers][encode], [decode branches][decode].

The public ciphertext scale setter only stores a double. The DCRT SetLevel specialization stores the level and checks limb count against noiseScaleDeg; it accesses the first polynomial element and does not reconstruct basis-specific tables or key rows. Its use therefore also presupposes populated elements. [Scale/degree metadata setters][scalesetter], [DCRT level setter][levelsetter]. Therefore neither setting level=0 on a reordered basis nor setting scalingFactor=2^100 proves the required paper representation.

Logical input scale 2^100 remains an independent encode/decode/high-low representation gate. It is not determined by the sum of Q bit widths. In particular, a stored double 2^100 is not proof that choosing p=100 is a valid ordinary 64-bit encoding path: that encoder includes a conversion through std::llround(scalingFactor) to uint64_t even though it separately reduces oversized coefficient values before conversion. The construction audit does not validate that route or select p/noiseScaleDeg. [64-bit coefficient path][enc64], [scale-to-integer conversion and CRT powers][encode].

## Observed factory identity and why not mutate a live context

GetContext compares scheme and crypto parameters and returns an existing match before considering creation. Ordered limb parameters are included through the CryptoParameters equality chain; ILDCRTParams compares native limb objects position by position, and their base equality includes modulus/root/order. Different ordered bases thus participate in distinct parameter identities; equivalent requests may be interned to one context. The schemeId argument is applied when the factory creates a new object, not when it returns an existing match. [Factory behavior][factory], [base equality][baseeq], [RLWE equality][rlwecompare], [RNS equality][rnscompare], [ordered equality][qeq], [leaf equality][elemeq].

A public direct CryptoContextImpl constructor exists, but normal cc->KeyGen calls GetContextForPointer, which searches the factory registry by pointer and throws if absent. A fresh shared pointer alone is therefore not the source-supported replacement for the registered factory route. [Public constructor][ccctor], [KeyGen entry point][cckeygen], [registry lookup][registry].

Immutability is an application lifecycle requirement, not a property enforced by these mutable public types:

- SetElementParams only replaces m_params; it does not invalidate or regenerate CRT tables, key material or ciphertexts. SetEncodingParams similarly only assigns the pointer. [Setters][basemut].
- A CryptoContextImpl copy shares m_params and m_scheme. ILDCRTParams copies share the per-limb parameter pointers. The RNS crypto-parameter copy constructor itself only initializes a subset of mode fields, not the full precomputed-table state. Copying and then substituting a basis is not an established fresh-context/table construction shortcut. [Context copy][ccctor], [basis copy][qcopy], [RNS copy][rnsclone].
- Precompute writes tables and mode fields based on its then-current basis. Keys and existing ciphertexts refer back to their context; changing a published object's basis/table meaning can therefore affect them. Public pointer access is not an immutable snapshot. [Precompute assignments][precompute], [crypto-object ownership][cryptobject].

Derived candidate lifecycle, **not code or an adopted sequence**: construct fresh basis/encoding/parameter objects; fully specify metadata and precompute from that exact basis before publishing it; configure the matching scheme/key-switch implementation; obtain and verify the registered CKKS context; enable the required operations and create matching keys; thereafter treat all objects and shared parameters as immutable. Account for factory reuse rather than clearing its global registry to force a new identity. [Public API inventory above][factoryapi], [normal generation's setup ordering][normalwire], [explicit HYBRID selection][schemeks].

## Observed security checks absent from manual construction

The ordinary high-level route sets noiseScale=1, flooding-distribution metadata, multiplicativeDepth and noiseEstimate; computes the digit setting; configures the scheme; and calls ParamsGenCKKSRNS. Manual constructors and PrecomputeCRTTables do not execute that high-level initializer. Fields must be deliberately specified; a manual context does not infer the ordinary generator's slot/depth/noise setup merely from an 11-limb Q. [Normal setup][normalwire], [constructor assignments][rlwector], [precompute body][precompute].

The normal parameter generator performs PRE-mode restrictions, security-bound/ring-dimension checks, batch-size checks, zero-batch defaulting, normal Q generation and the post-precompute actual QP/Q security-size check. Its HE-standard mapping is Gaussian versus ternary, with no h argument. HEStd_NotSet skips those standard-security guards. The constructors simply store a SecurityLevel; precompute computes arithmetic tables and does not invoke those guards. Thus even storing HEStd_128_classic in manually built parameters would not establish that such a check ran. [Normal restrictions/security checks][security], [normal Q/defaults/actual-size check][normalq], [RLWE constructor][rlwector], [RNS precompute][precompute], [CKKS precompute][ckkstables].

The high-level generator also contains backend/noise-flooding restrictions before parameter generation. These are distinct from mathematical consistency of CRT tables and cannot be inferred from successful table creation. [High-level checks][normalextras].

Do not call the normal ParamsGenCKKSRNS merely as a supposed read-only validator of a supplied Q: it constructs a new Q vector and calls SetElementParams before precomputing it. The normal fixed-prime selection has one first size and one common dcrtBits setting, so it does not preserve the supplied Base50x2 / Mult60x8 / Div40 vector. [Replacement of element parameters][normalq], [common-size generation][singleprime], [first/extra-prime handling][singlefirst].

Pending security work must use actual ordered Q/P, the actual h=128 secret distribution and the exposed key family. No HE-standard, RLWE-security or paper-security result was computed here; HEStd_NotSet remains an explicit unchecked-experiment limitation, not evidence.

## Observed key/context compatibility; inference for basis families

Normal KeyGen uses GetParamsPK, creates matching public elements from the sampled secret, drops private extra towers back to Q when needed, and aligns the tags. It rejects a null paramsPK with a precomputation diagnostic; that guard is not a full table-validity check. It still requests h=192 for ordinary sparse keys. Merely constructing a custom Q context does not provide the paper's h=128 secret. The existing h=128 audit retains the separate public custom-key candidates and their debugging/untested limitations. [Key-generation body][keygen], [basis selector][pkbasis].

HYBRID key generation takes Q/QP and all key-switch constants from the supplied new key's context, reads its actual secret, extends it from Q to QP and constructs matching per-partition rows. Consumption uses context partitions/complement tables by position, and selects Q key rows by their original index, shifting only P indices after prefix reduction. These are basis-specific objects, not arbitrary-subset dictionaries. [HYBRID generation][hybridgen], [decomposition/table use][hybriduse], [mod-down and row selection][hybridrows].

The public PrivateKeyImpl constructor/setter can bind a caller-provided polynomial to a fresh key object in a chosen context. Public CiphertextImpl constructors and SetElements can create a new object in a chosen context with explicit polynomial data. Those setters do not prove a correct secret relationship or a valid ciphertext conversion. [Private key object][skctor], [private element setter][skset], [ciphertext constructors][ctctor], [ciphertext element setters][ctelements].

Additional compatibility constraints:

- The context wrapper checks pointer identity for keys/ciphertexts; binary ciphertext operations also require equal context and key tag. A ciphertext in one family is not automatically accepted by another context simply because some primes match. [Checks][ctcheck].
- Evaluation-key maps are static and keyed by tag, not by a composite (context,basis,tag) key. EvalMultKeyGen skips a tag already present, and EvalMult selects by ciphertext tag. Reusing a tag across distinct basis families can select or retain the wrong table/key family. [Map declarations][evalmap], [generation cache][evalcache], [lookup][evallookup].
- If families are intended to represent the same underlying secret, independently calling ordinary KeyGen per context does not establish that relation; it samples separately. A future design must construct/prove the intended secret relation, per-family public/evaluation keys and transition semantics. Tags are only routing metadata. [Sampling][keygen], [ownership/tag equality][cryptobject].

**Derived feasibility, still pending:** a new registered context whose *full* Q is exactly a particular raised high basis can have tables generated for that ordered basis, avoiding the original-context non-prefix mismatch at the table-selection layer. A low basis can likewise be described independently. Public interfaces do not force an upstream patch for expressing those objects. However, the family must satisfy the partition-count guards, P selection, exact-secret/key consistency, context-binding and scale/decode constraints above. This note does not identify a one-call public cross-context family converter or prove a repeated Relin2 lifecycle.

The symbolic raised-basis concern in the permitted repeated-Relin2 note is therefore a valid design obligation, not proof of impossibility. No active first-Mult2 implementation or hosted CI result was inspected, contradicted or accepted by this source-only work.

## Minimal future design/runtime gates

Before adopting any ordered-Q family route:

1. Freeze each ordered full/active/raised basis and exact prime/root identities; keep Div40 distinct from auxiliary P. Verify limb widths, uniqueness, cyclotomic order and actual Q/P/QP after table creation.
2. Decide paper-compatible dnum semantics for every smaller family; do not assume the estimator's clamp exists in actual precompute. Decide whether differing generated P across families is permissible.
3. Define p, logical scale, physical high/low component scales, noiseScaleDeg, level, and decoding semantics together. Cover the 40-bit divide and 60-bit multiplication-prime drops with an independent oracle; no metadata-only correction is accepted.
4. Establish one approved h=128 construction and the exact required secret relation across families. Generate matching keys under distinct family identities and verify that original contexts, parameters, keys and ciphertext inputs remain unchanged.
5. Run bounded construction/key/encode/decode/rescale/Relin2 checks on Windows or CI, then second and ultimately eight repeated squarings. Inspect actual basis/table/key rows and invalid-state rejection, not just a green high-level result.
6. Separately record which normal validations were reproduced or deliberately bypassed and obtain appropriate h-aware security analysis before any security statement.

No gate above was executed here. No source/test/config changes, crypto computations, builds, commits, pushes, external-agent/browser/CI actions or further delegation occurred.

## Exact-source retrieval manifest

All files below were read from the official raw URLs at the required commit. SHA-256 and byte count cover the complete received raw bytes, computed in memory with Node. Only selected numbered excerpts were inspected; no upstream source copies were written locally. Repeated retrievals matched the listed hashes. This subtask's 27 successful distinct source-file reads total 562628 bytes; the queries were bounded and no prime generation, factorization, brute force, compilation or cryptographic execution occurred.

Two targeted path guesses returned 404 and supplied no evidence: `src/pke/lib/ciphertext.cpp` (actual source is ciphertext-impl.cpp) and `src/core/include/lattice/hal/default/elemparams.h` (actual source is lattice/hal/elemparams.h). They have no source-byte entries below.

| Official raw source at exact pin | Bytes | SHA-256 |
| --- | ---: | --- |
| [src/core/include/lattice/hal/default/ildcrtparams.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/ildcrtparams.h) | 14962 | `e530217069706ad970c53caad4edbcaeb8ea8ef38fbd613c1062fbebcbba35eb` |
| [src/core/include/lattice/hal/default/ilparams.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/ilparams.h) | 6072 | `40b7f34ef5186b013df015884f646bc9931f9797726c614cfda05d30556365c1` |
| [src/core/include/lattice/hal/elemparams.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/elemparams.h) | 10664 | `5ee08b4c8bc622050f28666af323617179608ca1fbbe924d10172613545d893d` |
| [src/pke/include/ciphertext.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/ciphertext.h) | 19392 | `57787d5bbb7ce43c4c3db004e4ea1be08aa7324cf9ea18d4f350c30aae5be55c` |
| [src/pke/include/cryptocontext.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h) | 176160 | `cb88d34595f87eb9c525279a2000b20d15b24c3d71f0d177bd10f1a933538cfb` |
| [src/pke/include/cryptocontextfactory.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontextfactory.h) | 3609 | `09b9a58957a62884e913c1be7a53249bd0c397a91060141255280d7c363880c6` |
| [src/pke/include/cryptoobject.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptoobject.h) | 4349 | `57692296aff301cbacce1d3c8d82e3bae5b724c319f95d2c617e286644d85a7c` |
| [src/pke/include/encoding/encodingparams.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/encoding/encodingparams.h) | 11542 | `43188eae044e9be0ad4e7bbd1262a768e5679789882c7ffa7504749ed2d5f053` |
| [src/pke/include/key/privatekey.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h) | 5593 | `45a9c659544bec8e81620703800c2a46ecb65e00ab71c29da8d9afb16abbdf58` |
| [src/pke/include/scheme/ckksrns/ckksrns-cryptoparameters.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-cryptoparameters.h) | 7505 | `b10acefbde0e36e7eb1c15e73d117a15e913221bbc5d19ce80768cc546192d54` |
| [src/pke/include/scheme/ckksrns/ckksrns-scheme.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-scheme.h) | 3222 | `b7c4802ca935ac6671a3a64bf25209f71d69becb715686d3f23d34533c08dc53` |
| [src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h) | 6677 | `803303b2c09df0c41ef143602a06065d9f3fa8fc1d7415f2bd8859755e2ff9cc` |
| [src/pke/include/schemebase/base-cryptoparameters.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-cryptoparameters.h) | 7770 | `b7f5e30106cc5898b495a195f835b304aaabb01e30b5599144637657d159ebfa` |
| [src/pke/include/schemebase/rlwe-cryptoparameters.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h) | 21793 | `36a3231c6df0ddaafb066757c06b70f933136013d5421370257477b832b5245b` |
| [src/pke/include/schemerns/rns-cryptoparameters.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h) | 61640 | `29e3d0666c41a36a4a7627fc4dd26487999ed2d81f6e0dd68046624275cb2340` |
| [src/pke/include/schemerns/rns-scheme.h](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-scheme.h) | 3415 | `d6a74e2a3b6b8f010a11be32fc7030a030c5044d99f769cd49bc629b035bc6e7` |
| [src/pke/lib/ciphertext-impl.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/ciphertext-impl.cpp) | 2753 | `719758fa691c83f5375be67ab3bf7f38f907b9709a8a60a7be8208bc51218d2e` |
| [src/pke/lib/cryptocontext.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp) | 42570 | `de14bbf46686facd807485e3471f6293271cc0d2f03ae300d64f14f71aa217db` |
| [src/pke/lib/cryptocontextfactory.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontextfactory.cpp) | 4060 | `b7b8da50247a2267e0d05bb1a211a69336a49b0eb0b18238a901d4a3fdd5cbdb` |
| [src/pke/lib/encoding/ckkspackedencoding.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp) | 22759 | `ce967bf68d80a63101984e8927298923c6c3c6754dc522e63355e7ec40655401` |
| [src/pke/lib/keyswitch/keyswitch-hybrid.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp) | 21228 | `872d5e594ac2343f9d055a4e628871987113f67f2142d3a664b2015f17e1d345` |
| [src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp) | 10675 | `038a8c285011a4626678022cb5f4e75217e0c2d931a261314afc4370448668fe` |
| [src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp) | 36039 | `68fd392d9aaabe27e7ebf1e1cf354dd4c856da7bfac2d98afd5b6d796214f282` |
| [src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp) | 25684 | `7476e1be5f7f1e3dc3d5d91af282340bc0e7d218323b6b13509bfa4bda096d8e` |
| [src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp) | 3317 | `1ee7cc32dc116c67254a27f554eb380537a033f679b8a160b8342819f6dc2c23` |
| [src/pke/lib/schemebase/base-pke.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-pke.cpp) | 7991 | `371a22ceaea7ed39a2ae164a1456966b130afbbcc5eafc9f323feda98a5d8e84` |
| [src/pke/lib/schemerns/rns-cryptoparameters.cpp](https://raw.githubusercontent.com/openfheorg/openfhe-development/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp) | 21187 | `4dbf003cce3cc02d92ebeb62398c2350c0f9e31d171b19a592e1e0fa6b9c9907` |

## Commit-pinned line references

[qctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/ildcrtparams.h#L119-L145
[qauto]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/ildcrtparams.h#L165-L195
[qcopy]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/ildcrtparams.h#L197-L211
[qeq]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/ildcrtparams.h#L288-L308
[limbctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/default/ilparams.h#L78-L89
[elemctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/elemparams.h#L78-L94
[elemeq]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/lattice/hal/elemparams.h#L213-L216
[encctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/encoding/encodingparams.h#L70-L78
[ckksctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-cryptoparameters.h#L51-L94
[rnsctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L100-L145
[rlwector]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h#L108-L132
[rnsdecl]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L173-L204
[precompute]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L44-L90
[parts]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L92-L135
[pgen]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L127-L183
[ptables]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L185-L236
[complements]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L238-L335
[ckkstables]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp#L44-L84
[flex]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp#L86-L175
[barrett]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp#L176-L188
[sfget]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L601-L649
[pkbasis]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L263-L269
[pget]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L348-L421
[estimate]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-cryptoparameters.cpp#L393-L444
[scheme]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/ckksrns-scheme.h#L56-L68
[schemeks]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-scheme.h#L62-L77
[enable]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp#L42-L62
[ccenable]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L952-L982
[factoryapi]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontextfactory.h#L56-L85
[factory]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontextfactory.cpp#L41-L77
[registry]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L118-L125
[cckeygen]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L1224-L1241
[ccctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L498-L547
[basemut]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-cryptoparameters.h#L150-L162
[baseeq]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-cryptoparameters.h#L201-L218
[rnscompare]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L155-L167
[rlwecompare]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/rlwe-cryptoparameters.h#L570-L588
[rnsclone]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-cryptoparameters.h#L68-L74
[normalwire]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h#L95-L147
[normalextras]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-internal.h#L55-L94
[security]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L105-L162
[normalq]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L164-L205
[singleprime]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L415-L445
[singlefirst]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp#L500-L529
[modreduce]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp#L172-L201
[makescale]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L389-L445
[encode]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L285-L333
[enc64]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L190-L225
[decode]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L336-L405
[scalesetter]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/ciphertext.h#L216-L270
[levelsetter]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/ciphertext-impl.cpp#L38-L54
[ctctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/ciphertext.h#L61-L99
[ctelements]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/ciphertext.h#L199-L214
[ctcheck]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L263-L345
[cryptobject]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptoobject.h#L55-L100
[keygen]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemebase/base-pke.cpp#L45-L98
[hybridgen]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L46-L129
[hybriduse]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L308-L378
[hybridrows]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L381-L434
[skctor]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h#L73-L90
[skset]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/key/privatekey.h#L127-L149
[evalcache]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp#L86-L104
[evalmap]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L242-L245
[evallookup]: https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L1864-L1878
