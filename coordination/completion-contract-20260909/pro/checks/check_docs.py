#!/usr/bin/env python3
"""Frozen, text-only COMPLETION-CONTRACT-01 document acceptance checker.

Reads exactly two Markdown files. Does not import supplied project code and
never runs an algorithm, transform, compiler, network request or child process.
The fixed fields are document labels, not a proof of algorithm correctness.
"""
import argparse
import json
from pathlib import Path
import re
import sys

EXPECTED = {
    'COMPLETION_CONTRACT_ID': 'COMPLETION-CONTRACT-01',
    'SOURCE_SNAPSHOT': 'a7f54de2701a1b9bc02660f66febeff707b56651',
    'ALGORITHM_CORRECTNESS': 'CONDITIONAL_SUPPORTED',
    'ORIGINAL_S100_LINUX_E80': 'FAIL',
    'ORIGINAL_S100_WINDOWS_E80': 'FAIL',
    'S116_LINUX_E80': 'PASS_SEPARATE_CHANGED_PARAMETERS',
    'S116_WINDOWS_E80': 'PASS_SEPARATE_CHANGED_PARAMETERS',
    'ANNULUS125_LINUX_E80': 'PASS_ONE_CHANGED_INPUT_SAMPLE',
    'ANNULUS125_WINDOWS_E80': 'NOT_RUN',
    'CURRENT_P_ECD': 'CERTIFIED_NOT_HISTORICAL_P',
    'TABLE3_ORIGINAL_STATISTICS': 'NOT_REPRODUCED_NOT_A_UNIVERSAL_ENGINEERING_GATE',
    'DEPLOYMENT_SECURITY': 'NOT_CERTIFIED',
    'OVERALL_FROZEN_ACCEPTANCE': 'NOT_ALL_PASSED',
    'NEXT_CRYPTO_RUN': 'NONE_JUSTIFIED',
}
FILES = ('CHECK_AND_HANDOFF.zh-CN.md', 'REPRODUCE.zh-CN.md')
HISTORICAL_HEADING = '## 2026-09-08 历史增补（其中“最新”仅指当时）'
S116_SENTENCE = ('在另行命名的 S116 改参实验中，Linux、Windows 均通过该实验沿用的绝对分量误差上限')


def validate(root: Path) -> dict:
    errors, texts = [], {}
    for name in FILES:
        path = root / name
        if path.is_symlink() or not path.is_file():
            errors.append('missing_or_nonregular_file:' + name)
            continue
        try:
            texts[name] = path.read_bytes().decode('utf-8')
        except (OSError, UnicodeError) as exc:
            errors.append('read_error:' + name + ':' + type(exc).__name__)
    if len(texts) == 2:
        handoff, reproduce = (texts[name] for name in FILES)
        top = handoff.split(HISTORICAL_HEADING, 1)[0]
        if handoff.count(HISTORICAL_HEADING) != 1:
            errors.append('historical_heading_not_unique')
        for key, value in EXPECTED.items():
            found = re.findall(r'^' + re.escape(key) + r': (.*)$', handoff, re.M)
            top_found = re.findall(r'^' + re.escape(key) + r': (.*)$', top, re.M)
            if found != [value] or top_found != [value]:
                errors.append('wrong_or_duplicate_current_field:' + key)
        if 'codex/public-s100-ecd-cell-20260909' not in top:
            errors.append('current_branch_missing')
        if 'fcd745ae30a3f54e37b8ac060c854c226e38feb1' not in top:
            errors.append('task_snapshot_missing')
        for label, phrase in {
            'conditional_correctness': '有条件',
            'not_original_p': '不是旧 p 的身份认证',
            'no_source_changes': '不改生产',
            'not_all_frozen_passed': '完整冻结数值验收尚未全项通过',
        }.items():
            if phrase not in top:
                errors.append('qualification_missing:' + label)
        if reproduce.count('## 当前状态限定（COMPLETION-CONTRACT-01，2026-09-09）') != 1:
            errors.append('reproduce_current_scope_missing')
        if reproduce.count(S116_SENTENCE) != 1:
            errors.append('s116_success_not_locally_scoped')
        if '原 S100 Linux／Windows E80 均 FAIL' not in reproduce:
            errors.append('reproduce_original_failure_missing')
    return {'status': 'PASS' if not errors else 'FAIL',
            'scope': 'Frozen documentation only; not production or FHE validation.',
            'field_count': len(EXPECTED), 'errors': errors}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, required=True)
    args = parser.parse_args()
    result = validate(args.root)
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 0 if result['status'] == 'PASS' else 1


if __name__ == '__main__':
    sys.exit(main())
