# Public S100 encoder-cap mathematical adoption review

## Disposition

**Accept the retained cap result as a mathematical certificate for its one
hash-bound public polynomial, conditional on root's separate transport,
source/build/input, dependency, and execution receipt.** I did not use the JSON
status label, commit strings, or literal call counters as attestations. Given
that external receipt and the already reviewed interval-transform source, the
result discharges Pro assumption A04

```text
||sigma(p)||_inf / 2^100 <= 127/128
```

for this current public `p`. Together with the adopted A01--A03 honest-path,
finite-support, and exact-ring assumptions, this instantiates the adopted
IL02--IL20 initial/eight-step strict-half-modulus certificate. It does not
change the original historical S100 E80 FAIL and does not establish historical
coefficient identity, Table 3, security, encoder accuracy relative to an ideal
transform, or a production precision PASS.

Root subsequently reported that its separate `GREEN_INTAKE.json` and
`GREEN_RUN_RECEIPT.json` gate is complete. I do not duplicate or independently
re-attest that intake here; on that root-owned receipt, the provenance
condition in this disposition is satisfied for root-level adoption. The
honest-path/source semantics remain the explicit theorem assumptions described
below.

No threshold, input, scale, modulus, root, key, noise, or historical secret was
changed or requested in this review.

## Exact certificate check

The actual retained `public_s100_cap.json` gives

```text
L = 26489769450904813335100288033287613016136625256900427194459063306113 / D
H = 26489769450904813335100288033287613016136625256900427194459834728848 / D
D = 26959946667150639794667015087019630673637144422540572481103610249216
C = 16129/16384 = (127/128)^2.
```

Independent `Fraction` parsing and integer cross multiplication verified
`L <= H < C`. In reduced form the exact positive squared-cap margin is

```text
C-H =
3160847133057432426075619511243329920620094993973901343117513703
/
1684996666696914987166688442938726917102321526408785780068975640576.
```

Equivalently, the decisive reduced cross-product difference is

```text
16129*den(H) - 16384*num(H)
= 51787319428012972868822950072210717419439636381268399605637344509952 > 0.
```

The verifier evaluates `p/S` at all odd `2N`-th roots and encloses squared
moduli (`diagnostics/certify_public_encoder.py:152--194`); its classification
uses the same exact cross multiplication (`:143--149`). The fact that the
largest lower endpoint and largest upper endpoint occur at different reported
indices is harmless: for exact values `z_i`,
`max_i lower_i <= max_i z_i <= max_i upper_i`.

A human-readable radius bound needs no square-root implementation. A second
integer comparison gives

```text
H < (495621/500000)^2,
```

so the retained enclosure implies rigorously

```text
max_odd roots |p(root)|/2^100 < 495621/500000
                                      = 0.991242
                                      < 127/128 = 0.9921875.
```

I did not run `canonical_bounds`, a transform, or an encoder to obtain either
comparison.

## Binding and source interpretation

The input JSON SHA256 recomputed as
`7cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f`,
exactly the payload hash in the cap result and in `public-encoding.sha256`.
Re-serializing its 32,768 accepted integer strings as one decimal integer plus
LF per coefficient gives
`66c36716c1445b4b2c6c47e06996c2b080792b0b9251fe075033f542acd540be`,
also exactly the cap-result field. The independently recomputed maximum
absolute coefficient is

```text
107971747414743578749861062769 < 2^97,
```

matching the result. Conditional on provenance, this exact bound directly
closes the current `p`'s initial coefficient-margin input and is stronger than
the earlier conditional StableRound consequence `M_enc < 2^123`; it does not
prove the correctness or ideal accuracy of StableRound and does not transfer
to an unbound historical `p`.

The payload fields are the frozen original profile: `N=32768`, 16,384 slots,
cyclotomic order 65,536, gap 1, scale `2^100`, and the expected eleven ordered
moduli/roots. The source path constructs the original near-unit input at
`tests/paper_full_eight_square_oracle.h:97--119`, selects it once in the dump
driver at `diagnostics/public_s100_encoding_dump.cpp:55--84`, and supplies the
fixed geometry/basis/scale to one existing `ComputeEncoding` call at
`src/high_precision_client_io.cpp:759--775`. The unchanged encoding body makes
the two inverse transforms, stable rounding, coefficient guard, and exact
residue conversion at `src/high_precision_client_io.cpp:463--496`. This is the
original input construction, not an annulus substitution.

The saved files record `certify_exit=0`, an `ENCODER_CAP_CERTIFIED` stdout,
four tiny model tests with `OK`, an API-negative pass, and one public-encoding
call. The separate metadata-only invocation correctly records
`encoding_calls=0`; that is not inconsistent with the distinct public-encoding
artifact's `encoding_calls=1`. These are observed artifact contents only. Root
owns the conclusion that they came from the reviewed source, runner, and
one-shot execution; literal counters are not runtime instrumentation.

## Theorem boundary and the two caps

The new result closes the stronger **encoded-polynomial** premise

```text
||sigma(p)||/S0 <= 127/128.
```

It must not be relabeled as the earlier root **fresh-phase** premise

```text
||sigma(p+f)||/S0 <= 255/256.
```

