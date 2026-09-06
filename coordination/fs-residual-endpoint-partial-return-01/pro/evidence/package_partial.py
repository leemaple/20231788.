from __future__ import annotations
from pathlib import Path, PurePosixPath
import ast, datetime, difflib, hashlib, json, os, platform, re, shutil, stat, subprocess, sys, zipfile, zlib

BUILD=Path('/mnt/data/endpoint_partial_build')
OUT=Path('/mnt/data/fs-residual-endpoint-partial-return-01')
ARCHIVE=Path('/mnt/data/fs-residual-endpoint-green-2fe655d4.zip')
assert not OUT.exists(), 'refuse overwriting a previous return'
OUT.mkdir()
(OUT/'evidence').mkdir();(OUT/'patches').mkdir();(OUT/'files/tests').mkdir(parents=True)
sha=lambda b:hashlib.sha256(b).hexdigest()
raw=ARCHIVE.read_bytes()
assert len(raw)==2145068 and sha(raw)=='7c70ed57b3a54eba16115d6dd600310d967f5077f669fb82c0875f76b63024c1'
verification={'archive':ARCHIVE.name,'archive_bytes':len(raw),'archive_sha256':sha(raw),'expected_identity':'PASS'}
with zipfile.ZipFile(ARCHIVE) as z:
    infos=z.infolist();assert len(infos)==175 and z.testzip() is None
    names=[i.filename for i in infos];fold=set()
    for i in infos:
        s=i.filename;p=PurePosixPath(s)
        assert not p.is_absolute() and all(t not in ('','.','..') for t in s.split('/'))
        assert not any(c in s for c in ('\\',':','\0')) and not i.is_dir() and not i.flag_bits&1
        assert stat.S_ISREG(i.external_attr>>16) and s.casefold() not in fold
        fold.add(s.casefold())
    manifests=[n for n in names if n=='MANIFEST.json' or n.endswith('/MANIFEST.json')]
    assert len(manifests)==1
    manifest_name=manifests[0]; prefix=manifest_name[:-len('MANIFEST.json')]
    manifest_bytes=z.read(manifest_name); manifest=json.loads(manifest_bytes)
    payload_names=set(names)-{manifest_name}
    # Detect the exact manifest payload list without presuming its wrapper keys.
    candidates=[]
    def lists(node):
        if isinstance(node,list):
            if len(node)==174 and all(isinstance(x,dict) for x in node): candidates.append(node)
            for v in node:lists(v)
        elif isinstance(node,dict):
            for v in node.values():lists(v)
    lists(manifest)
    accepted=[]
    for records in candidates:
        for pathkey in ('path','relative_path','name'):
            if all(pathkey in r and isinstance(r[pathkey],str) for r in records):
                for add_prefix in ('',prefix):
                    if {add_prefix+r[pathkey] for r in records}==payload_names:
                        accepted.append((records,pathkey,add_prefix))
    assert accepted, 'cannot locate an exact manifest payload closure'
    records,pathkey,add_prefix=accepted[0]
    project=[]; origin_details=[]
    def string_leaves(obj,path=''):
        if isinstance(obj,str): yield path,obj
        elif isinstance(obj,dict):
            for k,v in obj.items():yield from string_leaves(v,path+'.'+k)
        elif isinstance(obj,list):
            for k,v in enumerate(obj):yield from string_leaves(v,path+f'[{k}]')
    for entry in records:
        name=add_prefix+entry[pathkey]; data=z.read(name)
        digest_fields=[(k,v) for k,v in entry.items() if isinstance(v,str) and re.fullmatch('[0-9a-f]{64}',v)]
        assert any(v==sha(data) and ('sha' in k.lower() or 'hash' in k.lower()) for k,v in digest_fields), name
        size_fields=[(k,v) for k,v in entry.items() if type(v)is int and ('byte' in k.lower() or 'size' in k.lower())]
        assert any(v==len(data) for k,v in size_fields), name
        rel=name[len(prefix):] if name.startswith(prefix) else name
        if rel.startswith('project/'):
            repo_path=rel[len('project/'):]
            blob=hashlib.sha1(b'blob '+str(len(data)).encode()+b'\0'+data).hexdigest()
            origins=[{'field':k,'value':v} for k,v in string_leaves(entry) if v==blob]
            # Retain the exact recorded field names. No guessed role attribution.
            project.append({'path':repo_path,'bytes':len(data),'sha256':sha(data),'git_blob_sha1':blob,
                            'matching_manifest_git_blob_fields':origins,'result':'BYTE_IDENTICAL'})
            origin_details.append(len(origins))
    assert len(project)==39
    verification.update({'regular_members':175,'manifest_payloads':174,'crc':'PASS','safe_unique_regular_paths':'PASS',
       'all_payload_sizes_and_sha256':'PASS','manifest_self_exclusion':'PASS',
       'manifest_bytes':len(manifest_bytes),'manifest_sha256':sha(manifest_bytes),
       'project_file_count':39,'all_project_git_blobs_recorded':all(n>=1 for n in origin_details),
       'all_project_git_blobs_recorded_twice':all(n>=2 for n in origin_details),
       'project_files':sorted(project,key=lambda e:e['path'])})
    reference_map=[]
    relevant={
      'TASK.md':['Assignment and authority','Required implementation','Test-first delivery','Return artifacts'],
      'context/endpoint-red-return/coordination/fs-residual-endpoint-red-return-01/pro/ENDPOINT_SPEC.md':
        ['2^12','2^270','2^158','119','gzip','status','replay','UNRESOLVED'],
      'context/endpoint-red-return/coordination/fs-residual-endpoint-red-return-01/pro/TEST_PLAN.md':
        ['canonical','gzip','nonzero','RED'],
      'evidence/current-endpoint-red-run/coordination/fs-residual-endpoint-red-run-01/ACCEPTANCE.md':['33991083281','123','seven','RED'],
      'evidence/historical-original-e80/ACCEPTANCE.md':['33978202814','E80','FAIL'],
      'project/tests/paper_endpoint_observer_contract.h':['Observe','ExactAbsoluteDifference','RunSelfTest'],
      'project/tests/paper_full_eight_square_contract_test.cpp':['RunPaper','numericFailures','paper_owner_cleanup'],
      'project/tests/paper_full_eight_square_oracle.h':['Horner','SparseDecrypt','Scales'],
      'requirements/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md':['1000','1,000','correctness']}
    for path,terms in relevant.items():
        actual=prefix+path
        if actual not in names:continue
        data=z.read(actual); lines=data.decode('utf-8').splitlines()
        hits=[]
        for term in terms:
            for i,line in enumerate(lines,1):
                if term.lower() in line.lower():hits.append({'term':term,'line':i,'text':line})
        reference_map.append({'packet_path':path,'bytes':len(data),'sha256':sha(data),'locators':hits})
    # Every complete baseline file remains unchanged; no baseline is bundled.
    original_paths={p['path'] for p in project}

