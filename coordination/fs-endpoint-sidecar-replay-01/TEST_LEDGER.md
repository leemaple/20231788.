# FS endpoint scalar replay — bounded ledger

Base: `643bfac42fe2233fbda7f5676c3c2aece01e9496`

Environment: local macOS workspace; bundled CPython 3.12.14, stdlib Decimal
precision 256 and `ROUND_HALF_EVEN`. No C++, OpenFHE, crypto, FFT/NTT, CI,
browser, external model, push, or full 16,384-row Decimal replay was executed.
Wall-clock timestamps were not captured.

## Owned boundary

`paper_endpoint_sidecar_replay.py` consumes the structural result of the sealed
reader without importing that reader or any project module. It validates exactly
16,384 ordered rows, scans canonical exponents and derives serialization/replay
allowances before any squaring, then streams rows without retaining arrays of
Decimal or Fraction values. It independently reconstructs frozen dyadic `z`,
performs two powers through one shared eight-squaring Decimal kernel, derives
signed E0/E8/I8/A8 and R=`E8-I8-A8`, and retains component maxima with the
least-slot/real-first tie rule. Each maximum retains the four complete complex
E0/E8/I8/A8 values at that slot, not four scalars from only the maximizing
component. `replay_row(slot,row)` provides bounded replay at any later primary
receipt slot; reconciliation need not assume primary and replay argmax equality.

Both the exact reconstructed fresh value and its represented Decimal value are
checked against the 3/2 component one-norm disk. `L_replay`, Decimal P/R, U0 and
the four replay bounds follow ENDPOINT_SPEC section 6 and must remain within
2^-128. Extreme positive serialization exponents are rejected before constructing
large powers; only one row at a time can enter the exact Fraction fallback.

The TSV does not contain live I/A maxima or their signed tuples. This slice does
not invent them or parse an unspecified stdout grammar. Comparison with primary
receipts, their reporting quanta, all-nine-scale reconciliation, log/exit handling,
gzip, status, publication and upload remain separate unfinished packer work.

The spec mandates exactly Decimal256; no second-precision numerical replay was
substituted. Exact Fraction input/disk/bound checks provide a separate arithmetic
check, but are not claimed as a second numerical implementation or interval proof.

## Resource decision

A complete replay executes 16 complex squarings per slot: 262,144 complex
squarings and 1,048,576 Decimal multiplications across 16,384 slots, before the
remaining additions and validation. It was retained for hosted execution rather
than run on this Mac. The local tests use real-only and nonzero-imaginary
hand-worked scalars, three literal frozen-input points, bounded public
max/tie/bounds/row-lookup seams, and malformed/unsupported early exits. A complete
nonzero 16,384-row fixture exists, includes an E0 maximum distinguished only after
significant digit 28, but is explicitly `HOSTED NOT RUN`. The all-zero
reader fixture is not treated as physical replay evidence and no sparse sample is
reported as a full-slot replay.

## Actual retained RED/GREEN evidence

- `evidence/01-import-red-python312.txt`: initial missing-module RED.
- `evidence/02-overprecise-oracle-red-python312.txt`: a real second RED showed
  that an initial test incorrectly expected exact rational subtraction where
  Decimal256 correctly rounds across more than 256 significant places. The test
  oracle was corrected to a nonzero dyadic case that closes exactly within 256
  digits; implementation precision was not widened.
- `evidence/03-row-count-red-python312.txt`: absent full-sidecar seam RED.
- `evidence/04-budget-order-red-python312.txt`: preflight-budget seam RED.
- `evidence/05-frozen-input-red-python312.txt`: absent exact frozen-input seam RED.
- `evidence/06-final-light-green-python312.txt`: pre-review five-test GREEN.
- `evidence/07-complex-tuple-cycle-python312.txt`: review-finding RED and focused
  GREEN for complete-complex tuple retention and tie behavior.
- `evidence/08-shared-kernel-imag-python312.txt`: nonzero-imaginary worked fixture
  before and after extracting the single shared scalar kernel.
- `evidence/09-public-bounds-cycle-python312.txt`: public exact-bounds RED/GREEN.
- `evidence/10-primary-slot-cycle-python312.txt`: arbitrary primary-slot lookup
  RED/GREEN.
- `evidence/11-review-fixes-final-green-python312.txt`: ten-test GREEN before the
  hosted fixture received its explicit environment gate.
- `evidence/12-env-gated-final-green-python312.txt`: then-current ten-test discovery:
  nine PASS, one explicit hosted-only SKIP, 0.029s, exit 0. Setting
  `RUN_FULL_ENDPOINT_REPLAY=1` enables that single complete fixture remotely.
- `evidence/13-decimal-near-tie-cycle-python312.txt`: public summary RED proved
  ambient Decimal28 `abs` collapsed a difference at significant digit 29; focused
  GREEN follows the context-free `Decimal.copy_abs` magnitude fix. The same helper
  now guards maxima, disk checks and the streaming quarter fast path; Fraction
  magnitudes retain ordinary `abs`.
- `evidence/14-decimal-context-final-green-python312.txt`: current eleven-test
  discovery: ten PASS, one explicit hosted-only SKIP, 0.029s, exit 0. The complete
  16,384-row hosted fixture was not run on this Mac.

The sealed reader files remained byte-identical before and after this work:

- `tests/paper_endpoint_sidecar_reader.py`: `b24a7d9828a7cc6d086537afd7bec2a04018c3ce2509157d7416e2ff151ebf30`
- `tests/test_paper_endpoint_sidecar_reader.py`: `84fe5759a2d4ae6cc9dd393ffab105ac44083c0d0ba2e01922d1954245187791`

Current replay hashes:

- `tests/paper_endpoint_sidecar_replay.py`: `b0a879a90ed0e730c53eb569f7412841be5ba88bb8aef162b6a652e6d08367ef`
- `tests/test_paper_endpoint_sidecar_replay.py`: `c7e2ff12299e8955808a34a133b1ea9015699f3a1cb9efe5caa27b2ce9af0196`

Root's separate corrected-reader run (11 PASS, 0.695s, exit 0) is retained in
`ROOT_PYTHON312_CORRECTED.json`; it was not rerun or claimed as this slice's test.
