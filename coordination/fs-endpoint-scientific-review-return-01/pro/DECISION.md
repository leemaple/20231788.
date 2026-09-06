# FS-ENDPOINT-SCIENTIFIC-REVIEW-01 — decision

**Disposition: FULL-SLOT ATTRIBUTION CLOSED, CONDITIONALLY. ORIGINAL E80 FAIL. NO EVIDENCED PRODUCTION DEFECT. COMPLETE IMPLEMENTATION NOT ACCEPTED.**

**One next decision:** integrate the explicit, versioned **FS-PRECISION-CLAIM-SPLIT-01** contract described in `NEXT_STEP.md`: distinguish constructive-method correspondence, realized-chain residual characterization, and original-input precision. Preserve the current public-encryption/fixed-scale profile as a failed E80 target. This is a bounded documentation/report-contract correction, not a multiplication patch, an A80 gate, or another encrypted experiment. The proposed contract is supplied; repository integration and adoption have not occurred.

## 1. Identity and scope

Reviewed attachment: `fs-endpoint-scientific-review-ed5fd192.zip`, 9,896,298 bytes, SHA-256 `ee98c075f62e23cf99f0a06ce694b49290f1f5dbe275932988954d41aa919d23`. All 218 regular members, CRCs, the self-excluding manifest, and all 217 payload hashes passed. Manifest SHA-256: `bb8f9059770eb80b23056e899089f93d703e8605f888408fb27d687f6cf1ff47`.

