import csv
import hashlib
import io
import json
from pathlib import Path, PurePosixPath
import stat
import subprocess
import sys
import zipfile

def sha(data):
    return hashlib.sha256(data).hexdigest()

old = Path('/private/tmp/lossless-client-io-implementation-01-4ccc8fd.zip')
new = Path(sys.argv[1])
assert sha(old.read_bytes()) == '67cea2db1565550c7d96816d076a5d56be45e82f5175e9578e31ddbb50289f89'
assert 1000000 < new.stat().st_size < 5000000
pin = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
source_base = '4ccc8fd2e7617625d27e58a53eb3489e99466ed4'
upstream = '/private/tmp/h128-pro-packet.Lo406g/official-source'
repo = '/Users/lifeng/Documents/20231788-openfhe-lossless-io-implementation-20260905'
needed = {
    'src/core/lib/math/dftransform.cpp': '091220ee6b29ca1b0efbe8afa41a90098507720f59abcd0db1ed0a6e70fc26f3',
    'src/core/include/math/dftransform.h': '7492b8d882a421aa3fa28c2dc2d24548bc914d83e04a719e89c0ff7176dde900',
    'src/pke/include/encoding/ckkspackedencoding.h': '983981653104dc21680653f3b4e1108b51d0e8a39e40a0d8337f7d6502d0fe6a',
    'src/pke/include/encoding/plaintext.h': '8736ff85fa9366cc06a30d43a40e8dcaa55a240c9f2c881c880eb79f3cc340cb',
    'src/pke/include/encoding/plaintextfactory.h': '16f788d3e4cc32e8831bcbe7009f99a05e6ef6f0496c19e6e119e821f98b61a7',
}

with zipfile.ZipFile(old) as oz, zipfile.ZipFile(new) as nz:
    assert nz.testzip() is None
    infos = nz.infolist()
    names = [i.filename for i in infos]
    assert len(names) == len(set(names)) == len(set(n.casefold() for n in names))
    assert sum(i.file_size for i in infos) < 5000000
    roots = {n.split('/')[0] for n in names}
    assert len(roots) == 1
    prefix = next(iter(roots)) + '/'
    data = {}
    for i in infos:
        p = PurePosixPath(i.filename)
        assert not p.is_absolute() and '..' not in p.parts and str(p) == i.filename
        assert '\\' not in i.filename and ':' not in i.filename
        assert stat.S_IFMT(i.external_attr >> 16) in (0, stat.S_IFREG)
        assert not i.flag_bits & 1 and 0 < i.file_size < 2000000
        assert not ({'.git', 'node_modules', '__pycache__', 'build', '.env'} & set(p.parts))
        assert not any(x.lower().endswith(('.pem', '.p12', '.key', '.sqlite', '.db')) for x in p.parts)
        data[i.filename[len(prefix):]] = nz.read(i)
    manifest = data['MANIFEST.tsv']
    assert data['MANIFEST.sha256'].decode().strip().split() == [sha(manifest), 'MANIFEST.tsv']
    rows = list(csv.DictReader(io.StringIO(manifest.decode()), delimiter='\t'))
    assert len(rows) == len({r['path'] for r in rows})
    assert set(data) == {r['path'] for r in rows} | {'MANIFEST.tsv', 'MANIFEST.sha256'}
    for r in rows:
        b = data[r['path']]
        assert len(b) == int(r['bytes']) and sha(b) == r['sha256']
    oldprefix = 'lossless-client-io-implementation-01-4ccc8fd/'
    olddata = {n[len(oldprefix):]: oz.read(n) for n in oz.namelist()}
    payload = {n: b for n, b in olddata.items() if n not in {'MANIFEST.tsv', 'MANIFEST.sha256'}}
    assert len(payload) == 111
    assert all(data[n] == b for n, b in payload.items())
    projects = {n: b for n, b in payload.items() if n.startswith('project/')}
    assert len(projects) == 47
    for n, b in projects.items():
        assert subprocess.check_output(['git', '-C', repo, 'show', source_base + ':' + n[8:]]) == b
    for path, expected in needed.items():
        b = data['official-full/' + path]
        assert sha(b) == expected
        assert subprocess.check_output(['git', '-C', upstream, 'show', pin + ':' + path]) == b
    assert len([n for n in data if n.startswith('official-full/')]) == 64
    assert new.with_suffix('.zip.sha256').read_text().split() == [sha(new.read_bytes()), new.name]
    dest = Path('/private/tmp/io-source-correction-20260905.oeqUa5/root-verified')
    assert not dest.exists()
    nz.extractall(dest)
    assert all((dest / prefix / n).read_bytes() == b for n, b in data.items())
    print(json.dumps({'archive': str(new), 'bytes': new.stat().st_size, 'sha256': sha(new.read_bytes()), 'members': len(data), 'payloads': len(rows), 'original_payloads_unchanged': 111, 'project_blobs_exact_source_base': 47, 'official_files': 64, 'missing_five_exact_pin_match': 'PASS', 'new_paths': sorted(set(data) - set(olddata)), 'safe_paths_crc_modes_duplicates_manifest_sidecar_fresh_extraction': 'PASS', 'root_verified_staging': str(dest / prefix), 'runtime': 'NOT RUN'}, indent=2))
