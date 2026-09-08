"""Prepare an exact-source semantic review packet; no numerical execution."""
import hashlib
import importlib.util
import json
from pathlib import Path
import stat
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = '31e24bec1eb2db5d13de3b442a9e909e26db7206'
BRANCH = 'codex/public-encoder-cap-20260909'
PRIOR = Path('/Users/lifeng/Documents/20231788-openfhe-initial-lift-nonwrap-20260909/artifacts/handoffs/initial-lift-nonwrap-20260909/initial-lift-nonwrap-a4b815a.zip')
PRIOR_SHA = '0cde0f999e13445e2faec46a4bf34dbcf6e47131c53afcf9e57375051eed7bed'
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
    h.require(h.git('status', '--porcelain').strip() == b'', 'dirty handoff source')
    changed = h.git('diff', '--name-only', SOURCE, head).decode().splitlines()
    h.require(all(n.startswith('coordination/reproduction-adjudication-20260909/') for n in changed), 'non-handoff source drift')
    prior, rows = h.decode_manifest_archive(PRIOR, 3626371, PRIOR_SHA,
        '96a45b3de562d23d30090e2048cb66d2c0412318a691bc204988fad736e305a7',
        537, 'manifest_self_excluded', True)
    payloads = {}
    for name in sorted(rows):
        # Replace all active project code and the live atlas with exact current Git.
        if name == 'TASK.md' or name.startswith(('project/', 'docs/parameter-atlas/')):
            continue
        if name.endswith('.log'):
            h.ALLOWED_LOGS.add(name)
        h.add(payloads, name, prior[name], {'kind': 'verified_cleanroom_prior_reference', 'archive_sha256': PRIOR_SHA, 'row': rows[name]})
    h.add(payloads, 'provenance/INITIAL_LIFT_INPUT_MANIFEST.json', prior['MANIFEST.json'],
          {'kind': 'historical_source_manifest_not_current_project_identity', 'archive_sha256': PRIOR_SHA})

    def add_git(commit, path, dest=None):
        blob, mode, oid = h.git_entry(commit, path)
        h.add(payloads, dest or path, blob, {'kind': 'fixed_cleanroom_git_blob', 'commit': commit,
              'path': path, 'git_blob': oid, 'mode': mode})

    def tree(commit, *prefixes):
        return h.git('ls-tree', '-r', '--name-only', commit, '--', *prefixes).decode().splitlines()

    for path in tree(SOURCE, 'src', 'include', 'tests', 'diagnostics', '.github/workflows') + ['CMakeLists.txt']:
        add_git(SOURCE, path, 'project/' + path)
    for name in sorted(rows):
        if name.startswith('project/coordination/'):
            add_git(SOURCE, name.removeprefix('project/'), name)
    for path in tree(SOURCE, 'docs/parameter-atlas'):
        add_git(SOURCE, path)
    # Preserve full returned proof/check bytes and the current public cap evidence.
    for path in tree(SOURCE, 'coordination/initial-lift-nonwrap-20260909/pro',
                     'coordination/initial-lift-nonwrap-20260909/root-replay',
                     'coordination/public-s100-encoder-cap-20260909'):
        # Historical scanner regex text triggers the strict token-prefix gate.
        # Omit that unnecessary report; never weaken the scan or alter evidence.
        if path == 'coordination/initial-lift-nonwrap-20260909/pro/evidence/RETURN_TARGETED_SCAN.json':
            continue
        add_git(SOURCE, path)
    for name in ('ADOPTED_CONTRACT.md', 'ROOT_RETURN_REVIEW.md', 'PRO_RETURN_MATH_REVIEW.md',
                 'CANONICAL_REVIEW_DISPOSITION.md', 'PUBLIC_ENCODER_CANDIDATE_REVIEW.md', 'RETURN_INTAKE.json'):
        add_git(SOURCE, 'coordination/initial-lift-nonwrap-20260909/' + name)
    extra = [
        'coordination/paper-scale-precision-return-01/ACCEPTANCE.md',
        'coordination/paper-scale-precision-return-01/PRO_DIAGNOSIS.md',
        'coordination/s100-fresh-error-repair-01/FRESH_IDEAL_PROPAGATION.md',
        'coordination/s100-fresh-error-repair-01/PAPER_EXPERIMENT_PROVENANCE.md',
        'coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md',
        'coordination/parameter-atlas-20260908/PROJECT_PARAMETER_MAP.md',
        'coordination/parameter-atlas-20260908/ROOT_RANDOMNESS_MAP.md',
        'coordination/parameter-atlas-20260908/ROOT_PARAMETER_DICTIONARY.tsv',
        'coordination/parameter-atlas-20260908/ROOT_COVERAGE_CHECKLIST.md',
        'coordination/parameter-atlas-20260908/ROOT_PAPER_CROSSCHECK.md',
        'coordination/fs-endpoint-live-run-01/LINUX_AUDIT.json',
        'coordination/fs-endpoint-live-run-01/WINDOWS_AUDIT.json',
        'coordination/fs-endpoint-live-run-01/AUDIT_REVIEW.md',
    ]
    for path in extra:
        add_git(SOURCE, path, 'context/current/' + path)
    for name in ('TASK.md', 'TASK_PREFLIGHT.md'):
        add_git(head, 'coordination/reproduction-adjudication-20260909/' + name, name)

    h.validate_names(list(payloads))
    selected_targeted = h.targeted_content_scan(payloads, 'selected adjudication context')
    selected_gitleaks = h.gitleaks_scan(payloads, 'selected adjudication context')
    manifest = {'schema': 'reproduction-adjudication-input-v1', 'manifest_self_excluded': True,
        'source_commit': SOURCE, 'task_commit': head, 'branch': BRANCH, 'tracked_state': 'clean',
        'official_pin': 'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'historical_manifests_are_provenance_not_current_path_hashes': True,
        'omitted_historical_scan_report': 'coordination/initial-lift-nonwrap-20260909/pro/evidence/RETURN_TARGETED_SCAN.json; literal regex prefix, not task evidence; original Git bytes unchanged',
        'excluded': ['quarantined implementations', 'modified local OpenFHE', 'build outputs',
                     'runtime/browser state', 'credentials', 'nested archives', 'secret-bearing/full-slot ciphertext captures'],
        'selected_targeted_scan': selected_targeted, 'selected_gitleaks_scan': selected_gitleaks,
        'files': [{'path': n, 'bytes': len(v['bytes']), 'sha256': h.sha256(v['bytes']), 'origin': v['origin']}
                  for n, v in sorted(payloads.items())]}
    final = {n: v['bytes'] for n, v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode()
    out = ROOT / 'artifacts/handoffs/reproduction-adjudication-20260909/reproduction-adjudication-31e24be.zip'
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
    final_targeted = h.targeted_content_scan(content, 'decoded final adjudication archive')
    final_gitleaks = h.gitleaks_scan(content, 'decoded final adjudication archive')
    h.require(h.git('status', '--porcelain').strip() == b'', 'dirty after packaging')
    h.require(h.git('rev-parse', 'HEAD').decode().strip() == head, 'HEAD moved')
    receipt = {'archive_path': str(out), 'bytes': out.stat().st_size, 'sha256': h.sha256(out.read_bytes()),
        'members': len(final), 'manifest_sha256': h.sha256(final['MANIFEST.json']),
        'source_commit': SOURCE, 'task_commit': head, 'included_paths': sorted(final),
        'selected_targeted': selected_targeted, 'selected_gitleaks': selected_gitleaks,
        'final_targeted': final_targeted, 'final_gitleaks': final_gitleaks,
        'crc_paths_regular_manifest_byte_closure': 'PASS'}
    # Generated scan/manifest receipt, not an authored source edit.
    with (HERE / 'PACKET_RECEIPT.json').open('x') as f:
        json.dump(receipt, f, indent=2)
        f.write('\n')
    print(json.dumps({k: receipt[k] for k in ('archive_path', 'bytes', 'sha256', 'members',
          'source_commit', 'task_commit', 'manifest_sha256')}, indent=2))


if __name__ == '__main__':
    main()
