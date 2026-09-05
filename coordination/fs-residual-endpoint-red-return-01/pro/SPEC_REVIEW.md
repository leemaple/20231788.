# Independent specification review — FS-RESIDUAL-ENDPOINT-01

## Verdict and scope

**SPEC_READY for the minimal API/link RED, subject to Codex's adoption.** This is not numerical GREEN, not an observed RED, not a certified floating-point implementation, and not final acceptance of original E80. No production correction is justified or included. The corrected ENDPOINT_SPEC.md is the single normative candidate; it does not rely on reconciling the two supplied proposals later.

There is no unresolved blocker to *authoring this declaration/assertion-only RED*. There remain expressly unproved numerical-library premises and unexecuted compiler/runtime gates. Those prevent claiming unconditional numerical readiness, but do not require inventing a library proof or implementing GREEN before retained hosted RED. The numerical contract deliberately labels any later passing observer result CONDITIONAL. A certified-library requirement would require additional exact dependency closure and proof, not a relabeling of this package.

Independence here means this review inspected the supplied inputs and authored its own reasoning, scalar/static checks, specification and tests without using personal memory, a private checkout, an earlier task as authority, or returned archive scripts as executable code. It does not establish a separate provider, model backend attestation, or an additional externally independent reviewer seat. Codex owns adoption and hosted execution.

## Input identity and current evidence

The outer archive matches 2,125,153 bytes and SHA-256 `c046efc8fc95ced2e68da941dab02252d6ac76592e619571d6eb82dfc092ad74`; its manifest matches `92b2f1dcfd51620c2bc887292acdc40a10c3aacc0e7b47a54240c012cb3d6fed`. All12 regular members pass safe relative/case-unique path, no-link/no-encryption and CRC checks; all11 non-self manifest payloads match. The source nested ZIP matches its2,046,500-byte identity, all154 members and153 manifest payloads. The decision nested ZIP matches its60,905-byte identity, all9 members and8 payloads. These are direct byte checks, not source authenticity inferred from names. See STATIC_SCALAR_RESULTS.json and SOURCE_MAP.md.

The complete supplied paper text was read, its15-page PDF rendered, and physical pages7,8,12,13 visually inspected. Its SHA is `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`. All three integration contracts, current acceptance scope, TEST_SEAMS, both complete paper test/helper files and the relevant client/evaluator/dependency code were inspected. A file's presence/hash is not represented as a semantic audit of every unrelated function in all77 official files. The historical nested TASKs were not adopted as current instructions.

An independent parser of the primary `61:` stream finds one BEGIN/COMPLETE FAIL per host, nine scale receipts,835 named observations, seven retained finite E80 misses, and the cleanup receipt. The terminal E/2^-80 ratios reconstructed from the printed decimals are **10.416873724120** on Linux and **10.262604700037** on Windows. The unprefixed CTest replay is not a second chain. The supplied historical94,722-byte checker JSON and241-byte stdout were hash-checked, not re-executed. [SL:7046,7940–7942,8022–8026; SW:7368,8262–8264,8344–8348]

## Findings and resolutions

### FS-01 — Conditional arithmetic estimates must not become a library certificate

**Priority P1; resolved by explicit qualification and fail/unresolved rules.** The allowance proposal gives a useful fixed graph and generous constants, but pi<=u and absolute sin/cos<=8u remain assumptions. The supplied trig series' stopping rule and multiple-angle reconstruction do not constitute a proved8u absolute error bound. The provided nearest-rounding and division code supports source compatibility with the stated arithmetic model, not a complete transitive proof of conversion/constants/transcendentals. Cross precision alone cannot exclude common error. [AP:7–19,35–44; BOOST:618–716,1110–1279,1972–1987,2029–2030; TRIG:25–76,159–350]

An additional concrete mismatch is that the Windows log installs **Boost1.92.0-3**, while the supplied reference files are1.83.0. The workflow installs the rolling MSYS package. A statement that this Windows run validates pinned1.83 behavior would be false. ENDPOINT_SPEC requires the actual BOOST_VERSION receipt and a named CONDITIONAL model; unsupported graph/type/range gives UNRESOLVED. Missing exact dependency paths include `boost/multiprecision/cpp_bin_float/io.hpp`, `.../transcendental.hpp`, Boost.Math constants support and `cpp_dec_float.hpp`/number support. They are an input gap for a complete library proof, not a missing production implementation silently supplied from elsewhere. [SW:359–375,514–515; WF:204–215]

