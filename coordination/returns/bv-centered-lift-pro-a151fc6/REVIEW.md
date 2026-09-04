# Independent review — BV `digitSize=0` centered lift and fixed-key bound

## 1. Bounded verdict

**Disposition: `SOURCE_PREMISE_CLOSED__EXISTING_PROBE_UNCHANGED__HOSTED_EXECUTION_PENDING`.**

- **P0: 0.**
- **P1: 0.** The prior P1—the absent all-residue implementation/postcondition beneath `CRTDecompose(0)`—is closed by the newly supplied pinned OpenFHE source.
- **P2: 2, unchanged and nonblocking for this slice.** No numerical across-key sampler/tail statement is established, and no double-precision conclusion follows.
- **Production defect:** none demonstrated.
- **Candidate defect:** no compile-independent source, arithmetic, ordering, or certificate defect was found in the existing 363-insertion/3-deletion test-only probe.
- **Runtime status:** **NOT EXECUTED**. The patch has not been compiled or run in this review.
- **Universal status:** `universal_theorem_gate=UNPROVED` remains required. The source closure does not convert the fixed-key result into a paper-wide, all-key, security, or precision theorem.

The exact prior candidate may be transplanted unchanged as an additive diagnostic probe:

```text
patch SHA-256:
  c70a5963909bb1fc1c4f5bdabbd5baba7d013dab7f5cdd9b7ebaf325a2545871
baseline tests/mult2_e2e_oracle_test.cpp SHA-256:
  b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26
resulting full file SHA-256:
  92a2f03c0301ba0e16d6d52f72e2fef129f069bbdee2b27e6a25b7c49030538d
change:
  project/tests/mult2_e2e_oracle_test.cpp only; +363/-3
```

No replacement patch or redundant full changed file is included in this return.

## 2. Evidence identity and inspection boundary

### 2.1 Package verification

[OBSERVED] The supplied outer archive is exactly:

```text
bv-centered-lift-a151fc6.zip
bytes:   2,567,648
SHA-256: da2b38c78cdb03581996ad278ac96c1210ae0454ed325b01982c0762972cfa01
entries: 15 regular files
outer SOURCE-MANIFEST.json: 14 payload records, exact closure
```

[OBSERVED] Every nested ZIP passed CRC, relative-path, duplicate-name, encryption, and symlink checks. The following exact identities and archive-level closure mechanisms were verified:

| Archive | Bytes | SHA-256 | Verified closure |
|---|---:|---|---|
| `CURRENT-COMBINED-CONTEXT.zip` | 1,305,833 | `e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce` | root source manifest: 70 payloads |
| `CURRENT...!/ORIGINAL-PAIR-PRO-RETURN.zip` | 212,032 | `735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4` | `FILE_HASHES.sha256`: 65 files excluding itself |
| `LATEST-COMPLETE-PRO-RETURN.zip` | 58,604 | `bf15886a0cabc2347469f74ac4c356b3f0afd500fe7b79cc6fdf0d4d9c4f5ea0` | package manifest: 12 payloads; `SHA256SUMS`: 13 files |
| `PREVIOUS-FULL-BV-CONTEXT.zip` | 1,157,474 | `59a6f351e551ecbf8e1f8ee7e8577339ce5ca4d6d2b2b3c494bc514d71773e34` | root source manifest: 6 payloads |
| `PREVIOUS...!/BV-REVIEW-INPUT-4e6cce5.zip` | 1,064,259 | `83a28e43e72d0874700be3ed49f67e8ba9f85984507887fafcca4210ac2e7479` | root source manifest: 58 payloads |
| `PREVIOUS...!/ORIGINAL-PRO-RETURN.zip` | 66,462 | `50b3a85159c0a8d860c301ecea84cd7831f9d4a48d2fbe0eaff06d707ac8b5c9` | package manifest: 16 payloads; `SHA256SUMS`: 17 files |

The `input-SOURCE-MANIFEST.json` carried inside the original Pro return is an archived copy of that return's input manifest, not the return archive's closure manifest. It was not misapplied to the return directory; the return's own `PACKAGE-MANIFEST.json` and `SHA256SUMS` close correctly.

