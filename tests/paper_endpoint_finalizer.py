"""Independent endpoint finalization; no production/FHE/transform imports."""
import argparse
import hashlib
import os
from pathlib import Path
import re
import stat
import sys

from paper_endpoint_gzip import GzipError, encode_gzip, verify_gzip
from paper_endpoint_primary_reader import (
    EMPTY_OBSERVATION, MAX_LOG_BYTES, PrimaryIdentity, PrimaryLogError,
    PrimaryObservation, parse_primary_log,
)
from paper_endpoint_publication import (
    PublicationError, PublicationIdentity, publish_endpoint_evidence, select_endpoint_uploads,
)
from paper_endpoint_reconcile import ReconcileError, reconcile_replay, validate_primary_binding
from paper_endpoint_sidecar_reader import MAX_BYTES, SidecarError, read_sidecar
from paper_endpoint_sidecar_replay import ReplayError, replay_sidecar
from paper_endpoint_status import FIXED, INCOMPLETE_CAUSES, MAX_STATUS_BYTES, encode_status


class FinalizationError(ValueError):
    def __init__(self, reason, detail):
        super().__init__(f"{reason}: {detail}")
        self.reason = reason
        self.detail = detail


def _status(identity, exit_code, reason, observation=EMPTY_OBSERVATION):
    state, observer = INCOMPLETE_CAUSES[reason]
    stem = ("fs-residual-endpoint-01.v1-r1." + identity.source_commit + "." +
            identity.host + "." + identity.github_run_id + "." + identity.github_run_attempt)
    count = observation.numeric_gate_failures
    boost = observation.boost_version
    return dict(FIXED, source_commit=identity.source_commit, host=identity.host,
                github_run_id=identity.github_run_id, github_run_attempt=identity.github_run_attempt,
                chain_count=None, evidence_state=state, reason=reason, boost_version=boost,
                E80_disposition=observation.e80_disposition,
                observer_disposition=observer, numeric_gate_failures=count, row_count=0,
                canonical_bytes=None, canonical_sha256=None, gzip_bytes=None,
                gzip_sha256=None, gzip_filename=None, status_filename=stem + ".status.json",
                ctest_exit_code=exit_code, packer_disposition="FAIL")


def _real_directory(path):
    if (not isinstance(path, Path) or not path.is_absolute() or path.parent == path or
            not stat.S_ISDIR(path.lstat().st_mode) or path.resolve(strict=True) != path):
        raise ValueError("finalizer requires a normalized real child directory")


def _exact_canonical_entry(parent, expected_name):
    count = 0
    with os.scandir(parent) as entries:
        for entry in entries:
            count += 1
            if count > 1 or entry.name != expected_name:
                raise FinalizationError("INTEGRITY", "canonical boundary contains foreign entries")
    if count == 0:
        raise FinalizationError("NO_CANONICAL", "exact canonical candidate is absent")


def _bounded_read(path, maximum, *, allow_empty=False):
    try:
        info = path.lstat()
        if not stat.S_ISREG(info.st_mode) or path.resolve(strict=True) != path:
            raise FinalizationError("INTEGRITY", "input is not a normalized regular file")
        if not (0 if allow_empty else 1) <= info.st_size <= maximum:
            raise FinalizationError("FORMAT", "input outside bounded byte envelope")
        with path.open("rb") as stream:
            data = stream.read(maximum + 1)
        if len(data) != info.st_size or len(data) > maximum:
            raise FinalizationError("INTEGRITY", "input changed size while reading")
        return data
    except OSError as error:
        raise FinalizationError("IO_ERROR", "cannot read input") from error


def _ctest_timeout_transport(data, host):
    # Frozen one-test CTest transport, observed on both hosted shells. The final
    # summary is outside --output-on-failure's unprefixed child-output replay.
    if host == "windows":
        data = data.replace(b"\r\n", b"\n")
    footer = (b"\nThe following tests FAILED:\n"
              b"\t 61 - paper_full_eight_square_contract (Timeout)\n"
              b"Errors while running CTest\n")
    if not data.endswith(footer):
        return False
    result_line = re.compile(
        rb"(?:\A|\n)1/1 Test #61: paper_full_eight_square_contract "
        rb"\.\.\.\*\*\*Timeout +(?:0|[1-9][0-9]*)\.[0-9]{2} sec(?=\n)")
    matches = result_line.finditer(data, 0, len(data) - len(footer))
    return next(matches, None) is not None and next(matches, None) is None


