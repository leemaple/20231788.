# Repeated Mult2 candidate: original return integrity receipt

## Status and scope

2026-09-04, Asia/Shanghai. This is an original-return archival and integrity
record, not adoption, a code review disposition, or a successful second Mult2.
The returned candidate is a bounded shape/basis-routing probe plus a fixture
for the current rejection boundary. Its own ACTUAL_EXECUTION_LEDGER.md marks
semantic second Mult2 NOT IMPLEMENTED; C++ configure/build, Stage-1/Stage-2
runtime and Linux/Windows CI NOT RUN. No C++ build, crypto, CTest or CI was
performed by this archival task. Existing first-Mult2 evidence is not evidence
of second-operation success.

Returned source identity: frozen
`774fe2dcfca47d7a08cab9c04b29c430e354cf9f`; tested predecessor
`47907783a6141d0174da79eae264d779fc598f28`; official OpenFHE pin
`df495ba2e91739a6dc8f1de254fc5a41155ce504`.
The return's SOURCE_IDENTITY.json describes its original input packet separately
from this returned 63,963-byte ZIP; those archive identities must not be confused.

Local archival worktree was `codex/repeated-mult2-01` at
`8dcadb4544ed567c6de3a7d3825857f89470b29e`. Existing untracked
PAPER_H128_DISTRIBUTION_REQUIREMENT.md, REPEATED_MULT2_PROBE_SPEC_REVIEW.md and
REPEATED_MULT2_PROBE_STANDARDS_REVIEW.md were not edited in this task.
No Git mutation, browser/external-agent operation, source/test edit or patch
replay occurred. Candidate acceptance remains owned by main and pending.

## Original files preserved byte-for-byte

| New archived file | Bytes | SHA256 |
| --- | ---: | --- |
| `repeated-mult2-bounded-basis-routing-probe-774fe2d.zip` | 63963 | `bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2` |
| `repeated-mult2-bounded-basis-routing-probe-774fe2d.zip.sha256` | 121 | `8e08b5f7a00f135f0aae238e0466cac9995197b0cec40c72c52a66ac88670619` |
| `returned-decoded-gitleaks.json` | 3 | `37517e5f3dc66819f61f5a7bb8ace1921282415f10551d2defa5c3eb0985b570` |

ZIP and sidecar origins:
`/Users/lifeng/Downloads/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip`
and the same path plus `.sha256`.
Original root scanner report:
`/private/tmp/repeated-mult2-return-review.QGVKRy.scan.json`.

All three destination paths were absent. Copy used exclusive-create mode and
verified complete byte equality and SHA256 afterward; a different existing
target would have caused failure, not overwrite. No expanded source copy was
created. Existing ORIGINAL_VISIBLE_DELIVERY.md and input-recheck-gitleaks.json
were preserved, with unchanged SHA256 respectively:
`4e3d8144e490d6822be91d3c79489636860378f23e390a719b5f0432efc6746e`,
`37517e5f3dc66819f61f5a7bb8ace1921282415f10551d2defa5c3eb0985b570`.

## Independently observed integrity checks

- Downloaded ZIP: exact 63,963 bytes / supplied SHA256; sidecar: exact 121 bytes /
  supplied SHA256 and exact ZIP basename plus hash.
- Exactly 24 unique, case-fold-unique regular file members below one expected
  root prefix; safe relative paths; no symlinks, encrypted entries, directory
  entries, parent traversal, absolute paths or Windows path aliases. CRC PASS.
- Exactly 22 payload manifest entries; only MANIFEST.json and MANIFEST.sha256
  self-excluded. Every listed path, size and SHA256 matches; no extra/missing
  payload file. The manifest sidecar hashes MANIFEST.json correctly.
- `MANIFEST.json`: 3798 bytes; SHA256 `cdeeab6eb0890218386c64cf50838b79a68b220144aed3f62d999264bca7de4b`.
- `MANIFEST.sha256`: 80 bytes; SHA256 `00dc6c020be3176b7ae6b2f79720bedcf9f69dc7f66d82047bec20a1c315a623`.
- All 24 decoded members, totaling 231,371 bytes, are byte-identical to the
  existing decoded tree at
  `/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d`.
  Its paths are exactly the same set and were checked for symlinks before reads.
