# REVIEW — BV fixed-key conservative error bound

## 1. Verdict

**AMEND.**

- **Production defect found:** no.
- **ZCode Q4 core idea:** accepted in principle after material correction.
- **ZCode Q4 formula as written:** not accepted as the exact certificate.
- **Smallest defensible output:** one test-only PROBE patch, with no production or public-contract change.
- **Universal theorem status:** remains `UNPROVED`.
- **Precision status:** unchanged; no double-precision acceptance.

The packet supports a deterministic, key-conditioned bound that is uniform over all `c2` polynomials on a specified active basis, provided the BV `digitSize=0` cross-modulus digit is the centered source-tower lift. The pinned files show the call to `SwitchModulus`, but do not provide that method's implementation or a normative postcondition. That is the exact remaining blocking premise. The included runtime boundary probe is useful falsification evidence, but it cannot replace the missing all-residue contract.

No change to `DoubleCKKS::Relin2`, `Tensor2`, `RS2`, `Mult2`, any public header, CMake registration, input vectors, or the frozen decoded tolerance is justified.

Finding count for this bounded slice:

- **P0: 0.** No demonstrated production arithmetic or safety defect.
- **P1: 1.** The all-residue centered-lift contract of the pinned `NativePoly::SwitchModulus` is absent from the supplied source set; this blocks promotion of the nontrivial fixed-key bound.
- **P2: 2.** No numerical across-key DGG tail statement is available, and no conclusion about genuine double precision follows from this fixture.

## 2. Inputs and identities actually inspected

The following identities were recomputed before source inspection:

- outer ZIP: `bv-fixed-key-bound-c7cd790.zip`
  - bytes: `1,157,474`
  - SHA-256: `59a6f351e551ecbf8e1f8ee7e8577339ce5ca4d6d2b2b3c494bc514d71773e34`
  - 7 regular entries; safe relative paths; no duplicate names or symlinks;
  - outer manifest: 6 payload entries, all sizes and hashes matched, with exact closure.
- nested `BV-REVIEW-INPUT-4e6cce5.zip`
  - bytes: `1,064,259`
  - SHA-256: `83a28e43e72d0874700be3ed49f67e8ba9f85984507887fafcca4210ac2e7479`
  - 77 entries: 59 files and 18 directories; safe paths, no duplicates or symlinks;
  - nested manifest: 58 payload entries, all sizes and hashes matched, with exact closure.
- `ORIGINAL-PRO-RETURN.zip`
  - bytes: `66,462`
  - SHA-256: `50b3a85159c0a8d860c301ecea84cd7831f9d4a48d2fbe0eaff06d707ac8b5c9`
  - complete 18-file return; its 17-entry checksum list matched.
- `ZCODE-REVIEW.md`
  - bytes: `25,928`
  - SHA-256: `408106b4e3075e298f2a53cdf6396ec3dce5e9b0a7c52ee8fa33657fbc22ca45`;
  - `ZCODE-MANIFEST.sha256` was treated as a record of the reviewer's original directory layout, not as the closure definition for the flattened outer packet.
- `CODEX-RECONCILIATION.md`
  - bytes: `11,238`
  - SHA-256: `0cffb0f99071e83dca639b100367cb9c9124ee1d9e37123c20a0b992ba038c79`.

Source identity used throughout:

- documentation-only outer head: `c7cd7903042421894db71ce3aec4b00f99888a26`;
- exact project archive snapshot: `4e6cce53b23a6022bf6f942ab973aaa6bf9e5bf6`;
- tested source content: `9bf86cb53a1bbae3a3627fe5efc385d2a29c89ce`;
- branch recorded by the packet: `codex/mult2-01`, clean at selection;
- pristine OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The full verification record is in `evidence/input-verification.txt`.

## 3. Retained execution evidence, not re-execution

The retained logs establish the current boundary, not the candidate's runtime result.

### 3.1 Integrated conditional green

Run `33846077283`, tested source `9bf86cb`:

- Linux job `100938151001`: `44/44`, `0.61 s`.
- Windows job `100938151165`: `44/44`, `1.19 s`.

All four e2e cases print:

- `execution_certificate=PER_PATH_CONDITIONAL`;
- `conservative_E_Relin_available=false`;
- `universal_theorem_gate=UNPROVED`.

The two BV executions still show that the paper-style near-additivity observation is false for those keys:

