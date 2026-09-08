"""Exact-source independent semantic review packet; no builds or encrypted runs."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = '03f37b6ec7b8d6d87ff68d15d2843c3aa9ac6a9b'
BRANCH = 'codex/s100-annulus125-20260908'
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
    h.require(h.git('diff', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows') == b'', 'runtime source drift')
    prior_receipt = json.loads((ROOT / 'coordination/comprehensive-reassessment-20260908/PACKET_RECEIPT.json').read_text())
    h.require(prior_receipt['sha256'] == '08760af7def640afa06caf939c9ff7a5805d4cc6c567b49f24024cbf2f7c9158', 'prior packet identity changed')
    prior = Path(prior_receipt['archive_path'])
    decoded, rows = h.decode_manifest_archive(prior, 9045349, prior_receipt['sha256'],
        prior_receipt['manifest_sha256'], 231, 'manifest_self_excluded', True)
    inherited = [n for n in rows if n.startswith(('references/', 'baseline/', 'evidence/'))]
    # Explicitly inherited previously scanned diagnostic logs, not arbitrary local logs.
    h.ALLOWED_LOGS |= {n for n in inherited if n.endswith('.log')}
    payloads = {}
    for name in inherited:
        h.add(payloads, name, decoded[name], {'kind': 'verified_prior_cleanroom_reference_or_evidence', 'input_manifest_row': rows[name]})

    def add_git(commit, path, destination):
        blob, mode, oid = h.git_entry(commit, path)
        if destination.endswith('.log'):
            h.ALLOWED_LOGS.add(destination)
        h.add(payloads, destination, blob, {'kind': 'cleanroom_git_blob', 'commit': commit, 'path': path, 'mode': mode, 'git_blob': oid})

    add_git(head, 'coordination/annulus-independent-pro-review-20260908/TASK.md', 'TASK.md')
    paths = h.git('ls-tree', '-r', '--name-only', SOURCE, '--', 'src', 'include', 'tests', '.github/workflows').decode().splitlines()
    paths += ['CMakeLists.txt']
    for path in sorted(paths):
        add_git(SOURCE, path, 'project/' + path)
    prefix = 'coordination/s100-annulus125-20260908/'
    for path in h.git('ls-tree', '-r', '--name-only', head, '--', prefix).decode().splitlines():
        # Runtime artifacts and executable checks first; authored opinions withheld until first pass.
        destination = ('after-first-pass/' if path.endswith('.md') else 'project/') + path
        add_git(head, path, destination)
    for name in ['RESULT.zh-CN.md', 'EXECUTION_CONTRACT.md']:
        h.require('after-first-pass/' + prefix + name in payloads, 'missing bounded result/contract')
    pro_prefix = 'coordination/comprehensive-reassessment-20260908/pro/'
    for path in h.git('ls-tree', '-r', '--name-only', head, '--', pro_prefix).decode().splitlines():
        add_git(head, path, 'after-first-pass/' + path)
    for path in ['CHECK_AND_HANDOFF.zh-CN.md', 'coordination/comprehensive-reassessment-20260908/RETURN_DISPOSITION.md', 'coordination/s100-fresh-error-repair-01/PAPER_EXPERIMENT_PROVENANCE.md']:
        add_git(head, path, 'after-first-pass/' + path)
    h.validate_names(list(payloads))
    selected_targeted = h.targeted_content_scan(payloads, 'selected independent review input')
    selected_scan = h.gitleaks_scan(payloads, 'selected independent review input')
    manifest = {'schema': 'annulus-independent-review-v1', 'manifest_self_excluded': True,
        'source_commit': SOURCE, 'task_evidence_commit': head, 'branch': BRANCH, 'source_status': 'clean',
        'selected_targeted': selected_targeted, 'selected_gitleaks': selected_scan,
        'files': [{'path': n, 'bytes': len(v['bytes']), 'sha256': h.sha256(v['bytes']), 'origin': v['origin']} for n, v in sorted(payloads.items())]}
    final = {n: v['bytes'] for n, v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode()
    output = ROOT / 'artifacts/handoffs/annulus-independent-pro-review-20260908/annulus-independent-03f37b6.zip'
    output.parent.mkdir(parents=True, exist_ok=True)
    h.require(not output.parent.is_symlink(), 'symlink output directory')
    with zipfile.ZipFile(output, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
        for name, blob in sorted(final.items()):
            info = zipfile.ZipInfo(name, (2026, 9, 8, 0, 0, 0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, blob)
    verified = h.verify_final_archive(output, final)
    members = {n: {'bytes': b} for n, b in verified.items()}
    final_targeted = h.targeted_content_scan(members, 'decoded final independent review ZIP')
    final_scan = h.gitleaks_scan(members, 'decoded final independent review ZIP')
    h.require(h.git('status', '--porcelain') == b'', 'worktree changed while packaging')
    h.require(h.git('rev-parse', 'HEAD').decode().strip() == head, 'HEAD changed while packaging')
    raw = output.read_bytes()
    receipt = {'archive_path': str(output), 'bytes': len(raw), 'sha256': h.sha256(raw),
        'source_commit': SOURCE, 'task_evidence_commit': head, 'source_status': 'clean', 'branch': BRANCH,
        'members': len(final), 'included_paths': sorted(final), 'manifest_sha256': h.sha256(final['MANIFEST.json']),
        'selected_targeted': selected_targeted, 'selected_gitleaks': selected_scan,
        'final_targeted': final_targeted, 'final_gitleaks': final_scan, 'crc_manifest_exact_bytes': 'PASS'}
    with (HERE / 'PACKET_RECEIPT.json').open('x') as stream:
        json.dump(receipt, stream, indent=2, sort_keys=True)
        stream.write('\n')
    print(json.dumps({k: receipt[k] for k in ['archive_path', 'bytes', 'sha256', 'members', 'source_commit', 'task_evidence_commit', 'crc_manifest_exact_bytes']}, indent=2))


if __name__ == '__main__':
    main()
