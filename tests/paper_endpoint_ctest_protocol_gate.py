"""Hosted-only RED probe for the real CTest timeout/wrapper/status protocol."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

from paper_endpoint_status import StatusError, decode_status


MAX_PRIMARY_BYTES = 1024 * 1024
MAX_PHYSICAL_LINES = 96
MAX_RENDERED_LINE_BYTES = 512
_HEX40 = re.compile(r"[0-9a-f]{40}", re.ASCII)
_POSITIVE = re.compile(r"[1-9][0-9]*", re.ASCII)


class GateError(RuntimeError):
    pass


def _require(condition, message):
    if not condition:
        raise GateError(message)


def _hosted_identity():
    _require(sys.version_info[:2] == (3, 12), "requires exact Python 3.12")
    _require(os.environ.get("GITHUB_ACTIONS") == "true", "requires GitHub Actions")
    _require(os.environ.get("RUNNER_ENVIRONMENT") == "github-hosted",
             "requires a GitHub-hosted runner")
    _require(os.environ.get("RUN_ENDPOINT_CTEST_PROTOCOL_GATE") == "1",
             "requires explicit protocol-gate opt in")
    host = os.environ.get("ENDPOINT_TEST_HOST")
    _require(host in ("linux", "windows"), "invalid expected host")
    _require((host == "windows") == (os.name == "nt"), "host/platform mismatch")
    source_commit = os.environ.get("GITHUB_SHA", "")
    run_id = os.environ.get("GITHUB_RUN_ID", "")
    run_attempt = os.environ.get("GITHUB_RUN_ATTEMPT", "")
    _require(_HEX40.fullmatch(source_commit) is not None, "invalid source identity")
    _require(_POSITIVE.fullmatch(run_id) is not None, "invalid run identity")
    _require(_POSITIVE.fullmatch(run_attempt) is not None, "invalid run attempt")
    return host, source_commit, run_id, run_attempt


def _existing_real_directory(path, label):
    _require(isinstance(path, Path) and path.is_absolute(), label + " must be absolute")
    try:
        _require(path.is_dir() and not path.is_symlink(), label + " must be a real directory")
        path.resolve(strict=True)
    except OSError as error:
        raise GateError(label + " cannot be resolved") from error
    return path


def _bounded_regular(path, maximum, label, *, allow_empty=False):
    _require(isinstance(path, Path) and path.is_absolute(), label + " must be absolute")
    try:
        _require(path.is_file() and not path.is_symlink(), label + " must be a regular file")
        size = path.stat().st_size
        _require((allow_empty or size > 0) and size <= maximum,
                 label + " exceeds its byte envelope")
        data = path.read_bytes()
    except OSError as error:
        raise GateError("cannot read " + label) from error
    _require(len(data) == size, label + " changed while reading")
    return data


def _verify_checkout(source_root, expected_commit):
    _existing_real_directory(source_root, "source root")
    command = ["git", "-C", os.fspath(source_root), "rev-parse", "HEAD"]
    try:
        result = subprocess.run(command, check=False, stdin=subprocess.DEVNULL,
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                                text=True, timeout=20)
    except (OSError, subprocess.SubprocessError) as error:
        raise GateError("cannot inspect source checkout") from error
    _require(result.returncode == 0 and result.stdout == expected_commit + "\n",
             "source checkout identity mismatch")
    _require(len(result.stderr) <= 4096, "unbounded checkout diagnostic")
    required = (source_root / "tests" / "run_paper_endpoint_once.sh",
                source_root / "tests" / "paper_endpoint_finalizer.py",
                source_root / "tests" / "paper_endpoint_ctest_protocol_gate.py")
    _require(all(path.is_file() and not path.is_symlink() for path in required),
             "source checkout lacks the exact protocol files")


def _new_child(path, label):
    _require(isinstance(path, Path) and path.is_absolute(), label + " must be absolute")
    _require(path.parent != path and path.parent.is_dir() and not path.parent.is_symlink(),
             label + " parent must already be a real directory")
    _require(not os.path.lexists(path), label + " must be fresh")


def _cmake_bracket(value):
    _require(type(value) is str and value and "\x00" not in value and "]=]" not in value,
             "path cannot be represented in the CMake fixture")
    return "[=[" + value + "]=]"


def prepare(args):
    host, source_commit, run_id, run_attempt = _hosted_identity()
    _verify_checkout(args.source_root, source_commit)
    for path, label in ((args.fixture_source, "fixture source"),
                        (args.fixture_build, "fixture build"),
                        (args.marker, "emitter marker"),
                        (args.scratch, "synthetic scratch")):
        _new_child(path, label)
    stem = ("fs-residual-endpoint-01.v1-r1." + source_commit + "." + host + "." +
            run_id + "." + run_attempt)
    deepest_status_candidate = (args.scratch / "published" / stem / ".staging" /
                                (stem + ".candidate.status.json"))
    if host == "windows":
        candidate_text = os.fspath(deepest_status_candidate)
        _require(not candidate_text.startswith("\\\\?\\") and len(candidate_text) <= 247,
                 "synthetic publisher candidate exceeds the conservative MAX_PATH boundary")
    _require(args.python_native.is_absolute() and args.python_native.is_file(),
             "native Python path must be an absolute file")
    _require(args.gate_script_native.is_absolute() and args.gate_script_native.is_file(),
             "native gate-script path must be an absolute file")
    _require(args.gate_script_native.resolve(strict=True) == Path(__file__).resolve(strict=True),
             "gate script path does not name this source")
    args.fixture_source.mkdir(mode=0o700)
    python_value = _cmake_bracket(os.fspath(args.python_native))
    script_value = _cmake_bracket(os.fspath(args.gate_script_native))
    marker_value = _cmake_bracket(os.fspath(args.marker))
    cmake = f"""cmake_minimum_required(VERSION 3.20)
