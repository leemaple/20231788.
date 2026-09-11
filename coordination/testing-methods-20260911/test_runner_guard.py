import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('runner_guard', Path(__file__).with_name('runner_guard.py'))
guard = importlib.util.module_from_spec(spec)
spec.loader.exec_module(guard)


class RunnerGuardTests(unittest.TestCase):
    def test_only_new_exact_tag_first_attempt(self):
        event = {'created': True, 'deleted': False, 'forced': False}
        guard.event('push', guard.REF, '1', event)
        for key, value in [('created', False), ('deleted', True), ('forced', True)]:
            with self.subTest(key=key), self.assertRaises(ValueError):
                guard.event('push', guard.REF, '1', dict(event, **{key: value}))
        for args in [('workflow_dispatch', guard.REF, '1'), ('push', guard.REF+'x', '1'),
                     ('push', guard.REF, '2')]:
            with self.subTest(args=args), self.assertRaises(ValueError):
                guard.event(*args, event)

    def test_exact_single_controls_target(self):
        executable = '/tmp/owned-build/initial_phase_exact_contract_test'
        test = {'name': guard.TEST, 'command': [executable, '--controls']}
        guard.selection({'tests': [test]}, executable)
        for tests in [[], [test, test], [dict(test, name='other')],
                      [dict(test, command=[executable, '--baseline'])],
                      [dict(test, command=['/tmp/other', '--controls'])]]:
            with self.subTest(tests=tests), self.assertRaises(ValueError):
                guard.selection({'tests': tests}, executable)

    def test_exact_classified_result_and_commit(self):
        sha = 'a'*40
        lines = guard.expected_lines(sha)
        guard.result('\n'.join('1: '+x for x in lines), sha)
        for bad in ['\n'.join(lines[:-1]), '\n'.join(lines+lines),
                    '\n'.join(lines).replace('FRESH_SUPPORT_BOUND', 'OTHER'),
                    '\n'.join(lines).replace(sha, 'b'*40),
                    '\n'.join(lines)+'\n1: UNCLASSIFIED_FAILURE detail_suppressed']:
            with self.subTest(bad=bad), self.assertRaises(ValueError):
                guard.result(bad, sha)

    def test_only_current_record_in_complete_run_history(self):
        sha = 'a'*40
        run = {'id': 123, 'head_sha': sha, 'head_branch': guard.REF.split('/')[-1],
               'event': 'push', 'path': '.github/workflows/initial-phase-exact-once.yml',
               'run_attempt': 1}
        guard.history([{'workflow_runs': [run]}], '123', sha)
        for runs in [[], [run, dict(run, id=122)], [dict(run, head_sha='b'*40)],
                     [dict(run, run_attempt=2)], [dict(run, id=124)]]:
            with self.subTest(runs=runs), self.assertRaises(ValueError):
                guard.history([{'workflow_runs': runs}], '123', sha)

    def test_seal_only_regular_allowed_evidence_files(self):
        with tempfile.TemporaryDirectory(prefix='initial-phase-guard-') as raw:
            root = Path(raw)
            for name in ['provenance.txt', 'source-sha256.txt']:
                (root/name).write_text('public fixture\n')
            rows = guard.evidence(root, complete=False)
            self.assertEqual([r['path'] for r in rows], ['provenance.txt', 'source-sha256.txt'])
            self.assertTrue(all(r['bytes'] == 15 and len(r['sha256']) == 64 for r in rows))
            with self.assertRaises(ValueError):
                guard.evidence(root, complete=True)
            extra = root/'core.1'
            extra.write_text('public rejected fixture')
            with self.assertRaises(ValueError):
                guard.evidence(root, complete=False)
            extra.unlink()
            link = root/'ctest.log'
            link.symlink_to(root/'provenance.txt')
            with self.assertRaises(ValueError):
                guard.evidence(root, complete=False)
            link.unlink()
            link.mkdir()
            with self.assertRaises(ValueError):
                guard.evidence(root, complete=False)

    def test_complete_seal_cli_covers_every_uploaded_file_once(self):
        with tempfile.TemporaryDirectory(prefix='initial-phase-seal-') as raw:
            root = Path(raw)
            for name in guard.EVIDENCE:
                (root/name).write_text('public fixture\n')
            command = [sys.executable, '-B', '-I', str(Path(guard.__file__)),
                       'seal', str(root), 'a'*40, '123', 'success']
            completed = subprocess.run(command, check=True, capture_output=True, text=True)
            self.assertEqual(completed.stdout, 'seal=PASS\n')
            manifest = json.loads((root/'MANIFEST.json').read_text())
            self.assertEqual({row['path'] for row in manifest['files']}, guard.EVIDENCE)
            self.assertEqual(len(manifest['files']), len(guard.EVIDENCE))
            self.assertEqual(manifest['source_sha'], 'a'*40)
            self.assertEqual(manifest['execution_step_outcome'], 'success')
            self.assertTrue(manifest['manifest_self_excluded'])
            repeated = subprocess.run(command, capture_output=True, text=True)
            self.assertNotEqual(repeated.returncode, 0)


if __name__ == '__main__':
    unittest.main()
