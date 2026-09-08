"""Run only reviewed bounded Pro checks into new root-owned output files."""
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import subprocess
import sys


def main():
    here = Path(__file__).resolve().parent
    pro = here / 'pro'
    manifest_bytes = (pro / 'MANIFEST.json').read_bytes()
    assert hashlib.sha256(manifest_bytes).hexdigest() == '27518ad6b641509df4a971adb50e13f40576f31480e9791871f96223d7b950db'
    manifest = json.loads(manifest_bytes)
    for row in manifest['files']:
        data = (pro / row['path']).read_bytes()
        assert len(data) == row['bytes'] and hashlib.sha256(data).hexdigest() == row['sha256'], row['path']
    out = here / 'root-replay'
    assert not out.exists() and not out.is_symlink(), 'Never overwrite prior replay'
    out.mkdir()
    python = [sys.executable, '-B', '-I']
    commands = [
        python + [str(pro/'checks/run_checks.py'), '--input-dir', str(pro/'checks/bound_source'), '--out-dir', str(out)],
        python + [str(pro/'checks/check_candidate_scalars.py'), '--out', str(out/'candidate_scalar_checks.json')],
        python + [str(pro/'checks/check_candidate_static.py'), '--input-dir', str(pro/'checks/bound_source'), '--out', str(out/'candidate_static_checks.json')],
    ]
    executions = []
    for index, command in enumerate(commands, 1):
        start = datetime.now(timezone.utc)
        result = subprocess.run(command, capture_output=True, text=True, timeout=45)
        for kind, value in [('stdout', result.stdout), ('stderr', result.stderr)]:
            with (out/f'command{index}_{kind}.txt').open('x') as stream:
                stream.write(value)
        executions.append({'command': command, 'exit_code': result.returncode,
                           'elapsed_seconds': (datetime.now(timezone.utc)-start).total_seconds()})
        assert result.returncode == 0, f'Bounded check {index} failed; retained logs'
    outputs = ['results.json', 'conditional_spectral_certificate.json',
               'conditional_coarse_c25.json', 'K1_negative_control.json', 'summary.tsv',
               'candidate_scalar_checks.json', 'candidate_static_checks.json']
    comparisons = []
    for name in outputs:
        data = (out/name).read_bytes()
        assert data == (pro/'checks/results'/name).read_bytes(), name
        comparisons.append({'file': name, 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest(), 'byte_identical': True})
    receipt = {'utc': datetime.now(timezone.utc).isoformat(), 'executions': executions,
               'comparisons': comparisons, 'status': 'PASS',
               'scope': '9 finite algebra groups; 16384 public scalar formula values; 3 rational eight-step budgets; 7500 scalar interval endpoint assertions; 13 parser negatives; 2 in-memory patches. No transforms, crypto, sampling, C++ or CI.',
               'semantic_review': 'PENDING independent return reviews; arithmetic replay is not E80 or K-CAP verification'}
    with (out/'ROOT_REPLAY_RECEIPT.json').open('x') as stream:
        json.dump(receipt, stream, indent=2)
        stream.write('\n')
    print(json.dumps(receipt, indent=2))


if __name__ == '__main__':
    main()
