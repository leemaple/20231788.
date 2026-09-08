# PUBLIC-S100-ECD-CELL-01 mathematical adoption review

## Disposition

**Accept the certificate mathematically, conditional on root's separate exact
artifact/source/process intake.** The retained table certifies all 32,768
coefficients, with no refuted or inconclusive cell and a strictly positive
minimum clearance. Under the already reviewed inverse-embedding mathematics,
this closes the literal statement for the retained current public polynomial:

```text
p_j = nearest_ties_down(2^100 * sigma^{-1}(z)_j),  j=0,...,32767,
```

where `z` is the unchanged original dyadic S100 input and the rounding preimage
of `p_j` is `(p_j-1/2,p_j+1/2]`.

Consequently, the previously conditional current-`p` bounds

```text
||sigma(p)/2^100-z||_inf <= 2^-86
```

and

```text
||(sigma(p)/2^100)^256-z^256||_inf
  < (424504491253/10^12) * 2^-80
```

are now applicable for this public encoding. The second is an ideal-squaring,
encoding-only contribution below approximately `0.424504491253` of the E80
threshold. Neither conclusion includes PKE, Tensor2/Relin2/RS2, omitted LL,
readout, or historical-phase error.

No new mathematical defect or rounding mismatch was found. This is not a new
ciphertext run, production repair, historical-p identity proof, original S100
E80 PASS, Table 3 reproduction, or security result.

Root subsequently reported that its exact artifact/source/process intake and
independent all-row reclassification completed successfully, with retained
`GREEN_INTAKE`, `ROOT_INTAKE_REVIEW`, and run receipt. I do not duplicate that
transport/process attestation here; on the root-owned receipt, the condition in
the opening disposition is satisfied for root-level adoption.

## Evidence inspected

I re-read the independent source/mathematical review at
`coordination/reproduction-adjudication-20260909/MATH_REVIEW.md` and its exact
derivation of the powers-of-five Hermitian inverse, half-down cells, the two
closed-form coefficients, the semantic `p+1` negative, and the conditional
error propagation. I also read the retained candidate review to preserve its
explicit source/interval assumptions and the distinction between mathematical
classification and harness evidence.

Directly inspected retained run files:

- `execution/certificate/RESULT.json` and all rows of
  `execution/certificate/coefficient_cells.tsv`;
- `execution/controls/RESULT.json`, control/certificate stdout and exits;
- `execution/OUTCOME_AGREEMENT.json` and `execution/HARNESS_RESULT.json`.

The result binds the same public payload
`7cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f`
and coefficient stream
`66c36716c1445b4b2c6c47e06996c2b080792b0b9251fe075033f542acd540be`
reviewed previously. The result declares the reviewed candidate/core hashes
`e1a243ec...35e9` and `0167613b...412`; root owns verification that the saved
process actually used those bytes and the admitted source/environment.

The saved controls result records PASS for the N4/N8
constant-plus-middle models, N4 X, N4 X-cubed, and the negative X-cubed sign
sentinel. It records exactly four tiny inverses and no full transform in that
stage. Those contents are evidence read here; I did not execute the controls.

## Independent table classification

Using a standalone standard-library `csv`/integer script—not the returned
checker or inverse code—I read every row in order and recomputed the half-down
classification from its saved endpoints. For each row I required:

```text
denominator = 2^224,
real_lower <= real_upper,
imag_lower <= 0 <= imag_upper,
left  = p*2^224 - 2^223,
right = p*2^224 + 2^223,
CERTIFIED iff real_lower > left and real_upper <= right,
margin = min(real_lower-left, right-real_upper).
```

Observed/recomputed results:

```text
rows                  32768, consecutive indices 0..32767
CERTIFIED             32768
REFUTED                   0
INCONCLUSIVE              0
table SHA256           3a5dad65c2323ae1ab1b453a5cfeb73028328176982cf82458bcf0d5049a9245
minimum-margin index   26696
minimum-margin numerator
  19406664479890172665284974927688131526799829314786449218863104
denominator
  26959946667150639794667015087019630673637144422540572481103610249216
```

The counts, table hash, minimum numerator/denominator, and index agree exactly
with `RESULT.json`. All 32,768 `actual_p` entries also agree in order with the
retained public payload.

## Minimum-margin interpretation

The minimum is

```text
19406664479890172665284974927688131526799829314786449218863104 / 2^224
  = 7.198331925313573596383386835479568... * 10^-7
```

in **scaled ideal coefficient units**. At index 26696 the limiting side is the
right endpoint `p+1/2`: the saved real upper enclosure lies this positive
distance below that boundary. Thus even the closest complete saved enclosure
is strictly inside its rounding cell.

