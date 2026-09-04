# DIAGNOSIS — BV/Relin2 certificate failure

## 1. Terminal adjudication

**Decision: AMEND.**

The evidence supports three separate conclusions, which must not be collapsed into one:

1. **The current certificate contract is invalid as a theorem-level gate.** A maximum error observed in one ordinary `Relinearize` execution is not a conservative `E_Relin` for an input/key domain and cannot be substituted into Lemma 4.4 or Theorem 4.8 as though it had the paper's required quantifier.
2. **The retained BV executions also expose a real backend-to-proof mismatch.** For the pinned OpenFHE BV implementation, the error from the two `Relin2` paths is not related to the error from relinearizing their recombined operand by only the paper's `h` rounding allowance. The reverse-triangle lower bounds exceed `h` by roughly nine orders of magnitude. Thus the paper proof's near-additivity step is false for these executions of this backend.
3. **A production `Relin2` wrapper defect is not established.** Production follows the required raised-high, ordinary-high/ordinary-low relinearization, DCP, and recombination architecture. Existing exact public-seam tests and both-platform composition evidence pass. The new probe is required to check the same-input path identity directly before any green candidate is accepted.

Accordingly, this is neither a PASS nor a production-code repair. It is a test-contract correction plus an explicit implementation-theorem applicability gap. No conservative BV `E_Relin` is justified by the packet.

## 2. What actually failed

The diagnostic Linux run reaches all basis-agreement checks and reports:

| Case | Ordinary combined execution error `E_comb` | Pair execution error `E_pair` | `h` | Failed margin `E_pair-(E_comb+h)` |
|---|---:|---:|---:|---:|
| BV REAL | 197331007675 | 323602105437 | 43 | 126271097719 |
| BV COMPLEX | 181218269350 | 223094194606 | 44 | 41875925212 |

Source: `supplemental/bv-diagnostic-linux.txt:431-452`. The suite ends `42/44`; only `mult2_e2e_bv_real` and `mult2_e2e_bv_complex` fail (`supplemental/bv-diagnostic-linux.txt:455-463`).

The authoritative Windows update independently ends `42/44` in `1.18 s`, with the same two test names and the same failure message (`supplemental/matrix-red-windows.txt:456-485`). Its older matrix log does not contain the added BV numerical diagnostic, so no Windows BV magnitudes or `h` values are inferred.

HYBRID REAL and COMPLEX pass the same old inequality in the Linux diagnostic run with `(E_comb,E_pair,h)=(12,22,42)` and `(16,27,46)` (`supplemental/bv-diagnostic-linux.txt:413-429`). That is execution evidence only; it is not a proof that HYBRID satisfies the paper identity for an input domain.

The current BV tests abort at the disputed assertion before the independent final-output coefficient expression and decoded-slot checks execute. Therefore the retained logs do **not** establish that the current BV final output passes or fails those later checks.

## 3. Exact equations

Let:

- `Q = Q_l`, the seven-tower active modulus;
- `Q+ = q_div * Q`, the eight-tower raised basis;
- `T = (H,L)` be the three-component Tensor2 pair;
- `Dec_3(z) = z_0 + z_1 s + z_2 s^2`;
- `Dec_2(c) = c_0 + c_1 s`;
- `Rel_Q` be ordinary relinearization on basis `Q`;
- brackets denote centered reduction coefficientwise.

Define the ordinary relinearization error for one input by

```text
epsilon_Q(z) = [ Dec_2(Rel_Q(z)) - Dec_3(z) ]_Q.
```

Production `Relin2` performs the following operations (`project/src/double_ckks.cpp:807-856`):

```text
U = Rel_Q+( lift_Q+(q_div * H) )
(A,R) = DCP_q_div(U)            so q_div*A + R == U|_Q  (mod Q)
V = Rel_Q(L)
Relin2(T) = (A, R + V).
```

Independent recombination therefore gives the conditional identity

```text
Dec_2(RCB_q_div(Relin2(T)))
  = Dec_2(U|_Q) + Dec_2(V)                         (mod Q),
```

and hence

```text
delta_pair
  = epsilon_Q+(lift_Q+(q_div*H))|_Q + epsilon_Q(L) (mod Q).
```

For this exact key, ciphertext pair, basis, and execution, the coefficientwise triangle inequality gives a valid conditional certificate:

```text
||delta_pair||_infinity <= E_high_exec + E_low_exec,
```

