#!/usr/bin/env python3
"""Read-only, bounded exact scalar/document checks. No supplied module is imported.
Usage: python -B -I bounded_checks.py INPUT_ROOT OUTPUT_JSON
This is NOT an encoding, transform, sampling, compiler or FHE test.
"""
from fractions import Fraction as F
from decimal import Decimal, localcontext
from pathlib import Path
import hashlib
import json
import sys


def main(root: Path) -> dict:
    checks, observations = [], {}
    def check(name: str, ok: bool):
        checks.append({'id': name, 'pass': bool(ok)})
        if not ok:
            raise ValueError('check failed: ' + name)
    def load(rel):
        return json.loads((root / rel).read_text(encoding='utf-8'))
    def approx(v):
        with localcontext() as ctx:
            ctx.prec = 24
            return str(Decimal(v.numerator) / Decimal(v.denominator))
    T = F(1, 2**80)
    for host in ('LINUX', 'WINDOWS'):
        d = load(f'context/current/coordination/fs-endpoint-live-run-01/{host}_AUDIT.json')
        m = {r['id']: r for r in d['maxima']}
        check(host+'_retained_fail', d['status']['E80_disposition']=='FAIL' and d['status']['chain_count']==1 and d['status']['ctest_exit_code']==8)
        E,I,A = (m[k] for k in ('E8','I8','A8'))
        check(host+'_E_lower_over_T',F(E['lower'])>T)
        check(host+'_I_lower_over_T',F(I['lower'])>T)
        check(host+'_A_upper_below_T',F(A['upper'])<T)
        lower = F(I['lower'])-F(A['upper'])
        check(host+'_reverse_triangle_lower_over_T', lower>T)
        tup=E['signed_tuple']
        for component in ('real','imag'):
            residual=F(tup['E8.'+component])-F(tup['I8.'+component])-F(tup['A8.'+component])
            check(host+'_same_row_E_I_A_print_consistency_'+component, abs(residual)<F(1,10**120))
        observations[host]={'source':'ed5fd192a89d6d4728ad295e87cf06a3f4abc832', 'E8_component_over_T':approx(F(E['magnitude'])/T), 'I8_component_over_T':approx(F(I['magnitude'])/T), 'A8_component_over_T':approx(F(A['magnitude'])/T), 'reverse_triangle_lower_over_T':approx(lower/T), 'E8_argmax':{'slot':E['slot'],'component':E['component']}, 'qualification':'Scalar comparisons of retained conditional-observer audit bounds; not a new observation or all-row replay.'}
    # Bounded known-summary norm implications. No new trial or missing precision inferred.
    for host, value in [('LINUX','2.5905123324714234e-26'),('WINDOWS','3.4805603371613677e-26')]:
        e=F(value)
        check('S116_'+host+'_summary_component_below_T', e<T)
        check('S116_'+host+'_sqrt2_summary_upper_below_T',2*e*e<T*T)
    e=F('1.46231410388e-25')
    check('annulus_summary_complex_below_T',e<T)
    observations['S116_and_annulus']='Checks use rounded published summary values with large slack; precise historical execution remains the source evidence, not these truncated values.'
    cert=load('coordination/public-s100-ecd-cell-20260909/green-evidence/execution/certificate/RESULT.json')
    intake=load('coordination/public-s100-ecd-cell-20260909/GREEN_INTAKE.json')
    agreement=load('coordination/public-s100-ecd-cell-20260909/green-evidence/execution/OUTCOME_AGREEMENT.json')
    margin=cert['strict_minimum_margin']
    check('certificate_adopted_status_counts',cert['status']=='ECD_ROUNDING_CERTIFIED' and cert['counts']=={'CERTIFIED':32768,'REFUTED':0,'INCONCLUSIVE':0})
    check('certificate_summary_margin_positive', 0<int(margin['numerator'])<2**223 and int(margin['denominator'])==2**224 and margin['index']==26696)
    check('certificate_intake_margin_agreement',margin==intake['minimum_margin'])
    check('certificate_status_exit_agreement',agreement['status']==cert['status'] and all(type(agreement[k]) is int and agreement[k]==0 for k in ['declared_exit','observed_exit','saved_exit','table_intake_exit']))
    check('certificate_preserves_original_fail',cert['original_S100_E80']=='UNCHANGED_FAIL')
    # Recheck only a fixed scalar consequence, not the completed Ecd proof.
    R,delta=F(495621,500000),F(1,2**86)
    encoding_ratio=256*(R+delta)**255*delta/T
    check('encoding_only_ratio_existing_interval',F(424504491252,10**12)<=encoding_ratio<F(424504491253,10**12))
    check('encoding_only_not_exhaust_E80',encoding_ratio<F(1,2))
    check('current_cap_nonwrap_premise',R<F(127,128))
    observations['encoding_only_ratio_decimal']=approx(encoding_ratio)
    observations['scope']='No coefficient-cell reclassification, inverse, full input, new polynomial, noise or ciphertext was generated.'
    atlas=load('provenance/ATLAS_INPUT_MANIFEST.json')
    rows={x['path']:x for x in atlas['files']}
    for name in ['double_ckks.cpp','repeated_mult2.cpp','paper_h128_client_keypair.cpp']:
        rel='project/src/'+name
        check('current_same_as_atlas_'+name,hashlib.sha256((root/rel).read_bytes()).hexdigest()==rows[rel]['sha256'])
    old=(root/'coordination/initial-lift-nonwrap-20260909/pro/checks/bound_source/project/src/high_precision_client_io.cpp').read_text()
    new=(root/'project/src/high_precision_client_io.cpp').read_text()
    check('atlas_codec_bound_copy',hashlib.sha256(old.encode()).hexdigest()==rows['project/src/high_precision_client_io.cpp']['sha256'])
    include='#include "openfhe_2023_1788/public_s100_encoding_probe.h"\n'
    start='namespace diagnostic {\nEncodingInspection InspectFixedS100PublicEncoding('
    end='}  // namespace diagnostic\n\n'
    check('probe_single_addition_markers',new.count(include)==1 and new.count(start)==1 and new.count(end)==1)
    s=new.index(start); t=new.index(end,s)+len(end)
    stripped=(new[:s]+new[t:]).replace(include,'',1)
    check('existing_codec_bytes_preserved_after_probe_removal',stripped==old)
    observations['codec_delta']='One header include plus separate diagnostic function. This is a textual/source bridge, not historical binary equivalence.'
    return {'schema':'completion-contract-bounded-checks-v1','status':'PASS','checks':checks,'check_count':len(checks),'observations':observations,'prohibited_executions':{'FFT_NTT':0,'full_input_generation':0,'encoding':0,'sampling':0,'FHE':0,'compile_CI_accounts':0}}

if __name__=='__main__':
    try:
        if len(sys.argv)!=3: raise ValueError('expected INPUT_ROOT OUTPUT_JSON')
        root=Path(sys.argv[1]).resolve(strict=True)
        target=Path(sys.argv[2])
        result=main(root)
        with target.open('x',encoding='utf-8') as f: json.dump(result,f,ensure_ascii=False,indent=2); f.write('\n')
        print(f"PASS: {result['check_count']} bounded checks; no transform/FHE/build")
    except (OSError, ValueError, KeyError, TypeError) as exc:
        print(f'FAIL: {exc}',file=sys.stderr)
        raise SystemExit(1)
