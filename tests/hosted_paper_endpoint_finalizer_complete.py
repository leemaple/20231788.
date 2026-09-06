"""Hosted-only full synthetic finalizer gate; intentionally not test-discovered."""
from fractions import Fraction
import hashlib
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

from paper_endpoint_gzip import verify_gzip
import paper_endpoint_finalizer as finalizer
from paper_endpoint_publication import PublicationIdentity, select_endpoint_uploads
from paper_endpoint_status import decode_status
import test_paper_endpoint_primary_reader as primary_fixture
import test_paper_endpoint_sidecar_reader as sidecar_fixture


def _hosted_identity():
    if sys.version_info[:2] != (3, 12):
        raise SystemExit("hosted complete-finalizer gate requires Python 3.12")
    required = {
        "GITHUB_ACTIONS": "true",
        "RUNNER_ENVIRONMENT": "github-hosted",
        "RUN_ENDPOINT_COMPLETE_GATE": "1",
    }
    for name, expected in required.items():
        if os.environ.get(name) != expected:
            raise SystemExit(f"hosted complete-finalizer gate requires {name}={expected}")
    host = os.environ.get("ENDPOINT_TEST_HOST")
    if host not in ("linux", "windows"):
        raise SystemExit("ENDPOINT_TEST_HOST must be linux or windows")
    expected_platform = "linux" if host == "linux" else "win32"
    if sys.platform != expected_platform:
        raise SystemExit("ENDPOINT_TEST_HOST disagrees with the native Python platform")
    source_commit = os.environ.get("GITHUB_SHA", "")
    run_id = os.environ.get("GITHUB_RUN_ID", "")
    run_attempt = os.environ.get("GITHUB_RUN_ATTEMPT", "")
    if re.fullmatch(r"[0-9a-f]{40}", source_commit, flags=re.ASCII) is None:
        raise SystemExit("GITHUB_SHA must be the exact lowercase 40-hex source identity")
    for name, value in (("GITHUB_RUN_ID", run_id),
                        ("GITHUB_RUN_ATTEMPT", run_attempt)):
        if re.fullmatch(r"[1-9][0-9]*", value, flags=re.ASCII) is None:
            raise SystemExit(f"{name} must be a positive decimal identity")
    return PublicationIdentity(source_commit, host, run_id, run_attempt)


def _replace_count(data, old, new, expected_count):
    if data.count(old) != expected_count:
        raise RuntimeError("synthetic fixture token changed")
    return data.replace(old, new)


def _matching_full_inputs(identity):
    sidecar = sidecar_fixture.valid_bytes(scope="synthetic")
    replacements = (
        (f"meta\tsource_commit\t{sidecar_fixture.SOURCE}\n".encode("ascii"),
         f"meta\tsource_commit\t{identity.source_commit}\n".encode("ascii")),
        (b"meta\thost\tlinux\n", f"meta\thost\t{identity.host}\n".encode("ascii")),
        (b"meta\tgithub_run_id\t17\n",
         f"meta\tgithub_run_id\t{identity.github_run_id}\n".encode("ascii")),
        (b"meta\tgithub_run_attempt\t2\n",
         f"meta\tgithub_run_attempt\t{identity.github_run_attempt}\n".encode("ascii")),
        (b"meta\tnumeric_gate_failures\t7\n",
         b"meta\tnumeric_gate_failures\t2\n"),
    )
    for old, new in replacements:
        sidecar = _replace_count(sidecar, old, new, 1)

    allowance = Fraction(1, 1 << 854)
    primary_lines = primary_fixture.complete_lines(
        2, scope="synthetic", host=identity.host,
        fresh768=allowance, terminal768=allowance)
    line_ending = b"\r\n" if identity.host == "windows" else b"\n"
    primary = primary_fixture.ctest_log(primary_lines, line_ending=line_ending)
    primary = _replace_count(
        primary, primary_fixture.SOURCE.encode("ascii"),
        identity.source_commit.encode("ascii"), 3)
    primary = _replace_count(
        primary, b"github_run_id=101\t",
        f"github_run_id={identity.github_run_id}\t".encode("ascii"), 1)
    primary = _replace_count(
        primary, b"github_run_attempt=2\t",
        f"github_run_attempt={identity.github_run_attempt}\t".encode("ascii"), 1)
    return sidecar, primary


