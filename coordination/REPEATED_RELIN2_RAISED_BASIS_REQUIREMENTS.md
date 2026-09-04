# Repeated Relin2: raised-basis versus HYBRID prefix tables

Status: source observation and future design obligation, NOT a proposed patch
or runtime failure. Inspected2026-09-04 against precision source
7a5b523acd93243bf3efa200f1c11f632380c4dd (active tested code bd141806).
This extends PAPER_PRECISION_PARAMETER_GATES.md with one concrete lower-level
constraint. No code, tests, parameters, keys or running agent were changed.

## Existing first-multiplication route is aligned
src/double_ckks.cpp:924-951 clones the high tensor, multiplies its existing
towers by the fixed q_div, appends a zero tower using fullTowerParameters.back(),
sets level0, validates against fullModuli_ and calls context_->Relinearize.
For the currently admitted first pair, its basis is exactly the original full
basis minus that last q_div, so the reconstructed raised basis is the original
full basis. The existing behavior and first-Mult2 tests are not contradicted.

## What a naive second-multiplication extension would change
Illustrative symbolic basis order, not a performed encryption:
original full=[q0,q1,q2,q3,p], with p=q_div;
initial DCP pair=[q0,q1,q2,q3];
first RS2 result=[q0,q1,q2];
naively raising that high part again yields [q0,q1,q2,p].
That last list is NOT the length4 prefix [q0,q1,q2,q3] of the original context.
Setting a level integer or relaxing the ReadyForFirstMult/fullModuli_ check
cannot make the two ordered bases interchangeable.

This illustrates why removal of the current guards is not a repeated-lifecycle
implementation. It is not a proof that no correct repeated implementation is
possible; the full paper destination remains required.

## Pinned official HYBRID code is position/table dependent
Official OpenFHE1.5.0 df495ba2e91739a6dc8f1de254fc5a41155ce504:
[keyswitch-hybrid.cpp](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp).
The provided file's21228bytes/SHA256
872d5e594ac2343f9d055a4e628871987113f67f2142d3a664b2015f17e1d345
were rehashed; its exact-pinned provenance is already retained in
coordination/handoffs/first-mult2-precision-c9ee28d-source-provenance.json.

- Lines315-329 take the actual ciphertext sizeQl and the context's fixed
  GetNumPerPartQ/number-of-partitions values.
- Lines338-358 construct decomposition parts from GetParamsPartQ(part)
  (including a prefix of the last part), then copy ciphertext towers by index.
- Lines360-365 request complementary bases and basis-switch constants from
  GetParamsComplPartQ(sizeQl-1,part), GetPartQlHatInvModq(part,sizePartQl-1)
  and the context's corresponding indexed tables.
- Lines378-393 use the eval key's crypto-parameters and its P-to-Q mod-down
  tables; there is no dynamically inferred replacement ordered-Q setup here.
- Lines401-433 compute delta=originalQsize-sizeQl and select eval-key entry
  index i for every Q tower, shifting only auxiliary P entries by delta.
  Replacing the final active Q prime by p would therefore still select the
  original Q key entry at that POSITION, not magically the old final-p entry.

These explicit indexed accesses support a prefix-compatibility requirement for
this existing call route. They are not merely a descriptive ciphertext-level
metadata dependency. Silent parameter-object mutation would also have to
address precomputation and evaluation-key consistency, not only modulus labels.

## Next source-backed acceptance questions for Pro/Codex
After first-Mult2 precision acceptance, the repeated-use design must explicitly
account for each high raised basis and low active basis, its matching tables,
key rows and context/key identity. Candidate solutions need source and hosted
tests, not an assumption that all subsets of one RNS basis behave as prefixes.
Whether a narrowly managed basis-specific context/key family or another
public-primitive construction is the simplest valid solution is still OPEN;
neither is selected or implemented by this note. Avoid a mutable live-context
shortcut or speculative generic framework.

Freeze a second-Mult2 independent arithmetic/scale oracle BEFORE changing the
current rejection. Require two-step and eventually eight-step state/basis/scale,
key/input immutability and precision observations; preserve invalid-state
rejection. Paper Section6.3 specifically uses eight squarings WITHOUT its6.2
intermediate recombine/decompose refresh, so adding that refresh is not a
substitute for the requested parameter regime. Section6.3's 40/60 roles, fixed
h128 and security/auxiliary-basis choices remain separate explicit obligations.

This note was obtained solely from new clean-room source and pinned official
references. No old local implementation, compilation, crypto, benchmark,
security evaluation or external-model interruption occurred.
