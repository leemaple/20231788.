# Test design and oracle rationale

## Narrow change surface

Only `tests/experimental_precision116_eight_square_test.cpp` is added; only a final option-gated block is appended to `CMakeLists.txt`. Existing 85-file input content is otherwise preserved. The dedicated executable links the existing production library, not the original endpoint writer/observer implementation. It takes no alternate modes and calls neither original nor seam runners.

Reused items are frozen input generation/conversion, binary512 complex arithmetic, exact-integer arithmetic, anchor roots and direct Horner from `paper_full_eight_square_oracle.h`; frozen candidate literals, public profile/cipher/key checks, expected-exception checks and weak ownership helpers from `experimental_precision116_profile_seam.h`. The original-profile `ReadSecret`, `SparseDecrypt`, `RecombinedPolynomial`, `Scales` and original .cpp structural checks are not invoked. The seam's nonterminal-only pair/receipt checks are not invoked. Neither old output protocol is emitted.

## Exact candidate and scale oracle

`xp::kQ`, `xp::kP` and `xp::kWitness` are the already frozen test literals—not plan/receipt/ciphertext-derived expectations. Actual families, native towers, roots, P/QP, metadata58, HYBRID tables, key rows, root identity and signed weight128 are independently compared to them. Candidate-specific polynomial recovery takes an independently determined expected tower count; ciphertext dimensions are never used to choose a more convenient oracle modulus.

For r=0..8, the test computes the reduced rational

```
S0 = 2^116
Sr = 2^(116 * 2^r) /
     product(j=1..r, (frozen_Div * frozen_Q[10-j])^(2^(r-j)))
```

Thus Q[9]..Q[2], i.e. Mult7..Mult0, are consumed. The closed product is computed independently of production's recurrence and compared by exact numerator AND denominator equality against every exposed receipt. No receipt or plan value populates the oracle. The source-level pure arithmetic check here matched all nine frozen certificate rationals. S8/S0 is approximately 1.00001520269329669112; that is a parameter calculation, not a measured ciphertext error.

Each operation additionally checks its Rescaled → Relinearized → Tensor → previous-returned ancestry, using exact expected Tensor/Relinearized scale Sprev²/d. The terminal chain has exactly32 distinct nodes: Input + seven Reentries + eight each of Tensor, Relinearized and Rescaled. Every phase/family/operation/terminal flag, reduced scale and parent identity is checked. Actual returned receipt numerators/denominators are printed without binary64 conversion.

## Physical state mapping

| Observation | Family / phase | Local or absolute level | Active towers | Recorded scale / lifecycle |
|---|---|---:|---:|---|
| Fresh bound ciphertext | Root family0 | absolute0 | 11 | 2^116 / fresh |
| DCP round0 | family0 Input | local1 | 10 | 2^116 / ReadyForFirstMult |
| Returned rounds1–7 | family=r Reentry | local1 | 10-r | 2^116 / ReadyForRepeatedMult |
| Returned round8 | family7 terminal Rescaled | local2 | 2 | 2^116 / RefreshRequired |
| Terminal result/bound | Root family0; same terminal receipt | absolute9 | 2 Bases | 2^116 / RepeatedMult2Rcb |

Every returned high/low ciphertext is checked against the literal prefix, actual native roots and formats, context/parameter/tag identity, slots, arity, level, degree, integer scaling field and empty metadata. High/low wrappers/maps must be distinct. The exact logical scale is not replaced by the recorded value. Compatibility approximate fields are checked finite/positive; exact normalization is checked through the rational receipts. The source's transient Tensor recorded174 is not falsely presented as a directly retained ciphertext observation: the START profile line names it `expected_tensor_recorded_exp2`.

## One chain and client-only oracle

One candidate factory call samples one root; one production Encrypt takes exactly `paper_full_test::Inputs()` through the original high-precision conversion at S0. These original dyadics are representable in the binary512 input generator; no binary64 input construction, easier vector, reduced slot set or sample selection is introduced.

`Evaluate(plan, ciphertext)` receives no secret, client, callback or independent polynomial. It performs DCP once, eight public `Mult2(pair,pair)` operations, and terminal `RCBWithReceipt`. It checks public state and exact receipt ancestry, preserves operand/input snapshots, and verifies nonterminal Input and first-Reentry rejection. It returns the initial pair, eight returned pairs and terminal result. All secret/oracle work is performed client-side before or after this call. Incomplete evaluation may therefore leave fresh measurements and completed-round structural/scale lines, but not later client-side round-anchor observations; those partial facts are not full-chain results.

The candidate-specific oracle recovers the same signed h128 vector from all eleven actual root towers. For each observed ciphertext it switches each native c0/c1 tower through official inverse NTT, explicitly computes signed sparse negacyclic c0+c1*s modulo X^32768+1, and uses test-local exact CRT and centering. It calls no production Decrypt/DecryptCore, ring multiply, CRTInterpolate, FFT or codec to obtain the polynomial. Because q<2^60, modular additions are below2^61 and use portable uint64_t; composite products, CRT weights and scales use cpp_int.

