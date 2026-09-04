# Production I/O API: fresh source recheck

## Result and review boundary

**No blocking factual or citation defect found in the 25-source / 51-reference audit.** The four requested I/O conclusions are supported. One nonblocking wording refinement is recorded below; the audited document was not edited.
This is a fresh, separately requested read-only pass by the same agent that authored the audit, not an independent third-party reviewer endorsement. It cannot substitute for compile, cryptographic execution, precision, transport-roundtrip or security validation.
Date/context: 2026-09-04, Asia/Shanghai; /Users/lifeng/Documents/20231788-openfhe-codex-precision-01, codex/precision-01, HEAD 5cd9f37ca1361d22879aab1845b92341e4cfc34b at entry. Existing untracked audit/ZIP were preserved. Only this recheck note is written by this subtask.
Project workflow, engineering and research instructions were read. All source came from the permitted pristine official directory, the verified 53-source ZIP or the new six-source ZIP. No old implementation, installed/local OpenFHE, common/project, network, build, crypto execution, external agent, stage, commit or push was used.

## Exact object reviewed and evidence closure

- Audited [PRODUCTION_IO_OFFICIAL_API_AUDIT.md](/Users/lifeng/Documents/20231788-openfhe-codex-precision-01/coordination/PRODUCTION_IO_OFFICIAL_API_AUDIT.md): 36,445 bytes, 159 split lines; SHA-256 234ee8b8ff5f44e04e12e2434f02d37e1e0dabc2ae1aa04e3970949abc69365c.
- Official pin for every upstream citation: df495ba2e91739a6dc8f1de254fc5a41155ce504.
- Recomputed complete raw-byte identities for all 25 listed files (666,514 bytes): 25/25 sizes, SHA-256 and Git-blob SHA-1 values match the audit table. Fourteen Z and six N files also match their permitted archive provenance records. Five P files match the explicitly supplied official files; this pass did not independently authenticate them through a new upstream fetch.
- All 51 reference definitions resolve to listed source files and valid exact-pin line ranges; all 81 reference-style uses resolve. No duplicate definition, missing use target, unused definition or out-of-range anchor was found.
- Every one of the 51 cited ranges was freshly extracted and read, not merely regex-checked. Public-access sections and virtual dispatch were also checked; two existing declaration headers used for that check are identified below.
- All four local-file byte/hash rows match the current clean-room files. Relevant placeholder-cache, approximate-scale and all-coefficient Horner excerpts were reread; no local test primitive was treated as a production implementation.
- Existing [six-source ZIP](/Users/lifeng/Documents/20231788-openfhe-codex-precision-01/coordination/evidence/production-io-api/official-io-api-df495ba2e917.zip): 19,558 bytes; SHA-256 7962c07b03be705cc80cf265d308458e03c41507fb073f329cdd08c443e7f240. All seven ZIP entries passed the fresh unzip integrity check; its six source entries match the audit/provenance byte identities. No archive was rewritten.

Read coverage (reference IDs from the audited document):

| Group | Reference IDs | Result |
| --- | --- | --- |
| Standard inputs, encoding, decode and layout | input, make, cache, getter, encode, encscale, decode, postprocess, dftpublic, dftroots, dft, fit, slots | 13/13 supported |
| Public scheme, dispatch, decrypt/encrypt and defaults | getscheme, schemeio, pkedispatch, pkeoverride, ckksdecrypt, decryptcore, ccdecrypt, crt, rnsencrypt, encryptprefix, ccencrypt, ctdefaults, ctkey | 13/13 supported; qualify “always” as below |
| Plaintext, polynomial and integer access | ptbase, ptmutable, ptmetadata, ptfactory, polymutable, polyctor, dcrtctor, integercrt, orderedq, intin, intout, intfloat | 12/12 supported |
| Metadata, identity and serialization | ctmetadata, setlevel, sf, identity, object, ctarchive, dcrtarchive, serialbinary, serialjson, ctser, ccserjson, ccserbinary, ccload | 13/13 supported |

## Four requested semantic checks

### 1. Raw Decrypt versus DecryptCore flooding

