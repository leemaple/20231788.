# RS2 TDD preflight

Recorded: 2026-09-01 Asia/Shanghai

Status: **provisional test design only**. This is not an implementation task,
test patch, executed red/green result, or accepted diagnostic contract. Every
runtime fixture and count must be rebound to the final accepted Relin2
commit/tree under the `Deferred exact-green gate` section of
`rs2-preflight.md`.

The accepted Tensor2 commit `fb862a3...` does not yet contain Relin2 or an
actual `ReadyForRS2` output. It therefore cannot prove that a future failing
test is red only for missing RS2. The executable baseline must be the exact
Relin2 commit that has passed same-SHA Linux/Windows verification and review.

## Minimal executable matrix

Use one dispatching executable `tests/rs2_test.cpp <case>` for exactly 24
runtime cases, with every runtime negative independently registered in CTest.
Keep `rs2_api_contract_test` compile-only and outside CTest. The
`invalid_lifecycle_source_gate`, `short_active_basis_source_gate`, and
`rs2_call_order_source_gate` rows are three separately executed token/AST
audits, not CTest registrations, so none changes the symbolic `B+24` final
runtime count.

| Case | Mechanical obligation | Oracle class |
|---|---|---|
| `valid_coefficients_state_immutability` | Every member/component/tower/coefficient of `(A,B)`; named half-boundary and carry witnesses; `B != RS(low)`; complete output basis/state/scales/lifecycle; input and deep cache immediately unchanged; an explicit `module.RCB(rs2Output)` returns coefficient-exact `C`, has complete expected state/high-metadata provenance, and does not mutate the pair | Independent coefficient oracle plus state/lifecycle/public-RCB integration contract |
| `metadata_provenance` | Input outer-map, value-pointer, and deep values unchanged; output maps distinct from each other/input; keys/values come from input high; value pointers follow the verified OpenFHE shallow-clone contract; input-low-only metadata does not propagate | OpenFHE integration contract |
| `no_eval_key_dependency` | Create a valid input, remove only that tag's EvalMult keys, require RS2 to pass the independent oracle, prove cache remains absent, then deep-restore tag/vector/context/A/B and pointer identity through RAII | Integration plus static route gate |
| `reject_ready_for_first_mult` | Reject a DCP pair before arithmetic; pair/cache unchanged | Lifecycle |
| `reject_after_first_rs2` | Reject an RS2 result passed to RS2 again | Lifecycle |
| `tensor2_reject_after_first_rs2` | Tensor2 rejects the state while RCB accepts that same object | Lifecycle boundary |
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
| `invalid_lifecycle_source_gate` | Token/AST gate proves the complete lifecycle validation/dispatch rejects every invalid enum value on all paths with the rebound exact `std::invalid_argument` type and complete `DoubleCKKS: pair lifecycle is invalid` diagnostic, without requiring a particular `switch`/`if` syntax or adding a friend/setter solely to make an unreachable runtime fixture; execute it outside CTest | Static gate (excluded from runtime count) |
| `short_active_basis_source_gate` | Token/AST gate discovers every `ReadyForRS2` construction route and proves `RS2` has a fail-fast `<3` active-tower guard with the rebound exact `std::invalid_argument` type and complete `DoubleCKKS: RS2 input must contain at least three active Q towers` diagnostic after complete validation; before patch 06, when no RS2 lifecycle guard exists, it requires `ValidatePair -> short basis`; once the exact lifecycle guard exists, it requires `ValidatePair -> exact lifecycle -> short basis`; at every boundary it requires the short guard before the composite-degree check, RCB, Rescale, cloning, or arithmetic. The accepted public API cannot construct the malformed two-tower state, so this remains static-only and must not add a friend, setter, layout hack, or CTest fixture | Static gate (excluded from runtime count) |
| `rs2_call_order_source_gate` | Token/AST def-use gate proves the exact fail-fast order and complete dataflow from input high/accepted RCB through exactly two public output-returning Rescale calls, exact NativeInteger multiplication, compatible EvalSub, and returned `(A,B)`; rejects dead trusted calls plus an independent manual substitute; execute it outside CTest | Static gate (excluded from runtime count) |

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
- pair-manifest `contextIdentity`, bound `divisor`, ordered basis, level,
  current factor, degree, lifecycle, key tag, slots, format, and component
  count are each asserted directly; the bound divisor is checked separately
  from the descriptor's paper divisor even though both equal exact `q_div`;
