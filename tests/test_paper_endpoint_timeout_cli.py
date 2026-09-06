"""Tiny public-CLI checks against retained real CTest timeout transcripts."""
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from paper_endpoint_status import decode_status
import test_paper_endpoint_primary_reader as primary_fixture


TESTS = Path(__file__).resolve().parent
SOURCE = "0e3b82bdf71a49f21b497f4ad27a69792e18159a"
RUN_ID = "34018316144"
TIMEOUT_RESULT = (b"1/1 Test #61: paper_full_eight_square_contract "
                  b"...***Timeout   2.00 sec\n")


class EndpointTimeoutCliTests(unittest.TestCase):
    def finalize_bytes(self, data, *, host="linux", ctest_exit=8, capture_exit=0):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-timeout-") as temp:
            root = Path(temp).resolve()
            canonical, published = root / "canonical", root / "published"
            canonical.mkdir()
            published.mkdir()
            primary = root / "primary.ctest.log"
            primary.write_bytes(data)
            result = subprocess.run([
                sys.executable, "-B", str(TESTS / "paper_endpoint_finalizer.py"), "finalize",
                "--source-commit", SOURCE, "--host", host,
                "--github-run-id", RUN_ID, "--github-run-attempt", "1",
                "--primary-log", str(primary), "--ctest-exit-code", str(ctest_exit),
                "--capture-exit-code", str(capture_exit), "--scope", "synthetic",
                "--canonical-parent", str(canonical), "--published-parent", str(published),
            ], cwd=root, capture_output=True, timeout=10)
            self.assertEqual(result.returncode, ctest_exit or 1, result.stderr.decode())
            self.assertEqual(result.stdout, b"")
            self.assertEqual(result.stderr, b"")
            self.assertEqual(primary.read_bytes(), data)
            self.assertEqual(list(canonical.iterdir()), [])
            stem = f"fs-residual-endpoint-01.v1-r1.{SOURCE}.{host}.{RUN_ID}.1"
            self.assertEqual(list(published.iterdir()), [published / stem])
            status_file = published / stem / (stem + ".status.json")
            self.assertEqual(list((published / stem).iterdir()), [status_file])
            status = decode_status(status_file.read_bytes(), expected_source_commit=SOURCE,
                                   expected_host=host, expected_run_id=RUN_ID,
                                   expected_run_attempt="1")
            self.assertEqual(status["ctest_exit_code"], ctest_exit)
            self.assertEqual(status["row_count"], 0)
            self.assertIsNone(status["chain_count"])
            self.assertIsNone(status["gzip_filename"])
            self.assertEqual(status["packer_disposition"], "FAIL")
            return status

    def test_incomplete_foreign_and_replayed_timeout_text_is_not_an_event(self):
        data = (TESTS / "fixtures" / "endpoint_ctest_timeout_linux.log").read_bytes()
        cases = {
            "exit-code-only": b"",
            "configured-timeout-only": b"61: Test timeout computed to be: 2\n",
            "wrong-test-number": data.replace(b"#61:", b"#62:"),
            "wrong-test-name": data.replace(b"paper_full_eight_square_contract",
                                            b"some_other_test"),
            "prefixed-child-text": b"".join(b"61: " + line for line in data.splitlines(True)),
            "unprefixed-replay-before-failed-footer": data + (
                b"The following tests FAILED:\n"
                b"\t 61 - paper_full_eight_square_contract (Failed)\n"
                b"Errors while running CTest\n"),
            "conflicting-footer": data.replace(b" (Timeout)\n", b" (Failed)\n"),
            "missing-result": data.replace(TIMEOUT_RESULT, b""),
            "unterminated-footer": data[:-1],
            "wrong-duration-grammar": data.replace(b"2.00 sec", b"2e0 sec"),
        }
        for label, transcript in cases.items():
            with self.subTest(case=label):
                self.assertEqual(self.finalize_bytes(transcript)["reason"], "CTEST_FATAL")

    def test_timeout_keeps_earlier_typed_cause_and_capture_failure(self):
        data = (TESTS / "fixtures" / "endpoint_ctest_timeout_linux.log").read_bytes()
        failure = b"61: FS_ENDPOINT_FAILURE reason=NONFINITE detail=observed before timeout\n"
        for capture in (0, 23):
            with self.subTest(capture=capture):
                self.assertEqual(self.finalize_bytes(failure + data,
                    capture_exit=capture)["reason"], "NONFINITE")
        self.assertEqual(self.finalize_bytes(data, capture_exit=23)["reason"], "IO_ERROR")
        self.assertEqual(self.finalize_bytes(data[:-20], capture_exit=23)["reason"], "IO_ERROR")

    def test_timeout_preserves_validated_e80_facts_but_never_complete(self):
        data = (TESTS / "fixtures" / "endpoint_ctest_timeout_linux.log").read_bytes()
        for count, exit_code, reason in ((2, 8, "TIMEOUT"), (0, 0, "INTEGRITY")):
            with self.subTest(count=count, ctest_exit=exit_code):
                complete = primary_fixture.ctest_log(primary_fixture.complete_lines(
                    count, scope="synthetic")).replace(primary_fixture.SOURCE.encode(),
                    SOURCE.encode()).replace(b"github_run_id=101\t",
                    b"github_run_id=" + RUN_ID.encode() + b"\t").replace(
                    b"github_run_attempt=2\t", b"github_run_attempt=1\t")
                transcript = data.replace(TIMEOUT_RESULT, complete + TIMEOUT_RESULT)
                status = self.finalize_bytes(transcript, ctest_exit=exit_code)
                self.assertEqual(status["reason"], reason)
                self.assertEqual(status["evidence_state"], "FATAL")
                self.assertEqual(status["numeric_gate_failures"], count)
                self.assertEqual(status["E80_disposition"], "FAIL" if count else "PASS")
                self.assertEqual(status["boost_version"], 108300)

    def test_duration_field_is_not_hard_coded_to_the_two_second_probe(self):
        data = (TESTS / "fixtures" / "endpoint_ctest_timeout_linux.log").read_bytes()
        self.assertEqual(self.finalize_bytes(data.replace(
            b"2.00 sec", b"1200.01 sec"))["reason"], "TIMEOUT")

    def test_timeout_transport_with_zero_exit_is_integrity_failure(self):
        data = (TESTS / "fixtures" / "endpoint_ctest_timeout_linux.log").read_bytes()
        status = self.finalize_bytes(data, ctest_exit=0)
        self.assertEqual(status["reason"], "INTEGRITY")
        self.assertEqual(status["evidence_state"], "FATAL")

    def test_duplicate_timeout_result_is_not_a_single_observation(self):
        data = (TESTS / "fixtures" / "endpoint_ctest_timeout_linux.log").read_bytes()
        duplicate = data.replace(TIMEOUT_RESULT, TIMEOUT_RESULT + TIMEOUT_RESULT)
        self.assertEqual(self.finalize_bytes(duplicate)["reason"], "CTEST_FATAL")

    def test_constructed_crlf_is_supported_only_in_windows_mode(self):
        data = (TESTS / "fixtures" / "endpoint_ctest_timeout_windows.log").read_bytes()
        crlf = data.replace(b"\n", b"\r\n")
        self.assertEqual(self.finalize_bytes(crlf, host="windows")["reason"], "TIMEOUT")
        self.assertEqual(self.finalize_bytes(crlf, host="linux")["reason"], "FORMAT")

    def test_observed_hosted_transcripts_become_timeout_via_public_cli(self):
        for host in ("linux", "windows"):
            with self.subTest(host=host):
                data = (TESTS / "fixtures" / f"endpoint_ctest_timeout_{host}.log").read_bytes()
                status = self.finalize_bytes(data, host=host)
                self.assertEqual(status["reason"], "TIMEOUT")
                self.assertEqual(status["evidence_state"], "FATAL")
                self.assertEqual(status["observer_disposition"], "NOT_OBSERVED")
                self.assertEqual(status["E80_disposition"], "NOT_OBSERVED")
                self.assertIsNone(status["numeric_gate_failures"])


if __name__ == "__main__":
    unittest.main()
