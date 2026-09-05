#!/usr/bin/env python3
"""Read-only exact-source/new-run link RED audit; no compiler or numeric code."""
import collections
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
HERE = ROOT / 'coordination/fs-residual-endpoint-red-run-01'
SOURCE = '2fe655d493dcde5f05aa1515f41ca6823bba30bd'
PRIOR = '9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e'
PRODUCTION = 'b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
RUN = 33991083281
FUNCTIONS = {
    'Observe', 'ScaledOneNormExponent', 'DirectSparseReference768',
    'ExactAbsoluteDifference', 'AssessDifference', 'CanonicalDecimal',
    'IsCanonicalDecimal',
}


def git(*args):
    return subprocess.check_output(['git', *args], cwd=ROOT)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def need(condition, message):
    if not condition:
        raise AssertionError(message)


def only(items, label):
    need(len(items) == 1, label + ': expected exactly one')
    return items[0]


def main():
    terminal = json.loads((HERE / 'RUN_TERMINAL.json').read_bytes())
    run = terminal['run']
    need((run['id'], run['head_sha'], run['event'], run['run_attempt'],
          run['status'], run['conclusion']) ==
         (RUN, SOURCE, 'push', 1, 'completed', 'failure'), 'run identity/state')
    need(run['repository']['full_name'] == 'leemaple/20231788.', 'dotted repo')
    jobs = terminal['jobs']['jobs']
    need(len(jobs) == 2 and terminal['jobs']['total_count'] == 2, 'two jobs')
    paths = ['src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows/dcp-rcb.yml']
    changed = git('diff', '--name-only', PRIOR, SOURCE, '--', *paths).decode().splitlines()
    need(changed == ['tests/paper_endpoint_observer_contract.h',
                     'tests/paper_full_eight_square_contract_test.cpp'], 'two-path source delta')
    need(not git('diff', PRODUCTION, SOURCE, '--', 'src', 'include'), 'production unchanged')
    # Bind every current engineering input to the exact tested Git blob, while
    # permitting later documentation-only HEADs without changing the oracle.
    source_rows = []
    for name in git('ls-tree', '-r', '--name-only', SOURCE, '--', *paths).decode().splitlines():
        data = git('show', SOURCE + ':' + name)
        need((ROOT / name).read_bytes() == data, 'current source mismatch: ' + name)
        source_rows.append({'path': name, 'bytes': len(data), 'sha256': digest(data),
                            'git_blob': git('rev-parse', SOURCE + ':' + name).decode().strip()})
    preflight = json.loads((HERE / 'RAW_PREFLIGHT.json').read_bytes())
    rows = []
    for tag, job_id, filename in [('LINUX', 101373319837, 'LINUX_RAW.log'),
                                  ('WINDOWS', 101373319710, 'WINDOWS_LF.log')]:
        job = only([j for j in jobs if j['id'] == job_id], tag)
        need(job['run_id'] == RUN and job['head_sha'] == SOURCE and
             job['status'] == 'completed' and job['conclusion'] == 'failure', 'job identity')
        failed = [s for s in job['steps'] if s['conclusion'] == 'failure']
        build_step = only(failed, 'failed step')
        need(build_step['name'] == 'Build paper full eight-square contract', 'only paper build failed')
        runtime_step = only([s for s in job['steps'] if s['name'] ==
                            'Run paper full eight-square contract once'], 'runtime step')
        need(runtime_step['conclusion'] == 'skipped', 'paper runtime skipped')
        original = json.loads((ROOT / 'artifacts/handoffs/fs-residual-endpoint-red-run-01' /
                               (tag + '_CAPTURE.json')).read_bytes())['content'].encode()
        raw = (HERE / filename).read_bytes()
        recorded = only([r for r in preflight['records'] if r['platform'] == tag], 'preflight')
        need(digest(raw) == recorded['checked_in_sha256'] and
             digest(original) == recorded['decoded_sha256'], 'retained hashes')
        need(raw == original.replace(b'\r\n', b'\n'), 'only CRLF normalization')
        lines = raw.decode('utf-8-sig').splitlines()
        stripped = [re.sub(r'^\d{4}-\d\d-\d\dT\d\d:\d\d:\d\d\.\d+Z ', '', s)
                    for s in lines]
        text = '\n'.join(stripped)
        provenance = f'PROJECT_SOURCE_COMMIT={SOURCE} GITHUB_RUN_ID={RUN} GITHUB_RUN_ATTEMPT=1'
        provenance_line = only([i + 1 for i, s in enumerate(stripped) if s == provenance], 'provenance')
        compile_line = only([i + 1 for i, s in enumerate(stripped) if
                            'Building CXX object CMakeFiles/paper_full_eight_square_contract_test.dir/' in s], 'compile')
        link_line = only([i + 1 for i, s in enumerate(stripped) if
                         'Linking CXX executable paper_full_eight_square_contract_test' in s], 'link')
        need(provenance_line < compile_line < link_line, 'ordered compile/link')
        missing = []
        for i, s in enumerate(stripped):
            if re.search(r'undefined reference(?:s)? to', s):
                match = re.search(r'undefined reference(?:s)? to [`\x27]paper_endpoint_contract::(\w+)', s)
                need(match is not None and match.group(1) in FUNCTIONS, 'unexpected undefined symbol')
                need(i + 1 > link_line and 'paper_endpoint_observer_contract.h:' in s, 'endpoint link location')
                missing.append({'line': i + 1, 'function': match.group(1), 'diagnostic': s})
        need({x['function'] for x in missing} == FUNCTIONS, 'all seven undefined functions')
        errors = [{'line': i + 1, 'text': s} for i, s in enumerate(stripped)
                  if re.search(r': (?:fatal )?error:|##\[error\]', s)]
        need(len(errors) == 2 and all(x['line'] > link_line for x in errors), 'only terminal linker errors')
        need('ld returned 1 exit status' in errors[0]['text'] and
             'Process completed with exit code ' in errors[1]['text'], 'linker then workflow error')
        need(not re.search(r'^\s*Start\s+61:', text, re.M), 'no paper CTest start')
        for token in ['FS_RESIDUAL_SELFTEST result=', 'COMPLETE test=paper_full_eight_square_contract',
                      'OBS numeric_gate_failures=', 'OBS lifecycle=paper_owner_cleanup']:
            need(token not in text, 'unexpected endpoint/paper runtime output: ' + token)
        need(PIN in text and 'NATIVE_SIZE=64' in text and 'MATHBACKEND=4' in text, 'official build configuration')
        official_build = only([s for s in job['steps'] if s['name'] == 'Build and install OpenFHE'], 'official build')
        need(official_build['conclusion'] == 'success', 'official build completed')
        environment = [{'line': i + 1, 'text': s} for i, s in enumerate(stripped)
                       if re.search(r'^cmake version |^c\+\+ \(Ubuntu|^g\+\+\.exe |^  Version: |^  Image: |^Cache not found', s)]
        rows.append({'platform': tag, 'job_id': job_id, 'job_name': job['name'],
                     'completed_at': job['completed_at'], 'build_step': build_step,
                     'skipped_runtime_step': runtime_step, 'log_bytes': len(raw),
                     'log_sha256': digest(raw), 'log_lines': len(lines),
                     'provenance_line': provenance_line, 'compile_line': compile_line,
                     'link_line': link_line, 'undefined_diagnostics': missing,
                     'function_counts': dict(sorted(collections.Counter(x['function'] for x in missing).items())),
                     'error_lines': errors, 'environment': environment,
                     'new_paper_chain_observed': False, 'new_endpoint_selftest_observed': False})
    print(json.dumps({'scope': 'Exact-source link failure and absence of new runtime; not old-test inventory review.',
                      'source': SOURCE, 'run_id': RUN, 'run_conclusion': run['conclusion'],
                      'source_bindings': source_rows, 'hosts': rows,
                      'result': 'ACCEPT_EXPECTED_API_LINK_RED_ONLY',
                      'original_E80': 'FAIL retained from previous source9f; no new numeric result',
                      'numeric_or_complete_project_acceptance': False}, indent=2, sort_keys=True))


if __name__ == '__main__':
    main()
