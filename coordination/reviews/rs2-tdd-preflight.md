# RS2 TDD preflight

Recorded: 2026-09-01 Asia/Shanghai

Status: **provisional test design only**. This is not an implementation task,
test patch, executed red/green result, or accepted diagnostic contract. Every
runtime fixture and count must be rebound to the final accepted Relin2
commit/tree under `rs2-preflight.md:396-411`.

The accepted Tensor2 commit `fb862a3...` does not yet contain Relin2 or an
actual `ReadyForRS2` output. It therefore cannot prove that a future failing
test is red only for missing RS2. The executable baseline must be the exact
Relin2 commit that has passed same-SHA Linux/Windows verification and review.

## Minimal executable matrix

Use one dispatching executable `tests/rs2_test.cpp <case>` for exactly 25
runtime cases, with every runtime negative independently registered in CTest.
Keep `rs2_api_contract_test` compile-only and outside CTest. The
`invalid_lifecycle_source_gate` row is one separately executed token/AST audit,
not a CTest registration, so it does not change the symbolic `B+25` final
runtime count.

| Case | Mechanical obligation | Oracle class |
|---|---|---|
| `valid_coefficients_state_immutability` | Every member/component/tower/coefficient of `(A,B)`; named half-boundary and carry witnesses; `B != RS(low)`; complete output basis/state/scales/lifecycle; input and deep cache immediately unchanged; an explicit `module.RCB(rs2Output)` returns coefficient-exact `C`, has complete expected state/high-metadata provenance, and does not mutate the pair | Independent coefficient oracle plus state/lifecycle/public-RCB integration contract |
| `metadata_provenance` | Input outer-map, value-pointer, and deep values unchanged; output maps distinct from each other/input; keys/values come from input high; value pointers follow the verified OpenFHE shallow-clone contract; input-low-only metadata does not propagate | OpenFHE integration contract |
| `no_eval_key_dependency` | Create a valid input, remove only that tag's EvalMult keys, require RS2 to pass the independent oracle, prove cache remains absent, then deep-restore tag/vector/context/A/B and pointer identity through RAII | Integration plus static route gate |
| `reject_ready_for_first_mult` | Reject a DCP pair before arithmetic; pair/cache unchanged | Lifecycle |
| `reject_after_first_rs2` | Reject an RS2 result passed to RS2 again | Lifecycle |
| `tensor2_reject_after_first_rs2` | Tensor2 rejects the state while RCB accepts that same object | Lifecycle boundary |
| `reject_short_active_basis` | Reject an otherwise valid `ReadyForRS2` input with only two active towers before arithmetic | Basis |
| `reject_composite_degree_two` | FIXEDMANUAL context with `SetCompositeDegree(2)` supports construction/DCP/Tensor2/Relin2 without Rescale, then RS2 rejects before arithmetic with pair/cache unchanged | Fail-fast integration |
| `reject_manifest_ordered_basis` | Corrupt after fixture construction, then snapshot; require the existing complete manifest diagnostic | Complete validation |
| `reject_manifest_input_factor` | Independently corrupt input recorded factor and require exact full diagnostic | Descriptor |
| `reject_manifest_paper_divisor` | Independently corrupt `q_div` and require exact full diagnostic | Descriptor |
| `reject_manifest_high_scale` | Independently corrupt logical high scale and require exact full diagnostic | Descriptor |
| `reject_manifest_recombined_scale` | Independently corrupt recombined logical scale and require exact full diagnostic | Descriptor |
| `reject_high_level` | Corrupt only high level, exact diagnostic, no arithmetic or mutation | Member state |
| `reject_low_level` | Corrupt only low level, exact diagnostic, no arithmetic or mutation | Member state |
| `reject_high_recorded_factor` | Corrupt only high current factor, exact diagnostic, no arithmetic or mutation | Member state |
| `reject_low_recorded_factor` | Corrupt only low current factor, exact diagnostic, no arithmetic or mutation | Member state |
| `reject_high_noise_degree` | Corrupt only high degree, exact diagnostic, no arithmetic or mutation | Member state |
| `reject_low_noise_degree` | Corrupt only low degree, exact diagnostic, no arithmetic or mutation | Member state |
| `reject_high_aggregate_format` | Corrupt only high aggregate format, exact diagnostic | Member state |
| `reject_low_aggregate_format` | Corrupt only low aggregate format, exact diagnostic | Member state |
| `reject_high_native_tower_format` | Corrupt one named high NativePoly tower while aggregate format remains valid | Member state |
| `reject_low_native_tower_format` | Corrupt one named low NativePoly tower while aggregate format remains valid | Member state |
| `reject_high_tower_basis` | Corrupt only high ordered basis identity, not only count | Member state |
| `reject_low_tower_basis` | Corrupt only low ordered basis identity, not only count | Member state |
| `invalid_lifecycle_source_gate` | Token/AST gate proves the complete lifecycle validation/dispatch rejects every invalid enum value on all paths, without requiring a particular `switch`/`if` syntax or adding a friend/setter solely to make an unreachable runtime fixture; execute it outside CTest | Static gate (excluded from runtime count) |

