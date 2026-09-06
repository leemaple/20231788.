# FS endpoint sidecar reader — bounded evidence

Base: `643bfac42fe2233fbda7f5676c3c2aece01e9496`

Environment: local macOS workspace. The current retained checks use bundled Python
3.12.14. Earlier development receipts used system Python 3.9.6; those descriptions
below are provenance notes, not retained raw transcripts. Wall-clock timestamps
were not captured. No C++, crypto, FFT/NTT, CI, browser, external model, push, or
live endpoint data was used.

## Scope and interpretation

The owned seam is strict reading/validation of one uncompressed v1-r1 canonical
TSV. Synthetic fixtures use disposable `fs-endpoint-synthetic-` directories and
are accepted only when the caller explicitly requests `expected_scope="synthetic"`.
Live validation requires `expected_scope="live-single-chain"` and caller-supplied
source, host, run, and attempt identities.

The reader validates the exact 43 metadata records, 24 checks, 16,384 canonical
rows, frozen S0/S8, endpoint radius, numeric/E80 consistency, independently
derived allowances, PASS classification, the ten frozen Horner argmax anchors,
and the canonical slot-0/real tie for a zero distance. Canonical values are parsed
exactly into signed significand plus decimal exponent without allocating powers
as large as the exponent.

This is intentionally partial: no scalar replay, primary-log reconciliation,
all-nine-scale receipt validation, gzip/status processing, publication, writer,
CTest wrapper, or upload authorization exists. Nonzero argmax correctness and the
emitted distances cannot be independently reconstructed from this sidecar's
available fields alone. The reader rejects a direct leaf symlink; trusted
owned-directory, parent-symlink, and TOCTOU publication protection remains the
final packer's responsibility.

## Earlier development provenance (not raw retained evidence)

Initial command:

```sh
python3 --version
PYTHONPATH=tests python3 -m unittest -v tests/test_paper_endpoint_sidecar_reader.py
```

Initial RED: Python 3.9.6, exit 1, `ModuleNotFoundError: No module named
'paper_endpoint_sidecar_reader'`; one loader test errored.

First full-row execution after the initial reader implementation:

```sh
PYTHONPATH=tests python3 -m unittest -v \
  tests.test_paper_endpoint_sidecar_reader.SidecarReaderTests.test_reads_complete_16384_row_synthetic_sidecar
```

RED: exit 1, parsed `meta["row_count"]` was string `"16384"` instead of integer
`16384`. After the minimal typed-metadata correction, the same command exited 0;
one test passed in 0.115s.

Radius-guard tracer:

```sh
PYTHONPATH=tests python3 -m unittest -v \
  tests.test_paper_endpoint_sidecar_reader.SidecarReaderTests.test_rejects_endpoint_radius_above_five_fourths
```

RED: exit 1, `SidecarError` was not raised. After enforcing both endpoint maxima
`<=5/4`, the final owned suite covered this case successfully.

Caller-identity type tracer added afterward returned exit 1 with
`TypeError: cannot use a string pattern on a bytes-like object` for a bytes
source identity. The public boundary was tightened to reject non-string source
identities with `SidecarError`; the focused test and final suite then passed.

The original Python 3.9 final command was:

```sh
python3 --version
PYTHONPATH=tests python3 -m unittest -v tests/test_paper_endpoint_sidecar_reader.py
python3 -m py_compile tests/paper_endpoint_sidecar_reader.py tests/test_paper_endpoint_sidecar_reader.py
git diff --check
```

Original GREEN: Python 3.9.6; seven tests passed in 0.543s; `py_compile` and
`git diff --check` exited 0.

Root later observed the original seven-test suite on bundled Python 3.12.14 as
seven tests, six errors, one pass, 0.018s, exit 1: all six errors occurred while
the synthetic fixture called `str()` on the S8 numerator above Python's 4,300-digit
conversion limit, before reader behavior. Root owns that raw receipt; it is not
re-created or claimed as a retained file here.

## Retained current RED/GREEN evidence

The fixture now formats only its bounded large integers in base-10 chunks, without
globally changing Python's integer conversion limit. Its S8 expectation is the
closed product, while the reader retains the specified recurrence. The fixture
uses `C_fresh=C_terminal=3`, which independently yields nonzero fractional `C/S`,
`K=2^-98`, and a literal `fresh.cross=terminal.cross=2^-598+2^-854` expectation.

- `evidence/01-behavioral-red-python312.txt`: exact focused RED, exit 1. The
  unchanged reader accepted an invalid argmax and rejected the legal bounded
  denominator `2^40000`; the independent nonzero-K allowance test passed.
- `evidence/02-behavioral-green-python312.txt`: exact focused GREEN, four tests,
  exit 0 after only the owned reader corrections.
- `evidence/03-full-green-python312.txt`: exact full owned-suite GREEN, eleven
  tests in 0.630s, exit 0, before replacing the remaining expected-allowance
  derivation with independently reduced literal algebra.
- `evidence/04-independent-fixture-full-green-python312.txt`: exact current full
  GREEN after that fixture-independence correction, eleven tests in 0.628s,
  exit 0.

Current SHA-256:

- `tests/paper_endpoint_sidecar_reader.py`: `b24a7d9828a7cc6d086537afd7bec2a04018c3ce2509157d7416e2ff151ebf30`
- `tests/test_paper_endpoint_sidecar_reader.py`: `84fe5759a2d4ae6cc9dd393ffab105ac44083c0d0ba2e01922d1954245187791`
