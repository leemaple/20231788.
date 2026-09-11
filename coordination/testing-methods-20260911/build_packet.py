"""Bounded exact-source test-design handoff; source/archive checks only."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = '33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1'
BRANCH = 'codex/testing-methods-diagnosis-20260911'
PRIOR = Path('/Users/lifeng/Documents/20231788-openfhe-parameter-atlas-20260908/artifacts/handoffs/parameter-atlas-20260908/openfhe-parameter-atlas-a4b815a.zip')

def main():
    helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    assert hashlib.sha256(helper.read_bytes()).hexdigest() == 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'
    spec = importlib.util.spec_from_file_location('audited_helpers', helper)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    head = h.git('rev-parse','HEAD').decode().strip()
    h.require(h.git('branch','--show-current').decode().strip() == BRANCH, 'branch drift')
    h.require(h.git('diff','--quiet') == b'' and h.git('diff','--cached','--quiet') == b'', 'dirty tracked source')
    changed = h.git('diff','--name-only',SOURCE,head).decode().splitlines()
    h.require(all(p.startswith('coordination/testing-methods-20260911/') for p in changed),'runtime drift')
    before_status = h.git('status','--porcelain=v1','--untracked-files=all').decode()
    prior, rows = h.decode_manifest_archive(PRIOR,2273139,
        'abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68',
        '1dd20ae1b69b342f243ab57c9c7a8cadb9ca6002d5a95cc6d7746a787bdbc0ac',
        454,'manifest_self_excluded',True)
    payloads = {}
    for name in sorted(rows):
        if name.startswith(('official/','references/paper/','references/boost-1.83.0/')) or name == 'OFFICIAL_SOURCE_RECEIPT.json':
            h.add(payloads,name,prior[name],{'kind':'verified_pristine_official_or_supplied_paper',
                  'archive_sha256':h.sha256(PRIOR.read_bytes()),'row':rows[name]})

    def tree(*prefixes):
        return h.git('ls-tree','-r','--name-only',SOURCE,'--',*prefixes).decode().splitlines()

    def add_git(commit,path,dest=None):
        name = dest or path
        if name in payloads:
            return
        if name.endswith('.log'):
            h.ALLOWED_LOGS.add(name)
        raw,mode,oid = h.git_entry(commit,path)
        h.add(payloads,name,raw,{'kind':'fixed_cleanroom_git_blob','commit':commit,'path':path,'git_blob':oid,'mode':mode})

    for path in tree('src','include','tests','diagnostics','.github/workflows') + ['CMakeLists.txt']:
        add_git(SOURCE,path,'project/'+path)
    for path in tree('docs/parameter-atlas/reference'):
        if path.endswith(('.md','.json','.tsv')) and '/checks/' not in path:
            add_git(SOURCE,path)
    for path in [
        'docs/parameter-atlas/README.zh-CN.md','docs/parameter-atlas/REVIEW_NOTES.zh-CN.md',
        'CHECK_AND_HANDOFF.zh-CN.md','REPRODUCE.zh-CN.md',
        'coordination/completion-contract-20260909/ADOPTED_RESULT.zh-CN.md',
        'coordination/completion-contract-20260909/INDEPENDENT_ADOPTION_REVIEW.md',
        'coordination/completion-contract-20260909/pro/COMPLETION_CONTRACT.zh-CN.md',
        'coordination/completion-contract-20260909/pro/FINDINGS.md',
        'coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md',
        'coordination/s100-fresh-error-repair-01/FRESH_IDEAL_PROPAGATION.md',
        'coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py',
        'coordination/s100-fresh-error-repair-01/remote-green2-diagnostic-steps.log',
        'coordination/s100-fresh-error-repair-01/GREEN2_RESULT.md',
        'coordination/s100-fresh-error-repair-01/PAPER_EXPERIMENT_PROVENANCE.md',
        'coordination/s100-fresh-error-repair-01/INDEPENDENT_SEAM_AUDIT.md',
        'coordination/s100-output-finalization-01/GREEN_EVIDENCE.txt',
        'coordination/public-s100-ecd-cell-20260909/ADOPTED_RESULT.zh-CN.md',
        'coordination/public-s100-ecd-cell-20260909/MATH_ADOPTION_REVIEW.md',
        'coordination/public-s100-encoder-cap-20260909/ADOPTED_RESULT.zh-CN.md',
        'coordination/initial-lift-nonwrap-20260909/ADOPTED_CONTRACT.md',
        'coordination/relin2-bound-20260908/ADOPTED_CONTRACT.md',
        'coordination/build-provenance-20260908/ASSESSMENT.zh-CN.md',
        'coordination/fs-endpoint-live-run-01/ACCEPTANCE.md',
        'coordination/fs-endpoint-live-run-01/LINUX_AUDIT.json',
        'coordination/fs-endpoint-live-run-01/WINDOWS_AUDIT.json',
        'coordination/precision116-eight-square-return-01/ACCEPTANCE.md',
        'coordination/annulus-independent-pro-review-20260908/pro/REVIEW.md',
        'coordination/annulus-independent-pro-review-20260908/pro/results/old_s100_230.json',
        'coordination/s100-annulus125-20260908/RESULT.zh-CN.md']:
        add_git(SOURCE,path)
    for name in ['TASK.md','SKILL_RESEARCH.md']:
        add_git(head,'coordination/testing-methods-20260911/'+name,name)
    h.validate_names(list(payloads))
    selection_targeted = h.targeted_content_scan(payloads,'testing-methods source selection')
    selection_scan = h.gitleaks_scan(payloads,'testing-methods source selection')
    manifest = {'schema':'testing-methods-diagnosis-input-v1','manifest_self_excluded':True,
        'source_commit':SOURCE,'task_commit':head,'branch':BRANCH,'initial_git_status':before_status,
        'official_pin':'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'no_local_modified_openfhe_or_quarantined_implementation':True,
        'selection_targeted':selection_targeted,'selection_gitleaks':selection_scan,
        'files':[{'path':n,'bytes':len(v['bytes']),'sha256':h.sha256(v['bytes']),'origin':v['origin']} for n,v in sorted(payloads.items())]}
    final = {n:v['bytes'] for n,v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest,indent=2,sort_keys=True)+'\n').encode()
    output = ROOT / 'artifacts/handoffs/testing-methods-20260911/testing-methods-33722b9.zip'
    output.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(output,'x',compression=zipfile.ZIP_DEFLATED,compresslevel=6) as z:
        for name,raw in sorted(final.items()):
            info = zipfile.ZipInfo(name,(2026,9,11,0,0,0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(info,raw)
    decoded = h.verify_final_archive(output,final)
    parts = {n:{'bytes':v} for n,v in decoded.items()}
    final_targeted = h.targeted_content_scan(parts,'decoded testing-methods archive')
    final_scan = h.gitleaks_scan(parts,'decoded testing-methods archive')
    h.require(h.git('rev-parse','HEAD').decode().strip()==head,'HEAD changed')
    h.require(h.git('diff','--quiet') == b'' and h.git('diff','--cached','--quiet') == b'', 'tracked source changed')
    receipt = {'archive_path':str(output),'bytes':output.stat().st_size,'sha256':h.sha256(output.read_bytes()),
        'members':len(final),'expanded_bytes':sum(map(len,final.values())),'source_commit':SOURCE,'task_commit':head,
        'manifest_sha256':h.sha256(final['MANIFEST.json']),'included_paths':sorted(final),
        'selection_targeted':selection_targeted,'selection_gitleaks':selection_scan,
        'final_targeted':final_targeted,'final_gitleaks':final_scan,'archive_validation':'PASS',
        'initial_status':before_status,'final_status':h.git('status','--porcelain=v1','--untracked-files=all').decode()}
    with (HERE/'PACKET_RECEIPT.json').open('x') as f:
        json.dump(receipt,f,indent=2); f.write('\n')
    print(json.dumps({k:receipt[k] for k in ['archive_path','bytes','sha256','members','source_commit','task_commit','archive_validation']},indent=2))

if __name__ == '__main__':
    main()