- both members have two RLWE components, CKKS encoding, identical
  aggregate/per-tower Evaluation format, exact prefix basis, level 2, degree 2,
  and current factor `SF_T/baseSF`; each member's context pointer, actual key
  tag, slots, encoding, level, degree, factor, shape, aggregate format, and
  every component/tower field is asserted separately against the corresponding
  validated input/expected state. Merely proving both outputs share the same
  wrong value is insufficient.

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
3. exactly one accepted RCB, exactly two output-returning public Rescale calls,
   and exactly one output-returning public EvalSub call;
4. exact def-use flow: the input high member is the sole argument to the first
   Rescale; the accepted RCB return is the sole argument to the second Rescale;
   the first returned ciphertext is preserved as output `A`; a clone of that
   same return is multiplied by exact `q_div`; the second returned ciphertext
   is the minuend and the multiplied clone is the subtrahend of the sole public
   EvalSub; that subtraction return is output `B`; and the returned pair is
   exactly `(A,B)`;
5. no discarded/dead RCB, Rescale, multiplication, EvalSub, or pair-construction
   result and no parallel manual/private arithmetic path that merely accompanies
   counted trusted calls;
6. no rescale of low, in-place Rescale, LevelReduce, private ModReduce,
   approximate scalar EvalMult, indirect callable, or key/cache route;
7. exact `q_div` multiplication through
   `DCRTPoly::operator*=(NativeInteger)`, applied to a clone whose sole source is
   the validated first-Rescale return;
8. before cloning, multiplication, or EvalSub, separately validate each Rescale
   return's context pointer, actual tag, slots, CKKS encoding, aggregate and
   every native tower's Evaluation format, exact ordered basis, two-component
   shape, level, noise-scale degree, and current recorded factor;
9. completely recheck those same compatibility fields between the two EvalSub
   operands before public EvalSub so automatic level alignment cannot hide an
   error;
10. construct the exact returned `(A,B)` pair, run the complete accepted
    `ValidatePair` closure on that exact object after construction and before
    return, and cover every `AfterFirstRS2` lifecycle/manifest/member field;
11. no manual metadata/level/degree/factor repair and no production
    `try`/`catch`.

### Mandatory static-audit mutation proof

The lifecycle, short-basis, and call-order audits need disposable red/restore
evidence at the earliest boundary where each named mutation is mechanically
constructible, plus a complete rerun on the final exact source. Freeze the
complete audit and mutation-driver bytes and SHA-256 before the first mutation. Every
mode must use a separate parse-valid copy, declare its exact expected
failure-code set before execution, print the complete discovered construction,
call, result-use, and guard graph before verdict, require the named target code
below to be present, require actual and expected code sets to be identical, and
restore the exact pre-mutation source hash. A mode may change a call count only
when its row explicitly says so. No combined or representative mutation can
stand for another row.

The static-only lifecycle and short-basis audits require these independent
modes according to the staged schedule below; modes that need arithmetic may
not be back-claimed against the immediate-throw scaffold:

| Mode | Disposable mutation | Required target code |
|---|---|---|
| `L01` | bypass the invalid-enum rejection while leaving every named lifecycle route unchanged | `invalid-enum-route` |
| `L02` | change only the invalid-enum exception type | `invalid-enum-type` |
| `L03` | change one character of the complete invalid-enum diagnostic | `invalid-enum-diagnostic` |
| `S01` | add one otherwise unclassified project construction route for `ReadyForRS2` | `ready-construction-route` |
| `S02` | remove only RS2's own active-tower guard | `short-guard-missing` |
| `S03` | change only RS2's own threshold so two active towers are accepted | `short-guard-predicate` |
| `S04` | move only the short guard before complete `ValidatePair` | `short-order-validation` |
| `S05` | on patch 06 or later, move only the short guard before exact lifecycle validation | `short-order-lifecycle` |
| `S06` | move only the short guard after the composite-degree check | `short-order-composite` |
| `S07` | move one accepted RCB call before the short guard | `short-order-rcb` |
| `S08` | move one public Rescale call before the short guard | `short-order-rescale` |
| `S09` | move one ciphertext clone before the short guard | `short-order-clone` |
| `S10` | move one non-validation arithmetic expression before the short guard | `short-order-arithmetic` |
| `S11` | change only the short-guard exception type | `short-guard-type` |
| `S12` | change one character of the complete short-guard diagnostic | `short-guard-diagnostic` |

