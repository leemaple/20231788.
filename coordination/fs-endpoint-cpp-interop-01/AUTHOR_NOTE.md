# Author note

This is a bounded source-only TDD draft. Local compile and test status is
`NOT RUN`; no local transform, OpenFHE build, encryption, FFT, NTT, or FHE run
was performed. The header is intentionally not reachable from the current
executable, so the first hosted check should record RED for the absent dispatch
before integration adds the include and argument branch.

A bounded standalone Python Decimal calculation was used only to select and
rank the literal E8/I8 argmax candidates recorded in `BOUNDARY.md`. It did not
import or execute project code and was not a compile, test, diagnostic FFT, NTT,
or FHE run. The future full Python replay remains the acceptance oracle.

Minimal dispatch proposal for `paper_full_eight_square_contract_test.cpp`:

1. Include `paper_endpoint_interop_test.h`.
2. Before the existing observer-self-test branch and before `Run()`, recognize
   exactly `--endpoint-cpp-interop <trusted-parent> <linux|windows> <run-id>
   <run-attempt>`.
3. Call `ProduceCapturedEndpointEvidence(argv[2], argv[3], argv[4], argv[5],
   std::cout)` and return zero only after its real writer and primary emission
   complete. Reuse the existing typed endpoint-failure reporting pattern on
   failure. Do not emit legacy `BEGIN`, `RECEIPT`, cleanup, numeric-label, or
   `COMPLETE` records from this synthetic mode.

The hosted Python side should independently read the returned deterministic
synthetic path with `paper_endpoint_sidecar_reader.read_sidecar`, replay all
16384 rows with `paper_endpoint_sidecar_replay.replay_sidecar`, and check the
real endpoint-primary block against the same identity and exact invariants. It
may wrap that unmodified block in constructed test-only legacy transport framing
to reuse the existing production parser/finalizer. The receipt must distinguish
that framing from execution with `legacy_framing=constructed-test-only`,
`actual_crypto_chain_count=0`, a separately observed producer exit, and a
separately supplied synthetic CTest status 8. The wrapper must not replace any
C++-emitted endpoint scale, check, maximum, tuple, or TSV row with fixture data.
Adapting the finalizer/publication namespace and hosted workflow is outside this
author's file ownership.

The author performed only a source/static self-review. That is not the required
independent semantic, adversarial, compilation, or hosted acceptance review.
