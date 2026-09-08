#!/usr/bin/env python3
"""Scalar-only tests for the Ecd-cell one-shot event/source gate."""

from copy import deepcopy
import unittest

import check_ci_gate as gate


def fresh_event() -> dict:
    return {"ref": gate.REF, "created": True, "deleted": False,
            "forced": False}


class OneShotGateContract(unittest.TestCase):
    def test_exact_fresh_tag_creation_attempt_one_is_admitted(self):
        admitted, reason = gate.event_admitted(
            event_name="push", ref=gate.REF, run_attempt=1,
            event=fresh_event())
        self.assertTrue(admitted, reason)

    def test_every_event_boundary_mutation_is_rejected(self):
        base = {"event_name": "push", "ref": gate.REF, "run_attempt": 1,
                "event": fresh_event()}
        cases = {
            "branch": {"ref": "refs/heads/codex/public-s100-ecd-cell-20260909",
                       "event": {**fresh_event(), "ref":
                                 "refs/heads/codex/public-s100-ecd-cell-20260909"}},
            "wrong tag": {"ref": "refs/tags/public-s100-ecd-cell-wrong",
                          "event": {**fresh_event(), "ref":
                                    "refs/tags/public-s100-ecd-cell-wrong"}},
            "tag update": {"event": {**fresh_event(), "created": False}},
            "forced": {"event": {**fresh_event(), "forced": True}},
            "deleted": {"event": {**fresh_event(), "deleted": True}},
            "rerun": {"run_attempt": 2},
            "payload disagreement": {"event": {**fresh_event(), "ref":
                                                 "refs/tags/another-tag"}},
            "manual": {"event_name": "workflow_dispatch"},
        }
        for label, mutation in cases.items():
            inputs = deepcopy(base)
            inputs.update(mutation)
            with self.subTest(label=label):
                self.assertFalse(gate.event_admitted(**inputs)[0])

    def test_actual_workflow_source_contract(self):
        gate.verify_workflow_source()


if __name__ == "__main__":
    unittest.main(verbosity=2)