- SOURCE_IDENTITY.json matches the frozen source, tested source and official pin
  above. This checks the declared identity, not semantic correctness of patches.
- No old implementation or browser/session payload was added by this archival
  task. Secret scans below are evidence, not an absolute no-secret guarantee.

## Scanner attribution and fresh small-scope recheck

**Root-executed earlier scan:** root supplied the observation “gitleaks 8.30.1,
decoded 231371 bytes, 0 findings.” Its original report is the unchanged
returned-decoded-gitleaks.json (`[]\n`). The exact earlier command line was
not supplied to this task, so none is invented or attributed to this author.

**This author's independent rerun:** gitleaks 8.30.1, default maintained
rules, config environment overrides unset, no custom baseline; decoded-tree
target contains no ignore/config file; inline allow comments ignored.
Exact invocation:

```text
env -u GITLEAKS_CONFIG -u GITLEAKS_CONFIG_TOML GOMAXPROCS=2 /opt/homebrew/bin/gitleaks dir /private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d --no-banner --no-color --redact=100 --ignore-gitleaks-allow --gitleaks-ignore-path /private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d --report-format json --report-path - --timeout 60
```

Exit 0; report stdout:
```json
[]
```
Actual stderr:
```text
11:20PM INF scanned ~231371 bytes (231.37 KB) in 55.1ms
11:20PM INF no leaks found
```

This scan was limited to the verified 24-member decoded return, not the
repository, home directory, browser state or an OpenFHE installation.

## Actual read-only verification command

The following bounded command was executed successfully. It only reads the
download, manifest and pre-existing decoded payload and computes hashes; no
returned script or patch is executed.

```sh
/usr/bin/python3 - <<'PY'
from pathlib import Path,PurePosixPath
import hashlib,json,stat,zipfile,re
src=Path('/Users/lifeng/Downloads/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip')
side=Path(str(src)+'.sha256')
root=Path('/private/tmp/repeated-mult2-return-review.QGVKRy/repeated-mult2-bounded-basis-routing-probe-774fe2d')
prefix='repeated-mult2-bounded-basis-routing-probe-774fe2d/'
def sha(b): return hashlib.sha256(b).hexdigest()
b=src.read_bytes(); s=side.read_bytes()
assert len(b)==63963 and sha(b)=='bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2'
assert len(s)==121 and sha(s)=='8e08b5f7a00f135f0aae238e0466cac9995197b0cec40c72c52a66ac88670619'
assert s==(sha(b)+'  '+src.name+'\n').encode()
assert root.is_dir() and not root.is_symlink()
with zipfile.ZipFile(src) as z:
 entries=z.infolist(); names=[e.filename for e in entries]
 assert len(entries)==24==len(set(names))==len({n.casefold() for n in names})
 decoded={}
 for e in entries:
  p=PurePosixPath(e.filename)
  assert str(p)==e.filename and not p.is_absolute() and '\\' not in e.filename and '\0' not in e.filename
  assert all(x not in ('','.','..') and ':' not in x and not x.endswith((' ','.')) for x in p.parts)
  assert not any(re.fullmatch(r'(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\..*)?',x,re.I) for x in p.parts)
  assert stat.S_ISREG(e.external_attr>>16) and not e.is_dir() and not(e.flag_bits&1)
  assert e.filename.startswith(prefix)
  name=e.filename[len(prefix):]
  value=z.read(e.filename)
  assert len(value)==e.file_size
  loc=root
  for part in PurePosixPath(name).parts:
   loc=loc/part
   assert not loc.is_symlink()
  assert loc.is_file() and value==loc.read_bytes()
  decoded[name]=value
 assert z.testzip() is None
 m=json.loads(decoded['MANIFEST.json'])
 assert m['entry_count']==22 and m['self_excluded']==['MANIFEST.json','MANIFEST.sha256']
 byname={e['path']:e for e in m['entries']}
 assert len(byname)==len(m['entries'])==22 and set(byname)==set(decoded)-set(m['self_excluded'])
 for n,e in byname.items():
  assert len(decoded[n])==e['size'] and sha(decoded[n])==e['sha256'],n
 assert decoded['MANIFEST.sha256']==(sha(decoded['MANIFEST.json'])+'  MANIFEST.json\n').encode()
 ident=json.loads(decoded['SOURCE_IDENTITY.json'])
 assert ident['source_commit']=='774fe2dcfca47d7a08cab9c04b29c430e354cf9f'
 assert ident['tested_source_commit']=='47907783a6141d0174da79eae264d779fc598f28'
 assert ident['openfhe_pin']=='df495ba2e91739a6dc8f1de254fc5a41155ce504'
 actual=[]
 for p in root.rglob('*'):
  assert not p.is_symlink()
  if p.is_file(): actual.append(p.relative_to(root).as_posix())
 assert set(actual)==set(decoded) and len(actual)==24
 records=[{'path':n,'bytes':len(v),'sha256':sha(v)} for n,v in sorted(decoded.items())]
 print(json.dumps({'archive':{'bytes':len(b),'sha256':sha(b)},'sidecar':{'bytes':len(s),'sha256':sha(s),'exactArchiveNameAndHash':True},'memberCount':24,'manifestPayloadCount':22,'manifestSelfExclusions':m['self_excluded'],'uncompressedBytes':sum(len(v) for v in decoded.values()),'pathUniqueRegularNonEncryptedCRC':'PASS','allPayloadSizeSHA256':'PASS','manifestSHA256Sidecar':'PASS','existingDecodedExactByteAndPathEquality':'PASS','sourceIdentity':ident,'members':records},ensure_ascii=False))
PY
```