This quantity is a certified clearance from a half-integer decision boundary,
not the actual coefficient error, canonical encoding error, ciphertext noise,
or precision in bits. It justifies the global positive-margin GREEN but does
not improve the conservative per-coefficient error bound beyond `1/2` without
additional analysis.

The two analytic anchors also agree:

- index 0 has `p=-2^32`, real enclosure exactly the point `-2^32`, and
  imaginary enclosure exactly zero;
- index `N/2=16384` has `p=-2^32`, a symmetric real enclosure containing that
  exact value, and an imaginary enclosure containing zero.

These are the independently derived closed forms from the original input, not
values generated by a transform round trip. Together with the passed analytic
tiny controls and source review, they constrain sign, normalization, twist,
and orbit placement. They do not by themselves replace the all-row table.

## Activated encoding-only theorem

Because every ideal scaled coefficient lies inside the corresponding actual
cell, every real coefficient discrepancy has magnitude at most `1/2`.
Summing the `N` real polynomial coefficients in any canonical embedding gives

```text
delta <= N/(2*2^100) = 2^-86.
```

This is conservative; the strictly positive cell margins could make the
inequality strict, but no strengthening is needed. With the separately adopted
current-polynomial radius `R=495621/500000`, exact `Fraction` arithmetic again
gives

```text
424504491252/10^12
 <= 256*(R+2^-86)^255*2^-86 / 2^-80
 < 424504491253/10^12
 < 1/2.
```

The certificate therefore closes the missing premise of this conditional
calculation for the current public `p`. It also makes the former concern that
decimal160/220 agreement alone did not prove the ideal inverse immaterial for
this one input; it does not prove the production encoder correct for arbitrary
inputs or numerical dependencies.

## Adoption conditions and retained unknowns

This mathematical adoption relies on root's now-completed receipt establishing
that:

1. the table/result/control bytes are complete and immutable and match the
   exact remote artifact;
2. the executed inverse and interval-core bytes are the independently reviewed
   versions, with the frozen original input and retained current `p`;
3. the four tiny controls precede the one full inverse, declared/saved/observed
   exits agree, and no retry or precision/input substitution occurred;
4. no producer label or counter is treated as sole proof of process history.

Subject to those receipt facts, no remaining issue blocks **current-`p` Ecd
nearest-rounding adoption**. Remaining reproduction boundaries are separate:

- historical source-section similarity still does not prove the old run used
  coefficient-identical `p`;
- the original Linux and Windows S100 ciphertext chains remain E80 FAIL;
- public-encryption, evaluator, and readout contributions remain outside this
  encoding-only certificate;
- Table 3 empirical provenance/conditions and security certification remain
  unresolved.

## File identities and commands

| File | SHA256 |
| --- | --- |
| `execution/certificate/RESULT.json` | `669d03f982a49b5168aa36616289dc26f45bbece4bf7fc7555daacb46454e5b3` |
| `execution/certificate/coefficient_cells.tsv` | `3a5dad65c2323ae1ab1b453a5cfeb73028328176982cf82458bcf0d5049a9245` |
| `execution/controls/RESULT.json` | `62aae806299892391da19043d66a67913d9fc1d02a19fc15cb9378911b90749f` |
| `execution/OUTCOME_AGREEMENT.json` | `e347c4738236ca83e12f3274c71fc45bdfe1ea2ab9a90f713bcbf830816f24f7` |
| `execution/HARNESS_RESULT.json` | `be714aa4d4581858e801b91bbc47924dabacb8d38a2a105a13a488e42e58daf3` |
| prior `MATH_REVIEW.md` | `16bed758779a74ad2787717ac35bdbbb01c3db4df53ece17d7624c2ba1ceb649` |
| prior `CANDIDATE_REVIEW.md` | `6eb8231ff2b97228d1b0248d4b4163e8985e9931e4a010d3467c61c252c5dd42` |

Read-only commands were `find`, `du`, `wc`, `sed`, and `shasum`. My bounded
Python checks used only `csv`, `json`, `hashlib`, `fractions.Fraction`, and
`decimal.Decimal`; they reclassified saved rows, compared saved `actual_p`
integers to the retained JSON, inspected indices 0/16384/26696, and rechecked
the prior rational propagation inequality.

I did not execute/import `check_rounding`, FFT/NTT, any tiny/full transform, a
build, input generation, encoding, FHE, sampling, CI, browser, or Git. No
artifact, production source, prior review, input, threshold, or Pro return was
modified; this review file is the only edit.
