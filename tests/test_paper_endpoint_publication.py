"""Disposable synthetic publication tests; no live evidence or upload."""
import hashlib
import os
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from paper_endpoint_gzip import MAX_GZIP_BYTES, encode_gzip
from paper_endpoint_status import MAX_STATUS_BYTES, decode_status, encode_status
from paper_endpoint_publication import (
    PublicationError,
    PublicationIdentity,
    publish_endpoint_evidence,
    select_endpoint_uploads,
)


IDENTITY = PublicationIdentity(source_commit="1" * 40, host="linux",
                               github_run_id="42", github_run_attempt="1")
STEM = "fs-residual-endpoint-01.v1-r1." + "1" * 40 + ".linux.42.1"


def complete_record(canonical, compressed, *, failures=9, ctest_exit=8):
    return dict(
        schema="fs-residual-endpoint-status-v1-r1", source_commit="1" * 40,
        baseline_tested_source="9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e",
        production_source="b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89",
        openfhe_pin="df495ba2e91739a6dc8f1de254fc5a41155ce504",
        host="linux", github_run_id="42", github_run_attempt="1",
        test_name="paper_full_eight_square_contract", chain_count=1,
        evidence_state="COMPLETE", reason="NONE", assurance="CONDITIONAL",
        model="conditional-binary-nearest-direct-trig8u-v1", boost_version=108300,
        E80_disposition="FAIL" if failures else "PASS", A_disposition="NOT_ADOPTED",
        observer_disposition="PASS", numeric_gate_failures=failures, row_count=16384,
        canonical_bytes=len(canonical), canonical_sha256=hashlib.sha256(canonical).hexdigest(),
        gzip_bytes=len(compressed), gzip_sha256=hashlib.sha256(compressed).hexdigest(),
        gzip_filename=STEM + ".tsv.gz", status_filename=STEM + ".status.json",
        ctest_exit_code=ctest_exit, exit_code_convention="shell-status",
        packer_disposition="PASS")


def incomplete_record(reason="IO_ERROR", *, ctest_exit=8, failures=9):
    record = complete_record(b"x", encode_gzip(b"x"), failures=failures,
                             ctest_exit=ctest_exit)
    record.update(evidence_state="FATAL", reason=reason, observer_disposition="FAIL",
                  packer_disposition="FAIL", chain_count=None, row_count=0,
                  canonical_bytes=None, canonical_sha256=None, gzip_bytes=None,
                  gzip_sha256=None, gzip_filename=None, boost_version=None,
                  E80_disposition=("NOT_OBSERVED" if failures is None else
                                   ("FAIL" if failures else "PASS")))
    return record


