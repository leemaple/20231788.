#!/usr/bin/env python3
"""Pinned, fail-closed documentation intake. Never executes returned code."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path('/Users/lifeng/Downloads/OPENFHE-CKKS-PARAMETER-RANDOMNESS-ATLAS-01.zip')
ARCHIVE = ROOT / 'artifacts/returns/parameter-atlas-20260908' / SOURCE.name
DEST = ROOT / 'docs/parameter-atlas/pro'
RECEIPT = ROOT / 'coordination/parameter-atlas-20260908/ROOT_INTAKE.json'
HELPER = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'

def digest(data):
    return hashlib.sha256(data).hexdigest()

def require(condition, message):
    if not condition:
        raise RuntimeError(message)

def main():
    require(digest(HELPER.read_bytes()) == 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814', 'helper drift')
    spec = importlib.util.spec_from_file_location('pinned_scanner', HELPER)
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    blob = SOURCE.read_bytes()
    require(len(blob) == 918057, 'ZIP size mismatch')
    require(digest(blob) == '623abe4affa88d2e53ba67b77a0648d477177c7df668508fdb66edbcb6c9d535', 'ZIP identity mismatch')
    require(not DEST.exists() and not ARCHIVE.exists() and not RECEIPT.exists(), 'exclusive output exists')
    with zipfile.ZipFile(SOURCE) as archive:
        infos = archive.infolist()
        helper.validate_names([i.filename for i in infos])
        require(len(infos) == 33, 'member count mismatch')
        require(sum(i.file_size for i in infos) < 10000000, 'expanded size limit')
        for item in infos:
            require(not item.is_dir() and stat.S_ISREG(item.external_attr >> 16), 'nonregular member')
            require(not item.flag_bits & 1, 'encrypted member')
        require(archive.testzip() is None, 'CRC failure')
        payloads = {i.filename: {'bytes': archive.read(i)} for i in infos}
    manifest_bytes = payloads['MANIFEST.json']['bytes']
    require(digest(manifest_bytes) == '33b38355bc7ffbb2a5bd7d5171392f69025ff62cd68832dfdea3c6d69337945a', 'manifest identity mismatch')
    manifest = json.loads(manifest_bytes)
    require(manifest['manifest_self_excluded'] is True, 'manifest convention')
    rows = manifest['files']
    require(len(rows) == 32 and {r['path'] for r in rows} == set(payloads) - {'MANIFEST.json'}, 'manifest coverage')
    for row in rows:
        content = payloads[row['path']]['bytes']
        require(len(content) == row['bytes'] and digest(content) == row['sha256'], 'payload mismatch: ' + row['path'])
    require(manifest['source_binding']['project_commit'] == 'a4b815a733efe81897325e2a8e4c826a4ebfa439', 'project pin')
    require(manifest['source_binding']['official_commit'] == 'df495ba2e91739a6dc8f1de254fc5a41155ce504', 'OpenFHE pin')
    require(manifest['input']['sha256'] == 'abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68', 'input binding')
    scans = [helper.targeted_content_scan(payloads, 'decoded-return'), helper.gitleaks_scan(payloads, 'decoded-return')]
    DEST.mkdir(parents=True)
    ARCHIVE.parent.mkdir(parents=True, exist_ok=True)
    with ARCHIVE.open('xb') as handle:
        handle.write(blob)
    for name, item in sorted(payloads.items()):
        path = DEST / name
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open('xb') as handle:
            handle.write(item['bytes'])
        require(path.is_file() and not path.is_symlink() and path.read_bytes() == item['bytes'], 'extract mismatch')
    receipt = {
        'utc': datetime.now(timezone.utc).isoformat(),
        'conversation': 'https://chatgpt.com/c/6aa009d7-cca4-83ec-9aab-1edac246bb44',
        'author_ui': '6 / Pro; highest visible Power 5 of 5; backend unattested',
        'terminal_observation': '2026-09-08T14:38:35Z; Worked for 85m11s; no Stop answering',
        'archive': str(ARCHIVE), 'bytes': len(blob), 'sha256': digest(blob),
        'manifest_sha256': digest(manifest_bytes), 'regular_members': len(payloads),
        'destination': str(DEST), 'all_member_identities_verified': True,
        'scans': scans, 'returned_scripts_executed': False,
        'acceptance': 'Integrity intake only; semantic acceptance recorded separately.',
        'retrieval_note': 'Browser used default Downloads despite requested directory. Repeated UI download clicks retrieved the same author return; no duplicate author task submission. No further download needed.'
    }
    with RECEIPT.open('x') as handle:
        json.dump(receipt, handle, indent=2, ensure_ascii=False)
        handle.write('\n')
    print(json.dumps(receipt, indent=2, ensure_ascii=False))

if __name__ == '__main__':
    main()
