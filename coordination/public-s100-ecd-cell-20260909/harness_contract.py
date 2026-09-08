"""Public admission and outcome boundaries. No numerical imports or transforms."""
from pathlib import Path
import hashlib
import json

MANIFEST_SHA = '8ab0982505dcdbe314a5ddf33aec0d46ae2ab440f6a31c535133c91659e2a2fb'


def verify_delivery_bytes(bundle: Path) -> int:
    """Bootstrap trust using root's fixed digest, without importing any payload."""
    raw = (bundle / 'MANIFEST.sha256.json').read_bytes()
    if hashlib.sha256(raw).hexdigest() != MANIFEST_SHA:
        raise ValueError('unbound original manifest')
    rows = json.loads(raw)['files']
    expected = {row['path'] for row in rows} | {'MANIFEST.sha256.json'}
    observed = set()
    for path in bundle.rglob('*'):
        if path.is_symlink():
            raise ValueError('delivery symlink rejected')
        if path.is_file():
            observed.add(str(path.relative_to(bundle)))
    if observed != expected:
        raise ValueError('delivery member set mismatch')
    # Paths/uniqueness are fixed by the already trusted manifest digest.
    for row in rows:
        data = (bundle / row['path']).read_bytes()
        if len(data) != row['bytes'] or hashlib.sha256(data).hexdigest() != row['sha256']:
            raise ValueError('delivery payload mismatch: ' + row['path'])
    return len(rows)


def reserve_output(bundle: Path, requested: Path) -> Path:
    """Reserve one fresh outside path on a private single-writer filesystem."""
    bundle = bundle.resolve(strict=True)
    if not requested.is_absolute() or requested.name in ('', '.', '..'):
        raise ValueError('absolute new final directory required')
    if requested.exists() or requested.is_symlink():
        raise ValueError('output already exists or is a symlink')
    output = requested.parent.resolve(strict=True) / requested.name
    if output == bundle or bundle in output.parents:
        raise ValueError('output resolves inside immutable delivery')
    output.mkdir(mode=0o700)
    return output


def validate_outcome(result: dict, observed_exit: int) -> int:
    expected = {'ECD_ROUNDING_CERTIFIED': 0, 'ECD_ROUNDING_REFUTED': 3,
                'INCONCLUSIVE_INTERVAL': 4}.get(result.get('status'))
    declared = result.get('exit_code')
    if (expected is None or type(declared) is not int or
            type(observed_exit) is not int or declared != expected or observed_exit != expected):
        raise ValueError('result status, declared exit and actual process exit disagree')
    return expected
