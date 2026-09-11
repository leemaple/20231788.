# Independent first-pass mathematical review: initial phase

Date: 2026-09-11, Asia/Shanghai. Reviewer: separate Codex review context, independent fallback for the additional adversarial responsibility; requested model/backend identity is unverified. This is not a Fable, ZCode, or independent-provider review.

## First-pass decision and independence

**Conditional acceptance for the bounded N256 initialization diagnostic. No concrete P1/P2 mathematical or semantic defect was identified in the reviewed test.** Acceptance is of the proposed properties and their scope, conditional on the integration owner's verified build and remote execution. No build or cryptographic result is claimed here.

This first-pass judgment was written from `TASK.md`, the returned C++ and CMake append file, and frozen source/specification entries in the verified input ZIP. The reviewer did **not** open the return author's `DIAGNOSIS.md`, `TEST_PLAN.md`, `MUTATION_PLAN.md`, `COVERAGE.md`, execution conclusions, or any other returned author verdict before this document. The C++ comments were treated as claims to check against the official implementation, not as a mathematical authority.

Reviewed identities, observed by read-only hashing:

| Material | Identity |
| --- | --- |
| Returned `pro/tests/initial_phase_exact_contract_test.cpp` | 20,325 bytes; SHA256 `f9c6ba30e8d9b0d19c0e4d0baccd9621e18696f6f61e7801cdb91b8940490af3` |
| Returned `pro/CMakeLists.append.txt` | 1,122 bytes; SHA256 `8c049db6c2db1709856e18e68b228204e5904d4a1c2b5ba8c8a5e00bbadf5a7e` |
| Input `artifacts/handoffs/testing-methods-20260911/testing-methods-33722b9.zip` | SHA256 `572cf0db6765dfbb1c8ee658d2425fc8e27933a540adf7b1d1145310e50de809` |
| Frozen project source | `33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1`, as bound by the task and verified input ZIP |
| Official source pin | OpenFHE 1.5.0 `df495ba2e91739a6dc8f1de254fc5a41155ce504`, native64/backend4/HAVE_INT128 |

Work was restricted to `/Users/lifeng/Documents/20231788-openfhe-testing-methods-20260911`. Only the explicitly required project skill and engineering/model-routing instructions were read from the old parent directory. No quarantined implementation, modified OpenFHE, or neighboring `.zcode` source was read, reused, built, or executed. No Git command, browser, CI dispatch, sampler, crypto operation, FFT or NTT was executed. The only numerical execution was bounded standard-library integer/Decimal arithmetic on public constants. This review document is the only file owned or changed by this reviewer.

All `official/` and `project/` paths below refer to entries read directly from that input ZIP. Test line numbers refer to the returned source with the SHA256 above.

## Actual source path and support assumptions

The public path is real and is the same lower-level path used by the production client after encoding. `project/src/high_precision_client_io.cpp:621-624` constructs a large coefficient `Poly`, imports it into `DCRTPoly`, and calls `GetScheme()->Encrypt`. Test lines 208-220 and 390 exercise those constructors and that public scheme method. They deliberately bypass the high-precision encoder, binding checks and metadata updates at production lines 610-618 and 625-636; those are outside this slice.

`official/src/pke/include/schemebase/base-scheme.h:205-238` forwards Encrypt, Decrypt and EncryptZeroCore to the enabled PKE implementation. `official/src/pke/lib/scheme/ckksrns/ckksrns-scheme.cpp:42-47` installs `PKECKKSRNS`; `official/src/pke/include/scheme/ckksrns/ckksrns-pke.h:45-75` inherits encryption from `PKERNS` and overrides decryption. The fixed-Q adapter also validates actual implementation types, not just enum labels (`project/src/paper_h128_client_keypair.cpp:90-102`).

