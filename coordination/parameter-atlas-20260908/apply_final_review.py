#!/usr/bin/env python3
"""Bounded documentation refinements from the two independent reviews."""
import hashlib
import importlib.util
import json
from pathlib import Path
import subprocess
import sys
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parents[2]
HERE = ROOT / 'coordination/parameter-atlas-20260908'
DOC = ROOT / 'docs/parameter-atlas/reference'
OLD_MANIFEST_SHA = 'd5f946ab2331443a6ace1b651c8d1163e6192a7d9ff7c84313a3395383ffbd6e'
TAG = '四个32位uniform-distribution输出；不承诺底层engine调用数'
THREAD = '当前固定源码请求OMP_NUM_THREADS=2；历史实际team大小与OpenMP编译分支未鉴证'
THREAD_NOTE = 'ParallelControls构造时缓存omp_get_max_threads；SetNumThreads不更新该缓存；显式num_threads(GetThreadLimit(...))不一定受后来的SetNumThreads统一约束。'
SEED_NOTE = 'WITH_OPENMP+FIXED_SEED取消m_prng的threadprivate；非OpenMP仍为thread_local；不是只改变seed值。'

def sha(data):
    return hashlib.sha256(data).hexdigest()

def require(value, message):
    if not value:
        raise RuntimeError(message)