def finalize_endpoint(primary_log, identity, *, ctest_exit_code, capture_exit_code,
                      expected_scope, canonical_parent, published_parent, timed_out=False):
    """Finalize observed files; preserve failures, never accept a supplied summary."""
    if type(identity) is not PublicationIdentity:
        raise ValueError("identity must be PublicationIdentity")
    scalars = (identity.source_commit, identity.host, identity.github_run_id,
               identity.github_run_attempt)
    if any(type(value) is not str or len(value) > MAX_STATUS_BYTES for value in scalars):
        raise ValueError("identity fields must be bounded strings")
    if (re.fullmatch(r"[0-9a-f]{40}", identity.source_commit, flags=re.ASCII) is None or
            identity.host not in ("linux", "windows") or
            re.fullmatch(r"[1-9][0-9]*", identity.github_run_id, flags=re.ASCII) is None or
            re.fullmatch(r"[1-9][0-9]*", identity.github_run_attempt, flags=re.ASCII) is None):
        raise ValueError("invalid expected identity")
    for code in (ctest_exit_code, capture_exit_code):
        if type(code) is not int or not 0 <= code <= 255:
            raise ValueError("expected exact shell status 0..255")
    if type(timed_out) is not bool or expected_scope not in ("synthetic", "live-single-chain"):
        raise ValueError("invalid observed timeout or explicit scope")
    for parent in (canonical_parent, published_parent):
        _real_directory(parent)
    if (canonical_parent == published_parent or
            canonical_parent.parent != published_parent.parent or
            canonical_parent.name != "canonical" or published_parent.name != "published" or
            not isinstance(primary_log, Path) or primary_log.name != "primary.ctest.log" or
            primary_log.parent != published_parent.parent):
        raise ValueError("exact canonical/published/primary.ctest.log roles must share one scratch root")
    identity_arguments = dict(expected_source_commit=identity.source_commit,
                              expected_host=identity.host, expected_run_id=identity.github_run_id,
                              expected_run_attempt=identity.github_run_attempt)
    # Validate trusted identity before any identity-derived output path is used.
    initial_status = _status(identity, ctest_exit_code, "IO_ERROR")
    encode_status(initial_status, **identity_arguments)
    primary_identity = PrimaryIdentity(identity.source_commit, identity.host,
                                       identity.github_run_id, identity.github_run_attempt)
    try:
        data = _bounded_read(primary_log, MAX_LOG_BYTES, allow_empty=True)
    except FinalizationError as error:
        return publish_endpoint_evidence(
            published_parent, identity, _status(identity, ctest_exit_code, error.reason))
    transport_timeout = _ctest_timeout_transport(data, identity.host)
    timeout_conflict = transport_timeout and ctest_exit_code == 0
    try:
        primary = parse_primary_log(
            data, primary_identity, ctest_exit_code=ctest_exit_code,
            timed_out=timed_out or (transport_timeout and ctest_exit_code != 0),
            expected_scope=expected_scope)
    except PrimaryLogError as error:
        if error.first_failure is not None:
            reason = error.first_failure.reason
        else:
            reason = "IO_ERROR" if capture_exit_code else error.reason
        return publish_endpoint_evidence(
            published_parent, identity,
            _status(identity, ctest_exit_code, reason, error.observation))
    observation = PrimaryObservation(
        primary.numeric_gate_failures, primary.e80_disposition,
        primary.endpoint.boost_version if primary.endpoint is not None else None)
    if capture_exit_code:
        reason = primary.first_failure.reason if primary.first_failure is not None else "IO_ERROR"
    elif timeout_conflict and primary.first_failure is None:
        reason = "INTEGRITY"
    else:
        reason = primary.reason
        if (reason == "NO_CANONICAL" and ctest_exit_code != 0 and
                primary.numeric_gate_failures is None):
            reason = "CTEST_FATAL"
    if primary.evidence_state != "COMPLETE" or capture_exit_code or timeout_conflict:
        return publish_endpoint_evidence(
            published_parent, identity, _status(identity, ctest_exit_code, reason, observation))
    stem = initial_status["status_filename"].removesuffix(".status.json")
    canonical_stem = ("fs-endpoint-synthetic-" + ".".join(scalars)
                      if expected_scope == "synthetic" else stem)
    canonical_directory = canonical_parent / canonical_stem
    canonical_path = canonical_directory / (canonical_stem + ".tsv")
    try:
        _exact_canonical_entry(canonical_parent, canonical_stem)
        try:
            directory_mode = canonical_directory.lstat().st_mode
        except FileNotFoundError as error:
            raise FinalizationError("NO_CANONICAL", "canonical identity is absent") from error
        if (not stat.S_ISDIR(directory_mode) or
                canonical_directory.resolve(strict=True) != canonical_directory):
            raise FinalizationError("INTEGRITY", "canonical identity is not a real directory")
        _exact_canonical_entry(canonical_directory, canonical_stem + ".tsv")
        try:
            canonical_path.lstat()
        except FileNotFoundError as error:
            raise FinalizationError("NO_CANONICAL", "canonical leaf is absent") from error
        canonical_bytes = _bounded_read(canonical_path, MAX_BYTES)
        sidecar = read_sidecar(canonical_path, expected_scope=expected_scope, **identity_arguments)
        if _bounded_read(canonical_path, MAX_BYTES) != canonical_bytes:
            raise FinalizationError("INTEGRITY", "canonical bytes changed across validation")
    except OSError as error:
        return publish_endpoint_evidence(
            published_parent, identity, _status(identity, ctest_exit_code, "IO_ERROR", observation))
    except (FinalizationError, SidecarError) as error:
        return publish_endpoint_evidence(
            published_parent, identity, _status(identity, ctest_exit_code, error.reason, observation))
    try:
        validate_primary_binding(primary, sidecar)
        replay = replay_sidecar(sidecar)
        reconcile_replay(primary, sidecar, replay)
    except (ReconcileError, ReplayError) as error:
        return publish_endpoint_evidence(
            published_parent, identity, _status(identity, ctest_exit_code, error.reason, observation))
    try:
        gzip_bytes = encode_gzip(canonical_bytes)
        receipt = verify_gzip(
            gzip_bytes, canonical_size=len(canonical_bytes),
            canonical_sha256=hashlib.sha256(canonical_bytes).hexdigest(),
            gzip_size=len(gzip_bytes), gzip_sha256=hashlib.sha256(gzip_bytes).hexdigest())
        if receipt.canonical != canonical_bytes:
            raise GzipError("gzip round trip differs from validated canonical bytes")
    except GzipError:
        return publish_endpoint_evidence(
            published_parent, identity, _status(identity, ctest_exit_code, "INTEGRITY", observation))
    complete_status = dict(
        initial_status, chain_count=sidecar.meta["chain_count"], evidence_state="COMPLETE",
        reason="NONE", boost_version=observation.boost_version,
        E80_disposition=observation.e80_disposition, observer_disposition="PASS",
        numeric_gate_failures=observation.numeric_gate_failures, row_count=replay.row_count,
        canonical_bytes=receipt.canonical_size, canonical_sha256=receipt.canonical_sha256,
        gzip_bytes=receipt.gzip_size, gzip_sha256=receipt.gzip_sha256,
        gzip_filename=stem + ".tsv.gz", packer_disposition="PASS")
    return publish_endpoint_evidence(
        published_parent, identity, complete_status,
        canonical_bytes=canonical_bytes, gzip_bytes=gzip_bytes)


