# Fable 5.1 decision task — Relin2 BV nonzero-digit A-count oracle

Recorded: 2026-09-02 Asia/Shanghai

Status: completed as an unusable-provider attempt. The CLI emitted
`claude-fable-5`, not a verifiable Fable 5.1 identity, and its response cited
files that were not present in the sanitized packet. Do not use its conclusions.
See
[`../reviews/fable51-relin2-bv-nonzero-digit-oracle-invalid-receipt.md`](../reviews/fable51-relin2-bv-nonzero-digit-oracle-invalid-receipt.md).

## Background and exact boundary

This is a clean-room OpenFHE 1.5.0 implementation of the `t=2`
double-precision multiplication method from paper 2023/1788. The implementation
branch is `agent/codex-relin2-01` at exact clean commit
`3411d65e752272a70d6dc147e8e7239014221196`, tree
`572925d4819a32eefb258af1ed37b79deb551cc0`. Existing accepted Relin2 work is
validation-only and deliberately ends with
`DoubleCKKS: Relin2 is not implemented` for an otherwise valid input.

The next isolated TDD boundary is only the expected A-vector decomposition
count for BV key switching when `digitSize > 0`. It must not implement the B
count, BV entry basis/format, ciphertext raising, relinearization, or paper
arithmetic.

## Authoritative contract

For bound complete-Q tower moduli `q_i`, the required BV key-vector length is:

```text
sum_i ((q_i.GetMSB() + digitSize - 1) / digitSize)
```

The proposed public fixture uses multiplicative depth 3, FIXEDMANUAL,
`scalingModSize=30`, `firstModSize=35`, ring dimension 32, BV, and
`digitSize=10`. It has four complete-Q towers. A valid generated key must first
reach the current Relin2 scaffold and preserve the complete Tensor and key
cache. The negative control then removes only the final A entry and must receive
an exact project-owned `std::invalid_argument`; B and all other state remain
unchanged.

## Official OpenFHE 1.5.0 source facts

All paths below are from official OpenFHE exact commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.

`src/pke/lib/keyswitch/keyswitch-bv.cpp` computes the size as follows:

```cpp
uint32_t nWindows = 0;
for (uint32_t i = 0; i < sizeSOld; ++i) {
    arrWindows[i]  = nWindows;
    double sOldMSB = sOld.GetElementAtIndex(i).GetModulus().GetMSB();
    nWindows += std::ceil(sOldMSB / digitSize);
}
av.resize(nWindows);
bv.resize(nWindows);
```

`src/pke/lib/scheme/ckksrns/ckksrns-parametergeneration.cpp` constructs the
FIXEDMANUAL Q chain with:

```cpp
NativeInteger q        = FirstPrime<NativeInteger>(dcrtBits, cyclOrder);
moduliQ[numPrimes - 1] = q;
// for i=numPrimes-2 down to 1, alternate PreviousPrime(q) and NextPrime(q)
// ...
moduliQ[0] = LastPrime<NativeInteger>(firstModSize, cyclOrder);
```

`src/core/include/math/nbtheory-impl.h` defines:

```cpp
IntType q(IntType(1) << nBits);
// FirstPrime returns the first allowed prime above q.
// LastPrime returns an allowed prime below q and requires GetMSB()==nBits.
```

For `numPrimes=4`, `dcrtBits=30`, and `firstModSize=35`, one reviewer therefore
derived the ordered tower-MSB manifest `{35,31,30,31}` and expected count
`4+4+3+4=15` for digit size 10. Another reviewer derived
`{35,30,30,30}` and expected 13. A third, abandoned draft considered digit size
8 and expected 17, but the authoritative fixture name in the existing task uses
digit size 10.

## Exact questions

1. From the quoted official algorithms, which digit-size-10 tower-MSB manifest
   and decomposition count are correct: `{35,31,30,31}` / 15 or
   `{35,30,30,30}` / 13? Explain each tower without assuming generated A/B
   lengths.
2. Should the TDD oracle both compute the integer ceiling sum from the bound
   context's actual complete-Q tower parameters and independently assert the
   fixed manifest/count, so a future generator drift stops rather than weakens
   the test?
3. Is the existing exact diagnostic
   `DoubleCKKS: Relin2 evaluation key BV A vector length mismatch` adequate for
   both zero and nonzero digit-size A mismatches, or is a distinct nonzero-digit
   diagnostic necessary for correctness?
4. Identify any source-backed reason the proposed positive-control-then-A-only
   `pop_back()` fixture could be a false green or hit an earlier accepted guard.

## Required response

Return a concise decision with:

- inspected facts separated from inference;
- the exact correct manifest and count;
- the recommended durable oracle and diagnostic;
- any blocking flaw in the fixture;
- no code edits and no claims of builds or tests.

Do not discuss or implement B-vector validation, basis/format validation,
Relin2 arithmetic, RS2, Mult2, performance, or unrelated redesign.