for filename in ('endpoint_evidence_primitives.py','test_endpoint_evidence_primitives.py'):
    assert 'tests/'+filename not in original_paths
    data=(BUILD/filename).read_bytes()
    ast.parse(data.decode('utf-8'),filename=filename)
    (OUT/'files/tests'/filename).write_bytes(data)

# Test-first execution in disposable namespaces outside the deliverable.
red_dir=BUILD/'red_namespace';green_dir=BUILD/'green_namespace'
assert not red_dir.exists() and not green_dir.exists()
red_dir.mkdir();green_dir.mkdir()
test_name='test_endpoint_evidence_primitives.py'
shutil.copy2(BUILD/test_name,red_dir/test_name)
for filename in (test_name,'endpoint_evidence_primitives.py'):shutil.copy2(BUILD/filename,green_dir/filename)
command=[sys.executable,'-B','-m','unittest','-v','test_endpoint_evidence_primitives']
env=os.environ.copy();env['PYTHONPATH']='';env['PYTHONDONTWRITEBYTECODE']='1'
runs=[]
for label,cwd in [('python-red',red_dir),('python-green',green_dir)]:
    started=datetime.datetime.now(datetime.timezone.utc).isoformat()
    try:
        run=subprocess.run(command,cwd=cwd,env=env,capture_output=True,timeout=40,check=False)
        exitcode=run.returncode;stdout=run.stdout;stderr=run.stderr;timedout=False
    except subprocess.TimeoutExpired as e:
        exitcode=None;stdout=e.stdout or b'';stderr=e.stderr or b'';timedout=True
    (OUT/'evidence'/f'{label}.stdout').write_bytes(stdout)
    (OUT/'evidence'/f'{label}.stderr').write_bytes(stderr)
    (OUT/'evidence'/f'{label}.exit').write_text(('TIMEOUT' if exitcode is None else str(exitcode))+'\n')
    count=re.search(rb'Ran (\d+) tests? in ',stderr)
    item={'name':label,'command':command,'working_directory':str(cwd),'started_utc':started,
          'exit_code':exitcode,'timeout':timedout,'reported_unittest_cases':int(count.group(1)) if count else None,
          'stdout_bytes':len(stdout),'stdout_sha256':sha(stdout),'stderr_bytes':len(stderr),'stderr_sha256':sha(stderr),
          'test_file_sha256':sha((cwd/test_name).read_bytes()),'implementation_present':label=='python-green',
          'scope':'stdlib scalar/file/gzip/JSON primitives only; not numerical observer RED/GREEN'}
    if label=='python-red':item['expected_missing_module_observed']=exitcode!=0 and b"No module named 'endpoint_evidence_primitives'" in stderr
    else:item['unit_result']='PASS' if exitcode==0 else ('TIMEOUT' if timedout else 'FAIL')
    runs.append(item)

