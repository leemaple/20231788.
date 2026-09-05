# Independent Codex review — original repeated candidate 01

Candidate ZIP SHA256 `77d32a3d28b528722efa59633feb7225cb813e68092023fbd462f0d4d318fec5`,
source base `80d771c52df10bce1c60992b5e0edb4e64f145ca`.
Comparison: the two ordered original patches and nine complete changed/new
files versus the exact engineering base; later source-equivalent documentation
HEAD `36ad9d1` was independently confirmed. The candidate was retained in Git
at `0ab6f89`; the integrated RED is `7399db5`, not GREEN.

Reviews ran independently, with the complete current TASK and project workflow
as specification/standards. Reviewers did no compiler, crypto, CI or browser
work. Root reconciles evidence rather than treating model statements as facts.

## Standards

Reviewer: state_audit_0905. No hard/P0/P1 finding was established within the
supported sequential caller contract.

Two non-blocking judgement calls remain:

- Receipt constructor/append use a long cluster of positional primitive, enum
  and scale arguments (repeated_mult2.h:38–50; repeated_mult2.cpp:184–188,
  210–234). A private named-field phase state could reduce maintenance errors.
  No actual incorrect argument or supported-input counterexample was found.
- The test AllKeyRows snapshot visits the whole process-wide EvalMult cache
  (semantic_oracle.h:303–317, test:194,261). This is stronger but more coupled
  than checking only two owned tags and the sentinel; the test's current
  separate-process execution does not expose an observed failure.

Disposition: neither speculative refactor is added before GREEN. Codex owns
reconsideration only if an actual later slice/test needs it, under KISS/YAGNI.

The reviewer initially proposed high-priority complaints about clearing a tag
after external replacement and exposing mutable upstream context handles.
After comparing the actual TASK with DESIGN's explicit no-concurrent/adversarial
handle-mutation boundary, the reviewer withdrew both P1 classifications: no
counterexample obeying the supported caller contract was established. Moving
the plan getter alone would not create transitive immutability because the
public OpenFHE key/ciphertext also exposes that context. The retained conclusion
is therefore not an assertion of concurrent/adversarial safety. A future need
for that stronger boundary requires its own test-driven contract.

Static positives: production secret use is confined to setup; no evaluator
private-key member; no production catch; the test catches only the specified
invalid_argument rejection; no shared_ptr receipt cycle; new wrappers at
re-entry with value snapshots; legacy guards and nullptr overload preserved.

## Spec

Reviewer: next_path_audit. Final independent source review found no confirmed
P1/P2 or pre-RED oracle/fixture blocker: frozen dyadic literals are checked before crypto; the
output oracle uses independent negacyclic secret polynomial arithmetic, CRT by
actual prime/root identity, dH+L and Horner reconstruction; all 16 slots and
deltas use 2^-80; the actual Z/W are two direct public Mult2 results and their
evaluator scope is secret-free.

Specific anchors in complete/project/: test:190–203 performs the actual Z/W;
repeated_mult2.cpp:261–330 confines setup secrets, with exact `(phi,modulus,root)`
projection at 303–326; 180–235 derives positive cpp_int rational receipts, checked
independently by test:219–234; re-entry at 370–397 creates wrappers without
arithmetic/client operations, checked by test:174–188; oracle.h:81–170 and
test:265–313 enforce exact vectors, independent reconstruction, every slot,
delta and four fresh trials. Profile/P/QP/alpha-one/input/key/context checks
were reviewed. No requirement implementation defect was confirmed.

This finding permits observing the genuine missing-API RED; it does not
accept the GREEN implementation before hosted compile/runtime evidence.

## Independent integration axis

Reviewer: repeated_return_gate_plan. Original patch-chain/preimage identities,
57+1 binding order, host-specific warning flags, exact branch trigger and
unchanged test/oracle bytes through GREEN passed static review. GREEN changes
CMake only to connect its implementation translation unit.

The early default-build RED obstruction was confirmed and fixed with the
CI-only integrator delta recorded in INTEGRATED_RED_GATE_01.md. The reviewer
then independently checked the real five-path RED working tree, matching
test hashes, exact two-host ordering, paths, absence of failure swallowing and
continued applicability of original GREEN. Integration preflight PASS.

## Evidence boundary

No dynamic GREEN, warning-clean GREEN build, focused 1/1, full 58/58 or full
paper reproduction is established by these reviews. Hosted RED is running.
External Pro authored/self-reviewed the candidate; that is not an independent
second reviewer. A current-boundary ZCode/fallback final review remains to be
allocated after hosted evidence is stable. The two withdrawn, source-resolved
P1 hypotheses do not by themselves warrant an unresolved-dispute Fable call.
