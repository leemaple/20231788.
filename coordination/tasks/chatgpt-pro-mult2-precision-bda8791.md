Independent precision-design task. Attached ZIP: mult2-precision-design-bda8791.zip; 1025333 bytes; SHA-25610fdc5a5d7327eca2ab03c8b9b5cb3778ca69415179277b8dd3c73495ce6e251. All51 payloads plus manifest were freshly verified;30 project files byte-match the named Git commit,16 references are pinned official OpenFHE, and paper plus exact CI logs are supplied. Gitleaks8.30.1 stage/archive checks found no secrets. BV's separate certificate failure also reproduced on Windows42/44; it is handled by another independent task. Focus here on the true precision bottleneck and the next minimal test-first design. Do not assume prior conversation context.

# Double-CKKS genuine precision path: source diagnosis and minimal test-first design

## Objective and supplied context
For the user's clean-room implementation of paper2023/1788 t=2 on pristine OpenFHE1.5.0 (df495ba2e91739a6dc8f1de254fc5a41155ce504), determine the smallest mathematically sound path from the current functional Mult2 chain to genuine high-precision evidence and usable semantics. Do not treat degree2/recorded100-bit scaling as100 bits of information. Do not replace the requested Double-CKKS end state with ordinary low-precision compatibility.
All project/ files are from exact source bda879104c8a8b1ba6ac9301385b5b1919bef440, branch codex/mult2-01. Full paper PDF/text, all CMake source inputs, current numerical evidence, and pinned encoding/decoding/DFT/plaintext/parameter/key-switch references are included. You cannot assume prior chats, local/private file access or an installed OpenFHE environment.

## Observations, not conclusions
- Public DCP/Tensor2/Relin2/RS2/Mult2 and RCB exist. Named Mult2 composition had genuine runtime red and40/40 dual-platform green.
- Current independent coefficient oracle uses cpp_int CRT, secret-derived c0+c1*s+c2*s^2, independent negacyclic product and output pair recombination. No production RCB is used as coefficient expected output. Its first HYBRID REAL case passed both hosts atd414071,41/41 tests.
- N64, batch16, p30, first35, depth7, UNIFORM_TERNARY, degree2 encoding, FIXEDMANUAL, HEStd_NotSet. Seven active Q_l towers,215bits. q_div1073741953, q_l1073741441, logical/recorded ratio1.00000023655603276.
- Corrected logical slot errors were1.70335388852251439e-08 (Linux) and9.57152870503796827e-09 (Windows). Frozen1e-3 threshold only establishes functional behavior, not53/106 bits. Input slots include zero, signs, near-zero; ciphertext/key RNG was not seeded.
- HYBRID COMPLEX later passed at3087ff8 on Linux, corrected max1.3224836354509728e-09. BV REAL/COMPLEX failed an empirical relinearization bound comparison; that is an independent concurrent diagnosis. Current44-test branch is NOT wholly green.
- The current public lifecycle intentionally permits only first multiplication and marks RefreshRequired. This is an interim tested boundary, NOT the full project's final definition of success.
- Paper Section6.3/Table3 uses100-bit Delta, q_div about2^40, per-level q_l about2^60, N2^15, h128, and reports about81.8-bit average error after8 squarings/1000 executions. That is not an IEEE106-bit requirement or a result this project has reproduced. Separate the paper's100-bit scaling from measured precision.

## Concrete source questions
Codex read pinned ckkspackedencoding.cpp lines115-335 and336-493:
1. Native64 Encode performs double FFT, scales inverse coefficients by the depth-one scalingFactor, rounds into integers, and only later multiplies by additional CRT powers for noiseScaleDeg>1. Does degree2 atp30 therefore preserve only roughly30-bit initial rounding accuracy rather than a fresh60-bit encoding? Quantify a counterexample rather than infer information from metadata.
2. Decode has a FIXEDMANUAL pre-scale2^(-p*(degree-1)), double ConvertToDouble/FFT, and the REAL path adds Gaussian noise with a floor tied to sqrt(N)/8 and powP=2^-p. Determine which parts explain the observed1e-8 error and what cannot be inferred from two random samples. Do NOT disable production decryption-noise safeguards or claim independent secret-key diagnostic decoding is a safe production API.
3. Arbitrarily changing ciphertext recorded scalingFactor cannot transparently repair FIXEDMANUAL logical/recorded scale. Audit the exact public contract for RCB->Decrypt and what explicit caller-facing or test-only adapter is truly necessary.
4. Native128 still takes complex<double> and documents52-bit mantissa preservation. Distinguish a wider RNS word,100-bit scale, a53-bit host type, and an actual>53-bit canonical embedding. No accidental claim that Native128 alone supplies100-bit slots.
5. Can the necessary high-precision plaintext coefficients be constructed through existing public pristine OpenFHE plaintext/DCRT interfaces without an upstream fork, private-member access, or a mutable pair factory? If yes, specify/test a minimal project-owned adapter; if not, identify the exact API limitation and smallest decision needed.
6. The current generated context uses homogeneous scaling primes and q_div as the removed final prime. Explain which paper precision regime is possible under this contract, versus what explicit parameter-generation/lifecycle changes would be necessary for Table3's40/60 split. Do not silently equate p50/50 with the paper's exact parameters.
7. Define a justified error/precision acceptance target and experimental progression. Do not conflate coefficient correctness, canonical-slot accuracy, security, non-wrap, relinearization bounds or performance.

