#!/usr/bin/env python3
"""Run bounded documentation RED/GREEN, textual mutations and exact patch checks.
Only invokes this delivery's own check_docs.py in isolated Python. No input
project module is imported or executed. Originals are read only. Temporary
mutants are copies of TWO Markdown files, not algorithm inputs.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile

NAMES = ('CHECK_AND_HANDOFF.zh-CN.md', 'REPRODUCE.zh-CN.md')


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def apply_patch(originals: dict, patch_text: str) -> dict:
    """Strict unified-diff reader. Verifies context and both line counters."""
    lines = patch_text.splitlines(keepends=True)
    i, results = 0, {}
    while i < len(lines):
        if not lines[i].startswith('--- a/'):
            raise ValueError('expected old path header')
        name = lines[i][6:].strip()
        if name not in NAMES or name in results:
            raise ValueError('unexpected or duplicate patch target')
        i += 1
        if i >= len(lines) or lines[i] != '+++ b/' + name + '\n':
            raise ValueError('new path differs from frozen old path')
        i += 1
        old = originals[name].decode('utf-8').splitlines(keepends=True)
        built, cursor = [], 0
        while i < len(lines) and lines[i].startswith('@@ '):
            m = re.fullmatch(r'@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@[^\n]*\n', lines[i])
            if not m:
                raise ValueError('invalid hunk header')
            a, ac, b, bc = (int(m[1]), int(m[2] or 1), int(m[3]), int(m[4] or 1))
            if a < 1 or b < 1 or a - 1 < cursor:
                raise ValueError('unsupported or overlapping hunk')
            built.extend(old[cursor:a-1]); cursor = a-1
            if len(built) != b-1:
                raise ValueError('new hunk position mismatch')
            i += 1
            old_count, new_count = 0, 0
            while i < len(lines) and not lines[i].startswith(('@@ ', '--- a/')):
                line = lines[i]
                if not line or line[0] not in ' +-':
                    raise ValueError('unexpected patch line')
                prefix, content = line[0], line[1:]
                if prefix in ' -':
                    if cursor >= len(old) or old[cursor] != content:
                        raise ValueError('old context mismatch')
                    cursor += 1; old_count += 1
                if prefix in ' +':
                    built.append(content); new_count += 1
                i += 1
            if (old_count, new_count) != (ac, bc):
                raise ValueError('hunk line counts mismatch')
        built.extend(old[cursor:])
        results[name] = ''.join(built).encode('utf-8')
    if set(results) != set(NAMES):
        raise ValueError('patch does not target exactly the two documents')
    return results


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--input-root', type=Path, required=True)
    ap.add_argument('--delivery-root', type=Path, required=True)
    ap.add_argument('--evidence-dir', type=Path, required=True)
    args = ap.parse_args()
    delivery = args.delivery_root.resolve(strict=True)
    checker = delivery / 'checks/check_docs.py'
    candidates = delivery / 'docs/full_files'
    evidence = args.evidence_dir
    evidence.mkdir(parents=True, exist_ok=True)
    for reserved in ['doc_red.stdout', 'doc_red.stderr', 'doc_green.stdout',
                     'doc_green.stderr', 'DOC_CHECK_RECEIPT.json']:
        if (evidence / reserved).exists():
            raise ValueError('refusing to overwrite existing evidence: ' + reserved)
    old = {name: (args.input_root / name).read_bytes() for name in NAMES}
    new = {name: (candidates / name).read_bytes() for name in NAMES}
    bindings = json.loads((delivery / 'docs/DOCUMENT_PATCH_BINDINGS.json').read_text())['files']
    for row in bindings:
        name = row['path']
        if (len(old[name]), sha(old[name]), len(new[name]), sha(new[name])) != (
            row['before_bytes'], row['before_sha256'], row['after_bytes'], row['after_sha256']):
            raise ValueError('frozen document byte identity mismatch')
    if len(bindings) != 2 or {r['path'] for r in bindings} != set(NAMES):
        raise ValueError('unexpected document binding set')
    patched = apply_patch(old, (delivery / 'docs/01-completion-labels.patch').read_text())
    if patched != new:
        raise ValueError('patch result differs from complete candidate')
    old_tail = old[NAMES[0]].decode().split('\n', 1)[1].replace(
        '## 最新增补：请先看这里', '## 2026-09-08 历史增补（其中“最新”仅指当时）', 1).lstrip()
    if not new[NAMES[0]].decode().endswith(old_tail):
        raise ValueError('old handoff historical body was changed')

    def run(root: Path) -> dict:
        cmd = [sys.executable, '-B', '-I', str(checker), '--root', str(root)]
        p = subprocess.run(cmd, capture_output=True, check=False, timeout=15)
        return {'exit_code': p.returncode, 'stdout': p.stdout.decode('utf-8'),
                'stderr': p.stderr.decode('utf-8')}
    primary = {}
    for label, root, expected in [('red', args.input_root, 1), ('green', candidates, 0)]:
        res = run(root)
        for stream in ('stdout', 'stderr'):
            (evidence / f'doc_{label}.{stream}').write_text(res[stream], encoding='utf-8')
        if res['exit_code'] != expected:
            raise ValueError(f'{label} expected exit {expected}, got {res["exit_code"]}')
        primary[label] = {'observed_exit': res['exit_code'], 'expected_exit': expected,
                          'result': json.loads(res['stdout'])}
    mutations = [
        ('linux_fail_rewritten', NAMES[0], 'ORIGINAL_S100_LINUX_E80: FAIL', 'ORIGINAL_S100_LINUX_E80: PASS'),
        ('windows_fail_rewritten', NAMES[0], 'ORIGINAL_S100_WINDOWS_E80: FAIL', 'ORIGINAL_S100_WINDOWS_E80: PASS'),
        ('s116_scope_removed', NAMES[0], 'S116_LINUX_E80: PASS_SEPARATE_CHANGED_PARAMETERS', 'S116_LINUX_E80: PASS'),
        ('annulus_windows_invented', NAMES[0], 'ANNULUS125_WINDOWS_E80: NOT_RUN', 'ANNULUS125_WINDOWS_E80: PASS'),
        ('current_p_historical_conflation', NAMES[0], 'CURRENT_P_ECD: CERTIFIED_NOT_HISTORICAL_P', 'CURRENT_P_ECD: ALL_HISTORICAL_P_CERTIFIED'),
        ('all_gates_declared_pass', NAMES[0], 'OVERALL_FROZEN_ACCEPTANCE: NOT_ALL_PASSED', 'OVERALL_FROZEN_ACCEPTANCE: ALL_PASSED'),
        ('unconditional_algorithm_claim', NAMES[0], 'ALGORITHM_CORRECTNESS: CONDITIONAL_SUPPORTED', 'ALGORITHM_CORRECTNESS: UNIVERSALLY_PROVEN'),
        ('wrong_source_snapshot', NAMES[0], 'SOURCE_SNAPSHOT: a7f54de2701a1b9bc02660f66febeff707b56651', 'SOURCE_SNAPSHOT: ' + '0' * 40),
        ('duplicate_field', NAMES[0], 'ORIGINAL_S100_LINUX_E80: FAIL', 'ORIGINAL_S100_LINUX_E80: FAIL\nORIGINAL_S100_LINUX_E80: PASS'),
        ('historical_heading_removed', NAMES[0], '## 2026-09-08 历史增补（其中“最新”仅指当时）', '## 最新增补：请先看这里'),
        ('local_s116_success_scope_removed', NAMES[1], '在另行命名的 S116 改参实验中，Linux、Windows 均通过该实验沿用的绝对分量误差上限', 'Linux、Windows 均通过原定绝对分量误差上限'),
        ('missing_reproduce_file', NAMES[1], None, None),
    ]
    mutation_results = []
    with tempfile.TemporaryDirectory(prefix='completion_doc_text_') as tmp:
        folder = Path(tmp)
        for label, name, before, after in mutations:
            for f in NAMES:
                (folder / f).write_bytes(new[f])
            if before is None:
                (folder / name).unlink()
            else:
                text = new[name].decode('utf-8')
                if text.count(before) != 1:
                    raise ValueError('mutation not uniquely located: ' + label)
                (folder / name).write_text(text.replace(before, after, 1), encoding='utf-8')
            res = run(folder)
            if res['exit_code'] != 1:
                raise ValueError('mutation not rejected: ' + label)
            mutation_results.append({'id': label, 'observed_exit': 1,
                                     'result': json.loads(res['stdout'])})
    if any((args.input_root / f).read_bytes() != old[f] or (candidates / f).read_bytes() != new[f] for f in NAMES):
        raise ValueError('document baseline or candidate was mutated')
    receipt = {
        'schema': 'completion-contract-doc-check-v1', 'status': 'PASS',
        'completed_utc': datetime.now(timezone.utc).isoformat(),
        'primary_checks': primary, 'mutations': mutation_results,
        'mutations_rejected': len(mutation_results), 'checker_invocations': 2 + len(mutation_results),
        'patch_targets': list(NAMES), 'patch_to_complete_files': 'BYTE_EXACT',
        'historical_handoff_body': 'PRESERVED_EXCEPT_EXPLICIT_HEADING_RELABEL',
        'input_and_candidates_unchanged': True,
        'scope': 'Document labels and byte application only; NOT a production RED/GREEN or FHE test.',
    }
    (evidence / 'DOC_CHECK_RECEIPT.json').write_text(json.dumps(receipt, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('PASS: old/new document exits 1/0; 12 text mutations rejected; 2-file patch byte-exact.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