where each right-hand term is measured on an independently reconstructed ordinary public path, not on the production pair result being accepted.

The disputed current call instead constructs the combined degree-three ciphertext

```text
Z = q_div*H + L  on Q
```

and measures

```text
E_comb_exec = ||epsilon_Q(Z)||_infinity.
```

It then fatally asserts

```text
||delta_pair||_infinity <= E_comb_exec + h.
```

That inequality is equivalent to requiring the paper-additivity residual

```text
rho = [delta_pair - epsilon_Q(Z)]_Q
```

to satisfy `||rho||_infinity <= h`. This is exactly the substantive proof step that must be justified; it does not follow merely from calling the left value “empirical E_Relin”.

### Retained BV executions already refute the `h` residual bound

By the reverse triangle inequality,

```text
||rho||_infinity >= abs(||delta_pair||_infinity - ||epsilon_Q(Z)||_infinity).
```

Therefore:

```text
BV REAL:    ||rho||_infinity >= 323602105437 - 197331007675
                                  = 126271097762  > 43.
BV COMPLEX: ||rho||_infinity >= 223094194606 - 181218269350
                                  =  41875925256  > 44.
```

These are lower bounds; the proposed probe computes the exact coefficientwise residual. The lower bounds are already sufficient to reject the paper's `h`-only additivity allowance for these two pinned BV executions.

## 4. What `E_Relin` means in the paper

### Quantifier

Section 2.2 states that “there exists a (small) bound `E_Relin`” for the centered coefficient error of ordinary relinearization (`PAPER-2023-1788.txt:280-310`). Lemma 4.4 and Theorem 4.8 then use the same symbol inside preconditions and conclusions (`PAPER-2023-1788.txt:690-733`, `903-945`). For that use to be a theorem-level certificate, `E_Relin` must be an a priori or otherwise justified upper bound applicable to the relevant algorithm, key/parameter event, basis, and admissible ciphertext domain. A single observed maximum is not conservative merely because it was measured exactly.

There is a second, narrower point: if the paper's near-additivity identity holds for a particular execution, the exact error of `Rel_Q(q_div*H+L)` may be used in that one execution's inequality. The retained BV failure is therefore meaningful as a conformance probe of that proof step. It is just not a universal `E_Relin` certificate.

### Input domain and basis

The ordinary combined call in the current oracle is a degree-three ciphertext on `Q_l` (`project/tests/mult2_e2e_oracle_test.cpp:654-675`). The actual `Relin2` calls are different:

- the high operand is multiplied by `q_div`, lifted to the full eight-tower `Q+`, and relinearized there; and
- the low operand is relinearized on the seven-tower `Q_l`.

Production performs exactly those calls at `project/src/double_ckks.cpp:807-852`. Any backend-specific proof must cover those actual domains and the restriction/DCP relationship; a bound from only the combined `Q_l` call does not do that.

### Units

`E_Relin` is in centered **integer coefficient units** of `R_Q`, before CKKS decoding and before division by any recorded or logical scale. The `h` term is in the same units: the paper obtains it from `s*e`, using secret Hamming weight `h` and `||e||_infinity <= 1` (`PAPER-2023-1788.txt:735-759`). It is not:

- decoded slot error;
- a number of precision bits;
- the frozen `1e-3` logical-slot threshold; or
- a security parameter.

## 5. Why pinned OpenFHE BV does not inherit the paper's proof step

The paper's displayed ordinary relinearization is a scaled key-switch followed by coefficient rounding (`PAPER-2023-1788.txt:280-310`). Its Lemma 4.4 proof uses the resulting near-linearity:

```text
Rel(q_div*H) + Rel(L) = Rel(q_div*H + L) + (0,e),
||e||_infinity <= 1,
```

then derives `||s*e||_infinity <= h` (`PAPER-2023-1788.txt:735-759`).

Pinned OpenFHE dispatches generic `Relinearize` to `KeySwitchCore` for every extra component, adds the returned two components, and truncates to two components (`official-openfhe/base-leveledshe.cpp:319-340`). For BV with `digitSize=0`:

1. evaluation-key row `i` is generated so that, up to sign convention,

   ```text
   b_i + a_i*s_new = basis_i(s_old) - noiseScale*e_i
   ```

   (`official-openfhe/keyswitch-bv.cpp:49-103`, especially `86-95`);