The adapter samples exactly h128 using the official ternary constructor and calls the official secret-key `EncryptZeroCore` (`project/src/paper_h128_client_keypair.cpp:193-210`). The actual RNS implementation returns `(a*s + e, -a)` at full Q, because noiseScale is 1 (`official/src/pke/lib/schemerns/rns-pke.cpp:111-145`). Thus the independent public-key phase in test lines 309-319 must equal the key error `e`.

The public-key encryption path adds the supplied polynomial after obtaining `(pk0*v + e0, pk1*v + e1)` (`rns-pke.cpp:56-69,148-196`). Since the frozen secret-distribution flag is SPARSE_TERNARY, the ephemeral `v` uses the default dense ternary constructor, **not** h128. Consequently, in the integer negacyclic ring,

`c0 + c1*s - p = e*v + e0 + e1*s`.

The Gaussian constructor draws one integer vector and maps that same vector into every tower (`official/src/core/include/lattice/hal/default/dcrtpoly-impl.h:125-149`). The ternary constructor likewise uses one vector across towers (`:178-192`); the default h is zero, and that path yields coefficients in {-1,0,1} (`official/src/core/include/math/ternaryuniformgenerator-impl.h:103-143`). Test lines 276-291 independently check that the secret is coherent ternary with exactly 128 nonzero coefficients in every tower.

For the actual DGG, `GetDiscreteGaussianGenerator()` returns `m_dgg` (`official/src/pke/include/schemebase/rlwe-cryptoparameters.h:272-274`). At sigma `double(3.19f)`, the official inversion branch is selected because the Karney threshold is 300 (`official/src/core/include/math/discretegaussiangenerator.h:79` and `discretegaussiangenerator-impl.h:62-66`). Its finite table length is `ceil(sigma * 12.00610553538285)`; a returned sample is zero or a signed index from 1 through that length (`discretegaussiangenerator-impl.h:75-114`). If table lookup fails it throws; this cannot be mistaken for a numeric pass by the test.

Bounded Decimal arithmetic, without sampling, gave:

```text
sigma = 13379830 / 4194304 = 3.190000057220458984375
sigma * 12.00610553538285 = 38.29947734486616084945201874
G = ceil(...) = 39
F = G * (N + H + 1) = 39 * (256 + 128 + 1) = 15015
```

Each coefficient of `e*v` contains at most N signed terms of magnitude G; each coefficient of `e1*s` contains exactly H potentially nonzero signed terms of magnitude at most G. Add the single `e0` coefficient. Therefore F is a deterministic support bound for this pinned, finite inversion-sampler path; the proof needs no independence, seed assumption, frequency estimate or 1,000-trial campaign. It is not a claim that an ideal unbounded Gaussian has bounded support, nor a bound applicable to the Karney branch or arbitrary sigma. Test line 377 checks the actual DGG standard deviation, in addition to the public metadata checks.

## Transform oracle, sign and full coefficient coverage

For N=256 and a primitive order-512 root r, define `z_k = r^(2*reverse8(k)+1)`. The fixed native transform's table is `Table[reverse8(i)] = r^i` (`official/src/core/include/math/hal/intnat/transformnat-impl.h:714-738`). Its staged butterfly reads `Table[m+i]`, with sum/difference outputs, including the final paired stage (`:303-374`); this is the negacyclic evaluation ordering `EV[k] = a(z_k)`. The `Poly` coefficient-to-evaluation path calls that transform (`official/src/core/include/lattice/hal/default/poly-impl.h:420-439`). Root/sign/order were checked from the butterfly structure, not inferred from the test's comment.

Because reversing eight bits permutes 0..255, the points enumerate all odd powers of r. For coefficient indices i,j in 0..255,

`sum_k z_k^(j-i) = N` when i=j, and is zero otherwise, by the finite geometric series in `r^2`. The moduli are distinct primes under the adapter's basis validation (`project/src/paper_h128_client_keypair.cpp:67-87`), and N is invertible. Thus

`a_i = N^(-1) * sum_k EV[k] * z_k^(-i) mod q`

