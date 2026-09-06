"""Hosted-only actual C++ endpoint producer -> independent Python finalizer.

Legacy framing is constructed test input, never evidence of an encrypted chain
or CTest execution. Endpoint records and TSV bytes come only from the producer.
"""
import argparse
from decimal import Decimal
from fractions import Fraction
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

from hosted_paper_endpoint_finalizer_complete import _hosted_identity
from paper_endpoint_gzip import verify_gzip
from paper_endpoint_primary_reader import MAX_LOG_BYTES, PrimaryIdentity, parse_primary_log
from paper_endpoint_publication import select_endpoint_uploads
from paper_endpoint_sidecar_reader import MAX_BYTES, read_sidecar
from paper_endpoint_sidecar_replay import derive_replay_bounds, frozen_input
from paper_endpoint_status import decode_status
import test_paper_endpoint_primary_reader as framing_fixture


def require(condition, detail):
    if not condition:
        raise AssertionError(detail)


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def constructed_framing(endpoint_bytes, source, host):
    require(endpoint_bytes.endswith(b"\n") and len(endpoint_bytes) <= MAX_LOG_BYTES,
            "producer endpoint block is outside the byte envelope")
    normalized = endpoint_bytes.replace(b"\r\n", b"\n") if host == "windows" else endpoint_bytes
    require(b"\r" not in normalized and b"\0" not in normalized and normalized.isascii(),
            "producer endpoint block is not canonical ASCII transport")
    lines = normalized.splitlines()
    require(len(lines) == 39 and all(line.startswith(b"FS_ENDPOINT_") for line in lines),
            "producer must emit only its 39 actual endpoint records")
    # Discard every fixture endpoint record. Retain only explicit test framing.
    legacy = [line for line in framing_fixture.complete_lines(2, scope="synthetic", host=host)
              if not line.startswith("FS_ENDPOINT_")]
    legacy = [line.replace(framing_fixture.SOURCE, source).replace(
        "label=fresh retained sub-binary64 witness", "label=final full-slot 2^-80 gate")
        for line in legacy]
    require(legacy[-1].startswith("COMPLETE "), "legacy fixture boundary changed")
    ending = b"\r\n" if host == "windows" and endpoint_bytes.endswith(b"\r\n") else b"\n"
    prefix = framing_fixture.ctest_log(legacy[:-1], line_ending=ending)
    suffix = framing_fixture.ctest_log(legacy[-1:], line_ending=ending)
    # Only the CTest prefix is added; each actual producer payload/line ending survives.
    actual_block = b"".join(b"61: " + line for line in endpoint_bytes.splitlines(keepends=True))
    return prefix + actual_block + suffix


def check_known_zero_endpoints(sidecar, primary):
    require(sidecar.meta["coefficient_l1_fresh"] == 0 and
            sidecar.meta["coefficient_l1_terminal"] == 0,
            "actual producer did not retain the known zero polynomials")
    require(sidecar.meta["numeric_gate_failures"] == 2 and
            sidecar.meta["E80_disposition"] == "FAIL", "synthetic endpoint failures changed")
    for row in sidecar.rows:
        z = frozen_input(row.slot)
        actual_e0 = tuple(Fraction(Decimal(value.text)) for value in row.values[:2])
        require(actual_e0 == (-z[0], -z[1]), "actual C++ E0 does not equal exact -z")

    bounds = derive_replay_bounds(sidecar.meta, maximum_e0_exponent=None,
                                  maximum_e8_exponent=None)
    slots = sorted({0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383} |
                   {maximum.argmax_slot for maximum in primary.endpoint.maxima})
    require(slots == [0, 1, 256, 257, 512, 513, 768, 769, 1023, 16289, 16353, 16383],
            "known zero-endpoint fixture no longer selects the twelve exact oracle slots")
    # Independent exact Gaussian-integer arithmetic: no Binary768/Decimal power
    # graph and no C++-generated expectation. Only a dozen selected rows, not a
    # second full-slot replay. The actual finalizer performs the full replay once.
    for slot in slots:
        z = frozen_input(slot)
        a, b = (component * (1 << 75) for component in z)
        require(a.denominator == b.denominator == 1, "frozen input is not 75-bit dyadic")
        real, imag = a.numerator, b.numerator
        for _ in range(8):
            real, imag = real * real - imag * imag, 2 * real * imag
        expected = (-Fraction(real, 1 << 19200), -Fraction(imag, 1 << 19200))
        for retained, exact in zip(sidecar.rows[slot].values[2:], expected):
            actual = Fraction(Decimal(retained.text))
            quantum = (Fraction(0) if retained.signed_significand == 0 else
                       Fraction(1, 2 * 10 ** (109 - retained.decimal_exponent)))
            require(abs(actual - exact) <= quantum + bounds.live_e8,
                    "actual C++ E8 disagrees with exact Gaussian-integer oracle")
    a8 = primary.endpoint.maxima[3]
    require(a8.residual_id == "A8" and a8.magnitude_exact == 0 and
            a8.argmax_slot == 0 and a8.argmax_component == "real",
            "known zero arithmetic residual does not obey the tie policy")
    return slots