| Host/case | `h` | high path | low path | actual pair | triangle bound | paper additivity residual |
|---|---:|---:|---:|---:|---:|---:|
| Linux BV REAL | 48 | 198,484,134,122 | 237,154,150,901 | 254,855,425,176 | 435,638,285,023 | 315,680,066,831 |
| Linux BV COMPLEX | 46 | 153,692,346,235 | 204,571,468,839 | 267,589,326,739 | 358,263,815,074 | 312,458,843,020 |
| Windows BV REAL | 46 | 207,845,733,286 | 155,908,259,136 | 240,813,027,530 | 363,753,992,422 | 335,007,425,026 |
| Windows BV COMPLEX | 48 | 185,699,617,586 | 177,532,695,905 | 265,109,364,188 | 363,232,313,491 | 409,095,608,201 |

These records appear in `project/artifacts/tdd/mult2-bv-execution-certificate/linux.txt:410-446` and `project/artifacts/tdd/mult2-bv-execution-certificate/windows.txt:441-473`. They are evidence that the per-path execution certificate works and that the old one-combined-call inequality is not restored.

### 3.2 Historical red retained

- Probe-red run `33844736013`: `42/44` on both hosts.
- Original matrix and diagnostic runs: the same two BV tests failed at `pair relinearization error exceeded empirical E_Relin + h`.
- The Windows original matrix record reports `42/44` in `1.18 s`.

The candidate does not rewrite or reinterpret those failures as passes.

## 4. Source-derived BV equation

### 4.1 Evaluation-multiplication key direction

OpenFHE's ordinary multiplication-key generator forms `s^2` as the old key and uses `s` as the new key, then calls key-switch generation (`official-openfhe/base-leveledshe.cpp:136-144`). Thus the BV rows in this task switch the third ciphertext component from the extended key component `s^2` to the ordinary key component `s`.

For `digitSize=0`, OpenFHE creates one row per old-key tower. In row `i`, it samples `a_i`, places the old-key tower only at tower `i` of `b_i`, and subtracts `a_i*s + ns*e_i` (`official-openfhe/keyswitch-bv.cpp:49-103`). Therefore, in the full key basis,

\[
 b_i+a_i s-G_i(s^2)\equiv -n_s e_i.
\]

The sign is negative. The factor `n_s` is already present in the row residual. Norm bounds are sign-insensitive, but the sign and units matter when deciding whether `n_s` should be applied again.

The generic pinned constructor has a default `noiseScale=1` (`official-openfhe/rns-cryptoparameters.h:120-136`), but the retained e2e lines do not print the actual runtime value. The candidate reads and reports `GetNoiseScale()` instead of assuming it.

### 4.2 Ordinary relinearization path

The public `CryptoContext::Relinearize` retrieves the multiplication-key vector and delegates to the scheme (`official-openfhe/cryptocontext.h:2021-2031`). `RelinearizeInPlace` invokes `KeySwitchCore` on every component from index 2 onward, adds the two returned components to `c0,c1`, then resizes to two components (`official-openfhe/base-leveledshe.cpp:327-340`).

BV `KeySwitchCore` calls `CRTDecompose(digitSize)`, truncates every key row to the input basis, multiplies row `i` by digit `i`, and sums the rows (`official-openfhe/keyswitch-bv.cpp:245-278`). For `digitSize=0`, `CRTDecompose` returns one digit per input tower; digit `i` retains source tower `i` and calls `SwitchModulus` to emit that source tower on every other tower (`official-openfhe/dcrtpoly-impl.h:230-250`).

This gives, for fixed key `K`, input third component `x`, and a declared basis `I`,

\[
\operatorname{Err}_{K,I}(x)
\equiv
\sum_{i\in I} d_i(x)\rho_i^{(I)}
\pmod {Q_I},
\]

where `rho_i` is the centered row residual after basis restriction. This identity is independent of the paper's near-additivity premise.

### 4.3 Raised-high versus low basis

Production `Relin2`:

1. multiplies every active high tower by `q_div`;
2. appends an all-zero `q_div` tower;
3. calls ordinary relinearization on the full eight-tower basis;
4. separately relinearizes low on the seven-tower active prefix;
5. decomposes the high result and recombines the remainder with low (`project/src/double_ckks.cpp:807-852`).

The public composition remains exactly `RS2(Relin2(Tensor2(left,right)))` (`project/src/double_ckks.cpp:997-999`).

For the high call, `CRTDecompose(0)` allocates eight digits, but the source polynomial in the appended `q_div` tower is zero. Under the required modulus-switch mapping, digit 7 is identically zero. Only rows 0 through 6 affect the projection to `Q_l`. The low call directly has seven digits and uses the same seven rows after `EvalFastKeySwitchCore` drops the final key tower.

The exact active fixture moduli are:

```text
q_0 = 34359736577
q_1 = 1073744257
q_2 = 1073738753
q_3 = 1073742721
q_4 = 1073739649
q_5 = 1073742209
q_6 = 1073741441 = q_l
q_div = 1073741953
```

