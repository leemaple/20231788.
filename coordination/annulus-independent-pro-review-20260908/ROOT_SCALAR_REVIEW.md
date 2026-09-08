# Independent root-side scalar review

Scope: returned `pro/checks/independent_scalar.py` and `pro/checks/replay_old_s100.py`, their relationship to the retained raw rows and current clean-room test source, and the deductions in `pro/REVIEW.md` / `pro/NEXT_ACTION.md`. Workspace baseline supplied by the owner: `932d922`. Reviewer identity: independent Codex context, requested model/backend identity **requested-unverified**; no Fable identity claimed.

The reviewer read both checkers, the new C++ input/recording/decision path, the original input formula, and selected actual new/old raw rows before reading the author's review and next-action conclusions. First-pass judgment was sent to the owner before the author documents were read. This review did not execute the checkers, repeat full-slot arithmetic, build, run FFT/cryptography, create new samples, access the network, or perform Git actions. Root independently owns checker execution and complete receipt/hash validation. Returned numerical JSON values below are inspected retained results, not newly executed results of this reviewer.

## Decision

The new checker provides useful independent scalar recomputation of the recorded observations. Its formulas and the stated old-sample counterfactual are correct. No finding here invalidates the retained annulus sample or establishes a production arithmetic defect. Accept the scalar-review portion of the return, conditional on root's separate successful execution and source/receipt reconciliation. Preserve the distinction between this bounded acceptance and the incomplete full reproduction goal.

## 1. What is independently recomputed

`pro/checks/independent_scalar.py:40-51` independently expresses each dyadic input as integer numerators over 2^75 and each scale as a closed exact rational product. These match `tests/s100_annulus125_eight_square_test.cpp:56-66` and `tests/paper_full_eight_square_oracle.h:78-89`; the scale divisor exponent 2^k-1 correctly sums the contributions from all eight multiplication levels.

The parser binds the header, exact order/count of all 16384 rows, source/pin metadata, exact reduced scale numerators/denominators, declared maxima, gates and footer (`independent_scalar.py:53-84`). The arithmetic loop does not derive its answers from those declared maxima or an author verification JSON. For every row it forms x, u=x+E0 and v=x, then computes

    I8 = E0 * product(j=0..7, u_initial^(2^j) + x^(2^j)),
    A8 = E8 - I8.

This follows the exact complex identity a^256-b^256=(a-b) product_j(a^(2^j)+b^(2^j)). The loop order at lines 105-107 uses the pre-squaring powers in each factor. It separately squares both endpoints and compares their difference to the factor form. This differs from the earlier receiver's propagated-delta recurrence and from the C++ direct endpoint subtraction. It is an independent implementation of the same mathematical identity, not an independent cryptographic execution.

Lines 110-118 recompute squared complex norms, argmax slots, runner-up values, failure counts, and same-slot relative errors. Only afterwards do lines 125-140 compare gates, maxima and declared agreement. E8_prod minus E8_obs cancels their shared ideal endpoint, so the additional terminal consistency check at lines 116/130 is meaningful. The witness uses the difference of the two producer error rows, equal to actual_difference-ideal_difference, and correctly adds that to the independently computed ideal difference (lines 121-125).

The exact lower-bound check at line 120 is correct: if the integer numerator of |x|^2 is n over 2^150, then |x^256|=(n/2^150)^128; hence n^128>2^(150*128-10) proves |x^256|>2^-10. There is no missing square or factor of two.

## 2. The precise replay limitation

The TSV stores E0_obs, E8_obs and E8_prod errors, not raw w0/w8 slot values or integer decryption polynomials. The checker reconstructs w0=x+E0 and recomputes the original x^256. It cannot independently recover an unrecorded w8 and test whether the original producer subtracted the correct ideal value. That association comes from the inspected C++ recording path (`tests/s100_annulus125_eight_square_test.cpp:283-292`) plus execution provenance.

Thus “independent full-slot recomputation of the error records and propagation” is accurate. “Independently re-decrypted ciphertexts / revalidated every raw endpoint against an unrelated oracle” would be inaccurate. `REVIEW.md:48,104-106,116` already preserves the essential limitation. The checker also explicitly lists unavailable fresh-producer, 512/768, Horner and lineage evidence at lines 157-158. This is a scope boundary, not evidence of an algorithm false positive or a reason to dismiss the bounded result.

