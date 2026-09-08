"""Retain a hash-bound Pro return after integrity and secret checks; execute none."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
DOWNLOAD = Path('/Users/lifeng/Downloads/REPRODUCTION-ADJUDICATION-01-delivery.zip')
SOURCE = '31e24bec1eb2db5d13de3b442a9e909e26db7206'
ZIP_SHA = 'ca8aa74937bc425d91519ec032a2bf13a842dad37384b6e9172e9fd501871424'
MANIFEST_SHA = '8ab0982505dcdbe314a5ddf33aec0d46ae2ab440f6a31c535133c91659e2a2fb'


def main():
    helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    assert hashlib.sha256(helper.read_bytes()).hexdigest() == 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'
    spec = importlib.util.spec_from_file_location('reviewed_scans', helper)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.require(DOWNLOAD.is_file() and not DOWNLOAD.is_symlink(), 'nonregular download')
    raw = DOWNLOAD.read_bytes()
    h.require(len(raw) == 369089 and h.sha256(raw) == ZIP_SHA, 'download identity')
    prefix = 'REPRODUCTION-ADJUDICATION-01/'
    with zipfile.ZipFile(DOWNLOAD) as z:
        infos = z.infolist()
        h.require(len(infos) == 42 and sum(i.file_size for i in infos) == 952482, 'expanded inventory')
        h.validate_names([i.filename for i in infos])
        h.require(all(i.filename.startswith(prefix) and stat.S_ISREG(i.external_attr >> 16)
                      and not i.flag_bits & 1 for i in infos), 'root/mode/encryption')
        h.require(z.testzip() is None, 'CRC failure')
        decoded = {i.filename.removeprefix(prefix): z.read(i) for i in infos}
    manifest_bytes = decoded['MANIFEST.sha256.json']
    h.require(h.sha256(manifest_bytes) == MANIFEST_SHA, 'manifest identity')
    m = json.loads(manifest_bytes)
    h.require(m['schema'] == 'sha256-manifest-v1' and m['task'] == 'REPRODUCTION-ADJUDICATION-01', 'manifest schema/task')
    h.require(m['source_commit'] == SOURCE and m['self_excluded'] == 'MANIFEST.sha256.json', 'source/self exclusion')
    h.require(m['input_archive_sha256'] == '7e3ea9fee4a04d5535cc47aab42a71e38367db18c70b804a609551345affcbe4', 'input binding')
    rows = m['files']
    h.require(len(rows) == 41 and {r['path'] for r in rows} == set(decoded)-{'MANIFEST.sha256.json'}, 'manifest closure')
    for row in rows:
        b = decoded[row['path']]
        h.require(len(b) == row['bytes'] and h.sha256(b) == row['sha256'], 'payload identity: '+row['path'])
    bindings = json.loads(decoded['source/SOURCE_BINDINGS.json'])
    h.require(len(bindings) == 14, 'source binding count')
    for row in bindings:
        path = row['input_path']
        if path.startswith('project/'):
            path = path.removeprefix('project/')
        if path.startswith('context/current/'):
            path = path.removeprefix('context/current/')
        b = subprocess.check_output(['git', '-C', str(ROOT), 'show', SOURCE+':'+path])
        h.require(b == decoded[row['delivery_path']] and len(b) == row['bytes']
                  and h.sha256(b) == row['sha256'], 'Git binding: '+path)
    payloads = {n: {'bytes': b} for n, b in decoded.items()}
    scans = [h.targeted_content_scan(payloads, 'decoded adjudication return'),
             h.gitleaks_scan(payloads, 'decoded adjudication return')]
    archive = ROOT / 'artifacts/returns/reproduction-adjudication-20260909' / DOWNLOAD.name
    dest = HERE / 'pro'
    receipt = HERE / 'RETURN_INTAKE.json'
    for p in (archive, dest, receipt):
        h.require(not p.exists() and not p.is_symlink(), 'refusing overwrite: '+str(p))
    archive.parent.mkdir(parents=True, exist_ok=True)
    h.require(not archive.parent.is_symlink(), 'archive parent symlink')
    with archive.open('xb') as f:
        f.write(raw)
    dest.mkdir()
    for n, b in decoded.items():
        p = dest / n
        p.parent.mkdir(parents=True, exist_ok=True)
        h.require(not p.parent.is_symlink(), 'output parent symlink')
        with p.open('xb') as f:
            f.write(b)
        h.require(p.read_bytes() == b, 'retained byte drift')
    result = {'archive': str(archive), 'bytes': len(raw), 'sha256': ZIP_SHA,
        'manifest_sha256': MANIFEST_SHA, 'members': 42, 'expanded_bytes': 952482,
        'source_commit': SOURCE, 'bound_git_files': 14, 'crc_paths_modes_manifest_bytes': 'PASS',
        'scans': scans, 'destination': str(dest), 'scripts_executed': False,
        'semantic_acceptance': 'PENDING independent proof/candidate review and bounded scalar replay',
        'conversation': 'https://chatgpt.com/c/6aa06117-a1b8-83ec-bb22-710339c3b865',
        'terminal_observed_utc': '2026-09-08T20:10:29.733Z', 'displayed_duration': 'Worked for 43m 59s',
        'download_click_utc': '2026-09-08T20:11:07.184Z', 'download_clicks': 1}
    with receipt.open('x') as f:
        json.dump(result, f, indent=2)
        f.write('\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
