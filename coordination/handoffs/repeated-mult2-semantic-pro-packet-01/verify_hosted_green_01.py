"""Read-only adjudication of a completed job; never builds or dispatches CI."""
import hashlib
import json
import re
import subprocess
import sys
from decimal import Decimal, getcontext

SOURCE = 'd09f15f535f0dbf22ef89b33255e947166cc392a'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
RUN = 33940418513
TEST = 'repeated_mult2_semantic_two_square_contract'
job_id = int(sys.argv[1])
assert job_id in (101236605855, 101236605909)

def gh(path):
    return subprocess.check_output(['gh', 'api', 'repos/leemaple/20231788./actions/' + path])

job = json.loads(gh(f'jobs/{job_id}'))
assert job['run_id'] == RUN and job['head_sha'] == SOURCE
assert job['status'] == 'completed' and job['conclusion'] == 'success'
required = ['Verify OpenFHE provenance', 'Record clean-room run provenance',
            'Build warning-clean project', 'Run focused first-Mult2 precision contract',
            'Run focused Pair Add and Sub inputs to first Mult2', 'Run legacy 57-test checkpoint',
            'Record all CTest bindings', 'Build repeated-Mult2 semantic contract',
            'Run focused repeated-Mult2 two-operation semantic contract', 'Run clean-room project tests']
required += [f'Build {api} public API contract' for api in ['Relin2', 'RS2', 'Mult2', 'Add', 'Sub']]
steps = {step['name']: step for step in job['steps']}
assert all(steps[name]['conclusion'] == 'success' for name in required)
raw = gh(f'jobs/{job_id}/logs')
lines = [re.sub(r'^\ufeff?\S+Z ', '', line) for line in raw.decode('utf-8').splitlines()]
lines = [re.sub(r'\x1b\[[0-9;]*m', '', line) for line in lines]
assert f'PROJECT_SOURCE_COMMIT={SOURCE} GITHUB_RUN_ID={RUN} GITHUB_RUN_ATTEMPT=1' in lines
assert any(line.strip() == 'OPENFHE_COMMIT: ' + PIN for line in lines)
inventory_start = lines.index('{')
inventory, _ = json.JSONDecoder().raw_decode('\n'.join(lines[inventory_start:]))
assert len(inventory['tests']) == 58 and inventory['tests'][-1]['name'] == TEST
cmake = subprocess.check_output(['git', 'show', SOURCE + ':CMakeLists.txt'], text=True)
expected = [(name, command.split()) for name, command in
            re.findall(r'add_test\(NAME\s+(\S+)\s+COMMAND\s+([^)]*)\)', cmake)]
assert len(expected) == 58
actual = []
for test in inventory['tests'][:57]:
    command = list(test['command'])
    command[0] = command[0].replace('\\', '/').rsplit('/', 1)[-1].removesuffix('.exe')
    actual.append((test['name'], command))
assert actual == expected[:57]
assert sum('58: Test command:' in line and 'repeated_mult2_semantic_two_square_test' in line for line in lines) == 2
records = [json.loads(line.split('58: ', 1)[1]) for line in lines if line.startswith('58: {')]
assert len(records) == 16
getcontext().prec = 180
limit = Decimal(2) ** -80
profile = {'test': TEST, 'scope': 'low-N-two-operation-diagnostic', 'N': 64, 'batch': 16,
           'depth': 9, 'scaling_bits': 50, 'first_bits': 55, 'input_scale_bits': 100,
           'scaling': 'FIXEDMANUAL', 'key_switch': 'HYBRID', 'data_type': 'COMPLEX',
           'secret_distribution': 'UNIFORM_TERNARY', 'security': 'HEStd_NotSet', 'result_family': 1}
invocations = []
for label, group in [('focused', records[:8]), ('full', records[8:])]:
    assert [(r['trial'], r['stage']) for r in group] == [(t, s) for t in range(4) for s in (1, 2)]
    tags = []
    for r in group:
        assert all(r[k] == v for k, v in profile.items())
        d, m, m1, m2 = (int(r[k]) for k in ('d', 'm', 'm1', 'm2'))
        stage = r['stage']
        assert d == 1125899906843009 and m1 == 1125899906840833 and m2 == 1125899906844161
        assert r['evaluation_family'] == stage - 1 and m == (m1 if stage == 1 else m2)
        assert int(r['scale_numerator']) == 2 ** (200 if stage == 1 else 400)
        assert int(r['scale_denominator']) == (d * m1 if stage == 1 else d ** 3 * m1 ** 2 * m2)
        assert 0 <= Decimal(r['max_slot_error']) <= limit and 0 <= Decimal(r['delta_error']) <= limit
        assert r['observed_product_headroom_bits'] > 0
        if stage == 1:
            tags.append(r['tag'])
        else:
            assert tags[-1] == r['tag']
    assert len(set(tags)) == 4
    invocations.append({'label': label, 'records': group,
                        'max_slot_error': str(max(Decimal(r['max_slot_error']) for r in group)),
                        'max_delta_error': str(max(Decimal(r['delta_error']) for r in group))})
summaries = [line for line in lines if '100% tests passed' in line or 'Total Test time (real)' in line]
assert len(summaries) == 10
assert [int(re.search(r'out of (\d+)', line).group(1)) for line in summaries[::2]] == [1, 2, 57, 1, 58]
print(json.dumps({'source': SOURCE, 'pin': PIN, 'run': RUN, 'job': job,
                  'raw_log_bytes': len(raw), 'raw_log_sha256': hashlib.sha256(raw).hexdigest(),
                  'legacy_57_bindings_equal_exact_source': True,
                  'inventory_count': 58, 'new_runtime_command_observed_twice': True,
                  'limit_2_pow_minus_80': str(limit), 'test_summaries': summaries,
                  'invocations': invocations, 'verdict': 'HOST_GREEN_WITH_16_VERIFIED_RECORDS'}, indent=2))
