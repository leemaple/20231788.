#!/usr/bin/env python3
"""Bounded stdlib archive/scalar/source checks. Never compiles or executes FHE/NTT."""
import argparse
import ast
import hashlib
import json
import math
import struct
import zipfile
from decimal import Decimal, ROUND_CEILING
from pathlib import Path, PurePosixPath

SHA = '572cf0db6765dfbb1c8ee658d2425fc8e27933a540adf7b1d1145310e50de809'
SOURCE = '33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
PAPER_SHA = '61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac'

def require(value, label):
    if not value:
        raise RuntimeError(label)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('input_zip', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    raw = args.input_zip.read_bytes()
    require(len(raw) == 2630139 and hashlib.sha256(raw).hexdigest() == SHA, 'input identity')
    blob_count = 0
    with zipfile.ZipFile(args.input_zip) as z:
        entries = [x for x in z.infolist() if not x.is_dir()]
        require(len(entries) == 486 and len({x.filename for x in entries}) == 486, 'file count')
        for e in entries:
            p = PurePosixPath(e.filename)
            require(not p.is_absolute() and '..' not in p.parts, 'unsafe path')
            require(((e.external_attr >> 16) & 0o170000) != 0o120000, 'symlink')
        require(z.testzip() is None, 'zip CRC')
        manifest = json.loads(z.read('MANIFEST.json'))
        require(manifest['source_commit'] == SOURCE and manifest['official_pin'] == PIN, 'source pins')
        require(len(manifest['files']) == 485 and manifest['manifest_self_excluded'], 'manifest envelope')
        require({r['path'] for r in manifest['files']} | {'MANIFEST.json'} ==
                {e.filename for e in entries}, 'manifest path coverage')
        for r in manifest['files']:
            b = z.read(r['path'])
            require(len(b) == r['bytes'] and hashlib.sha256(b).hexdigest() == r['sha256'], 'manifest row')
            origin = r.get('origin', {})
            if 'git_blob' in origin:
                blob_count += 1
                git_blob = hashlib.sha1(b'blob ' + str(len(b)).encode() + b'\0' + b).hexdigest()
                require(git_blob == origin['git_blob'], 'git blob')
        paper = z.read('references/paper/PAPER-2023-1788.pdf')
        require(hashlib.sha256(paper).hexdigest() == PAPER_SHA, 'paper identity')
        existing = z.read('project/tests/paper_h128_client_keypair_contract_test.cpp').decode()
        context = existing[existing.index('Context MakeContext()'):existing.index('// Value snapshots')]
    delivery = Path(__file__).resolve().parent.parent
    test = (delivery / 'tests/initial_phase_exact_contract_test.cpp').read_text()
    require(context in test, 'current context/check fixture copied exactly')
    require(test.count('cc->GetScheme()->Encrypt(input,key.publicKey)') == 1, 'one payload encryption callsite')
    oracle = test[test.index('class DirectInverse final'):test.index('void PublicTransformAnchor')]
    for forbidden in ('.SetFormat(', '.CRTInterpolate(', 'DecryptCore(', 'ForwardTransform(', 'InverseTransform('):
        require(forbidden not in oracle, 'oracle shares forbidden transform')
    require('e.what()' not in test and 'assert(' not in test.replace('static_assert(', ''), 'logging/release check')
    require('std::ofstream' not in test and 'SetSeed' not in test, 'no captures/seeding')
    q = [1125899906826241, 1099511603713, 1099511630849]
    roots = [5834101087838, 694658335, 322807922]
    # Only scalar modular powers for frozen public constants, not a transform.
    require(all(pow(r, 256, m) == m-1 and pow(r, 512, m) == 1 for m, r in zip(q, roots)), 'root orders')
    require(all(math.gcd(q[i], q[j]) == 1 for i in range(3) for j in range(i)), 'coprime CRT')
    sigma = Decimal.from_float(struct.unpack('f', struct.pack('f', 3.19))[0])
    m = Decimal.from_float(12.00610553538285)
    g = int((sigma*m).to_integral_value(rounding=ROUND_CEILING))
    require(g == 39, 'successful Peikert support ceiling')
    f = g*(256+128+1)
    delta = 2*f+1
    require((f, delta) == (15015, 30031), 'noise/mutation bounds')
    require(min(q) > 2*(3*f+1), 'mutation no modular wrap')
    public = [(1<<100)+(1<<30)+1, -((1<<100)-(1<<29)-3), (1<<65)+7,
              -((1<<80)+5), (1<<54)+9, -11]
    product = math.prod(q)
    require(all(2*(abs(x)+f) < product for x in public), 'public coefficient nonwrap')
    require(-f+delta == f+1 and f+delta == 3*f+1, 'all coherent mutants outside support')
    # Parse our own source, not the supplied historical propagation checker.
    ast.parse(Path(__file__).read_text())
    report = dict(status='PASS_STATIC_ONLY', input_bytes=len(raw), input_files=486,
        manifest_rows=485, git_blob_rows_checked=blob_count, input_sha256=SHA,
        source_commit=SOURCE, official_pin=PIN, paper_sha256=PAPER_SHA,
        scalar_sigma_binary32_decimal=str(sigma), gaussian_support=g, fresh_bound=f,
        coherent_mutation_delta=delta, composite_Q=str(product), composite_bits=product.bit_length(),
        smallest_q=min(q), scalar_root_orders='PASS', copied_current_fixture='EXACT',
        compiled_cpp=False, ran_openfhe=False, ran_fft_or_ntt=False, sampled_randomness=False,
        tested_negative_controls_live=False)
    text = json.dumps(report, indent=2, ensure_ascii=False)+'\n'
    if args.output:
        args.output.write_text(text)
    print(text, end='')

if __name__ == '__main__':
    main()
