#!/usr/bin/env python3
"""Publish a corrected reading copy; retain the author's return unchanged."""
from datetime import datetime, timezone
import hashlib
import importlib.util
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
BASE = ROOT / 'docs/parameter-atlas'
RAW = BASE / 'pro'
DEST = BASE / 'reference'
HERE = ROOT / 'coordination/parameter-atlas-20260908'
IDENTIFIER_COUNTS = {
    'OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md': 1,
    'RANDOMNESS_PATHS.md': 2,
    'PARAMETERS.json': 7,
    'SOURCE_REFERENCES.json': 1,
    'SOURCE_COVERAGE.tsv': 1,
}
ROOT_NOTE = '> 根端勘误阅读版：精确函数名已修正；NTT 缓存结论限于表示/生命周期风险，不能据此认定已有算术失败。请与[根端复核说明](../REVIEW_NOTES.zh-CN.md)合读。`checks/` 保留原作者原稿自检记录，发布版复核另见 coordination 台账。\n\n'
NTT_CLARIFICATION = '\n\n根端复核补充：如果所有对象始终一致地使用缓存的同一有效根 A，正逆变换和环运算仍可能内部自洽，即使 metadata 写 B。计算错误还需要不一致的 EV 坐标、缓存时期或独立消费者等具体条件，不能由请求 root 不一致单独推出。详见[根端复核说明](../REVIEW_NOTES.zh-CN.md)。\n'

def digest(blob):
    return hashlib.sha256(blob).hexdigest()

def require(value, message):
    if not value:
        raise RuntimeError(message)

def main():
    require(not DEST.exists(), 'exclusive publication already exists')
    raw_manifest = (RAW / 'MANIFEST.json').read_bytes()
    require(digest(raw_manifest) == '33b38355bc7ffbb2a5bd7d5171392f69025ff62cd68832dfdea3c6d69337945a', 'raw manifest drift')
    payloads = {}
    changes = []
    for row in json.loads(raw_manifest)['files']:
        name = row['path']
        data = (RAW / name).read_bytes()
        require(len(data) == row['bytes'] and digest(data) == row['sha256'], 'raw member drift: ' + name)
        if name in IDENTIFIER_COUNTS:
            require(data.count(b'CreatePaperH128ClientKeyPair') == IDENTIFIER_COUNTS[name], 'identifier count drift')
            data = data.replace(b'CreatePaperH128ClientKeyPair', b'CreateFixedQH128ClientKeyPair')
        if name.endswith('.md') and '/' not in name:
            first, rest = data.decode().split('\n', 1)
            content = first + '\n\n' + ROOT_NOTE + rest.lstrip('\n')
            if name == 'FINDINGS.md':
                marker = '\n## F03 —'
                require(content.count(marker) == 1, 'F02 boundary')
                content = content.replace(marker, NTT_CLARIFICATION + marker)
            data = content.encode()
        if digest(data) != row['sha256']:
            changes.append({'path': name, 'author_sha256': row['sha256'], 'published_sha256': digest(data)})
        payloads[name] = {'bytes': data}
    helper_path = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    require(digest(helper_path.read_bytes()) == 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814', 'helper drift')
    spec = importlib.util.spec_from_file_location('pinned_scanner', helper_path)
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    scans = [helper.targeted_content_scan(payloads, 'published-reference'), helper.gitleaks_scan(payloads, 'published-reference')]
    for name, item in payloads.items():
        target = DEST / name
        target.parent.mkdir(parents=True, exist_ok=True)
        with target.open('xb') as handle:
            handle.write(item['bytes'])
    result_path = HERE / 'root-replay/published_document_consistency.json'
    require(not result_path.exists(), 'published check output exists')
    command = [sys.executable, '-B', '-I', str(DEST / 'scripts/check_document_consistency.py'), str(ROOT / 'artifacts/returns/parameter-atlas-20260908/verified-input'), str(DEST), str(result_path)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, check=False)
    require(result.returncode == 0, 'published consistency failure: ' + result.stdout.decode())
    require(json.loads(result_path.read_bytes())['passed'], 'published check did not pass')
    manifest = {
        'schema': 'root-reviewed-parameter-atlas-publication-v1',
        'utc': datetime.now(timezone.utc).isoformat(),
        'author_return_zip_sha256': '623abe4affa88d2e53ba67b77a0648d477177c7df668508fdb66edbcb6c9d535',
        'author_manifest_sha256': digest(raw_manifest),
        'author_raw_preserved_at': '../pro/',
        'root_review': '../REVIEW_NOTES.zh-CN.md',
        'source_commit': 'a4b815a733efe81897325e2a8e4c826a4ebfa439',
        'official_commit': 'df495ba2e91739a6dc8f1de254fc5a41155ce504',
        'manifest_self_excluded': True, 'changes': changes,
        'files': [{'path': name, 'bytes': len(item['bytes']), 'sha256': digest(item['bytes'])} for name, item in sorted(payloads.items())],
        'publication_check_command': command, 'publication_check_exit_code': result.returncode,
        'scans': scans,
        'scope': 'Documentation corrections only; original checks are retained as author evidence, not silently relabeled as checks of changed documents. No production/test/CI changes or FHE execution.',
    }
    with (DEST / 'MANIFEST.json').open('x') as handle:
        json.dump(manifest, handle, indent=2, ensure_ascii=False)
        handle.write('\n')
    print(json.dumps({'destination': str(DEST), 'payloads': len(payloads), 'changed_files': len(changes), 'published_static_check_exit': result.returncode, 'secret_findings': 0}, indent=2))

if __name__ == '__main__':
    main()
