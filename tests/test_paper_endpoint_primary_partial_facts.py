"""Public parse-error observation tests; no prefix reparsing or finalization."""
import unittest

import paper_endpoint_primary_reader as reader
from test_paper_endpoint_primary_reader import (
    IDENTITY, complete_lines, ctest_log, replace_line,
)


class PrimaryPartialFactsTests(unittest.TestCase):
    def test_late_unknown_record_retains_complete_legacy_facts_and_boost(self):
        data = (ctest_log(complete_lines(2)) +
                b"61: FS_ENDPOINT_UNKNOWN\tvalue=1\n")
        with self.assertRaises(reader.PrimaryLogError) as caught:
            reader.parse_primary_log(data, IDENTITY, ctest_exit_code=8)
        self.assertEqual(caught.exception.reason, "FORMAT")
        self.assertEqual(
            caught.exception.observation,
            reader.PrimaryObservation(2, "FAIL", 108300),
        )

    def test_uncommitted_records_do_not_invent_partial_facts(self):
        cases = (
            (ctest_log(["FS_ENDPOINT_UNKNOWN\tvalue=1"]),
            reader.PrimaryObservation(None, "NOT_OBSERVED", None)),
            (ctest_log(replace_line(
                complete_lines(2), "FS_ENDPOINT_SCALE\tindex=0\t",
                lambda line: line.replace("\tnumerator=", "\tnumerator=x", 1))),
             reader.PrimaryObservation(None, "NOT_OBSERVED", 108300)),
            (ctest_log(replace_line(
                complete_lines(2), "COMPLETE test=",
                lambda line: line.replace("failures: 2", "failures: 3"))),
             reader.PrimaryObservation(None, "NOT_OBSERVED", 108300)),
        )
        for data, expected in cases:
            with self.subTest(expected=expected), \
                    self.assertRaises(reader.PrimaryLogError) as caught:
                reader.parse_primary_log(data, IDENTITY, ctest_exit_code=8)
            self.assertEqual(caught.exception.observation, expected)


if __name__ == "__main__":
    unittest.main(verbosity=2)