class PublicationTests(unittest.TestCase):
    def test_complete_failing_e80_is_bound_and_selected_exactly(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-complete\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            root_path = Path(root).resolve()
            parent = root_path / "published"
            parent.mkdir()
            result = publish_endpoint_evidence(
                parent, IDENTITY, status, canonical_bytes=canonical,
                gzip_bytes=compressed)
            expected_dir = parent / STEM
            expected = (expected_dir / (STEM + ".tsv.gz"),
                        expected_dir / (STEM + ".status.json"))
            self.assertEqual(result.upload_paths, expected)
            self.assertEqual(result.required_exit_code, 8)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY), expected)
            self.assertEqual(expected[0].read_bytes(), compressed)

    def test_incomplete_publication_is_status_only_and_requires_failure_exit(self):
        status = incomplete_record("INTEGRITY", ctest_exit=0, failures=9)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            result = publish_endpoint_evidence(
                parent, IDENTITY, status, canonical_bytes=None, gzip_bytes=None)
            status_path = parent / STEM / (STEM + ".status.json")
            self.assertEqual(result.upload_paths, (status_path,))
            self.assertEqual(result.required_exit_code, 1)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY), (status_path,))
            self.assertEqual([path.name for path in (parent / STEM).iterdir()],
                             [STEM + ".status.json"])

    def test_incomplete_partial_status_creation_cleans_only_owned_paths(self):
        status = incomplete_record("IO_ERROR", ctest_exit=8, failures=9)
        original_open = Path.open
        injected = False

        class PartialWrite:
            def __init__(self, stream):
                self.stream = stream

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc_value, traceback):
                self.stream.close()
                return False

            def write(self, data):
                self.stream.write(data[:1])
                self.stream.flush()
                raise OSError("synthetic incomplete partial status write")

        def interrupt_status_write(path, mode="r", *args, **kwargs):
            nonlocal injected
            stream = original_open(path, mode, *args, **kwargs)
            if (not injected and mode == "xb" and
                    path.name.endswith(".candidate.status.json")):
                injected = True
                return PartialWrite(stream)
            return stream

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with mock.patch.object(Path, "open", interrupt_status_write):
                with self.assertRaises(PublicationError) as caught:
                    publish_endpoint_evidence(
                        parent, IDENTITY, status, canonical_bytes=None, gzip_bytes=None)
            self.assertEqual(caught.exception.required_exit_code, 8)
            self.assertEqual(caught.exception.first_cause,
                             "synthetic incomplete partial status write")
            self.assertIsNone(caught.exception.fallback_status_path)
            self.assertFalse((parent / STEM).exists())

    def test_status_last_interruption_preserves_observed_facts_in_io_fallback(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-interruption\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed, failures=0, ctest_exit=0)
        original_rename = Path.rename
        interrupted = False

        def interrupt_first_status_rename(path, target):
            nonlocal interrupted
            if path.name.endswith(".candidate.status.json") and not interrupted:
                interrupted = True
                raise OSError("synthetic status commit interruption")
            return original_rename(path, target)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with mock.patch.object(Path, "rename", interrupt_first_status_rename):
                with self.assertRaises(PublicationError) as caught:
                    publish_endpoint_evidence(
                        parent, IDENTITY, status, canonical_bytes=canonical,
                        gzip_bytes=compressed)
            status_path = parent / STEM / (STEM + ".status.json")
            self.assertEqual(caught.exception.required_exit_code, 1)
            self.assertEqual(caught.exception.fallback_status_path, status_path)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY), (status_path,))
            decoded = decode_status(
                status_path.read_bytes(), expected_source_commit="1" * 40,
                expected_host="linux", expected_run_id="42", expected_run_attempt="1")
            self.assertEqual(decoded["reason"], "IO_ERROR")
            self.assertEqual(decoded["ctest_exit_code"], 0)
            self.assertEqual(decoded["numeric_gate_failures"], 0)
            self.assertEqual(decoded["E80_disposition"], "PASS")
            self.assertEqual(decoded["boost_version"], 108300)
            self.assertEqual({path.name for path in (parent / STEM).iterdir()},
                             {STEM + ".status.json"})

    def test_status_commit_waits_until_owned_staging_cleanup_succeeds(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-status-last\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed, failures=0, ctest_exit=0)
        original_rmdir = Path.rmdir
        interrupted = False

        def interrupt_first_staging_cleanup(path):
            nonlocal interrupted
            if path.name == ".staging" and not interrupted:
                interrupted = True
                raise OSError("synthetic staging cleanup interruption")
            return original_rmdir(path)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with mock.patch.object(Path, "rmdir", interrupt_first_staging_cleanup):
                with self.assertRaises(PublicationError) as caught:
                    publish_endpoint_evidence(
                        parent, IDENTITY, status, canonical_bytes=canonical,
                        gzip_bytes=compressed)
            status_path = parent / STEM / (STEM + ".status.json")
            self.assertEqual(caught.exception.fallback_status_path, status_path)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY), (status_path,))
            decoded = decode_status(
                status_path.read_bytes(), expected_source_commit="1" * 40,
                expected_host="linux", expected_run_id="42", expected_run_attempt="1")
            self.assertEqual(decoded["reason"], "IO_ERROR")
            self.assertEqual(decoded["ctest_exit_code"], 0)
            self.assertEqual(decoded["numeric_gate_failures"], 0)
            self.assertEqual(decoded["E80_disposition"], "PASS")

    def test_reopened_status_must_equal_the_validated_status_bytes(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-status-integrity\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed, failures=9, ctest_exit=8)
        altered = dict(status, numeric_gate_failures=8)
        altered_bytes = encode_status(
            altered, expected_source_commit="1" * 40, expected_host="linux",
            expected_run_id="42", expected_run_attempt="1")
        original_open = Path.open
        altered_once = False

        def alter_first_candidate_status_read(path, mode="r", *args, **kwargs):
            nonlocal altered_once
            if (mode == "rb" and path.name.endswith(".candidate.status.json") and
                    not altered_once):
                altered_once = True
                with original_open(path, "wb") as stream:
                    stream.write(altered_bytes)
            return original_open(path, mode, *args, **kwargs)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with mock.patch.object(Path, "open", alter_first_candidate_status_read):
                with self.assertRaises(PublicationError) as caught:
                    publish_endpoint_evidence(
                        parent, IDENTITY, status, canonical_bytes=canonical,
                        gzip_bytes=compressed)
            fallback_path = parent / STEM / (STEM + ".status.json")
            self.assertEqual(caught.exception.fallback_status_path, fallback_path)
            decoded = decode_status(
                fallback_path.read_bytes(), expected_source_commit="1" * 40,
                expected_host="linux", expected_run_id="42", expected_run_attempt="1")
            self.assertEqual(decoded["reason"], "INTEGRITY")
            self.assertEqual(decoded["numeric_gate_failures"], 9)
            self.assertEqual(decoded["E80_disposition"], "FAIL")
            self.assertEqual(decoded["boost_version"], 108300)
            self.assertEqual(decoded["ctest_exit_code"], 8)

    def test_complete_gzip_preflight_failure_commits_truthful_incomplete_status(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-preflight-fallback\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed, failures=9, ctest_exit=8)
        status["gzip_sha256"] = "0" * 64
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with self.assertRaises(PublicationError) as caught:
                publish_endpoint_evidence(
                    parent, IDENTITY, status, canonical_bytes=canonical,
                    gzip_bytes=compressed)
            fallback_path = parent / STEM / (STEM + ".status.json")
            self.assertEqual(caught.exception.fallback_status_path, fallback_path)
            self.assertEqual(caught.exception.required_exit_code, 8)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY), (fallback_path,))
            decoded = decode_status(
                fallback_path.read_bytes(), expected_source_commit="1" * 40,
                expected_host="linux", expected_run_id="42", expected_run_attempt="1")
            self.assertEqual(decoded["reason"], "INTEGRITY")
            self.assertEqual(decoded["ctest_exit_code"], 8)
            self.assertEqual(decoded["numeric_gate_failures"], 9)
            self.assertEqual(decoded["E80_disposition"], "FAIL")
            self.assertEqual(decoded["boost_version"], 108300)

    def test_partial_stage_write_or_read_is_owned_cleaned_and_falls_back(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-partial-io\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed, failures=0, ctest_exit=0)
        original_open = Path.open

        class FaultingStream:
            def __init__(self, stream, operation, message):
                self.stream = stream
                self.operation = operation
                self.message = message

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc_value, traceback):
                self.stream.close()
                return False

            def write(self, data):
                if self.operation != "write":
                    return self.stream.write(data)
                self.stream.write(data[:max(1, len(data) // 2)])
                self.stream.flush()
                raise OSError(self.message)

            def read(self, *args):
                if self.operation == "read":
                    raise OSError(self.message)
                return self.stream.read(*args)

        for phase, suffix, mode in (
                ("gzip write", ".candidate.tsv.gz", "xb"),
                ("status write", ".candidate.status.json", "xb"),
                ("gzip read", ".candidate.tsv.gz", "rb"),
                ("status read", ".candidate.status.json", "rb")):
            with self.subTest(phase=phase), tempfile.TemporaryDirectory(
                    prefix="fs-endpoint-synthetic-publication-") as root:
                parent = Path(root).resolve() / "published"
                parent.mkdir()
                injected = False
                message = "synthetic " + phase + " fault"

                def inject_once(path, opened_mode="r", *args, **kwargs):
                    nonlocal injected
                    stream = original_open(path, opened_mode, *args, **kwargs)
                    if not injected and opened_mode == mode and path.name.endswith(suffix):
                        injected = True
                        operation = "write" if mode == "xb" else "read"
                        return FaultingStream(stream, operation, message)
                    return stream

                with mock.patch.object(Path, "open", inject_once):
                    with self.assertRaises(PublicationError) as caught:
                        publish_endpoint_evidence(
                            parent, IDENTITY, status, canonical_bytes=canonical,
                            gzip_bytes=compressed)
                fallback_path = parent / STEM / (STEM + ".status.json")
                self.assertEqual(caught.exception.fallback_status_path, fallback_path)
                self.assertEqual(caught.exception.first_cause, message)
                self.assertEqual(select_endpoint_uploads(parent, IDENTITY), (fallback_path,))
                decoded = decode_status(
                    fallback_path.read_bytes(), expected_source_commit="1" * 40,
                    expected_host="linux", expected_run_id="42",
                    expected_run_attempt="1")
                self.assertEqual(decoded["reason"], "IO_ERROR")
                self.assertEqual(decoded["numeric_gate_failures"], 0)
                self.assertEqual(decoded["E80_disposition"], "PASS")
                self.assertEqual(decoded["boost_version"], 108300)

    def test_fallback_or_cleanup_failure_remains_failure_with_first_cause(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-double-failure\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed, failures=0, ctest_exit=0)
        original_rename = Path.rename

        def reject_status_renames(path, target):
            if path.name.endswith(".candidate.status.json"):
                raise OSError("synthetic status storage unavailable")
            return original_rename(path, target)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            root_path = Path(root).resolve()
            parent = root_path / "published"
            parent.mkdir()
            with mock.patch.object(Path, "rename", reject_status_renames):
                with self.assertRaises(PublicationError) as caught:
                    publish_endpoint_evidence(
                        parent, IDENTITY, status, canonical_bytes=canonical,
                        gzip_bytes=compressed)
            self.assertIsNone(caught.exception.fallback_status_path)
            self.assertEqual(caught.exception.required_exit_code, 1)
            self.assertEqual(caught.exception.first_cause,
                             "synthetic status storage unavailable")
            with self.assertRaises(PublicationError):
                select_endpoint_uploads(parent, IDENTITY)

        interrupted = False

        def interrupt_status_once(path, target):
            nonlocal interrupted
            if path.name.endswith(".candidate.status.json") and not interrupted:
                interrupted = True
                raise OSError("synthetic first commit failure")
            return original_rename(path, target)

        original_unlink = Path.unlink

        def reject_orphan_cleanup(path, *args, **kwargs):
            if path.name.endswith(".tsv.gz") and not path.name.endswith(".candidate.tsv.gz"):
                raise OSError("synthetic orphan cleanup failure")
            return original_unlink(path, *args, **kwargs)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with mock.patch.object(Path, "rename", interrupt_status_once), \
                    mock.patch.object(Path, "unlink", reject_orphan_cleanup):
                with self.assertRaises(PublicationError) as caught:
                    publish_endpoint_evidence(
                        parent, IDENTITY, status, canonical_bytes=canonical,
                        gzip_bytes=compressed)
            self.assertIsNone(caught.exception.fallback_status_path)
            self.assertEqual(caught.exception.first_cause,
                             "synthetic first commit failure")

    def test_fallback_partial_status_creation_cleans_owned_paths_and_keeps_first_cause(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-partial-fallback\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed, failures=0, ctest_exit=0)
        original_open = Path.open
        original_rmdir = Path.rmdir
        status_writes = 0
        interrupted = False

        class PartialWrite:
            def __init__(self, stream):
                self.stream = stream

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc_value, traceback):
                self.stream.close()
                return False

            def write(self, data):
                self.stream.write(data[:1])
                self.stream.flush()
                raise OSError("synthetic fallback partial status write")

        def fail_fallback_write(path, mode="r", *args, **kwargs):
            nonlocal status_writes
            stream = original_open(path, mode, *args, **kwargs)
            if mode == "xb" and path.name.endswith(".candidate.status.json"):
                status_writes += 1
                if status_writes == 2:
                    return PartialWrite(stream)
            return stream

        def trigger_fallback_before_commit(path):
            nonlocal interrupted
            if path.name == ".staging" and not interrupted:
                interrupted = True
                raise OSError("synthetic original staging cleanup failure")
            return original_rmdir(path)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with mock.patch.object(Path, "open", fail_fallback_write), \
                    mock.patch.object(Path, "rmdir", trigger_fallback_before_commit):
                with self.assertRaises(PublicationError) as caught:
                    publish_endpoint_evidence(
                        parent, IDENTITY, status, canonical_bytes=canonical,
                        gzip_bytes=compressed)
            self.assertIsNone(caught.exception.fallback_status_path)
            self.assertEqual(caught.exception.required_exit_code, 1)
            self.assertEqual(caught.exception.first_cause,
                             "synthetic original staging cleanup failure")
            self.assertFalse((parent / STEM).exists())

    def test_identity_paths_preexistence_and_payload_binding_fail_closed(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-security\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            root_path = Path(root).resolve()
            parent = root_path / "published"
            parent.mkdir()
            bad_identity = PublicationIdentity("1" * 40, "linux", 42, "1")
            with self.assertRaises(PublicationError):
                publish_endpoint_evidence(
                    parent, bad_identity, status, canonical_bytes=canonical,
                    gzip_bytes=compressed)

            existing = parent / STEM
            existing.mkdir()
            stale = existing / "fs-endpoint-synthetic-stale"
            stale.write_bytes(b"do-not-remove")
            with self.assertRaises(PublicationError):
                publish_endpoint_evidence(
                    parent, IDENTITY, status, canonical_bytes=canonical,
                    gzip_bytes=compressed)
            self.assertEqual(stale.read_bytes(), b"do-not-remove")

            real = root_path / "real-published"
            real.mkdir()
            nonnormalized = real / ".." / "real-published"
            with self.assertRaises(PublicationError):
                publish_endpoint_evidence(
                    nonnormalized, IDENTITY, status, canonical_bytes=canonical,
                    gzip_bytes=compressed)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            wrong_hash = dict(status, gzip_sha256="0" * 64)
            with self.assertRaises(PublicationError) as caught:
                publish_endpoint_evidence(
                    parent, IDENTITY, wrong_hash, canonical_bytes=canonical,
                    gzip_bytes=compressed)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY),
                             (caught.exception.fallback_status_path,))

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with self.assertRaises(PublicationError) as caught:
                publish_endpoint_evidence(
                    parent, IDENTITY, status, canonical_bytes=b"x" * len(canonical),
                    gzip_bytes=compressed)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY),
                             (caught.exception.fallback_status_path,))

    def test_selector_rejects_orphans_mutation_and_entries_outside_allowlist(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-selection\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            result = publish_endpoint_evidence(
                parent, IDENTITY, status, canonical_bytes=canonical,
                gzip_bytes=compressed)
            sibling = parent / "fs-endpoint-synthetic-foreign.tmp"
            sibling.write_bytes(b"not selected")
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY), result.upload_paths)
            identity_dir = parent / STEM
            extra = identity_dir / "fs-endpoint-synthetic-extra.tmp"
            extra.write_bytes(b"not allowed")
            with self.assertRaises(PublicationError):
                select_endpoint_uploads(parent, IDENTITY)
            extra.unlink()
            gzip_path = result.upload_paths[0]
            gzip_path.write_bytes(compressed[:-1] + bytes([compressed[-1] ^ 1]))
            with self.assertRaises(PublicationError):
                select_endpoint_uploads(parent, IDENTITY)

        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            identity_dir = parent / STEM
            identity_dir.mkdir()
            (identity_dir / (STEM + ".tsv.gz")).write_bytes(compressed)
            with self.assertRaises(PublicationError):
                select_endpoint_uploads(parent, IDENTITY)

    def test_selector_preflights_file_size_before_reading_payload(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-size-preflight\n"
        compressed = encode_gzip(canonical)
        status = complete_record(canonical, compressed)
        original_stat = Path.stat
        original_open = Path.open

        for suffix, maximum in ((".tsv.gz", MAX_GZIP_BYTES),
                                (".status.json", MAX_STATUS_BYTES)):
            with self.subTest(suffix=suffix), tempfile.TemporaryDirectory(
                    prefix="fs-endpoint-synthetic-publication-") as root:
                parent = Path(root).resolve() / "published"
                parent.mkdir()
                publish_endpoint_evidence(
                    parent, IDENTITY, status, canonical_bytes=canonical,
                    gzip_bytes=compressed)

                def report_oversized(path, *args, **kwargs):
                    result = original_stat(path, *args, **kwargs)
                    if path.name.endswith(suffix):
                        fields = list(result)
                        fields[6] = maximum + 1
                        return os.stat_result(fields)
                    return result

                def reject_oversized_open(path, mode="r", *args, **kwargs):
                    if mode == "rb" and path.name.endswith(suffix):
                        raise AssertionError("oversized file was opened before size rejection")
                    return original_open(path, mode, *args, **kwargs)

                with mock.patch.object(Path, "stat", report_oversized), \
                        mock.patch.object(Path, "open", reject_oversized_open):
                    with self.assertRaises(PublicationError):
                        select_endpoint_uploads(parent, IDENTITY)

    def test_mixed_identity_repeat_and_invalid_shapes_are_rejected(self):
        incomplete = incomplete_record("IO_ERROR", ctest_exit=8, failures=9)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            root_path = Path(root).resolve()
            parent = root_path / "published"
            parent.mkdir()
            first = publish_endpoint_evidence(
                parent, IDENTITY, incomplete, canonical_bytes=None, gzip_bytes=None)
            original_status = first.upload_paths[0].read_bytes()
            with self.assertRaises(PublicationError):
                publish_endpoint_evidence(
                    parent, IDENTITY, incomplete, canonical_bytes=None, gzip_bytes=None)
            self.assertEqual(first.upload_paths[0].read_bytes(), original_status)
            mixed = original_status.replace(("1" * 40).encode(), ("2" * 40).encode(), 1)
            first.upload_paths[0].write_bytes(mixed)
            with self.assertRaises(PublicationError):
                select_endpoint_uploads(parent, IDENTITY)

        canonical = b"scope=synthetic\tfs-endpoint-synthetic-invalid-shape\n"
        compressed = encode_gzip(canonical)
        complete = complete_record(canonical, compressed)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            parent = Path(root).resolve() / "published"
            parent.mkdir()
            with self.assertRaises(PublicationError):
                publish_endpoint_evidence(
                    parent, IDENTITY, incomplete, canonical_bytes=b"foreign",
                    gzip_bytes=None)
            self.assertFalse((parent / STEM).exists())
            with self.assertRaises(PublicationError) as caught:
                publish_endpoint_evidence(
                    parent, IDENTITY, complete, canonical_bytes=canonical,
                    gzip_bytes=None)
            fallback_path = parent / STEM / (STEM + ".status.json")
            self.assertEqual(caught.exception.fallback_status_path, fallback_path)
            self.assertEqual(select_endpoint_uploads(parent, IDENTITY), (fallback_path,))

    def test_symlink_parent_identity_and_status_leaf_are_rejected(self):
        canonical = b"scope=synthetic\tfs-endpoint-synthetic-symlink\n"
        compressed = encode_gzip(canonical)
        complete = complete_record(canonical, compressed)
        with tempfile.TemporaryDirectory(prefix="fs-endpoint-synthetic-publication-") as root:
            root_path = Path(root).resolve()
            real = root_path / "real-published"
            real.mkdir()
            linked = root_path / "linked-published"
            try:
                linked.symlink_to(real, target_is_directory=True)
            except (OSError, NotImplementedError) as error:
                self.skipTest("platform cannot create disposable symlinks: " + str(error))
            with self.assertRaises(PublicationError):
                publish_endpoint_evidence(
                    linked, IDENTITY, complete, canonical_bytes=canonical,
                    gzip_bytes=compressed)

            identity_dir = real / STEM
            foreign = root_path / "fs-endpoint-synthetic-foreign-directory"
            foreign.mkdir()
            identity_dir.symlink_to(foreign, target_is_directory=True)
            with self.assertRaises(PublicationError):
                publish_endpoint_evidence(
                    real, IDENTITY, complete, canonical_bytes=canonical,
                    gzip_bytes=compressed)
            self.assertEqual(list(foreign.iterdir()), [])

            identity_dir.unlink()
            identity_dir = real / STEM
            identity_dir.mkdir()
            external_status = root_path / "fs-endpoint-synthetic-foreign.status.json"
            external_status.write_bytes(encode_status(
                incomplete_record(), expected_source_commit="1" * 40,
                expected_host="linux", expected_run_id="42", expected_run_attempt="1"))
            (identity_dir / (STEM + ".status.json")).symlink_to(external_status)
            with self.assertRaises(PublicationError):
                select_endpoint_uploads(real, IDENTITY)


if __name__ == "__main__":
    unittest.main()
