#!/usr/bin/env python3
"""Package the exact tested S116 source and bounded eight-square task, offline.

Reuse the already reviewed path/manifest/scanner helpers, not the older
builder's task-specific main or source selection. No compilation or upload.
"""
import importlib.util
import json
import os
from pathlib import Path
import stat
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = ROOT / 'coordination/precision116-eight-square-01'
BRANCH = 'codex/precision116-eight-square-20260907'
SOURCE = '2759fa90840946ef42957c7ba71ebea47e0e4995'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
PRIOR = Path('/Users/lifeng/Documents/20231788-openfhe-precision116-seam-20260907/artifacts/handoffs/precision116-pro-handoff-01/experimental-precision116-profile-seam-dbbbee0d.zip')
PRIOR_SHA = '75c29c9f1305c44fc64679827dcfc1e8b3ba475b48ab46900f71849c255f9a7b'
PRIOR_MANIFEST_SHA = '40a4e8ee30121c85800b6eea4a541b52c11df4fd7b56cdd8da1c4da11207be91'
OUTPUT = ROOT / 'artifacts/handoffs/precision116-eight-square-01/experimental-precision116-eight-square-2759fa90.zip'
HELPER = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'
TASK_SHA = '1fb9e2188ba8f4f90f3b4ac3d5fe7c4bdd248529363001366fe2e4cbfdf71a0d'
PREFLIGHT_SHA = '643e7c872c2a7542fadbb966075c03bb96d3ee2d083c4e2c96c667fa1eead057'


