# Independent semantic first pass

## Disposition

**Proceed to independent review and one first valid numerical observation per host using the test-only slice. No production change is justified by this source review alone.** I found no concrete candidate-specific production defect in the inspected factory, family/receipt transitions, client boundary or terminal adoption path. The eight-square numerical behavior remains unknown. This is a source-grounded disposition, not a green-CI endorsement or a proof of correctness.

Review input is exclusively the dispatched packet: engineering identity `2759fa90840946ef42957c7ba71ebea47e0e4995`, packaging identity `80f1c53044e846bf6f0cf558b48732b915b9c116`, and the verified manifest. No private/local repository, prior conversation, previous profile-author verdict, or remote branch state was used. Current `TASK.md` and `TASK_PREFLIGHT.md` govern; historical briefs do not override them. This first pass is separate from the previous profile author. The newly authored test's checks are self-review, **not** its required independent review.

Paths below are relative to the input packet. They identify inspected source, not independently executed behavior.

## Source findings

### Fixed profile and basis construction

`project/src/repeated_mult2.cpp:17–47,176–225,443–470` retains the original profile and uses a second fixed internal descriptor for the candidate. The candidate replaces only Base0/Base1/Div and their roots, uses base metadata exponent58, retains the eight middle primes and P, and constructs the eight families by deleting the second-last prime. There is no caller-configurable parameter profile. Family construction and validation check actual Q/P/QP identities and HYBRID tables rather than merely renaming a context.

The test binds to the frozen candidate literals in `project/tests/experimental_precision116_profile_seam.h:31–49`, independently matched to `requirements/candidate.json` and `requirements/STATIC_CERTIFICATE.json`. These are not obtained from a live plan. The three new-prime Proth form/witness checks and root-order checks passed as pure integer checks here; this is neither a ciphertext experiment nor a security estimate.

### Root secret and evaluation-key ownership

`project/src/repeated_mult2.cpp:351–377,380–398,443–463` invokes the fixed-Q key adapter once in family0, validates signed weight128, and projects each later family by a unique complete modulus/root/cyclotomic-order match. Each temporary projected private-key wrapper is local to key installation. Plan Data does not store the root or projected secrets; its destructor removes owned evaluation-key tags (`:242–254`). The row identity/coefficient seals are used by live validation.

The adapter samples the root and forms a public encryption key in `project/src/paper_h128_client_keypair.cpp:193–217`. The pinned sampler has internal sign-balancing rejection, not an accuracy-selected caller retry: see `references/official-full/src/core/include/math/ternaryuniformgenerator-impl.h:55–98,101–143`. Therefore “one sampler invocation” should not be misreported as a claim that the sampler uses no internal rejection or samples uniformly over every possible h128 sign assignment. The requested signed-h128 boundary is preserved; a security claim is not inferred.

The public test cannot inspect destroyed projected secrets. It checks ordered root subsets, family-local QP rows and actual root-secret consistency through independent sparse decryption of every returned pair. That is a bounded behavioral check, not a complete evaluation-key noise-equation or cryptographic proof. Pinned HYBRID key generation is inspected at `references/official-full/src/pke/lib/keyswitch/keyswitch-hybrid.cpp:51–129`.

### Scale and lifecycle semantics

`project/src/repeated_mult2.cpp:257–312` separates exact logical scales from recorded compatibility scales. Its receipts use Input/Reentry scale S, Tensor/Relinearized scale S²/d, and Rescaled scale S²/(d*m). Metadata58 yields recorded116 → recorded174 → recorded116. Reentry changes wrappers/context/tag/local level but not polynomial arithmetic or logical scale (`:514–542`). The terminal object validates the issuing plan and root-prefix wrapper (`:550–569`).

The returned round8 state is **not** family8 Reentry: it is family7 terminal Rescaled at local level2 with two Bases, followed by a root-context wrapper at absolute level9. The existing seam's nonterminal `CheckPair`/`CheckReceipt` cannot validate this state. The new test does not call those helpers.

### Arithmetic and client normalization

