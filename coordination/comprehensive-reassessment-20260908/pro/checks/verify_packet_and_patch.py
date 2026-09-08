#!/usr/bin/env python3
"""Read-only identity checks and apply-check on a private copy; never builds/runs project."""
from __future__ import annotations
import argparse, ast, hashlib, json, os, platform, shutil, stat, subprocess, sys, tempfile, zipfile
from pathlib import Path, PurePosixPath
from decimal import Decimal, localcontext
from fractions import Fraction

def sha(b: bytes) -> str: return hashlib.sha256(b).hexdigest()
def require(ok: bool,label: str) -> None:
    if not ok: raise RuntimeError(label)
def main() -> None:
    ap=argparse.ArgumentParser();ap.add_argument('input_zip',type=Path);ap.add_argument('input_root',type=Path);ap.add_argument('output_root',type=Path);a=ap.parse_args()
    raw=a.input_zip.read_bytes();require(len(raw)==9045349 and sha(raw)=='08760af7def640afa06caf939c9ff7a5805d4cc6c567b49f24024cbf2f7c9158','input ZIP identity')
    with zipfile.ZipFile(a.input_zip) as z:
        info=z.infolist();names=[i.filename for i in info];require(len(names)==231 and len(set(names))==231,'member count')
        for i in info:
            p=PurePosixPath(i.filename);mode=i.external_attr>>16
            require(not i.is_dir() and not p.is_absolute() and '..' not in p.parts and not stat.S_ISLNK(mode),'unsafe member')
        require(z.testzip() is None,'CRC')
        m=json.loads(z.read('MANIFEST.json'));require(len(m['files'])==230,'payload count');require(set(names)=={'MANIFEST.json'}|{r['path'] for r in m['files']},'manifest/member set')
        blob_count=0; transformed=[]
        for row in m['files']:
            b=z.read(row['path']);require(len(b)==row['bytes'] and sha(b)==row['sha256'],'payload hash '+row['path'])
            require((a.input_root/row['path']).read_bytes()==b,'extraction bytes')
            origin=row.get('origin',{})
            if origin.get('kind')=='cleanroom_git_blob' and origin.get('transform'):
                transformed.append({'path':row['path'],'transform':origin['transform'],'source_git_blob':origin['git_blob'],'source_sha256':origin['source_sha256']})
            elif origin.get('kind')=='cleanroom_git_blob':
                h=hashlib.sha1(b'blob '+str(len(b)).encode()+b'\0'+b).hexdigest()
                require(h==origin['git_blob'],'Git blob mismatch');blob_count+=1
        mr=sha(z.read('MANIFEST.json'))
    base=a.input_root/'project';new=a.output_root/'full_files';patch=a.output_root/'CONTRACT.patch'
    require((new/'CMakeLists.txt').read_bytes().startswith((base/'CMakeLists.txt').read_bytes()),'CMake existing prefix changed')
    with tempfile.TemporaryDirectory(prefix='reassessment-apply-') as temp:
        target=Path(temp)/'project';shutil.copytree(base,target)
        commands=[]
        for argv in [['git','apply','--check',str(patch)],['git','apply',str(patch)]]:
            r=subprocess.run(argv,cwd=target,text=True,capture_output=True)
            commands.append({'argv':argv,'exit_code':r.returncode,'stdout':r.stdout,'stderr':r.stderr});require(r.returncode==0,'patch application')
        require((target/'CMakeLists.txt').read_bytes()==(new/'CMakeLists.txt').read_bytes(),'applied CMake bytes')
        cp='tests/s100_annulus125_eight_square_test.cpp'
        require((target/cp).read_bytes()==(new/cp).read_bytes(),'applied new test bytes')
        for p in base.rglob('*'):
            if p.is_file() and p.name!='CMakeLists.txt':require(p.read_bytes()==(target/p.relative_to(base)).read_bytes(),'unintended existing-file edit')
    checked=[]
    for p in sorted((a.output_root/'checks').glob('*.py')):
        ast.parse(p.read_text(),filename=str(p));checked.append(p.name)
    installations=[]
    for root in ['/usr/local/lib','/usr/lib','/opt','/usr/local/include','/usr/include']:
        r=Path(root)
        if r.exists():
            # Bounded shallow probe only; do not claim a whole-filesystem inventory.
            for pattern in ['*OpenFHE*','*openfhe*','*/OpenFHEConfig.cmake','*/openfhe.h']:
                installations.extend(str(p) for p in r.glob(pattern))
    commands_version={}
    for binary,args in [('git',['--version']),('cmake',['--version']),('c++',['--version'])]:
        executable=shutil.which(binary)
        if executable:
            r=subprocess.run([executable]+args,text=True,capture_output=True);commands_version[binary]={'path':executable,'exit_code':r.returncode,'first_line':r.stdout.splitlines()[0] if r.stdout else ''}
    Q=[1125899904679937,1125899903827969,1152921504598720513,1152921504597016577,1152921504595968001,1152921504595640321,1152921504593412097,1152921504592822273,1152921504592429057,1152921504589938689,1099510054913]
    scale=Fraction(2**100)
    for k in range(1,9):scale=scale**2/(Q[-1]*Q[10-k])
    with localcontext() as ctx:
        ctx.prec=100; ratio=Decimal(scale.numerator)/Decimal(scale.denominator)/Decimal(2**100)
    result={'status':'PASS_STATIC_ONLY','input_zip_bytes':len(raw),'input_zip_sha256':sha(raw),'members':231,'payloads':230,
        'payload_bytes':sum(r['bytes'] for r in m['files']),'manifest_bytes':(a.input_root/'MANIFEST.json').stat().st_size,
        'input_manifest_sha256':mr,'direct_git_blob_rows_verified':blob_count,'transformed_rows_not_raw_git_blobs':transformed,'source_commit':m['source_commit'],'task_commit':m['task_commit'],
        'paper_sha256':sha((a.input_root/'references/paper/PAPER-2023-1788.pdf').read_bytes()),
        'official_reference_count':len(list((a.input_root/'references/official-full').rglob('*.*'))),
        'boost_reference_count':len(list((a.input_root/'references/boost-1.83.0').rglob('*.*'))),
        'patch_commands':commands,'python_ast_parsed':checked,'builds':0,'project_tests_executed':0,'new_encrypted_samples':0,'remote_writes':0,
        'environment':{'python':sys.version,'platform':platform.platform(),'tools':commands_version,
            'shallow_OpenFHE_installation_matches':sorted(set(installations)),
            'qualification':'No full pinned OpenFHE checkout/build was established. Probe is not a global absence proof.'},
        'S100_exact_S8_over_2pow100':str(ratio)}
    print(json.dumps(result,indent=2,ensure_ascii=False))
if __name__=='__main__':main()