[OBSERVED] All 54 current project-file entries in `CURRENT-SOURCE-PROVENANCE.json` match the extracted packet bytes, sizes, and SHA-256 values. This verifies the supplied snapshot against its supplied provenance record; it is not a claim of independent access to the user's Git repository.

### 2.2 Selected identities

```text
documentation head: a151fc6b9d04cb4d2066f59e3ea405e4e1bbb60c
branch label:       codex/integration-01
current tested source: d73824c2d382013c3aadbd7cb29c57008e839714
OpenFHE pin:        df495ba2e91739a6dc8f1de254fc5a41155ce504
current registrations: 53
```

[OBSERVED, retained—not executed here] The supplied current logs report 53/53 on Linux in 0.68 s and 53/53 on Windows in 2.27 s. The earlier fixed-key context also retains run `33844736013` at 42/44 on both hosts and run `33846077283` at 44/44 (Linux job `100938151001`, 0.61 s; Windows job `100938151165`, 1.19 s). None is represented as execution of the present candidate.

### 2.3 Exact current fixture

```text
N = 64
batch size = 16
CKKS p / scaling-modulus bits = 30
first modulus bits = 35
multiplicative depth = 7
scaling = FIXEDMANUAL
key switching = BV
digitSize = 0
maxRelinSkDeg = 2
secret distribution = UNIFORM_TERNARY
security level = HEStd_NotSet (functional-only)
active rows/towers = 7
q_0..q_6 =
  34359736577, 1073744257, 1073738753, 1073742721,
  1073739649, 1073742209, 1073741441
q_l = 1073741441
q_div = 1073741953
Q_l = 52656049226897061758347970843194892279389197066160739197584863617
q_div*q_l = 1152921231876374273
```

[OBSERVED/RECOMPUTED] All eight native moduli are positive and odd; the seven active moduli are pairwise coprime; `gcd(q_div,q_i)=1` for every active row; the largest modulus has 35 bits. The exact centered radii are:

```text
17179868288, 536872128, 536869376, 536871360,
536869824, 536871104, 536870720
```

The secret Hamming weight `h` is generated-key-specific and is measured by the existing oracle; it is not frozen to one retained log value.

### 2.4 New official-source hashes

All four newly supplied official files match `OFFICIAL-PROVENANCE.json`:

```text
04cd20f4e186c0c313f422f2d70c408f4e213989a45ed56645f887583a12567d  src/core/include/lattice/hal/default/poly-impl.h
3ac888c8165194a627822101d2a9f10ec8d3135e7e2eae8a91c92cb808793889  src/core/include/math/hal/intnat/mubintvecnat.h
61bf8fddca55c6ea0b13fb22fc7761d44980221a7e909513509292568045d594  src/core/lib/math/hal/intnat/mubintvecnat.cpp
22d49038f1b63a88a2da0fd13941b11bd2a255c1dd3e65304600992324b9080a  src/core/include/math/hal/intnat/ubintnat.h
```

## 3. Question 1 — exact centered-lift source premise

### 3.1 Exact call chain

[OBSERVED]

1. `DCRTPolyImpl::CRTDecompose(0)` creates coefficient and evaluation representations, returns one digit per source tower, keeps digit `i`'s own tower from the original evaluation representation, and for every other target tower copies source coefficient tower `i`, calls `PolyType::SwitchModulus`, then transforms it to evaluation form (`official-openfhe/dcrtpoly-impl.h:230-250`).
2. `PolyImpl::SwitchModulus` forwards to `m_values->SwitchModulus(modulus)` and then replaces the polynomial modulus/root parameters (`src/core/include/lattice/hal/default/poly-impl.h:400-406`).
3. That call is `NativeVectorT::SwitchModulus`, declared at `mubintvecnat.h:311-326` and implemented at `mubintvecnat.cpp:98-122`.
4. The implementation uses `halfQ = oldQ >> 1`, strict `v > halfQ`, `AddEqFast` for an increasing target modulus, and `ModSubEq` for an equal/decreasing target modulus. `AddEqFast` is at `ubintnat.h:318-327`; `ModSubEq` is at `ubintnat.h:884-902`.
5. This is not the lazy path. `LazySwitchModulus` merely applies unsigned modular reduction to each value (`mubintvecnat.cpp:124-129`).

