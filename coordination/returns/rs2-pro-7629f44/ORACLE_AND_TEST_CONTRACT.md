# RS2 oracle and vertical-test contract

## 1. Registered tests

| Patch | CTest name | Executable selector |
|---|---|---|
| R1 | `rs2_wrong_lifecycle_immutability` | `rs2_test wrong_lifecycle_immutability` |
| R2 | `rs2_valid_arithmetic_state_immutability` | `rs2_test valid_arithmetic_state_immutability` |

## 2. Intended red/green sequence

### R1 red

The packet baseline RS2 scaffold throws `std::logic_error` before validation.
The R1 test requires exact `std::invalid_argument` text:

```text
DoubleCKKS: RS2 requires ReadyForRS2 input
```

Therefore the derived expected test-level red diagnostic is:

```text
RS2 test failure: RS2 wrong-lifecycle guard threw the wrong exception type: DoubleCKKS: RS2 is not implemented
```

This is a static derivation from the source and test catch order; it is not an
observed execution log.

### G1 green

G1 runs existing `ValidatePair`, rejects any non-`ReadyForRS2` lifecycle with
the exact diagnostic above, and leaves the arithmetic scaffold untouched.

### R2 red

R2 calls RS2 on a real value produced by:

```text
fresh CKKS encryptions -> DCP -> Tensor2 -> official EvalMultKeyGen -> Relin2
```

On G1 it reaches the retained scaffold. The derived expected diagnostic is:

```text
RS2 unexpected exception: DoubleCKKS: RS2 is not implemented
```

Again, this is expected behavior from static control-flow inspection, not a run.

### G2 candidate green

G2 implements the two-rescale definition and terminal state. Actual green
status requires retained Linux and Windows execution evidence.

## 3. Independent centered-RS oracle

For ordered active moduli `q0,...,q_l`, let

```text
Q_l = product(q0,...,q_l)
q_l = final active odd prime
```

For every coefficient of every RLWE component:

1. Reconstruct `x mod Q_l` by textbook CRT using
   `boost::multiprecision::cpp_int`.
2. Map it to the unique centered integer representative in
   `(-Q_l/2, Q_l/2]`.
3. Compute the centered residue `r = center(x mod q_l)` in
   `(-q_l/2, q_l/2]`.
4. Compute the exact integer quotient:

   ```text
   RS_q_l(x) = (x - r) / q_l
   ```

5. Convert that signed quotient independently to residues modulo each retained
   prime `q0,...,q_(l-1)`.

The test does this separately for:

```text
expectedHigh       = RS_q_l(high)
expectedRecombined = RS_q_l(q_div*high + low)
expectedLow        = expectedRecombined - q_div*expectedHigh
```

It then compares all retained residues for:

- both output pair members;
- both RLWE components;
- every retained tower;
- every ring coefficient.

It also independently checks:

```text
RCB_q_div(output) == expectedRecombined mod Q_(l-1)
```

The expected values do not call RS2, OpenFHE `Rescale`,
`DropLastElementAndScale`, or any private project helper.

## 4. Fixture provenance and witnesses

The test first executes the untouched genuine public pipeline. It then retains
that genuine `ReadyForRS2` pair and installs deterministic coefficient vectors
through a test-only `const_pointer_cast` of the read-only ciphertext accessor.
It does not construct `CiphertextPair`, call a private constructor, or call a
private production helper. This invasive test-only operation should remain
confined to the test file and is separately listed as an integration-review
risk.

Boundary values include:

- `0`, `+1`, `-1`;
- `±(q_l-1)/2`;
- `±(q_l+1)/2` and one-step neighbors;
- values around the centered boundary of the composite active `Q_l`;
- both positive and negative variants in both RLWE components.

Because every modulus is odd, no even-modulus half-tie is invented.

Two explicit mutation witnesses are forced:

1. **`q_div` mistaken for `q_l`:** a coefficient at
   `(min(q_l,q_div)+1)/2` makes centered division by the two distinct primes
   differ.
2. **one-rescale shortcut:** a high coefficient `(q_l+1)/2` with recombined
   coefficient zero makes the correct low term
   `-q_div*RS(high)` differ from `RS(low)`.

The test also dynamically asserts that each witness actually distinguishes the
wrong mutation modulo the retained composite modulus.

## 5. State and immutability assertions

R2 checks all of the following:

- exact removal of only the final active `q_l`;
- output ordered basis is the context prefix `[q0,...,q_(l-1)]`, including
  declared aggregate basis and tower modulus/root/cyclotomic facts;
- level `1 -> 2`;
- noise-scale degree `3 -> 2`;
- exactly two RLWE components in each pair member;
- Evaluation format for aggregate polynomials and every tower;
- exact context pointer, key tag, slots, and `q_div` preservation;
- lifecycle `RefreshRequired`;
- exact recorded and two logical scale transitions;
- no output/input or high/low ciphertext pointer aliasing;
- unchanged input ciphertext values, identities, metadata-map identity,
  metadata-value identity/value, DCRT parameters, tower parameters, and exposed
  pair manifest;
- unchanged global evaluation-key map, rows, key identities/subtypes, key tags,
  contexts, and A/B DCRT entries.

## 6. Acceptance commands — not executed here

On the actual complete checkout and pinned official OpenFHE source, retain the
red output before each green, then run at least:

```bash
cmake --build <build> --parallel 2
cmake --build <build> --target rs2_api_contract_test --parallel 2
ctest --test-dir <build> --output-on-failure
```

Targeted checkpoints should additionally run:

```bash
ctest --test-dir <build> -R '^rs2_wrong_lifecycle_immutability$' --output-on-failure
ctest --test-dir <build> -R '^rs2_valid_arithmetic_state_immutability$' --output-on-failure
```

The R2 oracle must be mutation-probed by changing the dropped tower, substituting
`q_div` for `q_l`, replacing the two-rescale low formula with `RS(low)`, and
corrupting scale/state transitions; those mutations must fail before acceptance.
