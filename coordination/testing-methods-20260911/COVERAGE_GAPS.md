# Precision-diagnosis test coverage and bounded gaps

Date: 2026-09-11 (Asia/Shanghai). Read-only review base: clean-room commit
`33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1` on
`codex/testing-methods-diagnosis-20260911`. All `path:Lx-Ly` references below
are fixed to that commit. No build, FFT/NTT, sampling, cryptographic execution,
CI, browser action, source edit, or test edit was performed for this map.

This review applies the useful parts of the requested property-testing method:
state an executable invariant, use deterministic generated cases with a saved
seed, include boundary partitions, shrink a failure to the smallest coefficient
pattern, and retain explicit mutation sentinels. It does **not** infer a bug from
a missing test and does not propose a large randomized campaign.

## Result

The public arithmetic seams already have unusually strong exact-oracle,
boundary, negative-state, and immutability coverage. The highest-value uncovered
question is at public-key initialization: the adopted source proof gives a hard
coefficient envelope for honest PKE noise, but the live diagnostic does not turn
that bound into an assertion on its actual `p-m`. A second, lower-cost gap is
general dense/near-rounding-boundary coverage of the deterministic public
encoder. A paper-size all-slot intermediate-stage check is a real remaining
localization gap, but it is expensive and should not be dispatched unless the
initialization checks or an independent mathematical review give it a concrete
decision role.

## Existing coverage map

Legend: **yes** means an executed-test source or retained replay contract exists;
**partial** means a narrow fixed instance/anchor set rather than the general
property. Historical execution status is not re-proved by source presence.