Ordinary Decimal arithmetic at two precisions and a factor/direct agreement are numerical consistency evidence; they are not outward-rounded certification of all transcendental/FFT errors. The source and author conclusion correctly retain `CONDITIONAL_OBSERVER_NOT_FORMAL`.

## 3. Old S100 decomposition and the zero-residual counterfactual

`pro/checks/replay_old_s100.py:25-31` checks the supplied raw hash/size, source/run identity, old scale endpoints, complete row/check count and retained CTest exit8/FAIL. Lines 37-46 use the original base1015 formula and recompute both component and complex norms, I8 and A8 per slot. They do not subtract maxima. The declared old E80 status is retained, not retrospectively replaced by the stronger complex norm.

For each retained sample, with the original x and physical fresh phase held fixed, replacing all later deviation by A8=0 gives E8=I8. The returned results report max|I8|/T of approximately 11.63303632 (Linux) and 11.00110079 (Windows). Thus ideal later arithmetic still misses E80 on those retained starting points. The original component-norm I8 ratios are also above 10, so this result does not depend on changing the old norm.

For the actual sample, take a slot s* maximizing |I8|. Then

    |E8(s*)| >= max|I8| - max|A8|.

Using the separately recomputed maxima gives approximately 11.57355764 T and 10.90011649 T, both above T. This is a legitimate lower bound even when the maxima occur at different slots. It is not an exact attribution by subtracting two maxima. `REVIEW.md:150-156` and `NEXT_ACTION.md:12` use the inequality correctly.

The counterfactual means eliminating subsequent deviations while preserving the same fresh phase cannot rescue those two samples. It does not prove impossibility for every legitimate implementation change: changing the fresh phase, or producing a sufficiently large oppositely directed later residual, is a different mathematical intervention. No such admissible mechanism is supplied here. Root should preserve the author's explicit limitation rather than generalize this into a universal impossibility claim.

## 4. Relative precision and interpretation of the new domain

`independent_scalar.py:114-115,153-154` correctly computes max_s |E8_s|/|x_s^256| using the same slot in numerator and denominator. The inspected new retained result reports 82.49994914 absolute bits and 73.30262652 relative bits; the old results report approximately 73.18150757 and 73.27544621 relative bits. These are consistent with output contraction and do not justify claiming 82.5 relative bits or preservation of 80 significant bits.

For nonzero complex x, f(x)=x^256 has local relative condition number |x f'(x)/f(x)|=256, independent of the radius. Shrinking the radius reduces the absolute derivative 256|x|^255 and the output magnitude together. Consequently the much better absolute E8 in the new sample does not by itself demonstrate corresponding relative-information improvement. `REVIEW.md:126-135,158-160` states this correctly, including that new and old key/noise realizations were not paired.

## 5. Low-priority integration caution: analysis exit versus numerical status

`independent_scalar.py:127-128` deliberately accepts a correctly recorded numerical FAIL with process_exit=1; its CLI nevertheless returns 0 at line 168 after writing the result. Consequently its successful invocation means “analysis completed consistently,” not necessarily “the encrypted sample passed.” This is harmless for the current retained PASS when JSON.status is separately checked, but a caller must not infer numerical PASS from this checker exit code alone.

Disposition: use explicit `result.status == "PASS"` at the root acceptance boundary, together with the real program exit/receipt. No returned immutable file needs modification. This is a caller contract caveat, not a new production defect or an invalidation of the observed sample.

Other limitations are also correctly bounded: old observer check PASS values are reported at `replay_old_s100.py:67` rather than enforced as a new proof; new fresh observer agreement remains a runtime declaration. These scripts should remain analysis components accompanied by the existing provenance and runtime-control review, not be promoted to complete standalone evidence authorities.

## 6. Production repair and completion boundary

The scalar evidence identifies inherited fresh error as the dominant contribution in the retained old cases. Identifying that term does not establish that public encryption is implemented incorrectly. The return supplies no source counterexample or mechanism for a production repair that preserves the original stress inputs, parameters and public evaluator semantics. Printed theorem normalization and receiver-format defects cannot supply such a production RED.

`NO_JUSTIFIED_PRODUCTION_REPAIR` is supported as a decision about the evidence currently supplied. It is not “no possible improvement exists,” and it must not be used to close the user's full reproduction objective. The accepted status remains: one retained Linux annulus conditional endpoint PASS; original near-unit S100 FAIL unchanged; historical S116 PASS belongs to its changed parameters; table-3 same-source experimental reproduction and deployment assurance remain incomplete. No additional cryptographic sample is required by this review.
