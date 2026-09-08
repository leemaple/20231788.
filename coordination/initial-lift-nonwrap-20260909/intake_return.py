"""Validate and retain the exact Pro return; never run returned code."""
from datetime import datetime, timezone
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = Path('/Users/lifeng/Downloads/INITIAL-LIFT-NONWRAP-01-return.zip')
SHA = '241b123f520afc8cefe616435e0496c52d3899b0a9f4b5525d4f63562f87cb51'
HELPER = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'


def main():
    if hashlib.sha256(HELPER.read_bytes()).hexdigest() != 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814':
        raise RuntimeError('reviewed helper drift')
    spec = importlib.util.spec_from_file_location('pinned_checks', HELPER)
    checks = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(checks)
    require = checks.require
    require(SOURCE.is_file() and not SOURCE.is_symlink(), 'download not regular')
    raw = SOURCE.read_bytes()
    require(len(raw) == 238632 and checks.sha256(raw) == SHA, 'archive identity')
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        infos = archive.infolist()
        names = [item.filename for item in infos]
        checks.validate_names(names)
        require(len(names) == 80 and sum(item.file_size for item in infos) == 819613, 'expanded inventory')
        require(all(not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16) for item in infos), 'encrypted/nonregular member')
        require(archive.testzip() is None, 'CRC failure')
        decoded = {name: archive.read(name) for name in names}
    require(checks.sha256(decoded['MANIFEST.json']) == '27518ad6b641509df4a971adb50e13f40576f31480e9791871f96223d7b950db', 'manifest identity')
    manifest = json.loads(decoded['MANIFEST.json'])
    require(manifest['schema'] == 'initial-lift-nonwrap-return-manifest-v1', 'schema')
    require(manifest['task'] == 'INITIAL-LIFT-NONWRAP-01' and manifest['self_excluded'] == ['MANIFEST.json'], 'task/self-exclusion')
    require(manifest['payload_count'] == 79 and manifest['payload_bytes'] == 805000, 'payload accounting')
    require(manifest['input_binding'] == {
        'archive_filename': 'initial-lift-nonwrap-a4b815a.zip', 'archive_bytes': 3626371,
        'regular_members': 537,
        'archive_sha256': '0cde0f999e13445e2faec46a4bf34dbcf6e47131c53afcf9e57375051eed7bed',
        'manifest_sha256': '96a45b3de562d23d30090e2048cb66d2c0412318a691bc204988fad736e305a7',
        'production_commit': 'a4b815a733efe81897325e2a8e4c826a4ebfa439',
        'official_commit': 'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'evidence_commit': 'b86105b33294a84ce76d65f585eb16d25ae07156',
        'task_commit': '6d42e6ea2d807cd7fdb9fc1433349694a6f06b48',
    }, 'input binding')
    rows = manifest['files']
    require(len(rows) == 79 and {row['path'] for row in rows} == set(names)-{'MANIFEST.json'}, 'manifest closure')
    for row in rows:
        data = decoded[row['path']]
        require(len(data) == row['bytes'] and checks.sha256(data) == row['sha256'], 'member identity: '+row['path'])
    source_members = [name for name in names if name.startswith('checks/bound_source/project/')]
    require(len(source_members) == 4, 'bound source count')
    for name in source_members:
        path = name.removeprefix('checks/bound_source/project/')
        original = subprocess.check_output(['git', '-C', str(ROOT), 'show', manifest['input_binding']['production_commit']+':'+path])
        require(decoded[name] == original, 'bound source differs: '+path)
    payloads = {name: {'bytes': data} for name, data in decoded.items()}
    # Exact scanner-definition false positive, not an exemption for token values.
    scan_report = decoded['evidence/RETURN_TARGETED_SCAN.json']
    require(checks.sha256(scan_report) == '012d0842051fa47a343dec30a3f69e89c1e46b458e2efb1cbf25f6a0f426ac52', 'scan report drift')
    require(json.loads(scan_report)['patterns']['github_token'] == r'\b(?:gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{30,})\b', 'scanner pattern differs')
    literal = b'github_pat_[A-Za-z0-9_]{30,}'
    require(scan_report.count(literal) == 1, 'unexpected scanner literal count')
    targeted_payloads = dict(payloads)
    targeted_payloads['evidence/RETURN_TARGETED_SCAN.json'] = {'bytes': scan_report.replace(literal, b'REVIEWED_REGEX_LITERAL')}
    scans = [checks.targeted_content_scan(targeted_payloads, 'decoded initial-lift return; one exact scanner-definition literal masked'),
             checks.gitleaks_scan(payloads, 'decoded initial-lift return')]
    retained = ROOT / 'artifacts/returns/initial-lift-nonwrap-20260909' / SOURCE.name
    dest = HERE / 'pro'
    receipt = HERE / 'RETURN_INTAKE.json'
    for path in (retained, dest, receipt):
        require(not path.exists() and not path.is_symlink(), 'refusing overwrite')
    retained.parent.mkdir(parents=True, exist_ok=True)
    require(not retained.parent.is_symlink(), 'archive parent symlink')
    with retained.open('xb') as output:
        output.write(raw)
    dest.mkdir()
    for name, data in decoded.items():
        path = dest / name
        path.parent.mkdir(parents=True, exist_ok=True)
        require(not path.parent.is_symlink(), 'extraction parent symlink')
        with path.open('xb') as output:
            output.write(data)
        require(path.read_bytes() == data, 'retained payload differs')
    result = {
        'utc': datetime.now(timezone.utc).isoformat(), 'archive': str(retained),
        'bytes': len(raw), 'sha256': SHA, 'regular_members': 80, 'expanded_bytes': 819613,
        'manifest_sha256': checks.sha256(decoded['MANIFEST.json']),
        'manifest_paths_crc_hashes': 'PASS', 'bound_source_git_blobs_verified': source_members,
        'scans': scans, 'destination': str(dest),
        'targeted_false_positive': {'path': 'evidence/RETURN_TARGETED_SCAN.json',
            'sha256': checks.sha256(scan_report), 'finding': 'github_token pattern matched bare prefix inside its own regex definition',
            'disposition': 'exact known regex field verified; only that literal masked for targeted scan; all original bytes Gitleaks-scanned and retained unchanged'},
        'conversation': 'https://chatgpt.com/c/6aa0400a-2dc8-83ec-8b3e-85f019a32321',
        'title': '执行初始DCP推导', 'ui': '6 / Pro; highest visible Pro5of5 before submission; backend unattested',
        'terminal_observed_utc': '2026-09-08T18:05:10.683Z', 'displayed_duration': 'Worked for 60m 2s',
        'download_clicks': 1, 'returned_scripts_executed': False,
        'semantic_acceptance': 'PENDING independent review and bounded replay; intake integrity only',
    }
    with receipt.open('x') as output:
        json.dump(result, output, ensure_ascii=False, indent=2)
        output.write('\n')
    print(json.dumps(result, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
