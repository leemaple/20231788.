import pathlib,json,sys,re,hashlib
base=pathlib.Path('/mnt/data/atlas_work/input')
log=pathlib.Path('/mnt/data/atlas_work/reads.jsonl')
p=sys.argv[1]; data=(base/p).read_bytes();lines=data.decode('utf8',errors='replace').replace('\x00','␀').splitlines()
spec=sys.argv[2] if len(sys.argv)>2 else 'all';purpose=sys.argv[3] if len(sys.argv)>3 else 'source-study'
if spec=='all': spans=[(1,len(lines))]
elif spec.startswith('find:'):
 ids=[i for i,l in enumerate(lines,1) if re.search(spec[5:],l)]
 spans=[(max(1,i-3),min(len(lines),i+7)) for i in ids]
else:
 spans=[tuple(map(int,x.split('-'))) if '-' in x else (int(x),int(x)) for x in spec.split(',')]
print(f'=== {p} [{len(lines)} lines] ===')
for a,b in spans:
 b=min(b,len(lines)); print(f'--- L{a}-L{b} ---')
 for i in range(a,b+1): print(f'{i:5} {lines[i-1]}')
 with log.open('a') as f: f.write(json.dumps(dict(path=p,start=a,end=b,purpose=purpose,mode='displayed-for-semantic-reading',sha256=hashlib.sha256(data).hexdigest()),ensure_ascii=False)+'\n')
