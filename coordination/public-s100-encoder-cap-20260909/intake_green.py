#!/usr/bin/env python3
"""Read-only GREEN artifact intake. Never calls an encoding or transform."""
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
SOURCE = '28bab40431eebc85d72521c6d9dc840ecd675cd7'
BASE = 'a4b815a733efe81897325e2a8e4c826a4ebfa439'
ARTIFACT = ROOT/'artifacts/public-s100-encoder-cap-green-34265676284-1'
RETAINED = Path(__file__).resolve().parent/'green-evidence'


def require(condition, message):
    if not condition:
        raise ValueError(message)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    manifest = []
    for path in sorted(ARTIFACT.iterdir()):
        require(path.is_file() and not path.is_symlink(), 'nonregular artifact')
        name = path.name+'.txt' if path.name.endswith('.log') else path.name
        copy = RETAINED/name
        require(copy.is_file() and not copy.is_symlink(), 'missing/nonregular retained copy')
        data = path.read_bytes()
        require(data == copy.read_bytes(), 'retained bytes differ: '+path.name)
        manifest.append({'source_name': path.name, 'retained_name': name,
                         'bytes': len(data), 'sha256': sha(data)})
    require(len(manifest) == 36 and len(list(RETAINED.iterdir())) == 36, 'wrong file count')

    def read(name):
        return (ARTIFACT/name).read_text(encoding='utf-8')

    source_rows = []
    for row in read('project-source-sha256.txt').splitlines():
        digest, path = row.split('  ', 1)
        require(re.fullmatch('[0-9a-f]{64}', digest) is not None, 'invalid source hash')
        require('..' not in path and not path.startswith('/'), 'invalid source path')
        raw = subprocess.check_output(['git', 'show', SOURCE+':'+path], cwd=ROOT)
        require(sha(raw) == digest, 'source mismatch: '+path)
        source_rows.append(path)
    require(set(source_rows) == {
        'CMakeLists.txt', 'diagnostics/certify_public_encoder.py',
        'diagnostics/public_s100_encoding_dump.cpp',
        'include/openfhe_2023_1788/public_s100_encoding_probe.h',
        'src/high_precision_client_io.cpp', 'tests/paper_full_eight_square_oracle.h',
        'tests/public_encoder_scalar_contract_test.py',
        'tests/public_encoder_transform_contract_test.py'}, 'source manifest scope')
    require(len(source_rows) == 8, 'duplicate source rows')
    module_path = ROOT/'diagnostics/certify_public_encoder.py'
    expected_module = subprocess.check_output(['git', 'show', SOURCE+':diagnostics/certify_public_encoder.py'], cwd=ROOT)
    require(module_path.read_bytes() == expected_module, 'local parser differs from reviewed source')
    spec = importlib.util.spec_from_file_location('cap_intake_parser', module_path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)  # Definitions/constants only; no transform.
    raw = module.read_public_bytes(ARTIFACT/'public_s100_encoding.json')
    payload = json.loads(raw, object_pairs_hook=module.no_duplicates)
    coefficients = module.validate_payload(payload)
    cap = json.loads(read('public_s100_cap.json'), object_pairs_hook=module.no_duplicates)
    require(cap['public_payload_sha256'] == sha(raw), 'public byte hash mismatch')
    require(read('public-encoding.sha256').split()[0] == sha(raw), 'runner public hash mismatch')
    stream = ''.join(str(c)+'\n' for c in coefficients).encode('ascii')
    require(cap['coefficient_stream_sha256'] == sha(stream), 'coefficient stream mismatch')
    require(cap['maximum_absolute_coefficient'] == str(max(map(abs, coefficients))), 'coefficient maximum mismatch')
    require(cap['build_source_commit'] == payload['build_source_commit'] == SOURCE, 'wrong source identity')
    require(cap['base_source_commit'] == BASE, 'wrong production base')
    require(cap['compiler'] == payload['compiler'] == '13.3.0', 'compiler mismatch')
    require(cap['boost_version'] == payload['boost_version'] == '1_83', 'Boost mismatch')
    require(json.loads(read('metadata.json')) == {'build_source_commit': SOURCE, 'encoding_calls': 0, 'crypto_calls': 0}, 'metadata mismatch')
    require(read('public-cap.exit') == 'certify_exit=0\n', 'certifier exit')
    require(read('public-encode.exit') == 'encode_exit=0\n', 'encoder exit')
    require(read('api-negative.stdout') == 'API_NEGATIVE_PASS encoding_calls=0\n', 'API negative')
    require(read('public-encode.stdout') == 'PUBLIC_ENCODING_WRITTEN encoding_calls=1 crypto_calls=0\n', 'encoder banner')
    for name in ('public-cap.stderr', 'public-encode.stderr', 'metadata.stderr', 'api-negative.stderr'):
        require(read(name) == '', 'nonempty stderr: '+name)
    require('Ran 11 tests' in read('scalar-contracts.log') and read('scalar-contracts.log').endswith('OK\n'), 'scalar receipt')
    require('Ran 4 tests' in read('tiny-transform-models.log') and read('tiny-transform-models.log').endswith('OK\n'), 'tiny receipt')
    bound = cap['maximum_squared_modulus']
    lo, hi, den = (int(bound[k]) for k in ('lower_numerator', 'upper_numerator', 'denominator'))
    require(0 <= lo <= hi and den == 1 << 224, 'bad enclosure')
    require(cap['cap_squared'] == {'numerator': '16129', 'denominator': '16384'}, 'changed cap')
    require(hi*16384 <= 16129*den and cap['status'] == 'ENCODER_CAP_CERTIFIED', 'cap not certified')
    require(cap['n'] == 32768 and cap['grid_bits'] == 224 and cap['forward_canonical_transforms'] == 1, 'transform domain/count')
    require(cap['all_odd_roots_including_conjugates'] is True, 'missing conjugate roots')
    require(cap['butterflies'] == 245760 and cap['root_multiplications'] == 32767, 'transform structure')
    require(cap['historical_secret_pairing'] is None and cap['historical_E80_status'] == 'UNCHANGED_FAIL', 'historical overclaim')
    for key in ('max_lower_root_index', 'max_upper_root_index'):
        require(type(cap[key]) is int and 0 <= cap[key] < 32768, 'invalid max root index')
    for needle in (SOURCE, 'run_id=34265676284', 'run_attempt=1', 'stage=green',
                   'openfhe_sha=df495ba2e91739a6dc8f1de254fc5a41155ce504'):
        require(needle in read('provenance.txt'), 'missing run provenance')
    require(SOURCE in read('public-encoder-flags.make') and '-Werror' in read('public-encoder-flags.make'), 'build flags')
    for needle in ('MATHBACKEND:UNINITIALIZED=4', 'NATIVE_SIZE:UNINITIALIZED=64',
                   'WITH_OPENMP:BOOL=ON', 'WITH_REDUCED_NOISE:BOOL=OFF'):
        require(needle in read('openfhe-CMakeCache.txt'), 'wrong dependency configuration')
    print(json.dumps({'status': 'PASS_INTAKE_NOT_A_TRANSFORM_REPLAY', 'run_id': 34265676284,
        'source_commit': SOURCE, 'artifact_file_count': 36,
        'artifact_content_bytes': sum(row['bytes'] for row in manifest),
        'source_rows_matched_to_git': len(source_rows), 'public_payload_sha256': sha(raw),
        'coefficient_stream_sha256': sha(stream), 'exact_cap_comparison': True,
        'locally_executed_transforms': 0, 'historical_coefficient_identity': None,
        'files': manifest}, indent=2))


if __name__ == '__main__':
    main()
