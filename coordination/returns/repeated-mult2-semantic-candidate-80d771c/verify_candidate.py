#!/usr/bin/env python3
"""Offline source/patch/manifest verifier. Does NOT compile or run cryptography.

Usage: python verify_candidate.py --package . --input-archive INPUT.zip
Optional: --input-sidecar INPUT.zip.sha256
Requires Python 3.9+ and git; no external Python package, network or credentials.
"""
from __future__ import annotations
import argparse
import csv
import hashlib
import io
import json
from pathlib import Path, PurePosixPath
import re
import stat
import subprocess
import tempfile
import zipfile
from fractions import Fraction

BASE = '80d771c52df10bce1c60992b5e0edb4e64f145ca'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
ARCHIVE_SHA = '764baddb20d81c1168745ac31eb043d0d94cf1ba6b406d0194f9245a994196a2'
TASK_SHA = '40839c3450028f91fd8dc6bb3509e9dc848ec4168d82f081e94d7d4997fafe48'
VECTOR_SHA = '6a0dae07b55adf8552272407d4e8885b1e993808d9914fc250b508ecc8d772e6'
RED = '0001-red-repeated-mult2-semantic-two-square.patch'
GREEN = '0002-green-repeated-mult2-semantic-two-square.patch'
NAME = 'repeated_mult2_semantic_two_square_contract'

def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def parse_tsv(data: bytes) -> list[dict[str, str]]:
    return list(csv.DictReader(io.StringIO(data.decode('utf-8')), delimiter='\t'))

def files(root: Path) -> dict[str, bytes]:
    return {p.relative_to(root).as_posix(): p.read_bytes() for p in root.rglob('*') if p.is_file()}

def zip_payload(path: Path) -> dict[str, bytes]:
    with zipfile.ZipFile(path) as archive:
        names: set[str] = set()
        roots: set[str] = set()
        data: dict[str, bytes] = {}
        for entry in archive.infolist():
            name = entry.filename
            parts = PurePosixPath(name).parts
            require(name not in names and '\\' not in name and '\0' not in name and
                    parts and not name.startswith('/') and ':' not in parts[0] and
                    all(p not in ('..', '.') for p in parts), 'unsafe or duplicate ZIP member: '+name)
            names.add(name); roots.add(parts[0])
            mode = entry.external_attr >> 16
            require(not stat.S_ISLNK(mode), 'ZIP symlink: '+name)
            require(not entry.flag_bits & 1, 'encrypted ZIP member: '+name)
            if not entry.is_dir():
                require(stat.S_IFMT(mode) in (0, stat.S_IFREG), 'nonregular ZIP member: '+name)
                data['/'.join(parts[1:])] = archive.read(entry)
        require(len(roots) == 1, 'ZIP must have one root')
        require(archive.testzip() is None, 'ZIP CRC failure')
        return data