Tested source: `ed5fd192a89d6d4728ad295e87cf06a3f4abc832`; production identity: `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`; accepted evidence checkpoint: `1c7609593a9d51d6f27918b0bebe54d8f5eaeee6`; official dependency pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`. File hashes and supplied Git-origin records were checked, not recreated from Git. The engineering root branch and the actual diagnostic-run branch are distinct roles; see finding F8.

Paths below are original attachment paths. Their exact retained copies are under `evidence/`. Source statements were checked against source and the paper before adopting the root's conclusions. No earlier conversation or private repository was used.

## 2. What the evidence establishes

The production path corresponds to the constructive DCP/Tensor2/Relin2/RS2/RCB composition, including compensation of high-part rounding in the low part and exact receipt-based scaling. For these two realized fresh ciphertexts, the completed chain's full-slot endpoint is close to the ideal power of the **realized decrypted fresh value**. The source review, retained regression evidence, and independent endpoint arithmetic support this bounded method-correspondence conclusion.

They do **not** establish satisfaction of the original end-to-end contract. They also do not constitute a formal verification of every intermediate operation, absence of cancelling defects, all-key correctness, or cryptographic security. The sufficient nonwrap antecedent remains unavailable for the final multiplication; its failure does not prove an actual wrap. No specific production statement has been falsified by this review, so no production patch is justified.

The original requirement still unmet is precision relative to the intended input, including the frozen per-stage anchor and final full-slot/component predicates and Linux's final witness-accuracy predicate. Existing failing runs are evidence of that unmet requirement, not expendable diagnostics. See `project/tests/paper_full_eight_square_oracle.h:136–140,181–190`, `project/tests/paper_full_eight_square_contract_test.cpp:241–248`, and the retained failure records in `results/independent_*.json`.

## 3. Independent full-slot result

Let `z` be the exact intended slot input, `x0` the realized fresh endpoint, and `x8` the terminal endpoint. The componentwise definitions are

\[
E_0=x_0-z,\quad E_8=x_8-z^{256},\quad
I_8=x_0^{256}-z^{256},\quad A_8=x_8-x_0^{256}.
\]

Thus `E8 = I8 + A8` at the **same slot and component**. All norms here are maximum absolute real/imaginary component, not complex modulus. The fixed threshold is

\[
T=2^{-80}=8.2718061255302767487140869206996\times10^{-25}.
\]

| Full-slot maximum | Linux | Windows |
|---|---:|---:|
| E0 | 3.204017532932254842e-25 | 3.369070509870264832e-25 |
| E8 | 9.146473633649420509e-24 | 9.065305174086772224e-24 |
| I8 | 9.148366607222537304e-24 | 9.065627201616558162e-24 |
| A8 | 4.567109173312563309e-26 | 8.303554602237414717e-26 |
| E8 / T | 11.0574081341 | 10.9592814876 |
| A8 / T | 0.0552129620062 | 0.100383815532 |

These are rounded presentations. Exact grid bounds, retained rational allowances, hashes, source identities and maximizers are in `results/independent_linux.json` and `results/independent_windows.json`. The E0 maximizers are Linux 14324/real and Windows 8523/imag; A8 maximizers are Linux 11143/imag and Windows 15050/real. No subtraction of different-slot maxima was used.

At the **E8 maximizer**:

| Host / slot / component | Signed I8 | Signed A8 | Signed E8 |
|---|---:|---:|---:|
| Linux / 11656 / imag | -9.148366607222537304e-24 | +1.892973573116794951e-27 | -9.146473633649420509e-24 |
| Windows / 5091 / imag | -9.065627201616558162e-24 | +3.220275297859383657e-28 | -9.065305174086772224e-24 |

The added residual partly **cancels** the inherited error. Independently computed lower bounds for `|I8|`, and for `|I8| - |A8|`, exceed T after retaining the conditional allowances and serialization uncertainty. Both global A8 upper bounds remain below T. This corroborates **INHERITED-DOMINANT / ADDED-ENDPOINT-SMALL**, only for these two chains under the stated model.

Source locators: `project/coordination/fs-endpoint-live-run-01/LINUX_JOB.log:6706–6709` and `WINDOWS_JOB.log:7038–7041`. Decoded canonical TSV row `slot s` is line `s+70`: in particular Linux slot11656 is line11726, Windows slot5091 is line5161. The compressed originals remain unchanged.

### Arithmetic independence and limits

The supplied bounded receipt verifier passed on both hosts. Separately, the supplied Decimal256 **full numerical** replay and reconciliation were executed for every one of 16,384 rows on each host. The review-authored driver is not misrepresented as independent arithmetic.

`independent_endpoint_check.py` supplies genuinely separate arithmetic: no project imports, integer outward rounding on a 10^-200 grid, and the recurrence

\[
w_{r+1}=w_r^2,\qquad
\delta_{r+1}=\delta_r(2w_r+\delta_r),\qquad
w_0=z,\ \delta_0=E_0.
\]

After eight steps, `delta8 = I8`; `A8 = E8 - delta8`. This differs from subtracting two independently computed large powers in the supplied replay. Endpoint input intervals include the canonical half-decimal-ULP uncertainty and the independently rederived live E0/E8 allowances. All rows, 24 retained controls, nine exact scales, global extrema, and all four primary-selected signed tuples were checked. Unit/negative tests include exact rational arithmetic comparisons, disposition/source rejection and the paper sign example.

The integer scalar enclosures are rigorous for their supplied endpoint intervals. **Those endpoint intervals remain conditional** on the live observer model. This work does not certify Boost pi/sin/cos, the official inverse NTT, coefficient capture, or runner file-publication race behavior. An identity formed from the same reconstructed quantities is not additional independent cryptographic evidence. See F6.

### Linux's additional failure survives

The signed witness error is `E8[1].real - E8[0].real`, not the size of the witness itself. Linux gives approximately `-2.393896845757505603e-24`, exceeding `2T = 1.654361225106055350e-24` in magnitude. Its actual difference is nevertheless approximately `6.887102236611778988e-22`, above `2^-76`. Windows's error is approximately `+2.680656760381511884e-25`, below `2T`.

The independent comparison also retained `2 * 2^-120` for transfer between the two observed endpoints and the producer's decoded witness. The decisions do not change. Linux has **9** numerical misses and Windows **7**, not seven each. Locators: Linux log `6588–6605,6671`; Windows log `6921–6937,7003`; the original executable witness predicate is at `paper_full_eight_square_contract_test.cpp:241–248`.

## 4. Attainability and the supported claim

**For the two captured fresh values:** an ideal exact computation of `x0^256` already misses the original-input E80 target. Removing all added evaluation residual would not make them pass; at the worst components it would remove a small beneficial cancellation. No speculative multiplication correction follows from the observed final E80 failure.

**For other draws from the unchanged public-encryption profile:** this packet does not establish either guaranteed success or universal impossibility. A passing draw is not ruled out; a failure probability and the paper's average are not measured by two chains. The correct present disposition is **the fixed public100 profile is not qualified for the frozen original-input E80 claim**, rather than “80 bits can never be attained” or “the algorithm is complete.”

E0 is aggregate fresh error, not a measured decomposition into encoding, key-generation and encryption noise. The actual official public path contains `e_pk*v + e0 + s*e1`; the h128 secret does not make its ephemeral v sparse. This is a source-level explanation of why a client precision budget matters, not independent attribution of the observed E0 to a particular noise draw. See F4.

The paper's Section6.3/Table3 reports an empirical average, not the frozen stress contract. Its constructive method can be implemented correctly while a different public-encryption/input/profile contract fails its precision goal. Explicitly separating those claims is scientifically legitimate; turning the failed original gate into a pass is not.

A future original-E80 profile needs a positive inherited-plus-added precision budget, coherent physical scales/moduli, and its own bounded validation. This review selects **no replacement numerical profile**. Merely raising initial scale by `2^k` with fixed d and dropped primes gives `S8_new/S8_old = 2^(256k)`; four initial guard bits therefore multiply terminal scale by `2^1024`, not 16. A metadata-only or “add four bits” patch is unjustified. Exact source: `project/src/repeated_mult2.cpp:254–285`.

## 5. 中文说明

已经做出来的是：按照论文把一个高精度密文拆成“高位＋低位”，在密文上连续平方八次，再重新组合和解密；过程中没有刷新，也没有重新加密。

这次把两台主机各自全部 16,384 个槽位都重新算过，并用另一套整数区间算法交叉检查。结果显示：相对于“刚加密后实际得到的数”，后续乘法链新增的最终误差很小。但刚加密后的数已经与原始输入有一点偏差，连续平方会把这点偏差放大。即使后面的平方完全精确，这两份数据仍达不到原先要求。因此，原测试仍然失败，Linux 还多一个差值精度失败，不能宣布整个项目完成。

目前没有找到足以支持修改生产代码的具体错误。最难的剩余问题是：怎样为公开加密、输入范围、初始精度和后续计算共同制定一个真正能达到目标的精度预算，而不是再补一个观察器。下一步只做一件事：把“论文计算方法已得到有界验证”和“原始输入端到端 80 位目标尚未通过”写成明确分开的版本化交付契约，保留所有失败，不能把小的新增误差改名为原目标成功。