### FS-02 — Horner's integer conversion is a separate premise

**Priority P1; resolved as a runtime prerequisite without altering Horner.** The unchanged Horner path first uses `R(Int)`, which constructs Real from a decimal string, then accumulates unscaled coefficients and finally applies the inverse rational scale. It is not the proposed observer's exact integer-product/rounded-division input graph. Applying a conversion bound merely because the target is binary512 is insufficient. [O:52,301–313]

The corrected specification requires exact represented-integer conversion checks and exponent-range checks on the actual Horner inputs, preserves the old code/precision, and uses its own H_512 allowance. Producer transport is separately bounded and qualified; the producer's algorithm is not presumed exact. The RED's binary768 exact-difference fixture distinguishes exact dyadic comparison from a rounded subtraction that loses a low bit.

### FS-03 — Canonical decimal language was not canonical

**Priority P1; corrected and covered by RED fixtures.** The proposal's `[0-9]` leading-digit pattern accepts nonnormalized nonzero mantissas beginning0. Without a separate rule it also allows many zero/exponent spellings. [EP:20–31]

The replacement is leading1–9 for nonzero, exactly110 significant digits, explicit sign, lowercase e, exactly5 exponent digits, prohibition of e-00000, and one positive zero spelling. Exact nearest/ties-to-even rounding and carry normalization are specified independently of stream formatting. C++ and Python validation are separate. The RED asserts zero/negative-zero normalization, signed units,1/2, two exact half-even integer ties, valid spellings and15 malformed spellings. The executed Python check verifies only lexical fixtures and exact integer tie arithmetic, not a numeric formatter or codec.

### FS-04 — Comparison arithmetic needs its own boundary rule

**Priority P1; corrected and covered by RED fixtures.** A high-precision subtraction followed by a rounded comparison is not automatically exact at2^-120 or2^-128. Nor is producer discrepancy bounded by observer roundoff alone. The corrected contract compares represented values by exact dyadic/rational extraction, distinguishes two-bounded-path integrity from producer comparison, rejects raw excess, detects model contradictions, and marks threshold overlap or an excessive allowance UNRESOLVED. [AP:75–92; O:181–195,315–334]

The RED fixes both equality and adjacent cases, an over-ceiling case, unsupported-model cases, and a raw failure that must not be hidden by lack of model support. Old E80 predicates are not replaced by this classifier.

### FS-05 — Serialization error can be amplified on scalar replay

**Priority P1; corrected numerical specification, implementation deferred.** The110-digit sidecar is ample only after accounting for the fresh-error reconstruction and power map. A blanket claim that serialization is negligible, combined with the deliberately loose radius-2 factor2^263, is not a2^-128 proof for all accepted near-unit endpoints. The proposals did not freeze this replay bound. [AP:48–73; EP:20–31,79–91]

ENDPOINT_SPEC keeps110 digits, adds exact per-field decimal quantum bounds, checks both exact and represented replay fresh values in the3/2 disk, uses256-digit Decimal arithmetic and the fixed derivative256*(3/2)^255<2^158, and adds replay allowances to live allowances. The integer derivative inequality was checked here. This is bounded scalar reconstruction of the same evidence, not another chain or a transform. No packer/replay implementation is included in RED.

### FS-06 — External hashes and compression identity require a finite DAG

**Priority P1; proposal correction retained and made complete.** The old next-test draft requested sidecar hash/size in its own metadata; the new evidence proposal correctly moves them outside. The final contract expressly prohibits both canonical self-hash and status self-hash/size, defines actual gzip and canonical hashes externally in status, and permits a later external receipt to hash status. [NEXT:64–85; EP:75–91]

Canonical ASCII/LF/order rules, exact identity fields, all16384 rows, bounded parsing,24 prescribed check receipts, single-member gzip header/trailer rules and no cross-zlib compressed-byte identity claim are now frozen together. The four-row illustration is never live evidence. Synthetic fixtures have a separate disposable namespace and cannot be uploaded as live-chain artifacts. RED contains no sidecar, packer, hash/status writer, gzip producer or upload implementation.

### FS-07 — Preserve evidence before the original finite-failure exit

**Priority P1; specification resolved, GREEN work intentionally deferred.** Existing code counts only finite numeric misses, completes the later controls/cleanup, prints the count, then throws at the final Require. Publishing afterward would lose expected-failing evidence. Simply making an upload always-run does not fix failure before file closure. [O:134–141; T:368–394; SOL:11–12; EP:79,124–126]

