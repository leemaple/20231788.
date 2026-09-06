"""Fixed-source, bounded, secret-scanned scientific review packet generation."""
import importlib.util
import io
import json
import os
from pathlib import Path
import stat
import subprocess
import zipfile
import zlib

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CHECKPOINT = '1c7609593a9d51d6f27918b0bebe54d8f5eaeee6'
SOURCE = 'ed5fd192a89d6d4728ad295e87cf06a3f4abc832'
OUTPUT = ROOT / 'artifacts/handoffs/fs-endpoint-scientific-review-01/fs-endpoint-scientific-review-ed5fd192.zip'
spec = importlib.util.spec_from_file_location('verified_packet_helpers', ROOT / 'coordination/fs-residual-endpoint-green-01/build_packet.py')
helpers = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helpers)


def require(condition, message):
    if not condition:
        raise ValueError(message)


def run(*args):
    return subprocess.check_output(args)


def git(*args):
    return run('git', '-C', str(ROOT), *args)


def add_git(payloads, commit, path):
    raw = git('show', commit + ':' + path)
    helpers.add(payloads, 'project/' + path, raw, dict(kind='cleanroom_git', commit=commit, path=path))


def scan_with_gzip(payloads, phase):
    expanded = dict(payloads)
    for path, item in payloads.items():
        if path.endswith('.tsv.gz'):
            decoder = zlib.decompressobj(31)
            data = decoder.decompress(item['bytes'], 16777217)
            require(len(data) <= 16777216 and decoder.eof and not decoder.unused_data and not decoder.unconsumed_tail,
                    'bounded single-member gzip for scan')
            expanded[path + '.decoded-for-scan'] = dict(bytes=data, origin='virtual decoded payload; not archive member')
    return helpers.scan(expanded, phase)