## Actual exclusive copy command

Only the three new paths listed above are written by this command; this receipt
itself was subsequently created with apply_patch.

```sh
/opt/homebrew/bin/node <<'JS'
const fs=require('fs'),path=require('path'),crypto=require('crypto');
const dest='/Users/lifeng/Documents/20231788-openfhe-codex-repeated-mult2-01/coordination/returns/repeated-mult2-pro-774fe2d';
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
if(!fs.lstatSync(dest).isDirectory()||fs.lstatSync(dest).isSymbolicLink())throw Error('Unsafe destination');
const originals=[['ORIGINAL_VISIBLE_DELIVERY.md','4e3d8144e490d6822be91d3c79489636860378f23e390a719b5f0432efc6746e'],['input-recheck-gitleaks.json','37517e5f3dc66819f61f5a7bb8ace1921282415f10551d2defa5c3eb0985b570']];
for(const[n,h]of originals)if(hash(fs.readFileSync(path.join(dest,n)))!==h)throw Error('Existing evidence changed');
const inputs=[['/Users/lifeng/Downloads/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip','repeated-mult2-bounded-basis-routing-probe-774fe2d.zip',63963,'bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2'],['/Users/lifeng/Downloads/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip.sha256','repeated-mult2-bounded-basis-routing-probe-774fe2d.zip.sha256',121,'8e08b5f7a00f135f0aae238e0466cac9995197b0cec40c72c52a66ac88670619'],['/private/tmp/repeated-mult2-return-review.QGVKRy.scan.json','returned-decoded-gitleaks.json',3,'37517e5f3dc66819f61f5a7bb8ace1921282415f10551d2defa5c3eb0985b570']];
const records=[];
for(const [source,name,bytes,expected]of inputs){
 const original=fs.readFileSync(source);if(original.length!==bytes||hash(original)!==expected)throw Error('Source mismatch');
 const target=path.join(dest,name);let action;
 if(fs.existsSync(target)){if(fs.lstatSync(target).isSymbolicLink()||!fs.lstatSync(target).isFile()||!fs.readFileSync(target).equals(original))throw Error('Refuse different existing target');action='left-identical-existing-file';}
 else{fs.copyFileSync(source,target,fs.constants.COPYFILE_EXCL);action='created-exclusive-byte-copy';}
 const copied=fs.readFileSync(target);if(!copied.equals(original)||hash(copied)!==expected)throw Error('Copy differs');
 records.push({path:target,bytes:copied.length,sha256:hash(copied),action});
}
for(const[n,h]of originals)if(hash(fs.readFileSync(path.join(dest,n)))!==h)throw Error('Existing evidence changed');
process.stdout.write(JSON.stringify({records,existingEvidencePreserved:true,expandedSourceCopiesCreated:0}));
JS
```

Other read-only checks actually executed included `unzip -l` on the exact
downloaded ZIP, `gitleaks version`, `git status --short`,
`git log -1 --format='%H %D'`, and SHA256 checks on the two existing evidence
files. No tests/build/CI or candidate integration is implied by integrity PASS.
