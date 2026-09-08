"""Read-only scientific-source packet; verified archive generation, no FHE execution."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path, PurePosixPath
import stat
import subprocess
import tarfile
import zipfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
SOURCE = 'a4b815a733efe81897325e2a8e4c826a4ebfa439'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
BRANCH = 'codex/parameter-atlas-20260908'
HELPER = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
HELPER_SHA = 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814'
PRIOR = Path('/Users/lifeng/Documents/20231788-openfhe-s100-annulus125-20260908/artifacts/handoffs/annulus-independent-pro-review-20260908/annulus-independent-03f37b6.zip')


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--official-tar', type=Path, required=True)
    args = p.parse_args()
    assert hashlib.sha256(HELPER.read_bytes()).hexdigest() == HELPER_SHA
    spec = importlib.util.spec_from_file_location('audited_packet_helper', HELPER)
    h = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(h)
    h.ROOT = ROOT
    h.require(h.git('branch', '--show-current').decode().strip() == BRANCH, 'branch drift')
    head = h.git('rev-parse', 'HEAD').decode().strip()
    h.require(h.git('diff', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows') == b'', 'runtime drift')
    h.require(h.git('diff', '--name-only') == b'' and h.git('diff', '--cached', '--name-only') == b'', 'uncommitted tracked edits')
    untracked = set(h.git('ls-files', '--others', '--exclude-standard').decode().splitlines())
    h.require(untracked <= {'coordination/parameter-atlas-20260908/PROJECT_PARAMETER_MAP.md'}, 'unrelated untracked files')
    payloads = {}

    def add_git(commit, path, dest):
        blob, mode, oid = h.git_entry(commit, path)
        if dest.endswith('.log'):
            h.ALLOWED_LOGS.add(dest)
        h.add(payloads, dest, blob, {'kind': 'fixed_cleanroom_git_blob', 'commit': commit, 'path': path, 'git_blob': oid, 'mode': mode})

    add_git(head, 'coordination/parameter-atlas-20260908/TASK.md', 'TASK.md')
    paths = h.git('ls-tree', '-r', '--name-only', SOURCE, '--', 'src', 'include', 'tests', '.github/workflows').decode().splitlines()
    for path in sorted(paths + ['CMakeLists.txt']):
        add_git(SOURCE, path, 'project/' + path)
    context = [
        'coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md',
        'coordination/s100-fresh-error-repair-01/PAPER_EXPERIMENT_PROVENANCE.md',
        'coordination/s100-fresh-error-repair-01/GREEN2_RESULT.md',
        'coordination/fs-endpoint-live-run-01/ACCEPTANCE.md',
        'coordination/precision116-eight-square-return-01/ACCEPTANCE.md',
        'coordination/annulus-independent-pro-review-20260908/RETURN_DISPOSITION.md',
        'coordination/annulus-independent-pro-review-20260908/ROOT_REPLAY_230.json',
        'coordination/annulus-independent-pro-review-20260908/pro/REVIEW.md',
        'coordination/annulus-independent-pro-review-20260908/pro/results/old_s100_230.json',
        'coordination/s100-annulus125-20260908/RESULT.zh-CN.md',
    ]
    for path in context:
        add_git(SOURCE, path, 'context/' + path)
    for path in h.git('ls-tree', '-r', '--name-only', SOURCE, '--', 'coordination/s100-annulus125-20260908').decode().splitlines():
        if path.endswith('.py'):
            add_git(SOURCE, path, 'project/' + path)
    add_git(SOURCE, 'coordination/comprehensive-reassessment-20260908/check_receiver_agreement.py', 'project/coordination/comprehensive-reassessment-20260908/check_receiver_agreement.py')
    prior, prior_rows = h.decode_manifest_archive(PRIOR, 13743164,
        'c30b6e84a53712792cfda4ec17da7b434ee745aebad3949916a90fa6d528a0b1',
        '6cbd05178b788ead26ea12c1032c6fa8f22cad77e65488886a8d1b8f74fbbcdc',
        310, 'manifest_self_excluded', True)
    for name in sorted(prior_rows):
        if name.startswith(('references/paper/', 'references/boost-1.83.0/')):
            h.add(payloads, name, prior[name], {'kind': 'verified_supplied_paper_or_boost_reference', 'prior_manifest_row': prior_rows[name]})
    h.require(h.sha256(payloads['references/paper/PAPER-2023-1788.pdf']['bytes']) == '61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac', 'paper hash')

    # Public GitHub source metadata only. No local pre-existing OpenFHE checkout.
    command = ['/opt/homebrew/bin/gh', 'api', f'repos/openfheorg/openfhe-development/git/trees/{PIN}?recursive=1']
    tree = json.loads(subprocess.check_output(command, timeout=60))
    h.require(tree.get('truncated') is False, 'truncated upstream tree')
    official_index = {r['path']: r for r in tree['tree'] if r['type'] == 'blob'}
    tar_bytes = args.official_tar.read_bytes()
    h.require(0 < len(tar_bytes) < 64 * 1024 * 1024, 'archive size bound')
    selected = {}
    with tarfile.open(args.official_tar, 'r:gz') as tf:
        infos = tf.getmembers()
        names = [i.name for i in infos]
        h.require(len(names) == len(set(names)), 'duplicate tar names')
        for info in infos:
            path = PurePosixPath(info.name)
            h.require(not path.is_absolute() and '..' not in path.parts and '\\' not in info.name, 'unsafe official tar path')
            if len(path.parts) < 2:
                continue
            rel = '/'.join(path.parts[1:])
            include = rel.startswith(('src/core/include/', 'src/core/lib/', 'src/pke/include/', 'src/pke/lib/'))
            include |= rel in {'LICENSE', 'CMakeLists.txt', 'CMakeLists.User.txt', '.gitmodules', 'src/core/CMakeLists.txt', 'src/pke/CMakeLists.txt'}
            include |= rel.startswith(('configure/', 'cmake/')) and info.isfile()
            include |= 'blake' in rel.lower() and rel.endswith(('.c', '.h', '.cpp', '.hpp'))
            if not include or info.isdir():
                continue
            h.require(info.isfile() and 0 <= info.size <= 2 * 1024 * 1024, 'nonregular/oversized selected source')
            blob = tf.extractfile(info).read()
            row = official_index[rel]
            oid = hashlib.sha1(b'blob ' + str(len(blob)).encode() + b'\0' + blob).hexdigest()
            h.require(row['mode'] in {'100644', '100755'} and row['sha'] == oid and row['size'] == len(blob), 'upstream git blob mismatch: ' + rel)
            h.require(rel not in selected, 'duplicate stripped source')
            selected[rel] = blob
            h.add(payloads, 'official/' + rel, blob, {'kind': 'fresh_official_commit_archive_git_blob_verified', 'commit': PIN, 'git_blob': oid, 'path': rel})
    h.require(len(selected) > 200 and sum(map(len, selected.values())) < 32 * 1024 * 1024, 'unexpected selected source closure')
    source_receipt = {'official_commit': PIN, 'tree_sha': tree['sha'], 'tree_command': command,
        'download_url': f'https://api.github.com/repos/openfheorg/openfhe-development/tarball/{PIN}',
        'archive_bytes': len(tar_bytes), 'archive_sha256': h.sha256(tar_bytes), 'selected_files': len(selected),
        'selected_bytes': sum(map(len, selected.values())), 'every_selected_git_blob_verified': True,
        'local_modified_openfhe_used': False, 'compiled_or_executed': False}
    h.add(payloads, 'OFFICIAL_SOURCE_RECEIPT.json', (json.dumps(source_receipt, indent=2) + '\n').encode(), {'kind': 'root_source_acquisition_receipt'})
    h.validate_names(list(payloads))
    before_targeted = h.targeted_content_scan(payloads, 'selected atlas sources')
    before_scan = h.gitleaks_scan(payloads, 'selected atlas sources')
    manifest = {'schema': 'parameter-atlas-input-v1', 'manifest_self_excluded': True,
        'source_commit': SOURCE, 'task_commit': head, 'branch': BRANCH, 'official_pin': PIN,
        'source_status': 'tracked index/worktree clean; independent untracked map excluded',
        'excluded': ['old implementation', 'local modified OpenFHE', '.git', 'builds', 'installed dependencies', 'state/caches/databases', 'credentials', 'independent map', 'full-slot capture duplicates'],
        'targeted_selection': before_targeted, 'gitleaks_selection': before_scan,
        'files': [{'path': n, 'bytes': len(v['bytes']), 'sha256': h.sha256(v['bytes']), 'origin': v['origin']} for n, v in sorted(payloads.items())]}
    final = {n: v['bytes'] for n, v in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode()
    out = ROOT / 'artifacts/handoffs/parameter-atlas-20260908/openfhe-parameter-atlas-a4b815a.zip'
    out.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as z:
        for name, blob in sorted(final.items()):
            info = zipfile.ZipInfo(name, (2026, 9, 8, 0, 0, 0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(info, blob)
    decoded = h.verify_final_archive(out, final)
    check = {n: {'bytes': b} for n, b in decoded.items()}
    final_targeted = h.targeted_content_scan(check, 'decoded final atlas ZIP')
    final_scan = h.gitleaks_scan(check, 'decoded final atlas ZIP')
    h.require(h.git('rev-parse', 'HEAD').decode().strip() == head, 'HEAD moved')
    h.require(h.git('diff', '--name-only') == b'' and h.git('diff', '--cached', '--name-only') == b'', 'tracked edits during packaging')
    # A source-only mirror for root's later read-only review, not a build tree.
    mirror = ROOT / 'artifacts/reference-sources/parameter-atlas-openfhe-df495ba2'
    h.require(not mirror.exists(), 'exclusive source mirror already exists')
    for rel, blob in selected.items():
        dest = mirror / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        with dest.open('xb') as f:
            f.write(blob)
    receipt = {'archive_path': str(out), 'bytes': out.stat().st_size, 'sha256': h.sha256(out.read_bytes()),
        'members': len(final), 'manifest_sha256': h.sha256(final['MANIFEST.json']), 'included_paths': sorted(final),
        'source_commit': SOURCE, 'task_commit': head, 'official_source': source_receipt,
        'targeted_selection': before_targeted, 'gitleaks_selection': before_scan,
        'final_targeted': final_targeted, 'final_gitleaks': final_scan, 'crc_manifest_source_hash_closure': 'PASS'}
    with (HERE / 'PACKET_RECEIPT.json').open('x') as f:
        json.dump(receipt, f, indent=2)
        f.write('\n')
    print(json.dumps({k: receipt[k] for k in ['archive_path', 'bytes', 'sha256', 'members', 'source_commit', 'task_commit', 'crc_manifest_source_hash_closure']}, indent=2))


if __name__ == '__main__':
    main()