`PolyImpl::SwitchFormat` invokes the inverse or forward native CRT transform according to the current format (`poly-impl.h:420-439`). The proof below is about the coefficient integer represented by those transforms; it retains the ordinary OpenFHE representation-preservation premise rather than re-proving NTT correctness.

### 3.2 All-residue proof

[DERIVED] Let the canonical source residue satisfy

\[
0\le v<p,
\]

where the source modulus is positive and odd,

\[
p=2k+1,\qquad k=\left\lfloor p/2\right\rfloor.
\]

Define the centered source integer

\[
d_p(v)=
\begin{cases}
v,&v\le k,\\
v-p,&v>k.
\end{cases}
\]

The source computes `halfQ = p >> 1 = k` and uses the strict comparison `v > k`.

#### Case A: target `r > p`

The source sets `diff=r-p` and returns

\[
v'=v+\mathbf 1_{v>k}(r-p).
\]

- If `v<=k`, then `v'=v=d_p(v)`.
- If `v>k`, then

\[
v'=v+r-p=r+(v-p)\equiv d_p(v)\pmod r.
\]

This branch does not rely on modular reduction after addition. It cannot overflow a supported target value because

\[
v+(r-p)\le(p-1)+(r-p)=r-1.
\]

Thus the raw `AddEqFast` result is already the canonical target residue.

#### Case B: target `r <= p`

The source sets `diff=p-r` and calls `ModSubEq`. That routine first reduces both operands modulo `r` and returns the canonical modular difference.

- If `v<=k`, the subtrahend is zero, hence `v'=v mod r=d_p(v) mod r`.
- If `v>k`, then

\[
v'\equiv v-(p-r)=v-p+r\equiv d_p(v)\pmod r.
\]

The equal-modulus subcase `r=p` has `diff=0` and preserves `v`.

After the two internal operand reductions in `ModSubEq`, if `av<bv`, its mathematical result `av+r-bv` lies in `[1,r-1]`; otherwise `av-bv` lies in `[0,r-1]`. `NativeIntegerT` is explicitly a primitive unsigned-integer implementation (`ubintnat.h:95-143`). Therefore even an intermediate native addition is governed by defined unsigned arithmetic, and the final expression equals the same representable value below `r`; there is no signed-overflow undefined behavior. For the current fixture specifically, every modulus is at most 35 bits.

#### Zero, halfway, and every tower

- `v=0` never enters the negative-half branch and maps to zero for every positive target modulus.
- For odd `p=2k+1`, the strict test maps `v=k` to `+k`, `v=k+1` to `-k`, and `v=p-1` to `-1`. Hence every digit coefficient has magnitude at most `floor(p/2)`.
- The unswitched own tower contains `v`, which is congruent to the same `d_p(v)` modulo `p`.
- Every copied target tower contains `d_p(v) mod r` by the cases above.

Therefore every tower of zero-digit `CRTDecompose` represents the same centered source integer.

### 3.3 Width and supported-domain statement

[OBSERVED] `NativeInteger` is `NativeIntegerT<BasicInteger>`; the new files do not include the separate definition selecting the concrete `BasicInteger` width. They do establish that the implementation is parameterized by a primitive unsigned native type and supplies the relevant `uint32_t`, `uint64_t`, and optional `uint128_t` type traits (`ubintnat.h:73-143`). `NativeVectorT::SetModulus` rejects values exceeding the build's `MAX_MODULUS_SIZE` (`mubintvecnat.h:311-318`).

[DERIVED] The centered-lift proof is width-parametric: it needs only canonical residues and representable, supported positive moduli. It does not require an assumed 64-bit carry margin. The increasing branch is bounded by `r-1`, and the decreasing branch returns a value below `r` using defined unsigned arithmetic. The current fixture's maximum modulus bit length is 35; the probe's explicit conversions to `uint64_t` are therefore safe.

[PENDING] The hosted compiler will instantiate the actual configured alias, but this is no longer a mathematical/source blocker for A6/A7.

### 3.4 Disposition

- **A6 centered digit lift: CLOSED / SOURCE-PROVED** for canonical residues and supported positive odd native RNS moduli.
- **A7 zero lift: CLOSED / SOURCE-PROVED.** Zero stays zero under both branches; the final raised-high `q_div` source digit is therefore noncontributing.
- **Prior P1: CLOSED.** The earlier `q=3, v=2` ambiguity is resolved in favor of `-1`, not unsigned `2`, by the non-lazy source branch.

