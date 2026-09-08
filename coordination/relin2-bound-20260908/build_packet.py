"""Bounded mathematical handoff from verified clean-room bytes; no code execution."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = 'a4b815a733efe81897325e2a8e4c826a4ebfa439'
BASE = 'bbd4e73af74d1b072e3beb588cf9c7c4de3117cc'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
PRIOR_SHA = 'abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'


def main():
    helper = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    assert hashlib.sha256(helper.read_bytes()).hexdigest() == HELPER_SHA
    spec = importlib.util.spec_from_file_location('audited_handoff_helpers', helper)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    head = h.git('rev-parse', 'HEAD').decode().strip()
    h.require(h.git('branch', '--show-current').decode().strip() == 'codex/parameter-atlas-20260908', 'branch drift')
    h.require(h.git('diff', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows') == b'', 'runtime drift')
    h.require(h.git('diff', '--name-only') == b'' and h.git('diff', '--cached', '--name-only') == b'', 'tracked edits')
    untracked = set(h.git('ls-files', '--others', '--exclude-standard').decode().splitlines())
    allowed = {'coordination/relin2-bound-20260908/INDEPENDENT_SOURCE_MAP.md'}
    h.require(untracked <= allowed, 'unexpected untracked files')
    prior, rows = h.decode_manifest_archive(
        ROOT / 'artifacts/handoffs/parameter-atlas-20260908/openfhe-parameter-atlas-a4b815a.zip',
        2273139, PRIOR_SHA, '1dd20ae1b69b342f243ab57c9c7a8cadb9ca6002d5a95cc6d7746a787bdbc0ac',
        454, 'manifest_self_excluded', True)
    payloads = {}
    for name in sorted(rows):
        if name == 'TASK.md':
            continue
        if name.endswith('.log'):
            h.ALLOWED_LOGS.add(name)
        h.add(payloads, name, prior[name], {'kind': 'verified_cleanroom_atlas_input', 'archive_sha256': PRIOR_SHA, 'row': rows[name]})
    h.add(payloads, 'provenance/ATLAS_INPUT_MANIFEST.json', prior['MANIFEST.json'], {'kind': 'historical_source_manifest', 'archive_sha256': PRIOR_SHA})

    def add_git(commit, path, dest=None):
        blob, mode, oid = h.git_entry(commit, path)
        if (dest or path).endswith('.log'):
            h.ALLOWED_LOGS.add(dest or path)
        h.add(payloads, dest or path, blob, {'kind': 'fixed_cleanroom_git_blob', 'commit': commit, 'path': path, 'git_blob': oid, 'mode': mode})

    add_git(head, 'coordination/relin2-bound-20260908/TASK.md', 'TASK.md')
    for prefix in ('docs/parameter-atlas/reference', 'coordination/build-provenance-20260908'):
        for path in h.git('ls-tree', '-r', '--name-only', BASE, '--', prefix).decode().splitlines():
            add_git(BASE, path)
    for path in ('docs/parameter-atlas/README.zh-CN.md', 'docs/parameter-atlas/REVIEW_NOTES.zh-CN.md'):
        add_git(BASE, path)
    extras = [
        'coordination/parameter-atlas-20260908/OPENMP_PARAMETER_SEMANTICS.md',
        'coordination/parameter-atlas-20260908/ROOT_PAPER_CROSSCHECK.md',
        'coordination/fs-endpoint-scientific-review-return-01/pro/DECISION.md',
        'coordination/fs-endpoint-scientific-review-return-01/pro/FINDINGS.md',
        'coordination/s100-condition-decision-01/pro/DECISION.md',
    ]
    for path in extras:
        add_git(BASE, path, 'context/' + path)
    h.validate_names(list(payloads))
    pre_targeted = h.targeted_content_scan(payloads, 'selected Relin2 source material')
    pre_scan = h.gitleaks_scan(payloads, 'selected Relin2 source material')
    manifest = {'schema': 'relin2-implementation-bound-input-v1', 'manifest_self_excluded': True,
        'source_commit': SOURCE, 'evidence_commit': BASE, 'task_commit': head, 'official_pin': PIN,
        'branch': 'codex/parameter-atlas-20260908', 'tracked_state': 'clean',
        'independent_map_excluded': True, 'prior_archive_embedded': False,
        'excluded': ['quarantined implementation', 'local modified OpenFHE', 'builds', 'state', 'credentials', 'nested archives', 'full-slot captures', 'independent map'],
        'selected_targeted_scan': pre_targeted, 'selected_gitleaks_scan': pre_scan,
        'files': [{'path': n, 'bytes': len(v['bytes']), 'sha256': h.sha256(v['bytes']), 'origin': v['origin']} for n, v in sorted(payloads.items())]}
    final = {n: v['bytes'] for n, v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode()
    out = ROOT / 'artifacts/handoffs/relin2-bound-20260908/relin2-implementation-bound-a4b815a.zip'
    out.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as z:
        for name, blob in sorted(final.items()):
            info = zipfile.ZipInfo(name, (2026, 9, 8, 0, 0, 0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(info, blob)
    decoded = h.verify_final_archive(out, final)
    content = {n: {'bytes': b} for n, b in decoded.items()}
    post_targeted = h.targeted_content_scan(content, 'all decoded final Relin2 members')
    post_scan = h.gitleaks_scan(content, 'all decoded final Relin2 members')
    h.require(h.git('rev-parse', 'HEAD').decode().strip() == head, 'HEAD moved during packaging')
    h.require(h.git('diff', '--name-only') == b'' and h.git('diff', '--cached', '--name-only') == b'', 'tracked edits during packaging')
    receipt = {'archive_path': str(out), 'bytes': out.stat().st_size, 'sha256': h.sha256(out.read_bytes()),
        'members': len(final), 'manifest_sha256': h.sha256(final['MANIFEST.json']),
        'source_commit': SOURCE, 'evidence_commit': BASE, 'task_commit': head,
        'included_paths': sorted(final), 'selected_targeted': pre_targeted, 'selected_gitleaks': pre_scan,
        'final_targeted': post_targeted, 'final_gitleaks': post_scan,
        'crc_paths_regular_manifest_byte_closure': 'PASS', 'untracked_excluded_at_start': sorted(untracked)}
    with (HERE / 'PACKET_RECEIPT.json').open('x') as f:
        json.dump(receipt, f, indent=2)
        f.write('\n')
    print(json.dumps({k: receipt[k] for k in ('archive_path', 'bytes', 'sha256', 'members', 'source_commit', 'task_commit', 'manifest_sha256')}, indent=2))


if __name__ == '__main__':
    main()