| Public boundary | Deterministic property / exact boundary | Differential oracle | Mutation / non-mutation | Replay status |
| --- | --- | --- | --- | --- |
| High-precision encoder | **yes, fixed cases**: exact real/imaginary constants, signed nearest rounding, exact-half rejection, monomial and one mixed sparse polynomial (`tests/s100_fresh_error_diagnostic_test.cpp:L282-L330`). Production `StableRound` uses two precisions and rejects insufficient distance from a half (`src/high_precision_client_io.cpp:L417-L437`). | **yes, narrow plus one paper input**: the small controls generate slots by an independent direct Horner path (`tests/s100_fresh_error_diagnostic_test.cpp:L220-L245`); the current public S100 polynomial has an adopted all-32,768-coefficient interval certificate (`coordination/public-s100-ecd-cell-20260909/ADOPTED_RESULT.zh-CN.md:L13-L24`). | **yes**: wrong slot order, sign and scale sentinels, input/result ownership and repeat inspection (`tests/s100_fresh_error_diagnostic_test.cpp:L331-L341,L388-L410,L622-L629`). | **yes for current public `p`, not historical `p`**: certificate/replay is hash-bound to the saved current polynomial; it does not identify the historical Linux/Windows polynomial (`coordination/public-s100-ecd-cell-20260909/ADOPTED_RESULT.zh-CN.md:L38-L42`). |
| Public-key initialization and fresh phase | **partial**: fixed profile guards, exact h=128/sign-balance and key/context/cache lifecycle (`tests/paper_h128_client_keypair_contract_test.cpp:L199-L239,L259-L290,L408-L524`). The live S100 diagnostic performs one public encryption and repeats the shared encoder deterministically (`tests/s100_fresh_error_diagnostic_test.cpp:L576-L624`). | **yes, one sampled phase**: exact `m`, independently sparse-decrypted `p`, raw `p-m`, production decode, full-slot two-precision observations and ten Horner anchors are separated (`tests/s100_fresh_error_diagnostic_test.cpp:L588-L644,L646-L690`). | **yes for objects/state**: keys, input values, ciphertext receipts and clones are checked unchanged (`tests/s100_fresh_error_diagnostic_test.cpp:L625-L629`; `tests/precision_client_io_first_mult2_contract_test.cpp:L745-L776`). No mutation sentinel currently targets the adopted numerical PKE support envelope itself. | **partial**: retained endpoint TSV/status and scalar replayers reproduce observed summaries, not the historical random key/noise/ciphertext. The original run artifacts record source/run/status but contain no replayable secret/key state (`coordination/fs-endpoint-live-run-01/ARTIFACTS_METADATA.json:L1-L44`; `coordination/fs-endpoint-live-run-01/linux/fs-residual-endpoint-01.v1-r1.ed5fd192a89d6d4728ad295e87cf06a3f4abc832.linux.34039088536.1.status.json:L1`). |
| `DCP` / `RCB` | **yes**: 21 quotient/remainder boundary values include zero, signs, `d/2`, `Q/2`, `d`, and neighbours (`tests/dcp_rcb_test.cpp:L273-L303`). | **yes, every component/tower/coefficient**: independent `cpp_int` centered quotient/remainder and recombination (`tests/dcp_rcb_test.cpp:L368-L427,L478-L526`). | **yes**: input/pair deep immutability plus divisor, basis, tag, format, scale, level, degree and descriptor tampering (`tests/dcp_rcb_test.cpp:L483-L526,L652-L762`). | Deterministic fixtures are directly rerunnable; no missing precision-localizing replay was found at this seam. |
| `Tensor2` | **yes**: explicit negacyclic wrap, signed modular wrap and omitted-low-low witnesses (`tests/tensor2_test.cpp:L231-L243,L497-L528`). | **yes, every component/tower/coefficient**: schoolbook negacyclic convolution and exact cross-term oracle (`tests/tensor2_test.cpp:L265-L370,L480-L528`). | **yes**: both input pairs are snapshot-checked; right-input, mutual-slot and pre-arithmetic key compatibility negatives exist (`tests/tensor2_test.cpp:L530-L532,L591-L645`). | Deterministic fixtures are rerunnable; the remaining paper-size issue is slot coverage at intermediate rounds, not Tensor2's fixed exact algebra. |
| `Relin2` | **yes**: generated-key valid path plus controlled `+half`, `-half/carry`, and nonzero `v+w` witnesses (`tests/relin2_test.cpp:L4135-L4159,L4185-L4252`). | **yes, every component/tower/coefficient**: independently built public relinearization paths followed by exact `(u,v+w)` and `RCB` checks (`tests/relin2_test.cpp:L3825-L3911,L4048-L4070`). | **yes, extensive**: tensor/cache immutability plus missing/malformed/wrong-context/tag/subtype and HYBRID/BV shape/basis/format cases are registered (`CMakeLists.txt:L212-L242`). | Deterministic controlled fixtures are directly rerunnable; no uncovered exact-carry case with a stronger link to S100 was identified. |
| `RS2` | **yes**: 21 centered-rescale boundary values and explicit distinct `q_l`/`q_div` witnesses (`tests/rs2_test.cpp:L474-L509,L532-L575`). | **yes, every component/tower/coefficient**: independent centered CRT/rescale oracle proves `RCB(RS2(pair)) = RS(RCB(pair))` (`tests/rs2_test.cpp:L685-L756`). | **yes**: sentinels distinguish the incorrect `RS(low)` shortcut and division by `q_div`; result/input/cache immutability and public HYBRID/BV paths are checked (`tests/rs2_test.cpp:L718-L770,L798-L885`). | Deterministic fixtures are directly rerunnable; no uncovered prime-role boundary with a stronger link to S100 was identified. |
| Full repeated chain | **partial**: exact scales/receipts and all eight public `Mult2` calls are checked; fresh/final all slots are checked, but rounds 1-8 use only ten Horner anchors (`tests/paper_full_eight_square_contract_test.cpp:L258-L320,L335-L360`). | **partial**: each intermediate pair is sparse-decrypted to a full integer polynomial, but only coefficient maximum plus ten slot values are observed (`tests/paper_full_eight_square_oracle.h:L264-L332`). | **yes for ownership/state**: source/key values, receipts, terminal clone and unrelated key rows are protected (`tests/paper_full_eight_square_contract_test.cpp:L321-L334,L380-L416`). | **yes for endpoint scalar evidence**, not exact cryptographic re-execution. The accepted completion review explicitly says ten anchors do not prove all intermediate slots (`coordination/completion-contract-20260909/pro/COMPLETION_CONTRACT.zh-CN.md:L180-L186`). |

The default registry makes this distinction visible: operator tests and
small end-to-end real/complex cases are registered separately from the one
paper full-chain test (`CMakeLists.txt:L200-L267`). Passing names must not be
counted as independent random samples.

## At most three concrete, not-already-covered gaps