An independently written exact Python model exhaustively checked 130,000 `(p,r,v)` cases for every odd `p,r` from 3 through 101, including equal, increasing, and decreasing moduli. This is supporting evidence, not the proof; the proof is the source/algebra above.

## 4. Question 2 — reconciliation of the two fixed-key bounds

### 4.1 Key residual and quantifiers

[OBSERVED] `EvalMultKeyGen` constructs the old key `s^2` and switches it to `s` (`official-openfhe/base-leveledshe.cpp:136-144`). For BV `digitSize=0`, row `i` is generated as

\[
b_i=G_i(s^2)-a_i s-n_s e_i
\]

(`official-openfhe/keyswitch-bv.cpp:49-103`, especially 86-95). Thus, after projection to the active basis `I`, define

\[
\rho_i^{(I)}=
\operatorname{Center}_{Q_I}
\left(\pi_I(b_i)+\pi_I(a_i)\pi_I(s)-\pi_I(G_i(s^2))\right).
\]

It is the centered modular residual of `-n_s e_i`. It is not fitted to a checked ciphertext error, and it already includes the key-generation noise-scale factor. Multiplying this measured residual by `n_s` again would double-count that factor.

[DERIVED] Once the context, secret, evaluation key, row projection, ring dimension, and ordered basis are fixed, the residuals are fixed. The resulting bound is:

- **a posteriori in the generated key**;
- **a priori and uniform in the ciphertext third component** `x in R_{Q_I}`;
- basis-specific;
- deterministic for that key;
- not an unconditional statement over all generated keys.

### 4.2 Tight `l1` form and prior coarser form

Let digit row `i` have centered coefficients bounded by

\[
D_i=\left\lfloor q_i/2\right\rfloor.
\]

For any output coefficient of the negacyclic product,

\[
\left|(d_i\rho_i)_j\right|
\le D_i\sum_{k=0}^{N-1}|\rho_{i,k}|
=D_i\|\rho_i\|_1.
\]

Therefore Codex's tighter bound is

\[
\boxed{B_1(K,I)=\sum_iD_i\|\rho_i\|_1.}
\]

The existing candidate uses

\[
\boxed{B_\infty(K,I)=N\sum_iD_i\|\rho_i\|_\infty.}
\]

Since

\[
\|\rho_i\|_1\le N\|\rho_i\|_\infty,
\]

we have

\[
B_1(K,I)\le B_\infty(K,I).
\]

**Disposition:** the formulas are compatible. Codex's is tighter; the candidate's is valid and coarser. There is no justification to remove `N` from the infinity-norm formula. The candidate need not be rewritten merely to use the tighter norm. A later `l1` tightening would be justified only if hosted execution shows the coarser bound is too loose while the tighter source-derived bound is useful.

### 4.3 Centered modular versus unwrapped integer representatives

[DERIVED] `rho_i` is a finite centered representative modulo `Q_I`, not a recovered raw Gaussian polynomial. Choose centered integer representatives for each digit and each `rho_i`, and form the ordinary integer negacyclic sum

\[
z(x)=\sum_i d_i(x)\rho_i.
\]

It represents the modular key-switch error and obeys either bound above. If

\[
B(K,I)<Q_I/2,
\]

then every coefficient of `z(x)` lies strictly in the centered interval, so `z(x)` is the unique centered integer lift of the modular error. Without that inequality, the formula remains a modular bound but does not identify an unwrapped error/noise sample.

This is why an independently measured fixed evaluation-key residual is sufficient for a fixed-key ciphertext-uniform bound, but not for an unconditional all-key raw-noise theorem.

## 5. Question 3 — high/low paths, pair bound, RS2, and integer lifts

The following premises are explicit rather than hidden in `E_Relin`: the active moduli are positive, odd, and pairwise coprime; coefficient CRT uses the stated ordered basis; polynomial products are negacyclic modulo `X^N+1`; format conversion preserves the represented ring element; dropping the final tower is the canonical prefix projection; the fixed evaluation key maps the same-context `s^2` to `s`; DCP/RCB preserve their stated componentwise recombination identity; and any ordinary integer/rational interpretation is used only after the half-modulus conditions below. The RS2 `(h+1)/2` premise remains a separate downstream assumption/test obligation.

