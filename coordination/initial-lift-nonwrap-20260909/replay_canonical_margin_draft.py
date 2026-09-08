"""Replay the frozen bounded model without modifying its original evidence."""
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys
from tempfile import TemporaryDirectory


def main():
    here = Path(__file__).resolve().parent
    root = here.parents[1]
    script = here / 'check_canonical_margin_draft.py'
    expected = (here / 'ROOT_CANONICAL_MARGIN_DRAFT.json').read_bytes()
    assert hashlib.sha256(script.read_bytes()).hexdigest() == '5b4ccc33e215323c159e95460c79a25ab6d0466766d5988ee80b964c0a62a68a'
    assert hashlib.sha256(expected).hexdigest() == '77529bd00137a9556d6595ccc2e642301b83411c5773c3b9a8bd633f071bc0fc'
    with TemporaryDirectory(prefix='openfhe-canonical-replay-') as directory:
        replay_root = Path(directory)
        for source in (script, root / 'src/repeated_mult2.cpp',
                       root / 'tests/paper_full_eight_square_oracle.h'):
            destination = replay_root / source.relative_to(root)
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source, destination)
        replay_script = replay_root / script.relative_to(root)
        subprocess.run([sys.executable, '-B', '-I', str(replay_script)],
                       check=True, capture_output=True, text=True, timeout=30)
        actual = (replay_script.parent / 'ROOT_CANONICAL_MARGIN_DRAFT.json').read_bytes()
        assert actual == expected, 'Replay differs from frozen result; original evidence is untouched'
    print('PASS: exact frozen model replay; output byte-identical; original evidence untouched')


if __name__ == '__main__':
    main()
