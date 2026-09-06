"""Pure canonical endpoint status byte codec; no publication or evidence claims."""
import json
import re


MAX_STATUS_BYTES = 256 * 1024
KEYS = frozenset((
    "schema", "source_commit", "baseline_tested_source", "production_source",
    "openfhe_pin", "host", "github_run_id", "github_run_attempt", "test_name",
    "chain_count", "evidence_state", "reason", "assurance", "model",
    "boost_version", "E80_disposition", "A_disposition", "observer_disposition",
    "numeric_gate_failures", "row_count", "canonical_bytes", "canonical_sha256",
    "gzip_bytes", "gzip_sha256", "gzip_filename", "status_filename",
    "ctest_exit_code", "exit_code_convention", "packer_disposition"))
FIXED = dict(schema="fs-residual-endpoint-status-v1-r1",
             baseline_tested_source="9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e",
             production_source="b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89",
             openfhe_pin="df495ba2e91739a6dc8f1de254fc5a41155ce504",
             test_name="paper_full_eight_square_contract", assurance="CONDITIONAL",
             model="conditional-binary-nearest-direct-trig8u-v1",
             A_disposition="NOT_ADOPTED", exit_code_convention="shell-status")
_HEX40 = re.compile(r"[0-9a-f]{40}", re.ASCII)
_HEX64 = re.compile(r"[0-9a-f]{64}", re.ASCII)
_POSITIVE = re.compile(r"[1-9][0-9]*", re.ASCII)
INCOMPLETE_CAUSES = {
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


class StatusError(ValueError):
    pass


def _require(condition, message):
    if not condition:
        raise StatusError(message)


def _integer(value, field, minimum=0, maximum=None):
    _require(type(value) is int and value >= minimum and
             (maximum is None or value <= maximum), "invalid integer: " + field)


def _validate(record, *, expected_source_commit, expected_host,
              expected_run_id, expected_run_attempt):
    _require(type(record) is dict and record.keys() == KEYS, "exact status schema keys")
    expected = dict(expected_source_commit=expected_source_commit, expected_host=expected_host,
                    expected_run_id=expected_run_id, expected_run_attempt=expected_run_attempt)
    # Inputs are already caller-owned. Bound every textual scalar before
    # derived stem concatenation or JSON output allocation; no narrower run-ID
    # semantic range is invented. The whole-object byte limit is checked later.
    for key, value in (*expected.items(), *record.items()):
        if type(value) is str:
            _require(len(value) <= MAX_STATUS_BYTES, key + " exceeds scalar byte limit")
    _require(type(expected_source_commit) is str and
             _HEX40.fullmatch(expected_source_commit) is not None, "expected source identity")
    _require(expected_host in ("linux", "windows"), "expected host identity")
    _require(type(expected_run_id) is str and _POSITIVE.fullmatch(expected_run_id) is not None,
             "expected run id must be positive decimal string")
    _require(type(expected_run_attempt) is str and
             _POSITIVE.fullmatch(expected_run_attempt) is not None,
             "expected attempt must be positive decimal string")
    identity = dict(source_commit=expected_source_commit, host=expected_host,
                    github_run_id=expected_run_id, github_run_attempt=expected_run_attempt)
    for key, value in {**FIXED, **identity}.items():
        _require(type(record[key]) is str and record[key] == value,
                 "status identity/value mismatch: " + key)
    stem = ("fs-residual-endpoint-01.v1-r1." + expected_source_commit + "." +
            expected_host + "." + expected_run_id + "." + expected_run_attempt)
    _require(record["status_filename"] == stem + ".status.json", "status filename identity")
    _integer(record["ctest_exit_code"], "ctest_exit_code", maximum=255)
    if record["evidence_state"] != "COMPLETE":
        _validate_incomplete(record)
        return
    _integer(record["boost_version"], "boost_version", minimum=1)
    _integer(record["chain_count"], "chain_count", minimum=1, maximum=1)
    _integer(record["row_count"], "row_count", minimum=16384, maximum=16384)
    _integer(record["numeric_gate_failures"], "numeric_gate_failures")
    for key, value in dict(evidence_state="COMPLETE", reason="NONE",
                           observer_disposition="PASS", packer_disposition="PASS").items():
        _require(record[key] == value, "complete disposition: " + key)
    e80 = "PASS" if record["numeric_gate_failures"] == 0 else "FAIL"
    _require(record["E80_disposition"] == e80, "E80 count mismatch")
    _require((record["ctest_exit_code"] == 0) == (e80 == "PASS"), "E80 shell status mismatch")
    _integer(record["canonical_bytes"], "canonical_bytes", 1, 16 * 1024 * 1024)
    _integer(record["gzip_bytes"], "gzip_bytes", 18, 32 * 1024 * 1024 + 1024)
    for key in ("canonical_sha256", "gzip_sha256"):
        _require(type(record[key]) is str and _HEX64.fullmatch(record[key]) is not None,
                 "invalid digest: " + key)
    _require(record["gzip_filename"] == stem + ".tsv.gz", "gzip filename identity")


def _validate_incomplete(record):
    reason = record["reason"]
    _require(type(reason) is str and reason in INCOMPLETE_CAUSES, "incomplete reason")
    state, observer = INCOMPLETE_CAUSES[reason]
    _require(record["evidence_state"] == state and record["observer_disposition"] == observer,
             "incomplete cause classification")
    _require(record["packer_disposition"] == "FAIL", "incomplete packer must fail")
    for key in ("chain_count", "canonical_bytes", "canonical_sha256", "gzip_bytes",
                "gzip_sha256", "gzip_filename"):
        _require(record[key] is None, "incomplete must have null " + key)
    _integer(record["row_count"], "row_count", maximum=0)
    if record["boost_version"] is not None:
        _integer(record["boost_version"], "boost_version", minimum=1)
    count = record["numeric_gate_failures"]
    if count is None:
        _require(record["E80_disposition"] == "NOT_OBSERVED", "unobserved numeric count")
    else:
        _integer(count, "numeric_gate_failures")
        _require(record["E80_disposition"] == ("PASS" if count == 0 else "FAIL"),
                 "retained E80 count mismatch")


def encode_status(record, **expected_identity):
    _validate(record, **expected_identity)
    try:
        data = json.dumps(record, sort_keys=True, separators=(",", ":"),
                          ensure_ascii=True, allow_nan=False).encode("ascii") + b"\n"
    except ValueError as error:
        raise StatusError("status integer exceeds the JSON runtime envelope") from error
    _require(len(data) <= MAX_STATUS_BYTES, "status byte limit")
    return data


def decode_status(data, **expected_identity):
    _require(type(data) is bytes and 0 < len(data) <= MAX_STATUS_BYTES,
             "status must be bounded immutable bytes")
    _require(data.isascii() and data.endswith(b"\n"), "status requires ASCII and final LF")
    try:
        record = json.loads(data)
    except (ValueError, RecursionError) as error:
        raise StatusError("invalid JSON or unsupported JSON numeric/depth envelope") from error
    # The validated schema is flat and permits only string/int/null values.
    # Exact re-encoding also rejects duplicate keys, alternate escapes, key
    # order, whitespace, trailing newlines and noncanonical integer spellings.
    _require(encode_status(record, **expected_identity) == data, "noncanonical status JSON")
    return record
