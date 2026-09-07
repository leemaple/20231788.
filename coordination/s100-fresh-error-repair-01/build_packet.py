"""Exact clean-room source handoff; uses the prior audited scanner/ZIP helpers."""
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = 'e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb'
BRANCH = 'codex/s100-fresh-error-repair-20260907'
PRIOR = Path('/Users/lifeng/Documents/20231788-openfhe-precision116-seam-20260907/artifacts/handoffs/precision116-pro-handoff-01/experimental-precision116-profile-seam-dbbbee0d.zip')
PRIOR_SHA = '75c29c9f1305c44fc64679827dcfc1e8b3ba475b48ab46900f71849c255f9a7b'
HELPER = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'

def main():
    import hashlib
    assert hashlib.sha256(HELPER.read_bytes()).hexdigest() == HELPER_SHA
    spec = importlib.util.spec_from_file_location('packet_helpers', HELPER)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    h.require(h.git('status', '--porcelain') == b'', 'clean worktree required')
    h.require(h.git('branch', '--show-current').decode().strip() == BRANCH, 'wrong branch')
    head = h.git('rev-parse', 'HEAD').decode().strip()
    h.git('merge-base', '--is-ancestor', SOURCE, head)
    h.require(h.git('diff', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows') == b'', 'source drift')
    payloads = {}
    def add_git(commit, path, dest):
        blob, mode, oid = h.git_entry(commit, path)
        h.add(payloads, dest, blob, {'kind':'cleanroom_git_blob','commit':commit,'path':path,'mode':mode,'git_blob':oid})
    add_git(head, 'coordination/s100-fresh-error-repair-01/TASK.md', 'TASK.md')
    paths = h.git('ls-tree', '-r', '--name-only', SOURCE, '--', 'src', 'include', 'tests').decode().splitlines()
    paths += ['CMakeLists.txt', '.github/workflows/dcp-rcb.yml']
    for p in sorted(paths):
        add_git(SOURCE,p,'project/'+p)
    context = [
        'IMPLEMENTATION_DELIVERY.md','REPRODUCE.zh-CN.md',
        'coordination/PAPER_H128_OFFICIAL_API_SUPPORT.md',
        'coordination/PAPER_PRECISION_PARAMETER_GATES.md',
        'coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md',
        'coordination/fs-endpoint-live-run-01/ACCEPTANCE.md',
        'coordination/fs-endpoint-scientific-review-return-01/pro/DECISION.md',
        'coordination/fs-endpoint-scientific-review-return-01/pro/FINDINGS.md',
        'coordination/fs-precision-profile-feasibility-01/candidate.json',
        'coordination/precision116-eight-square-return-01/ROOT_RUN_RECEIPT.json',
    ]
    for p in context:
        add_git(SOURCE,p,'project/'+p)
    inherited, rows = h.decode_manifest_archive(PRIOR,1543581,PRIOR_SHA,
        '40a4e8ee30121c85800b6eea4a541b52c11df4fd7b56cdd8da1c4da11207be91',198,'manifest_self_excluded',True)
    for prefix,count in [('references/official-full/',77),('references/paper/',2),('references/boost-1.83.0/',4)]:
        names = [n for n in rows if n.startswith(prefix)]
        h.require(len(names)==count,'reference closure drift')
        for n in names:
            h.add(payloads,n,inherited[n],{'kind':'pinned_pristine_reference','archive_sha256':PRIOR_SHA,'input_manifest_row':rows[n]})
    h.validate_names(list(payloads))
    selected_targeted = h.targeted_content_scan(payloads,'selected payloads')
    selected_scan = h.gitleaks_scan(payloads,'selected payloads')
    manifest = {'manifest_self_excluded':True,'schema':'s100-fresh-error-repair-packet-v1',
        'source_commit':SOURCE,'task_commit':head,'branch':BRANCH,'worktree_status':'clean',
        'official_pin':'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'selected_targeted_scan':selected_targeted,'selected_gitleaks_scan':selected_scan,
        'files':[{'path':n,'bytes':len(v['bytes']),'sha256':h.sha256(v['bytes']),'origin':v['origin']} for n,v in sorted(payloads.items())]}
    final = {n:v['bytes'] for n,v in payloads.items()}
    final['MANIFEST.json']=(json.dumps(manifest,indent=2,sort_keys=True)+'\n').encode()
    output=ROOT/'artifacts/handoffs/s100-fresh-error-repair-01/s100-fresh-error-repair-e6c4cc1.zip'
    output.parent.mkdir(parents=True,exist_ok=True)
    h.require(not output.parent.is_symlink(),'symlink output directory')
    with zipfile.ZipFile(output,'x',compression=zipfile.ZIP_DEFLATED,compresslevel=6) as z:
        for n,b in sorted(final.items()):
            info=zipfile.ZipInfo(n,(2026,9,7,0,0,0));info.create_system=3
            info.external_attr=(stat.S_IFREG|0o644)<<16;info.compress_type=zipfile.ZIP_DEFLATED
            z.writestr(info,b)
    decoded=h.verify_final_archive(output,final)
    member_payloads={n:{'bytes':b} for n,b in decoded.items()}
    final_targeted=h.targeted_content_scan(member_payloads,'decoded final archive')
    final_scan=h.gitleaks_scan(member_payloads,'decoded final archive')
    h.require(h.git('status','--porcelain')==b'','worktree changed during packaging')
    raw=output.read_bytes()
    receipt={'archive_path':str(output),'bytes':len(raw),'sha256':h.sha256(raw),
        'source_commit':SOURCE,'task_commit':head,'branch':BRANCH,'source_status':'clean',
        'members':len(final),'included_paths':sorted(final),'manifest_sha256':h.sha256(final['MANIFEST.json']),
        'selected_targeted':selected_targeted,'selected_gitleaks':selected_scan,
        'final_targeted':final_targeted,'final_gitleaks':final_scan,'crc_manifest_exact_bytes':'PASS'}
    with (HERE/'PACKET_RECEIPT.json').open('x') as f:
        json.dump(receipt,f,indent=2,sort_keys=True);f.write('\n')
    print(json.dumps({k:receipt[k] for k in ['archive_path','bytes','sha256','members','source_commit','task_commit','crc_manifest_exact_bytes']},indent=2))

if __name__ == '__main__':
    main()