When an inherited guard makes a newly registered regression green on its first
execution, record an inherited green honestly. Do not weaken the fixture or
inject a fake failure to manufacture a red.

## Independent coefficient oracle

The expected coefficients must not use public Rescale, ModReduce,
LevelReduce, RS2, or RCB. For each component and coefficient, a test-owned
`cpp_int` implementation uses:

```text
Q  = product(input ordered basis)
q  = input ordered basis.back()          // exact active q_l
Q' = Q / q
h  = Center_Q(CRT(input.high))
l  = Center_Q(CRT(input.low))
RS(x) = (x - Center_q(x)) / q

A = RS(h)
z = Center_Q(q_div * h + l)
C = RS(z)
B = C - q_div * A  (mod Q')
```

Compare `A mod q_i` and `B mod q_i` against every remaining actual tower and
compare public RCB's result against `C mod q_i`. The oracle may convert only a
clone of each DCRTPoly to Coefficient format and must prove every original
input/output aggregate polynomial and every NativePoly tower remains in
Evaluation format.

For odd `q=2t+1`, construct named witness coordinates with:

```text
high = k_h * q + c_h
z    = k_z * q + c_z
low  = z - q_div * high
c_h, c_z in {+t, -t}
```

Cover normalized residues `t` and `t+1`, positive and negative quotient carry,
both RLWE components, nonzero high/low contributions, and a runtime-proved
coordinate where `B != RS(low)`. Random search, decrypt-only comparison, or
the recombination identity alone is insufficient.

Before invoking production, the fixed fixture must also prove in the actual
stored numeric representations that `q_l`, `q_div`, and `baseSF` are pairwise
distinguishable and that the relevant `/q_l`, `/q_div`, and `/baseSF` expected
results are distinguishable at named assertions. A fixture where two wrong
divisors round or serialize to the same expected field cannot validate the
division contract.

## Complete state oracle

For the finally observed `ReadyForRS2` input, freeze its actual ordered basis,
level 1, noise-scale degree 3, current recorded factor `SF_T`, lifecycle, and
descriptor fields. The output must prove independently:

- ordered basis equals the exact input prefix and removes exactly active
  `q_l`, while the manifest's paper divisor `q_div` is unchanged;
- level `1 -> 2` and degree `3 -> 2`;
- current recorded factor is `SF_T/baseSF`, not `SF_T/q_l`;
- output `PaperScaleDescriptor::inputRecordedScalingFactor` remains the
  transition input value `SF_T`; the `AfterFirstRS2` validator must check it
  separately from the pair's current recorded factor `SF_T/baseSF`;
- logical high and recombined scales are each divided by exact `q_l`, not by
  `baseSF` or `q_div`;
- lifecycle is the finally accepted after-first-RS2 state;
- both members have two RLWE components, CKKS encoding, identical
  aggregate/per-tower Evaluation format, and each member's context pointer,
  actual key tag, slots, and encoding separately equal the corresponding
  validated input source. Merely proving both outputs share the same wrong
  value is insufficient.