The inspected production path is DCP (`project/src/double_ckks.cpp:399–464`), Tensor2 (`:826–892`), Relin2 (`:894–1078`), RS2 (`:1080–1211`), Mult2 (`:1213–1226`) and terminal RCB (`:1246–1283`). Tensor's omitted low×low term is intentional in the paper method, not by itself a missing-term defect. RS2 separately rounds the high and recombined values and subtracts d times the rescaled high to obtain low; it must not be replaced by separately rescaling low. The supplied paper, printed pages7–8, Definitions4.1/4.3/4.5/4.7 and associated lemmas, explains this structure and its conditional lift assumptions.

Plan-bound high-precision I/O adopts the terminal issuing receipt's exact rational scale and rejects foreign issuing plans (`project/src/high_precision_client_io.cpp:443–503,659–676`). Encrypt uses the exact high-precision integer bridge (`:554–615`); Decrypt uses the official public-scheme `Poly*` route and its own high-precision transforms (`:678–731`), not binary64 slot decoding.

The independent test shares only the official inverse NTT for polynomial recovery. `references/official-full/src/core/include/lattice/hal/default/poly-impl.h:420–439` selects inverse NTT when switching an evaluation native tower to coefficients. The pinned `Poly*` decrypt route adds flooding only in NOISE_FLOODING_DECRYPT, not the required FIXED_NOISE_DECRYPT (`references/official-full/src/pke/lib/scheme/ckksrns/ckksrns-pke.cpp:71–95`); root-prefix secret handling is at `references/official-full/src/pke/lib/schemerns/rns-pke.cpp:199–224`. Thus exact independent endpoint CRT diagnostic agreement is an appropriate additional consistency check.

## What the supplied GREEN establishes—and does not

The supplied final status records identify RED run `34050734415`, source `70c37679f4760c6dc2bebc35bdfd741c238f315b`, and GREEN run `34051183115`, source `2759fa90840946ef42957c7ba71ebea47e0e4995`. RED's missing-factory compile errors are visible in `evidence/profile-seam/RED_LINUX_JOB.log:5643` and `RED_WINDOWS_JOB.log:5943`. They are not numerical RED observations.

GREEN's final regression checkpoints report 60 tests (`GREEN_LINUX_JOB.log:5622`, `GREEN_WINDOWS_JOB.log:5916`). The seam explicitly prints `squares=1` and `full_eight_square_E80=NOT_TESTED` (`GREEN_LINUX_JOB.log:5679–5685`, `GREEN_WINDOWS_JOB.log:5984–5990`); reported seam durations are 10.85 and 11.75 seconds. Its source `project/tests/experimental_precision116_profile_seam.h:283–375` performs no production Decrypt or one-square numeric comparison. Neither first-square E80 nor full-eight E80 follows from those successes.

The original profile's recorded FAIL in run `34039088536` is retained on the current task's authority; it was not rerun or re-adjudicated here. No evidence record is rewritten.

## Remaining limits and engineering decision

The candidate is a correctness experiment, not exact Table3 parameter replication: fresh scale, Bases/Div and QP exposure changed. The supplied paper's section6.3/Table3 is on printed page13. QP is about712 rather than680 bits; no inherited 128-bit-security assertion is made and no estimator is required or run.

At intermediate rounds, ten anchors cannot prove all-slot precision/nonwrap; neither endpoint agreement nor positive centered headroom establishes the intended Tensor/Relin lifts. Transient Tensor/Relin ciphertexts are not exposed by the stipulated public Mult2 calls. Their recorded174 behavior is source/runtime-invariant reviewed, not directly observed by this new harness. Upstream mutable context handles and normalization receipts are not cryptographic operand-authentication or hostile-concurrency guarantees.

**Next engineering action:** independently review the draft, then compile and execute the exact frozen test once on each hosted platform before authorizing a production fix. Preserve every attempt. A first valid observation may PASS. A compile/setup/timeout/oracle/harness failure is not a numerical RED; repair only a demonstrated harness issue separately and rerun the corrected frozen test before making a production decision.
