#!/usr/bin/env python3
"""Bounded synthetic TSV receiver regression; never a ciphertext experiment.

Seam: externally supplied annulus TSV -> accepted/rejected scalar evidence.
The user delegates routine validation choices; original numerical gates stay fixed.
Run against the immutable Pro receiver for RED, then an integrated copy for GREEN.
"""
import argparse
import importlib.util
import json
import sys
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("receiver", type=Path)
    args = parser.parse_args()
    path = args.receiver.resolve(strict=True)
    sys.path.insert(0, str(path.parent))
    spec = importlib.util.spec_from_file_location("annulus_receiver_under_test", path)
    receiver = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(receiver)
    baseline = receiver.synthetic()
    assert receiver.replay(baseline, "0" * 40, 0)["status"] == "PASS"
    # Slot 1024 is not part of the two-slot microdifference witness. Each
    # endpoint error remains below T; only their inconsistent readout is wrong.
    mutated = baseline.replace(
        "1024\t0\t0\t0\t0\t0\t0\n",
        "1024\t0\t0\t0\t0\t4e-25\t0\n",
    ).replace("max\tE8_prod\t0\t0\n", "max\tE8_prod\t1024\t4e-25\n")
    assert mutated != baseline
    try:
        receiver.replay(mutated, "0" * 40, 0)
    except ValueError as error:
        # Only deliberate rejection of this observable inconsistency qualifies.
        if str(error) != "terminal producer/observer disagreement":
            raise
        print(json.dumps({"status": "PASS_SYNTHETIC_ONLY", "positive": 1,
                          "rejected": "terminal producer/observer disagreement",
                          "encrypted_runs": 0}))
        return 0
    print(json.dumps({"status": "EXPECTED_REJECTION_MISSING", "positive": 1,
                      "incorrectly_accepted": "terminal producer/observer disagreement",
                      "encrypted_runs": 0}))
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
