"""Exact-source completion-contract handoff. No numerical or browser work."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = 'a7f54de2701a1b9bc02660f66febeff707b56651'
BRANCH = 'codex/public-s100-ecd-cell-20260909'
PRIOR = Path('/Users/lifeng/Documents/20231788-openfhe-public-encoder-cap-20260909/artifacts/handoffs/reproduction-adjudication-20260909/reproduction-adjudication-31e24be.zip')
PRIOR_SHA = '7e3ea9fee4a04d5535cc47aab42a71e38367db18c70b804a609551345affcbe4'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'


def main():
    helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    assert hashlib.sha256(helper.read_bytes()).hexdigest() == HELPER_SHA
    spec = importlib.util.spec_from_file_location('audited_helpers', helper)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    head = h.git('rev-parse', 'HEAD').decode().strip()
    h.require(h.git('branch', '--show-current').decode().strip() == BRANCH, 'branch drift')
    h.require(not h.git('status', '--porcelain').strip(), 'dirty handoff source')
    changed = h.git('diff', '--name-only', SOURCE, head).decode().splitlines()
    h.require(all(n.startswith('coordination/completion-contract-20260909/') for n in changed), 'source drift')
    prior, rows = h.decode_manifest_archive(PRIOR, 5400318, PRIOR_SHA,
        '776fd05dc43adf11ca9a912487ae8bf5c1269dfe69149addd8263cbe34b8edaf',
        760, 'manifest_self_excluded', True)
    payloads = {}
    for name in sorted(rows):
        if name in ('TASK.md', 'TASK_PREFLIGHT.md') or name.startswith(('project/', 'docs/parameter-atlas/')):
            continue
        if name.endswith('.log'):
            h.ALLOWED_LOGS.add(name)
        h.add(payloads, name, prior[name], {'kind':'verified_prior_cleanroom_context',
              'archive_sha256':PRIOR_SHA, 'row':rows[name]})
    h.add(payloads, 'provenance/ADJUDICATION_INPUT_MANIFEST.json', prior['MANIFEST.json'],
          {'kind':'historical_not_current_project_manifest', 'archive_sha256':PRIOR_SHA})

    def add_git(commit, path, dest=None):
        name = dest or path
        if name in payloads:
            return
        if name.endswith('.log'):
            h.ALLOWED_LOGS.add(name)
        raw, mode, oid = h.git_entry(commit, path)
        h.add(payloads, name, raw, {'kind':'fixed_cleanroom_git_blob', 'commit':commit,
              'path':path, 'git_blob':oid, 'mode':mode})

    def tree(*prefixes):
        return h.git('ls-tree', '-r', '--name-only', SOURCE, '--', *prefixes).decode().splitlines()

    for path in tree('src', 'include', 'tests', 'diagnostics', '.github/workflows') + ['CMakeLists.txt']:
        add_git(SOURCE, path, 'project/' + path)
    for name in sorted(rows):
        if name.startswith('project/coordination/'):
            add_git(SOURCE, name.removeprefix('project/'), name)
    for path in tree('docs/parameter-atlas', 'coordination/reproduction-adjudication-20260909',
                     'coordination/public-s100-ecd-cell-20260909',
                     'coordination/annulus-independent-pro-review-20260908',
                     'coordination/s100-annulus125-20260908'):
        add_git(SOURCE, path)
    for path in ['CHECK_AND_HANDOFF.zh-CN.md', 'REPRODUCE.zh-CN.md',
                 'coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md',
                 'coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md',
                 'coordination/paper-scale-integration-01/PRODUCTION_CONTRACT_01.md',
                 'coordination/paper-scale-integration-01/NOMINAL_SCALE_AUDIT_01.md',
                 'coordination/precision116-eight-square-return-01/ACCEPTANCE.md']:
        add_git(SOURCE, path)
    for name in ['RUNTIME_REVIEW.md', 'HOSTED_EXECUTION.md', 'ROOT_RUN_RECEIPT.json',
                 'RUN_34055816234_STATUS.json', 'ROOT_INTAKE.json', 'NUMERICAL_REVIEW.md',
                 'INTEGRATION_DISPOSITION.md', 'FINAL_INTEGRATION_REVIEW.md',
                 'DEFAULT_PROMOTION.md', 'ROOT_DEFAULT_RUN_RECEIPT.json',
                 'DEFAULT_RUN_34057018442_STATUS.json', 'FINAL_REQUIREMENTS_AUDIT.md',
                 'REPRODUCTION_GUIDE_REVIEW.md']:
        add_git(SOURCE, 'coordination/precision116-eight-square-return-01/' + name)
    # Retain every input-domain/acceptance contract by exact Git, not a guessed
    # claim about author input selection. Paths are enumerated from the snapshot.
    for path in tree('coordination'):
        if Path(path).name in ('INPUT_DOMAIN_AUDIT.md', 'INPUT_DOMAIN_AUDIT.zh-CN.md'):
            add_git(SOURCE, path)
    for name in ['TASK.md', 'REQUIREMENTS_PREFLIGHT.md']:
        add_git(head, 'coordination/completion-contract-20260909/' + name, name)
    h.validate_names(list(payloads))
    selected_targeted = h.targeted_content_scan(payloads, 'completion-contract selection')
    selected_gitleaks = h.gitleaks_scan(payloads, 'completion-contract selection')
    manifest = {'schema':'completion-contract-input-v1', 'manifest_self_excluded':True,
        'source_commit':SOURCE, 'task_commit':head, 'branch':BRANCH, 'tracked_state':'clean',
        'official_pin':'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'historical_manifests_are_provenance_not_current_project_identity':True,
        'excluded':['quarantined implementations','modified local OpenFHE','build outputs',
                    'runtime/browser state','credentials','nested archives','secret-bearing captures'],
        'selection_targeted_scan':selected_targeted, 'selection_gitleaks_scan':selected_gitleaks,
        'files':[{'path':n,'bytes':len(v['bytes']),'sha256':h.sha256(v['bytes']),'origin':v['origin']}
                 for n,v in sorted(payloads.items())]}
    final = {n:v['bytes'] for n,v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest,indent=2,sort_keys=True)+'\n').encode()
    output = ROOT / 'artifacts/handoffs/completion-contract-20260909/completion-contract-a7f54de.zip'
    output.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(output,'x',compression=zipfile.ZIP_DEFLATED,compresslevel=6) as z:
        for name,raw in sorted(final.items()):
            info = zipfile.ZipInfo(name,(2026,9,9,0,0,0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(info,raw)
    decoded = h.verify_final_archive(output,final)
    content = {n:{'bytes':raw} for n,raw in decoded.items()}
    final_targeted = h.targeted_content_scan(content,'decoded completion-contract archive')
    final_gitleaks = h.gitleaks_scan(content,'decoded completion-contract archive')
    h.require(not h.git('status','--porcelain').strip(),'dirty after packaging')
    h.require(h.git('rev-parse','HEAD').decode().strip()==head,'HEAD moved')
    receipt = {'archive_path':str(output),'bytes':output.stat().st_size,'sha256':h.sha256(output.read_bytes()),
        'members':len(final),'expanded_bytes':sum(map(len,final.values())),
        'manifest_sha256':h.sha256(final['MANIFEST.json']),'source_commit':SOURCE,'task_commit':head,
        'selected_targeted':selected_targeted,'selected_gitleaks':selected_gitleaks,
        'final_targeted':final_targeted,'final_gitleaks':final_gitleaks,'included_paths':sorted(final),
        'crc_paths_regular_manifest_bytes':'PASS'}
    with (HERE/'PACKET_RECEIPT.json').open('x') as f:
        json.dump(receipt,f,indent=2);f.write('\n')
    print(json.dumps({k:receipt[k] for k in ('archive_path','bytes','sha256','members','expanded_bytes',
                                          'source_commit','task_commit','manifest_sha256')},indent=2))


if __name__ == '__main__':
    main()
