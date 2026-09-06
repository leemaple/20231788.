"""Synthetic byte-codec tests; no live evidence or filesystem publication."""
import json
import unittest

from paper_endpoint_status import encode_status, decode_status, StatusError


IDENTITY = dict(expected_source_commit="1" * 40, expected_host="linux",
                expected_run_id="42", expected_run_attempt="1")
STEM = "fs-residual-endpoint-01.v1-r1." + "1" * 40 + ".linux.42.1"
CAUSES = {
    "MODEL_UNSUPPORTED": ("UNRESOLVED", "UNRESOLVED"),
    "ESTIMATOR_CEILING": ("UNRESOLVED", "UNRESOLVED"),
    "CONDITIONING": ("UNRESOLVED", "UNRESOLVED"),
    "NONFINITE": ("FATAL", "FAIL"), "INTEGRITY": ("FATAL", "FAIL"),
    "IDENTITY": ("FATAL", "FAIL"), "FORMAT": ("FATAL", "FAIL"),
    "REPLAY": ("FATAL", "FAIL"), "IO_ERROR": ("FATAL", "FAIL"),
    "CTEST_FATAL": ("FATAL", "NOT_OBSERVED"),
    "TIMEOUT": ("FATAL", "NOT_OBSERVED"),
    "NO_CANONICAL": ("MISSING", "NOT_OBSERVED"),
}


def complete_record():
    return dict(
        schema="fs-residual-endpoint-status-v1-r1", source_commit="1" * 40,
        baseline_tested_source="9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e",
        production_source="b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89",
        openfhe_pin="df495ba2e91739a6dc8f1de254fc5a41155ce504",
        host="linux", github_run_id="42", github_run_attempt="1",
        test_name="paper_full_eight_square_contract", chain_count=1,
        evidence_state="COMPLETE", reason="NONE", assurance="CONDITIONAL",
        model="conditional-binary-nearest-direct-trig8u-v1", boost_version=108300,
        E80_disposition="FAIL", A_disposition="NOT_ADOPTED",
        observer_disposition="PASS", numeric_gate_failures=9, row_count=16384,
        canonical_bytes=1000, canonical_sha256="2" * 64,
        gzip_bytes=100, gzip_sha256="3" * 64, gzip_filename=STEM + ".tsv.gz",
        status_filename=STEM + ".status.json", ctest_exit_code=8,
        exit_code_convention="shell-status", packer_disposition="PASS")


def incomplete_record(reason):
    record = complete_record()
    state, observer = CAUSES[reason]
    record.update(evidence_state=state, reason=reason, observer_disposition=observer,
                  packer_disposition="FAIL", chain_count=None, row_count=0,
                  canonical_bytes=None, canonical_sha256=None, gzip_bytes=None,
                  gzip_sha256=None, gzip_filename=None, numeric_gate_failures=None,
                  E80_disposition="NOT_OBSERVED", boost_version=None)
    return record


