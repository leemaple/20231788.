# Tensor2 scale derivation for the OpenFHE 1.5.0 seam

Recorded: 2026-09-01 Asia/Shanghai

## Scope and evidence boundary

This note resolves the scale question for the clean-room `t=2` Tensor2 slice.
It is an algorithm and OpenFHE-integration analysis, not a network-security
review. It does not claim an implementation, build, CTest, precision result,
or Windows result.

The derivation uses only:

- the user-supplied IACR ePrint 2023/1788 paper, especially Definition 3.3,
  Definition 4.1, Lemma 4.2, and the modulus-consumption discussion following
  Theorem 4.8;
- official pristine OpenFHE 1.5.0 at commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`;
- the current greenfield DCP/RCB public contract at exact commit
  `87c84b879c13b55cf15d6559d3317853228fdc05`.

No prior or local 2023/1788 implementation was inspected or reused.

## Paper algebra

Let the two valid input pairs be

```text
CT_i = (h_i, l_i)
c_i  = RCB_q(CT_i) = q h_i + l_i,
```

where `q = q_div`. Definition 4.1 gives

```text
Tensor2(CT_1, CT_2) = (H, L)
H = h_1 tensor h_2
L = h_1 tensor l_2 + l_1 tensor h_2.
```

Expanding the ordinary tensor product gives the exact ring identity

```text
c_1 tensor c_2
  = (q h_1 + l_1) tensor (q h_2 + l_2)
  = q^2 H + q L + (l_1 tensor l_2)
  = q * RCB_q(H, L) + (l_1 tensor l_2).
```

This is the identity in Lemma 4.2. Therefore Tensor2 deliberately omits the
low-low term and its recombination represents the ordinary product divided by
`q`, subject to the lemma's centered-range condition and its stated low-low
error.

## The two paper scales are distinct

For input pair `i`, define:

- `R_i`: the logical scale of the recombined value `q h_i + l_i`;
- `H_i`: the logical scale of its high quotient component `h_i`.

For a DCP-created pair, `H_i = R_i / q`. The Tensor2 result therefore has two
different, independently derived scale facts:

```text
H_out = H_1 * H_2
R_out = R_1 * R_2 / q.
```

They are consistent because

```text
q * H_out = q * (R_1/q) * (R_2/q) = R_1 * R_2 / q = R_out.
```

The current DCP manifest maps to these inputs as follows:

```text
H_i = pair_i.GetPaperScale().approximateLogicalScalingFactor
R_i = pair_i.GetPaperScale().inputRecordedScalingFactor
q   = pair_i.GetDivisor()
```

Consequently a Tensor2 test must compute both expected output values from the
two input manifests and `q`. Reading either expected value from the result
would make the oracle circular. The actual prime `q` must remain an integer
manifest fact; it must never be replaced by OpenFHE's approximate base scaling
factor.

## Observed OpenFHE behavior

The following are source facts, not execution claims.

1. Under `FIXEDMANUAL`,
   `CryptoParametersRNS::GetScalingFactorReal(level)` returns the fixed
   approximate factor `Delta_0 = 2^p`, independent of the level
   (`src/pke/include/schemerns/rns-cryptoparameters.h:601-620`).
2. CKKS encoding raises that base factor to `noiseScaleDeg` and records the
   result (`src/pke/lib/encoding/ckkspackedencoding.cpp:300-333`). For the
   current multi-tower Tensor2 state, decryption selects the `Poly` path and
   passes the ciphertext metadata into decoding
   (`src/pke/lib/cryptocontext.cpp:574-595`). On that path,
   `FIXEDMANUAL` uses the discrete noise-scale degree to obtain the matching
   power of `2^p` (`src/pke/lib/encoding/ckkspackedencoding.cpp:375-383,
   465-483`). The single-tower `NativePoly` path instead uses only one inverse
   base factor (`src/pke/lib/encoding/ckkspackedencoding.cpp:345-350`) and must
   not be used to validate this multi-tower degree contract.
3. `EvalMultNoRelin` performs `TypeCheck` and delegates to the scheme's
   multiplication (`src/pke/include/cryptocontext.h:1961-1972`).
4. For `FIXEDMANUAL`, ciphertext-ciphertext multiplication only aligns tower
   counts before the core operation; it does not rescale equal-level operands
   (`src/pke/lib/schemerns/rns-leveledshe.cpp:182-191,479-484`).
5. The core two-by-two convolution produces exactly three components, adds the
   input noise-scale degrees, and multiplies their recorded scaling factors
   (`src/pke/lib/schemebase/base-leveledshe.cpp:620-660`).
6. A real OpenFHE modulus reduction changes coefficients, drops a tower,
   increments the level, decrements the noise-scale degree, and divides the
   recorded factor by `GetModReduceFactor`; for `FIXEDMANUAL` that factor is
   `Delta_0` (`src/pke/lib/scheme/ckksrns/ckksrns-leveledshe.cpp:172-190` and
   `src/pke/include/schemerns/rns-cryptoparameters.h:636-649`).

On the accepted first-lifecycle input, both pair members have

```text
d_i = 2
S_i = Delta_0^2.
```

Raw `EvalMultNoRelin` therefore produces

```text
d_raw = d_1 + d_2 = 4
S_raw = S_1 * S_2 = Delta_0^4.
```

This raw metadata describes an ordinary multiplication. It does not yet
represent the one logical `q`-division that Tensor2 contributes without a
tower drop.

## Derived module normalization contract

The paper requires one logical rescale by the actual `q`; OpenFHE
`FIXEDMANUAL` represents scale depth in discrete powers of `Delta_0`; and the
supported parameterization chooses `q` near, but not equal to, that base
factor. To preserve both the paper algebra and OpenFHE's standard downstream
rescale metadata trajectory, this project adopts two simultaneous
representations:

```text
paper descriptor:
  highLogicalScalingFactor       = H_1 * H_2
  recombinedLogicalScalingFactor = R_1 * R_2 / q

