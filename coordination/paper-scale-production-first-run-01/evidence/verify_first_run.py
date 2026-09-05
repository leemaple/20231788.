#!/usr/bin/env python3
"""Read-only audit of retained Linux/Windows first production-run evidence."""
import hashlib
import json
import re
import shlex
import subprocess
import sys
from fractions import Fraction
from pathlib import Path

ROOT = Path("/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905")
HERE = ROOT / "artifacts/handoffs/paper-scale-production-first-run-01"
HEAD = "1853701d8862dabef804021d2dea1899776f38e2"
SOURCE = "b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89"
FROZEN = "448e9d3067796b64656f0746f4be5c4153b1271d"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
RUN = "33971779479"
ATTEMPT = "1"
HOST = (sys.argv[1] if len(sys.argv) > 1 else "windows").lower()

HOSTS = {
    "linux": {
        "job": "101321455160",
        "log": "LINUX_RAW.log",
        "capture": "LINUX_CONNECTOR_CAPTURE.json",
        "bytes": 455726,
        "sha": "75fd6a944d1b50b463e0ff934533e95d8f21cb9d3c7bb7a4cbf0b99f254ab4dd",
        "connector_bytes": 455726,
        "connector_sha": "75fd6a944d1b50b463e0ff934533e95d8f21cb9d3c7bb7a4cbf0b99f254ab4dd",
        "crlf": 0,
        "completed": "2026-09-05T14:31:57Z",
        "failed_step": "Build paper full eight-square contract",
    },
    "windows": {
        "job": "101321455226",
        "log": "WINDOWS_LF.log",
        "capture": "WINDOWS_CONNECTOR_CAPTURE.json",
        "bytes": 567908,
        "sha": "c6e009afe774af39fee24e3687323414f3ce5f1d5069084f06fe13b181f985bf",
        "connector_bytes": 575557,
        "connector_sha": "be4bf00801adef0eafddf3238e9f8a6cf231f0ccd2ff7170555a0996f78f6f54",
        "crlf": 7649,
        "completed": "2026-09-05T14:36:38Z",
        "failed_step": "Run paper full eight-square contract once",
    },
}


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def git(*args):
    return subprocess.check_output(["git", "-C", str(ROOT), *args]).decode("utf-8")


def git_show(commit, path):
    return git("show", f"{commit}:{path}")


def normalize_command(command):
    parts = list(command)
    parts[0] = parts[0].replace("\\", "/").rsplit("/", 1)[-1].removesuffix(".exe")
    return parts


def parse_shell_words(value):
    lexer = shlex.shlex(value, posix=True)
    lexer.whitespace_split = True
    lexer.commenters = ""
    lexer.escape = ""
    return list(lexer)


def built_target(lines, target):
    return any(
        re.fullmatch(r"\[\s*\d+%\] Built target " + re.escape(target), line)
        or re.fullmatch(r"\[(\d+)/(\1)\] Linking CXX executable " + re.escape(target) + r"\.exe", line)
        for line in lines
    )


require(HOST in HOSTS, "usage: verify_first_run.py [linux|windows]")
cfg = HOSTS[HOST]
require(git("rev-parse", "HEAD").strip() == HEAD, "repository HEAD changed")
require(not git("diff", "--name-only", SOURCE, HEAD, "--", "CMakeLists.txt", "include", "src", "tests").strip(),
        "HEAD contains production/test/CMake changes after audited source")
require(not git("diff", "--name-only", FROZEN, HEAD, "--", "CMakeLists.txt", "tests").strip(),
        "frozen CMake/tests changed after 448e9d3")

# Derive the immutable inventory and expected argv/backtrace lines from the frozen CMake file.
cmake = git_show(FROZEN, "CMakeLists.txt")
expected = []
for match in re.finditer(r"add_test\(\s*NAME\s+(\S+)\s+COMMAND\s+([^)]+)\)", cmake):
    expected.append({
        "name": match.group(1),
        "command": match.group(2).split(),
        "line": cmake.count("\n", 0, match.start()) + 1,
    })