def verify_input(path: Path, sidecar: Path | None) -> tuple[dict[str, bytes], dict]:
    raw = path.read_bytes()
    require(len(raw) == 1299850 and sha(raw) == ARCHIVE_SHA, 'input archive size/SHA mismatch')
    if sidecar:
        fields = sidecar.read_text().strip().split()
        require(len(fields) == 2 and fields[0] == ARCHIVE_SHA and
                fields[1].lstrip('*') == 'repeated-mult2-semantic-implementation-01-80d771c.zip',
                'input sidecar mismatch')
    data = zip_payload(path)
    manifest_hash, manifest_name = data['MANIFEST.sha256'].decode().strip().split()
    require(manifest_name == 'MANIFEST.tsv' and manifest_hash == sha(data[manifest_name]), 'input manifest auth')
    rows = parse_tsv(data[manifest_name])
    require(len(rows) == 127 and len({r['path'] for r in rows}) == 127, 'input manifest row closure')
    require(set(data) == {r['path'] for r in rows} | {'MANIFEST.tsv', 'MANIFEST.sha256'}, 'input exact closure')
    for row in rows:
        payload = data[row['path']]
        require(len(payload) == int(row['bytes']) and sha(payload) == row['sha256'], 'input payload mismatch '+row['path'])
    require(len(data['TASK.md']) == 21814 and sha(data['TASK.md']) == TASK_SHA, 'mandatory TASK identity')
    provenance = json.loads(data['SOURCE_PROVENANCE.json'])
    require(provenance['implementationBase']['commit'] == BASE and provenance['openfhe']['commit'] == PIN,
            'mandatory implementation/source-pin identities')
    project = {k[8:]: v for k, v in data.items() if k.startswith('project/')}
    require(len(project) == 40, 'implementation baseline file count')
    for row in rows:
        if row['path'].startswith('project/'):
            require(row['origin'] == 'git:'+BASE+':'+row['path'][8:], 'baseline origin mismatch')
    # Authenticate each declared origin against the fixed packet provenance.
    origin_counts = {'git':0, 'archive':0, 'user-attachment':0}
    evidence = {e['packetPath']:e for e in provenance['frozenExternalEvidence']}
    for row in rows:
        name,origin = row['path'],row['origin']
        if name.startswith('project/'):
            expected = 'git:'+BASE+':'+name[8:]
        elif name == 'TASK.md':
            expected = 'git:'+provenance['taskOverlay']['repositoryCommit']+':'+provenance['taskOverlay']['repositoryPath']
        elif name in ('PACKET_README.md','SOURCE_PROVENANCE.json'):
            expected = 'git:498d40014521a0f85dc970c2caf9e6267b37a909:coordination/handoffs/repeated-mult2-semantic-pro-packet-01/'+name
        elif name in evidence:
            entry=evidence[name]
            expected='git:'+entry['originCommit']+':coordination/'+name.rsplit('/',1)[-1]
            require(int(row['bytes'])==entry['bytes'] and row['sha256']==entry['sha256'],'frozen evidence identity '+name)
        elif name.startswith('evidence/repeated/prior-pro-return/'):
            expected='archive:'+provenance['priorRepeatedProReturn']['sourceArchiveSha256']+':'+name[len('evidence/repeated/prior-pro-return/'):]
        elif name.startswith('official/extracted/'):
            expected='archive:'+provenance['openfhe']['sourceArchiveSha256']+':'+name[len('official/extracted/'):]
        elif name.startswith('paper/'):
            expected='user-attachment:'+name[len('paper/'):]
        else:
            raise RuntimeError('unrecognized input origin path '+name)
        require(origin==expected,'input origin mismatch '+name)
        origin_counts[origin.split(':',1)[0]]+=1
    official = json.loads(data['official/extracted/OFFICIAL-SOURCE-PROVENANCE.json'])
    require(len(official) == 53, 'official source count')
    for entry in official:
        content = data['official/extracted/official-full/'+entry['path']]
        blob = hashlib.sha1(b'blob '+str(len(content)).encode()+b'\0'+content).hexdigest()
        require(len(content) == entry['bytes'] and sha(content) == entry['sha256'] and blob == entry['gitBlob'] and
                '/'+PIN+'/' in entry['url'], 'pinned official payload mismatch '+entry['path'])
    prior = {k[len('evidence/repeated/prior-pro-return/'):]: v for k, v in data.items()
             if k.startswith('evidence/repeated/prior-pro-return/')}
    mh, mn = prior['MANIFEST.sha256'].decode().strip().split()
    require(mn == 'MANIFEST.json' and sha(prior[mn]) == mh, 'prior inner manifest authentication')
    prior_manifest = json.loads(prior[mn])
    require(set(prior) == {e['path'] for e in prior_manifest['entries']} | {'MANIFEST.json','MANIFEST.sha256'},
            'prior expanded return exact closure')
    for entry in prior_manifest['entries']:
        require(len(prior[entry['path']]) == entry['size'] and sha(prior[entry['path']]) == entry['sha256'],
                'prior return payload mismatch '+entry['path'])
    return data, {'archive_size': len(raw), 'archive_sha256': sha(raw), 'manifest_payloads': len(rows),
                  'baseline_files':len(project), 'official_source_payloads':len(official),
                  'expanded_prior_files':len(prior), 'origins':origin_counts, 'status':'PASS',
                  'provenance_limit':'No external Git fetch/show was performed. Original standalone official/prior ZIPs are not embedded; their compressed archive hashes are producer provenance, not newly rehashed here.'}

def pair(raw: dict) -> tuple[Fraction, Fraction]:
    def dyad(v: dict) -> Fraction:
        n, k = int(v['n']), v['k']
        require(str(n) == v['n'] and k >= 0 and (k == 0 or n % 2 != 0), 'noncanonical dyadic')
        return Fraction(n, 1 << k)
    return dyad(raw['real']), dyad(raw['imag'])