OpenFHE metadata:
  outputNoiseScaleDegree = d_1 + d_2 - 1 = 3
  outputRecordedFactor   = S_1 * S_2 / Delta_0 = Delta_0^3.
```

The first representation preserves the actual-prime algebra. The second keeps
the ciphertext on OpenFHE's `FIXEDMANUAL` degree schedule so that the later
physical RS2 modulus reduction removes one further tower/degree and returns to
degree two. The multi-tower `FIXEDMANUAL` decoder does not read an arbitrary
floating `scalingFactor`, so the stored factor formula is not the only value
forced by decoding alone; it is the module contract derived for compatibility
with subsequent OpenFHE operations and validation. Equating `q` and
`Delta_0`, or storing `S_1*S_2/q` as though it were the adopted OpenFHE
schedule, would violate that contract by conflating the two representations.

For the current inputs, `R_i = S_i`, so the nominal difference is explicit:

```text
R_out / outputRecordedFactor = Delta_0 / q.
```

That ratio is generally close to, but not exactly, one. Paper-scale tests must
retain it rather than silently replacing `q` with `Delta_0`.

This normalization is metadata-only. Calling OpenFHE `ModReduce` in Tensor2
would violate Definition 4.1 by consuming a modulus and changing polynomial
coefficients. The implementation must instead set the derived module degree
and recorded-factor contract on both complete three-component outputs after all
input and mutual validation. It must not alter any coefficient, ordered tower,
or level for this normalization.

The reasoning above proves the normalization only for the currently supported
first lifecycle: DCP-created, level-one, degree-two `FIXEDMANUAL` pairs from the
same bound context. It is not a generic rule for arbitrary pair lifecycles or
scaling techniques.

## Acceptance consequences

A conforming Tensor2 result must establish all of the following independently:

- exact three-component negacyclic high-high convolution;
- exact three-component high-low plus low-high cross term;
- a named nonzero low-low witness that is demonstrably omitted;
- unchanged ordered `Q_l` basis and level one;
- the adopted output degree three and recorded factor
  `S_1*S_2/Delta_0` on both members;
- `H_out` and `R_out` computed from input manifests, not result metadata;
- exact integer divisor retention without treating it as `Delta_0`;
- complete immutability of both input pairs;
- project-attributed rejection of invalid right-input and mutual metadata
  before any OpenFHE arithmetic.

The existing `PaperScaleDescriptor` and DCP API remain frozen. The Tensor2
result needs a separate minimal descriptor naming both output paper scales;
the OpenFHE recorded factor and degree remain separate result invariants. No
Tensor2 source patch is accepted until the prescribed compile red, the
fail-before-access scaffold, complete runtime reds, and independent oracle
evidence are preserved in order.