require(len(expected) == 61, "frozen CMake must declare 61 tests")
require([item["name"] for item in expected[-4:]] == [
    "precision_client_io_first_mult2_contract",
    "repeated_mult2_semantic_two_square_contract",
    "paper_h128_client_keypair_contract",
    "paper_full_eight_square_contract",
], "frozen final four tests changed")
by_name = {item["name"]: item for item in expected}

# Prove the retained log is exactly the decoded connector content, allowing CRLF-to-LF only on Windows.
log_path = HERE / cfg["log"]
capture_path = HERE / cfg["capture"]
raw = log_path.read_bytes()
require(len(raw) == cfg["bytes"] and hashlib.sha256(raw).hexdigest() == cfg["sha"],
        "retained LF log byte identity changed")
require(raw.startswith(b"\xef\xbb\xbf") and b"\r\n" not in raw, "retained log is not UTF-8-BOM LF-only")
capture = json.loads(capture_path.read_text(encoding="utf-8"))
require(str(capture.get("run_id")) == RUN and str(capture.get("job_id")) == cfg["job"]
        and capture.get("source_commit") == SOURCE, "connector capture provenance mismatch")
connector = capture.get("content", "").encode("utf-8")
require(len(connector) == cfg["connector_bytes"] and hashlib.sha256(connector).hexdigest() == cfg["connector_sha"],
        "connector decoded byte identity changed")
require(connector.startswith(b"\xef\xbb\xbf") and connector.count(b"\r\n") == cfg["crlf"],
        "connector BOM/newline structure changed")
require((connector if HOST == "linux" else connector.replace(b"\r\n", b"\n")) == raw,
        "retained log is not the exact connector transform")

terminal = json.loads((HERE / "RUN_TERMINAL_01.json").read_text(encoding="utf-8"))
require(terminal["databaseId"] == int(RUN) and terminal["attempt"] == int(ATTEMPT)
        and terminal["headSha"] == SOURCE and terminal["conclusion"] == "failure",
        "run terminal metadata mismatch")
job = [item for item in terminal["jobs"] if item["databaseId"] == int(cfg["job"])]
require(len(job) == 1, "terminal job missing or duplicated")
job = job[0]
require(job["status"] == "completed" and job["conclusion"] == "failure"
        and job["completedAt"] == cfg["completed"], "terminal job status/time mismatch")
failed_steps = [step for step in job["steps"] if step["conclusion"] == "failure"]
require(len(failed_steps) == 1 and failed_steps[0]["name"] == cfg["failed_step"],
        "unexpected failed-step set")

decoded = raw.decode("utf-8")
lines = [re.sub(r"^\ufeff?\d{4}-\d\d-\d\dT\S+Z ?", "", line) for line in decoded.splitlines()]
text = "\n".join(lines)
provenance = f"PROJECT_SOURCE_COMMIT={SOURCE} GITHUB_RUN_ID={RUN} GITHUB_RUN_ATTEMPT={ATTEMPT}"
require(text.count(provenance) == 1, "source/run/attempt provenance missing or duplicated")
require(text.count(f"HEAD is now at {PIN[:7]}") == 1, "exact upstream checkout evidence missing or duplicated")
require(f"OPENFHE_COMMIT: {PIN}" in text and "-DNATIVE_SIZE=64" in text and "-DMATHBACKEND=4" in text,
        "pin/native/backend configuration evidence missing")

# Decode every live CTest JSON block and bind it to frozen name/order/argv/add_test provenance.
listings = []
for index, line in enumerate(lines):
    if line.strip() == "{" and index + 1 < len(lines) and '"backtraceGraph"' in lines[index + 1]:
        obj = json.JSONDecoder().raw_decode("\n".join(lines[index:]))[0]
        count = len(obj["tests"])
        require(count in (57, 60, 61), f"unexpected live inventory size {count}")
        require([item["name"] for item in obj["tests"]] == [item["name"] for item in expected[:count]],
                f"live {count} name/order mismatch")
        graph = obj["backtraceGraph"]
        for actual, want in zip(obj["tests"], expected):
            require(normalize_command(actual["command"]) == want["command"],
                    f"live argv mismatch: {want['name']}")
            node = graph["nodes"][actual["backtrace"]]
            require(graph["commands"][node["command"]] == "add_test" and node["line"] == want["line"],
                    f"live backtrace mismatch: {want['name']}")
            require(graph["files"][node["file"]].replace("\\", "/").endswith("CMakeLists.txt"),
                    "live backtrace file mismatch")
        listings.append({"line": index + 1, "count": count, "frozen_inventory_match": True})
