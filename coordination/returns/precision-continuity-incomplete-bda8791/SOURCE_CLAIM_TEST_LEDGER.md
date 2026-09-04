# Source / claim / test ledger

| ID | Status | Claim | Pinned source | Candidate test evidence | What remains |
|---|---|---|---|---|---|
| C1 | OBSERVED | CKKS slots are canonical evaluations at selected primitive roots; encoding applies inverse canonical embedding, scale and rounding | `PAPER-2023-1788.txt:209-219` | Direct evaluator uses explicit roots and monomial witnesses | Hosted candidate runtime |
| C2 | OBSERVED | Pinned special-transform ordering is generated from powers of 5 modulo the cyclotomic order | `official-openfhe/dftransform.cpp:50-60` | Hard-coded N=64 exponent table and `X^2` decimal witness | None for arithmetic; hosted transport remains |
| C3 | OBSERVED | Native64 Encode performs binary64 inverse transform and rounds at the depth-one scale before the later degree lift | `official-openfhe/ckkspackedencoding.cpp:115-133, 191-309, 331-332` | Negative control plus incomplete red fixture | Hosted red not yet observed |
| C4 | OBSERVED | Mutable public plaintext element access exists | `official-openfhe/plaintext.h:258-269` | Green fixture assigns a public `DCRTPoly` element | Must compile on both pinned hosts |
| C5 | OBSERVED | Packed-value getter is a separate cache/value path | `official-openfhe/plaintext.h:366-374` | Static verifier requires `GetCKKSPackedValue` absent | Production cache synchronization remains undecided |
| C6 | OBSERVED | Public Encrypt overload consumes `plaintext->GetElement<Element>()` | `official-openfhe/cryptocontext.h:1248-1262` | Injected DCRT is the object passed to Encrypt | Hosted runtime confirmation |
| C7 | OBSERVED | DCP and RCB are existing public project seams | `project/include/openfhe_2023_1788/double_ckks.h:143-149` | Test calls `module.DCP(input)` and `module.RCB(pair)` | None structurally |
| C8 | OBSERVED | DCP removes the fixed final divisor tower and returns the retained prefix pair | `project/src/double_ckks.cpp:406-425` plus constructor/validation around `240-278, 343-403` | State/basis checks compare input, pair and output tower order | Hosted test |
| C9 | OBSERVED | RCB computes `q_div·high + low` and retains recorded metadata | `project/src/double_ckks.cpp:1001-1011`; paper decomposition/recombination `PAPER-2023-1788.txt:437-477` | Actual under-test output is independently decrypted and evaluated | Hosted test |
| C10 | INFERRED | Existing public DCRT interfaces are sufficient for a test-only full-scale plaintext fixture without upstream/private access | C4-C6 and DCRT interfaces in supplied official references | Green fixture uses only public `DCRTPoly`, `NativeVector`, `SetValues`, mutable `GetElement` | Compilation/runtime on GCC and MinGW64 |
| C11 | PROPOSED | Fresh `2^100` construction and existing public DCP→RCB preserve the selected complex vector within `2^-80` | Mathematical rounding margin plus C7-C10 | Frozen focused CTest, four fresh keys per host | **NOT EXECUTED** |
| C12 | NOT CLAIMED | Homogeneous p50/50 reproduces paper Table 3 | Paper Table 3 region `PAPER-2023-1788.txt:1562-1590` instead uses about 100-bit scale, 40-bit Div and 60-bit Mult primes, N=2^15 | Candidate explicitly labels p50/50 diagnostic | Ordered parameter/lifecycle work outside scope |
| C13 | NOT CLAIMED | DCP→RCB green establishes Mult2 or repeated precision | No source supports that implication | Test ends immediately after RCB | Separate first-Mult2, refresh and repeated gates |
| C14 | NOT CLAIMED | `HEStd_NotSet` establishes security | Context deliberately disables standard security selection | State label only | Security parameterization outside scope |
| C15 | INHERITED/PENDING | Two BV empirical certificate tests fail independently | Supplied exact CI logs and follow-up brief | Candidate does not touch those paths | Separate BV task; full suite cannot be called green |

## Claim discipline

A successful hosted focused test supports only this bounded statement:

> Under the frozen homogeneous p50/50 diagnostic context and test-only
> multiprecision plaintext/oracle adapters, the existing public DCP→RCB path
> preserved the selected 16 complex slots, including `(2^-70,2^-73)` sub-binary64
> differences, within the predeclared `2^-80` gate for four fresh-key trials on
> the named host.

It does not support “100-bit slots,” “106-bit precision,” “Table 3 reproduced,”
“Mult2 high precision,” “refresh works,” “secure parameters,” or a performance
claim.

## Source identities

Exact SHA-256 values for every load-bearing source above are retained in
`evidence/PINNED_SOURCE_HASHES.txt`; archive-wide identities are in
`evidence/INPUT_INTEGRITY.txt`.
