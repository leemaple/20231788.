# Static preflight before first hosted probe

2026-09-05 Asia/Shanghai; base `cfe2fbc072bbd2154443ef71a641eabe449f34f5`.
Codex authored only the bounded upstream discovery probe/build plumbing while
the three nontrivial implementation/code-drafting tasks remained with Pro.
Root source review and two independent bounded Codex audits completed with no
blocking P0/P1 in this discovery scope. This is **not** a Pro or ZCode review,
nor a production acceptance decision.

Root fully read the 278-line probe and relevant pinned official constructor,
precompute, root-selection and context/getter definitions. A namespaced macro
mistake (`lbcrypto::NATIVEINT`) in the unsent draft was corrected before this
stable snapshot; `BASE_NUM_LEVELS_TO_DROP` was independently confirmed to be
an enum in exact-pin `constants-defs.h:127-129`. Direct public context
construction avoids retaining all 19 probe contexts in the factory cache.

Root's bounded Python source-only check confirmed all 12 pre-recorded modulus
and root literals occur in the probe; all 57 normalized `add_test` bodies
(NAME, COMMAND, order) are identical to base. SHA-256 of newline-joined
whitespace-collapsed complete `add_test` bodies:
`f8d4d906f16aa2352b09062f6bd524aa2a19449bcd8d31017d9ec12e9796a3ee`.
This checksum normalization includes NAME/COMMAND keywords; other review
normalizations are not interchangeable hashes.

`git diff --exit-code cfe2fbc -- include src tests` and `git diff --check`
passed. A bounded lexical check found no KeyGen/Encrypt/Decrypt/EvalMult/Mult2
or ReleaseAllContexts call and no production project header include. Lexical
absence is not a formal semantic proof; the whole-source review confirms the
narrow operation scope. The probe is not a new CTest and not linked to the
project evaluator. Both existing platform suites and five API builds remain
before its explicit target/run step. The branch is not a push trigger, so an
explicit workflow_dispatch at the pushed SHA is required.

Stable probe: 10,553 bytes, SHA-256
`75182935373c02656c43107a5134db61a5890d10bb6107a0dc9b8e48fb3193b7`.
Gitleaks 8.30.1 `gitleaks dir probes --no-banner --redact` scanned the prior
draft clean; the exact committed diff will also be scanned before push.
No ZIP or external-agent upload is needed for this public-repository CI step.

Limitations: RootOfUnity is an official call independent of the context, not
an independently implemented mathematical oracle. The earlier external
integer/root witnesses remain separately qualified. Precompute initializes
CRT/NTT tables, while no key/ciphertext or encryption/evaluation is created.
Direct context construction is probe-only, not evidence for future factory,
key generation, actual h=128 support, security or evaluator integration.
Compilation/runtime and all new hosted results remain **NOT RUN** at preflight.