class StatusTests(unittest.TestCase):
    def test_complete_failing_e80_remains_nonzero_in_canonical_status(self):
        record = complete_record()
        expected = json.dumps(record, sort_keys=True, separators=(",", ":")).encode() + b"\n"
        data = encode_status(record, **IDENTITY)
        self.assertEqual(data, expected)
        self.assertEqual(decode_status(data, **IDENTITY), record)
        self.assertEqual(record["numeric_gate_failures"], 9)
        self.assertEqual(record["ctest_exit_code"], 8)

    def test_schema_types_identity_and_complete_consistency_are_validated(self):
        cases = [
            ("extra", 1), ("schema", "foreign"), ("source_commit", "4" * 40),
            ("host", "windows"), ("github_run_id", "0042"),
            ("github_run_attempt", 1), ("baseline_tested_source", "5" * 40),
            ("production_source", "5" * 40), ("openfhe_pin", "5" * 40),
            ("boost_version", True), ("boost_version", 0),
            ("chain_count", True), ("row_count", 16384.0),
            ("numeric_gate_failures", False), ("numeric_gate_failures", -1),
            ("ctest_exit_code", True), ("ctest_exit_code", -1),
            ("ctest_exit_code", 256), ("ctest_exit_code", 0),
            ("canonical_bytes", 0), ("canonical_bytes", 16 * 1024 * 1024 + 1),
            ("canonical_sha256", "A" * 64), ("gzip_bytes", 17),
            ("gzip_sha256", None), ("gzip_filename", "../foreign.tsv.gz"),
            ("status_filename", "foreign.status.json"), ("E80_disposition", "PASS"),
            ("A_disposition", "PASS"), ("observer_disposition", "FAIL"),
            ("reason", "NONE "), ("assurance", "PROVED"),
            ("model", "unknown"), ("test_name", "other"),
            ("exit_code_convention", "other"), ("packer_disposition", "FAIL"),
        ]
        for key, value in cases:
            with self.subTest(key=key, value=value):
                record = complete_record()
                record[key] = value
                with self.assertRaises(StatusError):
                    encode_status(record, **IDENTITY)
        record = complete_record()
        del record["reason"]
        with self.assertRaises(StatusError):
            encode_status(record, **IDENTITY)

    def test_complete_zero_failure_record_requires_zero_exit(self):
        record = complete_record()
        record.update(numeric_gate_failures=0, E80_disposition="PASS", ctest_exit_code=0)
        self.assertEqual(decode_status(encode_status(record, **IDENTITY), **IDENTITY), record)
        record["ctest_exit_code"] = 8
        with self.assertRaises(StatusError):
            encode_status(record, **IDENTITY)

    def test_each_incomplete_cause_retains_truthful_nulls_and_classification(self):
        for reason in CAUSES:
            with self.subTest(reason=reason):
                record = incomplete_record(reason)
                self.assertEqual(decode_status(encode_status(record, **IDENTITY), **IDENTITY), record)
        record = incomplete_record("IO_ERROR")
        record.update(numeric_gate_failures=9, E80_disposition="FAIL", boost_version=109200)
        self.assertEqual(decode_status(encode_status(record, **IDENTITY), **IDENTITY), record)
        record.update(numeric_gate_failures=0, E80_disposition="PASS", ctest_exit_code=0)
        self.assertEqual(decode_status(encode_status(record, **IDENTITY), **IDENTITY), record)

    def test_incomplete_cannot_claim_chain_or_publish_gzip_or_invent_e80(self):
        for key, value in [("chain_count", 1), ("row_count", 16384),
                           ("canonical_bytes", 1), ("canonical_sha256", "0" * 64),
                           ("gzip_bytes", 18), ("gzip_sha256", "0" * 64),
                           ("gzip_filename", STEM + ".tsv.gz"), ("reason", "unknown"),
                           ("evidence_state", "COMPLETE"), ("evidence_state", "UNRESOLVED"),
                           ("observer_disposition", "PASS"), ("packer_disposition", "PASS"),
                           ("E80_disposition", "FAIL"), ("numeric_gate_failures", 0),
                           ("boost_version", False)]:
            with self.subTest(key=key, value=value):
                record = incomplete_record("NONFINITE")
                record[key] = value
                with self.assertRaises(StatusError):
                    encode_status(record, **IDENTITY)

    def test_decoder_requires_bounded_exact_canonical_ascii_json(self):
        record = complete_record()
        canonical = encode_status(record, **IDENTITY)
        variants = [canonical[:-1], canonical + b"\n", canonical.replace(b"\n", b"\r\n"),
                    b"\xef\xbb\xbf" + canonical, b" " + canonical,
                    json.dumps(record).encode() + b"\n", b"not json\n",
                    b"[]\n", b"null\n", b"{}\n", b"NaN\n",
                    canonical.replace(b'"chain_count":1', b'"chain_count":1.0'),
                    canonical.replace(b'"chain_count":1', b'"chain_count":true'),
                    canonical.replace(b'"chain_count":1', b'"chain_count":NaN'),
                    canonical.replace(b'"chain_count":1', b'"chain_count":1,"chain_count":1'),
                    canonical.replace(b'"reason":"NONE"', b'"reason":"N\\u004fNE"'),
                    canonical.replace(b'"reason":"NONE"', '"reason":"未知"'.encode()),
                    b" " * (256 * 1024 + 1), bytearray(canonical), canonical.decode()]
        for index, data in enumerate(variants):
            with self.subTest(index=index):
                with self.assertRaises(StatusError):
                    decode_status(data, **IDENTITY)

    def test_expected_identity_and_output_size_are_checked(self):
        for key, value in [("expected_source_commit", "A" * 40),
                           ("expected_host", "other"), ("expected_run_id", 42),
                           ("expected_run_id", "0"), ("expected_run_attempt", True)]:
            with self.subTest(key=key, value=value):
                arguments = dict(IDENTITY)
                arguments[key] = value
                with self.assertRaises(StatusError):
                    encode_status(complete_record(), **arguments)
        run_id = "1" * 100000
        stem = "fs-residual-endpoint-01.v1-r1." + "1" * 40 + ".linux." + run_id + ".1"
        record = complete_record()
        record.update(github_run_id=run_id, gzip_filename=stem + ".tsv.gz",
                      status_filename=stem + ".status.json")
        with self.assertRaises(StatusError):
            encode_status(record, **{**IDENTITY, "expected_run_id": run_id})

    def test_incomplete_retains_observed_exit_conflicts_without_normalizing(self):
        record = incomplete_record("CTEST_FATAL")
        record.update(numeric_gate_failures=0, E80_disposition="PASS", ctest_exit_code=137)
        self.assertEqual(decode_status(encode_status(record, **IDENTITY), **IDENTITY), record)
        record = incomplete_record("INTEGRITY")
        record.update(numeric_gate_failures=9, E80_disposition="FAIL", ctest_exit_code=0)
        self.assertEqual(decode_status(encode_status(record, **IDENTITY), **IDENTITY), record)

    def test_nested_values_never_satisfy_flat_schema(self):
        for key in complete_record():
            for value in ({"nested": 1}, [1]):
                with self.subTest(key=key, value=value):
                    record = complete_record()
                    record[key] = value
                    data = json.dumps(record, sort_keys=True, separators=(",", ":")).encode() + b"\n"
                    with self.assertRaises(StatusError):
                        decode_status(data, **IDENTITY)

    def test_exact_status_byte_ceiling_and_first_invalid_byte(self):
        limit = 256 * 1024
        record = complete_record()
        base_size = len(json.dumps(record, sort_keys=True, separators=(",", ":")).encode()) + 1
        extra_digits, remainder = divmod(limit - base_size, 3)
        run_id = "1" * (2 + extra_digits)
        stem = "fs-residual-endpoint-01.v1-r1." + "1" * 40 + ".linux." + run_id + ".1"
        record.update(github_run_id=run_id, gzip_filename=stem + ".tsv.gz",
                      status_filename=stem + ".status.json",
                      boost_version=108300 * 10 ** remainder)
        identity = {**IDENTITY, "expected_run_id": run_id}
        data = encode_status(record, **identity)
        self.assertEqual(len(data), limit)
        self.assertEqual(decode_status(data, **identity), record)
        record["boost_version"] *= 10
        oversized = json.dumps(record, sort_keys=True, separators=(",", ":")).encode() + b"\n"
        self.assertEqual(len(oversized), limit + 1)
        with self.assertRaises(StatusError):
            encode_status(record, **identity)
        with self.assertRaises(StatusError):
            decode_status(oversized, **identity)

    def test_scalar_envelope_precedes_identity_stem_and_json_allocation(self):
        with self.assertRaisesRegex(StatusError, "expected_run_id exceeds scalar byte limit"):
            encode_status(complete_record(), **{**IDENTITY, "expected_run_id": "1" * (256 * 1024 + 1)})
        record = complete_record()
        record["reason"] = "x" * (256 * 1024 + 1)
        with self.assertRaisesRegex(StatusError, "reason exceeds scalar byte limit"):
            encode_status(record, **IDENTITY)


if __name__ == "__main__":
    unittest.main()