# Independent source/patch checks; no C++/crypto/build invocation.
identities=[];patches=[]
for order,filename,role in [(1,test_name,'test-first'),(2,'endpoint_evidence_primitives.py','implementation')]:
    repo='tests/'+filename;data=(OUT/'files'/repo).read_bytes();text=data.decode('utf-8')
    diff=f'diff --git a/{repo} b/{repo}\nnew file mode 100644\n'+''.join(difflib.unified_diff([],text.splitlines(keepends=True),fromfile='/dev/null',tofile='b/'+repo))
    patch=f'{order:02d}-{role}.patch';(OUT/'patches'/patch).write_text(diff,encoding='utf-8',newline='\n')
    # Reconstruct the only added-file hunk independently and compare exact bytes.
    diff_lines=diff.splitlines(keepends=True);start=next(i for i,l in enumerate(diff_lines) if l.startswith('@@'))+1
    rebuilt=''.join(l[1:] for l in diff_lines[start:] if l.startswith('+')).encode('utf-8')
    assert rebuilt==data
    identities.append({'path':repo,'base_exists':False,'base_bytes':None,'base_sha256':None,
                       'result_bytes':len(data),'result_sha256':sha(data),'patch':patch,'patch_order':order})
    patches.append({'order':order,'path':'patches/'+patch,'role':role,'bytes':len(diff.encode()),'sha256':sha(diff.encode())})

writejson=lambda p,obj:(OUT/p).write_text(json.dumps(obj,sort_keys=True,indent=2,ensure_ascii=True)+'\n',encoding='ascii')
writejson('evidence/INPUT_VERIFICATION.json',verification)
writejson('evidence/SOURCE_REFERENCE_MAP.json',reference_map)
writejson('PATH_IDENTITIES.json',{'baseline_tested_source':'2fe655d493dcde5f05aa1515f41ca6823bba30bd',
 'documentation_head':'29e12670150f083be396686f0f4b92136758956f','changed_or_new':identities,
 'unchanged_baseline_paths':sorted(project,key=lambda e:e['path']),'patches':patches})
writejson('evidence/PYTHON_RUNS.json',{'environment':{'python':sys.version,'executable':sys.executable,
 'platform':platform.platform(),'zlib_compile_version':zlib.ZLIB_VERSION,'zlib_runtime_version':zlib.ZLIB_RUNTIME_VERSION,
 'pythonpath_override':'empty','bytecode_writes':False},'runs':runs})