def main():
    require(git('branch', '--show-current').decode().strip() == 'codex/paper-scale-implementation-20260905', 'branch')
    require(git('status', '--porcelain=v1') == b'', 'clean worktree required')
    head = git('rev-parse', 'HEAD').decode().strip()
    require(not OUTPUT.exists(), 'refuse overwrite')
    require(git('diff', '--name-only', SOURCE, head, '--', 'src', 'include', 'tests', 'CMakeLists.txt', '.github/workflows/dcp-rcb.yml') == b'', 'exact tested engineering source')
    require(git('diff', '--name-only', CHECKPOINT, head, '--', 'coordination/fs-endpoint-live-run-01') == b'', 'accepted evidence unchanged')
    paths = git('ls-tree', '-r', '--name-only', SOURCE, '--', 'src', 'include', 'tests').decode().splitlines()
    paths += ['CMakeLists.txt', '.github/workflows/dcp-rcb.yml']
    require(len(paths) == 84, 'fixed 84-file source inventory')
    payloads = {}
    helpers.add(payloads, 'TASK.md', (HERE / 'TASK.md').read_bytes(), dict(kind='cleanroom_task', commit=head))
    for path in paths:
        add_git(payloads, SOURCE, path)
    evidence = git('ls-tree', '-r', '--name-only', CHECKPOINT, '--', 'coordination/fs-endpoint-live-run-01').decode().splitlines()
    require(len(evidence) == 14, 'fixed accepted evidence set')
    for path in evidence:
        add_git(payloads, CHECKPOINT, path)
    context = helpers.REQUIREMENTS + helpers.SCIENTIFIC_DISPOSITION + [
        'coordination/fs-residual-endpoint-red-return-01/pro/ENDPOINT_SPEC.md',
        'coordination/fs-residual-endpoint-red-return-01/pro/TEST_PLAN.md',
        'coordination/fs-endpoint-wrapper-01/INTEGRATION_CONTRACT.md',
        'coordination/fs-endpoint-cpp-interop-hosted-01/GREEN_ACCEPTANCE.md',
        'coordination/fs-endpoint-live-integration-01/TDD_BOUNDARY.md',
        'coordination/fs-endpoint-live-integration-01/ROOT_INTAKE.json']
    for path in context:
        add_git(payloads, CHECKPOINT, path)
    prior_bytes = helpers.PRIOR_PACKET.read_bytes()
    require(len(prior_bytes) == helpers.PRIOR_PACKET_BYTES and helpers.sha256(prior_bytes) == helpers.PRIOR_PACKET_SHA256, 'pinned cleanroom reference packet')
    prior, rows = helpers.decode_manifest_archive(prior_bytes)
    official = sorted(path for path in rows if path.startswith('official-full/'))
    require(len(official) == 77, '77 official reference files')
    require(helpers.git(helpers.OFFICIAL_REPO, 'rev-parse', 'HEAD').decode().strip() == helpers.OFFICIAL_PIN, 'pristine official pin')
    require(helpers.git(helpers.OFFICIAL_REPO, 'status', '--porcelain=v1') == b'', 'no local OpenFHE modifications')
    for path in official:
        require(prior[path] == helpers.git_show(helpers.OFFICIAL_REPO, helpers.OFFICIAL_PIN, path.removeprefix('official-full/')), 'official exact Git bytes')
    selected_prior = official + helpers.BOOST_PATHS + helpers.PAPER_PATHS + helpers.HISTORICAL_SIGNED_EVIDENCE
    for path in helpers.PAPER_PATHS:
        cfg = helpers.PAPER_ORIGINALS[path]
        b = cfg['path'].read_bytes()
        require(len(b) == cfg['bytes'] and helpers.sha256(b) == cfg['sha256'] and prior[path] == b, 'user original paper binding')
    for path in selected_prior:
        helpers.add(payloads, 'references/' + path, prior[path], dict(kind='verified_prior_cleanroom_reference', prior_packet_sha256=helpers.PRIOR_PACKET_SHA256, prior_manifest_row=rows[path]))
    helpers.validate_name_set(list(payloads), credential_policy=True)
    selected_scan = scan_with_gzip(payloads, 'selected source/context/evidence plus both decoded canonical TSVs')
    manifest = dict(schema='fs-endpoint-scientific-review-input-v1', manifest_self_excluded=True,
                    documentation_head=head, tested_source=SOURCE, accepted_evidence_checkpoint=CHECKPOINT,
                    source_tree_status='clean', official_pin=helpers.OFFICIAL_PIN,
                    no_prior_project_source_reused=True, selection_scan=selected_scan,
                    files=[dict(path=path, bytes=len(item['bytes']), sha256=helpers.sha256(item['bytes']), origin=item['origin'])
                           for path, item in sorted(payloads.items())])
    final = {path: item['bytes'] for path, item in payloads.items()}
    final['MANIFEST.json'] = (json.dumps(manifest, indent=2, sort_keys=True) + '\n').encode()
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
        for path, data in sorted(final.items()):
            info = zipfile.ZipInfo(path, date_time=(2026, 9, 6, 0, 0, 0))
            info.create_system = 3
            info.external_attr = (stat.S_IFREG | 0o644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, data)
    archive_bytes = buffer.getvalue()
    decoded = helpers.verify_decoded_final(io.BytesIO(archive_bytes), final)
    final_scan = scan_with_gzip({path: dict(bytes=b) for path, b in decoded.items()}, 'decoded final archive and canonical TSVs')
    require(git('rev-parse', 'HEAD').decode().strip() == head and git('status', '--porcelain=v1') == b'', 'unchanged source during packaging')
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    with OUTPUT.open('xb') as stream:
        require(stream.write(archive_bytes) == len(archive_bytes), 'complete exclusive ZIP write')
    require(OUTPUT.read_bytes() == archive_bytes, 'closed ZIP byte equality')
    print(json.dumps(dict(archive_absolute_path=str(OUTPUT), archive_bytes=len(archive_bytes), archive_sha256=helpers.sha256(archive_bytes),
                          member_count=len(final), manifest_sha256=helpers.sha256(final['MANIFEST.json']),
                          documentation_head=head, tested_source=SOURCE, accepted_evidence_checkpoint=CHECKPOINT,
                          selected_scan=selected_scan, decoded_final_scan=final_scan, included_paths=sorted(final)), indent=2))


if __name__ == '__main__':
    main()