Every `gcd(q_div,q_i)=1`. Therefore multiplication by `q_div` is a permutation of every active residue field; it does not reduce the worst-case high-path digit domain.

## 5. Disposition of ZCode Q4

ZCode proposed, schematically,

\[
 n_s N\sum_i \|e_i\|_\infty\left\lceil q_i/2\right\rceil.
\]

The disposition is **ACCEPT THE CONSTRUCTION IDEA; AMEND THE FORMULA, DOMAIN, AND CLAIM.**

### 5.1 What is valid

For one fixed evaluation key and one declared basis, independently recovered row residuals can support a bound valid for every ciphertext third component on that basis. The bound is calculated from the key before the ciphertext being checked and is therefore not circular in the accepted path error.

The exact useful form is

\[
B_I(K)=N\sum_{i\in I}
D_i R_i,
\qquad
D_i=\left\lfloor q_i/2\right\rfloor,
\qquad
R_i=\|\rho_i^{(I)}\|_\infty.
\]

For the current pair path, `I={0,...,6}`, so there are seven effective rows, not eight. The high call's eighth digit is zero; the low call has seven rows. The row residuals must be reconstructed after restricting the key rows to `Q_l`; a full-basis centered norm cannot simply be reused as a prefix-basis integer norm.

### 5.2 Required corrections

1. **Measured residual versus raw Gaussian error.** If `e_i` denotes the raw DGG polynomial, the factor `n_s` belongs outside its norm. If the test measures `rho_i=b_i+a_i s-G_i(s^2)`, that residual already equals `-n_s e_i` modulo the basis; multiplying by `n_s` again double-counts the scale.
2. **Floor versus ceiling.** The exact maximum magnitude of an odd-modulus centered residue is `floor(q_i/2)=(q_i-1)/2`. `ceil(q_i/2)` is conservative but one unit looser for each weighted row.
3. **Active rows.** Seven rows contribute to the projected high and low errors. The allocated full-basis high row for `q_div` contributes zero.
4. **Basis.** `rho_i` is basis-specific. Here it is reconstructed in `R_{Q_l}`, after key-row projection.
5. **No `+h` in the direct pair term.** The direct path bound is `B_high+B_low`. The paper's `+h` comes from its separate near-additivity comparison involving three rounded Relin expressions; it is not a BV key-switch error term.
6. **No universal key claim.** The result is deterministic after conditioning on one generated key. It is not a probability-one finite bound on raw discrete-Gaussian samples over all keys.

### 5.3 Exact blocking premise

The residual polynomials alone are insufficient to obtain the nontrivial `D_i=floor(q_i/2)` factor. The lift performed by `SwitchModulus` must also be known.

Minimal gap witness: with source modulus `q_i=3` and source residue `2`, the centered lift is `-1`, satisfying `D_i=1`; the canonical nonnegative lift is `2`, violating that bound. Both represent the same source residue. Thus observing the key residual does not determine the digit magnitude.

The supplied `dcrtpoly-impl.h` identifies the `SwitchModulus` call but does not contain that method's body or an authoritative all-residue postcondition. Until that exact pinned contract is inspected, the useful fixed-key theorem remains conditional. A finite boundary test can falsify an implementation mismatch but cannot prove the mapping for approximately one billion residues per tower.

Without the centered-lift premise, a generic CRT digit can have centered `Q_l` norm as large as `floor(Q_l/2)`. That yields only a trivial modular bound and cannot establish the intended integer lift.

## 6. Pair and final-product propagation

Let `B_H` and `B_L` be the high and low fixed-key bounds after projection to `Q_l`. For the current domains, both equal `B_{Q_l}(K)`, so

\[
B_{\mathrm{pair}}=B_H+B_L=2B_{Q_l}(K).
\]

This directly bounds the relinearization contribution to the recombined pair. No separate `+h` is needed there. DCP/RCB bookkeeping adds no recombined ring error: production computes the high quotient and remainder, then adds the remainder to relinearized low, preserving the projected ring identity (`project/src/double_ckks.cpp:836-852`).

With

- `M_high` bounding each input high decryption,
- `M_low` bounding each input low decryption,
- `q_l` the RS2-dropped active prime,
- `h` the actual ternary-secret Hamming weight,

an execution whose required integer lifts are unique has the final coefficient bound

\[
\left\|
\operatorname{Dec}(\operatorname{Mult2})-
\frac{m_1m_2}{q_{\mathrm{div}}q_\ell}
\right\|_\infty
\le
\frac{N M_{\mathrm{low}}^2}{q_{\mathrm{div}}q_\ell}
+
\frac{B_{\mathrm{pair}}}{q_\ell}
+
\frac{h+1}{2}.
\]

