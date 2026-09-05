# New GREEN brief — independent specification audit

**No blocking findings.** The new TASK authorizes the deferred diagnostic implementation after the accepted dual-host API/link RED, preserves original E80 and conditional assurance, and requires the complete observer/evidence path before labeling the draft complete. Its explicit draft-only/local-execution limitations and later exact-SHA hosted sequence are consistent with adopted ENDPOINT_SPEC v1-r1 and TEST_PLAN.

One nonblocking correction before sending:

- **GREEN-BRIEF-01 (P3): parenthesize the slot-map remainder.** `TASK.md:64` writes `k=(5^s mod 65536-1)/2`, which leaves the subtraction's relation to the modulus visually ambiguous. Use `e_s=powmod(5,s,65536); k=(e_s-1)/2`, exactly as adopted `coordination/fs-residual-endpoint-red-return-01/pro/ENDPOINT_SPEC.md:80–81`. The existing “permutation of even bins” qualifier and full specification resolve the intended meaning; this requires no mathematical or acceptance change.

Audit boundary: independently reread the full workflow and engineering/external/model-routing references, then read this new 97-line TASK, current RED ACCEPTANCE and root adoption. The adopted specification/test plan were confirmed byte-identical to the complete versions read in the earlier independent pass. No old audit/checker or mathematical review was repeated. `git diff 2fe655d4 29e12670 -- src include tests CMakeLists.txt .github/workflows/dcp-rcb.yml` was empty; the current engineering selection contains 39 paths. Current branch/HEAD match TASK. The actual accepted RED documents confirm seven missing helpers, old60/API success on both hosts, and no observer/chain execution.

The packet builder/archive itself was outside this brief audit. No build, FHE/transform/codec, browser, external send, source edit, Git mutation or checker execution occurred. Requested Astra/high remains an unattested selector, not an independent-provider claim.