Construction-route defense is independent of RS2's own defense. On the final
accepted Relin2 base, the audit must enumerate every project source location
that constructs `ReadyForRS2`, bind its file/range and source hash, and generate
two independent modes for each route: remove only that route's short-basis
guard (`construction-short-guard-missing`) and relax only its threshold to
accept two towers (`construction-short-guard-predicate`). The presently known
Relin2 route binds `C01` to removal with required code
`construction-short-guard-missing` and `C02` to threshold relaxation with
required code `construction-short-guard-predicate`. If final AST discovery
finds any additional route, append the next two separately named modes for that
exact file/range and update every exact count/range before the RS2 task is
frozen. `S01` proves that a newly inserted route cannot escape discovery; it
does not replace any per-route guard mutation.

The call-order/def-use audit requires these independent modes on the first
green arithmetic source and again on the final exact source:

| Mode | Disposable mutation | Required target code |
|---|---|---|
| `R01` | remove the sole RCB call and provide a disposable substitute so the source remains parse-valid | `rcb-count` |
| `R02` | add a second discarded RCB call | `rcb-count` |
| `R03` | remove one Rescale call and provide a disposable substitute | `rescale-count` |
| `R04` | add a third discarded Rescale call | `rescale-count` |
| `R05` | replace only the first Rescale input with the wrong pair member | `first-rescale-source` |
| `R06` | replace only the second Rescale input with a value not returned by accepted RCB | `second-rescale-source` |
| `R07` | swap only the exact EvalSub minuend and subtrahend | `evalsub-operands` |
| `R08` | replace only returned `A` with another in-scope ciphertext alias | `return-pair` |
| `R09` | replace only returned `B` with another in-scope ciphertext alias | `return-pair` |
| `R10` | keep one RCB call but discard its return and feed the second Rescale from a manual substitute | `rcb-result-use` |
| `R11` | keep two Rescale calls but discard the first return and feed `A` from a manual substitute | `first-rescale-result-use` |
| `R12` | keep two Rescale calls but discard the second return and feed EvalSub from a manual substitute | `second-rescale-result-use` |
| `R13` | discard the exact NativeInteger multiplication result and feed EvalSub from a substitute | `multiply-result-use` |
| `R14` | discard the EvalSub return and feed `B` from a substitute | `evalsub-result-use` |
| `R15` | add one discarded pair construction while preserving the returned correct pair | `pair-result-use` |
| `R16` | move one non-validation member/tower read before complete `ValidatePair` | `validation-order` |
| `R17` | move one raw read inside the validation closure before that helper's null/shape gate | `validation-closure` |
| `R18` | move exact lifecycle validation after the first trusted call | `lifecycle-order` |
| `R19` | move the short-basis guard after the first trusted call | `short-order` |
| `R20` | move the composite-degree-one guard after the first trusted call | `composite-order` |
| `R21` | swap only the short-basis and composite-degree guards | `guard-order` |
| `R22` | move one ciphertext clone before all fail-fast guards complete | `clone-order` |
| `R23` | keep `DCRTPoly::operator*=(NativeInteger)` but replace exact `q_div` with another NativeInteger | `q-div-binding` |
| `R24` | replace `DCRTPoly::operator*=(NativeInteger)` with an out-of-place polynomial multiply and assignment | `native-multiply-technique` |
| `R25` | move the complete compatibility recheck after EvalSub | `compatibility-order` |
| `R26` | replace one non-in-place public Rescale route with in-place Rescale | `forbidden-inplace-rescale` |
| `R27` | add a `LevelReduce` route | `forbidden-level-reduce` |
| `R28` | add a private/internal ModReduce route | `forbidden-private-modreduce` |
| `R29` | replace exact NativeInteger scaling with approximate scalar `EvalMult` | `forbidden-approx-evalmult` |
| `R30` | add a parallel manual CRT/rescale arithmetic path while preserving trusted-call counts | `forbidden-manual-path` |
| `R31` | route one trusted call through a local function pointer | `forbidden-indirect-call` |
| `R32` | route one trusted call through `std::invoke` and a member pointer | `forbidden-std-invoke` |
| `R33` | add one evaluation-key-cache query | `forbidden-key-cache` |
| `R34` | add one evaluation-key-cache mutation | `forbidden-key-cache` |
| `R35` | add one manual metadata repair | `forbidden-metadata-repair` |
| `R36` | add one manual level repair | `forbidden-level-repair` |
| `R37` | add one manual noise-scale-degree repair | `forbidden-degree-repair` |
| `R38` | add one manual recorded-factor repair | `forbidden-factor-repair` |
| `R39` | wrap production arithmetic in a `try`/`catch` | `forbidden-try-catch` |
| `R40` | express one additional executable call through an ordinary function-like macro | `call-discovery-macro` |
| `R41` | express one additional executable call through an object-like macro | `call-discovery-object-macro` |
| `R42` | express one additional executable call through token pasting | `call-discovery-token-paste` |
| `R43` | express one additional executable call through a local function object | `call-discovery-function-object` |
| `R44` | add one previously unknown direct project-helper call | `call-classification` |
| `R45` | remove the sole EvalSub call and provide a disposable substitute | `evalsub-count` |
| `R46` | add a second EvalSub and use its return in the final pair | `evalsub-count` |
| `R47` | keep exact NativeInteger multiplication but clone its operand from a value other than the first-Rescale return | `multiply-source` |
| `R48` | express one additional executable call inside an immediately invoked capturing lambda | `call-discovery-capturing-iife` |
| `R49` | express one additional executable call inside a named capturing lambda and call that lambda | `call-discovery-called-lambda` |
| `R50` | express one additional executable call in a callable's default-argument expression | `call-discovery-default-argument` |
| `R51` | express one additional executable call in a lambda init-capture | `call-discovery-init-capture` |
| `R52` | express one additional executable call through direct pointer-to-member invocation syntax | `call-discovery-member-pointer` |
| `R53` | embed one additional executable call inside a wrapper already classified as allowed | `call-discovery-classified-wrapper` |