They concern different ring elements and have non-equivalent budgets. With the
adopted deterministic public-encryption error bound
`||sigma(f)|| <= 42040786944`, the newly certified encoded cap implies the
fresh cap because

```text
127/128 + 42040786944/2^100 < 255/256.
```

The reverse implication remains false in general. Thus the cap artifact
supplies Pro's A04 directly for the current `p`; it does not validate the
earlier unproved `E_enc <= 1/512` route or turn the two final numerical budgets
into duplicate measurements.

Under root's honest-source/provenance adoption, the applicable conclusion is:
the initial DCP and all eight listed multiplication/Relin2/RS2 lift gates are
strictly nonwrapping for this public `p` and every key/error instance admitted
by the adopted finite-support source contract. This is a sufficient worst-case
source-domain result, not an observation of a historical secret execution or
an E80 error analysis.

The limited historical source-section bridge says selected input and encoding
arithmetic sections match, but explicitly does not establish identical
historical coefficients or numerical build behavior. A historical application
therefore still requires the adopted A05-style coefficient/build equivalence;
profile names and commit labels alone are insufficient.

## Reviewed identities

Principal directly read files and SHA256 values:

| File | SHA256 |
| --- | --- |
| `artifacts/public-s100-encoder-cap-green-34265676284-1/public_s100_cap.json` | `c1b0840c6baac17c16edc8c90fe9ea9f51f7b0932c82315d2a7b3a0337caf3be` |
| `artifacts/public-s100-encoder-cap-green-34265676284-1/public_s100_encoding.json` | `7cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f` |
| `artifacts/public-s100-encoder-cap-green-34265676284-1/public-cap.exit` | `05be6308f31538b24c0c17f375596c18daf26c8fffb537f2f6fe17fa6c8b0752` |
| `artifacts/public-s100-encoder-cap-green-34265676284-1/metadata.json` | `91ac87d1ca7b822e08764363f1917faf53193550a104af587bf8fb66e8a05727` |
| `artifacts/public-s100-encoder-cap-green-34265676284-1/provenance.txt` | `d2b531916df8894c5c0eb13bccca4038e68d79ddf0a6fafec89a3eaf90230d83` |
| `artifacts/public-s100-encoder-cap-green-34265676284-1/tiny-transform-models.log` | `4a160235c0add54f7a5997815d0e01a072210439de4899c702c94dd3cd814662` |
| `diagnostics/certify_public_encoder.py` | `2dcf566fdc41d250e7dd72899431cd2d262ef4fc275f282a0a139295d0120be8` |
| `diagnostics/public_s100_encoding_dump.cpp` | `1e6c379ced9665057765c433fa5b2a4447a9e73a5034dc589988341d1d90a80f` |
| `src/high_precision_client_io.cpp` | `85b2b03954262344544dca96f84f8aa06044fb0fc74ff13f1a2317b67d666ac6` |
| `tests/paper_full_eight_square_oracle.h` | `08d6c4dfe6761b84d893d90c120418a29d8a2b96aa05304e95728d72e4c3e203` |
| `coordination/initial-lift-nonwrap-20260909/ADOPTED_CONTRACT.md` | `95a0ced1676453e546b8ece9cf993d7b7280407ffcc22a1030bec6911ee0d121` |
| `coordination/initial-lift-nonwrap-20260909/PRO_RETURN_MATH_REVIEW.md` | `38e173a645ade344086fbc02a72792a299a6b5a25853a13e12cc62c79b3cf725` |
| `coordination/initial-lift-nonwrap-20260909/PUBLIC_ENCODER_CANDIDATE_REVIEW.md` | `02d280d1fc00e10199c728d0fdac7603236d52459820fd402931278e997327e3` |
| `coordination/public-s100-encoder-cap-20260909/FIX_REVIEW.md` | `a2ec3bdb3536f543603a70e5b2dfe20a604214b73b4bd0657ee0892ebb48e600` |
| `coordination/public-s100-encoder-cap-20260909/GREEN_THIN_REVIEW.md` | `e81300851a9ad49aab89729060d29ef03e406a395b24ef739007939485e9fdc5` |
| `coordination/public-s100-encoder-cap-20260909/SOURCE_BRIDGE_REVIEW.md` | `0be7db6a7f948ccd7bbea3c4e010a79b6f82dd014e2d514b28366a1ac6b8809f` |

The supplied worktree identity was HEAD
`28bab40431eebc85d72521c6d9dc840ecd675cd7`; this review did not invoke Git to
re-attest it.

## Commands and execution exclusions

Read-only commands used were `wc -l`, `ls -l`, `sed -n`, `nl -ba`, and
`shasum -a 256`. The bounded Python checks used only `json`, `hashlib`,
`fractions.Fraction`, integer `max/abs`, and `math.isqrt`; they asserted the
endpoint ordering and cap cross product, recomputed both JSON bindings and the
maximum coefficient, and constructed the outward 18-decimal square-root
enclosure. The simpler rational `0.991242` enclosure above was then checked by
exact integer arithmetic.

I did not execute returned/candidate code, `canonical_bounds`, C++, an encoder,
FFT/NTT or any transform, FHE, a sampler, a build, CI, browser, or Git, and did
not read a quarantined implementation. No production, threshold, artifact, or
prior report was modified; this review file is the only edit.