project(endpoint_ctest_protocol NONE)
enable_testing()
foreach(index RANGE 1 60)
  add_test(NAME protocol_padding_${{index}} COMMAND \"${{CMAKE_COMMAND}}\" -E true)
endforeach()
add_test(NAME paper_full_eight_square_contract
  COMMAND {python_value} -B {script_value} emit-timeout --marker {marker_value})
set_tests_properties(paper_full_eight_square_contract PROPERTIES TIMEOUT 2)
""".encode("utf-8")
    cmake_path = args.fixture_source / "CMakeLists.txt"
    with cmake_path.open("xb") as stream:
        _require(stream.write(cmake) == len(cmake), "short CMake fixture write")
    _require(cmake_path.read_bytes() == cmake, "CMake fixture changed after close")
    print(json.dumps({"phase": "prepare", "host": host, "tests_registered": 61,
                      "timeout_seconds": 2,
                      "deepest_status_candidate_characters":
                          len(os.fspath(deepest_status_candidate))}, sort_keys=True))


def emit_timeout(args):
    host, _, run_id, run_attempt = _hosted_identity()
    canonical = os.environ.get("PAPER_ENDPOINT_CANONICAL_PARENT", "")
    _require(canonical and Path(canonical).is_absolute(),
             "wrapper did not supply a native canonical parent")
    _require(os.environ.get("PAPER_ENDPOINT_HOST") == host,
             "wrapper host environment mismatch")
    _require(os.environ.get("PAPER_ENDPOINT_RUN_ID") == run_id and
             os.environ.get("PAPER_ENDPOINT_RUN_ATTEMPT") == run_attempt,
             "wrapper run environment mismatch")
    _new_child(args.marker, "emitter marker")
    marker_data = (host + "\t" + run_id + "\t" + run_attempt + "\n").encode("ascii")
    with args.marker.open("xb") as stream:
        _require(stream.write(marker_data) == len(marker_data), "short emitter marker write")
    print("CTEST_PROTOCOL_SENTINEL host=" + host + " run=" + run_id +
          " attempt=" + run_attempt, flush=True)
    time.sleep(30)
    raise GateError("CTest failed to terminate the timeout emitter")


def _wrapper_status(path):
    data = _bounded_regular(path, 4, "wrapper exit receipt")
    _require(re.fullmatch(rb"(?:[1-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\n", data)
             is not None, "wrapper must retain an exact nonzero shell status")
    return int(data)


def inspect(args):
    host, _, run_id, run_attempt = _hosted_identity()
    status = _wrapper_status(args.wrapper_exit)
    expected_marker = (host + "\t" + run_id + "\t" + run_attempt + "\n").encode("ascii")
    _require(_bounded_regular(args.marker, 256, "emitter marker") == expected_marker,
             "emitter marker identity mismatch")
    data = _bounded_regular(args.primary_log, MAX_PRIMARY_BYTES, "primary CTest log",
                            allow_empty=False)
    _require(data.count(b"61: CTEST_PROTOCOL_SENTINEL") == 1,
             "real CTest #61 prefix/sentinel was not observed exactly once")
    _require(b"\x00" not in data, "primary CTest log contains NUL")
    lines = data.splitlines(keepends=True)
    _require(lines and len(lines) <= 4096, "primary CTest log physical-line envelope")
    if len(lines) <= MAX_PHYSICAL_LINES:
        selected = list(enumerate(lines, 1))
    else:
        selected = list(enumerate(lines[:64], 1))
        selected += list(enumerate(lines[-32:], len(lines) - 31))
    rendered = [{"line": number, "bytes_repr": repr(line[:MAX_RENDERED_LINE_BYTES]),
                 "truncated": len(line) > MAX_RENDERED_LINE_BYTES}
                for number, line in selected]
    # Captured lines are untrusted evidence bytes. JSON/repr output never executes them.
    print(json.dumps({"phase": "inspect", "host": host, "wrapper_exit": status,
                      "primary_bytes": len(data), "primary_sha256": hashlib.sha256(data).hexdigest(),
                      "crlf_count": data.count(b"\r\n"), "lf_count": data.count(b"\n"),
                      "line_count": len(lines), "escaped_lines": rendered}, sort_keys=True))


def _identity_paths(scratch, host, source_commit, run_id, run_attempt):
    stem = ("fs-residual-endpoint-01.v1-r1." + source_commit + "." + host + "." +
            run_id + "." + run_attempt)
    identity_dir = scratch / "published" / stem
    return stem, identity_dir, identity_dir / (stem + ".status.json")


def _validate_selection(args):
    host, source_commit, run_id, run_attempt = _hosted_identity()
    status_code = _wrapper_status(args.wrapper_exit)
    scratch = _existing_real_directory(args.scratch, "scratch root")
    stem, identity_dir, expected_status = _identity_paths(
        scratch, host, source_commit, run_id, run_attempt)
    manifest_data = _bounded_regular(args.manifest, 32768, "upload manifest")
    _require(b"\r" not in manifest_data and b"\x00" not in manifest_data and
             manifest_data.endswith(b"\n") and manifest_data.count(b"\n") == 1,
             "manifest must contain exactly one LF-terminated path")
    try:
        manifest_path = Path(manifest_data[:-1].decode("utf-8"))
    except UnicodeError as error:
        raise GateError("manifest path is not UTF-8") from error
    _require(manifest_path.is_absolute() and manifest_path == expected_status,
             "manifest is not the exact synthetic status path")
    _require(identity_dir.is_dir() and not identity_dir.is_symlink(),
             "published identity directory is absent or indirect")
    published_entries = list(identity_dir.iterdir())
    _require(published_entries == [expected_status],
             "published identity must contain status only")
    canonical = scratch / "canonical"
    _require(canonical.is_dir() and not canonical.is_symlink() and not list(canonical.iterdir()),
             "synthetic timeout must not retain canonical entries")
    status_data = _bounded_regular(expected_status, 256 * 1024, "selected status")
    identity = dict(expected_source_commit=source_commit, expected_host=host,
                    expected_run_id=run_id, expected_run_attempt=run_attempt)
    try:
        record = decode_status(status_data, **identity)
    except StatusError as error:
        raise GateError("selected status failed strict decoding") from error
    _require(record["evidence_state"] != "COMPLETE" and record["gzip_filename"] is None,
             "timeout transport must select incomplete status only")
    _require(record["ctest_exit_code"] == status_code,
             "status did not preserve the actual wrapper/CTest shell status")
    return stem, expected_status, status_data, record


def validate_selection(args):
    stem, _, status_data, record = _validate_selection(args)
    print(json.dumps({"phase": "validate-selection", "stem": stem,
                      "reason_before_strict_gate": record["reason"],
                      "status_sha256": hashlib.sha256(status_data).hexdigest()}, sort_keys=True))


def verify_timeout(args):
    stem, expected_status, status_data, record = _validate_selection(args)
    downloaded = _existing_real_directory(args.downloaded, "download directory")
    files = [path for path in downloaded.rglob("*") if path.is_file()]
    _require(len(files) == 1 and not files[0].is_symlink(),
             "downloaded artifact must contain exactly one regular file")
    _require(files[0].name == expected_status.name and
             _bounded_regular(files[0], 256 * 1024, "downloaded status") == status_data,
             "downloaded status differs from the selected status")
    _require(record["evidence_state"] == "FATAL" and record["reason"] == "TIMEOUT" and
             record["observer_disposition"] == "NOT_OBSERVED",
             "RED: finalizer did not classify the observed CTest timeout as TIMEOUT")
    print(json.dumps({"phase": "verify-timeout", "stem": stem, "reason": record["reason"],
                      "artifact_files": 1}, sort_keys=True))


def _parser():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    make = commands.add_parser("prepare")
    make.add_argument("--source-root", required=True, type=Path)
    make.add_argument("--fixture-source", required=True, type=Path)
    make.add_argument("--fixture-build", required=True, type=Path)
    make.add_argument("--marker", required=True, type=Path)
    make.add_argument("--scratch", required=True, type=Path)
    make.add_argument("--python-native", required=True, type=Path)
    make.add_argument("--gate-script-native", required=True, type=Path)
    emit = commands.add_parser("emit-timeout")
    emit.add_argument("--marker", required=True, type=Path)
    observe = commands.add_parser("inspect")
    observe.add_argument("--primary-log", required=True, type=Path)
    observe.add_argument("--marker", required=True, type=Path)
    observe.add_argument("--wrapper-exit", required=True, type=Path)
    for name in ("validate-selection", "verify-timeout"):
        command = commands.add_parser(name)
        command.add_argument("--scratch", required=True, type=Path)
        command.add_argument("--manifest", required=True, type=Path)
        command.add_argument("--wrapper-exit", required=True, type=Path)
        if name == "verify-timeout":
            command.add_argument("--downloaded", required=True, type=Path)
    return parser


def main(argv=None):
    args = _parser().parse_args(argv)
    operations = {"prepare": prepare, "emit-timeout": emit_timeout, "inspect": inspect,
                  "validate-selection": validate_selection, "verify-timeout": verify_timeout}
    try:
        operations[args.command](args)
        return 0
    except GateError as error:
        print("CTEST_PROTOCOL_GATE_FAILURE phase=" + args.command + " detail=" + str(error),
              file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