Three exhaustive field matrices are also mandatory; no one field or one result
may represent another:

- `V(result,field)` independently removes or corrupts `field` validation for
  each `result` in `{first-rescale, recombined-rescale}` and every field in
  `{context, actual-tag, slots, CKKS-encoding, component-count, level,
  noise-scale-degree, recorded-factor}`, plus each component's aggregate format
  and ordered-basis identity and every actual `(component,tower-index)` native
  format cell. If production shares a validator, the
  disposable mutation must route only the named result through a copied helper
  missing the named field so both result-specific gates are proved. Separate
  structural modes remove each result validation, validate a wrong alias, and
  move validation after that result's first arithmetic consumer (multiplication
  for the first result and EvalSub for the recombined result). An additional
  first-result structural mode changes only `validate -> clone -> multiply` to
  `clone -> validate -> multiply`; neither the later arithmetic-order mode nor
  `R22` may stand for it.
- `K(field)` independently removes or corrupts the pre-EvalSub compatibility
  recheck for every field in the same exact scalar set, every component's
  aggregate format and ordered-basis identity, and every actual
  `(component,tower-index)` native format cell. `R25` proves placement; it
  cannot stand for any `K(field)` completeness mode.
- Two independent compatibility-binding modes replace only the checked left
  operand and only the checked right operand with a wrong in-scope alias while
  leaving the actual EvalSub operands unchanged. They require distinct
  `compatibility-left-binding` and `compatibility-right-binding` target codes;
  field completeness and placement modes cannot stand for either binding.