def main():
    import hashlib
    if hashlib.sha256(HELPER.read_bytes()).hexdigest() != HELPER_SHA:
        raise RuntimeError('reviewed helper identity changed')
    spec = importlib.util.spec_from_file_location('reviewed_packet_helpers', HELPER)
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)  # __main__ is not executed.
    helper.ROOT = ROOT
    check = helper.require
    git = helper.git
    sha = helper.sha256
    check(git('branch', '--show-current').decode().strip() == BRANCH, 'wrong branch')
    check(git('status', '--porcelain=v1', '--untracked-files=all') == b'', 'packaging requires clean worktree')
    head = git('rev-parse', 'HEAD').decode().strip()
    git('merge-base', '--is-ancestor', SOURCE, head)
    changed = git('diff', '--name-only', SOURCE, head).decode().splitlines()
    check(all(p.startswith('coordination/') for p in changed), 'non-document source drift from tested commit')
    check(not OUTPUT.exists(), 'refusing to overwrite packet')

    evidence_names = ['RED_LINUX_JOB.log', 'RED_WINDOWS_JOB.log', 'RED_RUN_STATUS.json',
                      'GREEN_LINUX_JOB.log', 'GREEN_WINDOWS_JOB.log', 'GREEN_RUN_STATUS.json']
    helper.ALLOWED_LOGS.update('evidence/profile-seam/' + n for n in evidence_names if n.endswith('.log'))
    payloads = {}

    def add_git(commit, path, dest):
        blob, mode, oid = helper.git_entry(commit, path)
        helper.add(payloads, dest, blob, {'kind': 'cleanroom_git_blob', 'commit': commit,
                                        'path': path, 'mode': mode, 'git_blob': oid})
        return blob

    for name, expected in [('TASK.md', TASK_SHA), ('TASK_PREFLIGHT.md', PREFLIGHT_SHA)]:
        blob = add_git(head, 'coordination/precision116-eight-square-01/' + name, name)
        check(sha(blob) == expected, 'task/preflight changed after closure: ' + name)

    source_paths = sorted(git('ls-tree', '-r', '--name-only', SOURCE, '--', 'src', 'include', 'tests')
                          .decode().splitlines() + ['CMakeLists.txt', '.github/workflows/dcp-rcb.yml'])
    check(len(source_paths) == len(set(source_paths)) == 85, 'unexpected exact source inventory')
    for path in source_paths:
        add_git(SOURCE, path, 'project/' + path)

    requirements = {
        'coordination/precision116-pro-handoff-01/TASK.md': 'PREVIOUS_PROFILE_TASK.md',
        'coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md': 'CORRECTNESS_ACCEPTANCE_SCOPE.md',
        'coordination/PAPER_PRECISION_PARAMETER_GATES.md': 'PAPER_PRECISION_PARAMETER_GATES.md',
        'coordination/paper-scale-integration-01/PRODUCTION_CONTRACT_01.md': 'ORIGINAL_PAPER_PRODUCTION_CONTRACT.md',
        'coordination/paper-scale-integration-01/INPUT_DOMAIN_AUDIT_01.md': 'ORIGINAL_INPUT_DOMAIN_AUDIT.md',
        'coordination/fs-precision-profile-feasibility-01/candidate.json': 'candidate.json',
        'coordination/fs-precision-profile-feasibility-01/STATIC_CERTIFICATE.json': 'STATIC_CERTIFICATE.json',
        'coordination/fs-precision-profile-feasibility-01/ROOT_FRESH_BUDGET.md': 'CONDITIONAL_FRESH_BUDGET.md',
    }
    for path, name in requirements.items():
        add_git(SOURCE, path, 'requirements/' + name)
    for name in evidence_names:
        add_git(head, 'coordination/precision116-pro-return-01/' + name, 'evidence/profile-seam/' + name)

    # References come only from this pinned previously verified clean-room packet.
    # Old project bytes are never substituted for the tested source above.
    inherited, rows = helper.decode_manifest_archive(PRIOR, 1543581, PRIOR_SHA,
        PRIOR_MANIFEST_SHA, 198, 'manifest_self_excluded', True)
    check(json.loads(inherited['MANIFEST.json'])['official_pin'] == PIN, 'pristine pin mismatch')
    prefixes = {'references/official-full/': 77, 'references/boost-1.83.0/': 4,
                'references/paper/': 2, 'instructions/openfhe-2023-1788-workflow/': 4}
    for prefix, count in prefixes.items():
        names = sorted(n for n in rows if n.startswith(prefix))
        check(len(names) == count, 'reference closure changed: ' + prefix)
        for name in names:
            helper.add(payloads, name, inherited[name], {'kind': 'verified_packet_reference',
                'archive_sha256': PRIOR_SHA, 'manifest_sha256': PRIOR_MANIFEST_SHA,
                'input_manifest_row': rows[name]})

    expected_count = 2 + 85 + len(requirements) + len(evidence_names) + sum(prefixes.values())
    check(len(payloads) == expected_count, 'unexpected payload count')
    helper.validate_names(list(payloads))
    targeted = helper.targeted_content_scan(payloads, 'selected payloads')
    selected_scan = helper.gitleaks_scan(payloads, 'selected payloads before manifest')
    manifest = {
        'schema': 'experimental-precision116-eight-square-input-v1',
        'manifest_self_excluded': True, 'source_commit': SOURCE, 'official_pin': PIN,
        'packaging_documentation_head': head, 'branch': BRANCH, 'source_tree_status': 'clean',
        'no_quarantined_implementation': True, 'no_nested_archives': True,
        'semantic_review_independence': 'prior author/reviewer verdicts omitted; source/spec/raw execution evidence supplied',
        'selection_counts': {'source_test_build': 85, 'task_preflight': 2,
            'requirements': len(requirements), 'raw_job_logs_and_status': 6,
            'references_and_workflow': prefixes, 'total_payloads': len(payloads)},
        'targeted_selection_scan': targeted, 'selection_gitleaks_scan': selected_scan,
        'files': [{'path': n, 'bytes': len(v['bytes']), 'sha256': sha(v['bytes']), 'origin': v['origin']}
                  for n, v in sorted(payloads.items())],
    }
    final = {n: v['bytes'] for n, v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode()
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    check(not OUTPUT.parent.is_symlink(), 'unsafe output directory')
    with tempfile.NamedTemporaryFile(prefix='.eight-square-', suffix='.tmp', dir=OUTPUT.parent, delete=False) as temp:
        temporary = Path(temp.name)
    try:
        with zipfile.ZipFile(temporary, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
            for name, blob in sorted(final.items()):
                info = zipfile.ZipInfo(name, (2026, 9, 7, 0, 0, 0))
                info.create_system = 3
                info.external_attr = (stat.S_IFREG | 0o644) << 16
                info.compress_type = zipfile.ZIP_DEFLATED
                archive.writestr(info, blob)
        decoded = helper.verify_final_archive(temporary, final)
        members = {n: {'bytes': b} for n, b in decoded.items()}
        targeted_final = helper.targeted_content_scan(members, 'decoded final ZIP')
        final_scan = helper.gitleaks_scan(members, 'all decoded final ZIP members')
        check(git('rev-parse', 'HEAD').decode().strip() == head, 'HEAD changed during packaging')
        check(git('status', '--porcelain=v1', '--untracked-files=all') == b'', 'worktree changed during packaging')
        raw = temporary.read_bytes()
        os.link(temporary, OUTPUT)  # Exclusive publication; never overwrite.
    finally:
        temporary.unlink(missing_ok=True)  # Only this generated temporary artifact.
    check(OUTPUT.read_bytes() == raw, 'published bytes changed')
    print(json.dumps({'archive_absolute_path': str(OUTPUT), 'archive_bytes': len(raw),
        'archive_sha256': sha(raw), 'manifest_sha256': sha(final['MANIFEST.json']),
        'source_commit': SOURCE, 'packaging_documentation_head': head, 'branch': BRANCH,
        'source_tree_status': 'clean', 'regular_members': len(final), 'payload_members': len(payloads),
        'included_paths': sorted(final), 'crc_and_manifest_and_byte_equality': 'PASS',
        'targeted_selection_scan': targeted, 'selection_gitleaks_scan': selected_scan,
        'targeted_final_scan': targeted_final, 'decoded_final_gitleaks_scan': final_scan}, indent=2, sort_keys=True))


if __name__ == '__main__':
    main()
