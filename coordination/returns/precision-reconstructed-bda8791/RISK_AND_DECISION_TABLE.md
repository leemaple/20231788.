# Risk and decision table

> **Artifact status: RECONSTRUCTED.** The exact prior code/patch bytes were absent from the defective return and current workspace. This file accompanies newly drafted replacement bytes based on the exact supplied baseline and frozen contract. No byte-identity claim to the missing prior candidate is made.

| Item | Current evidence | Risk if wrong | Smallest falsifying check / decision | User-visible contract effect |
|---|---|---|---|---|
| Red/green continuity | Mechanically reapplied; complete contract/CMake/header hashes identical; green changes one fixture path | False TDD claim | Run bundled verifier from exact baseline | None to production; determines evidence validity |
| Public DCRT fixture compiles on pristine 1.5.0 | Source-backed proposal; no OpenFHE build here | Candidate stops at compile | Hosted warning-as-error builds on GCC and MinGW64 | None; fixture stays test-only |
| Encrypt consumes injected DCRT element | Pinned `CryptoContext::Encrypt` source does so | Stale cache might accidentally become relevant | Focused green plus source-level stack inspection if it fails | A future shipping codec still needs explicit cache semantics |
| Inverse transform geometry/order | Port tied to full M=128 pinned transform | Wrong slots can be injected | Direct evaluator plus hard-coded constant/X^32/X^2 witnesses | No production API change; invalidates test fixture only |
| Direct oracle ordering/sign | Explicit roots and independent monomial table | Matching wrong expected values | Witness failure before crypto | Invalidates precision evidence, not DCP/RCB implementation |
| `2^-80` gate | Frozen before hosted result; deterministic rounding bound ≤`2^-96` | Encryption noise or implementation loss may exceed it | Four fresh-key runs per host; do not relax after result | Determines whether this diagnostic precision boundary is met |
| 128-bit non-wrap headroom | Actual recovered coefficients checked each trial | Apparent slot accuracy could be modular aliasing | Retain assertion and per-trial modulus/coefficient bit counts | Separates transport accuracy from invalid wrap cases |
| Homogeneous p50/50 parameters | Explicit diagnostic context | Could be misreported as paper Table 3 | Keep labels and report actual q_div | No Table 3 or repeated-lifecycle claim |
| Standard binary64 negative control | Exact source values differ; binary64 values and standard encodings coincide | Could be mistaken for an upstream bug | Keep wording and control separate from success assertions | Standard API behavior remains unchanged and valid |
| Stale packed cache | Known after test-owned DCRT replacement | Accidental getter/serialization could return placeholder zeros | Static grep and no getter/decode use; keep fixture under `tests/` | Blocks promoting fixture to public codec |
| REAL decrypt masking | Not changed or invoked | Disabling it would weaken production safeguards | No code change; use secret/CRT oracle only in tests | Production security behavior preserved |
| Functional-suite provenance | Exact bda8791 context retained a 42/44 BV history; the user separately reports a later combined 53/53 on both hosts; neither is produced here | False precision or integration claim | Codex runs the transplanted tree and binds its exact source/logs | Functional green remains separate from this precision gate |
| First Mult2 / repeated precision | Not tested in this slice | DCP→RCB result may be overgeneralized | Separate pre-frozen first-Mult2 gate, then refresh/lifecycle gate | Final Double-CKKS destination remains pending |
| Production >53-bit I/O | No shipping API selected | Test evidence may not be usable by callers | User/Codex decision on exact type, validation, cache and serialization | Material future public contract decision |

## Decisions made in this package

- Keep one existing public behavior: DCP followed by RCB.
- Put all new construction and observation machinery under tests.
- Use exact decimal/power-of-two expected values and direct canonical evaluation.
- Freeze `2^-80`, four fresh-key trials, state/basis facts and non-wrap margin in
  the red patch.
- Change only fixture implementation in green.

## Decisions deliberately deferred

- a production multiprecision plaintext/input API;
- a production high-precision output representation;
- ordered 40/60 parameter generation;
- refresh and repeated multiplication;
- BV certificate correction;
- any security or performance claim.
