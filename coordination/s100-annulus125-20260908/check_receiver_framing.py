#!/usr/bin/env python3
"""Synthetic-only complete-LF TSV boundary regression; no encryption."""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import replay_annulus125 as receiver


def main():
    text = receiver.synthetic()
    assert receiver.replay(text, "0" * 40, 0)["status"] == "PASS"
    variants = {"missing_final_lf": text[:-1],
                "crlf": text.replace("\n", "\r\n")}
    accepted = []
    rejected = []
    for name, value in variants.items():
        try:
            receiver.replay(value, "0" * 40, 0)
        except ValueError as error:
            if str(error) != "invalid LF-terminated TSV framing":
                raise
            rejected.append(name)
        else:
            accepted.append(name)
    print(json.dumps({"status": "EXPECTED_REJECTION_MISSING" if accepted else "PASS_SYNTHETIC_ONLY",
                      "positive": 1, "rejected": rejected,
                      "incorrectly_accepted": accepted, "encrypted_runs": 0}))
    return bool(accepted)


if __name__ == "__main__":
    raise SystemExit(main())
