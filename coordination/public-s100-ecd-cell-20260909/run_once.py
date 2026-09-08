"""Reviewed one-shot GitHub harness; numerical payload bytes remain immutable."""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

HERE = Path(__file__).resolve().parent
PRO = HERE.parent / 'reproduction-adjudication-20260909' / 'pro'
sys.path.insert(0, str(HERE))
from harness_contract import MANIFEST_SHA, reserve_output, validate_outcome, verify_delivery_bytes


def require(condition, message):
    if not condition:
        raise ValueError(message)


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def write_json(path, value):
    with path.open('x', encoding='utf-8') as target:
        json.dump(value, target, indent=2)
        target.write('\n')


def now():
    return datetime.now(timezone.utc).isoformat()


def stage(output, name, script, *args):
    # -E/-s suppress environment and user-site injection while retaining the
    # immutable candidate's required sibling imports. No PYTHONPATH is supplied.
    command = [sys.executable, '-B', '-E', '-s', str(script), *map(str, args)]
    started = now()
    write_json(output / (name + '.started.json'), {'utc': started, 'command': command})
    with (output / (name + '.stdout')).open('x') as stdout, \
            (output / (name + '.stderr')).open('x') as stderr:
        result = subprocess.run(command, stdout=stdout, stderr=stderr, check=False)
    with (output / (name + '.exit')).open('x') as target:
        target.write(str(result.returncode) + '\n')
    write_json(output / (name + '.finished.json'),
               {'started_utc': started, 'finished_utc': now(), 'actual_exit': result.returncode})
    print(json.dumps({'stage': name, 'actual_exit': result.returncode}), flush=True)
    return result.returncode


def finish(output, numerical_exit):
    after = stage(output, 'delivery-after', PRO / 'tools/verify_delivery.py')
    final = numerical_exit if after == 0 else 2
    write_json(output / 'HARNESS_RESULT.json', {
        'schema': 'public-s100-ecd-harness-v1', 'exit_code': final,
        'stage_or_numerical_exit': numerical_exit, 'delivery_after_exit': after,
        'original_S100_E80': 'UNCHANGED_FAIL', 'finished_utc': now(),
        'note': 'A process or intake failure is not a production encoding verdict.'})
    rows = [{'path': str(p.relative_to(output)), 'bytes': p.stat().st_size, 'sha256': sha(p)}
            for p in sorted(output.rglob('*')) if p.is_file()]
    write_json(output / 'EVIDENCE_MANIFEST.json', {'files': rows, 'self_excluded': True})
    return final


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--reviewed', action='store_true')
    parser.add_argument('--output-directory', type=Path, required=True)
    args = parser.parse_args()
    require(args.reviewed, 'explicit independent review gate required')
    for key, value in [('GITHUB_ACTIONS', 'true'), ('RUNNER_ENVIRONMENT', 'github-hosted'),
                       ('RUNNER_OS', 'Linux'), ('GITHUB_RUN_ATTEMPT', '1'),
                       ('GITHUB_REF', 'refs/tags/public-s100-ecd-cell-once-20260909')]:
        require(os.environ.get(key) == value, 'wrong runner gate: ' + key)
    require(sys.version_info[:2] == (3, 12), 'frozen Python3.12.x required')
    require(verify_delivery_bytes(PRO) == 41, 'wrong original payload count')
    output = reserve_output(PRO, args.output_directory)
    write_json(output / 'ONCE_RESERVED.json', {
        'utc': now(), 'python': sys.version, 'executable': sys.executable,
        'run_id': os.environ['GITHUB_RUN_ID'], 'attempt': 1,
        'ref': os.environ['GITHUB_REF'], 'source_commit': os.environ['GITHUB_SHA'],
        'original_manifest_sha256': MANIFEST_SHA,
        'integration_sha256': {p.name: sha(p) for p in [Path(__file__), HERE / 'harness_contract.py']},
        'instruction': 'Do not delete reservation or retry on failure.'})
    for name, script, arguments in [
        ('delivery-before', PRO / 'tools/verify_delivery.py', []),
        ('scalars', PRO / 'tools/run_scalar_checks.py', ['--output', output / 'scalars.json']),
        ('controls', PRO / 'candidate/check_rounding.py',
         ['--controls', '--allow-reviewed-transform', '--output-dir', output / 'controls'])]:
        code = stage(output, name, script, *arguments)
        if code != 0:
            return finish(output, code)
    controls = json.loads((output / 'controls/RESULT.json').read_text())
    require(controls['status'] == 'PASS' and controls['tiny_inverse_transforms'] == 4 and
            controls['full_transforms'] == controls['FHE_calls'] == 0, 'invalid analytic controls')
    code = stage(output, 'certificate', PRO / 'candidate/check_rounding.py',
                 '--certify', '--allow-reviewed-transform', '--output-dir', output / 'certificate')
    if code not in (0, 3, 4):
        return finish(output, code)
    result = json.loads((output / 'certificate/RESULT.json').read_text())
    require(int((output / 'certificate.exit').read_text()) == code, 'saved process exit mismatch')
    validate_outcome(result, code)
    # Original receiver recomputes status/margins/counts from all32768 rows.
    intake = stage(output, 'table-intake', PRO / 'tools/verify_rounding_result.py', output / 'certificate')
    if intake != 0:
        return finish(output, 2)
    write_json(output / 'OUTCOME_AGREEMENT.json', {
        'status': result['status'], 'declared_exit': result['exit_code'],
        'observed_exit': code, 'saved_exit': code, 'table_intake_exit': intake,
        'original_S100_E80': 'UNCHANGED_FAIL',
        'metadata_boundary': 'Row status/margins/counts/hashes verified by original receiver. '
        'Mean controls, negative labels and operation counts are also backed by reviewed producer '
        'source and stage execution, not independently re-executed by this metadata helper.'})
    return finish(output, code)


if __name__ == '__main__':
    raise SystemExit(main())
