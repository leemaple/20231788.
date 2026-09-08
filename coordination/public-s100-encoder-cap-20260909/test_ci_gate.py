#!/usr/bin/env python3
"""Synthetic-only tests for the public encoder-cap workflow gate."""

from copy import deepcopy
import unittest

import check_ci_gate as gate


def event(ref: str) -> dict:
    return {"ref": ref, "created": True, "deleted": False, "forced": False}


class EventGateContract(unittest.TestCase):
    def test_exact_fresh_red_and_green_tags_are_allowed(self):
        for ref, expected in ((gate.RED_REF, "red"), (gate.GREEN_REF, "green")):
            stage, reason = gate.event_stage(
                event_name="push", ref=ref, run_attempt=1, event=event(ref))
            self.assertEqual(stage, expected, reason)

    def test_all_mutated_event_boundaries_are_rejected(self):
        base = {"event_name": "push", "ref": gate.GREEN_REF,
                "run_attempt": 1, "event": event(gate.GREEN_REF)}
        cases = {
            "branch": {"ref": "refs/heads/codex/public-encoder-cap-20260909",
                       "event": event("refs/heads/codex/public-encoder-cap-20260909")},
            "wrong tag": {"ref": "refs/tags/public-s100-encoder-cap-wrong",
                          "event": event("refs/tags/public-s100-encoder-cap-wrong")},
            "rerun": {"run_attempt": 2},
            "forced": {"event": {**event(gate.GREEN_REF), "forced": True}},
            "deleted": {"event": {**event(gate.GREEN_REF), "deleted": True}},
            "not created": {"event": {**event(gate.GREEN_REF), "created": False}},
            "mismatched payload ref": {"event": event(gate.RED_REF)},
            "not push": {"event_name": "workflow_dispatch"},
        }
        for label, mutation in cases.items():
            inputs = deepcopy(base)
            inputs.update(mutation)
            self.assertIsNone(gate.event_stage(**inputs)[0], label)

    def test_actual_workflow_source_contract(self):
        gate.verify_workflow_source(gate.parse_yaml(gate.WORKFLOW))


if __name__ == "__main__":
    unittest.main(verbosity=2)
