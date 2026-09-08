# Project parameter map at `a4b815a733efe81897325e2a8e4c826a4ebfa439`

## Scope, identity, and evidence labels

- Read-only source cross-check performed in `/Users/lifeng/Documents/20231788-openfhe-parameter-atlas-20260908`, branch `codex/parameter-atlas-20260908`, against the stated runtime-source baseline. Concurrent root documentation commits advanced HEAD to `1ef9a09`; the worker verified that `src`, `include`, `tests`, `CMakeLists.txt`, and `.github/workflows` remained identical to `a4b815a`. No build, test, encryption, key generation, random sample, FFT execution, network access, or Git mutation was performed by this worker.
- Requested worker selector: `gpt-5.6-sol`, requested reasoning `high`. Actual backend identity is unattested and is recorded as **requested-unverified**; no capability claim is inferred from the selector.
- `PROJECT` below means directly observed in current source/tests/build/workflow at the commit above.
- `PAPER-NOTE` means reported by the repository's clean-room paper cross-check, not re-derived from a paper file in this worktree. The direct paper/PDF/text is absent from this worktree, so those facts require the root/Pro official-source packet for primary confirmation.
- `UPSTREAM-UNKNOWN` means the project delegates behavior to pristine OpenFHE 1.5.0 but this bounded cross-check did not duplicate the planned upstream source survey.
- No conclusion about the absence of a new bug is made here. This is a factual inventory and change-impact map.

## One-page profile comparison

| Knob | Paper/S100 project profile | Experimental S116 project profile | S100 annulus125 experiment |
| --- | --- | --- | --- |
| Factory | `CreatePaperRepeatedMult2Setup()` | `CreateExperimentalPrecision116Setup()` | same paper/S100 factory |
| Production profile object | `kPaperProfile` | `kExperimentalPrecision116Profile` | no new production profile |
| Ring / cyclotomic order | `N=32768`, `M=65536` | same | same |
| Slots / packing gap | `16384`, gap `1` | same | same |
| Family count / squares | `8` / `8` | `8` / `8` (full experiment); single-operation seam also exists | `8` / `8` |
| Base metadata exponent | `50` | `58` | `50` |
| Fresh logical and recorded scale | exact `2^100`; double metadata `2^100` | exact `2^116`; double metadata `2^116` | exact/double `2^100` |
| Root Q | frozen 11-prime paper list | frozen 11-prime candidate list; Q0, Q1, Div changed | paper/S100 Q unchanged |
| Reserved P | `1152921504606584833`, root `4443670208963` | same | same |
| Secret | one signed sparse-ternary root secret with actual weight 128, projected to all families | same construction | same paper/S100 construction |
| Security mode | `HEStd_NotSet`; no project security certificate | same, explicitly `UNRESOLVED` | same, explicitly `UNRESOLVED` |
| Input family | original frozen `1015` dyadic vector | reuses original frozen `1015` dyadic vector | separate deterministic `999` dyadic vector inside radius `125/128` |
| Input RNG | none | none | none |
| Payload encryptions in the named run | one | one | one |
| Numerical gate | component error `<=2^-80`; codec cross-check `<=2^-120` | same | complex-modulus error `<=2^-80`, added error `<=2^-82`, witness gate; observer/producer/Horner agreement `<=2^-120` |
| Registration | expensive target explicitly built, test registered by default | one-op seam registered on explicit target; full eight-square target opt-in default OFF | opt-in default OFF and separate one-shot workflow |

Primary project sites: production profiles and constants are in `src/repeated_mult2.cpp:21-51`; the factories are at `src/repeated_mult2.cpp:443-469`; the original input is at `tests/paper_full_eight_square_oracle.h:97-118`; the annulus input is at `tests/s100_annulus125_eight_square_test.cpp:56-67`; and S116 explicitly reuses `paper_full_test::Inputs()` at `tests/experimental_precision116_eight_square_test.cpp:460-482`.

## Provenance classification

### Paper-facing facts (secondary `PAPER-NOTE` only in this worktree)

`coordination/PAPER_PRECISION_PARAMETER_GATES.md:9-19` reports that paper Section 6.3/Table 3 uses a 100-bit scale, 40-bit Div, 60-bit Mult, `t=2`, `N=2^15`, `h=128`, 11 Q primes, 60-bit auxiliary P, two Base primes of 50 bits, eight repeated squarings, and reports an average over 1000 executions. That note also distinguishes the paper setting from the small N64 diagnostic.

These statements are not promoted to direct-paper observations by this map. The direct paper is not present here, and the root/Pro packet must primary-check the paper's exact notation, prime roles, whether the listed roots/moduli are paper literals or project-selected compatible identities, and the precise statistical claim.

### Project decisions (`PROJECT`)

- The exact Q/P modulus/root values, all context algorithm modes, explicit family construction, exact receipt algebra, high-precision codec, S116 candidate, and annulus125 vector are project code decisions frozen in the sites below.
- S116 is explicitly described as an experimental correctness profile, not E80/security qualification, in `include/openfhe_2023_1788/repeated_mult2.h:58-63` and `src/repeated_mult2.cpp:41-51`. The full S116 test still labels security unresolved at `tests/experimental_precision116_eight_square_test.cpp:610-649`.
- Annulus125 is additive and does not replace the retained original S100 stress test (`tests/s100_annulus125_eight_square_test.cpp:1-2`; `CMakeLists.txt:333-355`).
- The public profile disables security-standard selection with `HEStd_NotSet`; the production validator requires that exact value (`src/repeated_mult2.cpp:133-146`). Therefore no 128-bit security conclusion follows from `h=128`.

