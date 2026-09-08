"""Package one lift/nonwrap task from pinned, scanned clean-room material."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = 'a4b815a733efe81897325e2a8e4c826a4ebfa439'
BASE = 'b86105b33294a84ce76d65f585eb16d25ae07156'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
PRIOR = Path('/Users/lifeng/Documents/20231788-openfhe-parameter-atlas-20260908/artifacts/handoffs/relin2-bound-20260908/relin2-implementation-bound-a4b815a.zip')
PRIOR_SHA = '1aba188016d93ca8b38ec72f0f2592c6b45de562c455cb33e6d303d95ec6dc4c'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'


def main():
    helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    assert hashlib.sha256(helper.read_bytes()).hexdigest() == HELPER_SHA
    spec = importlib.util.spec_from_file_location('audited_helpers', helper)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    head = h.git('rev-parse', 'HEAD').decode().strip()
    h.require(h.git('branch', '--show-current').decode().strip() == 'codex/initial-lift-nonwrap-20260909', 'branch drift')
    h.require(h.git('diff', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows') == b'', 'runtime drift')
    h.require(h.git('diff', '--name-only') == b'' and h.git('diff', '--cached', '--name-only') == b'', 'tracked edits')
    untracked = set(h.git('ls-files', '--others', '--exclude-standard').decode().splitlines())
    allowed = {'coordination/initial-lift-nonwrap-20260909/INDEPENDENT_INITIAL_MAP.md'}
    h.require(untracked <= allowed, 'unexpected untracked files')
    prior, rows = h.decode_manifest_archive(PRIOR, 3319606, PRIOR_SHA,
        '34b3a05c62c48a55dba5bdcdc8fde0e1f397cb831dce35a95f53cc97129422b3',
        500, 'manifest_self_excluded', True)
    payloads = {}
    for name in sorted(rows):
        if name in ('TASK.md', 'docs/parameter-atlas/README.zh-CN.md'):
            continue
        if name.endswith('.log'):
            h.ALLOWED_LOGS.add(name)
        h.add(payloads, name, prior[name], {'kind': 'verified_cleanroom_relin_input', 'archive_sha256': PRIOR_SHA, 'row': rows[name]})
    h.add(payloads, 'provenance/RELIN_INPUT_MANIFEST.json', prior['MANIFEST.json'], {'kind': 'source_manifest', 'archive_sha256': PRIOR_SHA})

    def add_git(commit, path, dest=None):
        blob, mode, oid = h.git_entry(commit, path)
        h.add(payloads, dest or path, blob, {'kind': 'fixed_cleanroom_git_blob', 'commit': commit, 'path': path, 'git_blob': oid, 'mode': mode})

    add_git(head, 'coordination/initial-lift-nonwrap-20260909/TASK.md', 'TASK.md')
    add_git(BASE, 'docs/parameter-atlas/README.zh-CN.md')
    prefix = 'coordination/relin2-bound-20260908/pro'
    for path in h.git('ls-tree', '-r', '--name-only', BASE, '--', prefix).decode().splitlines():
        add_git(BASE, path)
    for name in ('ADOPTED_CONTRACT.md', 'ROOT_RETURN_REVIEW.md', 'RETURN_MATH_REVIEW.md',
                 'ROOT_REPLAY.json', 'INDEPENDENT_SOURCE_MAP.md', 'ROOT_ALGEBRA_DRAFT.md',
                 'check_balanced_carry.py', 'BALANCED_CARRY_CHECK.json'):
        add_git(BASE, 'coordination/relin2-bound-20260908/' + name)
    for path in ('coordination/s100-fresh-error-repair-01/PAPER_EXPERIMENT_PROVENANCE.md',
                 'coordination/s100-fresh-error-repair-01/FRESH_IDEAL_PROPAGATION.md',
                 'coordination/annulus-independent-pro-review-20260908/pro/NEXT_ACTION.md',
                 'coordination/comprehensive-reassessment-20260908/pro/DECISION.md'):
        add_git(BASE, path, 'context/latest/' + path)
    h.validate_names(list(payloads))
    selected_targeted = h.targeted_content_scan(payloads, 'selected lift/nonwrap input')
    selected_gitleaks = h.gitleaks_scan(payloads, 'selected lift/nonwrap input')
    manifest = {'schema': 'initial-lift-nonwrap-input-v1', 'manifest_self_excluded': True,
        'source_commit': SOURCE, 'evidence_commit': BASE, 'task_commit': head, 'official_pin': PIN,
        'branch': 'codex/initial-lift-nonwrap-20260909', 'tracked_state': 'clean',
        'independent_initial_map_excluded': True,
        'excluded': ['quarantined implementation', 'modified local OpenFHE', 'builds', 'runtime/browser state', 'credentials', 'nested archives', 'full-slot captures', 'new independent initial map'],
        'selected_targeted_scan': selected_targeted, 'selected_gitleaks_scan': selected_gitleaks,
        'files': [{'path': n, 'bytes': len(v['bytes']), 'sha256': h.sha256(v['bytes']), 'origin': v['origin']} for n, v in sorted(payloads.items())]}
    final = {n: v['bytes'] for n, v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode()
    out = ROOT / 'artifacts/handoffs/initial-lift-nonwrap-20260909/initial-lift-nonwrap-a4b815a.zip'
    out.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as z:
        for name, blob in sorted(final.items()):
            info = zipfile.ZipInfo(name, (2026, 9, 9, 0, 0, 0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(info, blob)
    decoded = h.verify_final_archive(out, final)
    content = {n: {'bytes': b} for n, b in decoded.items()}
    final_targeted = h.targeted_content_scan(content, 'all decoded lift/nonwrap input')
    final_gitleaks = h.gitleaks_scan(content, 'all decoded lift/nonwrap input')
    h.require(h.git('rev-parse', 'HEAD').decode().strip() == head, 'HEAD moved')
    h.require(h.git('diff', '--name-only') == b'' and h.git('diff', '--cached', '--name-only') == b'', 'tracked edits during packaging')
    receipt = {'archive_path': str(out), 'bytes': out.stat().st_size, 'sha256': h.sha256(out.read_bytes()),
        'members': len(final), 'manifest_sha256': h.sha256(final['MANIFEST.json']),
        'source_commit': SOURCE, 'evidence_commit': BASE, 'task_commit': head,
        'included_paths': sorted(final), 'selected_targeted': selected_targeted, 'selected_gitleaks': selected_gitleaks,
        'final_targeted': final_targeted, 'final_gitleaks': final_gitleaks,
        'crc_paths_regular_manifest_byte_closure': 'PASS', 'untracked_excluded_at_start': sorted(untracked)}
    with (HERE / 'PACKET_RECEIPT.json').open('x') as f:
        json.dump(receipt, f, indent=2)
        f.write('\n')
    print(json.dumps({k: receipt[k] for k in ('archive_path', 'bytes', 'sha256', 'members', 'source_commit', 'task_commit', 'manifest_sha256')}, indent=2))


if __name__ == '__main__':
    main()
