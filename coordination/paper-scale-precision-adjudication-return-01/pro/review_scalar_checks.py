#!/usr/bin/env python3
"""Read-only ZIP/log and scalar audit. No FHE, transform, compiler, or network.
Usage: python review_scalar_checks.py INPUT.zip --output CHECK_RESULTS.json
The precision tolerances below check decimal serialization, NOT FHE acceptance.
"""
from __future__ import annotations
import argparse
from decimal import Decimal as D, getcontext
from fractions import Fraction as F
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import stat
import sys
import zipfile

SIZE = 2046500
SHA = '1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da'
MANIFEST_SHA = 'd19303d2d6fd5364f14c3076ed2d3f306a6e90da4d11d257deab0e6b1e7e2c98'
SOURCE = '9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e'
ANCHORS = [0, 1, 256, 257, 512, 513, 768, 769, 1023, 16383]
DIV = 1099510054913
MULT = [1152921504589938689, 1152921504592429057, 1152921504592822273,
        1152921504593412097, 1152921504595640321, 1152921504595968001,
        1152921504597016577, 1152921504598720513]
BASE = [1125899904679937, 1125899903827969]
ROOTS = [26113207984,150640639383,100545759574150,31693996050849,
         88651361085495,9679305630873,24428769072221,18776242964106,
         5821397352863,33888991361320,121567553]
getcontext().prec = 190
if hasattr(sys, 'set_int_max_str_digits'):
    sys.set_int_max_str_digits(0)


def require(ok: bool, message: str) -> None:
    if not ok:
        raise ValueError(message)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def decimal(x: F) -> D:
    return D(x.numerator) / D(x.denominator)


def multiply(a, b):
    return a[0]*b[0]-a[1]*b[1], a[0]*b[1]+a[1]*b[0]


def add(a, b):
    return a[0]+b[0], a[1]+b[1]


def subtract(a, b):
    return a[0]-b[0], a[1]-b[1]


def norm(a):
    return max(abs(a[0]), abs(a[1]))


