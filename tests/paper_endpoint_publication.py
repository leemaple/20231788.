"""Failure-preserving endpoint publication and exact upload selection."""
from dataclasses import dataclass
import os
from pathlib import Path
import re
import stat

from paper_endpoint_gzip import GzipError, MAX_GZIP_BYTES, verify_gzip
from paper_endpoint_status import MAX_STATUS_BYTES, StatusError, decode_status, encode_status


_HEX40 = re.compile(r"[0-9a-f]{40}", re.ASCII)
_POSITIVE = re.compile(r"[1-9][0-9]*", re.ASCII)


class PublicationError(RuntimeError):
    """Publication failure which a wrapper must expose as a failing exit."""

    def __init__(self, message, *, required_exit_code=1, fallback_status_path=None,
                 first_cause=None):
        super().__init__(message)
        self.required_exit_code = required_exit_code or 1
        self.fallback_status_path = fallback_status_path
        self.first_cause = message if first_cause is None else first_cause


@dataclass(frozen=True)
class PublicationIdentity:
    source_commit: str
    host: str
    github_run_id: str
    github_run_attempt: str


@dataclass(frozen=True)
class PublicationResult:
    upload_paths: tuple[Path, ...]
    required_exit_code: int


def _identity_kwargs(identity):
    if type(identity) is not PublicationIdentity:
        raise PublicationError("identity must be PublicationIdentity")
    scalars = (identity.source_commit, identity.host, identity.github_run_id,
               identity.github_run_attempt)
    if any(type(value) is not str or len(value) > 256 * 1024 for value in scalars):
        raise PublicationError("identity fields must be bounded strings")
    if (_HEX40.fullmatch(identity.source_commit) is None or
            identity.host not in ("linux", "windows") or
            _POSITIVE.fullmatch(identity.github_run_id) is None or
            _POSITIVE.fullmatch(identity.github_run_attempt) is None):
        raise PublicationError("invalid expected identity")
    return dict(expected_source_commit=identity.source_commit,
                expected_host=identity.host,
                expected_run_id=identity.github_run_id,
                expected_run_attempt=identity.github_run_attempt)


def _stem(identity):
    _identity_kwargs(identity)
    return ("fs-residual-endpoint-01.v1-r1." + identity.source_commit + "." +
            identity.host + "." + identity.github_run_id + "." +
            identity.github_run_attempt)


def _validate_parent(parent):
    if not isinstance(parent, Path) or not parent.is_absolute():
        raise PublicationError("published parent must be an absolute pathlib.Path")
    try:
        resolved = parent.resolve(strict=True)
    except OSError as error:
        raise PublicationError("published parent does not resolve") from error
    if resolved != parent or not parent.is_dir() or parent.parent == parent:
        raise PublicationError("published parent must be normalized real directory")
    current = parent
    while current.parent != current:
        if current.is_symlink():
            raise PublicationError("published parent contains symlink")
        current = current.parent
    return parent


def _validated_status_bytes(record, identity):
    try:
        data = encode_status(record, **_identity_kwargs(identity))
        decoded = decode_status(data, **_identity_kwargs(identity))
    except StatusError as error:
        raise PublicationError("invalid status record") from error
    if decoded != record:
        raise PublicationError("status round-trip mismatch")
    return data


def _required_exit(record):
    exit_code = record["ctest_exit_code"]
    if exit_code != 0:
        return exit_code
    return 0 if record["evidence_state"] == "COMPLETE" else 1


def _derived_failure_status(complete, reason):
    record = dict(complete)
    record.update(
        evidence_state="FATAL", reason=reason, observer_disposition="FAIL",
        packer_disposition="FAIL", chain_count=None, row_count=0,
        canonical_bytes=None, canonical_sha256=None, gzip_bytes=None,
        gzip_sha256=None, gzip_filename=None)
    return record


def _fallback_reason(error):
    current = error
    while current is not None:
        if isinstance(current, OSError):
            return "IO_ERROR"
        current = current.__cause__
    return "INTEGRITY"


