# PUBLIC-S100-ENCODER-CAP-01 independent static candidate review

Disposition: **CHANGES REQUIRED before the one-shot full public encoding**. One blocking build-provenance defect, two parser robustness defects, and one defective algebraic containment assertion are identified below. The integer interval transform is mathematically sound on its validated domain by this static review; no actual public polynomial or cap certificate has been produced or verified by this reviewer.

## Scope, identity, and first-pass requirements

Reviewer: independent Codex context `public_encoder_candidate_review`; requested selector GPT-6 Astra / high, backend identity **requested-unverified**. This is not an additional provider-diversity claim. Workflow and model-routing reference were read directly. Worktree observed at `/Users/lifeng/Documents/20231788-openfhe-initial-lift-nonwrap-20260909`, branch `codex/initial-lift-nonwrap-20260909`, HEAD `ae5686c26c1618ff47b3e0c0b0acf2d3a221b6ee`. Existing untracked intake/return/replay files were preserved. No AGENTS.md was found within this worktree.

Before reading the author's design/verdict, the reviewer reconstructed these acceptance requirements from the independent task brief and source:

1. Obtain the exact signed integer polynomial returned by the existing deterministic public encoder for the ORIGINAL fixed near-unit input, with N=32768, 16384 slots, gap=1, S=2^100 and the original ordered eleven Q/root pairs.
2. Make no context/setup/key/sampler/encryption/decryption call. A value-only diagnostic may reuse the encoder, but must not masquerade as a valid production binding or receipt.
3. Serialize all coefficients exactly; independently bind the bytes to source, input construction, numerical dependencies, toolchain and actual execution. JSON labels alone are insufficient provenance.
4. Enclose every odd 2N-th-root evaluation divided by 2^100 with outward-rounded arithmetic, including conjugates. Compare the maximum squared modulus with exactly 16129/16384. Distinguish certified cap, exceeded cap, and an interval that leaves the answer unknown.
5. Do not change the input, threshold, scale, parameters or noise. A result for this public polynomial applies to a historical sample only with relevant provenance equivalence. Original S100 full-eight-square E80 remains FAIL; S116 and annulus results do not repair that contract.

Reviewed the entire candidate Python verifier, four tiny transform tests, candidate C++ files/header/CMake, RED/GREEN patches, BASE_BINDING, author scalar/static check source, design and NEXT_ACTION. Compared candidate full C++ and CMake against the permitted clean-room worktree using read-only textual diffs. Consulted only the permitted pristine OpenFHE 1.5.0 mirror at `.../parameter-atlas-openfhe-df495ba2` for relevant storage and PRNG definitions. No other pre-existing implementation or modified OpenFHE tree was inspected. No candidate script, code, transform, FFT/NTT, sampler, build, test or CI was executed; no browser/external-agent action or Git mutation occurred. This report is the only reviewer-owned edit.

## Findings

### F1 — P1: CMake supplies an empty build-source identity

Location: `pro/candidate/full_files/CMakeLists.txt:364`; identical defect in `pro/candidate/01-red.patch:112`. The existing source identity is populated as `LOSSLESS_IO_SOURCE_COMMIT` at CMake lines 111–115. The new target instead expands `${OPENFHE1788_SOURCE_COMMIT}`, which is never assigned in this CMake file. Under the documented configuration (enable only the diagnostic option), it therefore defines the C++ macro to an empty string. The driver's fallback at `public_s100_encoding_dump.cpp:9–11` cannot help because the macro is defined. The driver encodes the full original input at lines 48–49 and writes the empty string at line 58; the verifier then rejects it at `certify_public_encoder.py:206–207` because it is not forty lowercase hexadecimal characters. Thus the documented workflow spends its sole valid encoding before a predictable provenance rejection. `NEXT_ACTION.md:17` incorrectly states that the original CMake mechanism already supplies this field.

Minimal correction: populate the macro from the existing `LOSSLESS_IO_SOURCE_COMMIT` value, and validate the actual compiled metadata before the first full encoding. Do not work around this by passing an arbitrary source label or editing the exported JSON. Root must still bind the commit plus uncommitted patch/tree hashes and dependencies; a correct label is necessary but not sufficient.

Required discriminating checks: configure without manually defining `OPENFHE1788_SOURCE_COMMIT`; inspect the generated target definition or an encoding-free binary metadata mode and establish that it equals the actual build source commit. An empty/unknown/malformed identity must fail before `ComputeEncoding`. Preserve the intended missing-definition RED and GREEN/API-negative receipts on Windows/GitHub.

### F2 — P2: The advertised input-size bound is checked after an unbounded read

Location: `pro/candidate/certify_public_encoder.py:237–240`.

`Path.read_bytes()` consumes the entire supplied file before `len(raw)>12_000_000` is checked. A mistaken or malformed oversized artifact can allocate arbitrarily large memory, exhaust the process and prevent the promised controlled rejection. This does not falsify a mathematical certificate, but it violates the stated bounded parser and can disrupt the execution host before any validation.