**Observed:** public SchemeBase::Decrypt dispatches to the PKE virtual Decrypt overload. CKKS PKE enablement installs PKECKKSRNS. Its Poly* overload calls DecryptCore, then adds polynomial noise only when both NOISE_FLOODING_DECRYPT and EXEC_EVALUATION hold; it changes to COEFFICIENT and returns Poly through one-tower conversion or CRTInterpolate. It does not execute CKKSPackedEncoding::Decode.
**Observed:** public SchemeBase::DecryptCore instead calls the virtual core directly. PKERNS supplies that override, and PKECKKSRNS does not override it. The core forms the RLWE decryption in EVALUATION format and has no polynomial-flooding branch. Therefore the audit's distinction is valid: a custom decoder using Poly* can retain configured polynomial flooding, while direct core access does not. Neither fact establishes equivalence to standard REAL Decode postprocessing.
Sources: [scheme wrappers](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-scheme.h#L205-L247), [CKKS installation](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp#L42-L47), [CKKS decryption](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp#L44-L95), [RNS core](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L199-L223), [PKE virtual declarations](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-pke.h#L113-L141), [RNS inheritance/override](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-pke.h#L53-L119).

### 2. Standard Decode clears raw coefficients

**Observed:** the successful CKKS context-decryption flow propagates metadata and invokes Decode. Decode reads centered coefficient values through ConvertToDouble, then calls SetValuesToZero on its NativePoly (line 372) or Poly (line 405), before later REAL-mode output processing. Thus a returned ordinary decoded Plaintext is not a lossless raw-coefficient readback route. The audit also correctly keeps polynomial flooding distinct from standard REAL Gaussian/projection/imaginary-clearing behavior.
Sources: [context flow](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp#L555-L602), [centering/conversion/clearing](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L336-L405), [subsequent output processing](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L421-L508).

### 3. Partial-packing stride is not all-coefficient canonical evaluation

**Observed:** ordinary Decode uses Nh=N/2, gap=Nh/slots, and reads indices i*gap and i*gap+Nh. The current N=64, batch=16 test's Horner loop instead consumes every supplied coefficient and evaluates selected roots with the listed powers-of-five exponents.
**Derived, not executed:** for gap=2, a formal polynomial with only an X coefficient has zero stride-extracted coefficient vector before standard postprocessing, but nonzero full canonical value at any selected root of unity. This symbolic witness demonstrates that the two numerical operations are not generally identical. It is not a claim about a sampled ciphertext's error, a cryptographic experiment, or the output of the subsequent Gaussian/REAL processing.
The audit correctly leaves the intended partial-packing projection/repetition rule pending and distinguishes this diagnostic (gap=2) from the parent-supplied full-packing target S=N/2 (gap=1). Full packing removes this particular coefficient omission, not the separate REAL-mode postprocessing differences.
Sources: [official extraction](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/encoding/ckkspackedencoding.cpp#L336-L405), [current Horner loop and roots](/Users/lifeng/Documents/20231788-openfhe-codex-precision-01/tests/precision_first_mult2_contract_test.cpp:395), [diagnostic dimensions](/Users/lifeng/Documents/20231788-openfhe-codex-precision-01/tests/precision_first_mult2_contract_test.cpp:39).

### 4. Element Encrypt does not fill CKKS metadata

**Observed:** SchemeBase::Encrypt(Element, key) delegates to PKERNS Encrypt, which copies the polynomial, invokes official EncryptZeroCore, converts the copy to EVALUATION, adds it to the encryption and sets noiseScaleDeg=1. The ciphertext constructor copies key context/tag. Default slots=0, level=0, scalingFactor=1, scalingFactorInt=1 and INVALID_ENCODING otherwise remain.
**Observed:** cc->Encrypt(Plaintext, publicKey) adds the metadata propagation that the direct Element call does not perform. It reads the public polynomial without calling Encode or checking IsEncoded. The audit is therefore correct that Candidate A needs explicit validated metadata and that Candidate B must not expose a stale/empty cached Plaintext as a coherent high-precision value object.
Sources: [scheme entry](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-scheme.h#L205-L212), [RNS encryption](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/schemerns/rns-pke.cpp#L40-L69), [key binding](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/ciphertext.h#L72-L82), [defaults](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/ciphertext.h#L499-L515), [high-level propagation](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/cryptocontext.h#L1250-L1266), [cache/getters](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/encoding/ckkspackedencoding.h#L61-L101), [getter types](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/encoding/ckkspackedencoding.h#L143-L152).

## Nonblocking refinement and handoff note

**R1 — Wording only:** [audit line 16](/Users/lifeng/Documents/20231788-openfhe-codex-precision-01/coordination/PRODUCTION_IO_OFFICIAL_API_AUDIT.md:16) says cc->Decrypt “always” invokes CKKS Decode. More exact wording is “after a valid result, the CKKS-encoded path invokes CKKS Decode.” The wrapper returns early when result.isValid is false and selects a different Decode call for other encodings. This does not invalidate the successful-CKKS raw-coefficient-clearing conclusion. Owner may make that small edit; this review did not modify the audit. [Exact conditions](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/cryptocontext.cpp#L580-L598).

For a Pro bundle extracted down to the 25 listed files, also include the following two already-verified 53-ZIP declaration headers if the reader is to inspect the virtual-dispatch chain without resolving includes. This is a bounded handoff convenience, not a new implementation dependency, missing runtime result, request to fetch more sources, or claim that a 25-file excerpt bundle is independently buildable.

| Existing official declaration header | Bytes | SHA-256 | Git-blob SHA-1 |
| --- | ---: | --- | --- |
| [src/pke/include/schemebase/base-pke.h](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemebase/base-pke.h) | 5637 | c34bc5b4d615076f3f17bc3d7222be810a067d1a2863e1c30f171bc5dcd54a3d | 8b8391c6242665fba1b001e34c422d4aac3f1bc0 |
| [src/pke/include/schemerns/rns-pke.h](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/include/schemerns/rns-pke.h) | 5308 | 1ed5ceb82280ef49fc532635ea85cd49bc7316ade517bdaa09915feaf7b05132 | cda6ea87811c2f8c3f528ce89ea934a37c6dcaad |

## What this result does not approve

The audit may proceed as source evidence for design/handoff. It does not authorize adopting Candidate A/B, copying a test secret-decryption oracle, omitting client validation or output-policy decisions, freezing repeated-stage basis/key/scale semantics, or calling the production I/O complete.
No encoder/decryptor, high-precision roundtrip, ciphertext archive roundtrip, no-wrap bound, slot-error threshold, noise-output policy or security claim was executed or established here. Those remain the scoped design and Windows/CI runtime gates already marked pending in the original audit.