def _write_manifest(published_parent, identity, manifest):
    _real_directory(published_parent)
    if (not manifest.is_absolute() or manifest.parent != published_parent.parent or
            any(character in manifest.as_posix() for character in "\r\n\0")):
        raise ValueError("manifest must be a single normalized sibling path in this scratch root")
    if os.path.lexists(manifest):
        raise ValueError("refusing to overwrite an existing manifest")
    paths = select_endpoint_uploads(published_parent, identity)
    lines = [path.as_posix() for path in paths]
    if (not lines or len(lines) != len(set(lines)) or
            any(any(character in line for character in "\r\n\0") for line in lines)):
        raise ValueError("selected path cannot be represented in the exact LF manifest")
    data = ("\n".join(lines) + "\n").encode("utf-8")
    with manifest.open("xb") as stream:
        if stream.write(data) != len(data):
            raise OSError("incomplete upload manifest write")
    if _bounded_read(manifest, len(data)) != data:
        raise FinalizationError("INTEGRITY", "closed manifest differs from exact selected paths")


def main(argv=None):
    if sys.version_info[:2] != (3, 12):
        raise ValueError("endpoint finalizer requires Python 3.12")
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    finalize = commands.add_parser("finalize")
    select = commands.add_parser("select")
    for command in (finalize, select):
        command.add_argument("--source-commit", required=True)
        command.add_argument("--host", choices=("linux", "windows"), required=True)
        command.add_argument("--github-run-id", required=True)
        command.add_argument("--github-run-attempt", required=True)
        command.add_argument("--published-parent", required=True, type=Path)
    finalize.add_argument("--primary-log", required=True, type=Path)
    finalize.add_argument("--ctest-exit-code", required=True, type=int)
    finalize.add_argument("--capture-exit-code", required=True, type=int)
    finalize.add_argument("--scope", required=True, choices=("live-single-chain", "synthetic"))
    finalize.add_argument("--canonical-parent", required=True, type=Path)
    select.add_argument("--manifest", required=True, type=Path)
    args = parser.parse_args(argv)
    identity = PublicationIdentity(args.source_commit, args.host,
                                   args.github_run_id, args.github_run_attempt)
    try:
        if args.command == "select":
            _write_manifest(args.published_parent, identity, args.manifest)
            return 0
        result = finalize_endpoint(
            args.primary_log, identity, ctest_exit_code=args.ctest_exit_code,
            capture_exit_code=args.capture_exit_code, expected_scope=args.scope,
            canonical_parent=args.canonical_parent, published_parent=args.published_parent)
        return result.required_exit_code
    except PublicationError as error:
        print(str(error), file=sys.stderr)
        return error.required_exit_code


if __name__ == "__main__":
    raise SystemExit(main())