Pair polynomials are reconstructed as Center(d*high+low,Q) using the independently checked active prefix. Binary512 direct Horner evaluates the original ten roots for anchors `{0,1,256,257,512,513,768,769,1023,16383}` and divides by the independently derived Sr. The ideal evolves by binary512 complex squaring of the ORIGINAL input, not of decrypted fresh values.

Fresh and final production Decrypt are the only two production decryptions. There is no intermediate production decryption, refresh, bootstrap, re-encryption or second encrypted chain. An additional exact round0 check compares DCP recombination with reduction of the independently recovered fresh polynomial. Final root-wrapper recovery must equal the independently recovered family7 recombined polynomial exactly. Endpoint CRT modulus/max-centered-coefficient/headroom diagnostics must also agree exactly with production.

## Frozen numerical predicates

| Gate | Implemented predicate |
|---|---|
| Both full endpoints | Max absolute real OR imaginary component error over ALL16384 slots <=2^-80 against original input / independent z^256 |
| Both codecs | Finite, nonnegative maximum cross-precision disagreement <=2^-120 |
| Both endpoint headrooms | Actual centered headroom >0; independent CRT diagnostic agreement |
| Both actual witnesses | actual(slot1.real-slot0.real)>2^-76 AND abs(actual_delta-expected_delta)<=2*2^-80 |
| Final expected witness | 2^-71<expected_delta<2^-70 |
| Published slot0 expectations | Real and imaginary published decimal scalars each agree within strict2^-150, unchanged literals |
| Final ideal domain | For every slot, .098²<ideal squared magnitude<.106² |
| Final actual domain | For every slot, actual squared magnitude>.09² |
| Independent anchors | Fresh, round0, each returned round1–8 and final max component error<=2^-80 against original-input ideal |
| Production/oracle anchors | Fresh and final disagreement<=2^-80 |
| Wrong nominal normalization | Horner of the ACTUAL independent final polynomial divided by 2^116, compared to ideal final anchor0, must have component error>2^-30 |
| Finite/domain consistency | Nonfinite values, malformed shapes/bases/receipts, inconsistent expected oracle or invalid states abort |

The wrong-scale control is not implemented as merely S8!=S0, or as an ideal-polynomial prediction. It operates on the independently recovered actual terminal polynomial. The pure static S8/S0 check does not exercise this runtime falsifier.

All component maxima, worst endpoint slot/component, ten-anchor errors, endpoint production/oracle disagreement, witness differences/disagreements, headroom, final minimum actual norm and failing-slot count, wrong-scale error, and per-round actual scales are written to a distinct experimental stdout label. Errors are reported at 100 scientific digits without relaxing the underlying comparisons. No observer schema, JSON writer, endpoint publisher or runtime ZIP is introduced.

## Failure classification and cleanup

Finite E80, actual-witness and actual-domain misses accumulate, allowing a valid chain to reach all remaining observations. Structural/receipt/root/CRT consistency failures, nonfinite values, codec disagreement, nonpositive headroom, invalid expected-domain/scalar checks and a non-discriminating wrong-normalization control throw at the test boundary. Their already emitted measurements remain partial facts, and the executable emits `ABORT result=INVALID_OR_INCOMPLETE numerical_result=NOT_ESTABLISHED` (exit2). This preserves hard original consistency boundaries rather than treating an invalid oracle as a numerical RED.

A completed, structurally/oracle-valid run performs candidate/receipt/ciphertext owner release and owned-row cleanup before its final result. Finite accumulated gate misses give COMPLETE/FAIL (executable exit1); no misses give COMPLETE/PASS (exit0). CTest's own exit status need not equal the executable's exit status, so classification uses the complete transcript and terminal label, not just CTest's integer. A timeout/crash/missing executable/configure failure is not a complete numerical observation.

The existing small N64 diagnostic setup remains live throughout candidate ownership checks. Only its small rows are copied; candidate row identities are tracked without retaining extra strong row owners. Foreign evaluator rejection reuses that setup. Foreign I/O rejection constructs the already supported N64/Q8 context **without keys**, rather than mutating the live diagnostic plan. All candidate and diagnostic owned rows are released; unrelated identities/coefficients, original root/public keys and input snapshots are preserved. There is no extra paper-sized keygen or chain for ownership.

## Explicit proof boundary

Ten anchors do not prove all-slot intermediate precision/nonwrap. Positive centered headroom of a centered representative does not prove absence of an earlier modular wrap. All-slot endpoints do not prove Tensor/Relin lift safety or Gaussian worst-case behavior. A single chain per host is a bounded correctness observation, not1000-trial replication, formal security, or universal reliability.