require([item["count"] for item in listings] == ([57, 60] if HOST == "linux" else [57, 60, 61]),
        "live inventory sequence mismatch")

# Bind all accepted Starts to one printed command and one Passed record.
active = None
invocations = []
paper_invocation = None
for index, line in enumerate(lines):
    start = re.match(r"\s*Start\s+(\d+):\s+(\S+)\s*$", line)
    command = re.match(r"(\d+): Test command:\s+(.+)$", line)
    passed = re.match(r"\s*(\d+)/(\d+) Test\s*#\s*(\d+):\s+(\S+)\s+\.+\s+Passed\s+([\d.]+) sec", line)
    failed = re.match(r"\s*(\d+)/(\d+) Test\s*#\s*(\d+):\s+(\S+)\s+\.+\*\*\*Failed\s+([\d.]+) sec", line)
    if start:
        require(active is None, "nested Start records")
        number, name = int(start[1]), start[2]
        require(expected[number - 1]["name"] == name, "Start number/name differs from frozen inventory")
        active = {"number": number, "name": name, "start_line": index + 1, "command": None, "output": []}
    elif command:
        require(active is not None and int(command[1]) == active["number"] and active["command"] is None,
                "printed command is not uniquely bound to active Start")
        argv = parse_shell_words(command[2])
        require(normalize_command(argv) == by_name[active["name"]]["command"],
                "executed argv differs from frozen CMake")
        active["command"] = argv
        active["command_line"] = index + 1
    elif passed or failed:
        end = passed or failed
        require(active is not None and active["command"] is not None, "result has no Start/command binding")
        require(int(end[3]) == active["number"] and end[4] == active["name"], "result identity mismatch")
        active.update(result="Passed" if passed else "Failed", result_line=index + 1,
                      ordinal=int(end[1]), group_size=int(end[2]), seconds=end[5])
        if active["number"] == 61:
            require(HOST == "windows" and failed is not None and paper_invocation is None,
                    "unexpected paper runtime result")
            paper_invocation = active
        else:
            require(passed is not None, "accepted invocation failed")
            invocations.append(active)
        active = None
    elif active is not None:
        active["output"].append({"line": index + 1, "text": line})
require(active is None and len(invocations) == 123, "expected 123 fully bound accepted passes")
groups = [[expected[2]], expected[55:57], expected[:57], [expected[57]], expected[58:60], expected[:60]]
offset = 0
for group in groups:
    actual = invocations[offset:offset + len(group)]
    require([item["name"] for item in actual] == [item["name"] for item in group], "pass group order mismatch")
    require([item["ordinal"] for item in actual] == list(range(1, len(group) + 1)), "pass group ordinal mismatch")
    require(all(item["group_size"] == len(group) for item in actual), "pass group denominator mismatch")
    offset += len(group)
require(offset == 123, "pass partition mismatch")