### 5.1 Effective rows and projection

[OBSERVED]

- Production multiplies each active raised-high tower by `q_div`, appends an all-zero final `q_div` tower, calls ordinary full-basis `Relinearize`, separately relinearizes low on the active prefix, decomposes the high result, and adds its exact remainder to low (`project/src/double_ckks.cpp:925-970`).
- BV key switching obtains `CRTDecompose(digitSize)`, truncates evaluation-key rows to the input basis, multiplies each row by the corresponding digit, and sums exactly `digits.size()` rows (`official-openfhe/keyswitch-bv.cpp:245-278`).
- Private DCP preserves the prefix identity `sourcePrefix = q_div*high + remainder` componentwise (`project/src/double_ckks.cpp:377-405`).

[DERIVED]

- Prefix-low has seven source towers and uses seven projected key rows.
- Raised-high allocates eight digits on `Q_l*q_div`; its eighth source tower is zero. The closed centered-lift proof makes that entire digit zero on every target tower, so row 7 contributes exactly zero.
- Projecting the full high-path key-switch error to `Q_l` leaves the same first seven projected row residuals used by the low bound.
- `gcd(q_div,q_i)=1` for all seven active towers, so multiplication by `q_div` permutes the source residue set. It does not justify a smaller worst-case digit domain. The candidate's unit checks are valid; the bound would remain conservative even if the domain were narrower.

Thus

\[
B_H=B_L=B_{Q_l}(K),
\qquad
B_{pair}=B_H+B_L=2B_{Q_l}(K).
\]

### 5.2 Gadget law

[DERIVED] At target tower `j`, only gadget row `G_j(s^2)` contains `s_j^2`; all other gadget rows are zero there. Digit `j` retains the original source residue of `c_2` at its own tower. Hence

\[
\sum_i d_i(c_2)G_i(s^2)=c_2s^2
\]

towerwise. Combining this with the row residual gives

\[
\operatorname{Err}_{K,I}(c_2)
\equiv\sum_i d_i(c_2)\rho_i^{(I)}\pmod {Q_I}.
\]

This is the BV backend equation. It does not use the paper's separate near-additivity premise.

### 5.3 No extra Relin2 `+h`

[DERIVED] The pair recombination identity carries the projected high error plus the low error. Exact DCP/recombination introduces no additional rounding term. Therefore the direct BV pair bound is `B_H+B_L`.

The paper's Relin2 `+h` arises from comparing two rounded Relin executions with a different combined Relin execution and then bounding a separate small rounding polynomial multiplied by the secret (`PAPER-2023-1788.txt:735-777`). The present proof does not make that comparison. Adding another `+h` to `B_H+B_L` would be unexplained slack: no operation in this direct path proof produces that additional term.

RS2 remains separate and retains its own Lemma 4.6 term (`PAPER-2023-1788.txt:817-891`):

\[
\frac{h+1}{2}.
\]

### 5.4 Final normalized bound

With input decomposition

\[
m_j=q_{div}\widehat m_j+\check m_j,
\]

let

\[
\|\widehat m_j\|_\infty\le M_H,
\qquad
\|\check m_j\|_\infty\le M_L.
\]

The corrected coefficient expression remains

\[
\boxed{
\left\|y-\frac{m_1m_2}{q_{div}q_l}\right\|_\infty
\le
\frac{NM_L^2}{q_{div}q_l}
+\frac{B_{pair}}{q_l}
+\frac{h+1}{2}.
}
\]

The factor `1/(q_div*q_l)` is an independently derived algebraic correction relative to the printed Theorem 4.8 target (`PAPER-2023-1788.txt:899-945`). It remains an unconfirmed discrepancy, not an author-confirmed erratum.

### 5.5 Sufficient integer-lift conditions and candidate inequality audit

A centered modular triangle inequality is not a no-wrap proof. For example, modulo 101, `40+40` centers to `-21`, and `21<=80` despite wrap.

The candidate uses explicit half-modulus conditions instead:

