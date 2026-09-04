# Bounded path from this tracer to the paper Section 6.3 destination

This file is a gate sequence, not an implementation plan for the current patch. Each gate must preserve the full destination: lossless usable I/O, first and repeated Double-CKKS multiplication, the paper’s ordered parameters, and honest evidence.

## G0 — accepted DCP→RCB precision transport

**State: observed green on Linux and Windows.**

N=64, p50/50, HYBRID, `HEStd_NotSet`, degree two, `2^100`; four fresh keys per host; all-slot and delta `2^-80`. This establishes only the test-owned high-precision fixture and DCP→RCB transport.

## G1 — first Mult2 precision at the current diagnostic context

**State: candidate in this delivery; hosted result pending.**

Run the new test unchanged. A pass closes one first-Mult2 HYBRID diagnostic. A failure identifies whether p50/50, first-Mult2 arithmetic/noise, or the test’s source assumptions need a precise correction. No threshold fitting is allowed.

## G2 — explicit ordered Div40/Mult60 parameter context

**Required decision before code:** approve a project-owned parameter construction seam that creates and validates the exact ordered chain without forking OpenFHE or weakening current pair validation.

The current `CCParams` path supplies one `scalingModSize` to the generated Q-chain (`official-openfhe/ckksrns-parametergeneration.cpp:L164-L180`) and the single-prime generator derives the non-first primes from that common bit size (`L415-L444`). Under FIXEDMANUAL, both the reported scaling factor and mod-reduce factor are the same approximate `2^p` (`official-openfhe/rns-cryptoparameters.h:L601-L649`). It therefore does not by itself express the paper’s ordered Base `50×2`, Mult `60×8`, Div `40` chain.

The minimal next parameter artifact must freeze and test:

```text
ring N                 = 2^15
input Delta            = 2^100
base Q0                = two ~50-bit primes
multiplication primes  = eight ~60-bit q_l primes
DCP divisor            = one final ~40-bit q_div prime
HYBRID auxiliary P     = ~60-bit primes
HYBRID gadget rank     = dnum 11
secret Hamming weight  = 128
```

It must also freeze the exact tower order expected by the existing architecture: DCP removes the final `q_div`; each RS2 consumes the current last active `q_l`; the base remains as the prefix. A constructor/precompute audit is required before selecting the public OpenFHE parameter API. This delivery does not claim that merely setting p=50, or merely using wider RNS words, produces this regime.

## G3 — production lossless input and output semantics

**Required public API decision.**

The current adapter is intentionally test-only because its private packed cache is stale. Production needs an explicit, documented representation whose source values, polynomial, exact scale, serialization, validation, and decode path agree. The smallest acceptable decision is an additive project-owned lossless codec/value type; it must not masquerade as a normal `CKKSPackedPlaintext` whose getters return placeholder doubles.

Required tests include decimal/rational complex input, exact dyadic controls, sub-binary64 differences, round-trip serialization if supported, wrong-context/basis/scale rejection, and output formatting that does not truncate evidence to binary64.

## G4 — lifecycle for repeated multiplication

**Required algorithm/API decision.**

Current first `Mult2` intentionally returns `RefreshRequired` (`project/src/double_ckks.cpp:L1100-L1112`) and a second `Mult2` is rejected. The full paper destination needs an explicit next-state operation—refresh/recombine-decompose or another source-justified transition—with its scale, basis, key, and error semantics specified. Do not silently remove the lifecycle guard or add automatic refresh.

Before repeated squaring, freeze tests for:

- exact state transition and tower consumption;
- lossless scale carry-forward;
- no mutation and key/context binding;
- independent coefficient/canonical oracle after each level;
- low-part growth measurements kept separate from universal bounds;
- failure at exhausted modulus or missing refresh material.

The paper notes that its 40/60 margin maintained precision without the Section 6.2 recombine/decompose strategy (`PAPER-2023-1788.txt:L1568-L1576`), but that observation does not automatically define this project’s public lifecycle. The project still needs an explicit, reviewed transition consistent with its representation.

## G5 — exact Section 6.3 / Table 3 experiment

**State: pending all prior gates.**

The paper’s target is not “100-bit metadata.” It is the concrete regime and protocol stated in Section 6.3:

- plaintext/scale about 100 bits;
- multiplicative depth 8;
- Double-CKKS `q≈2^60`, `q_div≈2^40`;
- N=`2^15`, h=128;
- dnum=11, Base `50×2`, Mult `60×8`, Div `40`, auxiliary P `60`;
- eight repeated squarings;
- 1,000 executions;
- reported average error precision about `-81.8` bits (`PAPER-2023-1788.txt:L1562-L1576,L1580-L1590`).

Before execution, define exactly:

1. input distribution and magnitude envelope;
2. whether the statistic is the mean of per-run infinity norms, the log of a mean, or a mean of log errors;
3. key/noise sampling and any reproducible experiment-only seeds;
4. failure/non-wrap exclusion rules without silently dropping runs;
5. exact canonical reference and scale at every squaring;
6. host/toolchain/threading and warm-up policy;
7. comparison to the paper as a reproduction attempt, not a promised match.

## G6 — security and performance claims

Security estimation and benchmark evidence are separate gates. G1 uses `HEStd_NotSet` and cannot support a security claim. G5 parameters require an actual security estimate under the selected secret distribution and full QP modulus. Latency, memory, ciphertext size, and key size must be measured under the frozen Table-3 protocol; functional or precision tests are not benchmarks.

## Stop conditions

Do not advance a paper claim when any preceding gate has only a proposal, a static check, or a diagnostic context. A G1 failure does not redefine the project as ordinary CKKS or low-precision compatibility; it directs the next experiment toward the exact ordered parameter and lifecycle decisions above.
