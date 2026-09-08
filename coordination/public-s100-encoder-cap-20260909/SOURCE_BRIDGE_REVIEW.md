# Independent historical source-section bridge review

Disposition: **accept the recorded source-section identity claim with its existing limitations**. No misleading literal match or new correctness finding was identified. This comparison does not establish historical coefficient equality, historical binary equivalence, or applicability of a future public-polynomial cap to an old encrypted run.

## Scope and reviewed identities

Observed successor HEAD `28bab40431eebc85d72521c6d9dc840ecd675cd7` with clean status at review start. Reviewed only the root's source-bridge checker, saved JSON, the final source-bridge paragraph in `RED_INTAKE_AND_GREEN_GATE.md`, and relevant permitted clean-room Git blobs/diffs from historical `ed5fd192a89d6d4728ad295e87cf06a3f4abc832` to production `a4b815a733efe81897325e2a8e4c826a4ebfa439`. No quarantined implementation or modified OpenFHE tree was used. Prior mathematical and F1–F4 findings were not reopened.

Reviewer: independent Codex context `public_encoder_candidate_review`; requested GPT-6 Astra/high, backend **requested-unverified**. No additional provider diversity is asserted.

Reviewed SHA256 values:

| Artifact | SHA256 |
| --- | --- |
| `check_encoding_source_bridge.py` | `83f6ec4eaad50a43507bbdb5e6499ebdc85640dc54fc3631e092067c46702d32` |
| `ENCODING_SOURCE_BRIDGE.json` | `73258a6be1d692f40816426a4587eb0143bcfd714a44ac5d02c5947ebb3e00af` |
| `RED_INTAKE_AND_GREEN_GATE.md` | `c92d5e19dd719b42600e10c8772b9560094000db1a5d3d81e4e13d426fc5b7ff` |

## What the extraction actually proves

The checker reads three exact Git blobs at each hard-coded commit using `git show`; it does not use the current working-tree versions as the historical comparator. `ROOT=Path(__file__).resolve().parents[2]` resolves to the project root at its present location. Section anchors must occur exactly once and in the expected order, so missing/ambiguous markers fail rather than silently selecting a different occurrence.

The five true fields have the stated narrow meanings:

1. The **entire** original public input/oracle header is byte-identical. Its historical and production SHA256 are both `08d6c4dfe6761b84d893d90c120418a29d8a2b96aa05304e95728d72e4c3e203`. This includes the original input formula/conversion and frozen constants, but does not prove that a historical executable invoked those functions or passed their result unmodified.
2. The range beginning at the independent-root-generation comment and ending immediately before `Forward` contains `TransformTable`, bit reversal, transform, inverse, `PaperRound` and `StableRound`. It is text-identical. It deliberately excludes forward/decryption code.
3. The historical encoding range begins at finite-input checks inside `Encrypt` and ends before the large-Poly/DCRT construction comment. The production range starts at the same finite-input checks inside `ComputeEncoding` and ends before returning its coefficient/residue pair. These ranges include the inverse calls, scale conversion, integer rounding/placement, composite modulus, full coefficient guard and exact residue conversion. Replacing exactly one `impl_->primary` with `primaryTable` and exactly one `impl_->check` with `checkTable` makes the two ranges text-identical. No other textual substitution is applied.
4. The public `ClientReal`, exact integer and complex type range before `PositiveRationalScale` is identical.
5. The working decimal type declarations, including primary 160/check 220, are identical.

The checker reports the entire codec as different, correctly: historical SHA256 is `ab6c1f2a39c339e951305ff19200d9b2ebd0d4a08b0a0a5c7eb212f4b74416d4`, production SHA256 is `41b4966f3fdcc251e7d25c82fb20f5daf35551a3df1f44c6b0cc24875e8ff368`. The public header also differs because the inspection structure/API were added; the checker does not incorrectly claim whole-header equality for that file.

## Omitted code and necessary qualifications

The extraction is not a complete transitive encoder equivalence checker. Its omissions matter if the result is reused beyond these exact fixed commits:

- The input/basis/context admission code, shape/scale prechecks and binding assignments before the finite-input loop are outside the matched arithmetic range. The full historical-to-production diff shows actual changes: metadata exponent and fresh-scale selection became plan-dependent instead of the old fixed 50/100 values, `ComputeEncoding` was extracted, and the inspection interface was added. These are not merely the two table renames. The document correctly says that complete files differ and separately requires the same admitted geometry/full basis/exact scale.
- Helpers preceding the independent-root comment, including `RequireFinite`, `Absolute`, `NegativePowerOfTwo`, `CompositeModulus` and basic complex arithmetic, are called by the compared bodies but are not all separately matched by this script. The independently read full codec diff shows no changes to those helper definitions between the two pinned commits. Therefore this omission does not falsify the current five equality fields. It does mean the script must not be advertised as a general checker of all encoding dependencies if future commit selectors change.
- `PositiveRationalScale` method implementations and table-construction/call-site binding are not fully covered by the selected public-type sections. The full diff does not change rational normalization/accessor implementations, but adds the plan-derived `FreshExactScale` behavior. Correct identity of the actual supplied scale remains an admitted semantic precondition, not a conclusion of the section matches.
- The post-core construction/encryption/ciphertext path is excluded. That is appropriate for the coefficient-source question and cannot support a claim of identical cryptographic behavior or historical samples.

The existing JSON's `historical_coefficient_identity: null` and remaining conditions, together with the final paragraph's explicit disclaimer, prevent these omissions from being misleading. No wording change is required for the literal source-only claim as written. A concise safe use is: “The listed source sections match after two table-reference renames; other code and numerical-build equivalence remain separate.” It would be incorrect to shorten this to “the historical encoder is identical.”

To connect actual coefficient results, the admitted geometry must be N32768/slots16384/gap1 with the same original ordered full basis and exact S100 scale; table objects must be generated from that same geometry; inputs must use the same original construction/conversion without intervening mutation. Relevant Boost headers, pi/trig/decimal/integer conversion behavior, compiler flags/semantics, backend/native-int configuration and linked official conversion implementations also matter. Source section equality alone neither measures those historical dependencies nor proves that finite-precision branches and rounded integers coincide. Exact historical coefficients or a sufficient robust-equivalence argument is still needed before transferring a result to the historical run.

## Independently observed bounded replay

After completely reading the source checker, ran only this source-read/hash/JSON comparison from the successor root:

```text
set -o pipefail
python3 -B -I coordination/public-s100-encoder-cap-20260909/check_encoding_source_bridge.py | cmp - coordination/public-s100-encoder-cap-20260909/ENCODING_SOURCE_BRIDGE.json
```

Observed exit 0, no output, approximately 0.06 seconds. The produced JSON therefore matches the retained artifact byte-for-byte; all five fixed comparisons were true and the whole-codec comparison was false. This is source comparison evidence only. No codec import/execution, C++ build, transform/FFT/NTT, sampling/FHE, CI/browser action, remote-run poll or Git mutation occurred. This report is the only owned edit.

Original S100 E80 FAIL and the historical applicability gap remain unchanged. The active remote GREEN run and any later cap result require their own exact-source execution receipts; this review makes no claim about their status.