1. `B_path < Q_l/2` and `B_pair < Q_l/2` establish unique integer lifts of the constructed key-switch errors.
2. Let

   \[
   A=q_{div}M_H+M_L.
   \]

   The pre-RS Tensor target expands as

   \[
   q_{div}\widehat m_1\widehat m_2
   +\widehat m_1\check m_2
   +\check m_1\widehat m_2,
   \]

   so its norm is at most

   \[
   N(q_{div}M_H^2+2M_HM_L)
   \le \frac{NA^2}{q_{div}}.
   \]

   A tight sufficient condition is therefore

   \[
   \frac{NA^2}{q_{div}}+B_{pair}<Q_l/2.
   \]

   The candidate checks the stronger

   \[
   NA^2+B_{pair}<Q_l/2,
   \]

   because `q_div>=1`. It is conservative, not logically defective, and it is derived from the high/low expansion—not from modular triangle acceptance.
3. The candidate's final conservative numerator is

   \[
   C=2NM_L^2+2q_{div}B_{pair}+q_{div}q_l(h+1),
   \]

   over denominator `2*q_div*q_l`. Its actual-error assertion

   \[
   2\,E_{numerator}\le C
   \]

   is dimensionally correct.
4. To identify the final centered output with the intended rational target on modulus `Q_l/q_l`, the candidate checks exactly

   \[
   2NA^2+C<q_{div}Q_l.
   \]

   This is the cross-multiplied target-plus-error half-modulus condition.

These checks are sufficient under the existing CRT basis, format-preservation, DCP/RCB identity, and RS2 rounding premises. The fixed-key key-switch bound is uniform over all third components on the declared basis; the downstream `M_H/M_L` and final no-wrap certificate remains specific to the exercised input envelope unless a separate predeclared plaintext domain is frozen.

## 6. Question 4 — audit of the existing probe

### 6.1 Static replay

[OBSERVED]

- The current baseline oracle hash is exactly `b27c15...e1b26`.
- `git apply --check` succeeds against the extracted current combined context.
- Applying the prior patch changes only `project/tests/mult2_e2e_oracle_test.cpp` with `+363/-3`.
- The replayed output is byte-identical to the archived candidate full file and has SHA-256 `92a2f03c...0538d`.
- The actual one-file delta passes `git diff --no-index --check`.
- The current CMake file still contains 53 CTest registrations and applies warning-as-error options to the oracle target.

### 6.2 Ordering and independence

[OBSERVED] In the candidate:

- key generation and `EvalMultKeyGen` occur first;
- `CheckBvCenteredDigitLiftBoundaryProbe` and `BuildFixedKeyBvBound` run immediately afterward;
- plaintext construction, encryption, Tensor2, Relin2, path-error observation, and final-error observation all occur later (`candidate/project/tests/mult2_e2e_oracle_test.cpp:1470-1527`).

The bound is therefore key-derived and fixed before the checked ciphertext errors. It is not computed from the pair error it later accepts.

The candidate independently:

- reconstructs projected evaluation-key row residuals with `cpp_int` CRT;
- multiplies secret polynomials by schoolbook negacyclic arithmetic;
- uses `floor(q_i/2)` and the coarser valid `N*infinity` bound;
- verifies key and basis immutability;
- checks 8 allocated high digits, 7 low digits, and the all-zero final high digit;
- compares independent high, low, and pair errors against the precomputed bounds;
- retains explicit pre-RS and final no-wrap conditions;
- leaves the existing vectors, `1e-3` functional tolerance, backend matrix, public seams, rejections, and registrations unchanged.

### 6.3 Labels

[OBSERVED] The patch preserves:

```text
execution_certificate=PER_PATH_CONDITIONAL
conservative_E_Relin_available=false
universal_theorem_gate=UNPROVED
```

and adds a separately named fixed-key candidate. Its string

```text
CANDIDATE_FIXED_KEY_CIPHERTEXT_UNIFORM_CONDITIONAL_ON_CENTERED_DIGIT_LIFT
```

is conservative but not false now that the condition is source-proved. Rewording it solely for style would create unnecessary churn. It must not be changed to `PROVED` merely because the source premise is closed or a finite hosted run passes.

### 6.4 Defect disposition

**No actual static defect requiring an edit was found.** The candidate can be transplanted unchanged as an additive diagnostic probe.

[PENDING] Warning-as-error compilation can still expose API, include, type, or platform issues that static reading cannot settle. Runtime can still show that a computed bound is trivial, a no-wrap condition fails, or an independent path error exceeds the source-derived bound. Those are reasons to reject or amend after execution, not reasons to rewrite the unexecuted candidate now.

