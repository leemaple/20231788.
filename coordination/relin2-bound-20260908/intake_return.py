"""Retain one exact Relin2 documentation return; never execute returned code."""
from datetime import datetime, timezone
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = Path('/Users/lifeng/Downloads/RELIN2-IMPLEMENTATION-BOUND-01-delivery.zip')
SHA = '68fbad707507d225e05f35ef4a2039b8ef693a615c2bcc68e7cd386f8dad6b4d'
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
    require(len(raw) == 189392 and checks.sha256(raw) == SHA, 'archive identity')
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        infos = archive.infolist()
        names = [item.filename for item in infos]
        checks.validate_names(names)
        require(len(names) == 24 and sum(item.file_size for item in infos) == 527329, 'expanded inventory')
        require(all(not item.flag_bits & 1 and stat.S_ISREG(item.external_attr >> 16) for item in infos), 'encrypted/nonregular member')
        require(archive.testzip() is None, 'CRC failure')
        decoded = {name: archive.read(name) for name in names}
    manifest = json.loads(decoded['MANIFEST.json'])
    require(manifest['schema'] == 'relin2-implementation-bound-delivery-manifest-v1', 'schema')
    require(manifest['task'] == 'RELIN2-IMPLEMENTATION-BOUND-01' and manifest['self_excluding'] is True, 'task/self-exclusion')
    expected_input = {
        'zip_filename': 'relin2-implementation-bound-a4b815a.zip', 'zip_bytes': 3319606,
        'zip_sha256': '1aba188016d93ca8b38ec72f0f2592c6b45de562c455cb33e6d303d95ec6dc4c',
        'outer_manifest_sha256': '34b3a05c62c48a55dba5bdcdc8fde0e1f397cb831dce35a95f53cc97129422b3',
        'source_commit': 'a4b815a733efe81897325e2a8e4c826a4ebfa439',
        'official_openfhe_pin': 'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'task_commit': 'def8ccfde3c67bf00d7dc6a009451704b1b045b4',
    }
    require(manifest['input'] == expected_input, 'input binding')
    rows = manifest['files']
    require(len(rows) == 23 and {row['path'] for row in rows} == set(names) - {'MANIFEST.json'}, 'manifest closure')
    for row in rows:
        data = decoded[row['path']]
        require(len(data) == row['bytes'] and checks.sha256(data) == row['sha256'], 'member identity: ' + row['path'])
    payloads = {name: {'bytes': data} for name, data in decoded.items()}
    scans = [checks.targeted_content_scan(payloads, 'decoded Relin2 return'), checks.gitleaks_scan(payloads, 'decoded Relin2 return')]
    retained = ROOT / 'artifacts/returns/relin2-bound-20260908' / SOURCE.name
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
        'bytes': len(raw), 'sha256': SHA, 'regular_members': 24, 'expanded_bytes': 527329,
        'manifest_sha256': checks.sha256(decoded['MANIFEST.json']),
        'manifest_paths_crc_hashes': 'PASS', 'scans': scans, 'destination': str(dest),
        'conversation': 'https://chatgpt.com/c/6aa0282e-7014-83ec-8a1f-973111a7a29d',
        'title': '执行误差契约推导', 'ui': '6 / Pro; highest visible Pro5of5 before submission; backend unattested',
        'terminal_observed_utc': '2026-09-08T16:26:49.473Z', 'displayed_duration': 'Worked for 64m 15s',
        'download_clicks': 1, 'download_action_observed_utc': '2026-09-08T16:30:49.835Z',
        'transport_note': 'Visible temporary rate-limit dialog: waited a few minutes, dismissed it, clicked final ZIP once; no resubmission or restart. Download SHA/size matched.',
        'returned_scripts_executed': False, 'semantic_acceptance': 'PENDING independent review and bounded replay; intake integrity only',
    }
    with receipt.open('x') as output:
        json.dump(result, output, ensure_ascii=False, indent=2)
        output.write('\n')
    print(json.dumps(result, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