The final `(h+1)/2` is the independent RS2 rounding term from Lemma 4.6 (`PAPER-2023-1788.txt:817-891`). It is not a second Relin2 correction.

The corrected product normalization is `1/(q_div*q_l)`. The printed Theorem 4.8 expression and its proof chain are inconsistent on that factor (`PAPER-2023-1788.txt:903-945`). This review retains the prior classification: **unconfirmed algebraic inference, not an author-confirmed erratum**.

A centered modular triangle inequality is not a no-wrap witness. For `Q=101`, `40+40` centers to `-21`, and `21 <= 40+40` still passes. The intended integer argument therefore separately needs:

1. a nontrivial key-switch bound, preferably `B_pair<Q_l/2`;
2. a pre-RS target-plus-error condition;
3. a final target-plus-error condition below `Q_{l-1}/2`.

The candidate uses the stronger paper-style pre-RS condition

\[
N(M_{\mathrm{high}}q_{\mathrm{div}}+M_{\mathrm{low}})^2+B_{\mathrm{pair}}<Q_\ell/2,
\]

and the exact rational final-lift condition stated in `BOUND-DERIVATION.md`.

## 7. Codex reconciliation — explicit disposition

All material Codex corrections are sustained.

- The ZCode formula remains a research lead until domains, lift semantics, scale, basis, wrap, and key quantifiers are explicit. This review supplies those items but identifies the missing pinned `SwitchModulus` contract as the remaining block.
- The proposed `+h` after the two path bounds is rejected as unnecessary in the direct BV derivation.
- Claims that the exercised identity catches every possible wrapper defect are narrowed: it discriminates the exercised values and enumerated metadata, not every input or hidden state.
- ZCode's statement that coefficient errors were approximately `1e-10` of the execution bound is rejected. Codex's exact retained-log ratios, approximately `0.041` through `0.118`, are the correct figures; slot error and coefficient-bound ratio are different quantities (`CODEX-RECONCILIATION.md:159-170`).
- The centered triangle check is retained as valid modular arithmetic but not accepted as evidence of no-wrap (`CODEX-RECONCILIATION.md:171-175`).
- “42 non-BV tests” is corrected to “the remaining 42 tests”; other registered unit tests also exercise BV (`CODEX-RECONCILIATION.md:152-154`).
- The missing-`1/q_div` reading remains an algebraic inference only (`CODEX-RECONCILIATION.md:131-132`).

## 8. Candidate disposition

The included patch is deliberately named:

`0001-PROBE-fixed-key-bv-bound.patch`

It changes only `project/tests/mult2_e2e_oracle_test.cpp`:

- baseline bytes/SHA-256: `56,370` / `b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26`;
- candidate bytes/SHA-256: `74,014` / `92a2f03c0301ba0e16d6d52f72e2fef129f069bbdee2b27e6a25b7c49030538d`;
- diff: 363 insertions, 3 deletions;
- production files changed: zero;
- CMake/test registrations changed: zero.

The candidate computes its key-only bound before plaintext creation and encryption, probes digit boundaries, reconstructs key residuals independently, checks the high/low digit domains, applies the bound to the existing independent per-path errors, and adds explicit no-wrap checks. It leaves the historical conditional certificate and frozen final execution-bound assertion in place.

It must not be described as green until the missing all-residue `SwitchModulus` contract is source-verified and hosted warning-as-error/CTest runs pass.

## 9. Effect on the double-precision objective

This proof slice does not establish the project's target precision.

A passing fixed-key BV coefficient bound would show only that one generated key and declared basis have a conservative key-switch envelope, and that the exercised coefficients remain within stated integer-lift conditions. It would not show:

- more than 53 reliable bits;
- a correct genuine high-precision encoder/decoder;
- paper-scale or secure parameters;
- a bound in the canonical embedding sufficient for repeated multiplication;
- successful refresh/second multiplication;
- a probability statement over key generation;
- Table 3 performance, security, or accuracy.

The current fixture is explicitly functional-only: `N=64`, `p=30`, first modulus 35 bits, depth 7, `FIXEDMANUAL`, `digitSize=0`, `UNIFORM_TERNARY`, and `HEStd_NotSet`. The host vectors and decoded comparisons use binary64, and the frozen `1e-3` threshold is not a bit-precision metric.

At larger parameter sets, the still-required checks include basis-specific row-bound scaling, an explicit DGG tail event or fixed-key policy, all no-wrap conditions, canonical-embedding growth, exact logical/recorded scale handling, a high-precision I/O oracle, and the independent refresh/repeated-multiplication path. Those tasks are not implemented here.