2. the switched component is decomposed by `CRTDecompose(0)` into one digit per **current active tower** (`official-openfhe/keyswitch-bv.cpp:245-277`);
3. `CRTDecompose(0)` copies each source-tower coefficient into the other active towers by modulus switching (`official-openfhe/dcrtpoly-impl.h:230-250`); and
4. each digit multiplies its noisy evaluation-key row and all rows are summed (`official-openfhe/keyswitch-bv.cpp:257-277`).

Writing this decomposition as `D_Q(z)_i`, the decryption error has the backend-specific form, up to sign,

```text
epsilon_Q(z) = -noiseScale * sum_i D_Q(z)_i * e_i.
```

The map `D_Q` is not integer-linear across modular carries. In general,

```text
D_Q(x) + D_Q(y) - D_Q(x+y) != 0.
```

The difference is multiplied by evaluation-key error polynomials. Nothing in the supplied BV source bounds the resulting residual by a single coefficient-rounding polynomial of infinity norm one. Consequently, two BV key switches can differ from one key switch of the sum by far more than `h` after decryption, exactly as the retained measurements show.

The high path has eight active towers before restriction while the low and combined paths have seven. That distinction must be modeled, but it is not by itself proven to be the numerical cause: with `digitSize=0` and the deliberately appended zero `q_div` residue, the extra digit may contribute zero and the active prefix may align after restriction. The decisive conclusion is narrower: the operands/decompositions differ, and the packet supplies no BV theorem that replaces the paper's rounding-additivity argument.

HYBRID is not an automatic theorem substitute. Its pinned implementation uses digit partitions, `ApproxSwitchCRTBasis`, and `ApproxModDown` (`official-openfhe/keyswitch-hybrid.cpp:314-399`). The two passing HYBRID executions show only that their observed residuals happened to fit `h`.

## 6. Ordered hypothesis adjudication

### H1 — one ordinary combined execution is not the required bound

**Status: CONFIRMED, with two distinct reasons.**

- As a universal/theorem `E_Relin`, one observed execution has the wrong quantifier.
- As an execution probe, the old inequality tests the paper's near-additivity step, and the retained BV numbers refute that step for this backend/key/ciphertext execution.

This is the confirmed cause of the current fatal gate being unsuitable as a generic BV acceptance certificate.

### H2 — production BV lift/key-switch/recombination is arithmetically wrong

**Status: NOT ESTABLISHED; backend proof compatibility is nevertheless disproved for the retained executions.**

Evidence against a wrapper-arithmetic defect:

- production `Tensor2` is the required high-high plus two cross products, excluding low-low (`project/src/double_ckks.cpp:645-694`);
- production `Relin2` validates the exact context/key/basis, raises high on `Q+`, performs the two ordinary calls, DCPs the raised result, and adds the low path (`project/src/double_ckks.cpp:696-870`);
- production `RS2` separately rescales high and recombination and derives the corrected low (`project/src/double_ckks.cpp:873-995`);
- `Mult2` is exactly `RS2(Relin2(Tensor2(left,right)))` (`project/src/double_ckks.cpp:997-999`);
- the exact Tensor2, Relin2, and RS2 public-seam tests and the composition test pass in the retained Linux and Windows matrices.

The proposed probe is still required because the current end-to-end BV run stops before later output checks. If the new coefficientwise path identity fails, H2 must be reopened immediately and 0002 must not be applied.

### H3 — independent coefficient decryption/CRT is wrong for BV

**Status: DISFAVORED, not logically eliminated by retained logs alone.**

The oracle decrypts `c0+c1*s+c2*s^2` in native towers, reconstructs with `cpp_int` CRT, and independently recombines the pair; production `RCB` is not used for that coefficient path (`project/tests/mult2_e2e_oracle_test.cpp:222-315`). It reaches ordered-basis agreement for BV, and the same machinery supports passing HYBRID cases. Separate exact seam tests also use independently implemented centered-CRT arithmetic.

Nevertheless, because no retained run yet contains the new same-input high/low path comparison, a subtle shared oracle defect cannot be declared impossible. Patch 0001 directly compares the production pair error with independently reconstructed ordinary high and low public paths coefficient by coefficient. A mismatch is a hard stop, not a reason to relax a bound.

## 7. Minimal corrective candidate

### Why there is no production patch

No supplied byte or run demonstrates that `double_ckks.cpp` deviates from the implemented Definition 4.3 architecture. Changing production arithmetic, key-switch technique, parameters, basis order, rescaling, key guards, or security/decryption protections would be speculative and violates the task's minimality boundary.

