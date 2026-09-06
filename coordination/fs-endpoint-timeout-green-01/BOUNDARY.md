# Observed CTest timeout -> public finalizer CLI

Root-owned vertical slice at base 0e3b82bdf71a49f21b497f4ad27a69792e18159a.
The user delegated ordinary internal seam and implementation choices. The already
agreed finalizer CLI / status filesystem seam and integration contract section 5
remain the seam; no new public flag or test runner is introduced.

Actual prerequisite RED: run 34018316144 attempt 1, Linux 101446076946 and
Windows 101446076882. Both ran CTest #61 once with a two-second timeout, returned
shell status 8, transported one exact status artifact, then failed the final
TIMEOUT assertion because finalizer reported CTEST_FATAL. This is not a new
crypto experiment and not a passing workflow. Source and exact logs are retained
in ../fs-endpoint-ctest-protocol-01. The three negative gates in run 34018316389
are separate accepted regression evidence, not this timeout RED.

The two tiny frozen fixtures are reconstructed from every escaped physical line
of the hosted inspector output, with size and SHA-256 equality checked against
the inspector's complete primary byte receipt. Linux 1515 bytes /
`85f8286ef172a81e22c3cdacfe50dfed37692b686c0f1faffe0f548bbbe0f6e0`;
Windows 1240 bytes /
`40d229aa770ca4fe7d0372f9b5e80ee6b57946bfb4c0f3fb26b24931ac22df81`.
Both actual streams are LF-only. Any CRLF test is a constructed Windows-mode
compatibility case, not a claim that the hosted Windows stream used CRLF.

First add one failing public-CLI test over the actual two transcripts; then
derive timeout only from the already bounded bytes that are given to the primary
reader and the actual nonzero CTest status. Retain the explicit data-seam
timed_out Boolean for existing callers; do not add a trusted CLI Boolean.
Use the exact one-test #61 result and final CTest failure summary, not occurrence
of the word timeout, child-prefixed text, or exit 8 alone. Preserve existing
typed first-cause, capture IO_ERROR, validated partial facts, and nonzero return.
Unsupported/truncated/contradictory transport must not be called TIMEOUT.

Root will then exercise a few discriminating transcript mutations through the
same CLI, without mocking internal readers or running full-slot replay locally.
Only tiny Python/real-filesystem checks may run on Mac. No local CMake/CTest/
compilation/FFT/NTT/FHE/16384-row fixture, numerical threshold or live-CI change.
After independent review, change only the dedicated protocol workflow trigger
to a new GREEN ref and authorize one synthetic CTest per host. Its wrapper
and jobs remain nonzero because timeout is deliberate; GREEN acceptance requires
the final TIMEOUT verifier and all intermediate transport steps to succeed.
Final paper-scale integration, all-slot diagnosis and original E80 acceptance
remain pending regardless of this slice's outcome.
