# Precursor review: accepted DCP→RCB high-precision tracer

## Verdict

**PASS, with a deliberately bounded claim.**

- **P0: 0**
- **P1: 0**
- **P2: 4 claim-boundary risks, none blocking this next additive slice**

The accepted precursor is adequate as a test-owned, first-stage high-precision transport fixture. It is not evidence for first `Mult2`, repeated multiplication, production lossless I/O, Table 3, security, performance, or a universal no-wrap theorem.

## Identity and continuity audited

The supplied outer archive matched its declared 1,163,390-byte identity and SHA-256 `e49e3fcb897ea7b9fa0cf31bc376a9289e0bd4fe8f4a1ff70a35622bd5fe0461`. Its 61 manifest payloads had exact path, size, and hash closure. The nested precursor delivery matched SHA-256 `601c7bfbf195d383146ad63b508797322ed22cab12590fc470a241753da6f906`; all 33 entries covered by its internal manifest also closed exactly. The 40 selected project files matched the supplied source provenance for `c9ee28d0370eeee1ec7a1965402ed0b5e91f425e`. See `evidence/INPUT_INTEGRITY.txt`.

The actual accepted runtime contract freezes N=64, batch=16, depth=7, p=50, first=55, HYBRID, FIXEDMANUAL, COMPLEX, a degree-two `2^100` input scale, four fresh-key trials, and an absolute `2^-80` gate (`project/coordination/handoffs/precision-runtime-contract-e38764a.json:L1-L44`). Its frozen current hashes agree with the supplied files.

### Red/green provenance

1. The earlier Windows build at `fe35a099…` did not reach CTest. It failed at four invalid `lbcrypto::Format` qualifications (`project/artifacts/tdd/precision-dcp-rcb/compile-failure-windows.txt:L53-L70`). This is a compilation event, not the intended runtime red.
2. The namespace-corrected runtime red at `e38764ab…` reached both hosts. Each ran 54 tests and failed only `precision_dcp_rcb_high_precision_contract`; the measured slot-delta errors were approximately `1.443717080128684e-15` on Linux and `1.443717080129157e-15` on Windows (`red-linux.txt:L108-L122,L540-L545`; `red-windows.txt:L132-L139,L562-L567`). The lossy binary64 fixture had already erased the sub-ULP source difference, so this red does **not** indict pristine encoding, DCP, or RCB.
3. The accepted fixture-only green changed `tests/precision_dcp_rcb_fixture.cpp`. Linux and Windows each passed 54/54. The four Linux trial records are in `green-linux.txt:L109-L119`; the four Windows records are in `green-windows.txt:L132-L142`. Full-suite totals are `green-linux.txt:L541-L543` and `green-windows.txt:L564-L566`.
4. Across the eight accepted trials, the worst recorded delta error was `5.9416098364710929682297021517222122255998e-28`; the worst all-slot error was `5.0925606891564857369051272102149462810231e-28`. These are observed samples, not a distributional or universal bound.
5. The archived original Pro source differs from the accepted current contract in exactly four `lbcrypto::Format::…`→`Format::…` replacements and from the accepted fixture in exactly three such replacements. Those corrections are mechanically consistent with the retained Windows compiler diagnostic. The current source, not the raw precursor patch bytes, is authoritative for this continuation.

## Source audit

### Test-only injection and stale-cache boundary

The accepted fixture performs explicit positive/negative rounding branches (`project/tests/precision_dcp_rcb_fixture.cpp:L58-L66`), computes the inverse special transform and rounds once at `2^100` (`L238-L248`), builds each DCRT tower through public constructors and `SetValues` (`L178-L200`), then replaces the public DCRT element (`L258-L263`). OpenFHE exposes mutable DCRT plaintext access (`official-openfhe/plaintext.h:L460-L471`), and `Encrypt` consumes that element while copying metadata (`official-openfhe/cryptocontext.h:L1250-L1263`).

The private `complex<double>` cache still describes the zero placeholder. The fixture correctly warns against packed-value getters, production `Decrypt`, serialization, or treating it as a shipping codec (`project/tests/precision_dcp_rcb_fixture.cpp:L258-L263`). The current accepted contract does not use those paths.

### Oracle independence and canonical order

The precursor does not use production RCB as its expected coefficient path. It independently:

- evaluates RLWE components as `c0+c1·s+…` using schoolbook negacyclic products per tower, then reconstructs centered coefficients with `cpp_int` CRT (`project/tests/precision_dcp_rcb_contract_test.cpp:L130-L140,L167-L248`);
- independently forms centered `q_div·high+low` (`L250-L279`);
- evaluates the recovered polynomial directly rather than invoking a matching forward special FFT (`L324-L363`);
- checks a constant, `X^32`, and a hard-coded ordered `X^2` table (`L365-L413`).

The ordering is tied to OpenFHE’s powers-of-five rotation group and root table (`official-openfhe/dftransform.cpp:L50-L69`) and its inverse special-transform ordering (`official-openfhe/dftransform.cpp:L209-L238`). These witnesses would expose a wrong normalization, phase sign, conjugation, slot permutation, or bit-reversal convention rather than allowing a paired inverse/forward error to cancel.

### Accepted public behavior

DCP removes the final divisor tower and constructs `high` and `low` such that the prefix minus `q_div·high` is the low part (`project/src/double_ckks.cpp:L376-L415`), then emits `ReadyForFirstMult` (`L418-L437`). This is the only production behavior certified by the accepted precision precursor. The next slice remains additive and does not modify this accepted test or fixture.

## P2 findings and observable dispositions

| ID | Finding | Current disposition criterion |
|---|---|---|
| P2-1 | The injected plaintext’s packed cache is stale. | Keep the adapter test-owned. A precision test passes this review only if source guards show no `GetCKKSPackedValue`, production `Decrypt`, serialization, or shipping-code use. A production codec remains a separate API decision. |
| P2-2 | “258-bit headroom” is the bit gap for observed centered representatives, not proof that no earlier modular wrap occurred and not a universal key-switch/noise theorem. | Continue to label it diagnostic. Universal non-wrap needs a stage-by-stage integer-history bound or an independently proven theorem gate. |
| P2-3 | N=64, homogeneous p50/50, and `HEStd_NotSet` do not reproduce the paper’s ordered Div40/Mult60, N=2^15, h=128 security regime. | Do not attach security or Table-3 labels until the exact ordered parameters, security estimate, and experiment protocol are separately frozen and run. |
| P2-4 | Four fresh, unseeded keys per host give eight observed samples, not deterministic replay or a probabilistic upper bound. | Retain exact logs and per-trial outputs. Any statistical claim needs a separately registered repeated experiment and stated sampling model. |

## Audit conclusion

The accepted precursor’s source, actual runtime red/green continuity, canonical oracle, and caveats are internally consistent. It is a sound dependency for an additive first-`Mult2` test-only tracer. No immutable accepted source is changed by the candidate in this delivery.
