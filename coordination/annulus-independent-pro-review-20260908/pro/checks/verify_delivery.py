#!/usr/bin/env python3
"""Read-only delivery QA. No encryption, FFT, builds, network, or new numerical experiment."""
from __future__ import annotations

import argparse
import ast
import datetime as dt
import hashlib
import json
import stat
import sys
import zipfile
from decimal import Decimal, localcontext
from pathlib import Path, PurePosixPath

ARCHIVE_SIZE = 13_743_164
ARCHIVE_SHA = 'c30b6e84a53712792cfda4ec17da7b434ee745aebad3949916a90fa6d528a0b1'
INPUT_MANIFEST_SHA = '6cbd05178b788ead26ea12c1032c6fa8f22cad77e65488886a8d1b8f74fbbcdc'
FIRST_SHA = 'eaea61c388cadb419097917d16ac93edf7abbce2833a87bf03e7389b9ef444e5'
COMMIT = '03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b'
RAW_SHA = 'b0d17e0c2c819b1b59921fd7f8020a5265ae6c70e3b5fa87152a63abfbe4b5fd'


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def read_json(path: Path):
    return json.loads(path.read_text(encoding='utf-8'))


def check(review: Path, packet: Path, archive: Path) -> dict:
    details: dict = {}
    freeze = read_json(review / 'FIRST_PASS.freeze.json')
    require(freeze['first_pass_sha256'] == FIRST_SHA, 'first pass identity')
    for name, identity in freeze['bound_files'].items():
        data = (review / name).read_bytes()
        require(len(data) == identity['bytes'] and sha(data) == identity['sha256'], f'frozen file changed: {name}')
    first_time = dt.datetime.fromisoformat(freeze['frozen_at_utc'])
    reads = [json.loads(line) for line in (review / 'READ_RANGES.jsonl').read_text(encoding='utf-8').splitlines()]
    before = [json.loads(line) for line in (review / 'FIRST_PASS_READ_RANGES.jsonl').read_text(encoding='utf-8').splitlines()]
    require(all(not x['path'].startswith('after-first-pass/') and not x['first_pass_frozen'] for x in before), 'author read before freeze')
    authors = [x for x in reads if x['path'].startswith('after-first-pass/')]
    require(bool(authors) and all(x['first_pass_frozen'] and dt.datetime.fromisoformat(x['read_at']) > first_time for x in authors), 'author ordering')
    require(reads[:len(before)] == before, 'pre-freeze reading prefix changed')
    for item in reads:
        require(sha((packet / item['path']).read_bytes()) == item['sha256'], 'read-source bytes changed')
    details['first_pass'] = {'sha256': FIRST_SHA, 'bound_files_unchanged': len(freeze['bound_files']),
                             'freeze_utc': freeze['frozen_at_utc'], 'first_author_read_utc': authors[0]['read_at'],
                             'pre_freeze_read_ranges': len(before), 'all_read_ranges': len(reads),
                             'ordering_consistent': True, 'trusted_external_timestamp': False}

    archive_data = archive.read_bytes()
    require(len(archive_data) == ARCHIVE_SIZE and sha(archive_data) == ARCHIVE_SHA, 'archive changed')
    with zipfile.ZipFile(archive) as z:
        require(z.testzip() is None, 'input CRC failure')
        names = z.namelist()
        require(len(names) == len(set(names)) == 310, 'input member count')
        for info in z.infolist():
            p = PurePosixPath(info.filename)
            mode = info.external_attr >> 16
            require(not info.is_dir() and not p.is_absolute() and '..' not in p.parts and '\\' not in info.filename,
                    'unsafe input path')
            require(stat.S_IFMT(mode) in (0, stat.S_IFREG), 'nonregular input member')
            require((packet / info.filename).read_bytes() == z.read(info.filename), 'input extraction modified')
        mb = z.read('MANIFEST.json')
        require(sha(mb) == INPUT_MANIFEST_SHA, 'input manifest changed')
        manifest = json.loads(mb)
        require(manifest['source_commit'] == COMMIT, 'source binding')
        require({f['path'] for f in manifest['files']} == set(names) - {'MANIFEST.json'}, 'input self-exclusion')
        for f in manifest['files']:
            b = z.read(f['path'])
            require(len(b) == f['bytes'] and sha(b) == f['sha256'], 'input payload hash')
    details['input'] = {'bytes': ARCHIVE_SIZE, 'sha256': ARCHIVE_SHA, 'manifest_sha256': INPUT_MANIFEST_SHA,
                        'regular_members_unchanged': 310, 'CRC': 'PASS', 'source_commit': COMMIT}

    new = [read_json(review / 'results' / f'independent_{p}.json') for p in (180, 230)]
    old = [read_json(review / 'results' / f'old_s100_{p}.json') for p in (180, 230)]
    for p, result in zip((180, 230), new):
        require(result['precision_decimal_digits'] == p and result['status'] == 'PASS', 'new scalar status')
        require(result['slots'] == 16384 and result['new_encrypted_runs'] == 0, 'new sample scope')
        require(result['raw_sha256'] == RAW_SHA and result['source_commit'] == COMMIT, 'new raw binding')
        require(result['process_exit_from_end_receipt'] == 0, 'actual process exit')
        require(all(result['gates'].values()) and all(v == 0 for v in result['failing_slots_per_gate'].values()), 'new gates')
        require(result['formal_numerical_proof'] is False, 'formal claim enlarged')
    differences: dict = {'new': {}, 'old': {}}
    with localcontext() as ctx:
        ctx.prec = 260
        tolerance = Decimal(2) ** -300
        for name in new[0]['maxima']:
            a, b = (r['maxima'][name] for r in new)
            require(a['slot'] == b['slot'], 'new argmax drift')
            delta = abs(Decimal(a['complex_modulus']) - Decimal(b['complex_modulus']))
            require(delta < tolerance, 'new precision drift')
            differences['new'][name] = str(delta)
        for host in ('linux', 'windows'):
            differences['old'][host] = {}
            for p, all_hosts in zip((180, 230), old):
                item = all_hosts[host]
                require(item['precision_decimal_digits'] == p and item['row_count'] == 16384, 'old rows/precision')
                require(item['retained_E80'] == 'FAIL' and item['recorded_ctest_exit'] == 8, 'old FAIL not retained')
                require(item['no_new_encryption'] is True and item['formal_error_proof'] is False, 'old claim enlarged')
            require(old[0][host]['slots_above_T'] == old[1][host]['slots_above_T'], 'old gate counts drift')
            for name in old[0][host]['maxima']:
                a, b = (r[host]['maxima'][name] for r in old)
                require(a['complex_slot'] == b['complex_slot'] and a['component_slot'] == b['component_slot'], 'old argmax drift')
                delta = abs(Decimal(a['complex_modulus']) - Decimal(b['complex_modulus']))
                require(delta < tolerance, 'old precision drift')
                differences['old'][host][name] = str(delta)
    details['two_precision_comparison'] = {'both_new_PASS': True, 'both_old_platforms_still_FAIL': True,
                                            'differences': differences, 'not_a_formal_error_proof': True}

    boundaries = read_json(review / 'results/boundary_challenges.json')
    require(boundaries['test_groups'] == 13 and boundaries['new_encrypted_runs'] == 0, 'boundary scope')
    require(boundaries['original_raw_unchanged_sha256'] == RAW_SHA, 'boundary raw binding')
    for host in ('linux', 'windows'):
        require(boundaries['tests']['zero_added_error_does_not_fix_old_samples'][host]['A8_zero_still_fails'], 'old idealization')
    hist = read_json(review / 'results/receiver_historical_red.json')
    current = read_json(review / 'results/receiver_current_green.json')
    for item in (hist, current):
        require(item['real_record_positive_PASS'] and item['mutants_are_not_actual_evidence'], 'synthetic separation')
        require(item['new_encrypted_runs'] == 0 and item['original_raw_sha256'] == RAW_SHA, 'receiver scope')
    require(all(x['accepted'] for x in hist['mutant_tests'].values()), 'historical expected RED absent')
    require(all(not x['accepted'] for x in current['mutant_tests'].values()), 'current GREEN absent')
    require('process_exit=1' in (review / 'logs/receiver_historical_red.log').read_text(), 'RED exit log')
    require('process_exit=0' in (review / 'logs/receiver_current_green.log').read_text(), 'GREEN exit log')
    require('process_exit=1' in (review / 'logs/literal_paper_red.log').read_text(), 'paper RED exit log')
    post = read_json(review / 'results/post_author_scalar.json')
    require(not post['gaussian_support_scalar_consequence']['new_annulus_lift_certified'], 'annulus lift overclaim')
    require(all(not r['full_slot_raw_independently_replayed'] for r in post['S116_complex_upper_bound_from_retained_component_receipts'].values()), 'S116 raw overclaim')
    details['real_vs_synthetic'] = {'negative_tests_separate': True, 'historical_receiver_expected_RED': True,
                                   'current_receiver_GREEN': True, 'production_RED_claimed': False,
                                   'new_encrypted_runs': 0}

    required = ['FIRST_PASS.md', 'REVIEW.md', 'RESULT_FOR_CRYPTO_EXPERT.zh-CN.md', 'NEXT_ACTION.md',
                'EXECUTION_LEDGER.md', 'REPRODUCE.md', 'EXTERNAL_REFERENCE_NOTES.md']
    for name in required:
        require((review / name).is_file() and len((review / name).read_bytes()) > 100, f'missing report: {name}')
    scripts = sorted((review / 'checks').glob('*.py'))
    for script in scripts:
        ast.parse(script.read_text(encoding='utf-8'), filename=str(script))
    for path in review.rglob('*.json'):
        read_json(path)
    require(not list(review.rglob('*.pyc')), 'bytecode payload unexpected')
    details['documents_and_scripts'] = {'required_reports': required, 'AST_parsed_scripts': [p.name for p in scripts],
                                       'JSON_parse': 'PASS'}
    output_manifest = review / 'MANIFEST.json'
    if output_manifest.exists():
        m = read_json(output_manifest)
        actual = {p.relative_to(review).as_posix() for p in review.rglob('*') if p.is_file() and p.name != 'MANIFEST.json'}
        require({e['path'] for e in m['files']} == actual, 'output manifest membership')
        for e in m['files']:
            b = (review / e['path']).read_bytes()
            require(len(b) == e['bytes'] and sha(b) == e['sha256'], 'output manifest hash')
        details['output_manifest'] = 'PASS_EXISTING_MANIFEST'
    else:
        details['output_manifest'] = 'NOT_YET_CREATED_AT_INITIAL_QA; subsequently verified during ZIP packaging'
    return {'status': 'PASS', 'checked_at_utc': dt.datetime.now(dt.timezone.utc).isoformat(),
            'python_version': sys.version, 'scope': 'read-only delivery QA; no numerical rerun or encrypted experiment',
            'checks': details}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--review', type=Path, required=True)
    parser.add_argument('--packet', type=Path, required=True)
    parser.add_argument('--zip', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    try:
        result = check(args.review.resolve(), args.packet.resolve(), args.zip.resolve())
    except (ValueError, OSError, KeyError, zipfile.BadZipFile, SyntaxError) as exc:
        result = {'status': 'FAIL', 'error_type': type(exc).__name__, 'error': str(exc)}
    args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 0 if result['status'] == 'PASS' else 1


if __name__ == '__main__':
    raise SystemExit(main())
