#!/usr/bin/env python3
"""Review-authored driver; arithmetic/readers below are SUPPLIED, not independently authored."""
import argparse, dataclasses, hashlib, json, pathlib, re, sys, tempfile, time
from fractions import Fraction

def serial(x):
    if dataclasses.is_dataclass(x): return {f.name:serial(getattr(x,f.name)) for f in dataclasses.fields(x)}
    if isinstance(x,Fraction):return str(x)
    if isinstance(x,dict):return {k:serial(v) for k,v in x.items()}
    if isinstance(x,(list,tuple)):return [serial(v) for v in x]
    return x

def main():
    p=argparse.ArgumentParser();p.add_argument('input_root',type=pathlib.Path);p.add_argument('host',choices=['linux','windows']);a=p.parse_args()
    root=a.input_root.resolve();sys.path.insert(0,str(root/'project/tests'))
    from paper_endpoint_gzip import verify_gzip
    from paper_endpoint_sidecar_reader import read_sidecar
    from paper_endpoint_sidecar_replay import replay_sidecar
    from paper_endpoint_primary_reader import PrimaryIdentity,parse_primary_log
    from paper_endpoint_reconcile import reconcile_replay
    from paper_endpoint_status import decode_status
    base=root/'project/coordination/fs-endpoint-live-run-01';parent=base/a.host
    identity=dict(expected_source_commit='ed5fd192a89d6d4728ad295e87cf06a3f4abc832',expected_host=a.host,expected_run_id='34039088536',expected_run_attempt='1')
    statusfile=next(parent.glob('*.status.json'));status=decode_status(statusfile.read_bytes(),**identity)
    gz=(parent/status['gzip_filename']).read_bytes()
    canonical=verify_gzip(gz,canonical_size=status['canonical_bytes'],canonical_sha256=status['canonical_sha256'],gzip_size=status['gzip_bytes'],gzip_sha256=status['gzip_sha256']).canonical
    log=(base/(a.host.upper()+'_JOB.log')).read_bytes()
    lines=[re.sub(rb'^(?:\xef\xbb\xbf)?2026-09-06T[0-9:.]+Z ',b'',x) for x in log.splitlines(keepends=True)]
    primary=b''.join(x for x in lines if x.startswith(b'61: '))
    parsed=parse_primary_log(primary,PrimaryIdentity(status['source_commit'],a.host,status['github_run_id'],status['github_run_attempt']),ctest_exit_code=8,expected_scope='live-single-chain')
    with tempfile.TemporaryDirectory(prefix='fs-review-full-replay-') as tmp:
        q=pathlib.Path(tmp)/(status['gzip_filename'][:-3]);q.write_bytes(canonical)
        sidecar=read_sidecar(q,expected_scope='live-single-chain',**identity)
    started=time.perf_counter();result=replay_sidecar(sidecar);reconcile_replay(parsed,sidecar,result);seconds=time.perf_counter()-started
    print(json.dumps(dict(result='SUPPLIED_FULL_REPLAY_AND_RECONCILE_PASS',host=a.host,source_commit=status['source_commit'],canonical_sha256=hashlib.sha256(canonical).hexdigest(),rows=result.row_count,scalar_replay_seconds=seconds,crypto_or_fft_executed=False,E80_disposition=status['E80_disposition'],numeric_gate_failures=status['numeric_gate_failures'],A_disposition=status['A_disposition'],replay=serial(result)),indent=2))
if __name__=='__main__':main()
