"""Small real-filesystem finalizer tests; full scalar replay is hosted-only."""
from pathlib import Path
import errno
import os
import subprocess
import sys
import tempfile
import unittest
from dataclasses import replace
from contextlib import contextmanager
from unittest.mock import patch

from paper_endpoint_publication import PublicationIdentity, select_endpoint_uploads
from paper_endpoint_status import decode_status
import paper_endpoint_finalizer as finalizer
import test_paper_endpoint_primary_reader as primary_fixture


IDENTITY = PublicationIdentity("a" * 40, "linux", "42", "1")


class EndpointFinalizerTests(unittest.TestCase):
    def test_scratch_roles_cannot_be_swapped_or_renamed(self):
        for canonical_name, published_name, log_name in (
                ("published", "canonical", "primary.ctest.log"),
                ("canonical", "published", "other.log"),
                ("inputs", "outputs", "primary.ctest.log")):
            with self.subTest(roles=(canonical_name, published_name, log_name)), \
                    tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-roles-") as temp:
                root = Path(temp).resolve()
                canonical, published = root / canonical_name, root / published_name
                canonical.mkdir()
                published.mkdir()
                with self.assertRaises(ValueError):
                    finalizer.finalize_endpoint(
                        root / log_name, IDENTITY, ctest_exit_code=8,
                        capture_exit_code=0, expected_scope="synthetic",
                        canonical_parent=canonical, published_parent=published)
                self.assertEqual(list(canonical.iterdir()), [])
                self.assertEqual(list(published.iterdir()), [])

    def test_invalid_identity_is_rejected_before_any_output(self):
        invalid = (
            replace(IDENTITY, source_commit=None),
            replace(IDENTITY, host=[]),
            replace(IDENTITY, github_run_id=42),
            replace(IDENTITY, github_run_attempt="0"),
            replace(IDENTITY, github_run_id="1" * (256 * 1024 + 1)),
        )
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-identity-") as temp:
            root = Path(temp).resolve()
            canonical, published = root / "canonical", root / "published"
            canonical.mkdir()
            published.mkdir()
            for identity in invalid:
                with self.subTest(field_types=tuple(type(value).__name__ for value in
                                  identity.__dict__.values())), self.assertRaises(ValueError):
                    finalizer.finalize_endpoint(
                        root / "primary.ctest.log", identity, ctest_exit_code=8,
                        capture_exit_code=0, expected_scope="synthetic",
                        canonical_parent=canonical, published_parent=published)
                self.assertEqual(list(canonical.iterdir()), [])
                self.assertEqual(list(published.iterdir()), [])

    def finish_bytes(self, data, *, ctest_exit=8, capture_exit=0, timed_out=False,
                     canonical_data=None, symlink_canonical=False, foreign_sibling=False):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-finalizer-") as temp:
            root = Path(temp).resolve()
            canonical, published = root / "canonical", root / "published"
            canonical.mkdir()
            published.mkdir()
            stem = "fs-endpoint-synthetic-" + "a" * 40 + ".linux.42.1"
            if foreign_sibling:
                (canonical / "foreign.txt").write_bytes(b"unrelated input preserved\n")
            candidate_directory = canonical / stem
            if symlink_canonical:
                foreign = root / "foreign"
                foreign.mkdir()
                candidate_directory.symlink_to(foreign, target_is_directory=True)
            elif canonical_data is not None:
                candidate_directory.mkdir()
            if canonical_data is not None:
                (candidate_directory / (stem + ".tsv")).write_bytes(canonical_data)
            primary_log = root / "primary.ctest.log"
            primary_log.write_bytes(data)
            result = finalizer.finalize_endpoint(
                primary_log, IDENTITY, ctest_exit_code=ctest_exit,
                capture_exit_code=capture_exit, expected_scope="synthetic",
                canonical_parent=canonical, published_parent=published, timed_out=timed_out)
            self.assertEqual(result.upload_paths, select_endpoint_uploads(published, IDENTITY))
            self.assertEqual(len(result.upload_paths), 1)
            status = decode_status(
                result.upload_paths[0].read_bytes(), expected_source_commit="a" * 40,
                expected_host="linux", expected_run_id="42", expected_run_attempt="1")
            if foreign_sibling:
                self.assertEqual((canonical / "foreign.txt").read_bytes(),
                                 b"unrelated input preserved\n")
            if canonical_data is None and not symlink_canonical and not foreign_sibling:
                self.assertEqual(list(canonical.iterdir()), [])
            elif canonical_data is not None:
                self.assertEqual((candidate_directory / (stem + ".tsv")).read_bytes(), canonical_data)
            return result.required_exit_code, status

    @staticmethod
    def complete_bytes(count):
        return primary_fixture.ctest_log(primary_fixture.complete_lines(
            count, scope="synthetic")).replace(primary_fixture.SOURCE.encode(), b"a" * 40).replace(
                b"github_run_id=101\t", b"github_run_id=42\t").replace(
                b"github_run_attempt=2\t", b"github_run_attempt=1\t")

    def test_missing_primary_produces_truthful_status_only(self):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-finalizer-") as temp:
            root = Path(temp).resolve()
            canonical = root / "canonical"
            published = root / "published"
            canonical.mkdir()
            published.mkdir()
            result = finalizer.finalize_endpoint(
                root / "primary.ctest.log", IDENTITY, ctest_exit_code=8,
                capture_exit_code=0, expected_scope="synthetic",
                canonical_parent=canonical, published_parent=published)
            self.assertEqual(result.required_exit_code, 8)
            self.assertEqual(result.upload_paths, select_endpoint_uploads(published, IDENTITY))
            self.assertEqual(len(result.upload_paths), 1)
            status = decode_status(
                result.upload_paths[0].read_bytes(), expected_source_commit="a" * 40,
                expected_host="linux", expected_run_id="42", expected_run_attempt="1")
            self.assertEqual(status["reason"], "IO_ERROR")
            self.assertEqual(status["evidence_state"], "FATAL")
            self.assertEqual(status["ctest_exit_code"], 8)
            self.assertIsNone(status["numeric_gate_failures"])
            self.assertEqual(status["E80_disposition"], "NOT_OBSERVED")
            self.assertIsNone(status["boost_version"])
            self.assertIsNone(status["gzip_filename"])
            self.assertEqual(list(canonical.iterdir()), [])

    def test_empty_or_launch_failure_log_retains_observed_execution_cause(self):
        cases = (
            (b"", 0, 23, False, "IO_ERROR"),
            (b"", 137, 0, True, "TIMEOUT"),
            (b"", 8, 0, False, "CTEST_FATAL"),
            (b"ctest: command not found\n", 127, 0, False, "CTEST_FATAL"),
            (b"", 0, 0, False, "NO_CANONICAL"),
        )
        for data, ctest_exit, capture_exit, timeout, reason in cases:
            with self.subTest(ctest=ctest_exit, capture=capture_exit, timeout=timeout):
                exit_code, status = self.finish_bytes(
                    data, ctest_exit=ctest_exit, capture_exit=capture_exit, timed_out=timeout)
                self.assertNotEqual(exit_code, 0)
                self.assertEqual(status["reason"], reason)
                self.assertIsNone(status["numeric_gate_failures"])
                self.assertEqual(status["E80_disposition"], "NOT_OBSERVED")

    def test_missing_canonical_retains_complete_primary_e80_observation(self):
        exit_code, status = self.finish_bytes(self.complete_bytes(2))
        self.assertEqual(exit_code, 8)
        self.assertEqual(status["reason"], "NO_CANONICAL")
        self.assertEqual(status["evidence_state"], "MISSING")
        self.assertEqual(status["numeric_gate_failures"], 2)
        self.assertEqual(status["E80_disposition"], "FAIL")
        self.assertEqual(status["boost_version"], 108300)
        self.assertIsNone(status["chain_count"])

    def test_capture_failure_never_accepts_complete_looking_prefix(self):
        exit_code, status = self.finish_bytes(self.complete_bytes(0), ctest_exit=0, capture_exit=23)
        self.assertNotEqual(exit_code, 0)
        self.assertEqual(status["reason"], "IO_ERROR")
        self.assertEqual(status["ctest_exit_code"], 0)
        self.assertEqual(status["numeric_gate_failures"], 0)
        self.assertEqual(status["E80_disposition"], "PASS")
        self.assertEqual(status["boost_version"], 108300)
        self.assertIsNone(status["gzip_filename"])

    def test_late_parser_error_retains_only_validated_primary_facts(self):
        unknown = b"61: FS_ENDPOINT_UNKNOWN\tvalue=1\n"
        cases = (
            (self.complete_bytes(2) + unknown, 8, 2, "FAIL", 108300),
            (self.complete_bytes(0) + unknown, 0, 0, "PASS", 108300),
            (self.complete_bytes(2).split(b"61: FS_ENDPOINT_SCALE", 1)[0] + unknown,
             8, None, "NOT_OBSERVED", 108300),
            (unknown, 8, None, "NOT_OBSERVED", None),
        )
        for data, ctest_exit, count, e80, boost in cases:
            with self.subTest(count=count, boost=boost, exit=ctest_exit):
                exit_code, status = self.finish_bytes(data, ctest_exit=ctest_exit)
                self.assertNotEqual(exit_code, 0)
                self.assertEqual(status["reason"], "FORMAT")
                self.assertEqual(status["numeric_gate_failures"], count)
                self.assertEqual(status["E80_disposition"], e80)
                self.assertEqual(status["boost_version"], boost)
                self.assertIsNone(status["gzip_filename"])

    def test_explicit_earlier_failure_precedes_missing_and_capture(self):
        data = b"61: FS_ENDPOINT_FAILURE reason=NONFINITE detail=synthetic numeric failure\n"
        for capture in (0, 23):
            with self.subTest(capture=capture):
                exit_code, status = self.finish_bytes(data, capture_exit=capture)
                self.assertEqual(exit_code, 8)
                self.assertEqual(status["reason"], "NONFINITE")
                self.assertIsNone(status["numeric_gate_failures"])

    def test_observed_timeout_retains_complete_e80_without_complete_evidence(self):
        exit_code, status = self.finish_bytes(self.complete_bytes(0), ctest_exit=137, timed_out=True)
        self.assertEqual(exit_code, 137)
        self.assertEqual(status["reason"], "TIMEOUT")
        self.assertEqual(status["numeric_gate_failures"], 0)
        self.assertEqual(status["E80_disposition"], "PASS")
        self.assertEqual(status["evidence_state"], "FATAL")

    def test_malformed_canonical_is_format_failure_not_complete(self):
        exit_code, status = self.finish_bytes(
            self.complete_bytes(2), canonical_data=b"not a canonical sidecar\n")
        self.assertEqual(exit_code, 8)
        self.assertEqual(status["reason"], "FORMAT")
        self.assertEqual(status["numeric_gate_failures"], 2)
        self.assertEqual(status["E80_disposition"], "FAIL")

    def test_foreign_canonical_directory_is_not_followed(self):
        exit_code, status = self.finish_bytes(
            self.complete_bytes(2), canonical_data=b"untouched foreign file\n", symlink_canonical=True)
        self.assertEqual(exit_code, 8)
        self.assertEqual(status["reason"], "INTEGRITY")
        self.assertEqual(status["numeric_gate_failures"], 2)

    def test_foreign_canonical_parent_sibling_is_rejected_before_leaf_parsing(self):
        for canonical_data in (None, b"not parsed while foreign sibling exists\n"):
            with self.subTest(candidate_present=canonical_data is not None):
                exit_code, status = self.finish_bytes(
                    self.complete_bytes(2), canonical_data=canonical_data, foreign_sibling=True)
                self.assertEqual(exit_code, 8)
                self.assertEqual(status["reason"], "INTEGRITY")
                self.assertEqual(status["numeric_gate_failures"], 2)

    def test_canonical_lstat_io_failures_are_not_absence(self):
        original_lstat = os.lstat
        for error_number in (errno.EACCES, errno.EIO):
            def filesystem_lstat(path, *args, **kwargs):
                candidate = Path(path)
                if candidate.parent.name == "canonical" and candidate.name.startswith(
                        "fs-endpoint-synthetic-"):
                    raise OSError(error_number, "synthetic filesystem boundary failure")
                return original_lstat(path, *args, **kwargs)

            with self.subTest(errno=error_number), patch("os.lstat", side_effect=filesystem_lstat):
                exit_code, status = self.finish_bytes(
                    self.complete_bytes(2), canonical_data=b"unchanged private candidate\n")
            self.assertEqual(exit_code, 8)
            self.assertEqual(status["reason"], "IO_ERROR")
            self.assertEqual(status["numeric_gate_failures"], 2)

    def test_cli_finalizes_failure_then_selects_exact_status_manifest(self):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-finalizer-cli-") as temp:
            root = Path(temp).resolve()
            canonical, published = root / "canonical", root / "published"
            canonical.mkdir()
            published.mkdir()
            identity_args = ["--source-commit", "a" * 40, "--host", "linux",
                             "--github-run-id", "42", "--github-run-attempt", "1"]
            command = [sys.executable, "-B", str(Path(finalizer.__file__).resolve())]
            result = subprocess.run(command + ["finalize", *identity_args,
                "--primary-log", str(root / "primary.ctest.log"), "--ctest-exit-code", "8",
                "--capture-exit-code", "0", "--scope", "synthetic",
                "--canonical-parent", str(canonical), "--published-parent", str(published)],
                cwd=root, capture_output=True, timeout=10)
            self.assertEqual(result.returncode, 8, result.stderr.decode())
            paths = select_endpoint_uploads(published, IDENTITY)
            manifest = root / "upload-paths.txt"
            selection_command = command + ["select", *identity_args,
                "--published-parent", str(published), "--manifest", str(manifest)]
            selected = subprocess.run(selection_command, cwd=root, capture_output=True, timeout=10)
            self.assertEqual(selected.returncode, 0, selected.stderr.decode())
            expected = (paths[0].as_posix() + "\n").encode("utf-8")
            self.assertEqual(manifest.read_bytes(), expected)
            repeated = subprocess.run(selection_command, cwd=root, capture_output=True, timeout=10)
            self.assertNotEqual(repeated.returncode, 0)
            self.assertEqual(manifest.read_bytes(), expected)

    def test_select_rejects_same_length_manifest_write_corruption(self):
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-manifest-") as temp:
            root = Path(temp).resolve()
            canonical, published = root / "canonical", root / "published"
            canonical.mkdir()
            published.mkdir()
            finalizer.finalize_endpoint(
                root / "primary.ctest.log", IDENTITY, ctest_exit_code=8,
                capture_exit_code=0, expected_scope="synthetic",
                canonical_parent=canonical, published_parent=published)
            paths = select_endpoint_uploads(published, IDENTITY)
            original_status = paths[0].read_bytes()
            manifest = root / "upload-paths.txt"
            original_open = Path.open

            @contextmanager
            def filesystem_open(path, mode="r", *args, **kwargs):
                with original_open(path, mode, *args, **kwargs) as stream:
                    if path == manifest and mode == "xb":
                        class CorruptingWriter:
                            def write(self, data):
                                return stream.write(b"X" * len(data))
                        yield CorruptingWriter()
                    else:
                        yield stream

            with patch.object(Path, "open", filesystem_open), \
                    self.assertRaises(finalizer.FinalizationError) as caught:
                finalizer.main(["select", "--source-commit", "a" * 40, "--host", "linux",
                    "--github-run-id", "42", "--github-run-attempt", "1",
                    "--published-parent", str(published), "--manifest", str(manifest)])
            self.assertEqual(caught.exception.reason, "INTEGRITY")
            self.assertEqual(select_endpoint_uploads(published, IDENTITY), paths)
            self.assertEqual(paths[0].read_bytes(), original_status)


if __name__ == "__main__":
    unittest.main()