def _first_cause_text(error):
    current = error
    while current.__cause__ is not None:
        current = current.__cause__
    return str(current)


def _read_bounded_regular(path, maximum, label, *, expected_size=None):
    try:
        if path.is_symlink():
            raise PublicationError(label + " must not be a symlink")
        info = path.stat()
    except OSError as error:
        raise PublicationError("cannot stat " + label) from error
    if not stat.S_ISREG(info.st_mode):
        raise PublicationError(label + " must be a regular file")
    if info.st_size <= 0 or info.st_size > maximum:
        raise PublicationError(label + " is outside its byte envelope")
    if expected_size is not None and info.st_size != expected_size:
        raise PublicationError(label + " byte count mismatch")
    try:
        with path.open("rb") as stream:
            data = stream.read(maximum + 1)
    except OSError as error:
        raise PublicationError("cannot read " + label) from error
    if len(data) != info.st_size or len(data) > maximum:
        raise PublicationError(label + " changed size while reading")
    return data


def _remove_owned_file(path):
    try:
        mode = path.lstat().st_mode
    except FileNotFoundError:
        return
    if not stat.S_ISREG(mode):
        raise PublicationError("owned publication path changed type")
    path.unlink()


def publish_endpoint_evidence(published_parent, identity, status_record, *,
                              canonical_bytes=None, gzip_bytes=None):
    """Publish one identity; derive truthful incomplete status after failure."""
    parent = _validate_parent(published_parent)
    stem = _stem(identity)
    status_data = _validated_status_bytes(status_record, identity)
    if status_record["evidence_state"] != "COMPLETE":
        if canonical_bytes is not None or gzip_bytes is not None:
            raise PublicationError("incomplete publication must be status only")
        identity_dir = parent / stem
        staging = identity_dir / ".staging"
        status_final = identity_dir / status_record["status_filename"]
        status_stage = staging / (stem + ".candidate.status.json")
        status_candidate = identity_dir / (stem + ".candidate.status.json")
        identity_claimed = False
        staging_claimed = False
        status_stage_created = False
        status_candidate_created = False
        try:
            identity_dir.mkdir(mode=0o700)
            identity_claimed = True
            staging.mkdir(mode=0o700)
            staging_claimed = True
            status_stream = status_stage.open("xb")
            status_stage_created = True
            with status_stream:
                status_stream.write(status_data)
            reread_status = _read_bounded_regular(
                status_stage, len(status_data), "staged status",
                expected_size=len(status_data))
            if reread_status != status_data:
                raise PublicationError("closed status bytes changed")
            decode_status(reread_status, **_identity_kwargs(identity))
            if os.path.lexists(status_final) or os.path.lexists(status_candidate):
                raise PublicationError("refusing to overwrite publication output")
            status_stage.rename(status_candidate)
            status_stage_created = False
            status_candidate_created = True
            staging.rmdir()
            staging_claimed = False
            status_candidate.rename(status_final)
            status_candidate_created = False
        except (FileExistsError, OSError, StatusError, PublicationError) as error:
            if not identity_claimed:
                raise PublicationError(
                    "identity or output already exists",
                    required_exit_code=_required_exit(status_record),
                    first_cause=_first_cause_text(error)) from error
            try:
                if status_stage_created:
                    _remove_owned_file(status_stage)
                if status_candidate_created:
                    _remove_owned_file(status_candidate)
                if staging_claimed:
                    staging.rmdir()
                identity_dir.rmdir()
            except (OSError, PublicationError) as cleanup_error:
                raise PublicationError(
                    "incomplete publication failed and cleanup did not complete",
                    required_exit_code=_required_exit(status_record),
                    first_cause=_first_cause_text(error)) from cleanup_error
            raise PublicationError(
                "incomplete publication failed before status commit",
                required_exit_code=_required_exit(status_record),
                first_cause=_first_cause_text(error)) from error
        uploads = select_endpoint_uploads(parent, identity)
        return PublicationResult(uploads, _required_exit(status_record))
    identity_dir = parent / stem
    staging = identity_dir / ".staging"
    gzip_final = identity_dir / status_record["gzip_filename"]
    status_final = identity_dir / status_record["status_filename"]
    status_candidate = identity_dir / (stem + ".candidate.status.json")
    claimed = False
    staging_claimed = False
    gzip_stage_created = False
    status_stage_created = False
    gzip_renamed = False
    status_candidate_created = False
    status_committed = False
    try:
        identity_dir.mkdir(mode=0o700)
        claimed = True
        staging.mkdir(mode=0o700)
        staging_claimed = True
        if type(canonical_bytes) is not bytes or type(gzip_bytes) is not bytes:
            raise PublicationError("complete publication requires immutable payload bytes")
        receipt = verify_gzip(
            gzip_bytes, canonical_size=status_record["canonical_bytes"],
            canonical_sha256=status_record["canonical_sha256"],
            gzip_size=status_record["gzip_bytes"],
            gzip_sha256=status_record["gzip_sha256"])
        if receipt.canonical != canonical_bytes:
            raise PublicationError("canonical bytes do not match gzip payload")
        gzip_stage = staging / (stem + ".candidate.tsv.gz")
        status_stage = staging / (stem + ".candidate.status.json")
        gzip_stream = gzip_stage.open("xb")
        gzip_stage_created = True
        with gzip_stream:
            gzip_stream.write(gzip_bytes)
        reread_gzip = _read_bounded_regular(
            gzip_stage, len(gzip_bytes), "staged gzip", expected_size=len(gzip_bytes))
        status_stream = status_stage.open("xb")
        status_stage_created = True
        with status_stream:
            status_stream.write(status_data)
        reread_status = _read_bounded_regular(
            status_stage, len(status_data), "staged status", expected_size=len(status_data))
        if reread_gzip != gzip_bytes or reread_status != status_data:
            raise PublicationError("closed staged bytes changed")
        verify_gzip(reread_gzip, canonical_size=status_record["canonical_bytes"],
                    canonical_sha256=status_record["canonical_sha256"],
                    gzip_size=status_record["gzip_bytes"],
                    gzip_sha256=status_record["gzip_sha256"])
        decode_status(reread_status, **_identity_kwargs(identity))
        if (os.path.lexists(gzip_final) or os.path.lexists(status_final) or
                os.path.lexists(status_candidate)):
            raise PublicationError("refusing to overwrite publication output")
        gzip_stage.rename(gzip_final)
        gzip_stage_created = False
        gzip_renamed = True
        status_stage.rename(status_candidate)
        status_stage_created = False
        status_candidate_created = True
        staging.rmdir()
        staging_claimed = False
        status_candidate.rename(status_final)
        status_candidate_created = False
        status_committed = True
    except (OSError, StatusError, GzipError, PublicationError) as original:
        if not claimed:
            raise PublicationError(
                "identity or output already exists",
                required_exit_code=_required_exit(status_record),
                first_cause=_first_cause_text(original)) from original
        if status_committed:
            raise PublicationError(
                "publication failed after status commit",
                required_exit_code=_required_exit(status_record),
                first_cause=_first_cause_text(original)) from original
        failure_status_record = _derived_failure_status(
            status_record, _fallback_reason(original))
        failure_data = _validated_status_bytes(failure_status_record, identity)
        fallback_stage = staging / (stem + ".candidate.status.json")
        fallback_stage_created = False
        try:
            if gzip_renamed:
                _remove_owned_file(gzip_final)
                gzip_renamed = False
            if gzip_stage_created:
                _remove_owned_file(gzip_stage)
                gzip_stage_created = False
            if status_stage_created:
                _remove_owned_file(status_stage)
                status_stage_created = False
            if status_candidate_created:
                _remove_owned_file(status_candidate)
                status_candidate_created = False
            if not staging_claimed:
                staging.mkdir(mode=0o700)
                staging_claimed = True
            fallback_stream = fallback_stage.open("xb")
            fallback_stage_created = True
            with fallback_stream:
                fallback_stream.write(failure_data)
            fallback_reread = _read_bounded_regular(
                fallback_stage, len(failure_data), "fallback status",
                expected_size=len(failure_data))
            if fallback_reread != failure_data:
                raise PublicationError("closed fallback status bytes changed")
            decode_status(fallback_reread, **_identity_kwargs(identity))
            if os.path.lexists(status_final):
                raise PublicationError("refusing to overwrite fallback status")
            fallback_stage.rename(status_candidate)
            fallback_stage_created = False
            status_candidate_created = True
            staging.rmdir()
            staging_claimed = False
            status_candidate.rename(status_final)
            status_candidate_created = False
        except (OSError, StatusError, PublicationError) as fallback_error:
            try:
                if gzip_renamed:
                    _remove_owned_file(gzip_final)
                if gzip_stage_created:
                    _remove_owned_file(gzip_stage)
                if status_stage_created:
                    _remove_owned_file(status_stage)
                if fallback_stage_created:
                    _remove_owned_file(fallback_stage)
                if status_candidate_created:
                    _remove_owned_file(status_candidate)
                if staging_claimed:
                    staging.rmdir()
                identity_dir.rmdir()
            except (OSError, PublicationError) as cleanup_error:
                raise PublicationError(
                    "complete publication failed and cleanup did not complete",
                    required_exit_code=_required_exit(failure_status_record),
                    first_cause=_first_cause_text(original)) from cleanup_error
            raise PublicationError(
                "complete publication failed and fallback did not commit",
                required_exit_code=_required_exit(failure_status_record),
                first_cause=_first_cause_text(original)) from fallback_error
        raise PublicationError(
            "complete publication failed; truthful incomplete status committed",
            required_exit_code=_required_exit(failure_status_record),
            fallback_status_path=status_final,
            first_cause=_first_cause_text(original)) from original

    uploads = select_endpoint_uploads(parent, identity)
    return PublicationResult(uploads, _required_exit(status_record))


