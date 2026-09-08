#!/usr/bin/env python3
"""Synthetic completed-process evidence seam; zero encryption/FFT/builds."""
import json
import hashlib
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
import replay_annulus125 as receiver
import finalize_once


class CompletedEvidence(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name).resolve()
        self.source = "0" * 40
        shared = {"source_commit": self.source, "contract": receiver.CONTRACT,
                  "entry": "DIRECT_BINARY_ONCE_NOT_CTEST"}
        self.write("program-start.json", {**shared, "argv": ["/synthetic/s100_annulus125_eight_square_test",
                   "--output", str(self.root / "raw.tsv")],
                   "started_utc": "2026-09-08T00:00:00+00:00", "timeout_seconds": 1200})
        self.write("program-end.json", {**shared, "returncode": 0, "timed_out": False,
                   "ended_utc": "2026-09-08T00:01:00+00:00"})
        (self.root / "raw.tsv").write_text(receiver.synthetic())
        (self.root / "stdout.txt").write_text(finalize_once.banner("PASS"))
        (self.root / "stderr.txt").write_text("")

    def tearDown(self):
        self.temp.cleanup()

    def write(self, name, value):
        (self.root / name).write_text(json.dumps(value) + "\n")

    def test_valid_evidence_uses_two_scalar_precisions_not_two_payloads(self):
        self.assertEqual(finalize_once.finalize(self.root, self.source), 0)
        result = json.loads((self.root / "verification.json").read_text())
        self.assertEqual(result["status"], "PASS")
        self.assertEqual([r["decimal_precision"] for r in result["replays"]], [180, 230])
        self.assertEqual(set(result["evidence_sha256"]),
                         {"program-start.json", "program-end.json", "raw.tsv", "stdout.txt", "stderr.txt"})
        for name, digest in result["evidence_sha256"].items():
            self.assertEqual(digest, hashlib.sha256((self.root / name).read_bytes()).hexdigest())
        with self.assertRaises(FileExistsError):
            finalize_once.finalize(self.root, self.source)

    def test_numerical_fail_is_preserved_as_fail_not_invalid(self):
        text = receiver.synthetic().replace("1024\t0\t0\t0\t0\t0\t0\n",
                                            "1024\t0\t0\t4e-25\t0\t4e-25\t0\n")
        for name in ("E8_obs", "E8_prod", "A8_obs"):
            text = text.replace(f"max\t{name}\t0\t0\n", f"max\t{name}\t1024\t4e-25\n")
        text = text.replace("gate\tA8_le_T_over_4\t1\n", "gate\tA8_le_T_over_4\t0\n")
        text = text.replace("status\tCOMPLETE\tPASS\n", "status\tCOMPLETE\tFAIL\n")
        (self.root / "raw.tsv").write_text(text)
        end = json.loads((self.root / "program-end.json").read_text())
        end["returncode"] = 1
        self.write("program-end.json", end)
        (self.root / "stdout.txt").write_text(finalize_once.banner("FAIL"))
        self.assertEqual(finalize_once.finalize(self.root, self.source), 1)
        self.assertEqual(json.loads((self.root / "verification.json").read_text())["status"], "FAIL")

    def test_wrong_source_and_truncated_evidence_cannot_pass(self):
        with self.assertRaisesRegex(ValueError, "receipt source/contract"):
            finalize_once.finalize(self.root, "1" * 40)
        (self.root / "raw.tsv").write_text(receiver.synthetic()[:-1])
        with self.assertRaisesRegex(ValueError, "LF-terminated"):
            finalize_once.finalize(self.root, self.source)
        self.assertFalse((self.root / "verification.json").exists())


if __name__ == "__main__":
    unittest.main(verbosity=2)