missing=[
 'All seven C++ endpoint helper implementations remain absent; the accepted missing-link baseline is not repaired.',
 'No C++ full-slot DFT/direct768 observer, exact represented-binary extraction, or all-slot control execution is delivered.',
 'No actual Horner R(Int) conversion applicability checks, endpoint radius checks, or 24 comparison receipts are integrated.',
 'No full E0/E8/I8/A8 signed maxima/argmax/tuple capture and no owner-cleanup-safe RunPaper handoff/publication are delivered.',
 'No complete ordered sidecar schema reader/writer, all-nine-scale primary CTest parser, or Decimal-256 signed-tuple replay is delivered.',
 'No filesystem ownership/atomic no-overwrite publication, complete status semantics, finalizer, exact upload selection, or failure-preserving single-CTest shell/workflow wiring is delivered.',
 'No C++ compilation, observer self-test, hosted synthetic evidence tests, Linux chain, Windows chain, or independent review was performed.'
]
writejson('RETURN_STATUS.json',{'task':'FS-RESIDUAL-ENDPOINT-01','authoring_disposition':'PARTIAL_NOT_GREEN',
 'complete_implementation_draft':False,'ready_for_hosted_observation':False,'ready_to_satisfy_linkage':False,
 'input_identity_verified':True,'original_E80':'FAIL_RETAINED_FROM_SUPPLIED_HISTORICAL_EVIDENCE',
 'new_observer_disposition':'NOT_IMPLEMENTED_NOT_RUN','primitive_test_run':runs[1]['unit_result'],
 'missing_boundaries':missing,'source_prerequisite_blocker_established':False,
 'reason':'The full authoring/integration task was not completed. No defect in the adopted specification is asserted.'})