def multiply(a, b):
    return a[0]*b[0]-a[1]*b[1], a[0]*b[1]+a[1]*b[0]

def verify_vectors(raw: bytes, header: str) -> dict:
    require(len(raw) == 13593 and sha(raw) == VECTOR_SHA, 'frozen vectors byte identity')
    data = json.loads(raw)
    canonical = data.copy(); recorded = canonical.pop('canonical_payload_sha256_without_this_field')
    require(sha(json.dumps(canonical,sort_keys=True,separators=(',',':'),ensure_ascii=False).encode()) == recorded,
            'frozen canonical payload hash')
    vectors = data['vectors']
    for name in ('X','Y','Z','W'):
        require(len(vectors[name]) == 16 and [x['slot'] for x in vectors[name]] == list(range(16)), 'vector slots')
        block = re.search(r'\bk'+name+r'\s*\{\{(.*?)\}\};',header,re.S)
        require(block is not None, 'literal header block '+name)
        literal = re.findall(r'\{\s*"(-?\d+)"\s*,\s*(\d+)U\s*\}',block.group(1))
        expected = [(row[part]['n'],str(row[part]['k'])) for row in vectors[name] for part in ('real','imag')]
        require(literal == expected, 'literal header transcription '+name)
    for i in range(16):
        z = multiply(pair(vectors['X'][i]),pair(vectors['Y'][i]))
        require(z == pair(vectors['Z'][i]) and multiply(z,z) == pair(vectors['W'][i]), 'exact semantic literal '+str(i))
    for name in ('Z','W'):
        first, second = pair(vectors[name][0]),pair(vectors[name][1])
        delta = pair(data['distinguishing_deltas'][name+'0_minus_'+name+'1'])
        require(delta == (first[0]-second[0],first[1]-second[1]), 'exact delta '+name)
        require(delta[0]**2+delta[1]**2 > Fraction(16,1<<160), 'delta distinction strength')
        block = re.search(r'\bk'+name+r'Delta(.*?);',header,re.S)
        require(block is not None,'delta header missing')
        literal = re.findall(r'\{\s*"(-?\d+)"\s*,\s*(\d+)U\s*\}',block.group(1))
        d = data['distinguishing_deltas'][name+'0_minus_'+name+'1']
        require(literal == [(d[p]['n'],str(d[p]['k'])) for p in ('real','imag')], 'delta header transcription')
    return {'status':'PASS','Z_exact_items':16,'W_exact_items':16,'complex_deltas':2,'frozen_json_sha256':sha(raw),
            'header_transcription':'PASS','cryptographic_precision':'NOT RUN'}

def bindings(data: bytes) -> list[tuple[str,str]]:
    return [(name,' '.join(command.split())) for name,command in
            re.findall(r'add_test\(NAME\s+(\S+)\s+COMMAND\s+([^\)]+)\)',data.decode())]

def clean_cpp(text: str) -> str:
    # For these bounded source files; not a general C++ parser or a compiler.
    pattern = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/',re.S)
    return pattern.sub(lambda m: ' '*(len(m.group(0))),text)

def function(text: str, marker: str) -> str:
    clean = clean_cpp(text); start = clean.index(marker); begin = clean.index('{',start); depth = 0
    for end in range(begin,len(clean)):
        if clean[end] == '{': depth += 1
        elif clean[end] == '}':
            depth -= 1
            if depth == 0: return text[begin+1:end]
    raise RuntimeError('unclosed function '+marker)

