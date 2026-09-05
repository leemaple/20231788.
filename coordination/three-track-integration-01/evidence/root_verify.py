#!/usr/bin/env python3
"""Read-only audit of an exact-source three-track hosted job; no crypto execution."""
import hashlib
import json
import re
import shlex
import subprocess
import sys
from fractions import Fraction
from pathlib import Path

SOURCE = "9c4d83b5cde16e5c5af89886bd73fe5252a99002"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
ROOT = Path("/Users/lifeng/Documents/20231788-openfhe-three-track-integration-20260905")
subprocess.run(["git", "-C", str(ROOT), "diff", "--exit-code", SOURCE, "--", "src", "include", "tests", "CMakeLists.txt", ".github/workflows"], check=True, stdout=subprocess.PIPE)
cmake = subprocess.check_output(["git", "-C", str(ROOT), "show", SOURCE + ":CMakeLists.txt"]).decode()
expected = []
for m in re.finditer(r"add_test\(\s*NAME\s+(\S+)\s+COMMAND\s+([^)]+)\)", cmake):
    expected.append({"name": m.group(1), "command": m.group(2).split(), "line": cmake.count("\n", 0, m.start()) + 1})
assert len(expected) == 60
by_name = {x["name"]: x for x in expected}
def normalized(command):
    parts = list(command)
    parts[0] = parts[0].replace("\\", "/").rsplit("/", 1)[-1].removesuffix(".exe")
    return parts
