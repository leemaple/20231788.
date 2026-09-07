"""Build exact-source independent review packet using audited ZIP/scan helpers."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = '2c14d7f394ec385029716d00f9c03fad974ba88b'
BASELINE = 'e6c4cc1ddfa261ee17681cbfc1ca6023660df5cb'
BRANCH = 'codex/s100-fresh-error-repair-20260907'
HELPER = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'

def main():
    if hashlib.sha256(HELPER.read_bytes()).hexdigest() != HELPER_SHA:
        raise RuntimeError('helper identity changed')
    spec = importlib.util.spec_from_file_location('audited_packet_helpers', HELPER)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    h.require(h.git('status', '--porcelain') == b'', 'clean worktree required')
    h.require(h.git('branch', '--show-current').decode().strip() == BRANCH, 'branch mismatch')
    head = h.git('rev-parse', 'HEAD').decode().strip()
    h.git('merge-base', '--is-ancestor', SOURCE, head)
    h.require(h.git('diff', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows') == b'', 'source drift')
    payloads = {}
    def add_git(commit, path, dest):
        blob, mode, oid = h.git_entry(commit, path)
        h.add(payloads, dest, blob, {'kind':'cleanroom_git_blob','commit':commit,'path':path,'mode':mode,'git_blob':oid})
    add_git(head, 'coordination/s100-condition-decision-01/TASK.md', 'TASK.md')
    add_git(SOURCE, 'coordination/s100-fresh-error-repair-01/remote-green1-failed-step.log', 'evidence/GREEN1_COMPILER.txt')
    paths = h.git('ls-tree', '-r', '--name-only', SOURCE, '--', 'src', 'include', 'tests').decode().splitlines()
    paths += ['CMakeLists.txt', '.github/workflows/dcp-rcb.yml']
    for path in sorted(paths):
        add_git(SOURCE, path, 'project/' + path)
    for path in ['include/openfhe_2023_1788/high_precision_client_io.h', 'src/high_precision_client_io.cpp', 'CMakeLists.txt', '.github/workflows/dcp-rcb.yml']:
        add_git(BASELINE, path, 'baseline/' + path)
    evidence_paths = [
        "coordination/s100-fresh-error-repair-01/GREEN2_RESULT.md",
        "coordination/s100-fresh-error-repair-01/GREEN2_RUN.json",
        "coordination/s100-fresh-error-repair-01/remote-green2-diagnostic-steps.log",
        "coordination/s100-fresh-error-repair-01/remote-green2-provenance-summary.log",
        "coordination/s100-fresh-error-repair-01/FRESH_IDEAL_PROPAGATION.md",
        "coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py",
        "coordination/s100-fresh-error-repair-01/PAPER_EXPERIMENT_PROVENANCE.md",
        "coordination/s100-fresh-error-repair-01/OBSERVER_ORDER_RED_RESULT.md",
        "coordination/s100-fresh-error-repair-01/OBSERVER_ORDER_RED_EVIDENCE.txt",
        "coordination/s100-fresh-error-repair-01/OBSERVER_ORDER_GREEN_RESULT.md",
        "coordination/s100-fresh-error-repair-01/OBSERVER_ORDER_GREEN_EVIDENCE.txt",
        "coordination/s100-independent-semantic-review-01/RETURN_DISPOSITION.md",
        "coordination/s100-independent-semantic-review-01/RETURN_INTAKE.json",
        "coordination/s100-independent-semantic-review-01/pro/REVIEW.md",
        "coordination/s100-independent-semantic-review-01/pro/CLAIM_BOUNDARY.md",
        "coordination/s100-independent-semantic-review-01/pro/NEXT_STEP.md",
        "coordination/s100-independent-semantic-review-01/pro/MANIFEST.json",
        "coordination/fs-endpoint-scientific-review-return-01/ACCEPTANCE.md",
        "coordination/fs-endpoint-scientific-review-return-01/MATH_REVIEW.md",
        "coordination/fs-endpoint-scientific-review-return-01/pro/DECISION.md",
        "coordination/fs-endpoint-scientific-review-return-01/pro/FINDINGS.md",
        "coordination/fs-endpoint-scientific-review-return-01/pro/results/independent_linux.json",
        "coordination/fs-endpoint-scientific-review-return-01/pro/results/independent_windows.json"
    ]
    for path in evidence_paths:
        add_git(head, path, 'evidence/' + path)
    prior = ROOT / 'artifacts/handoffs/s100-fresh-error-repair-01/s100-fresh-error-repair-e6c4cc1.zip'
    decoded, rows = h.decode_manifest_archive(prior,1514765,
        '58bdc872577c3e267dd2f1fe823bf1751242abf5520e211cceed2834be6e14aa',
        '28c37db169b61efe1afe5504c7a2694fcd7e2e640e6904a2322f3ffc966c3d8c',181,'manifest_self_excluded',True)
    for prefix, count in [('references/official-full/',77),('references/paper/',2),('references/boost-1.83.0/',4)]:
        names = [n for n in rows if n.startswith(prefix)]
        h.require(len(names) == count, 'reference closure changed')
        for name in names:
            h.add(payloads,name,decoded[name],{'kind':'pinned_pristine_reference','input_manifest_row':rows[name]})
    h.validate_names(list(payloads))
    selected_targeted = h.targeted_content_scan(payloads,'selected independent review')
    selected_scan = h.gitleaks_scan(payloads,'selected independent review')
    manifest = {'manifest_self_excluded':True,'schema':'s100-condition-decision-v1',
        'source_commit':SOURCE,'task_commit':head,'branch':BRANCH,'source_status':'clean',
        'selected_targeted':selected_targeted,'selected_gitleaks':selected_scan,
        'files':[{'path':n,'bytes':len(v['bytes']),'sha256':h.sha256(v['bytes']),'origin':v['origin']} for n,v in sorted(payloads.items())]}
    final = {n:v['bytes'] for n,v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest,indent=2,sort_keys=True)+'\n').encode()
    output = ROOT / 'artifacts/handoffs/s100-condition-decision-01/s100-condition-decision-2c14d7f.zip'
    output.parent.mkdir(parents=True,exist_ok=True)
    h.require(not output.parent.is_symlink(),'symlink output directory')
    with zipfile.ZipFile(output,'x',compression=zipfile.ZIP_DEFLATED,compresslevel=6) as archive:
        for name, blob in sorted(final.items()):
            info = zipfile.ZipInfo(name,(2026,9,7,0,0,0))
            info.create_system=3
            info.external_attr=(stat.S_IFREG|0o644)<<16
            info.compress_type=zipfile.ZIP_DEFLATED
            archive.writestr(info,blob)
    verified = h.verify_final_archive(output,final)
    members = {n:{'bytes':b} for n,b in verified.items()}
    final_targeted = h.targeted_content_scan(members,'decoded independent review ZIP')
    final_scan = h.gitleaks_scan(members,'decoded independent review ZIP')
    h.require(h.git('status','--porcelain') == b'','worktree changed while packaging')
    raw = output.read_bytes()
    receipt = {'archive_path':str(output),'bytes':len(raw),'sha256':h.sha256(raw),
        'source_commit':SOURCE,'task_commit':head,'source_status':'clean','branch':BRANCH,
        'members':len(final),'included_paths':sorted(final),'manifest_sha256':h.sha256(final['MANIFEST.json']),
        'selected_targeted':selected_targeted,'selected_gitleaks':selected_scan,
        'final_targeted':final_targeted,'final_gitleaks':final_scan,'crc_manifest_exact_bytes':'PASS'}
    with (HERE/'PACKET_RECEIPT.json').open('x') as stream:
        json.dump(receipt,stream,indent=2,sort_keys=True)
        stream.write('\n')
    print(json.dumps({k:receipt[k] for k in ['archive_path','bytes','sha256','members','source_commit','task_commit','crc_manifest_exact_bytes']},indent=2))

if __name__ == '__main__':
    main()