Minimal correction: open the input and read at most the limit plus one byte, reject an over-limit read before decoding/parsing, and retain the exact accepted bytes for hashing. For the documented complete-file contract, reject non-regular input if the root launcher does not already enforce that boundary.

Required check: an oversized file must be rejected through the parser-only path without allocating its full contents and without invoking `canonical_bounds`. Test the exact size boundary and LF/CR rejection as well. No full encoding or transform is needed for these checks.

### F3 — P2: Deeply nested malformed JSON escapes the documented rejection outcome

Location: `pro/candidate/certify_public_encoder.py:240`, with exception translation at lines 259–262.

A JSON array/object nested beyond Python's decoder recursion limit can fit well within 12 MB but raises `RecursionError` in `json.loads` before schema validation. The current exception tuple does not catch it, so the CLI emits a traceback and a generic failure exit instead of the documented `PUBLIC_CAP_REJECTED` / exit 4 (`NEXT_ACTION.md:50`). No invalid certificate is accepted, but the orchestration contract for malformed input is not upheld.

Minimal correction: enforce a bounded JSON nesting policy or translate decoder `RecursionError` at this parsing boundary into the same controlled rejection. Keep the schema fixed; there is no need for deeply nested values in this format.

Required check: deeply nested but byte-bounded JSON must produce exit 4 with no certificate and zero transform calls. Also exercise duplicate fields through the real JSON decoder, truncated JSON, invalid UTF-8 and the fixed-field type checks; the existing direct `no_duplicates` helper call does not exercise the complete CLI path.

### F4 — P2: The algebraic-root test checks overlap, not certified containment

Location: `pro/candidate/tests/test_transform_models.py:30–33`. Root drew attention to the exact direction of these inequalities during review; the reviewer independently reconstructed the following counterexample without execution.

Let `L=10+5*t/D` and `U=10+5*(t+1)/D`, so the exact target alpha=`10+5*sqrt(2)` satisfies `L<alpha<U` because sqrt(2) is irrational. The current assertions are `a<=U` and `b>=L`, which establish only that `[a,b]` overlaps `[L,U]`. A false zero-width result `a=b=U` passes those assertions, also passes the narrow-width check and `a>15`, but excludes alpha. This is a concrete logical defect in the test's claimed rigorous oracle, not merely a missing test case. It is not evidence of a defect in `canonical_bounds` itself.

Minimal correction: establish the sufficient containment conditions `a<=L` and `b>=U` using the same exact rational square-root enclosure, or use another rigorous algebraic containment predicate. This changes assertions only and preserves the existing four-transform budget. Verify that a deliberately false point interval at U is rejected by the oracle, using a scalar-only check; then run the corrected model with the other three tiny transforms on the authorized execution host.

## Mathematical review

The following conclusions are static inferences, not runtime receipts.

- `Interval.rational`, multiplication and division use Python floor division and explicit ceiling correctly even for negative numerators (`certify_public_encoder.py:37–81`). Multiplication considers all four endpoint products. Squaring uses zero when the input straddles zero. Complex rectangle arithmetic preserves inclusion; dependency loss only widens the result.
- The 96-term reciprocal-arctangent series and its adjacent partial sum enclose atan(1/5) and atan(1/239). Machin's identity uses the correct subtraction direction, and the outer conversion to the 2^-224 grid is outward (`109–124`). The identity's branch is justified by the small positive angles in the design. A rough numerical interval such as (3,22/7) alone would not prove the pi enclosure; here the alternating-series argument supplies it.
- The cosine polynomial ends at degree 62 and can be treated as the degree-63 Taylor polynomial because its degree-63 coefficient is zero. Its Lagrange remainder is bounded by |x|^64/64!. The sine polynomial ends at degree 63 and is also its degree-64 Taylor polynomial, giving |x|^65/65!. The fixed |x|<=1 guard covers pi/n for every permitted n>=4. Thus the apparently skipped orders in `127–138` are correct.
- With seed exp(i*pi/n), the twist multiplies coefficient j by seed^j. The positive-exponent radix-2 DIT then computes `sum_j (p_j/S) exp(i*pi*j/n) exp(2*pi*i*j*k/n)`. No 1/n normalization belongs in this forward polynomial evaluation. At a stage of length L, the maximum requested twiddle index is `(L/2-1)*2n/L = n-2n/L < n`, so the root table is sufficient (`141–169`).
- Outputs cover exactly all odd 2n-th roots. For real integer coefficients, conjugates have equal exact modulus; explicitly enclosing all n roots avoids an assumption about production slot order or projection. The full canonical maximum is consequently addressed rather than selected slots.
- Taking the maximum of per-root lower squared-modulus bounds and the maximum of their upper bounds encloses the maximum itself. The PASS condition uses upper<=cap, REFUTED uses lower>cap, and all remaining cases are UNKNOWN (`170–179`). There is no square root or floating-point threshold decision.
- The coefficient magnitude guard and bounded n keep accepted arithmetic finite. Width growth from recurrence and FFT may cause UNKNOWN, but does not create false certification. Actual width and cap outcome remain unobserved.

## Public encoding, provenance and patch boundary

