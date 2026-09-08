#!/usr/bin/env python3
"""Runner intake: scalar reclassification of public enclosures. No transforms.

This checks finite certificate bookkeeping, NOT the inverse-transform proof.
That proof and implementation require Codex review before a GREEN is adopted.
"""
from __future__ import annotations
import argparse,csv,json,sys
from pathlib import Path
BASE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'candidate'))
from interval_core import Interval,UNIT
from rounding_contract import (N,load_bound_fixture,require,classify_cell,cell_margin,
    PAYLOAD_SHA256,STREAM_SHA256,sha256_file)


def main() -> int:
    ap=argparse.ArgumentParser()
    ap.add_argument('result_directory',type=Path)
    args=ap.parse_args()
    _,p=load_bound_fixture()
    r=json.loads((args.result_directory/'RESULT.json').read_text())
    require(r['schema']=='public-s100-ecd-rounding-v1','wrong result schema')
    require(r['public_payload_sha256']==PAYLOAD_SHA256 and r['coefficient_stream_sha256']==STREAM_SHA256,'wrong input')
    require(r['candidate_sha256']==sha256_file(BASE/'candidate/check_rounding.py') and
            r['interval_core_sha256']==sha256_file(BASE/'candidate/interval_core.py'),'wrong candidate bytes')
    table=args.result_directory/'coefficient_cells.tsv'
    require(r['coefficient_cells_sha256']==sha256_file(table),'wrong enclosure table')
    counts={key:0 for key in ('CERTIFIED','REFUTED','INCONCLUSIVE')}
    minimum=None;index=None;total=0
    with table.open(newline='') as src:
        for row in csv.DictReader(src,delimiter='\t'):
            j=int(row['index']); require(j==total and j<N,'nonconsecutive coefficient index')
            require(int(row['actual_p'])==p[j] and int(row['denominator'])==UNIT,'wrong p or denominator')
            re=Interval(int(row['real_lower_num']),int(row['real_upper_num']))
            im=Interval(int(row['imag_lower_num']),int(row['imag_upper_num']))
            require(im.lo<=0<=im.hi,'nonreal inverse')
            status=classify_cell(re,p[j]);margin=cell_margin(re,p[j])
            require(row['cell_status']==status and int(row['strict_margin_num'])==margin,'wrong cell classification')
            counts[status]+=1
            if minimum is None or margin<minimum:minimum,index=margin,j
            total+=1
    require(total==N and counts==r['counts'],'incomplete/wrong result counts')
    require(r['strict_minimum_margin']=={'numerator':str(minimum),'denominator':str(UNIT),'index':index},'wrong minimum')
    expected='ECD_ROUNDING_REFUTED' if counts['REFUTED'] else (
        'INCONCLUSIVE_INTERVAL' if counts['INCONCLUSIVE'] or minimum<=0 else 'ECD_ROUNDING_CERTIFIED')
    require(r['status']==expected,'incorrect final status')
    require(r['counters']['n']==N and r['counters']['inverse_transforms']==1 and
            r['counters']['butterflies']==245760 and r['counters']['root_multiplications']==32767,'wrong counters')
    require(r['FHE_calls']==r['production_encoding_calls']==r['sampling_calls']==r['new_ciphertext_chains']==0,'scope violation')
    require(r['original_S100_E80']=='UNCHANGED_FAIL','historical verdict changed')
    print(json.dumps({'intake':'PASS','certificate_status':expected,'counts':counts,
                     'note':'Scalar consistency only; needs source/runner adoption'},ensure_ascii=False))
    return 0

if __name__=='__main__':
    try:raise SystemExit(main())
    except Exception as e:
        print(f'INTAKE_FAILED: {type(e).__name__}: {e}',file=sys.stderr)
        raise SystemExit(2)
