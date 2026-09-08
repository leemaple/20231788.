#!/usr/bin/env python3
"""Replay candidate unified diffs IN MEMORY and inspect source-only boundaries.
No compiler, linker, project test, transform, git checkout, or input edit.
"""
from pathlib import Path
import argparse
import ast
import hashlib
import json
import re


def replay(patch: str, state: dict[str,str]) -> None:
    lines=patch.splitlines(True);i=0
    while i<len(lines):
        if not lines[i].startswith('--- '):raise AssertionError('expected file header')
        old=lines[i][4:].strip();new=lines[i+1][4:].strip();i+=2
        assert new.startswith('b/')
        name=new[2:]
        assert not name.startswith('/') and '..' not in Path(name).parts
        before='' if old=='/dev/null' else state[name]
        original=before.splitlines(True);position=0;out=[]
        while i<len(lines) and not lines[i].startswith('--- '):
            match=re.fullmatch(r'@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@\n',lines[i])
            if not match:raise AssertionError('unsupported hunk header: '+lines[i])
            start=int(match[1]);old_count=int(match[2]) if match[2] is not None else 1
            new_count=int(match[4]) if match[4] is not None else 1
            index=start-1 if start else 0
            assert index>=position
            out.extend(original[position:index]);position=index;i+=1;consumed=produced=0
            while i<len(lines) and not lines[i].startswith(('@@ ','--- ')):
                line=lines[i];i+=1;prefix=line[0];body=line[1:]
                if prefix in (' ','-'):
                    assert position<len(original) and original[position]==body
                    position+=1;consumed+=1
                if prefix in (' ','+'):out.append(body);produced+=1
                assert prefix in (' ','+','-')
            assert consumed==old_count and produced==new_count
        out.extend(original[position:]);state[name]=''.join(out)


def main():
    a=argparse.ArgumentParser();a.add_argument('--input-dir',required=True,type=Path)
    a.add_argument('--out',type=Path,default=Path(__file__).resolve().parents[1]/'checks/results/candidate_static_checks.json')
    args=a.parse_args()
    root=Path(__file__).resolve().parents[1];c=root/'candidate'
    names=['src/high_precision_client_io.cpp','CMakeLists.txt']
    state={p:(args.input_dir/'project'/p).read_text() for p in names}
    before=dict(state)
    replay((c/'01-red.patch').read_text(),state)
    assert state['src/high_precision_client_io.cpp']==before['src/high_precision_client_io.cpp']
    assert 'EncodingInspection InspectFixedS100PublicEncoding' in state['include/openfhe_2023_1788/public_s100_encoding_probe.h']
    assert 'InspectFixedS100PublicEncoding' not in state['src/high_precision_client_io.cpp']
    replay((c/'02-green.patch').read_text(),state)
    for p,text in state.items():assert text==(c/'full_files'/p).read_text(),p
    original=before['src/high_precision_client_io.cpp'];green=state['src/high_precision_client_io.cpp']
    def encoding(s):return s[s.index('EncodingWork ComputeEncoding('):s.index('\n}  // namespace',s.index('EncodingWork ComputeEncoding('))]
    assert encoding(original)==encoding(green)
    body=green[green.index('namespace diagnostic {'):green.index('}  // namespace diagnostic')]
    assert body.count('ComputeEncoding(binding,values,spec,primary,check)')==1
    for name in ['BindContext','CreatePaperRepeatedMult2Setup','CreateFixedQH128ClientKeyPair','GetPRNG','Encrypt','Decrypt','SetFormat','PreCompute','RootOfUnity']:
        assert not re.search(r'\b'+name+r'\s*\(',body)
    assert body.index('values.size()!=16384')<body.index('TransformTable<Primary>')
    driver=state['diagnostics/public_s100_encoding_dump.cpp']
    assert 'paper_full_test::ClientInputs(paper_full_test::Inputs())' in driver
    for name in ['CreatePaperRepeatedMult2Setup','GetPRNG','Encrypt','Decrypt','ReadSecret','Horner','AnchorRoots','SetFormat']:
        assert not re.search(r'\b'+name+r'\s*\(',driver)
    assert 'EXCLUDE_FROM_ALL' in state['CMakeLists.txt'][len(before['CMakeLists.txt']):]
    assert 'add_test' not in state['CMakeLists.txt'][len(before['CMakeLists.txt']):]
    for p in [root/'checks/run_checks.py',c/'certify_public_encoder.py',c/'tests/test_transform_models.py']:
        ast.parse(p.read_text())
    binding=json.loads((c/'BASE_BINDING.json').read_text())
    for item in binding['files']:
        assert hashlib.sha256((c/'full_files'/item['path']).read_bytes()).hexdigest()==item['candidate_sha256']
        if item['base_sha256']:
            assert hashlib.sha256((args.input_dir/'project'/item['path']).read_bytes()).hexdigest()==item['base_sha256']
    result={'status':'PASS','patches_replayed_in_memory':2,'full_files_equal':len(state),
            'ComputeEncoding_body_unchanged':True,'default_CI_or_CTest_changed':False,
            'input_files_written':0,'CXX_compiled_or_executed':False,'FFT_NTT_executed':False,
            'RED_link_failure_observed':False,'GREEN_runtime_observed':False,
            'scope':'source-text/AST/patch consistency, NOT compiler or runtime verification'}
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps(result))
if __name__=='__main__':main()
