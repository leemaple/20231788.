# C++ endpoint interop test boundary

Base source: `3897832bdaf95c05062f1bcec161b929e60d1ede`.

The new test-only public seam is
`paper_endpoint_contract::synthetic::interop_test::ProduceCapturedEndpointEvidence`.
It accepts an existing exclusive trusted parent, caller-supplied hosted OS and
GitHub run identity, and an output stream. It binds `source_commit` to the
configured `PAPER_SOURCE_COMMIT` and `boost_version` to the compiled
`BOOST_VERSION`; callers cannot relabel either value.

The producer constructs only two centered all-zero degree-32768 integer
polynomials, the frozen exact `S0` and `S8`, ten known-zero Horner values, and
16384 known-zero client values. It calls the real `CaptureEndpointEvidence`,
`WriteEndpointEvidence`, and `EmitEndpointEvidencePrimary` functions. It does
not construct an OpenFHE context, generate keys, encrypt, create ciphertexts,
run any production transform, NTT, or FHE operation, or call the normal paper
`Run()`. `CaptureEndpointEvidence` does execute its full-slot diagnostic
binary512/binary768 twisted FFT observers and direct control references. Those
observer transforms are part of the hosted test and must not be run locally.

The independent expectations are fixed by the frozen input formula: `E0=-z`,
`E8=I8=-z^256`, and `A8=0`. The C++ boundary checks every retained E0/E8 row,
the literal E0 maximum at slot 16353 imag, the literal E8/I8 maximum at slot
16289 imag, and the zero A8 least-slot tie. Both E0 and E8 exceed the unchanged
`2^-80` original error gate, so the synthetic endpoint-only count is exactly 2
and the emitted `E80_disposition` remains `FAIL`; `A_disposition` remains
`NOT_ADOPTED`. This deliberately is not evidence that the live chain ran.

The literal argmax sources are distinct:

- E0 slot 16353 imag follows exactly from the dyadic input formula. Its large
  component is
  `a(s)=1015/1024-(floor(s/2) mod 16)/65536+s/2^75`; the class penalty
  `2^-16` is larger than the entire slot perturbation (less than `2^-61`), so
  the maximum is in residue class zero. The largest such half-slot is 8176 and
  its larger member is `s=16353`. Its phase is 3, so `z=(b,-a)` and `E0=-z`
  selects the positive imaginary component.
- E8/I8 slot 16289 imag was obtained by an actually executed bounded standalone
  200-digit Python `Decimal` enumeration of the frozen formula and exactly eight
  complex squarings, with no repository import. It gave magnitude
  `0.10465060006182522730584052744902439461689881415376073080...` at slot
  16289 imag. This is independent candidate-selection evidence, not a certified
  interval proof. The exact command and complete retained tool result are
  recorded in `ARGMAX_CANDIDATE_RECEIPT.json`; it records the winning candidate
  only and does not establish a runner-up gap. The calculation was not rerun to
  create that receipt.

The algebraic identities `E0=-z`, `E8=I8=-z^256`, and `A8=0` are independent
consequences of choosing zero fresh and terminal observations. The header's
per-row binary768 eight-squaring graph intentionally uses the same precision
and operation order as Capture, so that exact C++ equality is a consistency
assertion, not the sole oracle. The independent acceptance oracle is the Python
Decimal256 replay of all canonical C++ writer rows and its reconciliation with
the actual C++ primary maxima.

The writer's required synthetic stem remains
`fs-endpoint-synthetic-<source>.<host>.<run>.<attempt>`. The current Python
finalizer instead derives the live `fs-residual-endpoint-01.v1-r1...` stem and
the current primary parser requires a full legacy chain transcript. Neither is
worked around here: the hosted interop test must consume the synthetic stem and
the real endpoint-primary block without presenting constructed framing as a
legacy-chain execution.

Integration may keep the production primary reader unchanged by surrounding the
actual C++ endpoint-primary block with explicitly constructed synthetic legacy
transport records, as the existing hosted finalizer fixtures do. That framing is
test input only: its gate receipt must say
`legacy_framing=constructed-test-only` and `actual_crypto_chain_count=0`, record
the C++ producer exit separately, and supply the expected synthetic CTest status
8 separately. It is not evidence of CTest execution, owner cleanup, receipts, or
an encrypted chain. All endpoint scales, checks, maxima, tuples, and sidecar rows
used for reconciliation must come unchanged from the actual C++ producer and
its actual TSV; fixture-generated endpoint summaries are forbidden.

No existing main, source, test, CMake, workflow, or Python file is modified in
this branch. Compilation and execution are intentionally left for the exact
hosted RED/green sequence owned by integration.
