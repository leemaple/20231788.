# Completed one-shot evidence seam

Owner-authored `finalize_once.py` does not launch processes or encrypt. It validates the
completed wrapper receipts, exact source/entry/output path/frozen1200second timeout,
timestamp ordering, actual process status, exact stdout footer and empty stderr, and
regular bounded raw TSV. It reads raw bytes once and calls the reviewed scalar receiver
at180 and230decimal digits. Decisions/gates/maximizing slots must agree and maximum
values differ by less than2^-300. It exclusively writes verification.json only after
all checks; a valid numericalFAIL is written asFAIL and exits1, while invalid evidence
fails loudly without a verification artifact. Absence is not a numericalFAIL orPASS.

One cohesive public seam tested: completed process directory -> retained verification.
First test execution before implementation: exit1, ModuleNotFoundError finalize_once,
an explicit missing greenfield capability, not a scientific or regression RED.
Initial implementation run revealed a fixture-only macOS /var versus /private/var
canonical-path mismatch (2errors/1failure); the real wrapper already records resolved
paths, so the fixture was corrected withPath.resolve without weakening the gate.
Final local run:3/3PASS in1.567seconds, Python-B-I. Cases cover syntheticPASS, synthetic
valid numericFAIL with A8 above its gate, wrong source, truncated finalLF and repeated
verification refusal. Both positive outcomes replay one synthetic TSV at two precisions,
never produce two payloads. Printed hashes are synthetic fixtures, not measured results:
PASS06eabe785a976e55485fdd26de3eced544d566284ed329309c4ab1878be09818;
FAIL05eef5f9e5a8b96f7f7267f2bf70983f337c1a12fc6a3e3fbab4fb348fa41903.

The finalizer checks internal consistency, not independent runtime attestation. It must
be paired with exact GitHub source/run/job/provenance and trusted one-shot entry evidence.
Cross-precision agreement remains CONDITIONAL_OBSERVER_NOT_FORMAL. No fresh-noise lift,
universal annulus/key guarantee, original near-unit repair, Table3statistics or security
claim follows. Independent integration review is pending at this checkpoint.