## Architecture / non-breakable boundaries
Keep the existing public seams and metadata/format/key/basis validation. DCP removes q_div at level0degree2; Tensor2 omits low-low at same Q_l; Relin2 uses the correct high lift and low key switch; RS2 independently rescales high and recombination by native q_l then corrects low, returning level2degree2. Mult2 composes them, RCB uses q_div*high+low. Logical and recorded scales are tracked separately.
Later RS2 mixed-native-format/declared-basis fixes and Add/Sub are isolated branches and not in this exact source; their reconciliation is a separate integration gate. Do not change them here.
An apparent missing1/q_div in printed Theorem4.8 is an inference, not author-confirmed erratum. Use the exact integral witness in MULT2_SCALE_ALGEBRA_CHECK.md; the earlier ideal5/7 example was not an exact centered-rescale value.
Current empirical certificates explicitly leave conservative_E_Relin unavailable and universal_theorem_gate=UNPROVED. Do not relabel those as universal proof.

## Deliverables
Return a downloadable, hashed ZIP containing:
- PRECISION_DIAGNOSIS.md with exact pinned source/paper lines, observed/inferred/pending labels, ranked bottlenecks, and independent small arithmetic examples.
- DESIGN.md: recommended minimal route plus rejected alternatives/tradeoffs; explicitly separate necessary production API decisions from test-only oracles. Keep the full user's paper implementation as the destination, with first-multiplication and repeated-multiplication gates clearly staged.
- A test-first proposal and, when self-contained within existing public seams, minimal candidate test-only red/probe patches and final changed files. Prefer Boost cpp_int/cpp_dec_float already available through the test dependency for independent high-precision references, not a generic framework. Public API changes are proposals pending Codex/user seam confirmation, not silently adopted.
- A finite experiment matrix: encoding-only coefficient/slot floor; Encrypt/Decrypt baseline; DCP/RCB; first Mult2; then a justified repeated-multiplication/refresh plan. Include exact parameter selections, independent oracle, frozen acceptance criterion before green, repeat count/seeds where possible, and invalid/non-wrap boundaries.
- Specify how to measure true>53-bit accuracy without double inputs/expected values or double-formatted output truncating the evidence. Include representative non-dyadic complex values and exact dyadic/control coefficients where useful; no claim from only trivial zero/constants.
- Commands and claim-to-evidence ledger. Do not fabricate CI or numerical results; if OpenFHE is absent, compilation/runtime remain NOT EXECUTED. Build-only/static checks do not certify precision.
- Risk/decision table: what is proven, what needs hosted experimentation, and which choices materially affect the user-visible contract.

## Tests and prohibitions
Genuine tests use pinned pristine OpenFHE on GitHub Actions/Linux and Windows MinGW64, warning-as-error, at most2 build threads. No OpenFHE or crypto execution on the user's Mac. The actual current commands are cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<pristine-install>; cmake --build build --parallel 2; explicit public API targets; ctest --test-dir build --verbose --output-on-failure.
No old implementation, modified local OpenFHE, credentials/browser state, external messages, git push/merge/CI dispatch, production security-guard removal, benchmark/security claim from HEStd_NotSet, tuple t>2, speculative frameworks, blanket try/catch, or silent normalization of expected answers. KISS/YAGNI/TDD.
Primary scope is diagnosis/design and bounded test-only candidates; BV E_Relin certificate correction is another task, so report necessary interactions but do not independently overwrite that correction.
