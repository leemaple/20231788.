#!/usr/bin/env python3
"""Own stdlib-only input, patch and scalar audit. NO imported project/archive code.
No compiler, numeric codec, transform, crypto, CI or network operation is used.
The only subprocesses are git apply --check and git apply on disposable bytes.
"""
from __future__ import annotations
import argparse
import hashlib
import io
import json
import math
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import sys
import tempfile
import zipfile
from decimal import Decimal, localcontext
from fractions import Fraction as F

OUTER_SHA = 'c046efc8fc95ced2e68da941dab02252d6ac76592e619571d6eb82dfc092ad74'
SOURCE_SHA = '1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da'
DECISION_SHA = 'f20f89a67e233cd9bc39553dd20b923dbc663c2939dc79dd0fafd756ec58fba5'

def sha(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()

def require(ok: bool, message: str) -> None:
    if not ok:
        raise ValueError(message)

def archive(data: bytes, expected_size: int, expected_sha: str,
            manifest_sha: str, root: str = '') -> tuple[dict[str, bytes], dict]:
    require(len(data) == expected_size and sha(data) == expected_sha, 'archive identity')
    with zipfile.ZipFile(io.BytesIO(data)) as z:
        names, folded = set(), set()
        for i in z.infolist():
            p = PurePosixPath(i.filename)
            require(i.filename and not p.is_absolute() and str(p) == i.filename and
                    all(s not in ('', '.', '..') for s in p.parts) and
                    not any(c in i.filename for c in ('\\', ':', '\x00')),
                    'unsafe archive path')
            require(not i.is_dir() and not i.flag_bits & 1, 'directory or encrypted member')
            mode = (i.external_attr >> 16) & 0xffff
            require(stat.S_IFMT(mode) in (0, stat.S_IFREG), 'nonregular member')
            require(i.filename not in names and i.filename.casefold() not in folded, 'duplicate path')
            names.add(i.filename); folded.add(i.filename.casefold())
        require(z.testzip() is None, 'CRC failure')
        payload = {n: z.read(n) for n in names}
    mp = root + 'MANIFEST.json'
    require(sha(payload[mp]) == manifest_sha, 'manifest identity')
    m = json.loads(payload[mp])
    require(m['manifest_self_excluded'] is True, 'manifest is not self-excluding')
    entries = m['files']
    require(len({e['path'] for e in entries}) == len(entries), 'duplicate manifest entry')
    require({root + e['path'] for e in entries} == names - {mp}, 'manifest closure')
    for e in entries:
        b = payload[root + e['path']]
        require(len(b) == e['bytes'] and sha(b) == e['sha256'], 'payload identity: ' + e['path'])
    return {n[len(root):]: b for n, b in payload.items()}, {
        'bytes': len(data), 'sha256': sha(data), 'regular_members': len(names),
        'manifest_payloads': len(entries), 'manifest_sha256': manifest_sha,
        'safe_paths_crc_exact_manifest': 'PASS'}

def p2(e: int) -> F:
    return F(1 << e) if e >= 0 else F(1, 1 << -e)

def rational(x: F) -> dict:
    return {'numerator': str(x.numerator), 'denominator': str(x.denominator)}

def canonical(s: str) -> bool:
    zero = '+0.' + '0' * 109 + 'e+00000'
    return s == zero or (re.fullmatch(r'[+-][1-9]\.[0-9]{109}e[+-][0-9]{5}', s,
                                     flags=re.ASCII) is not None and not s.endswith('e-00000'))

def run(input_path: Path, package: Path) -> dict:
    if hasattr(sys, 'set_int_max_str_digits'):
        sys.set_int_max_str_digits(100000)
    outer, a = archive(input_path.read_bytes(), 2125153, OUTER_SHA,
        '92b2f1dcfd51620c2bc887292acdc40a10c3aacc0e7b47a54240c012cb3d6fed')
    src, b = archive(outer['input/paper-scale-precision-adjudication-9f6c8eae.zip'],
        2046500, SOURCE_SHA, 'd19303d2d6fd5364f14c3076ed2d3f306a6e90da4d11d257deab0e6b1e7e2c98')
    dec, c = archive(outer['review/paper-scale-precision-adjudication-decision-v1.zip'],
        60905, DECISION_SHA, '3b0a977496260c799101934f605c285c77065146ccf5217b7906296e58893b91',
        'precision_adjudication_decision_v1/')
    result = {'schema': 'fs-endpoint-own-static-scalar-audit-v1', 'archives': [a,b,c],
              'archive_code_executed': False, 'transforms_or_codecs_executed': False}
    result['paper'] = {'bytes': len(src['paper/PAPER-2023-1788.pdf']),
                       'sha256': sha(src['paper/PAPER-2023-1788.pdf'])}
    q = [1125899904679937,1125899903827969,1152921504598720513,1152921504597016577,
         1152921504595968001,1152921504595640321,1152921504593412097,1152921504592822273,
         1152921504592429057,1152921504589938689,1099510054913]
    scales = [p2(100)]
    for i in range(1,9):
        scales.append(scales[-1] ** 2 / (q[-1] * q[10-i]))
        closed = F(1 << (100 * (1 << i)), math.prod((q[-1]*q[10-j]) ** (1 << (i-j))
                                                  for j in range(1,i+1)))
        require(scales[-1] == closed, 'scale recurrence / closed product')
    scale_text = f'{scales[8].numerator}/{scales[8].denominator}'
    result['scale'] = {'recurrence_equals_independent_closed_product_all_nine': True,
                       'S8_numerator_decimal_digits': len(str(scales[8].numerator)),
                       'S8_denominator_decimal_digits': len(str(scales[8].denominator)),
                       'S8_fraction_ascii_sha256': sha(scale_text.encode()),
                       'S8_is_not_S0': scales[8] != scales[0]}
    # Modular integer index mapping, NOT a numerical transform.
    exponents = [pow(5,s,65536) for s in range(16384)]
    bins = [(e-1)//2 for e in exponents]
    require(len(set(exponents)) == 16384 and all(e % 4 == 1 for e in exponents), 'slot exponents')
    require(set(bins) == set(range(0,32768,2)), 'slot bins are an even-bin permutation')
    require(set(exponents).isdisjoint({(-e) % 65536 for e in exponents}), 'conjugate sets overlap')
    result['integer_slot_map'] = {'slots': len(bins), 'permutation_of_all_even_bins': True,
                                  'disjoint_conjugates': True,
                                  'ordered_bins_ascii_sha256': sha(','.join(map(str,bins)).encode())}
    checks = []
    for p in (512,768):
        u=p2(-p); gamma3=3*u/(1-3*u); root=2*(2*F(22,7)*(2*u/(1-2*u))+8*u)
        dmajor=(1+gamma3)/(1-16*128*u)-1
        hmajor=((32768+4)*128*u)/(1-(32768+4)*128*u)
        powmajor=p2(256)*(255*16*u)/(1-255*16*u)
        require(root < 64*u and dmajor < 4096*u and hmajor < p2(24)*u and powmajor < p2(270)*u,
                'conditional rational majorants')
        checks.append({'binary_bits': p, 'root64u': True, 'D4096uK': True,
                       'H2pow24uK': True, 'P2pow270u': True})
    # This is a formal modulus cap, not the actual terminal C or an observed K.
    cap = F(32768*(q[0]*q[1]-1),2) / scales[8]
    require(cap < p2(14), 'formal terminal K cap')
    require(3**255 < 2**405, 'radius 3/2 derivative < 2^158')
    result['conditional_scalar_checks'] = {'majorants': checks,
        'terminal_formal_K_cap_at_most_2pow14': True,
        'terminal_actual_C_or_K_measured': False,
        'radius_3_over_2_derivative_lt_2pow158': True,
        'premises_are_not_library_certificates': True}
    examples=[]
    for k in (0,14,32):
        for p in (512,768):
            u=p2(-p); D=p2(12)*u*p2(k); H=p2(24)*u*p2(k)
            P=p2(270)*u; Q=P+p2(263)*D; R=p2(264)*u
            Bs=[D+R,D+P+R,Q+P+R,D+Q+R]
            identity=Bs[1]+Bs[2]+Bs[3]+2*R
            require(max([D,H,P,Q,R,identity,*Bs]) <= p2(-128), 'synthetic allowance example')
            examples.append({'synthetic_k_exponent': k,'binary_bits':p,'all_named_bounds_below_ceiling':True})
    result['synthetic_allowance_examples_not_live_data']=examples
    zero='+0.'+'0'*109+'e+00000'; one='+1.'+'0'*109+'e+00000'
    valid=[zero,one,'-5.'+'0'*109+'e-00001']
    invalid=['nan','inf','-0.'+'0'*109+'e+00000','+0.'+'0'*109+'e-00000',
             '+0.'+'0'*109+'e+00001','+0.1'+'0'*108+'e+00000','+1.'+'0'*109+'e-00000',
             '+1.'+'0'*109+'E+00000','+1.'+'0'*109+'e+0000','+1.'+'0'*108+'e+00000',
             '1.'+'0'*109+'e+00000',one+'\n',one+'\r',' '+one,one+'\t']
    require(all(canonical(s) for s in valid) and not any(canonical(s) for s in invalid), 'decimal grammar')
    for v, expected in ((10**110+5,10**109), (10**110+15,10**109+2)):
        quotient, remainder=divmod(v,10)
        rounded=quotient+(remainder>5 or (remainder==5 and quotient%2==1))
        require(rounded==expected, 'integer half-even fixture')
    result['synthetic_decimal_grammar']={'valid_count':3,'invalid_count':15,
        'exact_integer_half_even_fixture_count':2,'numeric_codec_executed':False,
        'live_sidecar_generated':False}
    logs={}
    for host,path in [('linux','evidence/signed-diagnostic-run/LINUX_RAW.log'),
                      ('windows','evidence/signed-diagnostic-run/WINDOWS_LF.log')]:
        lines=src[path].decode().splitlines()
        primary=[(i+1,l.split(' 61: ',1)[1]) for i,l in enumerate(lines) if ' 61: ' in l]
        obs={}
        for _,l in primary:
            m=re.fullmatch(r'OBS field=(\S+) value=(\S+)',l)
            if m:
                require(m[1] not in obs, 'duplicate primary field')
                obs[m[1]]=m[2]
        begin=[i for i,l in primary if l.startswith('BEGIN test=')]
        completion=[(i,l) for i,l in primary if l.startswith('COMPLETE test=')]
        scale=[i for i,l in primary if l.startswith('RECEIPT operation=')]
        misses=[i for i,l in primary if l.startswith('OBS numeric_gate=FAIL')]
        require(len(begin)==1 and len(completion)==1 and 'result=FAIL' in completion[0][1] and
                len(scale)==9 and len(misses)==7, 'historical primary run inventory')
        f=F(obs['final.full_max_component_error'])*p2(80)
        with localcontext() as ctx:
            ctx.prec=60
            ratio=format(Decimal(f.numerator)/Decimal(f.denominator),'.12f')
        cleanup=[i for i,l in primary if l=='OBS lifecycle=paper_owner_cleanup owned_absent=8 unrelated_unchanged=2 result=PASS']
        require(len(cleanup)==1, 'cleanup receipt')
        logs[host]={'path':path,'primary_BEGIN_line':begin[0], 'primary_COMPLETE_line':completion[0][0],
                    'scale_receipts':len(scale),'retained_E80_misses':len(misses),
                    'field_count':len(obs),'cleanup_line':cleanup[0],
                    'final_E_over_2pow_minus80_from_printed_decimal':ratio,
                    'replay_not_counted_as_a_trial':True}
    w=src['evidence/signed-diagnostic-run/WINDOWS_LF.log'].decode()
    require('boost-1.92.0-3' in w, 'Windows package version evidence')
    result['historical_logs_only']=logs
    result['windows_logged_boost_package']='1.92.0-3 (references in input are 1.83.0)'
    result['historical_checker_not_executed']={
        'json_bytes':len(dec['CHECK_RESULTS.json']),'json_sha256':sha(dec['CHECK_RESULTS.json']),
        'stdout_bytes':len(dec['CHECK_STDOUT.txt']),'stdout_sha256':sha(dec['CHECK_STDOUT.txt'])}
    # Actual patch applicability and exhaustive identity preservation on supplied engineering bytes.
    bases=json.loads((package/'BASE_HASHES.json').read_text())
    engineering={n.removeprefix('project/'):v for n,v in src.items() if n.startswith('project/')}
    require(len(engineering)==44,'engineering file count')
    with tempfile.TemporaryDirectory(prefix='fs-endpoint-static-') as td:
        root=Path(td)
        for n,v in engineering.items():
            (root/n).parent.mkdir(parents=True,exist_ok=True); (root/n).write_bytes(v)
        commands=[]
        for extra in (['--check'],[]):
            args=['git','apply',*extra,str((package/'RED.patch').resolve())]
            done=subprocess.run(args,cwd=root,capture_output=True,text=True,check=False)
            require(done.returncode==0, 'git apply: '+done.stderr)
            commands.append({'argv':args,'exit_code':done.returncode,'stdout':done.stdout,'stderr':done.stderr})
        for entry in bases['changed_paths']:
            n=entry['path'];old=engineering.get(n);new=(root/n).read_bytes()
            require((None if old is None else sha(old))==entry['base_sha256'],'base hash')
            require((None if old is None else len(old))==entry['base_bytes'],'base size')
            require(new==(package/'files'/n).read_bytes() and sha(new)==entry['result_sha256'] and
                    len(new)==entry['result_bytes'],'full file/patch mismatch')
        for entry in bases['unchanged_files']:
            v=(root/entry['path']).read_bytes()
            require(v==engineering[entry['path']] and len(v)==entry['bytes'] and sha(v)==entry['sha256'],
                    'unchanged file changed')
        changed=sorted(n for n in set(engineering)|{str(p.relative_to(root)) for p in root.rglob('*') if p.is_file()}
                       if engineering.get(n)!=(root/n).read_bytes())
        require(changed==sorted(e['path'] for e in bases['changed_paths']),'changed path scope')
        old=engineering['tests/paper_full_eight_square_contract_test.cpp'].decode()
        new=(root/'tests/paper_full_eight_square_contract_test.cpp').read_text()
        prefix=new[:new.index('int main(int argc, char** argv)')].replace('#include "paper_endpoint_observer_contract.h"\n','')
        require(prefix==old[:old.index('int main()')],'original test body changed')
        require(new[new.index('    try { Run(); return 0; }'):]==old[old.index('    try { Run(); return 0; }'):],
                'normal main completion changed')
        cmake=engineering['CMakeLists.txt'].decode()
        require(len(re.findall(r'^\s*add_test\(',cmake,flags=re.M))==61,'CTest inventory')
        header=(root/'tests/paper_endpoint_observer_contract.h').read_text()
        names=['Observe','ScaledOneNormExponent','DirectSparseReference768','ExactAbsoluteDifference',
               'AssessDifference','CanonicalDecimal','IsCanonicalDecimal']
        for name in names:
            require(re.search(r'\b'+name+r'\([^;{}]*\);',header,re.S) is not None,'missing API declaration')
            require(not re.search(r'\b'+name+r'\([^;{}]*\)\s*\{',header,re.S),'unexpected helper definition')
        result['patch']={'git_apply_commands':commands,'changed_paths':changed,
             'project_input_files':44,'unchanged_project_files':len(bases['unchanged_files']),
             'normal_body_and_completion_byte_identical':True,'CMake_and_workflow_byte_identical':True,
             'old_oracle_byte_identical':True,'CTest_inventory':61,
             'intentionally_undefined_test_local_functions':names,'Cplusplus_compilation_executed':False,
             'RED_observed':False,'expected_RED':'paper target link/API failure after unchanged old60/five API checkpoint'}
    references=json.loads((package/'SOURCE_REFERENCES.json').read_text())['references']
    refmap={e['alias']:e for e in references}
    areas={'source':src,'outer':outer,'decision':dec}
    for e in references:
        content=areas[e['archive']][e['path']]
        linecount=content.count(b'\n')+(not content.endswith(b'\n'))
        require(len(content)==e['bytes'] and sha(content)==e['sha256'] and
                linecount==e['physical_lf_lines'],'source reference identity')
    citation_ranges=[]
    for document in ('SPEC_REVIEW.md','ENDPOINT_SPEC.md','TEST_PLAN.md','EXECUTION_LEDGER.md'):
        text=(package/document).read_text()
        for match in re.finditer(r'\b([A-Z][A-Z0-9]*):(\d+(?:[–-]\d+)?(?:,\d+(?:[–-]\d+)?)*)',text):
            alias, ranges=match.groups()
            require(alias in refmap,'unknown source alias: '+alias)
            entry=refmap[alias]
            for piece in ranges.split(','):
                pair=re.split('[–-]',piece)
                lo=int(pair[0]);hi=int(pair[-1])
                require(1<=lo<=hi<=entry['physical_lf_lines'],'citation range overflow')
                raw=areas[entry['archive']][entry['path']].split(b'\n')
                excerpt=b'\n'.join(raw[lo-1:hi])
                citation_ranges.append({'document':document,'alias':alias,'start_line':lo,
                                        'end_line':hi,'excerpt_sha256':sha(excerpt)})
    result['citation_audit']={'source_reference_files':len(references),
                              'range_count':len(citation_ranges),'ranges':citation_ranges,
                              'scope':'byte identity and physical line bounds, not automated semantic adjudication'}
    result['result']='PASS_STATIC_SCALAR_ONLY'
    return result

def main() -> None:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_zip',type=Path)
    parser.add_argument('--package',type=Path,default=Path(__file__).resolve().parent)
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    result=run(args.input_zip,args.package)
    text=json.dumps(result,indent=2,sort_keys=True,ensure_ascii=True)+'\n'
    if args.output:
        args.output.write_text(text,encoding='ascii')
    print('PASS_STATIC_SCALAR_ONLY: 3 archives; exact payload closure; scalar bounds; 2-path patch; no compiler/transform/crypto.')

if __name__=='__main__':
    main()