### Probe

`patches/0001-PROBE-per-path-relin2-execution-oracle.patch` changes only `tests/mult2_e2e_oracle_test.cpp` and adds:

- independent reconstruction of `lift(q_div*H)` on the full basis;
- separate public ordinary relinearization of that high operand and `L`;
- independent restriction of the high result by dropping the known final `q_div` tower;
- independent coefficient decryption of both paths;
- exact coefficientwise comparison

  ```text
  actual Relin2 pair error == high-path error + low-path error  (mod Q_l);
  ```

- per-path maxima and their triangle bound; and
- the exact coefficientwise paper-additivity residual.

The old fatal `E_comb+h` assertion remains after these probes, so the historical BV red is not rewritten as a pass.

### Conditional green candidate

`patches/0002-GREEN-separate-execution-certificate.patch`, applied only after a successful hosted probe, does the minimum test-contract correction:

- the separate combined-call error remains diagnostic;
- `paper_additivity_residual <= h` becomes an explicitly reported, non-universal observation;
- the accepting execution certificate uses `E_high_exec + E_low_exec`, measured on independently constructed paths rather than from the production result error;
- the same conditional bound replaces the invalid sample `E_Relin+h` in the non-wrap witness and corrected execution-specific coefficient expression; and
- the output labels the result `PER_PATH_CONDITIONAL` and the theorem gate `UNPROVED`.

The final changed file is only:

```text
tests/mult2_e2e_oracle_test.cpp
```

Its candidate SHA-256 is:

```text
4be87bd5b16f5139b7b39d09143865ff1d4df60b4c533ddd08407d5f99da8c14
```

Vectors, `1e-3` decoded threshold, `p`, `N`, key-switch selections, key guards, production code, and all 44 test registrations are unchanged.

## 8. Why the candidate is not circular arithmetic acceptance

The per-path triangle is an execution certificate, not the sole arithmetic oracle. Wrong implementation arithmetic remains gated independently by:

- Tensor2's coefficient/tower negacyclic-product oracle;
- Relin2's exact raised-high, ordinary-path, DCP quotient/remainder, state, and immutability checks;
- RS2's independent centered division/rounding and recombination oracle;
- Mult2's exact named-composition equality;
- independent end-to-end `cpp_int` decryption, pair recombination, input product, output coefficient expression, and decoded logical-scale path.

The candidate does not compute a bound from `empiricalPairRelinError` and then accept that same value. It computes the bound from two separately constructed ordinary public calls and first requires their coefficientwise sum to equal the production pair error. A wrapper error therefore fails the identity before the triangle can accept anything. A Tensor2 or RS2 error remains exposed by the separate exact seam tests and the final independent coefficient path.

This still does not make the path bound universal: it is conditioned on this exact generated key, ciphertexts, basis, and execution.

## 9. Theorem 4.8 scale interaction

The paper's printed comparison on page 8 uses `1/q_l` times the input product (`PAPER-2023-1788.txt:903-935`), while the proof and modulus-consumption paragraph describe division by both `q_div` and `q_l` (`PAPER-2023-1788.txt:937-951`). The supplied exact integral witness in `project/coordination/MULT2_SCALE_ALGEBRA_CHECK.md:12-38` supports a missing `1/q_div` interpretation.

This remains an independent mathematical inference, not an author-confirmed erratum. The candidate preserves the existing logical/recorded correction `2^(2p)/(q_div*q_l)` and the corrected execution-specific coefficient expression, but it does not claim to amend the paper. The separate encoder/precision track is not changed.

## 10. Remaining gates

Before integration, all of the following remain mandatory:

1. run patch 0001 on hosted Linux GCC and Windows MinGW64 with BV REAL/COMPLEX and HYBRID regressions;
2. require the coefficientwise same-input path identity to pass;
3. inspect the exact `paper_additivity_residual` and preserve the old red as evidence of the backend proof gap;
4. apply 0002 only after the probe result is understood;
5. run all 44 tests and the explicitly named API targets under `-Werror`;
6. treat a green candidate only as tested implementation/execution evidence;
7. retain `conservative_E_Relin_available=false` and `universal_theorem_gate=UNPROVED` until a backend-specific proof or conservative bound is independently supplied.

No evidence in this packet supports a conservative BV or HYBRID `E_Relin` for the theorem's full input/key domain.
