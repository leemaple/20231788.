#!/usr/bin/env python3
"""Replay three fully read static scripts, reconcile independent root results.

Only authenticated documentation scripts execute; source packet never executes.
No build, FHE, FFT/NTT, sampler, network, or Git mutation.
"""
from datetime import datetime, timezone
from fractions import Fraction
import hashlib
import importlib.util
import json
import math
from pathlib import Path
import stat
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = ROOT / 'coordination/parameter-atlas-20260908'
DOC = ROOT / 'docs/parameter-atlas/pro'
INPUT_ZIP = ROOT / 'artifacts/handoffs/parameter-atlas-20260908/openfhe-parameter-atlas-a4b815a.zip'
INPUT = ROOT / 'artifacts/returns/parameter-atlas-20260908/verified-input'
CHECKS = HERE / 'root-replay'
SCRIPT_NAMES = ['check_input_identity.py', 'check_scalar_constants.py', 'check_document_consistency.py']

def sha(data):
    return hashlib.sha256(data).hexdigest()

def require(condition, message):
    if not condition:
        raise RuntimeError(message)

def main():
    sys.set_int_max_str_digits(100000)
    require(not INPUT.exists() and not CHECKS.exists(), 'exclusive output already exists')
    require(sha(INPUT_ZIP.read_bytes()) == 'abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68', 'input ZIP drift')
    manifest_bytes = (DOC / 'MANIFEST.json').read_bytes()
    require(sha(manifest_bytes) == '33b38355bc7ffbb2a5bd7d5171392f69025ff62cd68832dfdea3c6d69337945a', 'return manifest drift')
    manifest = json.loads(manifest_bytes)
    for row in manifest['files']:
        data = (DOC / row['path']).read_bytes()
        require(sha(data) == row['sha256'] and len(data) == row['bytes'], 'return member drift')
    helper_path = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    require(sha(helper_path.read_bytes()) == 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814', 'scanner drift')
    spec = importlib.util.spec_from_file_location('pinned_scanner', helper_path)
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    with zipfile.ZipFile(INPUT_ZIP) as z:
        helper.validate_names(z.namelist())
        require(z.testzip() is None, 'input CRC')
        require(all(stat.S_ISREG(i.external_attr >> 16) for i in z.infolist()), 'input member type')
        src_manifest = json.loads(z.read('MANIFEST.json'))
        require({r['path'] for r in src_manifest['files']} | {'MANIFEST.json'} == set(z.namelist()), 'input manifest set')
        for row in src_manifest['files']:
            require(sha(z.read(row['path'])) == row['sha256'], 'input member hash')
        INPUT.mkdir(parents=True)
        for name in z.namelist():
            target = INPUT / name
            target.parent.mkdir(parents=True, exist_ok=True)
            with target.open('xb') as handle:
                handle.write(z.read(name))
    CHECKS.mkdir()
    arguments = [
        [str(INPUT_ZIP), str(CHECKS / 'input_identity.json')],
        [str(INPUT), str(CHECKS / 'scalar_constants.json')],
        [str(INPUT), str(DOC), str(CHECKS / 'document_consistency.json')],
    ]
    commands = []
    for script, args in zip(SCRIPT_NAMES, arguments):
        command = [sys.executable, '-B', '-I', str(DOC / 'scripts' / script), *args]
        result = subprocess.run(command, cwd=ROOT, capture_output=True, check=False)
        with (CHECKS / (script + '.stdout.txt')).open('xb') as handle:
            handle.write(result.stdout)
        with (CHECKS / (script + '.stderr.txt')).open('xb') as handle:
            handle.write(result.stderr)
        commands.append({'command': command, 'exit_code': result.returncode})
        require(result.returncode == 0, 'static script failed: ' + script)
    raw = (CHECKS / 'scalar_constants.json').read_bytes()
    require(raw == (DOC / 'checks/scalar_constants.json').read_bytes(), 'scalar replay differs')
    scalar = json.loads(raw)
    independent = json.loads((HERE / 'ROOT_STATIC_PARAMETER_CHECK.json').read_bytes())
    for name, profile in scalar['profiles'].items():
        original = independent['profiles'][name]
        require(original['ordered_Q_and_roots'] == [[int(v['q']), int(v['root'])] for v in profile['ordered_primes'][:-1]], 'independent Q/root disagreement')
        require(original['P_and_root'] == [int(profile['ordered_primes'][-1][k]) for k in ('q', 'root')], 'independent P disagreement')
        for key in ('Q_bit_length', 'QP_bit_length'):
            require(original[key] == profile[key], 'independent bit count disagreement')
        for before, after in zip(original['rounds'], profile['steps']):
            denominator = math.prod(v['integer'] ** v['exponent'] for v in before['exact_scale_denominator_factors'])
            root_fraction = Fraction(1 << before['exact_scale_numerator_pow2'], denominator)
            pro_fraction = Fraction(int(after['scale_output']['numerator']), int(after['scale_output']['denominator']))
            require(root_fraction == pro_fraction, 'independent exact scale disagreement')
            require(before['consumed_Mult'] == int(after['dropped_mult']), 'drop disagreement')
            require(before['family_Q_count'] == len(after['family_Q']) and before['input_pair_Q_count'] == len(after['active_Q_before']) and before['output_pair_Q_count'] == len(after['active_Q_after']), 'basis shape disagreement')
    refs = json.loads((DOC / 'SOURCE_REFERENCES.json').read_bytes())['references']
    for key, ref in refs.items():
        url = ref['fixed_url']
        if not url:
            continue
        prefix = ref['path'].split('/', 1)[0]
        rel = ref['path'].split('/', 1)[1]
        repo = 'openfheorg/openfhe-development' if prefix == 'official' else 'leemaple/20231788.'
        expected = f'https://github.com/{repo}/blob/{ref["commit"]}/{rel}#L{ref["start_line"]}-L{ref["end_line"]}'
        require(url == expected, 'fixed URL path mismatch: ' + key)
    # Verified input source remains unchanged after scripts; no trust in a claim alone.
    for row in src_manifest['files']:
        require(sha((INPUT / row['path']).read_bytes()) == row['sha256'], 'input modified by script')
    report = {
        'utc': datetime.now(timezone.utc).isoformat(), 'commands': commands,
        'all_static_scripts_exit_zero': True, 'document_check_groups': 22,
        'scalar_replay_byte_identical': True, 'scalar_sha256': sha(raw),
        'independent_root_profile_and_16_scale_rounds_match': True,
        'all_127_reference_path_pin_line_links_match': True,
        'input_unchanged_after_replay': True,
        'scope': 'Documentation/hash/lexical/public-integer/Fraction checks only, not FHE numerical accuracy or complete semantic/security proof.',
    }
    with (CHECKS / 'RECONCILIATION.json').open('x') as handle:
        json.dump(report, handle, indent=2, ensure_ascii=False)
        handle.write('\n')
    print(json.dumps(report, indent=2, ensure_ascii=False))

if __name__ == '__main__':
    main()