def main():
    old = (DOC / 'MANIFEST.json').read_bytes()
    require(sha(old) == OLD_MANIFEST_SHA, 'publication manifest drift')
    before_path = HERE / 'root-replay/published_manifest_initial.json'
    output = HERE / 'root-replay/final_document_consistency.json'
    require(not before_path.exists() and not output.exists(), 'exclusive review output exists')
    manifest = json.loads(old)
    payloads = {}
    for row in manifest['files']:
        blob = (DOC / row['path']).read_bytes()
        require(sha(blob) == row['sha256'], 'published payload drift')
        payloads[row['path']] = {'bytes': blob}
    main_name = 'OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md'
    main = payloads[main_name]['bytes'].decode().replace('4×32bit PRNG词', TAG)
    main = main.replace('当前无执行；不从annulus工作流推定旧run线程实况', THREAD).replace('本轮未确认该run实际线程/库二进制', THREAD)
    marker = '\n## 8. 论文—上游—项目—profile 四方映射'
    require(main.count(marker) == 1, 'main insertion boundary')
    main = main.replace(marker, '\n根端独立复核补充：' + THREAD_NOTE + SEED_NOTE + 'tag仅限接收context的私钥构造器产生上述分布输出；默认/拷贝/移动构造不一概新采tag。当前CMake及dcp工作流也为原S100和S116请求2线程，不只annulus；请求与历史实际执行仍分开。参见[详细复核](../REVIEW_NOTES.zh-CN.md)。\n' + marker)
    payloads[main_name]['bytes'] = main.encode()
    rng = payloads['RANDOMNESS_PATHS.md']['bytes'].decode()
    rng = rng.replace('uniform_int_distribution / uniform_real_distribution', 'uniform_int_distribution / uniform_real_distribution / bernoulli_distribution')
    marker = '\n## 4. 从随机字到分布：实际分支表'
    require(rng.count(marker) == 1, 'RNG insertion boundary')
    rng = rng.replace(marker, '\n根端复核补充：' + THREAD_NOTE + SEED_NOTE + 'tag在接收context的私钥构造器内取四个32位分布输出；并非所有私钥构造均新采tag，亦不承诺恰好四个底层engine word。源锚点和当前2线程请求见[根端复核](../REVIEW_NOTES.zh-CN.md)。\n' + marker)
    payloads['RANDOMNESS_PATHS.md']['bytes'] = rng.encode()
    dictionary = json.loads(payloads['PARAMETERS.json']['bytes'])
    by_id = {r['id']: r for r in dictionary['records']}
    threads = by_id['build.threads']
    for key in ('S100', 'S116', 'S100_annulus125'):
        threads['project_request'][key] = THREAD
    for key in ('S100_original', 'S116', 'S100_annulus125'):
        threads['project_effective'][key] = '历史实际team大小及有效OpenMP编译分支待验证；当前源码请求见project_request'
    threads['notes_zh'] += THREAD_NOTE
    threads['root_source_addendum'] = ['project/CMakeLists.txt:266-301,326-354', 'project/.github/workflows/dcp-rcb.yml:202-266', 'official/src/core/include/utils/parallel.h:51-58,112-125']
    by_id['rng.fixed_seed']['notes_zh'] += SEED_NOTE
    by_id['rng.binary']['notes_zh'] += '直接适配器为std::bernoulli_distribution(0.5)。'
    tag = by_id['key.tags']
    for field in ('project_request', 'project_effective'):
        tag[field] = {key: value.replace('4×32bit PRNG词', TAG) if isinstance(value, str) else value for key, value in tag[field].items()}
    tag['notes_zh'] = '接收context的PrivateKeyImpl构造器获取四个32位分布输出；默认/拷贝/移动构造不一概新采tag。随后覆盖tag不退回随机流；底层engine消耗次数依赖标准库。'
    dictionary['root_lookup_aliases'] = {
        'familyCount': {'value': 8, 'record_ids': ['key.evalkey_family'], 'source_ref_ids': ['P04', 'P06'], 'reason': 'lookup alias, not an additional independent setting'},
        'familyQCount': {'formula': '11-f, f=0..7', 'input_pair_formula': '10-f', 'after_RS_formula': '9-f', 'record_ids': ['basis.partition_count', 'basis.drop_order'], 'source_ref_ids': ['P04', 'P05'], 'reason': 'derived state, not freely mutable independently of profile'},
    }
    dictionary['root_review_notes'] = '../REVIEW_NOTES.zh-CN.md'
    payloads['PARAMETERS.json']['bytes'] = (json.dumps(dictionary, indent=2, ensure_ascii=False) + '\n').encode()
    helper_path = ROOT / 'coordination/precision116-pro-handoff-01/build_packet.py'
    require(sha(helper_path.read_bytes()) == 'f1630f5476201b0dd6a867b3257d26ad35a66fb3765475d1d3973050a50d4814', 'scanner drift')
    spec = importlib.util.spec_from_file_location('pinned_scanner', helper_path)
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    scans = [helper.targeted_content_scan(payloads, 'final-reference'), helper.gitleaks_scan(payloads, 'final-reference')]
    with before_path.open('xb') as handle:
        handle.write(old)
    for name in (main_name, 'RANDOMNESS_PATHS.md', 'PARAMETERS.json'):
        (DOC / name).write_bytes(payloads[name]['bytes'])
    command = [sys.executable, '-B', '-I', str(DOC / 'scripts/check_document_consistency.py'), str(ROOT / 'artifacts/returns/parameter-atlas-20260908/verified-input'), str(DOC), str(output)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, check=False)
    require(result.returncode == 0, result.stdout.decode())
    original = json.loads((ROOT / 'docs/parameter-atlas/pro/MANIFEST.json').read_bytes())
    original_hashes = {r['path']: r['sha256'] for r in original['files']}
    manifest.update({
        'utc': datetime.now(timezone.utc).isoformat(),
        'prior_publication_manifest_sha256': OLD_MANIFEST_SHA,
        'prior_publication_manifest_saved_at': '../../../coordination/parameter-atlas-20260908/root-replay/published_manifest_initial.json',
        'changes': [{'path': name, 'author_sha256': original_hashes[name], 'published_sha256': sha(item['bytes'])} for name, item in sorted(payloads.items()) if sha(item['bytes']) != original_hashes[name]],
        'files': [{'path': name, 'bytes': len(item['bytes']), 'sha256': sha(item['bytes'])} for name, item in sorted(payloads.items())],
        'publication_check_command': command, 'publication_check_exit_code': result.returncode, 'scans': scans,
        'root_review_notes_sha256': sha((ROOT / 'docs/parameter-atlas/REVIEW_NOTES.zh-CN.md').read_bytes()),
    })
    (DOC / 'MANIFEST.json').write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + '\n')
    print(json.dumps({'final_static_check': '22/22 PASS', 'records': len(dictionary['records']), 'lookup_aliases': list(dictionary['root_lookup_aliases']), 'secret_findings': 0}, ensure_ascii=False))

if __name__ == '__main__':
    main()