results = []
for supplied in sys.argv[1:]:
    path = Path(supplied)
    raw = path.read_bytes()
    lines = [re.sub(r"^\ufeff?\d{4}-\d\d-\d\dT\S+Z ?", "", x) for x in raw.decode().splitlines()]
    text = "\n".join(lines)
    provenance = f"PROJECT_SOURCE_COMMIT={SOURCE} GITHUB_RUN_ID=33964209898 GITHUB_RUN_ATTEMPT=1"
    assert provenance in text and PIN in text
    assert "-DNATIVE_SIZE=64" in text and "-DMATHBACKEND=4" in text
    listings = []
    for i, line in enumerate(lines):
        if line.strip() == "{" and i + 1 < len(lines) and '"backtraceGraph"' in lines[i + 1]:
            obj = json.JSONDecoder().raw_decode("\n".join(lines[i:]))[0]
            count = len(obj["tests"])
            assert count in (57, 60)
            assert [x["name"] for x in obj["tests"]] == [x["name"] for x in expected[:count]]
            graph = obj["backtraceGraph"]
            for actual, want in zip(obj["tests"], expected):
                assert normalized(actual["command"]) == want["command"]
                node = graph["nodes"][actual["backtrace"]]
                assert graph["commands"][node["command"]] == "add_test"
                assert node["line"] == want["line"]
                assert graph["files"][node["file"]].replace("\\", "/").endswith("CMakeLists.txt")
            listings.append({"line": i + 1, "count": count, "source_backtraces_match": True})
    assert [x["count"] for x in listings] == [57, 60]
    active = None
    invocations = []
    for i, line in enumerate(lines):
        begin = re.match(r"\s*Start\s+(\d+):\s+(\S+)\s*$", line)
        command = re.match(r"(\d+): Test command:\s+(.+)$", line)
        end = re.match(r"\s*(\d+)/(\d+) Test\s*#\s*(\d+):\s+(\S+)\s+\.+\s+Passed\s+([\d.]+) sec", line)
        if begin:
            assert active is None
            number, name = int(begin[1]), begin[2]
            assert expected[number - 1]["name"] == name
            active = dict(number=number, name=name, start_line=i + 1, command=None, output=[])
        elif command:
            assert active and int(command[1]) == active["number"] and active["command"] is None
            # CTest prints argv, not a POSIX shell command: Windows path
            # separators must survive tokenization, including inside quotes.
            lexer = shlex.shlex(command[2], posix=True)
            lexer.whitespace_split = True
            lexer.commenters = ""
            lexer.escape = ""
            parts = list(lexer)
            assert normalized(parts) == by_name[active["name"]]["command"]
            active["command"] = parts
            active["command_line"] = i + 1
        elif end:
            assert active and active["command"] and int(end[3]) == active["number"] and end[4] == active["name"]
            active.update(result="Passed", result_line=i + 1, ordinal=int(end[1]), group_size=int(end[2]), seconds=end[5])
            invocations.append(active)
            active = None
        elif active:
            active["output"].append({"line": i + 1, "text": line})
    assert active is None and len(invocations) == 123
    expected_groups = [[expected[2]], expected[55:57], expected[:57], [expected[57]], expected[58:60], expected]
    offset = 0
    for group in expected_groups:
        actual = invocations[offset:offset + len(group)]
        assert [x["name"] for x in actual] == [x["name"] for x in group]
        assert [x["ordinal"] for x in actual] == list(range(1, len(group) + 1))
        assert all(x["group_size"] == len(group) for x in actual)
        offset += len(group)
    api = ["relin2", "rs2", "mult2", "add", "sub"]
    def built_target(target):
        # Unix Makefiles and Windows Ninja emit different completion records.
        return any(line == "[100%] Built target " + target or
                   re.fullmatch(r"\[(\d+)/\1\] Linking CXX executable " + re.escape(target) + r"\.exe", line)
                   for line in lines)
    for prefix in api:
        assert built_target(prefix + "_api_contract_test")
    for target in [x["command"][0] for x in expected[57:]]:
        assert built_target(target)
    io_records, repeated_records, h128_records = [], [], []
    for inv in invocations:
        output = inv["output"]
        if inv["number"] == 58:
            numeric = [x for x in output if f"test={expected[57]['name']} result=PASS" in x["text"]]
            assert len(numeric) == 1
            n = numeric[0]
            record = dict(re.findall(r"(\w+)=([^\s]+)", n["text"]))
            assert record["source"] == SOURCE and record["openfhe_pin"] == PIN
            for k, v in {"N":"64", "S":"16", "gap":"2", "native":"64", "backend":"4", "depth":"7", "first_bits":"55", "malformed_key_rejections":"32"}.items():
                assert record[k] == v, k
            errors = ["max_fresh_public_error", "max_fresh_oracle_error", "max_product_public_error", "max_product_oracle_error", "input_public_delta_error", "input_oracle_delta_error", "product_public_delta_error", "product_oracle_delta_error"]
            for k in errors:
                assert 0 <= Fraction(record[k]) <= Fraction(1, 2**80), (k, record[k])
            for k in ["max_horner_component_disagreement", "cross_precision_error"]:
                assert 0 <= Fraction(record[k]) <= Fraction(1, 2**120)
            assert Fraction(int(record["exact_scale_numerator"]), int(record["exact_scale_denominator"])) == Fraction(2**200, int(record["q_div"]) * int(record["q_l"]))
            assert int(record["centered_headroom"]) > 0
            markers = [
                "first_modulus_boundary_fixture ready=1 actual_first_bits=56 Q_towers=8 N=64 M=128 fixture_new_keypairs=0",
                "first_modulus_boundary_rejection passed",
                "clone_isolation_contract passed fresh_and_result=1 coefficients=1 scalars=1 present_empty_maps=1",
                "shared_params_fixture allocation_plan additional_matching_keypairs=1 additional_eval_keygen_calls=1 actual_first_bits=55 isolated_context_and_bases=1",
                "shared_params_fixture ready=1 valid_clone_bind_decrypt=1 actual_first_bits=55 Q_towers=8 fixture_new_keypairs=1 fixture_eval_keygen_calls=1 crypto_complete_before_drift=1",
                "shared_params_mutation ready=1 Q_towers_before=8 Q_towers_after=7 immutable_receipt_towers=8 next_boundary=CloneForEvaluation",
                "shared_params_boundary_rejection passed boundary=CloneForEvaluation",
                "shared_params_boundary_rejection passed boundary=BindFirstMult2Rcb",
                "shared_params_boundary_rejection passed boundary=Decrypt",
                "shared_params_drift_contract passed boundaries=3 owned_eval_tag_cleanup=1"]
            anchor_lines = [n["line"]]
            for marker in markers:
                hits = [x["line"] for x in output if x["text"] == "58: " + marker]
                assert len(hits) == 1
                anchor_lines += hits
            assert anchor_lines == sorted(anchor_lines) and anchor_lines[-1] < inv["result_line"]
            io_records.append({"numeric": record, "ordered_anchor_lines": anchor_lines})
        elif inv["number"] == 59:
            records = [json.loads(x["text"].split(": ", 1)[1]) for x in output if x["text"].startswith('59: {"test":')]
            assert [(x["trial"], x["stage"]) for x in records] == [(trial, stage) for trial in range(4) for stage in (1, 2)]
            for r in records:
                assert r["test"] == expected[58]["name"] and r["N"] == 64 and r["batch"] == 16
                assert r["depth"] == 9 and r["secret_distribution"] == "UNIFORM_TERNARY"
                d, m1, m2 = int(r["d"]), int(r["m1"]), int(r["m2"])
                stage = r["stage"]
                assert (d, m1, m2) == (1125899906843009, 1125899906840833, 1125899906844161)
                want = Fraction(2**200, d*m1) if stage == 1 else Fraction(2**400, d*d*d*m1*m1*m2)
                assert Fraction(int(r["scale_numerator"]), int(r["scale_denominator"])) == want
                assert r["evaluation_family"] == stage - 1 and r["result_family"] == 1
                assert int(r["m"]) == (m1 if stage == 1 else m2)
                assert r["tag"].endswith("-mult2-family-1") and r["observed_product_headroom_bits"] > 0
                for k in ("max_slot_error", "delta_error"):
                    assert 0 <= Fraction(r[k]) <= Fraction(1, 2**80)
            repeated_records += records
        elif inv["number"] == 60:
            markers = ["valid-path assertions passed (diagnostic, not paper evidence)",
                       "50 named rejection assertions passed (no injected tag collisions)",
                       "two-call uniqueness and owned-tag cache-isolation assertions passed"]
            found = []
            for marker in markers:
                hits = [x["line"] for x in output if x["text"] == "60: " + marker]
                assert len(hits) == 1
                found += hits
            assert found == sorted(found)
            h128_records.append({"marker_lines": found, "named_rejections": 50, "scope": "fixed-Q N256 diagnostic; source assertions bound to actual exit PASS"})
    assert len(io_records) == 2 and len(repeated_records) == 16 and len(h128_records) == 2
    for inv in invocations:
        del inv["output"]
    results.append({"log": str(path), "bytes": len(raw), "sha256": hashlib.sha256(raw).hexdigest(),
                    "source": SOURCE, "status": "PASS_RETAINED_EXECUTION_AUDIT", "live_listings": listings,
                    "actual_bindings": invocations, "api_targets": 5, "contract_targets": 3,
                    "io_records": io_records, "repeated_stage_records": repeated_records, "h128_invocations": h128_records,
                    "scope": "three diagnostic modules co-build and regressions, not integrated full-paper operations"})
print(json.dumps({"audit":"root independent full retained-log parser", "mac_crypto":"NOT_RUN", "jobs":results}, indent=2))