# Validate each high-value semantic output inside its bound CTest process.
io_records, repeated_records, h128_records = [], [], []
for invocation in invocations:
    output = invocation["output"]
    if invocation["number"] == 58:
        hit = [item for item in output if f"test={expected[57]['name']} result=PASS" in item["text"]]
        require(len(hit) == 1, "I/O numeric PASS missing or duplicated")
        record = dict(re.findall(r"(\w+)=([^\s]+)", hit[0]["text"]))
        require(record["source"] == SOURCE and record["openfhe_pin"] == PIN, "I/O source/pin mismatch")
        for key, value in {"N":"64", "S":"16", "gap":"2", "native":"64", "backend":"4", "depth":"7",
                           "first_bits":"55", "malformed_key_rejections":"32"}.items():
            require(record[key] == value, f"I/O frozen field mismatch: {key}")
        for key in ("max_fresh_public_error", "max_fresh_oracle_error", "max_product_public_error",
                    "max_product_oracle_error", "input_public_delta_error", "input_oracle_delta_error",
                    "product_public_delta_error", "product_oracle_delta_error"):
            require(0 <= Fraction(record[key]) <= Fraction(1, 2**80), f"I/O 2^-80 gate failed: {key}")
        for key in ("max_horner_component_disagreement", "cross_precision_error"):
            require(0 <= Fraction(record[key]) <= Fraction(1, 2**120), f"I/O 2^-120 gate failed: {key}")
        require(Fraction(int(record["exact_scale_numerator"]), int(record["exact_scale_denominator"])) ==
                Fraction(2**200, int(record["q_div"]) * int(record["q_l"])), "I/O exact scale mismatch")
        require(int(record["centered_headroom"]) > 0, "I/O centered headroom not positive")
        io_records.append(record)
    elif invocation["number"] == 59:
        records = [json.loads(item["text"].split(": ", 1)[1]) for item in output
                   if item["text"].startswith('59: {"test":')]
        require([(item["trial"], item["stage"]) for item in records] ==
                [(trial, stage) for trial in range(4) for stage in (1, 2)], "repeated trial/stage matrix mismatch")
        for record in records:
            d, m1, m2 = int(record["d"]), int(record["m1"]), int(record["m2"])
            require((d, m1, m2) == (1125899906843009, 1125899906840833, 1125899906844161),
                    "repeated divisor/moduli mismatch")
            want = Fraction(2**200, d*m1) if record["stage"] == 1 else Fraction(2**400, d*d*d*m1*m1*m2)
            require(Fraction(int(record["scale_numerator"]), int(record["scale_denominator"])) == want,
                    "repeated exact scale mismatch")
            require(record["N"] == 64 and record["batch"] == 16 and record["depth"] == 9
                    and record["secret_distribution"] == "UNIFORM_TERNARY", "repeated frozen profile mismatch")
            require(record["evaluation_family"] == record["stage"] - 1 and record["result_family"] == 1,
                    "repeated family routing mismatch")
            require(0 <= Fraction(record["max_slot_error"]) <= Fraction(1, 2**80)
                    and 0 <= Fraction(record["delta_error"]) <= Fraction(1, 2**80),
                    "repeated numerical gate failed")
        repeated_records.extend(records)
    elif invocation["number"] == 60:
        markers = [
            "valid-path assertions passed (diagnostic, not paper evidence)",
            "50 named rejection assertions passed (no injected tag collisions)",
            "two-call uniqueness and owned-tag cache-isolation assertions passed",
        ]
        marker_lines = []
        for marker in markers:
            hits = [item["line"] for item in output if item["text"] == "60: " + marker]
            require(len(hits) == 1, f"h128 marker missing or duplicated: {marker}")
            marker_lines.extend(hits)
        require(marker_lines == sorted(marker_lines), "h128 marker order mismatch")
        h128_records.append({"marker_lines": marker_lines, "named_rejections": 50})
require(len(io_records) == 2 and len(repeated_records) == 16 and len(h128_records) == 2,
        "semantic invocation counts mismatch")

api_targets = [f"{name}_api_contract_test" for name in ("relin2", "rs2", "mult2", "add", "sub")]
accepted_targets = [item["command"][0] for item in expected[57:60]]
production_library_built = built_target(lines, "openfhe_2023_1788") or any(
    re.fullmatch(r"\[\d+/\d+\] Linking CXX static library libopenfhe_2023_1788\.a", line) for line in lines)
require(production_library_built, "production library completion missing")
for target in api_targets + accepted_targets:
    require(built_target(lines, target), f"target completion missing: {target}")

