# Pro fixed-key BV return: source-gap reconciliation

Observed 2026-09-04 Asia/Shanghai. Owner Codex.
Completed conversation: Diagnose BV Relin2 Failure
https://chatgpt.com/c/6a9a5824-3e5c-83ec-83ed-a73acf3dc062
Stop was absent on the observed completed response. No stop, refresh, reminder
or duplicate live-task submission occurred.

## Independently verified delivery

 /Users/lifeng/Downloads/bv-fixed-key-bound-independent-follow-up.zip
 58604 bytes
 SHA256 bf15886a0cabc2347469f74ac4c356b3f0afd500fe7b79cc6fdf0d4d9c4f5ea0

Codex inspected all14 ZIP members: nonempty regular files, safe relative paths,
no duplicates/symlinks/encryption/cache/binaries. CRC passed.
PACKAGE-MANIFEST.json covers exactly12 payloads with matching sizes/hashes;
SHA256SUMS covers exactly13 files excluding itself, all hashes pass.
Gitleaks8.30.1 scanned177077 extracted bytes, zero findings.
The full14-file return is retained byte-exact under
coordination/returns/bv-fixed-key-pro-c7cd790/.

Read the complete REVIEW, BOUND-DERIVATION, execution ledger and complete
363-insertion/3-deletion proposed patch. No delivered script was executed.
The candidate's exact baseline test matches current clean-room source:
b27c15ceb2ab886077701187cd9700d89aad9bf8feb3904cd0dfccd1c78e1b26.
git apply --check -p2 succeeds on the current combined tree without modifying it.
Candidate full file SHA256:
92a2f03c0301ba0e16d6d52f72e2fef129f069bbdee2b27e6a25b7c49030538d.
Candidate patch SHA256:
c70a5963909bb1fc1c4f5bdabbd5baba7d013dab7f5cdd9b7ebaf325a2545871.
Candidate is NOT integrated or compiled by this receipt.

## Pro verdict and Codex disposition

Pro: AMEND, P0=0, P1=1, P2=2, no demonstrated production defect.
The explicit P1 is the missing all-residue centered-lift implementation below
the supplied CRTDecompose call site. This is a gap in the evidence packet,
not evidence that actual OpenFHE uses an incorrect lift.

The independent Codex note already committed before this response completed,
coordination/CODEX_BV_CENTERED_LIFT_SOURCE_NOTE.md, supplies four additional
exact pristine files at df495ba2e91739a6dc8f1de254fc5a41155ce504,
their Git blob/SHA256 provenance and a source-by-source all-residue derivation:
Poly::SwitchModulus -> NativeVector::SwitchModulus -> native modular arithmetic.
The nonnegative/negative-half branches yield the same centered source integer
modulo every target; zero remains zero. This is not LazySwitchModulus.
Codex considers the named source premise DERIVED under the stated valid-native
odd-modulus domain, pending independent review of these newly supplied files.
Pro had not seen them in its prior packet, so its P1 must not be called
independently closed merely because Codex has the missing source.

The remaining conclusions agree:
- projected fixed-key row residual r_i already includes -ns*e_i; no extra ns;
- exact digit radius floor(q_i/2), with ceil only a looser sufficient choice;
- full raised-high allocates eight digits but final zero tower contributes zero;
  both projected high and prefix-low use the seven active residual rows;
- direct pair bound B_H+B_L, no derived extra Relin2+h;
- RS2 independently retains (h+1)/2 and normalization1/(q_div*q_l);
- fixed-key ciphertext-uniform modular bound is not an unconditional raw
  Gaussian-noise, paper-wide, or precision theorem;
- modular triangle acceptance alone is not a no-wrap proof.

Codex derived sum_i floor(q_i/2)*||r_i||_1. Pro uses the valid coarser
N*sum_i floor(q_i/2)*||r_i||_infinity. These agree via ||r_i||_1<=N||r_i||_infinity;
they are not conflicting formulas or grounds to remove N from the latter.
Statements about unbounded Gaussian support are mathematical distribution
claims, not an inspected implementation-level tail cutoff in the sampler.
A numerical sampler/tail/security claim remains outside current evidence.

## Proposed probe review and next action

The proposed test-only patch calculates the key bound before plaintext,
encryption or path-error observation, recovers residuals with independent
integer/CRT arithmetic, probes centered digit boundaries and the zero high
digit, adds per-path/pair/conservative coefficient inequalities and explicit
pre-RS/final half-modulus conditions. It preserves all existing functional
assertions, vectors, tolerance, registered cases and conditional/unproved labels.
Its boundary probe is a falsifier, not a substitute for the source proof.
The new tests do not by themselves establish every required integer-lift premise
for arbitrary future inputs. Actual hosted compilation/runtime is still missing.

Next: complete-context follow-up containing the FULL prior packet and return,
the four pristine source files/provenance, the Codex derivation and toy evidence,
for independent closure or refutation of the named P1 and precise probe plan.
Keep the candidate labels conditional until the intended evidence is established.
A future additive probe is not fabricated missing-feature red/green.
First high-precision DCP/RCB RED is independently running at fe35a09940e3f3f5388aa735e93ef1a8c5a5deb4,
run33862006375; no dependency on this review is introduced.
The full precision, repeated lifecycle and paper-parameter goals remain active.
Fable5.1's prior403-before-inference remains no usable review, not a blocker.

Pre-commit: all14 archival files rechecked byte-identical; active baseline test
hash matches; staged Gitleaks scanned190979 bytes with zero findings. Six
context-only blank lines in the one original candidate .patch trigger the
unfiltered whitespace check; only that exact archival patch path was excluded.
All other staged files pass the whitespace check. No active source/build/test
file changed, so this evidence-only commit uses [skip ci].
