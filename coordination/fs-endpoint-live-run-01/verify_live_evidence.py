"""Read-only receipt audit, not a local FHE/FFT/full numerical replay."""
from decimal import Decimal
from fractions import Fraction
import hashlib
import json
from pathlib import Path
import re
import sys
import tempfile
ROOT = Path(__file__).resolve().parents[2]
BASE = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / 'tests'))
from paper_endpoint_gzip import verify_gzip
from paper_endpoint_primary_reader import PrimaryIdentity, parse_primary_log
from paper_endpoint_sidecar_reader import read_sidecar
from paper_endpoint_reconcile import validate_primary_binding
from paper_endpoint_status import decode_status
SOURCE = 'ed5fd192a89d6d4728ad295e87cf06a3f4abc832'
RUN = '34039088536'
ATTEMPT = '1'
LOG_SHA256 = {'linux': 'ba76bd3b955fb6bd5e022dcd1d1d6aeb6a8ac657ea8188cef75ab48c0d9d016c', 'windows': '92201a4bb4a628a330df2d11f2bbb1ce885d651e4ddee946fcdc9284d7754384'}

def digest(data):
    return hashlib.sha256(data).hexdigest()

def audit(host):
    if not host in ('linux', 'windows'):
        raise ValueError("evidence check failed: host in ('linux', 'windows')")
    stem = 'fs-residual-endpoint-01.v1-r1.' + '.'.join((SOURCE, host, RUN, ATTEMPT))
    parent = BASE / host
    expected = {parent / (stem + suffix) for suffix in ('.status.json', '.tsv.gz')}
    if not set(parent.iterdir()) == expected:
        raise ValueError('evidence check failed: set(parent.iterdir()) == expected')
    if not all((p.is_file() and (not p.is_symlink()) for p in expected)):
        raise ValueError('evidence check failed: all((p.is_file() and (not p.is_symlink()) for p in expected))')
    payload = {p.name: p.read_bytes() for p in expected}
    status = decode_status(payload[stem + '.status.json'], expected_source_commit=SOURCE, expected_host=host, expected_run_id=RUN, expected_run_attempt=ATTEMPT)
    if not (status['evidence_state'] == 'COMPLETE' and status['reason'] == 'NONE'):
        raise ValueError("evidence check failed: status['evidence_state'] == 'COMPLETE' and status['reason'] == 'NONE'")
    if not (status['E80_disposition'] == 'FAIL' and status['ctest_exit_code'] == 8):
        raise ValueError("evidence check failed: status['E80_disposition'] == 'FAIL' and status['ctest_exit_code'] == 8")
    if not (status['row_count'] == 16384 and status['chain_count'] == 1):
        raise ValueError("evidence check failed: status['row_count'] == 16384 and status['chain_count'] == 1")
    receipt = verify_gzip(payload[stem + '.tsv.gz'], canonical_size=status['canonical_bytes'], canonical_sha256=status['canonical_sha256'], gzip_size=status['gzip_bytes'], gzip_sha256=status['gzip_sha256'])
    raw = (BASE / (host.upper() + '_JOB.log')).read_bytes()
    if not digest(raw) == LOG_SHA256[host]:
        raise ValueError('evidence check failed: digest(raw) == LOG_SHA256[host]')
    if not raw.startswith(b'\xef\xbb\xbf2026-09-06T'):
        raise ValueError("evidence check failed: raw.startswith(b'\\xef\\xbb\\xbf2026-09-06T')")
    if not (b'Cleaning up orphan processes' in raw and b'tokens truncated' not in raw):
        raise ValueError("evidence check failed: b'Cleaning up orphan processes' in raw and b'tokens truncated' not in raw")
    transport = [re.sub(b'^(?:\\xef\\xbb\\xbf)?2026-09-06T[0-9:.]+Z ', b'', line) for line in raw.splitlines(keepends=True)]
    primary = b''.join((line for line in transport if line.startswith(b'61: ')))
    parsed = parse_primary_log(primary, PrimaryIdentity(SOURCE, host, RUN, ATTEMPT), ctest_exit_code=8, expected_scope='live-single-chain')
    if not (parsed.evidence_state == 'COMPLETE' and parsed.cleanup_seen):
        raise ValueError("evidence check failed: parsed.evidence_state == 'COMPLETE' and parsed.cleanup_seen")
    if not parsed.numeric_gate_failures == status['numeric_gate_failures']:
        raise ValueError("evidence check failed: parsed.numeric_gate_failures == status['numeric_gate_failures']")
    if not parsed.endpoint.boost_version == status['boost_version']:
        raise ValueError("evidence check failed: parsed.endpoint.boost_version == status['boost_version']")
    fs_records = [line for line in primary.splitlines() if line.startswith(b'61: FS_ENDPOINT_')]
    if not len(fs_records) == 39:
        raise ValueError('evidence check failed: len(fs_records) == 39')
    if not sum((line.startswith(b'61: BEGIN test=paper_full_eight_square_contract ') for line in primary.splitlines())) == 1:
        raise ValueError("evidence check failed: sum((line.startswith(b'61: BEGIN test=paper_full_eight_square_contract ') for line in primary.splitlines())) == 1")
    if not sum((line.startswith(b'61: COMPLETE test=paper_full_eight_square_contract ') for line in primary.splitlines())) == 1:
        raise ValueError("evidence check failed: sum((line.startswith(b'61: COMPLETE test=paper_full_eight_square_contract ') for line in primary.splitlines())) == 1")
    if not sum((b'1/1 Test #61: paper_full_eight_square_contract' in line for line in transport)) == 1:
        raise ValueError("evidence check failed: sum((b'1/1 Test #61: paper_full_eight_square_contract' in line for line in transport)) == 1")
    summary_pattern = (rb'100% tests passed, 0 tests failed out of ([0-9]+)' if host == 'linux'
                       else rb'100% tests passed out of ([0-9]+)')
    regression_counts = [int(x) for x in re.findall(summary_pattern, raw)]
    if not regression_counts == [1, 2, 57, 1, 2, 60]:
        raise ValueError('evidence check failed: regression_counts == [1, 2, 57, 1, 2, 60]')
    if not sum((line.startswith(b'BEGIN test=paper_full_eight_square_contract ') for line in transport)) == 1:
        raise ValueError("evidence check failed: sum((line.startswith(b'BEGIN test=paper_full_eight_square_contract ') for line in transport)) == 1")
    if not sum((line.startswith(b'COMPLETE test=paper_full_eight_square_contract ') for line in transport)) == 1:
        raise ValueError("evidence check failed: sum((line.startswith(b'COMPLETE test=paper_full_eight_square_contract ') for line in transport)) == 1")
    run = json.loads((BASE / 'RUN_TERMINAL.json').read_bytes())
    if not (run['headSha'] == SOURCE and run['status'] == 'completed' and (run['conclusion'] == 'failure')):
        raise ValueError("evidence check failed: run['headSha'] == SOURCE and run['status'] == 'completed' and (run['conclusion'] == 'failure')")
    job_id = {'linux': 101502439156, 'windows': 101502439304}[host]
    job = next((j for j in run['jobs'] if j['databaseId'] == job_id))
    if not (job['status'] == 'completed' and job['conclusion'] == 'failure'):
        raise ValueError("evidence check failed: job['status'] == 'completed' and job['conclusion'] == 'failure'")
    for name, conclusion in (('Build paper full eight-square contract', 'success'), ('Run and finalize paper endpoint once', 'failure'), ('Select exact endpoint upload', 'success'), ('Upload exact endpoint evidence', 'success')):
        if not [s['conclusion'] for s in job['steps'] if s['name'] == name] == [conclusion]:
            raise ValueError("evidence check failed: [s['conclusion'] for s in job['steps'] if s['name'] == name] == [conclusion]")
    result_index = next((i for i, line in enumerate(transport) if b'1/1 Test #61: paper_full_eight_square_contract' in line))
    raw = b''.join(transport[result_index:])
    if not raw.count(b'Process completed with exit code 8.') == 1:
        raise ValueError("evidence check failed: raw.count(b'Process completed with exit code 8.') == 1")
    if not b'With the provided path, there will be 2 files uploaded' in raw:
        raise ValueError("evidence check failed: b'With the provided path, there will be 2 files uploaded' in raw")
    if not ('Artifact fs-residual-endpoint-' + host + '-' + RUN + '-1.zip successfully finalized').encode() in raw:
        raise ValueError("evidence check failed: ('Artifact fs-residual-endpoint-' + host + '-' + RUN + '-1.zip successfully finalized').encode() in raw")
    raw = (BASE / (host.upper() + '_JOB.log')).read_bytes()
    with tempfile.TemporaryDirectory(prefix='endpoint-receipt-read-') as directory:
        canonical_path = Path(directory) / (stem + '.tsv')
        canonical_path.write_bytes(receipt.canonical)
        sidecar = read_sidecar(canonical_path, expected_scope='live-single-chain', expected_source_commit=SOURCE, expected_host=host, expected_run_id=RUN, expected_run_attempt=ATTEMPT)
    validate_primary_binding(parsed, sidecar)
    rows = [[str(row.slot)] + [value.text for value in row.values] for row in sidecar.rows]
    for m in parsed.endpoint.maxima:
        row = rows[m.argmax_slot]
        values = {(name, component): value.text for name, component, value in m.tuple_values}
        if not row[1:] == [values[name, component] for name in ('E0', 'E8') for component in ('real', 'imag')]:
            raise ValueError("evidence check failed: row[1:] == [values[name, component] for name in ('E0', 'E8') for component in ('real', 'imag')]")
    for group, m in enumerate(parsed.endpoint.maxima[:2]):
        maximum = max((Decimal(v).copy_abs() for row in rows for v in row[1 + 2 * group:3 + 2 * group]))
        if not maximum == Decimal(m.magnitude.text).copy_abs():
            raise ValueError('evidence check failed: maximum == Decimal(m.magnitude.text).copy_abs()')
    maxima = {m.residual_id: m for m in parsed.endpoint.maxima}
    threshold = Fraction(1, 1 << 80)
    emax = maxima['E8']
    same = {(name, component): value for name, component, value in emax.tuple_values}
    inherited = same['I8', emax.argmax_component]
    added = same['A8', emax.argmax_component]
    inherited_lower = abs(inherited.value) - inherited.quantum - maxima['I8'].allowance
    added_upper = abs(added.value) + added.quantum + maxima['A8'].allowance
    summary = []
    for m in maxima.values():
        summary.append(dict(id=m.residual_id, magnitude=m.magnitude.text, slot=m.argmax_slot, component=m.argmax_component, lower=str(m.interval_lower), upper=str(m.interval_upper), allowance=str(m.allowance), threshold_ratio_exact=str(m.magnitude.value / threshold), signed_tuple={name + '.' + component: value.text for name, component, value in m.tuple_values}))
    return dict(host=host, source_commit=SOURCE, run_id=RUN, audit_result='EVIDENCE_AUDIT_PASS', numerical_result='E80_FAIL', job_result='FAIL', scope='public sidecar/status/binding readers, downloaded hashes/gzip, selected hosted CTest61 records and exact serialized E0/E8 extrema; no local full numerical replay', job_log_bytes=len(raw), job_log_sha256=digest(raw), job_log_selected_ctest61_bytes=len(primary), job_log_selected_ctest61_sha256=digest(primary), runner_primary_file_identity='NOT_RETAINED; selected hosted records are not its byte identity', actual_primary_endpoint_records=len(fs_records), actual_paper_test_invocations=1, unprefixed_failure_replay_count=1, regression_groups=regression_counts, status=status, numeric_gate_labels=list(parsed.numeric_gate_labels), maxima=summary, inherited_max_lower_exceeds_original_limit=maxima['I8'].interval_lower > threshold, added_max_upper_below_original_limit=maxima['A8'].interval_upper < threshold, inherited_max_lower_over_added_max_upper_exact=str(maxima['I8'].interval_lower / maxima['A8'].interval_upper), same_e8_max=dict(slot=emax.argmax_slot, component=emax.argmax_component, inherited_lower_exceeds_original_limit=inherited_lower > threshold, inherited_lower_over_added_upper_exact=str(inherited_lower / added_upper), reverse_triangle_lower_exceeds_original_limit=inherited_lower - added_upper > threshold, added_opposes_inherited=inherited.value * added.value < 0), files=[dict(path=str((parent / name).relative_to(ROOT)), bytes=len(data), sha256=digest(data)) for name, data in sorted(payload.items())])
if __name__ == '__main__':
    if not len(sys.argv) == 2:
        raise ValueError('evidence check failed: len(sys.argv) == 2')
    print(json.dumps(audit(sys.argv[1]), indent=2))