- `P(field)` independently removes or corrupts the `AfterFirstRS2` output
  validator predicate for each distinct pair-manifest field in
  `{context-identity, bound-divisor, manifest-ordered-basis, manifest-level,
  manifest-current-recorded-factor, manifest-noise-scale-degree, lifecycle,
  key-tag, slots, format, component-count, descriptor-input-recorded-factor,
  descriptor-paper-divisor, logical-high-scale, logical-recombined-scale}`;
  independently for each member in `{high,low}` and each member field in
  `{context, actual-tag, slots, CKKS-encoding, ciphertext-element-count, level,
  noise-scale-degree, recorded-factor}`; plus each member/component aggregate
  format and ordered-basis identity and every actual
  `(member,component,tower-index)` native format cell. The pair manifest's
  bound divisor and descriptor paper divisor are separate cells, as are every
  high/low member field and the similarly named manifest field. Separate
  structural modes remove the final `ValidatePair`, validate a wrong pair, and
  return before validation while leaving a dead validation call behind.

Before freezing the RS2 task, expand every `V`, `K`, and `P` Cartesian cell and
each structural mode into an exact unique mode name and count in the byte-frozen
driver. Every cell must require its own result-specific target code; shared or
combined mutations are forbidden.

All executable source invocations and every `ReadyForRS2` construction must be
discovered and classified in both directions; a finite receiver-name grep is
not a proof of completeness. Retain the byte-frozen audit and driver, every
mutation transform or fixture, complete raw logs, exact expected/actual
failure-code table, pre/mutated/restored hashes, and final green output. Patch
08 acceptance requires the full `S01`-`S12`, all construction-route modes,
`R01`-`R53`, and expanded `V`/`K`/`P` proof; final acceptance requires the full
three-audit mutation proof on the final exact source. No mutated bytes may enter
an applied patch, commit, build, or hosted run.

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
4. Add the complete manifest/member/composite validation tests and the
   `short_active_basis_source_gate`. The static short-basis gate must red on the
   scaffold's missing or misordered guard, then become green with only the
   complete validation/short-basis/composite boundary. At this boundary its
   order is `ValidatePair -> short basis -> composite degree` because the exact
   RS2 lifecycle guard does not exist yet. Retain the first-green proof for
   `S01`-`S04`, `S06`, `S11`-`S12`, and every discovered construction-route
   mode; `S05` and `S07`-`S10` are not yet constructible.
5. Add the wrong-lifecycle runtime case after validation is green, then add the
   minimal lifecycle guard. Run the invalid-enum source gate at both boundaries
   and distinguish an authentic new red from an inherited green. If it is an
   inherited green, retain `L01`-`L03` at patch 05; otherwise retain them at the
   first authentic green after patch 06. At patch 06, rerun the unchanged
   short-basis gate, require its stricter final order `ValidatePair -> exact
   lifecycle -> short basis -> composite degree`, and retain `S05`; arithmetic
   modes `S07`-`S10` remain deferred.
6. Add one complete valid arithmetic/state/RCB test and the complete
   `rs2_call_order_source_gate`; both remain red at the scaffold. The runtime
   test fails through its unchanged complete oracle, while the source gate must
   report the exact missing trusted-call/dataflow set rather than stop at a
   token count.
7. Add the smallest complete formula. The unchanged runtime oracle and unchanged
   call-order/def-use gate must both become green; rerun the source gate after
   every later production-source boundary. At this first complete arithmetic
   surface, run and retain the full `S01`-`S12`, every construction-route mode,
   `R01`-`R53`, and expanded `V`/`K`/`P` mutation/restore proof.
8. Add metadata, cache, and after-lifecycle regressions, distinguishing new red
   from inherited green.
9. Rerun all three static audits after every later production-source boundary;
   on the final exact source rerun and retain the complete `L01`-`L03`,
   `S01`-`S12`, every construction-route mode, `R01`-`R53`, and expanded
   `V`/`K`/`P` mutation/restore proof. Then prove unique CTest name/command
   identity and same-SHA Linux/Windows green before any acceptance claim.