The specified future insertion closes/reopens/validates canonical evidence after cleanup and before the unchanged Require. A later wrapper retains the actual CTest status, finalizes evidence once even when it is nonzero, and then propagates failure. Only the exact gzip/status identity paths are uploaded; missing/fatal/timeout evidence yields explicit incomplete status or job failure, never PASS. None of those implementations is in this RED. The current normal path remains byte-identical apart from its pre-Run dispatch.

### FS-08 — Full-slot DFT direction, slot selection, conditioning and cost

**Priority P2; source-compatible mathematical plan with unmeasured runtime.** The positive unnormalized twist/DFT evaluates the integer polynomial at xi^(5^s). Selecting ordinary bins0..16383, using the supplied generic negative-forward FFT, or silently reusing producer special-transform roots would be wrong. Exact modular arithmetic independently confirms a permutation of all even bins and disjoint conjugates. [PAPER:213–247; ODFT:96–161,241–268; O:283–314]

The fixed DFT/Horner estimates use the actual coefficient one-norm, not a coefficient maximum. Conditional scalar majorants for D=2^12*u*K, H=2^24*u*K, P=2^270*u, and the residual envelopes were checked without a transform. K examples are explicitly synthetic. The formal terminal K cap<=2^14 does not supply the unknown actual fresh C. Direct same-precision immutable table reuse and separate direct768 sparse reference are required. Mantissa-only table lower bounds21 MiB and262144 direct trig calls show plausible scale, not a1200-second runtime guarantee. Existing timeout remains unchanged.

### FS-09 — No remove-d patch and no A-based replacement acceptance

**Priority P1; rejected alternatives, frozen boundary retained.** Visual inspection confirms the Theorem4.8 isolated display differs from its constructive Lemma4.2 and scale paragraph. Current source omits low×low in Tensor2, performs the two distinct RS2 rescale calls, recombines d*high+low, and binds independent S8. Nothing here justifies deleting d. The sufficient nonwrap hypothesis is not instantiated; failure to establish it is not algorithm failure. [PAPER:608–660,903–960; PDF pp.7–8; DC:826–894,1080–1285; RP:254–285; IO:645–716]

The old decision's proposed A80 criterion is not adopted. The supplied primary evidence continues to fail original E80. Added full-slot A is diagnostic characterization only, and cannot establish all intermediate slots, all keys, a security level, or the paper's empirical average. No trial quota or1000-run obligation is restored. [PC:147–174; SCOPE:1–35; ROOT:17–43]

### FS-10 — Minimal honest RED and exact source preservation

**Priority P2; ready for hosted observation, not observed here.** The candidate changes one existing source and adds one narrow test-contract/fixture header. All seven helper declarations deliberately have no definitions. The normal no-argument paper target references the self-test branch at link time, so the intended failure is unresolved test-local symbols at paper-target linkage, after the unchanged legacy/API checkpoint. No stub, expected unsupported exception, implementation header, public production API, CTest entry, workflow change or CMake change is present.

Actual `git apply --check` and disposable `git apply` returned0. The applied files equal the provided complete files byte-for-byte; all43 other supplied project files remain identical, including all production code, old oracle, CMake, workflow, legacy/API tests and contracts. The original normal test body and completion catch are also byte-identical. This is patch/static evidence, **not** C++ compilation. Earlier unrelated compiler failure must not be relabeled as the expected RED. [CM:43–69,133–139,261–266; WF:147–158,387–412; STATIC_SCALAR_RESULTS.json:patch]

## Disposition matrix

| Question | Result |
|---|---|
| Supplied archive identities, safe paths, CRC and exact manifests | PASS, directly checked |
| Minimal patch applicability and changed-path preservation | PASS, directly checked |
| Integer slot map, rational majorant inequalities and synthetic grammar fixtures | PASS_STATIC_SCALAR_ONLY |
| Numerical-library accuracy premises | CONDITIONAL / not certified |
| C++ compilation or observed API RED | NOT RUN |
| New full-slot observer/control execution | NOT RUN; definitions absent |
| New encrypted chain or Linux/Windows CI | NOT RUN |
| Original recorded E80 | FAIL retained on both supplied runs |
| Replacement A acceptance | NOT_ADOPTED |
| Final scientific/project acceptance | NOT CLAIMED |

The complete executable specification and future test order are in ENDPOINT_SPEC.md and TEST_PLAN.md. Exact base/result identities are in BASE_HASHES.json. No ordinary technical adoption decision is requested from the user; Codex owns the next retained RED and later GREEN authorization.
