"""Exact public CI artifact intake. Hashes/CSV integers only; no transforms."""
import csv
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import stat
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = 'c5cf80bc31a47cf6e2aa7846d175c8b99d485d75'
RUN = 34275429052
ZIP_SHA = 'af951dcfcb6cd61ed92098ef612313a475fd06820ce430b6c4e821882e3bd8ca'
GH = '/opt/homebrew/bin/gh'


def need(ok, message):
    if not ok:
        raise ValueError(message)


def sha(raw):
    return hashlib.sha256(raw).hexdigest()


def api(path):
    return subprocess.run([GH, 'api', 'repos/leemaple/20231788./' + path],
                          check=True, capture_output=True).stdout


def main():
    os.chdir(ROOT)
    need(not (HERE / 'green-evidence').exists(), 'receipt already retained; no repeat intake')
    run_raw = api(f'actions/runs/{RUN}')
    run = json.loads(run_raw)
    need(run['head_sha'] == SOURCE and run['status'] == 'completed' and
         run['conclusion'] == 'success' and run['run_attempt'] == 1, 'wrong terminal run')
    archive = api('actions/artifacts/10075501743/zip')
    need(len(archive) == 12689634 and sha(archive) == ZIP_SHA, 'API artifact ZIP identity')
    with zipfile.ZipFile(io.BytesIO(archive)) as z:
        infos = z.infolist()
        need(len({i.filename for i in infos}) == len(infos), 'duplicate archive names')
        for item in infos:
            path = PurePosixPath(item.filename)
            need(not path.is_absolute() and '..' not in path.parts and
                 '\\' not in item.filename and str(path) == item.filename and
                 not item.is_dir() and stat.S_IFMT(item.external_attr >> 16) in (0, stat.S_IFREG),
                 'nonregular or unsafe artifact member')
        need(z.testzip() is None, 'ZIP CRC mismatch')
        files = {i.filename: z.read(i) for i in infos}
    downloaded = ROOT / f'artifacts/ecd-cell-run-{RUN}'
    need({str(p.relative_to(downloaded)) for p in downloaded.rglob('*') if p.is_file()} == set(files),
         'gh extracted set mismatch')
    for name, raw in files.items():
        need((downloaded / name).read_bytes() == raw, 'gh extracted byte mismatch')
    def obj(name):
        return json.loads(files[name])
    prefix = 'execution/'
    manifest = obj(prefix + 'EVIDENCE_MANIFEST.json')
    rows = manifest['files']
    need({prefix + r['path'] for r in rows} | {prefix + 'EVIDENCE_MANIFEST.json'} ==
         {p for p in files if p.startswith(prefix)}, 'execution manifest incomplete')
    for row in rows:
        raw = files[prefix + row['path']]
        need(len(raw) == row['bytes'] and sha(raw) == row['sha256'], 'execution hash mismatch')
    git_bound = 0
    for name in ('integration-sha256.txt', 'pro-manifest-sha256.txt', 'original-pro-source-sha256.txt'):
        for line in files[name].decode().splitlines():
            digest, path = line.split(None, 1)
            blob = subprocess.run(['git', 'show', SOURCE + ':' + path.strip()],
                                  check=True, capture_output=True).stdout
            need(sha(blob) == digest, 'source Git blob hash mismatch: ' + path)
            git_bound += 1
    reservation = obj(prefix + 'ONCE_RESERVED.json')
    need(reservation['source_commit'] == SOURCE and reservation['run_id'] == str(RUN) and
         reservation['attempt'] == 1 and reservation['python'].startswith('3.12.14 '), 'reservation mismatch')
    stages = ['delivery-before', 'scalars', 'controls', 'certificate', 'table-intake', 'delivery-after']
    for stage in stages:
        need(files[prefix + stage + '.exit'] == b'0\n' and files[prefix + stage + '.stderr'] == b'',
             'stage exit/stderr mismatch: ' + stage)
        need(obj(prefix + stage + '.finished.json')['actual_exit'] == 0, 'stage receipt mismatch')
    need(files['harness.exit'] == b'harness_exit=0\n' and files['harness-tee.exit'] == b'tee_exit=0\n',
         'outer harness/logger mismatch')
    need(obj(prefix + 'HARNESS_RESULT.json')['exit_code'] == 0, 'final harness mismatch')
    result = obj(prefix + 'certificate/RESULT.json')
    need(result['status'] == 'ECD_ROUNDING_CERTIFIED' and type(result['exit_code']) is int and
         result['exit_code'] == 0, 'wrong numerical status')
    table = files[prefix + 'certificate/coefficient_cells.tsv']
    need(sha(table) == result['coefficient_cells_sha256'] ==
         '3a5dad65c2323ae1ab1b453a5cfeb73028328176982cf82458bcf0d5049a9245', 'table identity')
    raw_p = (HERE.parent / 'reproduction-adjudication-20260909/pro/fixtures/public_s100_encoding.json').read_bytes()
    need(sha(raw_p) == result['public_payload_sha256'] ==
         '7cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f', 'input identity')
    p = [int(v) for v in json.loads(raw_p)['coefficients']]
    unit = 1 << 224
    minimum = None
    means = []
    total = 0
    for row in csv.DictReader(io.StringIO(table.decode()), delimiter='\t'):
        j = int(row['index'])
        need(j == total and j < 32768 and int(row['actual_p']) == p[j], 'row order or p mismatch')
        lo, hi = int(row['real_lower_num']), int(row['real_upper_num'])
        il, iu = int(row['imag_lower_num']), int(row['imag_upper_num'])
        left, right = (2 * p[j] - 1) * unit // 2, (2 * p[j] + 1) * unit // 2
        need(lo <= hi and il <= 0 <= iu and int(row['denominator']) == unit, 'invalid enclosure')
        margin = min(lo - left, right - hi)
        need(margin > 0 and lo > left and hi <= right and
             int(row['strict_margin_num']) == margin and row['cell_status'] == 'CERTIFIED', 'uncertified row')
        if minimum is None or margin < minimum[0]:
            minimum = (margin, j)
        if j in (0, 16384):
            need(lo <= -(1 << 32) * unit <= hi, 'analytic mean not enclosed')
            need(hi <= (2 * (p[j] + 1) - 1) * unit // 2 and
                 lo > (2 * (p[j] - 1) + 1) * unit // 2, 'semantic plus/minus negative not disjoint')
            means.append(j)
        total += 1
    need(total == 32768 and result['counts'] == {'CERTIFIED':32768,'REFUTED':0,'INCONCLUSIVE':0}, 'coverage')
    need(result['strict_minimum_margin'] == {'numerator':str(minimum[0]),'denominator':str(unit),
                                          'index':minimum[1]}, 'minimum mismatch')
    controls = obj(prefix + 'controls/RESULT.json')
    need(controls['status'] == 'PASS' and controls['tiny_inverse_transforms'] == 4 and
         len(controls['cases']) == 5, 'tiny controls mismatch')
    need(obj(prefix + 'scalars.json')['checks_count'] == 53 and
         obj(prefix + 'scalars.json')['status'] == 'PASS', 'scalar gate mismatch')
    need(result['original_S100_E80'] == 'UNCHANGED_FAIL' and
         result['FHE_calls'] == result['sampling_calls'] == result['production_encoding_calls'] ==
         result['new_ciphertext_chains'] == 0, 'scope mismatch')
    scan = subprocess.run(['/opt/homebrew/bin/gitleaks', 'stdin', '--ignore-gitleaks-allow',
        '--gitleaks-ignore-path', '/dev/null', '--max-decode-depth', '5', '--redact', '--no-banner', '--no-color'],
        input=b'\n'.join(name.encode()+b'\n'+raw for name,raw in sorted(files.items())), capture_output=True,
        env={'PATH':'/opt/homebrew/bin:/usr/bin:/bin','GOMAXPROCS':'2','LANG':'C','LC_ALL':'C'})
    need(scan.returncode == 0, 'strict artifact secret scan failed')
    destination = HERE / 'green-evidence'
    destination.mkdir()
    for name, raw in files.items():
        target = destination / name
        target.parent.mkdir(parents=True, exist_ok=True)
        with target.open('xb') as dst:
            dst.write(raw)
    archive_path = ROOT / f'artifacts/ecd-cell-run-{RUN}.zip'
    with archive_path.open('xb') as dst:
        dst.write(archive)
    receipt = {'status':'PASS', 'run_id':RUN, 'source_commit':SOURCE,
        'archive_sha256':ZIP_SHA, 'archive_bytes':len(archive), 'files':len(files),
        'expanded_bytes':sum(map(len,files.values())), 'source_Git_bindings':git_bound,
        'all_rows_independently_reclassified':total, 'exact_means_checked':means,
        'certificate_status':result['status'], 'minimum_margin':result['strict_minimum_margin'],
        'strict_gitleaks_exit':scan.returncode, 'strict_gitleaks_log':scan.stderr.decode(),
        'local_transforms':0,'FHE_calls':0,'original_S100_E80':'UNCHANGED_FAIL'}
    for name, value in [('RUN_RECEIPT.json', run), ('GREEN_INTAKE.json', receipt)]:
        with (HERE / name).open('x') as dst:
            json.dump(value, dst, indent=2); dst.write('\n')
    print(json.dumps(receipt))


if __name__ == '__main__':
    main()
