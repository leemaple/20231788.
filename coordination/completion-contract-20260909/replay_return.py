"""Replay reviewed scalar/text checks on a verified input packet, never FHE."""
import hashlib
import json
from pathlib import Path, PurePosixPath
import stat
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    source = ROOT / 'artifacts/handoffs/completion-contract-20260909/completion-contract-a7f54de.zip'
    assert sha(source.read_bytes()) == '31f5f6767718e2adaa1aae018dee8ceb44973f20125d0cef466a921a54174042'
    dest = ROOT / 'artifacts/completion-contract-input-replay-20260909'
    assert not dest.exists()
    with zipfile.ZipFile(source) as z:
        assert z.testzip() is None
        infos = z.infolist()
        assert len(infos) == 1007 and len({i.filename.casefold() for i in infos}) == 1007
        for i in infos:
            p = PurePosixPath(i.filename)
            assert not p.is_absolute() and '..' not in p.parts and str(p) == i.filename
            assert '\\' not in i.filename and ':' not in i.filename
            assert stat.S_ISREG(i.external_attr >> 16) and not i.flag_bits & 1
        manifest = json.loads(z.read('MANIFEST.json'))
        rows = manifest['files']
        assert len(rows) == 1006 and {r['path'] for r in rows} == set(z.namelist()) - {'MANIFEST.json'}
        for row in rows:
            data = z.read(row['path'])
            assert len(data) == row['bytes'] and sha(data) == row['sha256']
        z.extractall(dest)
    delivery = HERE / 'pro'
    evidence = HERE / 'root-replay'
    evidence.mkdir()
    inspected = {'bounded_checks.py': 'd6b2a12612b2a22353e557f53bf3fd5137e05b5c8d22a8c63c1a7091af46ce56',
                 'check_docs.py': '1c0549e8a58f8372edfecaa0d624ebd38c2af602f6d261ad137c23f436789d2f',
                 'doc_review_checks.py': 'f4d21a0fedb60f6c5741915fd5e6fe88373d59a64bfc82c7e1f5743c3e7f64b8'}
    for name, digest in inspected.items():
        assert sha((delivery / 'checks' / name).read_bytes()) == digest
    commands = [
        ('scalar', [sys.executable, '-B', '-I', str(delivery / 'checks/bounded_checks.py'), str(dest), str(evidence / 'BOUNDED_CHECKS.json')]),
        ('docs', [sys.executable, '-B', '-I', str(delivery / 'checks/doc_review_checks.py'), '--input-root', str(dest), '--delivery-root', str(delivery), '--evidence-dir', str(evidence)]),
    ]
    results = []
    for name, command in commands:
        result = subprocess.run(command, capture_output=True, timeout=45, check=False)
        for stream in ('stdout', 'stderr'):
            (evidence / (name + '.' + stream)).write_bytes(getattr(result, stream))
        (evidence / (name + '.exit')).write_text(str(result.returncode) + '\n')
        assert result.returncode == 0, (name, result.returncode)
        results.append({'name': name, 'command': command, 'exit': result.returncode})
    scalar = json.loads((evidence / 'BOUNDED_CHECKS.json').read_text())
    assert scalar == json.loads((delivery / 'evidence/BOUNDED_CHECKS.json').read_text())
    actual = json.loads((evidence / 'DOC_CHECK_RECEIPT.json').read_text())
    prior = json.loads((delivery / 'evidence/DOC_CHECK_RECEIPT.json').read_text())
    actual.pop('completed_utc')
    prior.pop('completed_utc')
    assert actual == prior
    record = {'status': 'PASS', 'scalar_checks': scalar['check_count'], 'scalar_result': 'BYTE_SEMANTIC_EQUAL',
              'doc_red_exit': 1, 'doc_green_exit': 0, 'rejected_text_mutations': actual['mutations_rejected'],
              'doc_result': 'EQUAL_EXCEPT_EXECUTION_TIMESTAMP', 'commands': results,
              'FFT_NTT_encoding_sampling_FHE_build_CI': 0}
    (evidence / 'ROOT_REPLAY.json').write_text(json.dumps(record, indent=2) + '\n')
    print(json.dumps(record, indent=2))


if __name__ == '__main__':
    main()