### G1 — Assert the adopted PKE coefficient envelope on one actual fresh phase (highest priority)

**Missing property.** The adopted initialization contract proves, subject to
the fixed successful sampler path, `f = p-m = e_pk*v+e0+s*e1` and
`||f||_coeff <= 1,282,983` (with canonical bound `42,040,786,944`), but the live
fresh diagnostic only requires raw-lift headroom and observer consistency; it
does not assert either adopted envelope (`coordination/initial-lift-nonwrap-20260909/ADOPTED_CONTRACT.md:L7-L18` versus
`tests/s100_fresh_error_diagnostic_test.cpp:L610-L621,L632-L690`).

**Callable interfaces and smallest discriminating experiment.** Reuse exactly
one invocation of the existing opt-in fresh diagnostic path—no retry and no
new chain: `CreatePaperRepeatedMult2Setup()` ->
`HighPrecisionClientIO::InspectEncoding()` -> `Encrypt()` ->
`BoundCiphertext::CloneForEvaluation()`. Reuse its test-only `ReadSecret`,
`SparseDecrypt`, and `RawDifference`; scan all 32,768 raw coefficients and assert
`max |p_j-m_j| <= 1,282,983`. Independently evaluate the already-constructed raw
difference at all slots (or, cheaper first, the existing ten anchors) and compare
against the canonical envelope. These public setup/I/O entry points are declared
at `include/openfhe_2023_1788/repeated_mult2.h:L58-L62,L117-L123` and
`include/openfhe_2023_1788/high_precision_client_io.h:L144-L168`.

Add a test-oracle mutation sentinel, not a production mutation: change one copied
raw difference coefficient to `1,282,984` and require the envelope checker to
fail. This proves the check is capable of discriminating its target.

**Falsifiable prediction.** A violation contradicts a currently adopted
initialization premise and would localize a concrete sampler/profile/lift issue
before `DCP`; a violation is large enough to be a plausible contributor to S100.
A pass does **not** say the noise is normally distributed, typical, secure, or
small enough for E80 after conditioning; it only rules out this hard-envelope
failure in that one sample. Because a fresh draw is not the historical draw, it
cannot replay run 34039088536.

### G2 — Dense, shrinkable encoder round-trip and near-half partitions (lower cost)

**Missing property.** Existing public-encoder controls are excellent but sparse:
constants, a monomial, one mixed polynomial, quarter-integer nearest cases, and
exact ambiguous halves. The current all-coefficient certificate is for one
specific S100 polynomial. There is no deterministic generated property over
dense signed polynomials, nor accepted cases close to both sides of the
ambiguous-half exclusion (`tests/s100_fresh_error_diagnostic_test.cpp:L282-L341`;
`coordination/public-s100-ecd-cell-20260909/ADOPTED_RESULT.zh-CN.md:L20-L24,L38-L42`).

**Callable interface and smallest discriminating experiment.** On the existing
N64/S16 profile, use a fixed recorded seed to generate 8 bounded dense signed
coefficient vectors on the legal projection lanes. Compute their slots through
the already independent direct-Horner helper and require
`InspectEncoding(slots,spec).signedCoefficients == coefficients`. Partition two
additional constant cases so the intended scaled coefficient is
`k + 1/2 - 2^-300` and `k + 1/2 + 2^-300`; both must be accepted with the
paper half-down result, while the existing exact-half case remains rejected.
On failure, shrink in this order: zero coefficients, halve magnitudes, minimize
the nonzero support, then move the fractional distance toward the half. Record
the seed and minimized vector. `EncodingInspection` explicitly exposes the exact
pre-residue coefficients used by `Encrypt` (`include/openfhe_2023_1788/high_precision_client_io.h:L70-L80,L153-L160`).

**Falsifiable prediction.** A minimized dense mismatch or wrong near-half side
would be a current transform/rounding bug and could affect fresh precision. A
pass generalizes the present sparse controls but is unlikely to explain the old
S100 outcome, because the current S100 `p` already has a strict all-coefficient
certificate with a minimum positive half-cell margin. This gap is therefore
contract hardening, not a reason to reopen the adopted current-`p` certificate.

