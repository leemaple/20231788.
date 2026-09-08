#!/usr/bin/env python3
"""Read existing GitHub jobs; retain only bounded non-secret build evidence.

No downloads of libraries, execution of historical code, dispatch, or rerun.
The full log is hashed in memory; only selected build-step records are saved.
"""
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import re
import subprocess

HERE = Path(__file__).resolve().parent
GH = '/opt/homebrew/bin/gh'
REPO = 'leemaple/20231788.'
RUNS = {
    34039088536: 'original-S100',
    34055816234: 'S116',
    34184869227: 'S100-annulus125',
}
BUILD_STEPS = {
    'Configure OpenFHE', 'Verify OpenFHE provenance',
    'Cache pristine OpenFHE install', 'Post Cache pristine OpenFHE install',
    'Configure pinned OpenFHE native64 backend4',
    'Restore pristine OpenFHE install cache', 'Record exact OpenFHE cache result',
    'Verify exact clean OpenFHE source and sole official remote',
    'Configure clean-room project', 'Configure isolated one-shot candidate build',
}
SIGNALS = re.compile(r'(?i)(WITH_OPENMP|OpenMP|fopenmp|NATIVE_SIZE|MATHBACKEND|WITH_REDUCED_NOISE|WITH_NATIVEOPT|FIXED_SEED|HAVE_INT128|Cache restored|Cache not found|Cache saved|cache_hit=|df495ba2e91739a6dc8f1de254fc5a41155ce504|OPENFHE_COMMIT|CXX compiler identification|C compiler identification|Setting Native|Setting default|Setting MATHBACKEND)')
SECRET = re.compile(r'(?i)(authorization|cookie|github_token|gh_token|password|bearer|gh[pousr]_[a-z0-9_]{20,})')
ANSI = re.compile(r'\x1b\[[0-?]*[ -/]*[@-~]')

def call(*args):
    return subprocess.check_output([GH, *args], cwd=HERE, timeout=60)

def main():
    output = HERE / 'GITHUB_BUILD_EVIDENCE_V2.json'
    if output.exists():
        raise RuntimeError('exclusive evidence output exists')
    report = {'utc': datetime.now(timezone.utc).isoformat(), 'scope': 'Read-only existing run/job/artifact metadata and selected log signals; no new CI/test or binary inspection.', 'collector_revision': 2, 'limitation': 'gh may label log steps UNKNOWN STEP. Selection is by signal, not step name; reported step labels are preserved, not inferred. Attempt 1 remains unchanged; its zero selected lines were a collector limitation.', 'runs': []}
    for run_id, label in RUNS.items():
        meta = json.loads(call('run', 'view', str(run_id), '--repo', REPO, '--json', 'databaseId,headSha,status,conclusion,createdAt,updatedAt,jobs,url'))
        artifact_data = json.loads(call('api', f'repos/{REPO}/actions/runs/{run_id}/artifacts'))
        row = {key: meta[key] for key in ('databaseId','headSha','status','conclusion','createdAt','updatedAt','url')}
        row['label'] = label
        row['artifacts'] = [{k: art[k] for k in ('id','name','size_in_bytes','expired','created_at')} for art in artifact_data['artifacts']]
        row['jobs'] = []
        for job in meta['jobs']:
            data = call('run', 'view', '--job', str(job['databaseId']), '--repo', REPO, '--log')
            selected = []
            for line in data.decode('utf-8', 'replace').splitlines():
                parts = line.split('\t', 2)
                if len(parts) != 3 or not SIGNALS.search(parts[2]):
                    continue
                if SECRET.search(parts[2]):
                    continue
                selected.append({'step': parts[1], 'line': ANSI.sub('', parts[2])})
            row['jobs'].append({
                'id': job['databaseId'], 'name': job['name'], 'url': job['url'],
                'status': job['status'], 'conclusion': job['conclusion'],
                'steps': [{k: step[k] for k in ('number','name','status','conclusion','startedAt','completedAt')} for step in job['steps'] if step['name'] in BUILD_STEPS or 'Build and install' in step['name']],
                'log_bytes': len(data), 'log_sha256': hashlib.sha256(data).hexdigest(),
                'selected_nonsecret_build_lines': selected,
            })
            print(json.dumps({'run': run_id, 'job': job['name'], 'selected': len(selected)}, ensure_ascii=False), flush=True)
        report['runs'].append(row)
    with output.open('x') as handle:
        json.dump(report, handle, indent=2, ensure_ascii=False)
        handle.write('\n')
    print(str(output))

if __name__ == '__main__':
    main()
