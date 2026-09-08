"""Verify and retain the exact terminal Pro return; execute no returned code."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent


def main():
    helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    assert hashlib.sha256(helper.read_bytes()).hexdigest() == 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'
    spec = importlib.util.spec_from_file_location('audited_helpers', helper)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    path = ROOT / 'artifacts/completion-contract-pro-return-20260909/COMPLETION-CONTRACT-01-a7f54de-review.zip'
    raw = path.read_bytes()
    h.require(len(raw) == 130320, 'archive size')
    h.require(h.sha256(raw) == '7bc432c855b61079dc75e9daf39486a371a7ec1d56f2a4574ad474bbd6e27cba', 'archive hash')
    with zipfile.ZipFile(path) as z:
        h.require(z.testzip() is None, 'CRC')
        infos = z.infolist()
        h.require(len(infos) == 31, 'member count')
        h.validate_names([i.filename for i in infos])
        for i in infos:
            h.require(stat.S_ISREG(i.external_attr >> 16) and not i.flag_bits & 1, 'member type')
        decoded = {i.filename: z.read(i) for i in infos}
    manifest = json.loads(decoded['MANIFEST.sha256.json'])
    h.require(manifest['self_excluded'] is True and manifest['self_path'] == 'MANIFEST.sha256.json', 'manifest self')
    h.require(manifest['input_source_commit'] == 'a7f54de2701a1b9bc02660f66febeff707b56651', 'source')
    h.require(manifest['input_task_commit'] == 'fcd745ae30a3f54e37b8ac060c854c226e38feb1', 'task')
    h.require(manifest['input_zip_sha256'] == '31f5f6767718e2adaa1aae018dee8ceb44973f20125d0cef466a921a54174042', 'input archive')
    rows = manifest['files']
    h.require(len(rows) == 30 and len({r['path'] for r in rows}) == 30, 'manifest rows')
    h.require({r['path'] for r in rows} == set(decoded) - {'MANIFEST.sha256.json'}, 'coverage')
    for row in rows:
        data = decoded[row['path']]
        h.require(len(data) == row['bytes'] and h.sha256(data) == row['sha256'], 'payload hash')
    payloads = {n: {'bytes': data} for n, data in decoded.items()}
    targeted = h.targeted_content_scan(payloads, 'terminal completion-contract return')
    strict = h.gitleaks_scan(payloads, 'terminal completion-contract return')
    dest = HERE / 'pro'
    h.require(not dest.exists(), 'refuse overwrite')
    for name, data in decoded.items():
        target = dest / name
        target.parent.mkdir(parents=True, exist_ok=True)
        with target.open('xb') as f:
            f.write(data)
    receipt = {'archive_path': str(path), 'archive_bytes': len(raw), 'archive_sha256': h.sha256(raw),
               'members': len(decoded), 'expanded_bytes': sum(map(len, decoded.values())),
               'manifest_sha256': h.sha256(decoded['MANIFEST.sha256.json']),
               'source_commit': manifest['input_source_commit'], 'task_commit': manifest['input_task_commit'],
               'crc_paths_regular_manifest': 'PASS', 'targeted': targeted, 'gitleaks': strict,
               'returned_code_executed': False, 'files': sorted(decoded)}
    with (HERE / 'RETURN_INTAKE.json').open('x') as f:
        json.dump(receipt, f, indent=2)
        f.write('\n')
    print(json.dumps({k: receipt[k] for k in ('archive_bytes', 'archive_sha256', 'members', 'expanded_bytes', 'manifest_sha256')}, indent=2))


if __name__ == '__main__':
    main()