def select_endpoint_uploads(published_parent, identity):
    """Return the exact validated allowlist for one committed identity."""
    parent = _validate_parent(published_parent)
    stem = _stem(identity)
    identity_dir = parent / stem
    if not identity_dir.is_dir() or identity_dir.is_symlink():
        raise PublicationError("missing real identity directory")
    status_path = identity_dir / (stem + ".status.json")
    try:
        status_data = _read_bounded_regular(
            status_path, MAX_STATUS_BYTES, "status commit marker")
        status = decode_status(status_data, **_identity_kwargs(identity))
    except (StatusError, PublicationError) as error:
        raise PublicationError("invalid committed status") from error
    expected_names = {status["status_filename"]}
    uploads = []
    if status["evidence_state"] == "COMPLETE":
        gzip_path = identity_dir / status["gzip_filename"]
        try:
            gzip_data = _read_bounded_regular(
                gzip_path, MAX_GZIP_BYTES, "gzip payload",
                expected_size=status["gzip_bytes"])
            verify_gzip(gzip_data, canonical_size=status["canonical_bytes"],
                        canonical_sha256=status["canonical_sha256"],
                        gzip_size=status["gzip_bytes"],
                        gzip_sha256=status["gzip_sha256"])
        except (GzipError, PublicationError) as error:
            raise PublicationError("invalid committed gzip payload") from error
        expected_names.add(status["gzip_filename"])
        uploads.append(gzip_path)
    try:
        actual_names = set()
        for entry in identity_dir.iterdir():
            actual_names.add(entry.name)
            if len(actual_names) > 2:
                raise PublicationError("publication directory has extra entries")
    except OSError as error:
        raise PublicationError("cannot enumerate identity directory") from error
    if actual_names != expected_names:
        raise PublicationError("publication directory is not an exact allowlist")
    uploads.append(status_path)
    return tuple(uploads)