paper = None
if HOST == "linux":
    require(paper_invocation is None and "BEGIN test=paper_full_eight_square_contract" not in text,
            "Linux paper runtime unexpectedly started")
    require(not built_target(lines, "paper_full_eight_square_contract_test"), "Linux paper target unexpectedly linked")
    build_group = [index + 1 for index, line in enumerate(lines)
                   if line == "##[group]Run cmake --build build --target paper_full_eight_square_contract_test --parallel 2"]
    process_exit = [index + 1 for index, line in enumerate(lines)
                    if line == "##[error]Process completed with exit code 2."]
    require(len(build_group) == len(process_exit) == 1 and build_group[0] < process_exit[0],
            "Linux paper build boundary/exit mismatch")
    build_lines = lines[build_group[0]:process_exit[0]]
    require(sum("Building CXX object CMakeFiles/paper_full_eight_square_contract_test.dir/" in line
                for line in build_lines) == 1, "Linux paper compilation command missing or duplicated")
    array_bounds = [index + 1 for index, line in enumerate(lines)
                    if "error: array subscript [24, 1152921504606846975] is outside array bounds" in line]
    require(len(array_bounds) == 2, "Linux expected two Boost array-bounds diagnostics")
    require("boost/multiprecision/cpp_bin_float.hpp" in text and "copy_and_round" in text
            and "unsigned int D = 512" in text and "unsigned int Digits = 1536" in text
            and "libboost1.83-dev:amd64 (1.83.0-2.1ubuntu3.2)" in text,
            "Linux Boost cpp_bin_float512-to-1536 diagnostic context missing")
    compiler_errors = [line for line in build_lines if ": error:" in line]
    require(len(compiler_errors) == 2 and all("[-Werror=array-bounds=]" in line for line in compiler_errors),
            "Linux paper build contains an additional compiler error")
    make_cascade = [line for line in build_lines if re.match(r"gmake(?:\[\d+\])?: \*\*\* .* Error [12]$", line)]
    require(len(make_cascade) == 4 and "cc1plus: all warnings being treated as errors" in build_lines,
            "Linux expected compiler/make cascade changed")
    paper = {
        "target_compiled": False,
        "runtime_processes": 0,
        "failure": "compile-time Boost 1.83 cpp_bin_float512-to-1536 copy_and_round -Werror=array-bounds",
        "primary_diagnostic_lines": array_bounds,
        "make_cascade_lines": [lines.index(line) + 1 for line in make_cascade],
    }