def exact_input(s: int):
    t = s//2
    a = F(1015, 1024)-F(t % 16, 65536)+F(s, 2**75)
    b = F(1+(t//16) % 8, 1024)*(-1 if (t//512) % 2 else 1)
    return [(a,b),(-b,a),(-a,-b),(b,-a)][(t//128) % 4]


def strip_time(line: str) -> str:
    return re.sub(r'^\d{4}-\d\d-\d\dT\S+Z ', '', line)


def audit(path: Path) -> dict:
    raw = path.read_bytes()
    require(len(raw) == SIZE and sha(raw) == SHA, 'Input archive identity')
    with zipfile.ZipFile(path) as archive:
        infos = archive.infolist()
        names = [i.filename for i in infos]
        require(len(names) == len(set(names)) == 154, 'Unique 154 ZIP members')
        for i in infos:
            p = PurePosixPath(i.filename)
            mode = (i.external_attr >> 16) & 0xffff
            require(not i.is_dir() and not p.is_absolute() and '..' not in p.parts
                    and '\\' not in i.filename and ':' not in i.filename
                    and p.as_posix() == i.filename and stat.S_ISREG(mode),
                    'Unsafe/nonregular ZIP member: '+i.filename)
        require(archive.testzip() is None, 'ZIP CRC')
        data = {name: archive.read(name) for name in names}
    require(sha(data['MANIFEST.json']) == MANIFEST_SHA, 'Manifest identity')
    manifest = json.loads(data['MANIFEST.json'])
    require(manifest['manifest_self_excluded'] is True, 'Manifest self exclusion')
    payloads = manifest['files']
    require(len(payloads) == 153 and {p['path'] for p in payloads} == set(names)-{'MANIFEST.json'},
            'Manifest exact closure')
    for p in payloads:
        require(len(data[p['path']]) == p['bytes'] and sha(data[p['path']]) == p['sha256'],
                'Manifest mismatch: '+p['path'])
    result = {'method':'stdlib ZIP, integer/Fraction, Decimal(190); no transform or encrypted computation',
              'input':{'bytes':len(raw),'sha256':sha(raw),'manifest_sha256':MANIFEST_SHA,
                       'regular_members':154,'verified_payloads':153,'crc':'PASS'}, 'hosts':{}}
    scales = [F(2**100)]
    for m in MULT:
        scales.append(scales[-1]**2 / (DIV*m))
    eps = D(2)**-80
    for host, filename in [('linux','LINUX_RAW.log'),('windows','WINDOWS_LF.log')]:
        prefix = 'evidence/signed-diagnostic-run/'
        log_bytes = data[prefix+filename]
        lines = log_bytes.decode('utf-8-sig').splitlines()
        plain = [strip_time(l) for l in lines]
        live = [(n,t[4:]) for n,t in enumerate(plain,1) if t.startswith('61: ')]
        begin = [i for i,(_,t) in enumerate(live) if t.startswith('BEGIN test=paper_')]
        end = [i for i,(_,t) in enumerate(live) if t.startswith('COMPLETE test=paper_')]
        require(len(begin) == len(end) == 1 and begin[0] < end[0], host+' one live stream')
        stream = live[begin[0]:end[0]+1]
        require(SOURCE in stream[0][1] and 'chain_count=1' in stream[0][1]
                and 'result=FAIL' in stream[-1][1], host+' source/chain/outcome')
        obs, receipts, profiles, misses = {}, [], [], []
        for line, text in stream:
            match = re.fullmatch(r'OBS field=(\S+) value=(\S+)', text)
            if match:
                require(match[1] not in obs, 'Duplicate numeric field')
                value = D(match[2]); require(value.is_finite(), 'Nonfinite field')
                obs[match[1]] = (value, line)
            if text.startswith('RECEIPT '):
                receipts.append((line, dict(re.findall(r'(\w+)=(\S+)', text))))
            if text.startswith('PROFILE '):
                profiles.append(tuple(map(int, re.findall(r'=(\d+)', text))))
            if text.startswith('OBS numeric_gate=FAIL label='):
                misses.append({'line':line,'label':text.split('label=',1)[1]})
        require(len(stream)==979 and len(obs)==835 and len(receipts)==9 and len(misses)==7,
                host+' stream inventory')
        wanted_profiles=[]
        q_all=BASE+list(reversed(MULT))+[DIV]
        for f in range(8):
            pairs=list(zip(q_all,ROOTS)); pairs=pairs[:10-f]+[pairs[-1]]
            wanted_profiles += [(f,t,q,root) for t,(q,root) in enumerate(pairs)]
        require(profiles == wanted_profiles*2, host+' two identical exact profile passes')
        receipt_rows=[]
        for r,(line,receipt) in enumerate(receipts):
            require(int(receipt['operation'])==r and F(int(receipt['exact_n']),int(receipt['exact_d']))==scales[r],
                    host+' exact scale receipt')
            expected={'family':min(r,7),'local_level':2 if r==8 else 1,'towers':10-r,
                      'recorded_exp2':100,'degree':2,'terminal':int(r==8)}
            require(all(int(receipt[k])==v for k,v in expected.items()), host+' receipt metadata')
            receipt_rows.append({'round':r,'line':line,'exact_rational_match':True,
                                 'numerator_bits':scales[r].numerator.bit_length(),
                                 'denominator_bits':scales[r].denominator.bit_length(),
                                 'S_over_2pow100':decimal(scales[r])/D(2)**100})
        def value(field): return obs[field][0]
        def vector(field): return value(field+'.real'),value(field+'.imag')
        checks = {k:D(0) for k in ['printed_w0_consistency','E_minus_I_minus_A',
                                  'I_from_E0','A_from_E0','L_from_previous','aggregate_max_rounding']}
        rows=[]; ratios=[]
        for anchor in ANCHORS:
            exact_z=exact_input(anchor); z=tuple(map(decimal,exact_z))
            e0=vector(f'diag.fresh.anchor_{anchor}.E'); fresh=add(z,e0); p=fresh; previous=fresh
            checks['printed_w0_consistency']=max(checks['printed_w0_consistency'],
                norm(subtract(vector(f'diag.fresh.anchor_{anchor}.w0'),fresh)))
            for r in range(1,9):
                exact_z=multiply(exact_z,exact_z); z=tuple(map(decimal,exact_z)); p=multiply(p,p)
                field=f'diag.round_{r}.anchor_{anchor}'
                e,i,a,l=[vector(field+'.'+term) for term in ['E','I','A','L']]
                actual=add(z,e)
                comparisons={'E_minus_I_minus_A':subtract(e,add(i,a)),
                             'I_from_E0':subtract(i,subtract(p,z)),
                             'A_from_E0':subtract(a,subtract(actual,p)),
                             'L_from_previous':subtract(l,subtract(actual,multiply(previous,previous)))}
                for k,v in comparisons.items(): checks[k]=max(checks[k],norm(v))
                ratios.append(norm(i)/norm(a))
                previous=actual
        for r in range(1,9):
            maxima={}
            for term in ['E','I','A','L']:
                v,anchor,component=max((abs(value(f'diag.round_{r}.anchor_{a}.{term}.{c}')),a,c)
                    for a in ANCHORS for c in ['real','imag'])
                maxima[term]={'value':v,'anchor':anchor,'component':component,
                             'line':obs[f'diag.round_{r}.anchor_{anchor}.{term}.{component}'][1]}
                reported=f'round_{r}.anchor_max_component_error' if term=='E' else f'diag.round_{r}.{term}_anchor_max_component'
                checks['aggregate_max_rounding']=max(checks['aggregate_max_rounding'],abs(v-value(reported)))
            field=f'diag.round_{r}.anchor_{maxima["E"]["anchor"]}'
            component=maxima['E']['component']
            rows.append({'round':r,'maxima':maxima,'signed_at_E_max':
                         {t:value(field+'.'+t+'.'+component) for t in ['E','I','A','L']}})
        require(checks['printed_w0_consistency']<D('1e-98'), host+' printed w0')
        require(checks['aggregate_max_rounding']<D('1e-66'), host+' aggregate decimal rounding')
        require(all(v<D('1e-120') for k,v in checks.items() if k not in
                    ['printed_w0_consistency','aggregate_max_rounding']),host+' signed identities')
        audit_json=json.loads(data[prefix+host.upper()+'_VERIFICATION.json'])
        require(audit_json['sha256']==sha(log_bytes),host+' supplied audit raw hash')
        for binding in audit_json['actual_pass_bindings']:
            require(re.search(r'\bStart\s+'+str(binding['number'])+': '+re.escape(binding['name'])+r'$',
                              plain[binding['start_line']-1]) is not None,host+' bound Start')
            command=plain[binding['command_line']-1].split('Test command: ',1)[1]
            require(re.findall(r'"([^\"]*)"|(\S+)',command),host+' argv syntax')
            argv=[a or b for a,b in re.findall(r'"([^\"]*)"|(\S+)',command)]
            require(argv==binding['command'],host+' bound argv')
            record=plain[binding['result_line']-1]
            require(re.search(r'Test\s+#\s*'+str(binding['number'])+': '+re.escape(binding['name']),record)
                    and re.search(r'\bPassed\s+'+re.escape(binding['seconds'])+r' sec$',record),host+' bound PASS')
        require(len(audit_json['actual_pass_bindings'])==123,host+' 123 bound executions, not unique tests')
        payload='\n'.join(t for _,t in stream)+'\n'
        require(sha(payload.encode())==audit_json['paper']['live_payload_sha256'],host+' payload hash')
        replay=[i for i,t in enumerate(plain) if t==stream[0][1]]
        require(len(replay)==1,host+' one unprefixed CTest replay')
        replay_end=next(i for i in range(replay[0],len(plain)) if plain[i]==stream[-1][1])
        replay_lines=plain[replay[0]:replay_end+1]
        replay_noise=[replay[0]+i+1 for i,t in enumerate(replay_lines) if t=='Errors while running CTest']
        require([t for t in replay_lines if t!='Errors while running CTest']==[t for _,t in stream],
                host+' identical CTest replay excluding identified CTest stderr')
        coefficient=max(v for k,(v,_) in obs.items() if k.endswith('coefficient_max_over_scale'))
        selected={k:{'value':v,'line':line} for k,(v,line) in obs.items()
                  if not re.match(r'diag\.(fresh|round_\d+)\.anchor_\d+\.',k)}
        result['hosts'][host]={'raw_path':prefix+filename,'raw_bytes':len(log_bytes),'raw_sha256':sha(log_bytes),
            'stream_lines':[stream[0][0],stream[-1][0]],'payload_sha256':sha(payload.encode()),
            'field_count':len(obs),'identical_replay_lines':[replay[0]+1,replay_end+1],'ctest_stderr_lines':replay_noise,
            'numeric_misses':misses,'receipt_checks':receipt_rows,'signed_identity_max_discrepancies':checks,
            'same_anchor_min_I_over_A':min(ratios),'rounds':rows,'selected_fields':selected,
            'final_full_over_2powminus80':value('final.full_max_component_error')/eps,
            'ordinary_Horner_rounding_estimate_16N2B_u':16*D(32768)**2*coefficient*D(2)**-512,
            '123_supplied_Start_argv_PASS_bindings_checked':True,
            'limitations':['Not original local capture/Git/parser execution','No cryptographic run',
                          'Signed values are rounded log observations, not full-slot evidence']}
    N,h,sigma,S=D(32768),D(128),D('3.19'),D(2)**100
    rho_min=((D(1015)/1024-D(15)/65536)**2+(D(1)/1024)**2).sqrt()
    rho_max=((D(1015)/1024+D(16383)*D(2)**-75)**2+(D(8)/1024)**2).sqrt()
    gains=[{'round':r,'complex_gain_lower':D(2**r)*rho_min**(2**r-1),
            'complex_gain_upper':D(2**r)*rho_max**(2**r-1)} for r in range(1,9)]
    result['analytic_scalars']={'gate_2powminus80':eps,'rho_lower':rho_min,'rho_upper':rho_max,
        'output_magnitude_lower':rho_min**256,'output_magnitude_upper':rho_max**256,'derivative_bounds':gains,
        'zero_added_sufficient_fresh_component_budget_all_stages':eps/(D(2).sqrt()*max(g['complex_gain_upper'] for g in gains)),
        'zero_added_sufficient_fresh_component_budget_terminal':eps/(D(2).sqrt()*gains[-1]['complex_gain_upper']),
        'heuristic_integer_coefficient_RMS':sigma*(2*N/3+1+h).sqrt(),
        'heuristic_mean_slot_component_RMS':sigma*(N/2*(2*N/3+1+h)).sqrt()/S,
        'nearest_coefficient_rounding_slot_bound':N/(2*S),
        'S8_over_S0':decimal(scales[8])/S,'Qbase_over_S0':D(BASE[0]*BASE[1])/S,
        'ideal_terminal_sufficient_headroom_fraction':2*decimal(scales[8])*rho_max**256/D(BASE[0]*BASE[1])}
    headroom=D(result['analytic_scalars']['ideal_terminal_sufficient_headroom_fraction'])
    result['analytic_scalars']['ideal_headroom_b_limit']=(1/headroom).ln()/D(2).ln()/256
    for host,report in result['hosts'].items():
        b=D(report['selected_fields']['diag.round_7.coefficient_max_over_scale']['value'])*decimal(scales[7])
        report['theorem_4_8_antecedent_lower_bound_ratio']=2*N*b*b/D(BASE[0]*BASE[1]*MULT[7])
        report['observed_final_full_component_bits']=-D(report['selected_fields']['final.full_max_component_error']['value']).ln()/D(2).ln()
        supplied=json.loads(data['evidence/signed-diagnostic-run/'+host.upper()+'_SIGNED_ERROR.json'])
        for row,other in zip(report['rounds'],supplied['rounds']):
            require(row['round']==other['round'],host+' supplied signed round')
            for term in ['E','I','A','L']:
                mine,theirs=row['maxima'][term],other['maxima'][term]
                require(mine['anchor']==theirs['anchor'] and abs(mine['value']-D(theirs['value']))<D('1e-66'),
                        host+' supplied signed maximum/anchor')
        report['supplied_signed_32_maxima_and_anchors_independently_matched']=True
    original=json.loads(data['evidence/RUN_TERMINAL_01.json'])
    signed=json.loads(data['evidence/signed-diagnostic-run/RUN_TERMINAL_01.json'])
    require(original['databaseId']==33971779479 and original['attempt']==1 and original['event']=='push'
            and original['headSha']=='b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89'
            and original['conclusion']=='failure', 'Original supplied terminal metadata')
    require(signed['run']['id']==33978202814 and signed['run']['run_attempt']==1
            and signed['run']['event']=='push' and signed['run']['head_sha']==SOURCE
            and signed['run']['status']=='completed' and signed['run']['conclusion']=='failure',
            'Signed supplied terminal metadata')
    old_linux=data['evidence/LINUX_RAW.log'].decode('utf-8-sig').splitlines()
    old_windows=data['evidence/WINDOWS_LF.log'].decode('utf-8-sig').splitlines()
    require(not any('61: BEGIN test=paper_' in t for t in old_linux), 'Original Linux has no paper runtime')
    require(any('error:' in t and 'array-bounds' in t for t in old_linux), 'Original Linux compiler failure')
    old_live=[(i,t.split('61: ',1)[1]) for i,t in enumerate(old_windows,1) if '61: ' in t]
    require(any('round_4.anchor_max_component_error' in t for _,t in old_live)
            and not any('round_5.' in t for _,t in old_live), 'Original Windows observation boundary')
    result['supplied_run_metadata_checked']={'original':{'run':33971779479,'attempt':1,'event':'push',
            'jobs':[{k:j[k] for k in ['databaseId','name','conclusion']} for j in original['jobs']]},
        'signed':{'run':33978202814,'attempt':1,'event':'push',
            'jobs':[{k:j[k] for k in ['databaseId','name','conclusion']} for j in signed['jobs']]},
        'boundary':'Packet consistency only, not a live hosting query or original capture reproduction'}
    preflight=json.loads(data['evidence/signed-diagnostic-run/RAW_PREFLIGHT.json'])
    windows=data['evidence/signed-diagnostic-run/WINDOWS_LF.log']
    reconstructed=windows.replace(b'\n',b'\r\n')
    require(len(reconstructed)==preflight['logs'][1]['original_decoded_bytes'] and
            sha(reconstructed)==preflight['logs'][1]['original_decoded_sha256'], 'Reconstructed CRLF hash')
    result['windows_CRLF_reconstruction']={'bytes':len(reconstructed),'sha256':sha(reconstructed),
            'limitation':'Mechanical reconstruction from supplied LF; unavailable original capture NOT reproduced'}
    result['result']='PASS: bounded archive/log/scalar audit; original FHE precision result remains FAIL'
    return result


def main() -> None:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive',type=Path)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    try:
        result=audit(args.archive)
        args.output.write_text(json.dumps(result,indent=2,default=str)+'\n',encoding='utf-8')
    except (OSError,ValueError,KeyError,zipfile.BadZipFile) as exc:
        raise SystemExit('AUDIT FAILED: '+str(exc)) from exc
    print(result['result'])
    for host,report in result['hosts'].items():
        print(host, 'stream',report['stream_lines'], 'fields',report['field_count'],
              'numeric misses',len(report['numeric_misses']), 'E8/gate',
              format(report['final_full_over_2powminus80'],'.12f'))

if __name__=='__main__':
    main()