class HostedCompleteFinalizerTests(unittest.TestCase):
    def test_complete_failing_e80_evidence_is_published_and_remains_nonzero(self):
        identity = _hosted_identity()
        sidecar_bytes, primary_bytes = _matching_full_inputs(identity)
        self.assertTrue(sidecar_bytes.endswith(b"\n"))
        self.assertNotIn(b"\r", sidecar_bytes)
        if identity.host == "windows":
            self.assertEqual(primary_bytes.count(b"\n"), primary_bytes.count(b"\r\n"))
        else:
            self.assertNotIn(b"\r", primary_bytes)

        stem = ("fs-residual-endpoint-01.v1-r1." + identity.source_commit + "." +
                identity.host + "." + identity.github_run_id + "." +
                identity.github_run_attempt)
        with tempfile.TemporaryDirectory(
                prefix="fs-endpoint-synthetic-",
                dir=Path(os.environ["RUNNER_TEMP"]).resolve(strict=True)) as temporary:
            root = Path(temporary).resolve()
            canonical_parent = root / "canonical"
            published_parent = root / "published"
            canonical_parent.mkdir(mode=0o700)
            published_parent.mkdir(mode=0o700)
            canonical_directory = canonical_parent / stem
            canonical_directory.mkdir(mode=0o700)
            canonical_path = canonical_directory / (stem + ".tsv")
            canonical_path.write_bytes(sidecar_bytes)
            primary_path = root / "primary.ctest.log"
            primary_path.write_bytes(primary_bytes)

            command = [
                sys.executable, "-B", str(Path(finalizer.__file__).resolve()), "finalize",
                "--source-commit", identity.source_commit,
                "--host", identity.host,
                "--github-run-id", identity.github_run_id,
                "--github-run-attempt", identity.github_run_attempt,
                "--published-parent", str(published_parent),
                "--primary-log", str(primary_path),
                "--ctest-exit-code", "8",
                "--capture-exit-code", "0",
                "--scope", "synthetic",
                "--canonical-parent", str(canonical_parent),
            ]
            completed = subprocess.run(
                command, cwd=root, capture_output=True, timeout=600, check=False)

            self.assertEqual(completed.returncode, 8, completed.stderr.decode(
                "utf-8", errors="replace"))
            self.assertEqual(completed.stdout, b"")
            self.assertEqual(completed.stderr, b"")
            self.assertEqual(canonical_path.read_bytes(), sidecar_bytes)
            self.assertEqual(primary_path.read_bytes(), primary_bytes)

            selected = select_endpoint_uploads(published_parent, identity)
            self.assertEqual(len(selected), 2)
            self.assertEqual(
                tuple(path.name for path in selected),
                (stem + ".tsv.gz", stem + ".status.json"),
            )
            self.assertTrue(all(path.parent == published_parent / stem for path in selected))

            gzip_bytes = selected[0].read_bytes()
            status = decode_status(
                selected[1].read_bytes(),
                expected_source_commit=identity.source_commit,
                expected_host=identity.host,
                expected_run_id=identity.github_run_id,
                expected_run_attempt=identity.github_run_attempt,
            )
            self.assertEqual(
                {key: status[key] for key in (
                    "evidence_state", "reason", "observer_disposition",
                    "packer_disposition", "chain_count", "row_count",
                    "numeric_gate_failures", "E80_disposition", "A_disposition",
                    "boost_version", "ctest_exit_code",
                )},
                {
                    "evidence_state": "COMPLETE", "reason": "NONE",
                    "observer_disposition": "PASS", "packer_disposition": "PASS",
                    "chain_count": 1, "row_count": 16384,
                    "numeric_gate_failures": 2, "E80_disposition": "FAIL",
                    "A_disposition": "NOT_ADOPTED", "boost_version": 108300,
                    "ctest_exit_code": 8,
                },
            )
            self.assertEqual(status["canonical_bytes"], len(sidecar_bytes))
            self.assertEqual(
                status["canonical_sha256"], hashlib.sha256(sidecar_bytes).hexdigest())
            self.assertEqual(status["gzip_bytes"], len(gzip_bytes))
            self.assertEqual(status["gzip_sha256"], hashlib.sha256(gzip_bytes).hexdigest())
            self.assertEqual(status["gzip_filename"], stem + ".tsv.gz")
            self.assertEqual(status["status_filename"], stem + ".status.json")
            receipt = verify_gzip(
                gzip_bytes,
                canonical_size=status["canonical_bytes"],
                canonical_sha256=status["canonical_sha256"],
                gzip_size=status["gzip_bytes"],
                gzip_sha256=status["gzip_sha256"],
            )
            self.assertEqual(receipt.canonical, sidecar_bytes)


if __name__ == "__main__":
    unittest.main(verbosity=2)