test_count=sum(1 for node in ast.walk(ast.parse((BUILD/test_name).read_text())) if isinstance(node,(ast.FunctionDef,ast.AsyncFunctionDef)) and node.name.startswith('test_'))
notes='''# FS-RESIDUAL-ENDPOINT-01 — partial return, not GREEN

## Decision

**PARTIAL_NOT_GREEN. Do not push this as the diagnostic GREEN or schedule the next chain from it.**
The complete requested implementation has not been delivered. This is a bounded,
usable standard-library primitive slice plus test-first evidence. It does not
satisfy the existing seven C++ declarations, and does not remove the hosted link
failure. No missing source/API prerequisite or contradiction in v1-r1 was
established. The blocking boundary is unfinished implementation/integration,
not a claimed specification defect, service dependency, or numerical result.

The original full-chain E80 failure remains unchanged and separate. The new
observer has no live numerical disposition. A primitive unit-test PASS, when
recorded in the ledger, is not endpoint observer GREEN or paper acceptance.

## Actual delivered interfaces

`files/tests/endpoint_evidence_primitives.py` contains no project imports and no
command-line finalizer. `files/tests/test_endpoint_evidence_primitives.py` is a
standalone `unittest` suite. They add two repository-relative paths only. No
production, public include, existing oracle, old test body/inventory, CMake or
workflow path is changed. All 39 input engineering paths remain byte-identical;
`PATH_IDENTITIES.json` gives every identity and the two ordered patches.

| Adopted requirement fragment | Actual function | Exact delivered boundary |
| --- | --- | --- |
| Exact C/S power-of-two ceiling | `scaled_one_norm_exponent` | Exact integer bit-length/cross-comparison, zero sentinel, negative exponents, positive reduced scale. It does not calculate polynomial C. |
| Classification precedence | `classify` | Exact Fraction comparisons; invalid, raw disagreement, unsupported/excess budget, two-path contradiction, then threshold overlap. It does not ascertain model support. |
| Conditional endpoint formulas | `endpoint_allowances` | D/H/P/Q/R and E0/E8/I8/A8/identity formulas. No library proof or live applicability checks. |
| Disk replay formula | `replay_allowances` | Exact Lipschitz inequality and specified replay budgets including supplied transport quanta. No Decimal replay is performed. |
| Canonical decimal text | `canonical_decimal`, `is_canonical_decimal`, `parse_canonical_decimal` | Exact integer half-even rounding from supplied Fraction, strict independent scanner/parser, quantum, unique zero and exponent range. No C++ represented-binary extraction or C++ independent validator. |
| Bounded scalar records | `parse_uint`, `integer_text`, `reduced_nonnegative` | Bounded canonical integers without altering Python's global integer-string limit; malformed unreduced receipts rejected. |
| Byte envelope | `validate_ascii_lines` | ASCII/LF/no BOM/CR/NUL, 16 MiB and 32768-byte lines. **Not the ordered metadata/check/header/16384-row schema.** |
| Deterministic gzip member | `gzip_bytes`, `validate_gzip` | Fixed header, raw level-9 deflate, one member, CRC/ISIZE, decompression cap, external byte/hash verification. **Not filesystem publication or schema attestation.** |
| Strict status envelope | `strict_status_json` | Duplicate/float/nonfinite/boolean rejection, exact caller-provided keys, sorted compact ASCII LF, no recognized self-size/hash fields. **Not complete status-schema semantics or CTest/E80 consistency.** |

## Mathematical and transport qualifications

All computations in this slice are exact Python integer/Fraction arithmetic.
There are no FFT, NTT, crypto, codec, polynomial decrypt, roots, pi, sin/cos or
complex-power computations. Test code never treats the arithmetic allowance
formulas as proof of the conditional Boost premises. The module accepts exact
Fractions at its formatting boundary; it does not assert those inputs came from
binary512 or binary768. The loss-of-low-bit test concerns Python Fraction only,
not the missing C++ extraction implementation.

The formatter uses an integer approximation only to initialize the decimal
exponent search. Exact rational comparisons determine the final exponent, and
integer quotient/remainder and parity decide rounding. No nonzero input is
silently replaced by zero. Parsing returns the printed value and its individual
rounding quantum. Full tuple replay and complex-transport quantum aggregation
are intentionally not claimed.

`gzip_bytes` deliberately can encode empty bytes; gzip validity alone is not
complete canonical evidence. The test suite explicitly demonstrates that empty
gzip decodes but is rejected by the canonical envelope validator. Similarly,
JSON envelope validity is not status semantic validity. These lower-level APIs
must never be used as upload authorization without the missing outer validators.
Canonical equivalence is not a claim of identical compressed bytes across zlib
versions. The actual interpreter and zlib versions are in the execution ledger.

Integer records are capped at 10000 digits, canonical exponent grammar at five
digits, and formatter input components at 400000 bits. These are explicit bounds
of this partial module. Their integration with complete runtime range checks
has not been adjudicated. No unsupported underflow or decimal conversion premise
has been silently adopted.

## Missing implementation boundary

'''+''.join(f'{i}. {item}\n' for i,item in enumerate(missing,1))+'''
## Integration and patch use

Apply `patches/01-test-first.patch`, then `patches/02-implementation.patch` relative
to the exact `project/` root, not the archive root. Alternatively copy both
complete files under `files/`; do not both copy and apply the same patches.
The test-first stage fails because the new Python primitive module is absent.
It is not a numerical RED and is unrelated to the accepted hosted C++ API/link
RED. The implementation stage only makes this primitive suite runnable.

This return contains no speculative C++ interface declarations, stubs,
placeholder DFT, broad framework, historical source copy, dependencies,
credentials, raw polynomials, keys, ciphertexts or live canonical evidence.
The earlier incomplete evidence-reader work is not misrepresented as a complete
reader here; only the independently testable primitive boundary is packaged.

## No-extra-trial rule

No CI push, dispatch, repeat, build or chain is authorized by this partial
package. The prescribed future full-task order and all resource limits remain
unchanged, but require a complete independently reviewed implementation first.
No compile/integrity/unsupported/timeout result should trigger an automatic
second chain to obtain a sidecar. The historical E80 failure is not reclassified.
'''
(OUT/'IMPLEMENTATION_NOTES.md').write_text(notes,encoding='utf-8')
ledger=f'''# Test ledger — actual versus NOT RUN

## Overall disposition

**PARTIAL_NOT_GREEN.** Primitive arithmetic/format/compression/JSON unit tests do
not establish observer correctness, live evidence integrity, or E80 acceptance.

## Input verification (executed)

Input archive: `{ARCHIVE.name}`; {len(raw)} bytes;
SHA-256 `{sha(raw)}`. CRC, safe unique regular member paths, all 174 payload sizes
and SHA-256 hashes, manifest closure/self-exclusion and the 39-file engineering
inventory were checked with the Python standard library. Exact per-path Git-blob
matches and recorded manifest field names are in `evidence/INPUT_VERIFICATION.json`.
No remote repository was queried.

## Actual test-first primitive runs

Environment: `{platform.platform()}`.
Python executable: `{sys.executable}`.
Python: `{sys.version.splitlines()[0]}`.
zlib compile/runtime: `{zlib.ZLIB_VERSION}` / `{zlib.ZLIB_RUNTIME_VERSION}`.

Both runs used:

```text
{sys.executable} -B -m unittest -v test_endpoint_evidence_primitives
```

`PYTHONPATH` was empty and `PYTHONDONTWRITEBYTECODE=1`. The baseline namespace
contained only the test file; the next namespace contained the byte-identical
test file and the implementation. Each subprocess had a 40-second execution cap.
They are disposable local scalar/file test namespaces, not chain evidence.

| Run | Actual exit | Actual unittest count | Meaning |
| --- | ---: | ---: | --- |
| Python primitive RED | {runs[0]['exit_code']} | {runs[0]['reported_unittest_cases']} | Missing new module observed: {runs[0].get('expected_missing_module_observed')} |
| Python primitive implementation | {runs[1]['exit_code']} | {runs[1]['reported_unittest_cases']} | {runs[1]['unit_result']} for this primitive slice only |

The test source defines {test_count} test methods, with deterministic subcases.
Do not count subcases or repeated invocations as additional unique test bodies.
Exact commands, working directories, UTC start times, code identities, exit codes,
stdout/stderr byte counts and hashes are in `evidence/PYTHON_RUNS.json`. The raw
stdout/stderr and `.exit` records are retained beside it. A missing-module import
failure is an expected API-absence baseline, not a numerical failing assertion.
No older transient run is substituted for these retained executions.

## Executed source checks

Both Python files were parsed with `ast.parse`. Both ordered new-file patches
were independently reconstructed from their added hunk lines and matched against
the complete delivered file bytes. No original engineering file was altered.
The output archive itself was reopened and every returned payload/manifest entry
was checked during packaging. No C++ compile or source execution was used.

## NOT RUN / NOT IMPLEMENTED

All seven C++ helpers, the four sparse all-slot controls at both precisions,
endpoint FFTs, direct768 comparison, C++ canonical validator and malformed-input
tests, actual Horner conversion checks, 24-check receipts, all-slot signed
metrics, scalar Decimal replay, schema-specific filesystem/wrapper tests, CTest
parsing, owned directory/atomic publication and workflow integration are not
implemented in this return and were not run. No OpenFHE, codec, FFT, NTT,
encryption, decrypt, build configuration, C++ compilation, library install,
benchmark, hosted test, CI dispatch, push or independent agent call occurred.

## Future commands (not executed; blocked on complete integration)

The currently retained build/link RED is not repaired by this package. Do not
interpret the following as ready-to-run instructions for this partial patch:

```text
cmake --build <existing-build-directory> --target paper_full_eight_square_contract_test --parallel 2
<existing-build-directory>/paper_full_eight_square_contract_test --endpoint-observer-self-test
ctest --test-dir <existing-build-directory> --show-only=json-v1
ctest --test-dir <existing-build-directory> --verbose --output-on-failure -R '^paper_full_eight_square_contract$'
```

The complete-task workflow must retain old60/API gates before paper build,
execute the self-test once, then required synthetic evidence checks, then the
existing no-argument paper CTest exactly once per host with the existing timeout,
serial setting and OMP_NUM_THREADS=2. The missing wrapper must capture the actual
CTest shell status, finalize once even on nonzero and preserve that nonzero.
One Linux and one Windows chain are the maximum next authorized observation
following completed integration/review, not an execution claim or pass promise.
There must be no automatic repeat after unsupported, compile, integrity or
timeout failure. New-source identity must not be mislabeled as 2fe or 9f.

## Scientific status separation

Historical supplied E80: **FAIL retained**. This work does not rerun, repair,
weaken or replace it. New endpoint observer: **NOT IMPLEMENTED / NOT RUN**.
Primitive test outcome: **{runs[1]['unit_result']}**, only within the boundaries above.
No full-slot inherited-error attribution, interval root certificate, production
correctness conclusion, or adoption of A is made.
'''
(OUT/'TEST_LEDGER.md').write_text(ledger,encoding='utf-8')
review='''# Changelog and review checklist

## Changes

Two new test-local Python files only: exact arithmetic/serialization/compression
primitives and their standalone deterministic tests. Test-first and implementation
patches are nonoverlapping. All 39 existing engineering paths remain unchanged.
No C++ signature, production API, oracle arithmetic, precision gate, metadata,
input formula, scale rule, control inventory, CTest entry or workflow is changed.

## Independent review checklist

- Confirm `PARTIAL_NOT_GREEN` and that the missing seven helper symbols remain
  missing; reject any attempt to present this as a drop-in diagnostic GREEN.
- Verify input/output manifest closure, exact baseline identities and patch order.
- Inspect exact K and classification precedence separately from conditional-model
  applicability, which is not implemented.
- Inspect integer half-even rounding, carry, unique zero, grammar, quanta and range
  limits. The C++ validator and represented-value extraction remain missing.
- Inspect gzip member/header/CRC/ISIZE/decompression checks and external identities;
  do not confuse compression validity with endpoint schema or upload eligibility.
- Inspect strict JSON envelope scope; complete status values/relationships, source
  ownership, original CTest parsing and failure propagation remain missing.
- Read actual RED/implementation stdout, stderr and exit evidence. No hosted or
  numerical observer result may be inferred from these primitive tests.

## Source-reference map

`evidence/SOURCE_REFERENCE_MAP.json` records exact packet paths, sizes, SHA-256 and
line/text matches for the adopted v1-r1 spec/test plan, current declarations,
original oracle and paper-test seams, correctness scope and retained acceptance
records. `evidence/INPUT_VERIFICATION.json` closes all source payload identities.
Historical statements retain their historical authority only. No external
OpenFHE/Boost version or sparse-reference closure claim is introduced.
'''
(OUT/'CHANGELOG_REVIEW_CHECKLIST.md').write_text(review,encoding='utf-8')
(OUT/'README.md').write_text('''# Partial diagnostic implementation return

**Disposition: PARTIAL_NOT_GREEN. Not ready for integration as GREEN, linkage,
or hosted observation.**

Start with `IMPLEMENTATION_NOTES.md` for the exact delivered/missing boundary,
`TEST_LEDGER.md` for retained execution results, and `RETURN_STATUS.json` for the
machine-readable disposition. Complete source files are under `files/`; the
ordered, nonoverlapping test-first and implementation patches are under `patches/`.

This package preserves the original E80 FAIL and makes no new observer or chain
acceptance claim. All 39 baseline engineering files remain byte-identical.
''',encoding='utf-8')
# Include the authoring/verification recipe as evidence, not a project dependency.
(OUT/'evidence'/'package_partial.py').write_bytes(Path(__file__).read_bytes())
# Self-excluding returned manifest and safe flat ZIP (no nested archives).
payloads=[]
for path in sorted(OUT.rglob('*')):
    if path.is_file():
        data=path.read_bytes();payloads.append({'path':path.relative_to(OUT).as_posix(),'bytes':len(data),'sha256':sha(data)})