else:
    require(paper_invocation is not None and paper_invocation["ordinal"] == 1
            and paper_invocation["group_size"] == 1, "single paper CTest binding missing")
    require(built_target(lines, "paper_full_eight_square_contract_test"), "Windows paper target did not link")
    build_commands = [index + 1 for index, line in enumerate(lines)
                      if "--target paper_full_eight_square_contract_test --parallel 2" in line]
    object_lines = [index + 1 for index, line in enumerate(lines)
                    if "Building CXX object CMakeFiles/paper_full_eight_square_contract_test.dir/" in line]
    link_lines = [index + 1 for index, line in enumerate(lines)
                  if re.fullmatch(r"\[(\d+)/(\1)\] Linking CXX executable paper_full_eight_square_contract_test\.exe", line)]
    require(len(build_commands) == len(object_lines) == len(link_lines) == 1,
            "Windows paper build command/compile/link evidence not unique")
    require(invocations[-1]["result_line"] < build_commands[0] < object_lines[0] < link_lines[0]
            < listings[-1]["line"] < paper_invocation["start_line"],
            "Windows paper build/list/run ordering changed")
    output = [item["text"] for item in paper_invocation["output"]]
    prefixed = [line.split("61: ", 1)[1] for line in output if line.startswith("61: ")]
    begin = [i for i, line in enumerate(prefixed) if line.startswith("BEGIN test=paper_full_eight_square_contract ")]
    complete = [i for i, line in enumerate(prefixed) if line.startswith("COMPLETE test=paper_full_eight_square_contract ")]
    require(len(begin) == len(complete) == 1 and begin[0] < complete[0], "paper process markers missing/duplicated")
    actual_payload = prefixed[begin[0]:complete[0] + 1]
    require(f"source={SOURCE}" in actual_payload[0] and f"openfhe_pin={PIN}" in actual_payload[0]
            and "chain_count=1" in actual_payload[0], "paper BEGIN provenance/profile mismatch")
    failure_reason = "paper contract: round_4 independent anchor 2^-80 gate"
    require(actual_payload[-1].endswith("reason=" + failure_reason), "paper failure reason mismatch")

    # CTest prints failed output twice: first live with `61:`, then one unprefixed replay.
    result_index = paper_invocation["result_line"] - 1
    replay_end = next(i for i in range(result_index + 1, len(lines)) if lines[i] == "Errors while running CTest")
    replay = [line for line in lines[result_index + 1:replay_end] if line]
    require(replay == actual_payload, "unprefixed CTest replay differs from the one live process payload")
    require(text.count("BEGIN test=paper_full_eight_square_contract") == 2
            and text.count("COMPLETE test=paper_full_eight_square_contract") == 2,
            "paper live+replay marker count changed")

    fields = {}
    for line in actual_payload:
        match = re.fullmatch(r"OBS field=(\S+) value=(\S+)", line)
        if match:
            require(match[1] not in fields, f"paper field duplicated: {match[1]}")
            fields[match[1]] = match[2]
    exact_values = {
        "fresh.full_max_component_error": "3.380628068541526514072840073085484952608435963e-25",
        "fresh.codec_cross_precision": "4.327899444029515408578455222124729267038817044e-165",
        "fresh.expected_witness_real_difference": "2.646977960169688559588507814623881131410598755e-23",
        "fresh.actual_witness_real_difference": "2.643737400140613584155459658569040437880194418e-23",
        "fresh.anchor_max_component_error": "1.081632465294796426394472742886068841698281297e-25",
        "fresh.anchor_vs_production": "4.954005368621470060097734507696221211843730662e-102",
        "round_1.anchor_max_component_error": "2.148498437837611997103189667813446415046284698e-25",
        "round_2.anchor_max_component_error": "4.217839811308896046540254884767216034003689298e-25",
        "round_3.anchor_max_component_error": "8.144972185928475769270431936848113164028308842e-25",
        "round_4.anchor_257_error": "1.142170280496632008599576688979386875030880238e-24",
        "round_4.anchor_512_error": "1.519830543686982243577737227091582941391260472e-24",
        "round_4.anchor_768_error": "1.214424390898591942905150790287712338091263505e-24",
        "round_4.anchor_max_component_error": "1.519830543686982243577737227091582941391260472e-24",
    }
    require({key: fields.get(key) for key in exact_values} == exact_values, "paper retained exact values changed")
    anchor_suffixes = ["0", "1", "256", "257", "512", "513", "768", "769", "1023", "16383"]
    expected_field_names = {
        "fresh.full_max_component_error", "fresh.codec_cross_precision",
        "fresh.expected_witness_real_difference", "fresh.actual_witness_real_difference",
        "fresh.anchor_max_component_error", "fresh.anchor_vs_production",
        *(f"fresh.anchor_{slot}_error" for slot in anchor_suffixes),
        *(f"round_{round_}.anchor_{slot}_error" for round_ in range(1, 5) for slot in anchor_suffixes),
        *(f"round_{round_}.anchor_max_component_error" for round_ in range(1, 5)),
    }
    require(set(fields) == expected_field_names, "fresh/round-1-through-4 observed field inventory changed")
    gate = Fraction(1, 2**80)
    require(all(Fraction(fields[f"round_{round_}.anchor_max_component_error"]) <= gate for round_ in (1, 2, 3)),
            "round 1-3 anchor gate relation changed")
    require(Fraction(fields["round_4.anchor_max_component_error"]) > gate, "round 4 no longer exceeds gate")
    receipts = [int(match[1]) for line in actual_payload
                if (match := re.match(r"RECEIPT operation=(\d+) ", line))]
    require(receipts == [0, 1, 2, 3, 4], "observed receipt prefix changed")
    require(sum(line.startswith("PROFILE family=") for line in actual_payload) == 60,
            "paper family/tower profile count changed")
    require([line for line in actual_payload if line.startswith("OBS rejection=")] == [
        "OBS rejection=nonterminal_input result=PASS",
        "OBS rejection=nonterminal_first_square result=PASS",
    ], "paper preterminal rejection evidence changed")

    paper_source = git_show(FROZEN, "tests/paper_full_eight_square_contract_test.cpp")
    require("const auto evaluation=Evaluate(setup.plan,source);" in paper_source
            and "for (std::size_t round=1;round<=8;++round)" in paper_source
            and paper_source.index("const auto evaluation=Evaluate(setup.plan,source);") <
                paper_source.index("for (std::size_t round=1;round<=8;++round) {", paper_source.index("const auto evaluation=Evaluate")),
            "paper source no longer proves Evaluate completes before per-round checks")
    require("for (std::size_t round=1;round<=8;++round) {\n        pair=evaluator.Mult2(pair,pair);" in paper_source,
            "paper source no longer encodes eight-square evaluation loop")
    paper = {
        "target_compiled": True,
        "target_build_command_line": build_commands[0],
        "target_object_line": object_lines[0],
        "target_link_line": link_lines[0],
        "runtime_processes": 1,
        "ctest_replay_copies": 1,
        "full_eight_square_chain_executed": True,
        "chain_basis": "Evaluate returns only after its 1..8 Mult2 loop; round-4 checks occur afterward",
        "live_payload_lines": len(actual_payload),
        "observed_receipt_operations": receipts,
        "exact_values": exact_values,
        "observed_fields": fields,
        "gate_2^-80_decimal_approx": "8.271806125530276748714086920699628535658121109e-25",
        "rounds_1_to_3_gate": "PASS",
        "round_4_gate": "FAIL",
        "failure_reason": failure_reason,
        "unreached_after_round_4_failure": [
            "round_5 through round_8 per-round receipt/pair/anchor checks and outputs",
            "fresh source ciphertext immutability check",
            "root secret and public key immutability check",
            "terminal result receipt/ciphertext check",
            "BindRepeatedRcb and mutable-clone isolation check",
            "final client decrypt/full-slot/sub-binary64-witness checks",
            "terminal sparse decrypt/two-base modulus/final-anchor checks",
            "wrong-normalization rejection, scalar anchors, and nonzero-domain checks",
            "foreign issuer/client rejection checks",
            "final family/key-row recheck and paper-owner cleanup checks",
            "COMPLETE result=PASS lifecycle marker",
        ],
    }