`public_s100_encoding_dump.cpp:48` uses the unchanged `paper_full_test::ClientInputs(paper_full_test::Inputs())`. The permitted `tests/paper_full_eight_square_oracle.h:97–119` contains the original deterministic slot formula and binary512-to-decimal100 conversion; the driver does not invoke that header's secret extraction, Horner oracle or encryption helpers. This source path, not merely the string `original-s100-near-unit-v1`, identifies the input.

The new diagnostic function (`high_precision_client_io.cpp:760–775`) rejects wrong shape before root-table construction. `ContextBinding binding{}` initializes a value aggregate containing empty shared pointers; it does not instantiate `CryptoParametersCKKSRNS` or a crypto context. The full-basis constants match the original test header, and the no-plan `FreshExactScale()` branch returns exactly 2^100 (`504–507`). The reused encoder reads geometry, fullBasis and that scale on this path. It computes the same primary/check transforms and stable rounding and returns the actual pre-residue coefficients (`463–496`). It does not call `BindContext`, `ValidatePlan`, `Encrypt` or `Decrypt` on the diagnostic path.

The pristine upstream `src/core/include/math/hal/bigintdyn/mubintvecdyn.h:108–109` constructor used for residues only copies the modulus and initializes vector storage; `ubintdyn.h:141–160` gives deterministic zero storage/default construction. The pristine PRNG's global state starts null (`src/core/lib/math/distributiongenerator.cpp:48–53`) and construction occurs lazily inside `GetPRNG` (`93–109`), which this path does not call. No hidden sampling was identified in the reviewed call path. This remains a source-path conclusion subject to actual build/library provenance, not a runtime counter attestation.

The read-only diffs show that the full candidate C++ differs from the current permitted production source only by the new include and diagnostic function, matching the GREEN patch; CMake differs only by the nine-line opt-in target block, matching the RED patch. Existing `ComputeEncoding` text is unchanged. The new header and driver text agree on inspection with their RED patch additions. Root owns cryptographic hash and patch replay receipts; this review does not claim to have executed an independent replay. The target is OFF and EXCLUDE_FROM_ALL and has no new CTest registration. The callable probe symbol itself is compiled in the library after GREEN even when the executable option is OFF; this does not cause execution by itself.

The output decimal strings are exact signed integers, the parser rejects wrong dimensions/scale/basis, noncanonical strings and changed fixed scalar types, and the result binds both full JSON bytes and the newline-delimited coefficient stream by SHA256. The parser cannot authenticate that coefficients came from the original encoder: a synthetic zero polynomial with matching labels is deliberately accepted by the scalar helper test. The design and result explicitly reserve source adoption for a separate root receipt (`certify_public_encoder.py:249–252`, `NEXT_ACTION.md:15–19`), which is appropriate. That gate must be enforced before attaching a cap status to the original source or historical chain.

The driver's output-existence check is not atomic with its later file open, as the design already acknowledges. This is acceptable only for a root-controlled unique output path with no concurrent writer; it is not a concurrency or one-shot security mechanism. GCC/MinGW's `__VERSION__` is the documented scope; MSVC compatibility is not established. Neither limitation is evidence of a numerical defect.

## Test adequacy and remaining acceptance work

The four unrun tiny transform models are meaningful sanity checks: zero detects spurious noise; X has unit modulus; `[3,1,1,2]` at N=4 has maximum squared modulus 15; `[-2,1,1,2]` has maximum `10+5*sqrt(2)` and challenges coefficient-centering intuition. The first N=4 identity follows directly by evaluating at exp(i*pi/4), where `(3-r)^2+(1+3r)^2=15`, r=sqrt(2)/2. The second maximum follows as `(-2-r)^2+(1+3r)^2=10+5*sqrt(2)`. These supply independent algebraic oracles instead of comparing with the production FFT.

Coverage gap: all nonzero transform examples are far above the cap. Neither the exactly-equal `<=` boundary nor an interval straddling the cap (UNKNOWN) is exercised, and the helper scalar checks do not cover that classification code. Before accepting the three-way contract, add a small scalar classification test against exact enclosures below/equal/above/straddling 16129/16384, ideally through a factored helper used by the real verifier. This can preserve the four-transform budget. If root chooses additional transforms instead, record the bounded plan revision before running them. Do not silently expand to large statistical testing.

The scalar author tests check rational endpoint containment but their positive pi/sincos inequalities are coarse sanity checks; the proof arguments above, followed by the actual tiny transform receipts, provide the needed additional basis. The current static checker confirms source/patch consistency but misses F1 because it never checks whether the CMake value being substituted is defined. The only C++ runtime negative checks empty shape and cannot discover an invalid compiled build identity.

Acceptance remains pending: fix or explicitly dispose of the findings in an independently owned successor without altering the immutable Pro return; obtain actual RED/GREEN and parser/tiny-model receipts; audit exact source/input/dependencies/toolchain and output path before the one full encoding; run one full outward interval transform on its exact bytes; interpret only its certified/refuted/unknown result. No cap success is asserted here. The original S100 E80 FAIL remains unchanged regardless of this static review or any later public cap certification.
