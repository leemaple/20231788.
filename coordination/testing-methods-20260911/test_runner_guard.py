import importlib.util
from pathlib import Path
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


if __name__ == '__main__':
    unittest.main()
