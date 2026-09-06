"""Tiny public-CLI canonical namespace checks; no full fixture or replay."""
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from paper_endpoint_status import decode_status
import test_paper_endpoint_primary_reader as primary_fixture


class EndpointCanonicalScopeCliTests(unittest.TestCase):
    def finalize_candidate(self, *, scope, prefix, host="linux"):
        source = primary_fixture.SOURCE
        suffix = f"{source}.{host}.101.2"
        canonical_stem = prefix + suffix
        published_stem = "fs-residual-endpoint-01.v1-r1." + suffix
        data = b"deliberately malformed tiny sidecar\n"
        primary = primary_fixture.ctest_log(primary_fixture.complete_lines(
            2, scope=scope, host=host))
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-scope-") as temp:
            root = Path(temp).resolve()
            canonical, published = root / "canonical", root / "published"
            canonical.mkdir()
            published.mkdir()
            candidate = canonical / canonical_stem
            candidate.mkdir()
            leaf = candidate / (canonical_stem + ".tsv")
            leaf.write_bytes(data)
            log = root / "primary.ctest.log"
            log.write_bytes(primary)
            result = subprocess.run([
                sys.executable, "-B",
                str(Path(__file__).resolve().with_name("paper_endpoint_finalizer.py")),
                "finalize", "--source-commit", source, "--host", host,
                "--github-run-id", "101", "--github-run-attempt", "2",
                "--primary-log", str(log), "--ctest-exit-code", "8",
                "--capture-exit-code", "0", "--scope", scope,
                "--canonical-parent", str(canonical), "--published-parent", str(published),
            ], cwd=root, capture_output=True, timeout=10)
            self.assertEqual(result.returncode, 8, result.stderr.decode())
            self.assertEqual(result.stdout, b"")
            self.assertEqual(result.stderr, b"")
            self.assertEqual(log.read_bytes(), primary)
            self.assertEqual(leaf.read_bytes(), data)
            self.assertEqual(list(canonical.iterdir()), [candidate])
            self.assertEqual(list(candidate.iterdir()), [leaf])
            target = published / published_stem
            status_file = target / (published_stem + ".status.json")
            self.assertEqual(list(published.iterdir()), [target])
            self.assertEqual(list(target.iterdir()), [status_file])
            status = decode_status(status_file.read_bytes(),
                expected_source_commit=source, expected_host=host,
                expected_run_id="101", expected_run_attempt="2")
            self.assertEqual(status["ctest_exit_code"], 8)
            self.assertEqual(status["numeric_gate_failures"], 2)
            self.assertEqual(status["E80_disposition"], "FAIL")
            self.assertEqual(status["row_count"], 0)
            self.assertIsNone(status["gzip_filename"])
            return status

    def test_writer_synthetic_namespace_reaches_sidecar_validation(self):
        for host in ("linux", "windows"):
            with self.subTest(host=host):
                status = self.finalize_candidate(scope="synthetic",
                    prefix="fs-endpoint-synthetic-", host=host)
                self.assertEqual(status["reason"], "FORMAT")

    def test_live_namespace_still_reaches_sidecar_validation(self):
        for host in ("linux", "windows"):
            with self.subTest(host=host):
                status = self.finalize_candidate(scope="live-single-chain",
                    prefix="fs-residual-endpoint-01.v1-r1.", host=host)
                self.assertEqual(status["reason"], "FORMAT")

    def test_other_scope_namespace_is_rejected_without_reading_leaf(self):
        for scope, prefix in (("synthetic", "fs-residual-endpoint-01.v1-r1."),
                              ("live-single-chain", "fs-endpoint-synthetic-")):
            for host in ("linux", "windows"):
                with self.subTest(scope=scope, host=host):
                    status = self.finalize_candidate(scope=scope, prefix=prefix, host=host)
                    self.assertEqual(status["reason"], "INTEGRITY")


if __name__ == "__main__":
    unittest.main()