writejson('MANIFEST.json',{'schema':'bounded-diagnostic-partial-return-manifest-v1','self_excluded':'MANIFEST.json','files':payloads})
zip_path=Path('/mnt/data/fs-residual-endpoint-partial-return-01.zip')
assert not zip_path.exists()
with zipfile.ZipFile(zip_path,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as outzip:
    for path in sorted(OUT.rglob('*')):
        if path.is_file():
            info=zipfile.ZipInfo(path.relative_to(OUT).as_posix(),(2026,9,6,0,0,0));info.create_system=3
            info.external_attr=(stat.S_IFREG|0o644)<<16;info.compress_type=zipfile.ZIP_DEFLATED
            outzip.writestr(info,path.read_bytes(),compress_type=zipfile.ZIP_DEFLATED,compresslevel=9)
with zipfile.ZipFile(zip_path) as check:
    assert check.testzip() is None
    names=check.namelist();assert len(names)==len(set(names))==len(payloads)+1
    assert set(names)=={e['path'] for e in payloads}|{'MANIFEST.json'}
    for e in payloads:
        data=check.read(e['path']);assert len(data)==e['bytes'] and sha(data)==e['sha256']
outer=zip_path.read_bytes();digest=sha(outer)
Path(str(zip_path)+'.sha256').write_text(f'{digest}  {zip_path.name}\n',encoding='ascii')
Path('/mnt/data/fs-residual-endpoint-partial-return-01-outer-identity.json').write_text(json.dumps({'path':zip_path.name,'bytes':len(outer),'sha256':digest,'regular_members':len(payloads)+1,'payloads':len(payloads),'disposition':'PARTIAL_NOT_GREEN'},sort_keys=True,indent=2)+'\n')
print(json.dumps({'ZIP':str(zip_path),'bytes':len(outer),'sha256':digest,'test_runs':runs,'disposition':'PARTIAL_NOT_GREEN'},indent=2))