## 7. Question 5 — Gaussian and precision boundaries

### 7.1 Ideal distribution versus implementation sampler

[DERIVED] An ideal mathematical discrete Gaussian on the integers has unbounded support. That statement alone does not describe the exact finite implementation's sampler, rejection loops, tables, cutoffs, or effective support.

[PENDING] The necessary sampler implementation and parameter semantics were not supplied for this question. Therefore this review makes no implementation-level unbounded-support claim, no numerical tail claim, and no failure-probability claim.

This does not block the fixed-key result: after a key is generated, the finite modular row residuals are directly reconstructed from that fixed key. A cross-key probabilistic theorem would require a separately specified sampler model, tail function, and target probability.

### 7.2 Double-precision boundary

The fixed-key conservative gate, a passing `1e-3` decoded check, and a binary64 result do not establish more than 53 reliable bits. Larger-parameter work still has to cover, at minimum, useful bound tightness, paper-scale/security-appropriate parameters, high-precision input/output, canonical-embedding error, rescale/rounding growth, and the repeated lifecycle/refresh objective. Those remain open and are not executed or redesigned here.

## 8. Explicit disposition ledger

| Item | Disposition | Basis |
|---|---|---|
| Prior P1: missing all-residue `SwitchModulus` semantics | **CLOSED** | exact pinned source plus algebraic proof |
| A6 centered lift | **CLOSED** | non-lazy strict-half source branches |
| A7 zero lift/final inactive digit | **CLOSED** | zero preserved in both branches; own tower zero |
| Codex `sum D_i||rho_i||_1` | **ACCEPTED** | tighter coefficient convolution bound |
| Prior `N sum D_i||rho_i||inf` | **ACCEPTED** | follows from `l1 <= N*inf`; coarser |
| Extra `n_s` on measured `rho_i` | **REJECTED** | `rho_i` already contains `-n_s e_i` modulo the basis |
| Fixed-key ciphertext-uniform quantifier | **ACCEPTED** | key fixed before ciphertext; proof quantifies over all `c2` in declared ring |
| Unconditional/all-key Gaussian theorem | **NOT ESTABLISHED** | sampler/tail model absent; fixed-key result has different quantifier |
| High 8 / low 7 / final zero row | **CONFIRMED BY SOURCE; RUNTIME PENDING** | production and key-switch call paths plus centered zero lift |
| Pair bound `B_H+B_L` | **ACCEPTED** | exact recombination of two actual path errors |
| Additional Relin2 `+h` | **REJECTED AS REQUIRED TERM** | belongs to unused near-additivity comparison, not direct path bound |
| RS2 `(h+1)/2` | **RETAINED** | separate downstream rounding term |
| `1/(q_div*q_l)` normalization | **RETAINED AS INFERENCE** | algebraically required; not author-confirmed erratum |
| Existing probe patch | **UNCHANGED / READY FOR HOSTED PROBE** | exact static replay and logic audit |
| Universal theorem / precision completion | **OPEN** | expressly beyond this evidence |

## 9. Permitted claims

### 9.1 Permitted now, after this source review

It is defensible to state:

> At pinned OpenFHE commit `df495...504`, BV `digitSize=0` `CRTDecompose` propagates each canonical source coefficient as its centered integer into every target tower through the non-lazy `SwitchModulus` path. Consequently, the derived fixed-key bounds `sum D_i||rho_i||_1` and `N sum D_i||rho_i||inf` are valid, basis-specific, ciphertext-uniform bounds for a fixed evaluation key, subject to the stated CRT/ring/format premises. The existing test-only probe is statically applicable unchanged to the current baseline.

It is not yet defensible to claim that the candidate compiles, that its bound is nontrivial for a generated key, that its runtime assertions pass, or that the current 53-case suite passes with the patch.

### 9.2 Permitted only after successful hosted validation

For each actually generated fixed key whose candidate assertions pass, it would be defensible to state:

> The key-derived bound was computed before plaintext/encryption and is uniform over every BV third component on the declared active basis for that fixed key. The exercised public Mult2 cases satisfied the independent path, pair, integer-lift, and final coefficient checks, while the complete current regression suite remained green.

