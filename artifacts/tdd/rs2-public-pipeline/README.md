# Untouched public-pipeline coverage

This is a coverage extension to an already implemented RS2, not a newly claimed
red-green implementation cycle. Core RS2 red/green remains recorded separately.

The added public-seam case uses genuine nonzero complex encryptions, DCP,
Tensor2 and Relin2 under both HYBRID and BV (digit size zero). It never replaces
the ciphertext coefficients. After Relin2 it removes only its own nonempty-tag
evaluation-key entry, then checks RS2 and RCB against the independent cpp_int
CRT/centered-quotient oracle for every component, tower and coefficient. Existing
metadata/state/input snapshot checks apply; the shallow parameter/key snapshot
limitations documented by the reviewers are not claimed resolved here.

The deterministic controlled case still must distinguish the RS(low) shortcut.
The untouched case does not require a random encryption to supply that witness;
all of its exact quotient/correction/recombination assertions remain mandatory.

These ring-32, depth-3, p=30, first-modulus-35 fixtures test the RS2 ciphertext
algebra and state only. They do not assert non-wrapping full plaintext products,
decoded multiplication accuracy, paper error bounds, production security, or
53/106-bit precision. No Mac build or cryptographic execution is authorized.

Initial hosted result is pending; do not substitute this plan for CI evidence.
