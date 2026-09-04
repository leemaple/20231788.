# Codex disposition of the paper-parameter public API source audits

Observed2026-09-04 before any repeated-Mult2 implementation. Codex read the
complete h128, ordered-Q and raised-basis notes. These are constraints and
candidate public interfaces, not an adopted design or executed cryptography.

Codex independently re-fetched all53 distinct primary files in the two research
manifests using the GitHub contents API at pristine pin
 df495ba2e91739a6dc8f1de254fc5a41155ce504.
All1018652 raw source bytes match both reported SHA256/size and Git blob SHA1.
An initial direct raw-host Node fetch timed out without supplying evidence;
using the official GitHub contents API succeeded. No old/local OpenFHE read.
Verified source archive /private/tmp/repeated-mult2-pro-handoff.azsrhb/verified-paper-public-sources.zip,
219995bytes,SHA256 8cc569e227c959c94c9289227a27f741536fab30e351d16aade5f65db5e31784; it is a source handoff artifact,
not a build/install or a modified upstream checkout.

Main personally inspected exact numbered excerpts for10 ordered-Q critical
files: ordered ILDCRTParams constructor, CKKS constructor/precompute declaration,
RNS precompute/partition guard/P selection versus estimator clamp, registry
lookup and KeyGen, factory interning, actual HYBRID selection and KEYSWITCH
no-op, FIXEDMANUAL getter behavior, FLEXIBLEAUTO recurrence and standard Decode.
The actual guard rejects L<=10 with numPartQ11; the estimator's clamp is a
separate function. The40/60 flexible-ratio rejection is a source derivation,
not a runtime result. Actual-prime arithmetic and double scale labels remain
separate. Main's earlier h128 cross-check separately read7 primary source files
plus the common KeyGen/Multiparty/security excerpts; those results remain
source-supported only. Neither operation certifies h-aware security.

Disposition: use the notes as explicitly provisional constraints in a complete
Pro design/coding handoff. Public fresh registered context/table/key routes are
plausible enough to test; no claim of inevitability, impossibility, paper-
parameter equivalence or repeated-lifecycle correctness is adopted now.
Require same-secret matching per-family keys, distinct routing/tag semantics,
ordered Q/P consistency, scale/level state, immutable published objects, and
no decryption/re-encryption or secret use in evaluator operations. Preserve
initial/active decomposition semantics rather than silently changing dnum.

The current first-Mult2 source4790778 still has both actual55/55 passes and
16 actual precision samples; it is not contradicted by a constraint on the
unimplemented second step. Current first-Mult2 independent ZCode review is
live and isolated. No Mac build, crypto, security estimation or benchmark was
performed by this audit; no existing source/test/API behavior was changed.
