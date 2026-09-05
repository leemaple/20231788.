# Table 3 reserved-P candidate 01

Status: **source-derived numerical candidate, not an adopted production
profile, frozen RED oracle, OpenFHE execution or paper result**. Prepared by
Codex plus independent Codex source/number review on 2026-09-05 while the
implementation Pro tasks run. The exact integers are in the adjacent JSON.
No active Pro task was given a new requirement or interrupted by this work.

## Paper and pinned-source boundary

The user paper, section 6.3 / Table 3 (supplied text lines 1562–1590), specifies
the t=2 example N=32768, h=128, dnum=11, Base50x2, Mult60x8, Div40, P60,
100-bit fresh scale, eight squarings and a 1000-execution error measurement.
The paper does not supply these exact primes/roots. They are a candidate
OpenFHE adapter selection, not the authors' recovered constants.

Only pristine OpenFHE commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`
was inspected in the clean detached sparse checkout prepared for this project.
Root rechecked that checkout was clean and independently read:

| Source | Lines / claim | SHA-256 |
| --- | --- | --- |
| `src/pke/lib/schemerns/rns-cryptoparameters.cpp` | 44–183: partition guard, maxBits/sizeP, downward P selection and Q collision skip | `4dbf003cce3cc02d92ebeb62398c2350c0f9e31d171b19a592e1e0fa6b9c9907` |
| `src/pke/lib/scheme/ckksrns/ckksrns-cryptoparameters.cpp` | 185–187: aux prime step is 2N | `038a8c285011a4626678022cb5f4e75217e0c2d931a261314afc4370448668fe` |
| `src/core/include/math/nbtheory-impl.h` | 183–228: canonical minimum root; 328–392: first/last/next/previous prime searches | `a83c35425d95dc42aa8c5043d54c4324dedc9bc8e588f70d384b74a91aa9b0b4` |

## Concrete choice and why it matters

For N=32768 the congruence step is 65536. The candidate expected value of
`FirstPrime(60,65536)` is **1152921504608747521**, whose actual integer bit
length is **61**. The API's argument is not a guarantee that the returned
prime is at most 60 bits. Using that value as a Q limb at alpha=1 would raise
maxBits to 61 and sizeP to ceil(61/60)=2, contrary to a single P60 target.

The stock P builder begins with that first-prime search, but **takes a previous
prime before accepting each P limb**. Reserve its expected first accepted
60-bit value **1152921504606584833** for P alone. Select the eight 60-bit Q
limbs strictly below it, as in the JSON. Use the two listed 50-bit Base limbs
and the listed 40-bit Div limb. All twelve numbers are distinct; each is 1
modulo 65536. Do not insert reserved P into any family Q.

With actual full-Q size L and numPartQ=L, alpha=ceil(L/L)=1 and each
partition's product is one 40/50/60-bit limb. Thus sizeP=1. For every such
subset family, the same reserved P remains absent from Q and the public
builder should select it again. This is a sufficient source-derived strategy,
not a requirement that the current low-N repeated slice share P across families.

The initial ordered Q is Base0, Base1, Mult0..Mult7, Div. After dropping Div,
the multiplication primes are consumed from the right: **m1=Mult7, ...,
m8=Mult0**. Storage order must not silently become consumption order.
The exact QP product has integer bit length 680; its 60-digit decimal log2 is
`679.999997929449936131542807480352491672055980842809865673443`.
This is consistent with nominal 680 bits, not equality to 2^680.

## Family boundary that remains explicit

Passing dnum=11 to a newly constructed full-Q family with only L<=10 limbs
fails the official partition guard. An 11-limb full context whose ciphertext
uses a shorter active prefix is different: precomputation still saw 11 limbs.
For the accepted repeated setup's shrinking **fresh** B_i=A_i||Div families,
use their actual full L as numPartQ to retain alpha=1; record that as an
OpenFHE family-local adapter decision, not a claim that Table 3 explicitly
specified every intermediate dnum. The candidate covers B0 size11 down to B8
size3, with A0 size10 down to A8 size2; actual returned setup must verify these.

No new context should be created just to enforce P equality. Do not change
the accepted repeated mechanism or expose secrets to the evaluator. h128,
family secret projection and exact scale ownership remain separate gates.

## Checks actually performed

Root used `/opt/homebrew/bin/openssl`, OpenSSL **3.6.3**, and Python **3.14.5**.
An independent reviewer used a separate integer implementation and LibreSSL;
its tool versions are not substituted for the root environment.

- OpenSSL `prime` on all twelve candidate moduli and the expected FirstPrime
  value returned `is prime`. This is strong probable-prime evidence, **not**
  a formal primality certificate.
- Bounded exact Python integer checks passed for all twelve: actual bit width,
  congruence, uniqueness, `r^65536 mod q=1`, `r^32768 mod q=q-1`, and r equals
  the minimum of its 32768 odd powers. This verifies exact arithmetic/order
  witnesses; equality to the actual upstream returned roots remains pending.
- Independent root exact-product/Decimal calculation confirmed QP bit length
  and log2 above. For L=1..11, numPartQ=L algebraically satisfies alpha=1 and
  the partition guard. That algebra does not claim CKKS runtime support for
  an arbitrary one-limb context.

No OpenFHE build, NTT, keygen, cryptographic test, benchmark or CI was run on
the Mac or elsewhere for this candidate. No old local implementation was read.

## Required next remote evidence

A narrowly scoped exact-pin profile-construction probe must independently
compare the actual public getters to these pre-recorded values: ordered Q,
P/QP modulus/root pairs, full versus active sizes, alpha and family-local
numPartQ, CKKS/profile/features and immutability. Include a separate negative
control with reserved P inserted into Q: the P builder should skip it. Never
read expected values from the same object being tested. Only after source and
remote evidence agree may a reviewed profile be frozen for the next genuine
RED/GREEN integration slice. This does not waive production I/O, h128, eight
semantic squarings, the 1000-run experiment, or applicable security evidence.