### OpenFHE behavior not independently surveyed here (`UPSTREAM-UNKNOWN`)

- Exact PRNG implementation, default/random seed source, sampling algorithms, and how many internal random draws occur inside `DCRTPoly::TugType`, `EncryptZeroCore`, `EvalMultKeyGen`, and scheme `Encrypt`.
- The implementation details behind `PrecomputeCRTTables`, `Relinearize`, `Rescale`, `EvalMultNoRelin`, and the pinned CKKS parameter constructor. The project validates many returned values and dimensions, but those checks are not a substitute for the upstream survey.
- In the small diagnostic `CCParams` path only, `STANDARD`, `HPS`, `FIXED_NOISE_MULTIPARTY`, and threshold `1` are deliberately left to pinned OpenFHE defaults and validated afterward (`src/repeated_mult2.cpp:409-414`, `src/repeated_mult2.cpp:111-159`). In `MakeFamily`, those values are explicit positional constructor arguments (`src/repeated_mult2.cpp:176-191`).

## Exact production profile settings

### Compile/runtime platform envelope

- Project requires `find_package(OpenFHE 1.5.0)` and rejects another reported base version: `CMakeLists.txt:9-13`.
- Workflows pin OpenFHE commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`: `.github/workflows/dcp-rcb.yml:39-70`, `.github/workflows/s100-annulus125.yml:27-30,89-108`, `.github/workflows/s100-annulus125-once.yml:15-18,82-102`.
- Production repeated profiles require `NATIVEINT=64`, `MATHBACKEND=4`: `src/repeated_mult2.cpp:21`; fixed-Q h128 additionally requires `HAVE_INT128` and `MAX_MODULUS_SIZE=60`: `src/paper_h128_client_keypair.cpp:16-20`. Workflows configure `NATIVE_SIZE=64`, `MATHBACKEND=4`: `.github/workflows/dcp-rcb.yml:96-108`; `.github/workflows/s100-annulus125.yml:143-156`; `.github/workflows/s100-annulus125-once.yml:134-147`.

### Frozen S100 Q and P

Production definition (`src/repeated_mult2.cpp:23-40`):

The nominal prime-width pattern is Base `50,50`, Mult `60` repeated eight times, Div `40`, and reserved P `60`; these labels describe integer bit widths, while the exact identities below are authoritative.

| Index / role | Q modulus | root of unity |
| --- | ---: | ---: |
| Q0 / Base | 1125899904679937 | 26113207984 |
| Q1 / Base | 1125899903827969 | 150640639383 |
| Q2 / Mult | 1152921504598720513 | 100545759574150 |
| Q3 / Mult | 1152921504597016577 | 31693996050849 |
| Q4 / Mult | 1152921504595968001 | 88651361085495 |
| Q5 / Mult | 1152921504595640321 | 9679305630873 |
| Q6 / Mult | 1152921504593412097 | 24428769072221 |
| Q7 / Mult | 1152921504592822273 | 18776242964106 |
| Q8 / Mult | 1152921504592429057 | 5821397352863 |
| Q9 / Mult | 1152921504589938689 | 33888991361320 |
| Q10 / Div | 1099510054913 | 121567553 |

Reserved P is modulus `1152921504606584833`, root `4443670208963` (`src/repeated_mult2.cpp:32`). Independent test copies are in `tests/paper_full_eight_square_oracle.h:29-47`; family and key checks consume them in `tests/paper_full_eight_square_contract_test.cpp:50-120`.

### Frozen S116 Q and P

Production definition (`src/repeated_mult2.cpp:41-51`): Q0=`288230191468118017`, root=`43136605093011213`; Q1=`288230165698314241`, root=`82872750907637397`; Q2..Q9 are the S100 Q2..Q9; Q10/Div=`72057589742960641`, root=`50608680790172261`; P is unchanged.

Thus the candidate's nominal width pattern is Base `58,58`, Mult `60` repeated eight times, Div `56`, and P `60`; it is the code comment's `d56-b58` candidate, not a scale-only variant.

The independent test literal copy and the three project-supplied Proth witnesses `{5,7,11}` are in `tests/experimental_precision116_profile_seam.h:31-49`. Structural checks of congruence, roots, basis order, tables, and witnesses are at `tests/experimental_precision116_profile_seam.h:64-118,119-197` and `tests/experimental_precision116_eight_square_test.cpp:87-99`.

### Context, scheme, and precomputation profile shared by S100/S116

`MakeFamily` is the sole production construction site for all eight large families (`src/repeated_mult2.cpp:176-225`):

- geometry `M=65536`, therefore `N=32768`; encoding `EncodingParamsImpl(baseMetadataBits,16384)`;
- sigma/distribution parameter `3.19F`; assurance measure `36.0F`; security `HEStd_NotSet`;
- noise scale `1`; secret distribution `SPARSE_TERNARY`; max relinearization secret-key degree `2`;
- key switching `HYBRID`; scaling `FIXEDMANUAL`; encryption `STANDARD`; multiplication `HPS`; PRE `NOT_SET`;
- multiparty `FIXED_NOISE_MULTIPARTY`; execution `EXEC_EVALUATION`; decryption noise `FIXED_NOISE_DECRYPT`;
- threshold parties `1`; statistical security setting `30`; adversarial queries `1`; interactive boot compression `SLACK`; composite degree `1`; register word size `NATIVEINT`; CKKS type `COMPLEX`;
- multiplicative depth is `Q-count - 1`, hence families 0..7 use depths `10,9,8,7,6,5,4,3`;
- explicit `noiseEstimate=0`, `floodingDistributionParameter=0`;
- `PrecomputeCRTTables(HYBRID,FIXEDMANUAL,STANDARD,HPS,numPartQ=Q-count,auxBits=60,extraBits=0)`. This makes alpha=1: one Q tower per partition and one 60-bit P.
- Scheme components enabled: `PKE`, `KEYSWITCH`, `LEVELEDSHE`.

The constructor call supplies the listed distribution/security/algorithm/noise/execution values through explicit positional arguments (`src/repeated_mult2.cpp:184-187`); subsequent code explicitly sets multiplicative depth, noise estimate, and flooding distribution (`:188-189`), precomputes the CRT tables (`:190-191`), and sets/enables the scheme (`:206-208`). This matters when reviewing defaults: none of `STANDARD`, `HPS`, `FIXED_NOISE_MULTIPARTY`, or threshold `1` is merely inherited in the large-family path. Conversely, fields not named by that constructor signature or a subsequent setter are not silently assigned a project policy by this map; their upstream defaults remain `UPSTREAM-UNKNOWN` unless the returned-state validator checks them.

Returned-state validation repeats the effective contract, including modes/defaults and `GetScalingFactorReal(j)==GetModReduceFactor(j)==2^baseMetadataBits`, at `src/repeated_mult2.cpp:111-159`.

### Family transition and basis roles

- Factory builds eight contexts from the root Q then removes the **second-last** current Q tower after each family (`src/repeated_mult2.cpp:443-451`). Q10/Div is never removed. Resulting Q lengths are 11..4; after DCP removes Div, active pair lengths are 10..3; each RS2 removes that family's last active Mult prime, yielding returned pair lengths 9..2.
- Constructor independently checks each transition by erasing the predecessor's second-last tower (`src/repeated_mult2.cpp:257-277`).
- `GetDivisor()` is always root Q's last modulus (`src/repeated_mult2.cpp:321`). `DoubleCKKS` likewise binds `divisor_` to the last Q and `firstPairModuli_` to Q without that last tower (`src/double_ckks.cpp:255-274`).
- Each family owns distinct context/parameters/scheme/Q/P/QP objects and key tag. The root secret is projected by complete `(modulus,root,cyclotomicOrder)` identity into later family Q bases (`src/repeated_mult2.cpp:351-378`).

### Exact and recorded scale state machine

Let `b=50` for S100/annulus125 and `b=58` for S116; `d=Q10`; family `f`'s rescale prime is its second-last Q, `m_f` (`src/repeated_mult2.cpp:279-310`).

- Initial exact scale: `S_0=2^(2b)/1`; recorded double scale: `R_0=2^(2b)` (`src/repeated_mult2.cpp:279-283,322-326`).
- Tensor/Relin exact scale in family `f`: `T_f=S_f^2/d`; Tensor recorded scale: `R_f^2 / 2^b` (`src/repeated_mult2.cpp:297-305`).
- RS2 exact scale: `S_(f+1)=T_f/m_f`; recorded: `(R_f^2/2^b)/2^b`. Because all FIXEDMANUAL getter values are exactly `2^b`, recorded scale returns to `2^(2b)` after every square (`src/repeated_mult2.cpp:306-310`).
- Nonterminal RS2 is wrapper-reentered into the next family without arithmetic, scale reset, encryption/decryption, or refresh (`src/repeated_mult2.cpp:509-536`).
- The independently closed S100 oracle is `tests/paper_full_eight_square_oracle.h:78-91`; S116's independent closed-product oracle is `tests/experimental_precision116_eight_square_test.cpp:70-85`.
- Evaluator metadata calculations and runtime validation consume the same base/divisor/rescale factors at `src/double_ckks.cpp:241-278,370-395,440-462,502-603,645-696,826-890,1080-1210`.

## Inputs and numerical contracts

### Original frozen `1015` dyadic family (S100 and S116)

For slot `s in [0,16383]`, `t=floor(s/2)` (`tests/paper_full_eight_square_oracle.h:97-112`):

```text
a = 1015/1024 - (t mod 16)/65536 + s*2^-75
b = (1 + floor(t/16) mod 8)/1024
if floor(t/512) is odd: b = -b
phase floor(t/128) mod 4 chooses (a,b), (-b,a), (-a,-b), or (b,-a)
```

No random input generation or runtime input override exists. Conversion to decimal100 client values is at `tests/paper_full_eight_square_oracle.h:114-123`. Consumers:

- original S100 full chain: `tests/paper_full_eight_square_contract_test.cpp:258-281`;
- S100 fresh-only diagnostic: `tests/s100_fresh_error_diagnostic_test.cpp:564-600`;
- S116 one-operation seam: `tests/experimental_precision116_profile_seam.h:300-306`;
- S116 eight-square experiment: `tests/experimental_precision116_eight_square_test.cpp:460-482`.

Independent endpoint code duplicates the exact rational formula and therefore must move with this knob: `tests/paper_endpoint_diagnostics.cpp:284-300`, `tests/paper_endpoint_interop_test.h:20-35`, and `tests/paper_endpoint_sidecar_replay.py:316-329`. Their unit tests include hard-coded input anchors at `tests/test_paper_endpoint_sidecar_replay.py:139-147`.

Original/S116 numerical contracts include:

- full-slot and anchor component error `<=2^-80`, codec cross-precision `<=2^-120`: `tests/paper_full_eight_square_oracle.h:181-195,304-333`; S116 mirrors them in `tests/experimental_precision116_eight_square_test.cpp:323-375`;
- witness delta and final magnitude-domain checks: `tests/paper_full_eight_square_contract_test.cpp:242-249,345-359`; S116 at `tests/experimental_precision116_eight_square_test.cpp:390-418`;
- wrong nominal normalization must err by `>2^-30`: S100 at `tests/paper_full_eight_square_contract_test.cpp:345-348`, S116 at `tests/experimental_precision116_eight_square_test.cpp:549-552`.

### Separate S100 annulus125 family

For slot `s`, `t=floor(s/2)`, the phase/sign construction is identical but `a` uses `999/1024` (`tests/s100_annulus125_eight_square_test.cpp:56-67`):

```text
a = 999/1024 - (t mod 16)/65536 + s*2^-75
b = (1 + floor(t/16) mod 8)/1024
same sign and four-phase rotation rules as above
```

The test sets `radius=125/128`, requires every input's squared norm `<radius^2`, requires `|z^256|^2>2^-20`, and checks the slot0/1 sub-binary64 input witness `|z1-z0|^2=2^-150` (`tests/s100_annulus125_eight_square_test.cpp:242-257`). Fresh scale is exactly `2^100` (`:258-264`).

Its gates are on complex modulus rather than component max: `T=2^-80`; fresh observed, terminal observed, terminal producer errors `<=T`; added terminal error `<=T/4` (implemented squared as `<=T^2/16`); and the slot witness disagreement `<=2T` in norm (implemented squared as `<=4T^2`) (`tests/s100_annulus125_eight_square_test.cpp:283-305`). It emits `security=UNRESOLVED` and retains original S100 FAIL in evidence metadata (`:234-240`).

The annulus vector is entirely test-local. Changing `999`, radius, phase, perturbation, or gates does **not** require changing production profiles, but it does require changing the test's evidence schema/contract label if semantics change, its scalar/replay/finalizer coordination outside this bounded file, and both annulus workflows' reviewed-source assumptions.

## Complete project randomness boundary inventory for the S100/S116 paths

No project call sets a PRNG seed, supplies a seed argument, uses `std::random_device`, `std::mt19937`, or constructs a C++ standard distribution. The `seed` fields in `tests/data/paper_h128_profile.json:50-118` are primitive-root search/certificate witnesses for a small contract fixture, **not runtime cryptographic PRNG seeds**. The table distinguishes calls that are visibly sampling/generating/encrypting from upstream constructors whose possible tag-generation behavior cannot be classified without the intentionally separate upstream survey.

### Production call sites

| Direct site | Purpose | Named-path multiplicity | Upstream randomness details |
| --- | --- | ---: | --- |
| `src/paper_h128_client_keypair.cpp:198-200` `DCRTPoly::TugType` + DCRT constructor weight 128 | one sparse ternary root secret | once per S100 or S116 large setup | `UPSTREAM-UNKNOWN`; project checks signed coefficients and exact weight afterward |
| `src/paper_h128_client_keypair.cpp:200-203` `PrivateKeyImpl(context)` | allocate root secret wrapper and obtain its new key tag | once per large setup | **possible additional upstream randomness boundary**: whether tag creation is random, counter-based, or deterministic is `UPSTREAM-UNKNOWN` |
| `src/paper_h128_client_keypair.cpp:204-210` `EncryptZeroCore(sk)` | derive public key `(a*s+e,-a)` on full Q | once per large setup | `UPSTREAM-UNKNOWN` internal uniform/error draws |
| `src/repeated_mult2.cpp:351-378` `EvalMultKeyGen` | family-local relinearization row | 8 calls per large setup; 2 per small diagnostic setup | `UPSTREAM-UNKNOWN` internal draws; later family secret is deterministic projection, not a fresh secret sample |
| `src/repeated_mult2.cpp:373-375` `PrivateKeyImpl(f.context)` then `SetKeyTag(tag)` | temporary projected-family secret wrapper | 7 constructions per large setup; 1 per diagnostic setup | **possible additional upstream randomness boundary**: any auto-created constructor tag is immediately replaced, but whether construction consumed randomness is `UPSTREAM-UNKNOWN` |
| `src/high_precision_client_io.cpp:606-635` scheme `Encrypt(element,publicKey)` | payload encryption after deterministic exact encoding | one per named S100/S116/annulus run | `UPSTREAM-UNKNOWN` internal encryption draws |
| `src/repeated_mult2.cpp:432-439` `KeyGen` | small N64 diagnostic root keys | once per diagnostic setup | `UPSTREAM-UNKNOWN`; not used to create S100/S116 root secret |

`PublicKeyImpl(context,tag)` at `src/paper_h128_client_keypair.cpp:209` receives the already selected tag explicitly and is not counted as a visible sampling operation. `GenCryptoContext(seedParameters)` at `src/repeated_mult2.cpp:415` chooses the diagnostic Q/root basis from explicit parameter settings; the project supplies no PRNG seed, but this map does not assert whether pinned upstream prime search is stochastic. Both boundaries should be settled by the upstream survey before anyone claims an exhaustive count of internal draws.

### Named experiment composition

- Original S100 full contract creates one small foreign diagnostic setup (`tests/paper_full_eight_square_contract_test.cpp:380-403`), then one paper setup and one payload encryption (`:258-281`). Its visibly stochastic upstream calls are therefore: diagnostic `KeyGen` 1 + diagnostic `EvalMultKeyGen` 2 + h128 sample 1 + `EncryptZeroCore` 1 + paper `EvalMultKeyGen` 8 + payload `Encrypt` 1. In addition it crosses the upstream-opaque private-key-constructor boundaries tabulated above.
- S116 full experiment similarly creates one diagnostic setup (`tests/experimental_precision116_eight_square_test.cpp:569-603`) and one candidate setup/encryption (`:460-482`) with the same call-count structure.
- S116 one-operation seam also wraps its candidate in one live diagnostic setup (`tests/experimental_precision116_profile_seam.h:376-414`).
- Annulus125 creates no diagnostic setup in its `Run`; its visibly stochastic calls are h128 sample 1 + `EncryptZeroCore` 1 + 8 eval-key generations + one payload encryption (`tests/s100_annulus125_eight_square_test.cpp:226-267`), plus the constructor boundaries above.
- S100 fresh-only diagnostic has the same large-setup randomness as annulus125 and one payload encryption (`tests/s100_fresh_error_diagnostic_test.cpp:564-616`).
- `InspectEncoding`, transform tables, input vectors, exact scale oracles, CRT observers, and Horner checks are deterministic and call no RNG (`include/openfhe_2023_1788/high_precision_client_io.h:153-157`; `src/high_precision_client_io.cpp:333-495`).
- None of these named paths contains retry-on-failure or seed-changing logic. Annulus explicitly says no retry/seed change (`tests/s100_annulus125_eight_square_test.cpp:311`); S116's evaluation loop says one operation per round and no retries (`tests/experimental_precision116_eight_square_test.cpp:264-292`).

### Other test-suite randomness call sites (not S100/S116 profile setters)

The broader low-N regression suite directly invokes OpenFHE `KeyGen`, `EvalMultKeyGen`, or `Encrypt` in these files/functions; changing global randomness policy can affect them even when profile constants do not:

- `tests/dcp_rcb_test.cpp:316-320`; `tests/tensor2_test.cpp:209-214,628-630`;
- `tests/mult2_test.cpp:64-79`; `tests/mult2_e2e_oracle_test.cpp:1470-1489,1729-1755`;
- `tests/pair_add_test.cpp:50-57`; `tests/pair_sub_test.cpp:50-57`; `tests/pair_arithmetic_test.cpp:914-939,1030-1075,1289-1293`;
- `tests/precision_dcp_rcb_contract_test.cpp:590-627`; `tests/precision_first_mult2_contract_test.cpp:969-1022`; `tests/precision_client_io_first_mult2_contract_test.cpp:626-649,729-761`;
- `tests/relin2_test.cpp` has repeated negative/shape fixtures rooted at `MakeContext` (`:389-410`) and direct key/encryption/eval-key calls throughout `:415-2931`, plus fixtures at `:3362-3401,4127-4129`;
- `tests/rs2_test.cpp:775-821,848-853,892-971`; `tests/repeated_mult2_semantic_two_square_test.cpp:51-57`;
- `tests/paper_h128_client_keypair_contract_test.cpp:260-279,481-517`.

Additional upstream-opaque private-key wrapper constructions occur at `tests/precision_client_io_first_mult2_contract_test.cpp:549`, `tests/repeated_mult2_semantic_two_square_test.cpp:48`, and `tests/paper_h128_client_keypair_contract_test.cpp:264`; they are not visibly secret-sampling expressions, but their possible automatic tag-generation behavior belongs in the upstream survey. Copy constructors and public-key constructors supplied an explicit tag are not classified here as visible random calls.

`tests/paper_endpoint_evidence_writer_test.h:135-140` uses a steady-clock value only to make a synthetic temporary filename unique; it is not cryptographic or input randomness.

## Setup/key/basis/scale/evaluator/client dependency graph

```text
kPaperProfile (b=50, frozen Q/P) OR kExperimentalPrecision116Profile (b=58, frozen Q'/P)
  -> Data::CreatePaperGeometrySetup(profile)
     -> MakeFamily x8
        -> DCRT Q params + EncodingParams(b,16384)
        -> CryptoParametersCKKSRNS modes/depth
        -> PrecomputeCRTTables(alpha=1, aux=60, extra=0)
        -> distinct CKKS context + PKE/KEYSWITCH/LEVELEDSHE
        -> next family deletes current second-last Q (Mult); Div survives
     -> CreateFixedQH128ClientKeyPair(B0)
        -> one weight-128 sparse ternary secret sample
        -> EncryptZeroCore -> root public key
     -> InstallFamilyKeys
        -> root EvalMultKeyGen
        -> deterministic secret projection by modulus/root/order for families 1..7
        -> EvalMultKeyGen per projected family; seal rows
     -> RepeatedMult2Plan constructor
        -> validate every family/key row
        -> issue exact Input/Tensor/Relin/RS2/Reentry receipt chain
  -> client: HighPrecisionClientIO(plan)
     -> binds B0 context, exact geometry, Q/P/QP/PK/partitions, b and fresh exact scale
     -> deterministic input -> exact integer encoding at 2^(2b)
     -> scheme Encrypt once -> fresh BoundCiphertext
  -> evaluator: DoubleCKKS(plan)
     -> DCP drops Div into (high,low), Input receipt
     -> repeat family f=0..7:
        Tensor2: 3 public no-relin products/add; scale square/base
        Relin2: family eval-key row; raises high with Div; relinearizes; private DCP; combines low
        RS2: RCB; two Rescale calls by m_f; rebuilds low; Rescaled receipt
        if f<7: Reenter wrapper into family f+1, no arithmetic/randomness/client action
     -> terminal RCBWithReceipt: d*high+low and root-context wrapper, absolute level 9
  -> client: BindRepeatedRcb -> exact terminal receipt scale
     -> Decrypt(rootSecret) -> centered coefficients -> high-precision forward transform -> slots
  -> independent test oracle(s): CRT/sparse secret/Horner/full-slot ideal and numerical gates
```

Concrete boundaries: setup/families/keys at `src/repeated_mult2.cpp:347-470`; client construction/encoding/encryption/decryption at `src/high_precision_client_io.cpp:500-635,684-755`; evaluator primitives at `src/double_ckks.cpp:370-462,826-890,894-1077,1080-1282`.

## Runtime selectors and overrides

- There is **no runtime profile parameter CLI** for Q, P, scale, h, ring dimension, slots, modes, or input formula. The factories are closed and accept no caller parameters (`include/openfhe_2023_1788/repeated_mult2.h:58-63`; `src/repeated_mult2.cpp:465-469`).
- CMake opt-ins, all default OFF:
  - full S116 eight-square: `OPENFHE_2023_1788_ENABLE_EXPERIMENTAL_PRECISION116_EIGHT_SQUARE` (`CMakeLists.txt:281-303`);
  - S100 fresh diagnostic: `OPENFHE_2023_1788_ENABLE_S100_FRESH_ERROR_DIAGNOSTIC` (`CMakeLists.txt:305-331`);
  - annulus125: `OPENFHE_2023_1788_ENABLE_S100_ANNULUS125` (`CMakeLists.txt:333-356`).
- The one-operation S116 seam is an alternate CLI mode of `paper_full_eight_square_contract_test`, selected only by exact argument `--experimental-precision116-profile-seam` (`tests/paper_full_eight_square_contract_test.cpp:439-447`) and registered at `CMakeLists.txt:273-279`.
- Annulus executable accepts only `--controls` or `--output NEW_FILE.tsv`; output must not pre-exist (`tests/s100_annulus125_eight_square_test.cpp:226-240,315-323`). CTest fixes its output filename from the configured Git commit (`CMakeLists.txt:343-354`).
- `OMP_NUM_THREADS=2`, serial execution, and 1200-second test timeout are fixed for paper/S116/annulus expensive tests (`CMakeLists.txt:266-302,321-355`). The S100 fresh executable additionally rejects any other/missing OMP setting (`tests/s100_fresh_error_diagnostic_test.cpp:702-713`).
- `dcp-rcb.yml` has one manual workflow input, `s100_scope`, default `fresh`, alternatives `fresh`/`controls-only` (`.github/workflows/dcp-rcb.yml:25-35`); it affects whether the expensive S100 fresh diagnostic runs, not cryptographic parameters (`:179-214`).
- S116 full is enabled only on its historical observation branch in that workflow (`.github/workflows/dcp-rcb.yml:235-259`; Windows mirror `:606-639`).
- Annulus compile-controls workflow enables the option but directly runs only keyless `--controls` (`.github/workflows/s100-annulus125.yml:165-216`). The actual encrypted run is separately tag-gated, attempt-1-only, option-enabled, OMP2, and one-shot (`.github/workflows/s100-annulus125-once.yml:3-26,156-191`).

## Modification-coupling matrix: what must move with each knob

This list names direct consumers and tests that would become stale. It is not permission to change them.

| Knob changed | Production/build sites | Tests/oracles/evidence parsers requiring review/update |
| --- | --- | --- |
| OpenFHE version/commit | `CMakeLists.txt:9-13`; all workflow pin/env/cache/config lines; pin comments in `src/high_precision_client_io.cpp:150,335,619-627,720-723` | hard-coded pins in `tests/paper_full_eight_square_contract_test.cpp:31,380-436`, `tests/experimental_precision116_eight_square_test.cpp:35-36,610-649`, `tests/s100_annulus125_eight_square_test.cpp:234-239`, `tests/s100_fresh_error_diagnostic_test.cpp:710-713`; all structural/behavior suites because constructor/default/PRNG behavior may change |
| native word/backend/int128 | compile assertions in `src/repeated_mult2.cpp:21`, `src/paper_h128_client_keypair.cpp:16-20`, `src/high_precision_client_io.cpp:59-66`; workflow OpenFHE configuration | large-profile tests' static assertions and identity output: `tests/paper_full_eight_square_contract_test.cpp:23,380-383`, `tests/experimental_precision116_profile_seam.h:29`, `tests/experimental_precision116_eight_square_test.cpp:610-649`, `tests/s100_fresh_error_diagnostic_test.cpp:43-44`; independent native-arithmetic oracles |
| S100 Q modulus/root/order or P | `src/repeated_mult2.cpp:23-40`; `MakeFamily`, family transition, precomputation, key projection, scale divisor | `tests/paper_full_eight_square_oracle.h:29-47`, `tests/paper_full_eight_square_contract_test.cpp:50-180`, `tests/s100_annulus125_eight_square_test.cpp:101-151`, `tests/s100_fresh_error_diagnostic_test.cpp:588-614`; endpoint readers/replayers with Q literals: `tests/paper_endpoint_primary_reader.py:11-19,266`, `tests/paper_endpoint_sidecar_reader.py:135-140,289`, `tests/paper_endpoint_sidecar_replay.py:316-329`; their `test_paper_endpoint_*` fixtures |
| S116 Q0/Q1/Div/root or P | `src/repeated_mult2.cpp:41-51`; same family/precompute/key/scale chain | `tests/experimental_precision116_profile_seam.h:31-49,64-197,225-249`; `tests/experimental_precision116_eight_square_test.cpp:60-99,101-223,295-322,461-478`; candidate prime certificates/provenance outside this bounded map must also be regenerated/re-reviewed |
| base metadata exponent `b` (50/58) | profile initializer `src/repeated_mult2.cpp:40-51`; encoding/validation `:111-159,176-191`; scale receipts `:279-326`; client-I/O binding/fresh scale `src/high_precision_client_io.cpp:176-245,500-545`; DoubleCKKS metadata algebra | every hard-coded `2^100`/`2^116`, `EncodingParams(50/58)`, and independent scale product in full/one-op/fresh/annulus tests; endpoint writer/reader/replay scale formulas (`tests/paper_endpoint_evidence_writer.cpp:193`, `tests/paper_endpoint_primary_reader.py:266`, `tests/paper_endpoint_sidecar_reader.py:139`, `tests/paper_endpoint_sidecar_replay.py:63-71,174-175`); low-N precision contracts with S100 metadata |
| family count / square count | reservations/loops and terminal family assumptions in `src/repeated_mult2.cpp:245-311,443-463`; terminal fixed family/operation/level/basis in `src/high_precision_client_io.cpp:521-544,684-700`; terminal wrapper in `src/double_ckks.cpp:1246-1282` | all arrays of 9 scales, loops `1..8`, 32-receipt count, terminal family 7/level 9/two-Q assertions in the full S100/S116/annulus tests and endpoint readers/writers; CTest labels and evidence schemas |
| tower deletion rule / Div position | `src/repeated_mult2.cpp:448-451,273-277`; `src/double_ckks.cpp:255-274,398-437,1080-1210` | all ordered-prefix/tower-count assertions in `tests/paper_full_eight_square_contract_test.cpp:55-180`, `tests/experimental_precision116_profile_seam.h:119-197,225-249`, `tests/experimental_precision116_eight_square_test.cpp:60-85,194-223`, `tests/s100_annulus125_eight_square_test.cpp:101-151`; exact scale oracles and CRT observers |
| HYBRID partition count/aux bits/digit size/key-switch mode | `src/repeated_mult2.cpp:111-159,176-225`; key generation/install `:351-378`; Relin2 validation/operation `src/double_ckks.cpp:894-1077` | full family/key-row checks, S116 `CheckTables/CheckFamilies`, fixed-Q h128 contract, and the dedicated `relin2_*` matrix registered at `CMakeLists.txt:212-242` |
| secret distribution / hamming weight | `src/repeated_mult2.cpp:133-146,176-187,380-398`; `src/paper_h128_client_keypair.cpp:163-217` | `tests/paper_full_eight_square_oracle.h:198-223`, `tests/experimental_precision116_profile_seam.h:198-224`, `tests/experimental_precision116_eight_square_test.cpp:101-126`; `tests/paper_h128_client_keypair_contract_test.cpp`; sparse-decryption complexity/assumptions in full oracles |
| sigma/noise/decryption modes | `src/repeated_mult2.cpp:111-159,176-191`; validation in h128/client I/O; randomness occurs in public/eval/payload generation | paper/S116 structural checks; h128 contract; high-precision I/O contract; every stochastic numerical experiment. Do not merely edit an expected literal because upstream generated distributions and error outcomes may change |
| geometry N/M/slots/gap | `src/repeated_mult2.cpp:67-95,111-159,176-191`; client constants/geometry `src/high_precision_client_io.cpp:17-20,59-64,176-245`; transforms/encoding/decoding | all full-profile test constants, input generator size/phase, anchor powers, sparse CRT loops, client state assertions, endpoint schema/readers; fixed-Q h128 checks; O(N*h) independent decrypt assumptions |
| original `1015` input formula | no production change | every consumer/duplicate listed in the original-input section, plus published scalar anchors/magnitude/witness gates in full S100 and S116 tests; endpoint evidence/replay expectations. S116 currently shares this knob intentionally |
| annulus `999`, `125/128`, or its gates | no production change | `tests/s100_annulus125_eight_square_test.cpp:56-67,226-311`; annulus CMake contract label/filename/labels and both annulus workflows; coordination replay/finalizer/result artifacts must be treated as a new contract, not silently overwritten |
| client numeric precisions 100/160/220 or round margins | `include/openfhe_2023_1788/high_precision_client_io.h:23-24`; `src/high_precision_client_io.cpp:54-57,333-495,740-755` | client-I/O contracts, S100 fresh encoding-inspection controls, full endpoint codec `2^-120` checks, decimal bridges in all full experiments. Changing digits can change ambiguous-half/range behavior even with unchanged crypto parameters |
| error gates (`2^-80`, `2^-120`, witnesses) | no production change | original and S116 full numerical contracts; annulus has different norm gates; endpoint writer/reader/reconcile hard-coded labels and identity fields (`tests/paper_endpoint_evidence_writer.cpp:454-478`, `tests/paper_endpoint_primary_reader.py:33-46,695`, `tests/paper_endpoint_sidecar_reader.py:259-289`) |
| randomness/seed policy | no current explicit seed setter | production direct RNG callsites table above plus every broader test-suite callsite. A deterministic seed feature would be a new API/policy and would require independent review of security/test separation; do not confuse profile JSON witness `seed` with PRNG control |
| OMP threads/timeouts/registration option | `CMakeLists.txt:266-355`; relevant workflows | executable preconditions (`tests/s100_fresh_error_diagnostic_test.cpp:702-713`), one-shot guards, CTest-selection tests, evidence timing/provenance. These are execution controls, not mathematical parameters |

## All context-parameter setter sites outside the large fixed-profile constructor

These are separate low-N tests/diagnostics; they are not alternative ways to configure S100/S116. They matter when a global OpenFHE/default or generic `DoubleCKKS` contract changes.

| Site | Effective explicit profile / runtime knobs |
| --- | --- |
| `src/repeated_mult2.cpp:400-422` | diagnostic prime-generator setup: N64/S16, depth9, scale50, first55, FIXEDMANUAL/HYBRID, 10 large digits, digit0, maxRelin2, UNIFORM_TERNARY, HEStd_NotSet, sigma3.19, COMPLEX, PRE NOT_SET, EXEC_EVALUATION, FIXED_NOISE_DECRYPT, stat30/query1, noise estimate0, SLACK, composite1, register word `NATIVEINT`; STANDARD/HPS/FIXED_NOISE_MULTIPARTY/threshold1 are checked pinned defaults |
| `tests/precision_client_io_first_mult2_contract_test.cpp:290-304` | supported N64/S16 client context: depth7, scale50, first55 by default (56 negative fixture), FIXEDMANUAL/HYBRID, digit0/maxRelin2/largeDigits0, UNIFORM_TERNARY, HEStd_NotSet, COMPLEX, PRE NOT_SET, EXEC_EVALUATION, FIXED_NOISE_DECRYPT, sigma3.19, noise estimate0, desired precision25, stat30/query1, SLACK/composite1/register64 |
| `tests/paper_full_eight_square_contract_test.cpp:203-223`; `tests/experimental_precision116_eight_square_test.cpp:421-443`; `tests/s100_fresh_error_diagnostic_test.cpp:116-137` | duplicate N64/S16 context-only negative/control fixtures with the same supported client profile; no key generation in these constructors |
| `tests/dcp_rcb_test.cpp:254-265` | N32/S8; depth runtime; scale30; first runtime; scaling runtime (FIXEDMANUAL default/FIXEDAUTO negative); HEStd_NotSet |
| `tests/tensor2_test.cpp:173-183`; `tests/pair_add_test.cpp:32-42`; `tests/pair_sub_test.cpp:32-42` | N32/S8; depth3; scale30; first35; FIXEDMANUAL; HEStd_NotSet |
| `tests/pair_arithmetic_test.cpp:410-430` | same N32 core with runtime key-switch, batch, data type; digit0 |
| `tests/relin2_test.cpp:389-410` | N32; depth/batch/key-switch/digit/maxRelin all runtime; scale30/first35/FIXEDMANUAL/HEStd_NotSet |
| `tests/rs2_test.cpp:512-529` | N32/S8 depth3 scale30 first35 FIXEDMANUAL/HEStd_NotSet; runtime key switch; digit0 |
| `tests/mult2_test.cpp:22-60` | N64/S16 depth7 scale30 first35 FIXEDMANUAL/HYBRID, uniform ternary, REAL, HEStd_NotSet |
| `tests/mult2_e2e_oracle_test.cpp:34-49,807-823` | N64/S16 depth7 scale30 first35 digit0/maxRelin2, runtime HYBRID/BV and REAL/COMPLEX, uniform ternary, HEStd_NotSet |
| `tests/precision_dcp_rcb_contract_test.cpp:37-49,444-459`; `tests/precision_first_mult2_contract_test.cpp:38-51,640-655` | N64/S16 depth7 scale50 first55 digit0/maxRelin2, FIXEDMANUAL/HYBRID, uniform ternary, HEStd_NotSet, COMPLEX; S100-scale precision contracts |
| `tests/paper_h128_client_keypair_contract_test.cpp:55-96` and `tests/data/paper_h128_profile.json:1-49` | separate N256/S128, depth2, scale40/first50, 3 primes/largeDigits3, digit0/maxRelin2, SPARSE_TERNARY, sigma3.19, HEStd_NotSet, FIXEDMANUAL/HYBRID, PRE NOT_SET, EXEC_EVALUATION/FIXED_NOISE_DECRYPT, stat30/query1, composite1/register64/COMPLEX/noise estimate0/SLACK; reconstructs constructor defaults and then explicitly sets noise scale1 and flooding0; fixed-Q h128 adapter contract, not the N32768 paper/S116 family |

## Review checklist for a future comprehensive atlas

The root/Pro document should be considered inconsistent with current source if it:

1. treats S116 or annulus125 as paper parameters, or claims either is security-qualified;
2. calls the paper/S100 `50` metadata setting the fresh scale instead of distinguishing base `2^50` from fresh/recorded `2^100`;
3. omits roots of unity, P, alpha-one partition tables, or family-specific key rows when proposing a Q change;
4. says each family has an independently sampled secret; only the root is sampled, later secrets are exact projections;
5. says inputs are randomized or seed-controlled; all three named input families are deterministic and the crypto RNG seed is not exposed here;
6. assumes a scale-only S116 switch; S116 changes Q0, Q1, Div, roots, base metadata, exact scales, and all dependent tests;
7. conflates the annulus complex-modulus gate with the original/S116 componentwise gate;
8. treats recorded double scale as terminal logical normalization; the terminal exact rational receipt is authoritative;
9. overlooks the default-OFF CMake gates and branch/tag/attempt workflow gates;
10. treats project postcondition checks as a primary upstream source survey or as proof of cryptographic security.
