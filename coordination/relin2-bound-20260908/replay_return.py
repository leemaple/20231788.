"""Root replay of four fully read, hash-pinned bounded return scripts."""
from datetime import datetime, timezone
from fractions import Fraction
import csv
import hashlib
import io
import json
from pathlib import Path
import re
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
PRO = HERE / 'pro'
PYTHON = '/Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3'
INPUT = ROOT / 'artifacts/handoffs/relin2-bound-20260908/relin2-implementation-bound-a4b815a.zip'
WORK = ROOT / 'artifacts/replays/relin2-bound-20260908'
PINS = {
    'model_checks.py': 'b165fabb42e257e4d75c43c434164c50efda859d6fe6d39a286ae43977422415',
    'public_bounds.py': '41b7d882186510d781704b21c606f4b4e08f37b967651e893898c3903b7b47da',
    'verify_delivery.py': 'f056cf8323a5b58cbeaa5f028c1f66c178d7b03d6cf699f6d0b8893001aaf69d',
    'verify_input_archive.py': '0cc8358bb4abe571f00bb598be60b80674cae40bb49d658575a2846ebda57979',
}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def require(ok, label):
    if not ok:
        raise RuntimeError(label)


def main():
    sys.set_int_max_str_digits(100000)
    require(not WORK.exists(), 'exclusive replay directory exists')
    receipt = HERE / 'ROOT_REPLAY.json'
    require(not receipt.exists(), 'exclusive replay receipt exists')
    for name, expected in PINS.items():
        require(sha((PRO / 'checks' / name).read_bytes()) == expected, 'reviewed script drift')
    require(sha((PRO / 'MANIFEST.json').read_bytes()) == '5a35ca6b224c6737af831f33a9e714112af903fb1b7103376ecd7647c7fc6026', 'return manifest drift')
    raw = INPUT.read_bytes()
    require(len(raw) == 3319606 and sha(raw) == '1aba188016d93ca8b38ec72f0f2592c6b45de562c455cb33e6d303d95ec6dc4c', 'input ZIP drift')
    sources = list(csv.DictReader(io.StringIO((PRO / 'SOURCE_MAP.tsv').read_text()), delimiter='\t'))
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        require(archive.testzip() is None, 'input CRC')
        for row in sources:
            data = archive.read(row['path'])
            require(sha(data) == row['sha256'], 'source-map hash')
            if row['line_start']:
                require(1 <= int(row['line_start']) <= int(row['line_end']) <= len(data.splitlines()), 'source line interval')
        source = archive.read('project/src/repeated_mult2.cpp')
    require(source == (ROOT / 'src/repeated_mult2.cpp').read_bytes(), 'runtime source mismatch')
    WORK.mkdir(parents=True)
    subset = WORK / 'input/project/src'
    subset.mkdir(parents=True)
    with (subset / 'repeated_mult2.cpp').open('xb') as handle:
        handle.write(source)
    # This subset contains the sole source consumer of public_bounds.py.
    # verify_input_archive separately verifies every member of the original ZIP.
    commands = [
        ('verify_input_archive.py', [str(INPUT), '--output', str(WORK / 'INPUT_VERIFICATION.json')]),
        ('model_checks.py', ['--output', str(WORK / 'model_results.json')]),
        ('public_bounds.py', ['--input-root', str(WORK / 'input'), '--output', str(WORK / 'public_bounds.json'), '--table', str(WORK / 'public_bounds.tsv')]),
        ('verify_delivery.py', [str(PRO)]),
    ]
    runs = []
    for name, args in commands:
        command = [PYTHON, '-B', '-I', str(PRO / 'checks' / name), *args]
        result = subprocess.run(command, cwd=PRO, env={'PATH': '/usr/bin:/bin', 'LANG': 'C', 'LC_ALL': 'C'}, capture_output=True, timeout=30, check=False)
        for suffix, content in (('stdout.txt', result.stdout), ('stderr.txt', result.stderr)):
            with (WORK / (name + '.' + suffix)).open('xb') as handle:
                handle.write(content)
        runs.append({'script': name, 'argv': command, 'exit': result.returncode,
                     'stdout_sha256': sha(result.stdout), 'stderr_bytes': len(result.stderr)})
        require(result.returncode == 0 and result.stderr == b'', 'replay failed: ' + name)
    comparisons = []
    for output, original in (
        ('INPUT_VERIFICATION.json', 'evidence/INPUT_VERIFICATION.json'),
        ('model_results.json', 'checks/model_results.json'),
        ('public_bounds.json', 'checks/public_bounds.json'),
        ('public_bounds.tsv', 'checks/public_bounds.tsv'),
    ):
        data = (WORK / output).read_bytes()
        require(data == (PRO / original).read_bytes(), 'replay byte mismatch: ' + output)
        comparisons.append({'output': output, 'bytes': len(data), 'sha256': sha(data), 'byte_identical_to_return': True})
    # Reconcile with the independent root formula written before Pro returned.
    constants = re.search(r'kPaperQ\{\{(.*?)\}\};', source.decode(), re.S).group(1)
    q = [int(x) for x, _ in re.findall(r'\{(\d+)ULL,(\d+)ULL\}', constants)]
    d, p, n, h, e, scale = q[-1], 1152921504606584833, 32768, 128, 39, Fraction(2**100)
    active = q[:-1]
    numbers = json.loads((WORK / 'public_bounds.json').read_text())
    crosschecks = []
    for index, row in enumerate(numbers['families']):
        def B(primes):
            return n * Fraction(n * e * sum(v - 1 for v in primes) + (1+h)*(p-1), p)
        rough = (B(active + [d]) + B(active)) * d / scale**2
        value = row['normalized_local_relin_bound']
        pro_bound = Fraction(int(value['numerator']), int(value['denominator']))
        omitted_zero_digit_term = Fraction(n*n*e*(d-1), p) * d / scale**2
        require(rough - pro_bound == omitted_zero_digit_term and pro_bound > 0, 'independent normalization reconciliation')
        crosschecks.append({'step': index+1, 'pro_bound_below_root_bound': True,
                            'exact_difference_is_root_extra_zero_d_digit_allowance': True})
        scale = scale**2 / (d * active[-1])
        active = active[:-1]
    result = {'utc': datetime.now(timezone.utc).isoformat(), 'status': 'PASS',
              'script_pins': PINS, 'work_directory': str(WORK), 'runs': runs,
              'byte_comparisons': comparisons, 'source_map_rows_hash_and_text_intervals_verified': len(sources),
              'independent_root_formula_comparisons': crosschecks,
              'scope': '13 deterministic public synthetic model groups; 8 exact Fraction family bounds; archive/delivery integrity. No FHE, FFT/NTT, sampling, production tests, or historical endpoint replay.',
              'semantic_acceptance': 'Still requires mathematical/source review; these checks do not establish original S100 E80.',
              'input_subset_note': 'Only frozen repeated_mult2.cpp is staged for the sole public_bounds source consumer; full original archive separately verified.'}
    with receipt.open('x') as handle:
        json.dump(result, handle, ensure_ascii=False, indent=2)
        handle.write('\n')
    print(json.dumps(result, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
