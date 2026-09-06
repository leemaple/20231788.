"""Hosted-only discriminating finalizer negatives; not test-discovered."""
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

import hosted_paper_endpoint_finalizer_complete as complete_fixture
import paper_endpoint_finalizer as finalizer
from paper_endpoint_publication import PublicationIdentity, select_endpoint_uploads
from paper_endpoint_status import decode_status


ZERO = "+0." + "0" * 109 + "e+00000"
CONDITIONING_E0_REAL = "+1." + "0" * 109 + "e+00000"
MISMATCHED_A8_IMAG = "+1." + "0" * 109 + "e-00080"
MISMATCHED_A8_IMAG_Q_DEN = "2" + "0" * 189


def _hosted_identity():
    if sys.version_info[:2] != (3, 12):
        raise SystemExit("hosted negative-finalizer gate requires Python 3.12")
    required = {
        "GITHUB_ACTIONS": "true",
        "RUNNER_ENVIRONMENT": "github-hosted",
        "RUN_ENDPOINT_NEGATIVE_GATE": "1",
    }
    for name, expected in required.items():
        if os.environ.get(name) != expected:
            raise SystemExit(f"hosted negative-finalizer gate requires {name}={expected}")
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


class HostedNegativeFinalizerTests(unittest.TestCase):
    @staticmethod
    def _stem(identity):
        return ("fs-residual-endpoint-01.v1-r1." + identity.source_commit + "." +
                identity.host + "." + identity.github_run_id + "." +
                identity.github_run_attempt)

    def _replace_exact(self, data, old, new, expected_count):
        self.assertIs(type(data), bytes)
        self.assertEqual(data.count(old), expected_count,
                         "frozen synthetic fixture token changed")
        result = data.replace(old, new)
        self.assertEqual(result.count(new), expected_count,
                         "canonical mutation count changed")
        self.assertTrue(result.isascii())
        return result

    def _run_case(self, identity, sidecar_bytes, primary_bytes, *, reason,
                  evidence_state, observer_disposition):
        self.assertTrue(sidecar_bytes.endswith(b"\n"))
        self.assertNotIn(b"\r", sidecar_bytes)
        self.assertTrue(sidecar_bytes.isascii())
        self.assertTrue(primary_bytes.isascii())
        if identity.host == "windows":
            self.assertEqual(primary_bytes.count(b"\n"), primary_bytes.count(b"\r\n"))
        else:
            self.assertNotIn(b"\r", primary_bytes)

        stem = self._stem(identity)
        runner_temp = Path(os.environ["RUNNER_TEMP"]).resolve(strict=True)
        with tempfile.TemporaryDirectory(
                prefix="fs-endpoint-synthetic-", dir=runner_temp) as temporary:
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
            self.assertEqual(
                tuple(path.name for path in selected),
                (stem + ".status.json",),
            )
            self.assertTrue(all(path.parent == published_parent / stem for path in selected))
            status = decode_status(
                selected[0].read_bytes(),
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
                    "boost_version", "ctest_exit_code", "canonical_bytes",
                    "canonical_sha256", "gzip_bytes", "gzip_sha256", "gzip_filename",
                    "status_filename",
                )},
                {
                    "evidence_state": evidence_state,
                    "reason": reason,
                    "observer_disposition": observer_disposition,
                    "packer_disposition": "FAIL",
                    "chain_count": None,
                    "row_count": 0,
                    "numeric_gate_failures": 2,
                    "E80_disposition": "FAIL",
                    "A_disposition": "NOT_ADOPTED",
                    "boost_version": 108300,
                    "ctest_exit_code": 8,
                    "canonical_bytes": None,
                    "canonical_sha256": None,
                    "gzip_bytes": None,
                    "gzip_sha256": None,
                    "gzip_filename": None,
                    "status_filename": stem + ".status.json",
                },
            )

    def test_primary_sidecar_count_mismatch_is_integrity_status_only(self):
        identity = _hosted_identity()
        sidecar_bytes, primary_bytes = complete_fixture._matching_full_inputs(identity)
        sidecar_bytes = self._replace_exact(
            sidecar_bytes,
            b"meta\tnumeric_gate_failures\t2\n",
            b"meta\tnumeric_gate_failures\t3\n",
            1,
        )
        self._run_case(
            identity, sidecar_bytes, primary_bytes,
            reason="INTEGRITY", evidence_state="FATAL", observer_disposition="FAIL")

    def test_reconstructed_fresh_disk_excess_is_conditioning_status_only(self):
        identity = _hosted_identity()
        sidecar_bytes, primary_bytes = complete_fixture._matching_full_inputs(identity)
        old_row = ("\n0\t" + "\t".join((ZERO,) * 4) + "\n").encode("ascii")
        new_row = ("\n0\t" + "\t".join((CONDITIONING_E0_REAL, ZERO, ZERO, ZERO)) +
                   "\n").encode("ascii")
        sidecar_bytes = self._replace_exact(sidecar_bytes, old_row, new_row, 1)
        self._run_case(
            identity, sidecar_bytes, primary_bytes,
            reason="CONDITIONING", evidence_state="UNRESOLVED",
            observer_disposition="UNRESOLVED")

    def test_primary_signed_a8_imag_mismatch_is_integrity_status_only(self):
        identity = _hosted_identity()
        sidecar_bytes, primary_bytes = complete_fixture._matching_full_inputs(identity)
        old = (f"A8.imag={ZERO}\tA8.imag_q_num=0\tA8.imag_q_den=1").encode("ascii")
        new = (f"A8.imag={MISMATCHED_A8_IMAG}\tA8.imag_q_num=1\t"
               f"A8.imag_q_den={MISMATCHED_A8_IMAG_Q_DEN}").encode("ascii")
        primary_bytes = self._replace_exact(primary_bytes, old, new, 4)
        self.assertEqual(len(MISMATCHED_A8_IMAG_Q_DEN), 190)
        self.assertEqual(MISMATCHED_A8_IMAG_Q_DEN, str(2 * 10 ** 189))
        self._run_case(
            identity, sidecar_bytes, primary_bytes,
            reason="INTEGRITY", evidence_state="FATAL", observer_disposition="FAIL")


if __name__ == "__main__":
    unittest.main(verbosity=2)