is precisely the inverse implemented by test lines 223-257. There is no missing twist factor or use of the ordinary cyclic inverse formula. The test checks `r^N=-1`, `r^(2N)=1` and inverse existence at runtime. For this power-of-two N, `r^N=-1` establishes exact order 2N in each prime field.

The oracle uses `cpp_int` extended Euclid, modular arithmetic and direct summation; it does not invoke the official inverse transform or CRT routine. The independent forward anchor also evaluates the public polynomial by Horner at every point (`:258-274`). The vector is signed, non-palindromic, spans indices 0,1,2,127,128,255 and exceeds both binary64 exact-integer and uint64 widths (`:184-195`). It helps reject a shared wrong convention between the independent forward and inverse formulas, because the real official forward output must match that formula for the chosen polynomial.

The schoolbook convolution in `Phase` adds terms when i+j<N and subtracts them when i+j>=N (`:293-307`), correctly applying `X^N=-1`; it is not pointwise coefficient multiplication. Loop bounds cover all 256 coefficients, all 256 evaluation coordinates and all three towers. Import, forward anchor, inverse anchor, secret, public-key phase, fresh phase and final `Poly` comparison have no sampled-coefficient shortcut.

This is finite vector coverage, not a proof of all possible inputs or a universal transform test. A single sparse public anchor does not exclude every linear defect that happens to vanish on that vector; the actual randomized secret/ciphertext phases provide additional exercised values but no quantified defect-detection probability.

## Centering, CRT uniqueness and decrypt comparison

Test `Center` maps residues into the unique symmetric interval for each odd modulus using `2*x > q` (`:62-63`). The public coefficients may greatly exceed an individual q, so `FreshResidual` correctly subtracts p **before** centering modulo that q (`:326-328`). It does not interpret each tower's centered phase as the full plaintext coefficient.

The reviewed public constants give

```text
Q = 1361129441679586514673376111479624265217 (130 bits)
min(q_i) = 1099511603713
3*F + 1 = 45046
min(q_i) > 2*(3*F+1)
2*(max_i |p_i| + F) < Q
```

Therefore a legitimate residual has one unambiguous small integer lift in every tower. The test requires exact cross-tower equality before checking support (`:330-333`). For the baseline, p+f also lies strictly inside the centered interval modulo full Q (`:194`). The modulo-Q expectation remains well-defined even without that latter nonwrap check; the check is what additionally supports its intended signed integer interpretation.

`PKECKKSRNS::Decrypt(Poly*)` computes the official phase, changes format to coefficients, then CRT-interpolates when more than one tower remains (`official/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:70-95`). Under the asserted FIXED_NOISE_DECRYPT path, it does not add flooding noise. The independent phase has already established that every tower equals p+f mod q_i. Uniqueness of CRT then supplies an expected representative `Mod(p+f,Q)` without calling official CRT inside the oracle. Test lines 335-344 compare every returned coefficient exactly with that expectation.

Using the actual same ciphertext and secret as inputs to both phase calculations is appropriate for this differential check. It is not a common-mode reuse of official INTT, native modular multiplication, or CRT to compute the expected answer. However, f is measured from the ciphertext, rather than from separately observed `v,e,e0,e1`; therefore this comparison proves agreement with that measured phase, **not** an independent exact reconstruction of the encryption randomness or the intended noise distribution.

## Controlled mutations and scientific interpretation

Both controls modify owned copies of the one fresh ciphertext; the baseline is not resampled. Official `Ciphertext::Clone()` copies its elements (`official/src/pke/include/ciphertext.h:402-405`) and native `Poly` copy construction owns a fresh values vector (`official/src/core/include/lattice/hal/default/poly.h:133-136`). Test lines 345-360 make additional value copies before mutation; lines 397-398 recheck the baseline.

Adding delta to **all evaluation coordinates** in one tower adds the coefficient polynomial delta at index 0, since a constant evaluates identically at every point. Hence the mutation at lines 348-357 is the claimed coefficient perturbation, not an accidental evaluation impulse.

