"""One-shot event, exact test selection and classified output gates. No FHE."""
import argparse
import json
from pathlib import Path
import re

REF = 'refs/tags/initial-phase-exact-once-20260911'
TEST = 'initial_phase_exact_contract'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'


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
    parser.add_argument('mode', choices=['event', 'selection', 'result'])
    parser.add_argument('path', type=Path)
    parser.add_argument('arguments', nargs='*')
    args = parser.parse_args()
    if args.mode == 'event':
        name, ref, attempt = args.arguments
        event(name, ref, attempt, json.loads(args.path.read_text()))
    elif args.mode == 'selection':
        executable, = args.arguments
        selection(json.loads(args.path.read_text()), executable)
    else:
        sha, = args.arguments
        result(args.path.read_text(), sha)
    print(f'{args.mode}=PASS')


if __name__ == '__main__':
    main()