The explicit `module.RCB(rs2Output)` integration check must validate the RCB
return's complete ordered basis, level, degree, current recorded factor,
context pointer, actual tag, slots, encoding, aggregate/per-tower format, and
high-member metadata provenance before comparing all coefficients to the
independent `C`.

## Metadata and cache boundaries

Use distinct high-only and low-only sentinels. Snapshot the input outer-map
identity, every value-pointer identity, and every deep value before production.
Both output outer maps must be new containers and both sets of metadata values
must have the finally verified high-member provenance. The presently expected
OpenFHE boundary is shallow sharing of value pointers with input high, not
project-owned deep isolation; rebind that claim to actual Relin2 output first.

The no-evaluation-key test removes keys only after a valid input exists. Its
RAII guard records and restores the full tag entry, vector order, context,
polynomial A/B state, and pointer identities. RS2 must neither query, generate,
insert, erase, reorder, nor restore a key-cache entry.

For every production call, complete pair and deep-cache snapshots must be the
last two observations before the call. Their comparisons must be the first two
observations after return or after the exact exception assertion.

## Production call-order/source gate

Apply a token/AST-aware gate to the complete `DoubleCKKS::RS2` body and its
transitive project helpers. Require:

1. complete `ValidatePair`, then lifecycle, active-basis, and
   composite-degree-one checks; the source gate must identify and whitelist
   exactly the transitive `ValidatePair` validation closure, whose raw
   member/tower reads are allowed only after its null and shape gates;
2. no RCB, Rescale, clone, **non-validation** raw element access, or arithmetic
   before those checks finish; element/tower reads inside the whitelisted
   validation closure remain permitted because complete manifest validation
   requires them;
3. exactly one accepted RCB and exactly two output-returning public Rescale
   calls;
4. no rescale of low, in-place Rescale, LevelReduce, private ModReduce,
   approximate scalar EvalMult, indirect callable, or key/cache route;
5. exact `q_div` multiplication through
   `DCRTPoly::operator*=(NativeInteger)`;
6. complete compatibility recheck before public EvalSub so automatic level
   alignment cannot hide an error;
7. no manual metadata/level/degree/factor repair and no production
   `try`/`catch`.

All executable source invocations must be discovered and classified in both
directions; a finite receiver-name grep is not a proof of completeness.

## Provisional diagnostics

These strings are suitable candidates but remain unfrozen until the actual
Relin2 lifecycle/validator is audited:

```text
DoubleCKKS: RS2 input lifecycle must be ReadyForRS2
DoubleCKKS: RS2 input must contain at least three active Q towers
DoubleCKKS: RS2 requires composite degree one
DoubleCKKS: pair lifecycle is invalid
```

Every exception test must bind and require the final exact exception type as
well as compare the complete `exception.what()` string. The provisional type
is `std::invalid_argument`; a different derived/base exception or a generic
`std::exception` catch cannot pass accidentally. Substring matching is
forbidden. Reuse existing manifest/member diagnostics where the same validator
field already has an accepted exact message.

## Red-green order

1. Close the accepted Relin2 exact-base/manifest gate.
2. Compile red only for the appended lifecycle symbol and exact RS2 API type.
3. Add an immediate-throw scaffold; all inherited tests remain green.
4. Add the complete manifest/member/composite validation tests, then make only
   that validation boundary green.
5. Add the wrong-lifecycle runtime case after validation is green, then add the
   minimal lifecycle guard. Run the invalid-enum source gate at both boundaries
   and distinguish an authentic new red from an inherited green; rerun it after
   every later source-changing boundary and on the final exact SHA.
6. Add one complete valid arithmetic/state/RCB test; it remains red at the
   scaffold until the smallest complete formula makes its unchanged oracle
   green.
7. Add metadata, cache, and after-lifecycle regressions, distinguishing new red
   from inherited green.
8. Prove final unique CTest name/command identity and same-SHA Linux/Windows
   green before any acceptance claim.
