"""Build the renewed scientific handoff from pinned clean-room Git and references."""
import gzip
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = '3c02988fb5655dc6ea48f4f7d559e62d9d4a9d37'
BRANCH = 'codex/s100-fresh-error-repair-20260907'
HELPER = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'

def main():
    if hashlib.sha256(HELPER.read_bytes()).hexdigest() != HELPER_SHA:
        raise RuntimeError('audited helper changed')
    spec = importlib.util.spec_from_file_location('audited_helpers', HELPER)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    h.require(h.git('status', '--porcelain') == b'', 'clean worktree required')
    h.require(h.git('branch', '--show-current').decode().strip() == BRANCH, 'branch changed')
    head = h.git('rev-parse', 'HEAD').decode().strip()
    h.git('merge-base', '--is-ancestor', SOURCE, head)
    h.require(h.git('diff', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows') == b'', 'source drift')
    allowed_logs = {
        'evidence/coordination/s100-fresh-error-repair-01/remote-green2-diagnostic-steps.log',
        'evidence/coordination/s100-fresh-error-repair-01/remote-green2-provenance-summary.log',
    }
    h.ALLOWED_LOGS |= allowed_logs
    prior = ROOT / 'artifacts/handoffs/s100-condition-decision-01/s100-condition-decision-2c14d7f.zip'
    decoded, rows = h.decode_manifest_archive(prior, 1620988,
        '3bbb738d56a17f76807546f5f7fc4a1f4c83e889164e1dab71f0409d190e417b',
        '0eb3c9674262cbaf9bc79202139576343b039b32826b2ff2af8194860e00d80b',
        200, 'manifest_self_excluded', True)
    payloads = {}
    for name, row in rows.items():
        if name.startswith(('references/', 'baseline/', 'evidence/')):
            h.add(payloads, name, decoded[name], {'kind':'pinned_previous_evidence_or_reference', 'input_manifest_row':row})
    def add_git(commit, path, dest):
        blob, mode, oid = h.git_entry(commit, path)
        h.add(payloads, dest, blob, {'kind':'cleanroom_git_blob', 'commit':commit, 'path':path, 'mode':mode, 'git_blob':oid})
    add_git(head, 'coordination/comprehensive-reassessment-20260908/TASK.md', 'TASK.md')
    paths = h.git('ls-tree', '-r', '--name-only', SOURCE, '--', 'src', 'include', 'tests').decode().splitlines()
    paths += ['CMakeLists.txt', '.github/workflows/dcp-rcb.yml', 'README.md', 'REPRODUCE.zh-CN.md', 'CHECK_AND_HANDOFF.zh-CN.md']
    for path in sorted(paths):
        add_git(SOURCE, path, 'project/' + path)
    evidence = {
        's100-condition-decision-01': ['RETURN_DISPOSITION.md', 'RETURN_INTAKE.json', 'pro/DECISION.md', 'pro/NEXT_ACTION.md', 'pro/MANIFEST.json'],
        's100-output-finalization-01': ['TASK.md', 'REVIEW.md', 'RED_RESULT.md', 'RED_EVIDENCE.txt', 'GREEN_RESULT.md', 'GREEN_EVIDENCE.txt'],
        'precision116-eight-square-return-01': ['ACCEPTANCE.md', 'HOSTED_EXECUTION.md', 'ROOT_RUN_RECEIPT.json', 'RUN_34055816234_STATUS.json', 'RUNTIME_REVIEW.md', 'NUMERICAL_REVIEW.md', 'FINAL_REQUIREMENTS_AUDIT.md'],
        'fs-endpoint-live-run-01': ['ACCEPTANCE.md', 'RUN_TERMINAL.json', 'LINUX_AUDIT.json', 'WINDOWS_AUDIT.json', 'AUDIT_REVIEW.md', 'LOG_TRANSPORT.md'],
    }
    for directory, names in evidence.items():
        for name in names:
            path = f'coordination/{directory}/{name}'
            add_git(SOURCE, path, 'evidence/' + path)
    # Decompress the two existing full-slot public diagnostic tables so both
    # selected and final secret scans see their exact content, not opaque gzip.
    for platform in ['linux', 'windows']:
        prefix = f'coordination/fs-endpoint-live-run-01/{platform}'
        for path in h.git('ls-tree', '-r', '--name-only', SOURCE, '--', prefix).decode().splitlines():
            blob, mode, oid = h.git_entry(SOURCE, path)
            origin = {'kind':'cleanroom_git_blob', 'commit':SOURCE, 'path':path, 'mode':mode, 'git_blob':oid, 'source_sha256':h.sha256(blob)}
            dest = 'evidence/' + path
            if path.endswith('.tsv.gz'):
                h.require(len(blob) < 5_000_000, 'unexpected compressed table size')
                blob = gzip.decompress(blob)
                h.require(len(blob) < 50_000_000, 'unexpected decoded table size')
                dest = dest.removesuffix('.gz')
                origin['transform'] = 'gzip.decompress; original Git gzip source/hash retained above'
            else:
                h.require(path.endswith('.status.json'), 'unexpected endpoint evidence file')
            h.add(payloads, dest, blob, origin)
    h.validate_names(list(payloads))
    selected_targeted = h.targeted_content_scan(payloads, 'renewed selected source/evidence')
    selected_scan = h.gitleaks_scan(payloads, 'renewed selected source/evidence')
    manifest = {'schema':'comprehensive-reassessment-20260908-v1', 'manifest_self_excluded':True,
        'source_commit':SOURCE, 'task_commit':head, 'branch':BRANCH, 'source_status':'clean',
        'selected_targeted':selected_targeted, 'selected_gitleaks':selected_scan,
        'files':[{'path':n,'bytes':len(v['bytes']),'sha256':h.sha256(v['bytes']),'origin':v['origin']} for n,v in sorted(payloads.items())]}
    final = {n:v['bytes'] for n,v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True)+'\n').encode()
    output = ROOT / 'artifacts/handoffs/comprehensive-reassessment-20260908/comprehensive-reassessment-3c02988.zip'
    output.parent.mkdir(parents=True, exist_ok=True)
    h.require(not output.parent.is_symlink(), 'symlink output directory')
    with zipfile.ZipFile(output, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
        for name, blob in sorted(final.items()):
            info = zipfile.ZipInfo(name, (2026,9,8,0,0,0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG|0o644)<<16
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info,blob)
    verified = h.verify_final_archive(output,final)
    members = {n:{'bytes':b} for n,b in verified.items()}
    final_targeted = h.targeted_content_scan(members, 'renewed decoded final ZIP')
    final_scan = h.gitleaks_scan(members, 'renewed decoded final ZIP')
    h.require(h.git('status','--porcelain') == b'', 'worktree changed while packaging')
    h.require(h.git('rev-parse','HEAD').decode().strip() == head, 'HEAD changed while packaging')
    raw = output.read_bytes()
    receipt = {'archive_path':str(output), 'bytes':len(raw), 'sha256':h.sha256(raw),
        'source_commit':SOURCE, 'task_commit':head, 'source_status':'clean', 'branch':BRANCH,
        'members':len(final), 'included_paths':sorted(final), 'manifest_sha256':h.sha256(final['MANIFEST.json']),
        'selected_targeted':selected_targeted, 'selected_gitleaks':selected_scan,
        'final_targeted':final_targeted, 'final_gitleaks':final_scan, 'crc_manifest_exact_bytes':'PASS'}
    with (HERE/'PACKET_RECEIPT.json').open('x') as stream:
        json.dump(receipt, stream, indent=2, sort_keys=True)
        stream.write('\n')
    print(json.dumps({k:receipt[k] for k in ['archive_path','bytes','sha256','members','source_commit','task_commit','crc_manifest_exact_bytes']},indent=2))

if __name__ == '__main__':
    main()