| Mutation | Proof for every baseline satisfying the checks | Required class |
| --- | --- | --- |
| One tower plus 1 | Only tower 0's coefficient-0 residual becomes f0+1. It remains within the centering interval because q0 is much larger than 2(F+1), and differs from the other two residuals by exactly 1. | `FRESH_CRT_COHERENCE` |
| All towers plus 30031 = 2F+1 | All coefficient-0 residuals become f0+2F+1, in [F+1,3F+1]. The per-tower headroom prevents recentering, so coherence holds but support necessarily fails. | `FRESH_SUPPORT_BOUND` |

`ExpectRejected` accepts only a `NumericFailure` of the expected class (`:362-371`). Other contract failures, library exceptions, process failures and timeouts do not satisfy this gate. In fault-only modes the intended numerical failure exits 1 (`:400-405,420`); a runner must inspect that classification and must not count any arbitrary nonzero exit as a caught numerical fault. The `--controls` invocation tests both controls against one same in-memory sample. Separate invocations of `--baseline` and either fault-only mode generate new samples; they must not be described as sharing noise across processes.

These are deliberately corrupted-ciphertext negative controls. Their success shows the diagnostic detects the intended coherence/support violations, not that a specific production implementation defect was introduced and repaired. They do not separately mutation-test the final decrypt comparison or exhaust possible transform faults.

## Acceptance limits and remaining evidence

1. A passing run would establish this fixed N256 profile's exact public import/transform anchor, ternary h128 key coherence, bounded/coherent public-key and fresh phases, and agreement of official `Poly*` decrypt with an independent exact phase oracle for one fresh sample. It would also establish the two declared negative controls on that same sample.
2. Coherent deviations that leave the residual within [-15015,15015] can pass the support check. Missing or altered error terms can also remain in that set. This test does not observe the exact `e*v+e0+e1*s` decomposition and does not validate the shape or variance of the sampling distribution. A PASS therefore cannot eliminate all initialization defects or justify concluding that normal noise amplification is the unique original failure cause.
3. N256, three towers and this public integer vector are not the historical N32768 original-S100 ciphertext, paper input, modulus chain, encoder or eight-square operation. Size-specific transforms/tables, production binding/metadata, downstream precision and historical noise remain untested here. No exact-zero encryption roundtrip or approximate-slot identity is asserted, which is appropriate.
4. A numerical failure would reveal a violated necessary invariant or disagreement in this current slice. Its exact source could still be the oracle, profile, dependency/build mismatch, adapter or public PKE path; the failure label alone is not an adjudicated root cause. The first-pass review found the proposed oracle mathematically sound but does not replace execution evidence.
5. The CMake append is opt-in, excludes the binary from the default build, supplies the configured source commit, and registers a serial 180-second `--controls` CTest. The root owner must explicitly build that target and retain exact source/dependency identities, classified output and remote run receipts. The declared dependency-pin string in test output is not itself a binary-provenance check. Runtime frozen-basis checks and compile-time platform assertions still need to pass.

Pending evidence: compilation under the intended remote toolchain(s); baseline and same-process controls execution; classified red receipts if fault-only modes are run; exact built-source and pristine-dependency reconciliation. No request for extra providers, a large statistical sweep, production changes or a new approval boundary follows from this static review.

The original S100 failure and threshold remain unchanged. This is a useful additional discriminating initialization test, with the limitations above, rather than proof of original-S100 completion.

## Review execution record

Read-only commands used `shasum -a 256`, `wc -c`, `unzip -Z1`, and `unzip -p <verified ZIP> <specific entry> | nl -ba` or focused `rg`/`sed` reads. The current task and returned C++/CMake were read directly in the clean-room. No returned author analysis was read.

The sole numerical command was `python3 -c` importing only `decimal.Decimal`, `decimal.ROUND_CEILING` and `math.prod`, evaluating the public sigma fraction, finite support product, F, mutation constants, Q and the two headroom inequalities. It produced the public values recorded above; no file, random state, source transformation or cryptographic data was accessed by that command.