def run_gate(producer, root):
    # Shared hosted identity gate requires RUN_ENDPOINT_COMPLETE_GATE=1 too;
    # the dedicated workflow explicitly supplies both opt-ins.
    identity = _hosted_identity()
    require(os.environ.get("RUN_ENDPOINT_CPP_INTEROP_GATE") == "1",
            "explicit hosted C++ interop opt-in is required")
    require(os.environ.get("OMP_NUM_THREADS") == "2", "hosted diagnostic concurrency is frozen")
    source_root = Path(__file__).resolve().parent.parent
    source = subprocess.run(["git", "-C", str(source_root), "rev-parse", "HEAD"],
                            capture_output=True, check=True, timeout=20)
    require(source.stdout == (identity.source_commit + "\n").encode("ascii"),
            "hosted source checkout differs from expected compiled source")
    require(producer.is_absolute() and producer.is_file() and not producer.is_symlink(),
            "producer must be an existing absolute regular executable")
    require(root.is_absolute() and not os.path.lexists(root) and
            root.parent.resolve(strict=True) == root.parent,
            "output root must be a fresh child of an existing normalized directory")
    suffix = (identity.source_commit + "." + identity.host + "." +
              identity.github_run_id + "." + identity.github_run_attempt)
    stem = "fs-residual-endpoint-01.v1-r1." + suffix
    synthetic_stem = "fs-endpoint-synthetic-" + suffix
    deepest = root / "published" / stem / ".staging" / (stem + ".candidate.status.json")
    require(identity.host != "windows" or len(str(deepest)) <= 247,
            "publisher path exceeds the conservative Windows boundary")
    root.mkdir(mode=0o700)
    canonical, published = root / "canonical", root / "published"
    canonical.mkdir(mode=0o700)
    published.mkdir(mode=0o700)
    receipt_path = root / "gate-receipt.json"
    receipt = dict(source_commit=identity.source_commit, host=identity.host,
        github_run_id=identity.github_run_id, github_run_attempt=identity.github_run_attempt,
        stage="before-producer", actual_crypto_chain_count=0, actual_ctest_invocations=0,
        legacy_framing="constructed-test-only", supplied_synthetic_ctest_exit=8,
        producer_exit=None, deepest_status_candidate_characters=len(str(deepest)))
    receipt_path.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    stdout_path, stderr_path = root / "producer.stdout.txt", root / "producer.stderr.txt"
    command = [str(producer), "--endpoint-cpp-interop", str(canonical), identity.host,
               identity.github_run_id, identity.github_run_attempt]
    with stdout_path.open("xb") as stdout, stderr_path.open("xb") as stderr:
        try:
            result = subprocess.run(command, stdin=subprocess.DEVNULL, stdout=stdout, stderr=stderr,
                                    cwd=source_root, check=False, timeout=900)
        except (subprocess.TimeoutExpired, OSError) as error:
            # Preserve a truthful diagnostic stage, then retain the real failure
            # and traceback. No retry, successful status, or invented exit code.
            receipt.update(stage="producer-call-failed", producer_failure_type=type(error).__name__)
            receipt_path.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
            raise
    receipt.update(stage="producer-returned", producer_exit=result.returncode,
                   producer_stdout_bytes=stdout_path.stat().st_size,
                   producer_stderr_bytes=stderr_path.stat().st_size)
    receipt_path.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    require(stdout_path.stat().st_size <= MAX_LOG_BYTES and stderr_path.stat().st_size <= 65536,
            "producer transport exceeds its retained byte envelope")
    endpoint_bytes, stderr_bytes = stdout_path.read_bytes(), stderr_path.read_bytes()
    receipt.update(producer_stdout_sha256=sha256(endpoint_bytes),
                   producer_stderr_sha256=sha256(stderr_bytes))
    receipt_path.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(receipt, sort_keys=True), flush=True)
    require(result.returncode == 0,
            f"actual C++ producer returned {result.returncode}; required synthetic mode is not usable")
    require(stderr_bytes == b"", "successful producer emitted unexpected stderr")
    primary_bytes = constructed_framing(endpoint_bytes, identity.source_commit, identity.host)
    primary_path = root / "primary.ctest.log"
    primary_path.write_bytes(primary_bytes)
    canonical_path = canonical / synthetic_stem / (synthetic_stem + ".tsv")
    require(canonical_path.is_file() and not canonical_path.is_symlink() and
            0 < canonical_path.stat().st_size <= MAX_BYTES, "actual synthetic TSV is missing")
    canonical_bytes = canonical_path.read_bytes()
    arguments = dict(expected_source_commit=identity.source_commit, expected_host=identity.host,
                     expected_run_id=identity.github_run_id, expected_run_attempt=identity.github_run_attempt)
    sidecar = read_sidecar(canonical_path, expected_scope="synthetic", **arguments)
    primary = parse_primary_log(primary_bytes, PrimaryIdentity(identity.source_commit, identity.host,
        identity.github_run_id, identity.github_run_attempt), ctest_exit_code=8, expected_scope="synthetic")
    require(primary.evidence_state == "COMPLETE", "constructed transport did not retain complete endpoint facts")
    exact_slots = check_known_zero_endpoints(sidecar, primary)
    finalized = subprocess.run([sys.executable, "-B", str(source_root / "tests/paper_endpoint_finalizer.py"),
        "finalize", "--source-commit", identity.source_commit, "--host", identity.host,
        "--github-run-id", identity.github_run_id, "--github-run-attempt", identity.github_run_attempt,
        "--primary-log", str(primary_path), "--ctest-exit-code", "8", "--capture-exit-code", "0",
        "--scope", "synthetic", "--canonical-parent", str(canonical),
        "--published-parent", str(published)], cwd=root, capture_output=True, check=False, timeout=600)
    require(finalized.returncode == 8 and finalized.stdout == finalized.stderr == b"",
            "independent finalizer did not preserve the supplied synthetic failure silently")
    selected = select_endpoint_uploads(published, identity)
    require(tuple(p.name for p in selected) == (stem + ".tsv.gz", stem + ".status.json"),
            "actual data did not reach complete exact publication")
    status = decode_status(selected[1].read_bytes(), **arguments)
    require(status["evidence_state"] == "COMPLETE" and status["reason"] == "NONE" and
            status["row_count"] == 16384 and status["numeric_gate_failures"] == 2 and
            status["E80_disposition"] == "FAIL" and status["ctest_exit_code"] == 8 and
            status["boost_version"] == sidecar.meta["boost_version"], "complete status facts mismatch")
    gzip_bytes = selected[0].read_bytes()
    roundtrip = verify_gzip(gzip_bytes, canonical_size=status["canonical_bytes"],
        canonical_sha256=status["canonical_sha256"], gzip_size=status["gzip_bytes"],
        gzip_sha256=status["gzip_sha256"])
    require(roundtrip.canonical == canonical_bytes and canonical_path.read_bytes() == canonical_bytes and
            primary_path.read_bytes() == primary_bytes and stdout_path.read_bytes() == endpoint_bytes,
            "validated C++ bytes changed across finalization")
    receipt.update(stage="complete", result="PASS", exact_e0_rows=16384,
        exact_gaussian_e8_slots=exact_slots, independent_finalizer_exit=finalized.returncode,
        canonical_bytes=len(canonical_bytes), canonical_sha256=sha256(canonical_bytes),
        primary_sha256=sha256(primary_bytes), gzip_bytes=len(gzip_bytes), gzip_sha256=sha256(gzip_bytes),
        status_sha256=sha256(selected[1].read_bytes()), boost_version=status["boost_version"],
        evidence_state=status["evidence_state"], E80_disposition=status["E80_disposition"],
        caveat="synthetic diagnostic interop only; original live E80 remains FAIL")
    receipt_path.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(receipt, sort_keys=True), flush=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--producer", type=Path, required=True)
    parser.add_argument("--output-root", type=Path, required=True)
    args = parser.parse_args()
    run_gate(args.producer, args.output_root)