bindings = []
for invocation in invocations:
    bindings.append({key: value for key, value in invocation.items() if key != "output"})
result = {
    "audit": "independent retained first-production-run parser",
    "host": HOST,
    "repository_head": HEAD,
    "production_source": SOURCE,
    "frozen_tests_source": FROZEN,
    "openfhe_pin": PIN,
    "run_id": RUN,
    "run_attempt": int(ATTEMPT),
    "job_id": cfg["job"],
    "log": str(log_path),
    "bytes": len(raw),
    "sha256": hashlib.sha256(raw).hexdigest(),
    "connector_capture": str(capture_path),
    "connector_decoded_bytes": len(connector),
    "connector_decoded_sha256": hashlib.sha256(connector).hexdigest(),
    "connector_transform": "identity" if HOST == "linux" else "CRLF-to-LF only; 7649 pairs",
    "terminal": {"status": job["status"], "conclusion": job["conclusion"],
                 "completed_at": job["completedAt"], "sole_failed_step": failed_steps[0]["name"]},
    "status": "PASS_EVIDENCE_AUDIT_WITH_EXPECTED_FAILURE",
    "live_listings": listings,
    "accepted_pass_count": len(bindings),
    "execution_partition": [1, 2, 57, 1, 2, 60],
    "actual_pass_bindings": bindings,
    "production_library_built": production_library_built,
    "api_targets_built": api_targets,
    "accepted_contract_targets_built": accepted_targets,
    "io_numeric_records": io_records,
    "repeated_stage_records": repeated_records,
    "h128_invocations": h128_records,
    "paper": paper,
    "checks_not_run_by_this_parser": ["compiler", "FHE runtime", "FFT/NTT", "CI", "browser"],
}
print(json.dumps(result, indent=2, sort_keys=True))
