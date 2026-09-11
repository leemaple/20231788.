"""One-shot event, exact test selection and classified output gates. No FHE."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import stat

REF = 'refs/tags/initial-phase-exact-once-20260911'
TEST = 'initial_phase_exact_contract'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
EVIDENCE = set('''provenance.txt source-sha256.txt run-history-gate.txt
openfhe-submodules.txt toolchain.txt openfhe-configure.log openfhe-build.log
openfhe-install.log openfhe-CMakeCache.txt project-configure.log project-build.log
project-CMakeCache.txt executable-sha256.txt linked-libraries.txt
linked-openfhe-sha256.txt ctest-selection.json core-dump-controls.txt ctest.log
result-gate.txt'''.split())


def require(ok, label):
    if not ok:
        raise ValueError(label)


def event(name, ref, attempt, payload):
    require(name == 'push' and ref == REF and attempt == '1', 'EVENT_IDENTITY')
    require(payload.get('created') is True and payload.get('deleted') is False
            and payload.get('forced') is False, 'EVENT_STATE')


def selection(payload, executable):
    tests = payload.get('tests')
    require(isinstance(tests, list) and len(tests) == 1, 'TEST_COUNT')
    require(tests[0].get('name') == TEST
            and tests[0].get('command') == [executable, '--controls'], 'TEST_COMMAND')


def history(pages, run_id, sha):
    require(isinstance(pages, list) and bool(pages), 'HISTORY_PAGES')
    runs = [run for page in pages for run in page['workflow_runs']
            if run.get('path') == '.github/workflows/initial-phase-exact-once.yml'
            and run.get('head_branch') == REF.split('/')[-1]
            and run.get('event') == 'push']
    require(len(runs) == 1, 'HISTORY_DUPLICATE_OR_ABSENT')
    require(str(runs[0]['id']) == run_id and runs[0].get('head_sha') == sha
            and runs[0].get('run_attempt') == 1, 'HISTORY_IDENTITY')


def evidence(root, complete):
    require(not root.is_symlink() and root.is_dir(), 'EVIDENCE_ROOT')
    paths = sorted(root.iterdir())
    names = {path.name for path in paths}
    require(names <= EVIDENCE, 'EVIDENCE_ALLOWLIST')
    require({'provenance.txt', 'source-sha256.txt'} <= names, 'EVIDENCE_PROVENANCE')
    require(not complete or names == EVIDENCE, 'EVIDENCE_INCOMPLETE_SUCCESS')
    rows = []
    for path in paths:
        info = path.lstat()
        require(stat.S_ISREG(info.st_mode) and info.st_size <= 32*1024*1024,
                'EVIDENCE_FILE_TYPE_OR_SIZE')
        data = path.read_bytes()
        rows.append({'path': path.name, 'bytes': len(data),
                     'sha256': hashlib.sha256(data).hexdigest()})
    return rows


def expected_lines(sha):
    require(re.fullmatch('[0-9a-f]{40}', sha) is not None, 'SOURCE_SHA')
    return [
        'baseline=PASS profile=initial-phase-n256-h128-v1 N=256 towers=3 H=128 G=39 F=15015',
        'mutation=one-tower-plus-one status=REJECTED reason=FRESH_CRT_COHERENCE',
        'mutation=all-towers-plus-30031 status=REJECTED reason=FRESH_SUPPORT_BOUND',
        f'slice=PASS originalS100=UNCHANGED source_base={sha} dependency_pin_declared={PIN}',
    ]


def result(text, sha):
    lines = [re.sub(r'^\d+: ', '', line) for line in text.splitlines()]
    require(not any(line.startswith(('NUMERIC_FAIL', 'CONTRACT_FAIL', 'UNCLASSIFIED_FAILURE'))
                    for line in lines), 'UNEXPECTED_FAILURE')
    observed = [line for line in lines if line.startswith(('baseline=', 'mutation=', 'slice='))]
    require(observed == expected_lines(sha), 'CLASSIFIED_OUTPUT')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('mode', choices=['event', 'selection', 'result', 'history', 'seal'])
    parser.add_argument('path', type=Path)
    parser.add_argument('arguments', nargs='*')
    args = parser.parse_args()
    if args.mode == 'event':
        name, ref, attempt = args.arguments
        event(name, ref, attempt, json.loads(args.path.read_text()))
    elif args.mode == 'selection':
        executable, = args.arguments
        selection(json.loads(args.path.read_text()), executable)
    elif args.mode == 'history':
        run_id, sha = args.arguments
        history(json.loads(args.path.read_text()), run_id, sha)
    elif args.mode == 'seal':
        sha, run_id, outcome = args.arguments
        rows = evidence(args.path, complete=outcome == 'success')
        manifest = {'schema': 'initial-phase-public-evidence-v1', 'source_sha': sha,
                    'run_id': run_id, 'execution_step_outcome': outcome,
                    'manifest_self_excluded': True, 'files': rows}
        with (args.path/'MANIFEST.json').open('x') as stream:
            json.dump(manifest, stream, indent=2)
            stream.write('\n')
    else:
        sha, = args.arguments
        result(args.path.read_text(), sha)
    print(f'{args.mode}=PASS')


if __name__ == '__main__':
    main()