Even then, do not claim an all-key probability-one `E_Relin`, a numerical Gaussian tail, secure paper-scale parameters, the printed theorem literally as written, more than 53-bit precision, refresh/repeated multiplication, Table 3, security, or performance completion.

## 10. Smallest next hosted validation plan

Use the exact current source and the unchanged prior patch.

1. **Identity and application**
   - require oracle baseline SHA-256 `b27c15...e1b26`;
   - require OpenFHE pin `df495...504`;
   - run `git apply --check`, apply the patch, and require resulting oracle SHA-256 `92a2f03c...0538d`;
   - require only the oracle test file to differ.
2. **Warning-as-error build** using the supplied workflow and pristine OpenFHE 1.5.0:

   ```bash
   cmake -S project -B build \
     -DCMAKE_BUILD_TYPE=Debug \
     -DCMAKE_PREFIX_PATH="$OPENFHE_PREFIX"
   cmake --build build --parallel 2
   cmake --build build --target \
     relin2_api_contract_test rs2_api_contract_test mult2_api_contract_test \
     add_api_contract_test sub_api_contract_test --parallel 2
   ```

3. **Focused discriminators first**:

   ```bash
   ctest --test-dir build \
     -R '^mult2_e2e_bv_(real|complex)$' \
     --verbose --output-on-failure
   ```

   Require the centered boundary probe, 8/7 digit domains, zero final digit, seven active residual rows, precomputed path/pair bounds, both no-wrap inequalities, and final coefficient bound to pass. Require the existing conditional/unproved labels to remain unchanged.
4. **Full current suite**:

   ```bash
   ctest --test-dir build --verbose --output-on-failure
   ```

   Require all 53 registrations, both hosts, input/key/cache immutability, HYBRID regressions, public API targets, lifecycle rejections, frozen vectors, and frozen tolerance to remain unchanged.
5. **Optional fresh-key falsifier, not acceptance theorem:** invoke each BV test binary in separate fresh processes using a predeclared repetition count. Treat any failure as a defect; treat all passes only as additional implementation evidence.

Fail immediately on a warning, build failure, basis/row mismatch, nonzero eighth digit, bound not below half modulus, path/pair overrun, failed integer-lift inequality, label escalation, changed threshold, or any existing test regression. Do not increase the bound from observed ciphertext errors to cure a failure.

## 11. Execution ledger

### Actually executed in this review

- exact outer archive size and SHA-256 verification;
- recursive ZIP CRC, safe-path, duplicate-name, encryption, and symlink checks;
- all archive-level source/package/hash-list closure checks described in Section 2;
- comparison of all 54 current project files with the supplied current provenance record;
- four new official-source size/hash checks;
- full source inspection of `CRTDecompose`, `SwitchModulus`, `LazySwitchModulus`, native addition/subtraction, BV key generation/key switching, relinearization, and current Relin2/DCP paths;
- independently written exact scalar witness over 130,000 odd-modulus cases;
- exact current modulus/radius/GCD/product calculations;
- independent small-ring checks reconciling the `l1` and `N*infinity` bounds;
- the modulo-101 wrap counterexample;
- patch application check, application, one-file comparison, full-file equality, insert/delete count, and whitespace check;
- current CMake registration count and inspection of retained current Linux/Windows logs;
- final deliverable extraction, member, size, hash, manifest, and sidecar checks.

### NOT EXECUTED

- OpenFHE configuration or compilation;
- C++ warning-as-error build;
- key generation, encryption, CKKS, DCP, Tensor2, Relin2, RS2, Mult2, or decryption runtime;
- CTest;
- GitHub Actions or any CI dispatch/rerun/cancel;
- Windows or user-Mac execution;
- any supplied binary or supplied witness script;
- numerical sampler-tail analysis;
- high-precision, repeated-use, security, or performance experiments.

## 12. Final bounded conclusion

The exact source gap named in the prior review is resolved. The pinned non-lazy `SwitchModulus` path implements the centered source lift for all canonical residues in the supported positive odd native-modulus domain; zero is preserved. This validates both fixed-key bound forms and the seven-effective-row high/low argument.

The existing probe has no demonstrated static defect and should be hosted unchanged. The result remains a fixed-key, basis-specific conservative certificate plus input-specific downstream no-wrap checks—not a universal `E_Relin`, not theorem/precision acceptance, and not evidence that the full Double-CKKS goal is complete.