def verify_replay(package: Path, original: dict[str,bytes]) -> dict:
    baseline = {k[8:]:v for k,v in original.items() if k.startswith('project/')}
    p1,p2 = package/'patches'/RED,package/'patches'/GREEN
    events = []
    with tempfile.TemporaryDirectory(prefix='mult2-offline-replay-') as temporary:
        root = Path(temporary)
        for name,raw in baseline.items():
            dest=root/name;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(raw)
        def run(patch: Path, apply: bool=False, rejected: bool=False) -> None:
            args=['git','apply']+([] if apply else ['--check'])+[str(patch.resolve())]
            result=subprocess.run(args,cwd=root,text=True,capture_output=True,check=False)
            require((result.returncode!=0) if rejected else (result.returncode==0), 'patch result '+patch.name+': '+result.stderr)
            events.append({'patch':patch.name,'mode':'apply' if apply else 'check','returncode':result.returncode,
                           'expected_rejection':rejected,'stderr':result.stderr})
        run(p2,rejected=True);require(files(root)==baseline,'negative replay changed baseline')
        run(p1);run(p1,apply=True);red=files(root)
        frozen=json.loads((package/'verification/RED_FREEZE.json').read_text())
        for name,h in frozen['files'].items(): require(sha(red[name])==h,'RED freeze mismatch '+name)
        run(p2);run(p2,apply=True);green=files(root)
        changed={name for name in set(baseline)|set(green) if baseline.get(name)!=green.get(name)}
        complete=files(package/'complete/project')
        require(changed==set(complete),'complete files do not exactly cover changed tree')
        for name,raw in complete.items(): require(green[name]==raw,'replay/complete byte mismatch '+name)
        for name,raw in red.items():
            if name.startswith('tests/'):require(green[name]==raw,'GREEN modified RED or legacy test '+name)
        for name,raw in baseline.items():
            if name.startswith('tests/'):require(green[name]==raw,'legacy assertion bytes changed')
        b,r,g=(bindings(x['CMakeLists.txt']) for x in (baseline,red,green))
        require(len(b)==57 and len(g)==58 and b==g[:57] and r==g and g[-1]==(NAME,'repeated_mult2_semantic_two_square_test'),
                '57+1 exact CTest binding/order contract')
        expected=parse_tsv((package/'EXPECTED_CTEST_BINDINGS.tsv').read_bytes())
        require([(e['name'],e['command']) for e in expected]==g,'expected binding ledger mismatch')
        workflow=green['.github/workflows/dcp-rcb.yml'].decode()
        require(workflow.count('      - codex/repeated-mult2-semantic-01\n')==1,'push branch')
        require(workflow.count("-R '^"+NAME+"$'")==2,'both focused CI jobs')
        removed=re.sub(r'      - name: Run focused repeated-Mult2 two-operation semantic contract\n.*?(?=      - name:)',
                       '',workflow,flags=re.S).replace('      - codex/repeated-mult2-semantic-01\n','')
        require(removed==baseline['.github/workflows/dcp-rcb.yml'].decode(),'old workflow changed beyond branch/focus insertion')
        cmake=green['CMakeLists.txt'].decode();msvc,nonmsvc=cmake.split('if(MSVC)\n',1)[1].split('else()\n',1)
        require('repeated_mult2_semantic_two_square_test PRIVATE /W4 /WX' in msvc and
                'repeated_mult2_semantic_two_square_test PRIVATE -Wall -Wextra -Wpedantic -Werror' in nonmsvc,
                'host warning-as-error flags')
        cpp=green['src/repeated_mult2.cpp'].decode()
        evaluator=clean_cpp(function(green['tests/repeated_mult2_semantic_two_square_test.cpp'].decode(),'Evaluation Evaluate('))
        require(not re.search(r'\b(?:PrivateKey|GetPrivateElement|DecryptPair|DecryptPolynomial|Encrypt|KeyGen|PrepareClient)\b',evaluator),
                'secret/client use in evaluator scope')
        factory=function(cpp,'RepeatedMult2ClientSetup CreateRepeatedMult2DiagnosticSetup(')
        outside=clean_cpp(cpp.replace(factory,''))
        require(not re.search(r'\b(?:PrivateKeyImpl|GetPrivateElement|KeyGen|EvalMultKeyGen)\b',outside),
                'private-key operation outside client setup')
        reentry=clean_cpp(function(cpp,'CiphertextPair DoubleCKKS::Reenter('))
        require(not re.search(r'\b(?:DCP|Encrypt|Decrypt|KeyGen|Relinearize|Rescale|ModReduce)\s*\(',reentry),
                'forbidden refresh/arithmetic call in re-entry')
        require('DEBUG_KEY' in green['include/openfhe_2023_1788/repeated_mult2.h'].decode(),'debug secret-storage guard')
        # Literal preservation of the coefficient kernels, not a compiler proof.
        oldcpp=baseline['src/double_ckks.cpp'].decode();newcpp=green['src/double_ckks.cpp'].decode()
        anchors={'Tensor2':'    lbcrypto::ConstCiphertext<lbcrypto::DCRTPoly> leftHigh',
                 'Relin2':'    ValidateTensorResult(tensor);','RS2':'    ValidatePair(relinearized);',
                 'RCB':'    auto result = pair.high_->Clone();'}
        for method,anchor in anchors.items():
            old=function(oldcpp,'DoubleCKKS::'+method+'(');new=function(newcpp,'DoubleCKKS::'+method+'(')
            old=old[old.index(anchor):];new=new[new.index(anchor):]
            new=re.sub(r'    if \(plan_\) \{\n        AttachReceipt\(result, [^\n]+\);\n    \}\n','',new)
            require(old==new,'coefficient kernel changed '+method)
        return {'status':'PASS','baseline_files':len(baseline),'green_files':len(green),'complete_files':len(complete),
                'bindings':58,'preserved_bindings':57,'red_tests_unchanged':True,'kernel_literal_preservation':True,
                'structural_scope_checks':'PASS (bounded lexical checks plus separate source review, not compilation)',
                'events':events,'compile':'NOT RUN','runtime':'NOT RUN','hosted':'NOT RUN'}