**Numerical limit.** The proposed `2^-300` offset must be constructed and checked
in the existing multiprecision types; if conversion to `ClientReal` cannot retain
the prescribed side exactly, the fixture is invalid and must be moved farther
from the half, not silently accepted as a rounding failure.

### G3 — Full-slot observation at the earliest failing intermediate round (defer unless justified)

**Missing property.** The original paper-size test sparse-decrypts each round's
recombined polynomial but evaluates only ten anchors at rounds 1-8. Fresh and
terminal values alone have full 16,384-slot checks (`tests/paper_full_eight_square_contract_test.cpp:L286-L320,L335-L344`).
Historical Linux first missed an anchor gate at round 3 and Windows at round 4;
the retained final disposition says the endpoint is inherited-dominant but does
not give all-slot intermediate localization (`coordination/fs-endpoint-live-run-01/ACCEPTANCE.md:L16-L35`).

**Callable interfaces and smallest discriminating experiment.** Only if a new
paper-chain execution is independently justified, reuse one predeclared chain
and the existing public sequence `DCP` -> `Tensor2` -> `Relin2` -> `RS2` (or
`Mult2`) -> `RCB`; these are the public methods at
`include/openfhe_2023_1788/double_ckks.h:L147-L162`. At the earliest round whose
fixed anchors fail, apply an independent full-slot forward transform to the
already reconstructed `RecombinedPolynomial`. For every slot retain
`E_r = w_r-z^(2^r)`, `I_r = w_0^(2^r)-z^(2^r)`, and
`A_r = w_r-w_0^(2^r)`, and check same-slot `E_r=I_r+A_r` before taking maxima.
Do not run all eight extra transforms until the first selected round answers the
question.

**Falsifiable prediction.** If full-slot `A_r` is already comparable to or larger
than `I_r`, a post-initialization evaluator contribution deserves seam-by-seam
localization. If `A_r` is small while `I_r` explains `E_r`, the result reinforces
the current initialization/conditioning diagnosis. A new draw cannot prove the
historical draw, and even a large `A_r` does not by itself say whether Tensor2,
Relin2, or RS2 caused it; separate pre/post-seam phase observations would then be
a follow-on, not part of this smallest experiment.

## Exact known failure fixture and replay limit

The deterministic **input and parameter fixture** is known exactly:

- `N=32768`, `slots=16384`, `M=65536`, `S0=2^100`, ordered original Q/roots and
  `d=1099510054913` are literal at
  `tests/paper_full_eight_square_oracle.h:L29-L47,L78-L91`.
- For slot `s`, let `t=floor(s/2)`,
  `a=1015/1024-(t mod 16)/65536+s*2^-75`, and
  `b=(1+(floor(t/16) mod 8))/1024`, negate `b` when
  `floor(t/512)` is odd, then apply the four quarter rotations. The literal
  implementation is `tests/paper_full_eight_square_oracle.h:L97-L119`.
- Historical run `34039088536`, attempt 1, source
  `ed5fd192a89d6d4728ad295e87cf06a3f4abc832`, completed one public encryption
  and eight squares on each host. Linux final maximum component error was
  approximately `9.14647363e-24`; Windows approximately `9.06530517e-24`, both
  above `2^-80` (`coordination/fs-endpoint-live-run-01/ACCEPTANCE.md:L1-L18,L20-L35`).

There is **no exact replayable cryptographic failure fixture**: the historical
secret, public/evaluation keys, encryption randomness, and ciphertext chain are
not retained as a reconstructible test vector. The saved status/TSV artifacts
replay the observations and causal scalar decomposition, not the randomized
OpenFHE execution. Therefore the exact known fixture is the input/profile plus
the immutable observed result; it must not be described as a deterministic
ciphertext reproduction. The recent replay of the separate ten-anchor `A+B`
ideal-propagation record likewise reproduces a scalar implication for its own
fresh sample, not run 34039088536.

## Recommendation boundary

Give G1 to the next independent review as the primary candidate because it
directly tests an adopted initialization premise with one bounded sample and an
exact falsifier. G2 is cheap generality/rounding hardening. Defer G3 unless G1
fails, an independent review finds a precise evaluator hypothesis, or the user
explicitly authorizes a new paper-size cryptographic run. None of these gaps is
itself evidence of a production defect, and none authorizes changing S100 input,
threshold, parameters, noise, or truth data.