def verify_source_lines(package: Path, original: dict[str,bytes]) -> dict:
    index=json.loads((package/'verification/SOURCE_LINE_INDEX.json').read_text())
    entries=index['entries'];require(len({e['id'] for e in entries})==len(entries),'duplicate source line IDs')
    for entry in entries:
        require(entry['scope'] in ('input','candidate'),'invalid source line scope')
        raw=original[entry['path']] if entry['scope']=='input' else (package/'complete/project'/entry['path']).read_bytes()
        require(sha(raw)==entry['sha256'],'source-line file hash '+entry['id'])
        lines=raw.split(b'\n');a,b=entry['start_line'],entry['end_line']
        require(1<=a<=b<=len(lines),'source line range '+entry['id'])
        require(sha(b'\n'.join(lines[a-1:b]))==entry['range_sha256_lf_without_final_lf'],'source range hash '+entry['id'])
    ledger=(package/'SOURCE_CLAIM_TEST_LEDGER.md').read_text()
    for entry in entries: require('['+entry['id']+']' in ledger,'uncited source anchor '+entry['id'])
    return {'status':'PASS','file_line_anchors':len(entries),'scope':'byte/range authentication, not a correctness proof'}

def verify_manifest(package: Path) -> dict:
    data=files(package)
    h,name=data['MANIFEST.sha256'].decode().strip().split()
    require(name=='MANIFEST.tsv' and sha(data[name])==h,'candidate manifest authentication')
    rows=parse_tsv(data[name]);payload={r['path'] for r in rows}
    require(len(payload)==len(rows) and set(data)==payload|{'MANIFEST.tsv','MANIFEST.sha256'},'candidate exact closure')
    for r in rows:
        name=r['path'];raw=data[name]
        require(raw and len(raw)==int(r['bytes']) and sha(raw)==r['sha256'],'candidate payload '+name)
        require(r['origin'] and r['source_commit'] and r['base_commit'],'candidate provenance fields '+name)
        require(not any(x in ('.git','__pycache__','build','.cache') for x in PurePosixPath(name).parts),'excluded member '+name)
    return {'status':'PASS','payload_files':len(rows),'self_exclusions':['MANIFEST.tsv','MANIFEST.sha256']}

def main() -> None:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--package',type=Path,required=True)
    parser.add_argument('--input-archive',type=Path,required=True)
    parser.add_argument('--input-sidecar',type=Path)
    args=parser.parse_args();package=args.package.resolve()
    source,input_report=verify_input(args.input_archive,args.input_sidecar)
    original=source['evidence/repeated/prior-pro-return/contracts/SECOND_MULT2_EXACT_VECTORS.json']
    require((package/'contracts/SECOND_MULT2_EXACT_VECTORS.json').read_bytes()==original,'frozen contract was changed')
    result={'input':input_report,
            'vectors':verify_vectors(original,(package/'complete/project/tests/repeated_mult2_exact_vectors.h').read_text()),
            'replay':verify_replay(package,source),
            'source_lines':verify_source_lines(package,source),
            'manifest':verify_manifest(package)}
    print(json.dumps(result,indent=2))

if __name__=='__main__':
    main()
