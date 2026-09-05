#!/usr/bin/env python3
"""Read-only audit of retained Linux/Windows signed-diagnostic run evidence."""
import hashlib
import json
import re
import shlex
import subprocess
import sys
from fractions import Fraction
from math import gcd
from pathlib import Path

ROOT = Path("/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905")
HERE = ROOT / "artifacts/handoffs/paper-scale-signed-diagnostic-run-01"
HEAD = "a33d5f1fcfeb51cd63021b1ec1d24d6e76187a1c"
SOURCE = "9f6c8eae06afb342dfa8c8efff9f64ee45b2ab8e"
FROZEN = "448e9d3067796b64656f0746f4be5c4153b1271d"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
RUN = "33978202814"
ATTEMPT = 1
HOST = (sys.argv[1] if len(sys.argv) > 1 else "linux").lower()

HOSTS = {
    "linux": {
        "job": "101338538686",
        "job_name": "linux-gcc",
        "log": "LINUX_RAW.log",
        "capture": "LINUX_CONNECTOR_CAPTURE.json",
        "bytes": 906589,
        "sha": "44eb5102695106bbe80a693ce4028b4160923f521d683c2d6af293bfc76040fa",
        "capture_file_sha": "500e689b54a340ac947ca68d8c77d314a895a20628352ce16fc92acf270f2cc8",
        "connector_bytes": 906589,
        "connector_sha": "44eb5102695106bbe80a693ce4028b4160923f521d683c2d6af293bfc76040fa",
        "crlf": 0,
        "completed": "2026-09-05T16:39:49Z",
        "failed_step": "Run paper full eight-square contract once",
        "image": "ubuntu-24.04",
        "image_version": "20260831.293.1",
        "cmake": "3.31.6",
        "compiler": "c++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0",
    },
    "windows": {
        "job": "101338538587",
        "job_name": "windows-mingw64",
        "log": "WINDOWS_LF.log",
        "capture": "WINDOWS_CONNECTOR_CAPTURE.json",
        "bytes": 915610,
        "sha": "9ff1e0f25faafac5b57da07519b8f81b2a8b765f30ecec9e1c74d838395c1dea",
        "capture_file_sha": "afb5ef78b55bedf91183f15e99c4c09186da47cc06040f8c4c473f048d040210",
        "connector_bytes": 924947,
        "connector_sha": "7f781be87251a85da24a8a51ff25b0176f3215789718f528d6472c49a89b27f8",
        "crlf": 9337,
        "completed": "2026-09-05T16:41:43Z",
        "failed_step": "Run paper full eight-square contract once",
        "image": "windows-2022",
        "cmake": "4.4.2",
        "compiler": "g++.exe (Rev3, Built by MSYS2 project) 16.2.0",
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


def line_hits(lines, predicate):
    return [index + 1 for index, line in enumerate(lines) if predicate(line)]


def unique_line(lines, predicate, label):
    hits = line_hits(lines, predicate)
    require(len(hits) == 1, f"{label} missing or duplicated: {hits}")
    return hits[0]


def built_target(lines, target):
    return line_hits(lines, lambda line: bool(
        re.fullmatch(r"\[\s*\d+%\] Built target " + re.escape(target), line)
        or re.fullmatch(r"\[(\d+)/(\1)\] Linking CXX executable " + re.escape(target) + r"\.exe", line)
    ))


require(HOST in HOSTS, "usage: verify_signed_run.py [linux|windows]")
cfg = HOSTS[HOST]

# Repository and frozen inventory authority.
require(git("rev-parse", "HEAD").strip() == HEAD, "repository HEAD changed")
require(not git("status", "--short").strip(), "tracked/untracked repository state is not clean")
require(not git("diff", "--name-only", SOURCE, HEAD, "--", ".github/workflows", "CMakeLists.txt", "include", "src", "tests").strip(),
        "HEAD contains code/test/build/workflow changes after tested source")
require(not git("diff", "--name-only", FROZEN, SOURCE, "--", "CMakeLists.txt", ".github/workflows/dcp-rcb.yml").strip(),
        "frozen CMake/workflow inventory changed")
changed_tests = git("diff", "--name-only", FROZEN, SOURCE, "--", "tests").splitlines()
require(changed_tests == [
    "tests/paper_full_eight_square_contract_test.cpp",
    "tests/paper_full_eight_square_oracle.h",
], "unexpected frozen-test changes")

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

# Byte identity: connector strings are authoritative decoded content, not HTTP bytes.
log_path = HERE / cfg["log"]
capture_path = HERE / cfg["capture"]
raw = log_path.read_bytes()
require(len(raw) == cfg["bytes"] and hashlib.sha256(raw).hexdigest() == cfg["sha"],
        "retained LF log byte identity changed")
require(raw.startswith(b"\xef\xbb\xbf") and b"\r\n" not in raw,
        "retained log is not UTF-8-BOM LF-only")
capture_file = capture_path.read_bytes()
require(hashlib.sha256(capture_file).hexdigest() == cfg["capture_file_sha"],
        "connector capture file identity changed")
capture = json.loads(capture_file.decode("utf-8"))
require(str(capture.get("run_id")) == RUN and str(capture.get("job_id")) == cfg["job"]
        and capture.get("source_commit") == SOURCE
        and "not HTTP transport bytes" in capture.get("representation", ""),
        "connector capture provenance/representation mismatch")
connector = capture.get("content", "").encode("utf-8")
require(len(connector) == cfg["connector_bytes"] and hashlib.sha256(connector).hexdigest() == cfg["connector_sha"],
        "connector decoded byte identity changed")
require(connector.startswith(b"\xef\xbb\xbf") and connector.count(b"\r\n") == cfg["crlf"],
        "connector BOM/newline structure changed")
normalized_connector = connector if HOST == "linux" else connector.replace(b"\r\n", b"\n")
require(normalized_connector == raw, "retained log is not the exact connector newline transform")

# Terminal metadata and exact failed step.
terminal_path = HERE / "RUN_TERMINAL_01.json"
terminal_file = terminal_path.read_bytes()
terminal = json.loads(terminal_file.decode("utf-8"))
run = terminal["run"]
require(run["id"] == int(RUN) and run["run_attempt"] == ATTEMPT and run["head_sha"] == SOURCE
        and run["status"] == "completed" and run["conclusion"] == "failure"
        and run["event"] == "push" and run["updated_at"] == "2026-09-05T16:41:43Z",
        "run terminal metadata mismatch")
jobs = [item for item in terminal["jobs"] if item["databaseId"] == int(cfg["job"])]
require(len(jobs) == 1, "terminal job missing or duplicated")
job = jobs[0]
require(job["name"] == cfg["job_name"] and job["status"] == "completed"
        and job["conclusion"] == "failure" and job["completedAt"] == cfg["completed"],
        "terminal job status/name/time mismatch")
failed_steps = [step for step in job["steps"] if step["conclusion"] == "failure"]
require(len(failed_steps) == 1 and failed_steps[0]["name"] == cfg["failed_step"],
        "unexpected terminal failed-step set")
other_non_success = [step for step in job["steps"]
                     if step not in failed_steps and step["conclusion"] != "success"]
expected_skips = ([{"status":"completed", "conclusion":"skipped", "name":"Post Cache pristine OpenFHE install"}]
                  if HOST == "linux" else [])
require([{key: step[key] for key in ("status", "conclusion", "name")} for step in other_non_success] == expected_skips,
        "terminal contains an unexpected non-success step outside the paper failure")

decoded = raw.decode("utf-8")
lines = [re.sub(r"^\ufeff?\d{4}-\d\d-\d\dT\S+Z ?", "", line) for line in decoded.splitlines()]
text = "\n".join(lines)

# Runner, source, toolchain, exact upstream and real OpenFHE build/install evidence.
require(text.count(f"Image: {cfg['image']}") == 1, "runner image missing/duplicated")
if HOST == "linux":
    require(text.count(f"Version: {cfg['image_version']}") == 1, "Linux image version mismatch")
require(text.count(f"cmake version {cfg['cmake']}") == 1, "actual CMake version mismatch")
require(text.count(cfg["compiler"]) == 1, "actual compiler version mismatch")
provenance = f"PROJECT_SOURCE_COMMIT={SOURCE} GITHUB_RUN_ID={RUN} GITHUB_RUN_ATTEMPT={ATTEMPT}"
require(text.count(provenance) == 1, "source/run/attempt provenance missing or duplicated")
require(text.count(f"HEAD is now at {PIN[:7]}") == 1, "exact OpenFHE checkout evidence missing or duplicated")
for marker in (
    "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_UNITTESTS=OFF", "-DBUILD_EXAMPLES=OFF",
    "-DBUILD_BENCHMARKS=OFF", "-DBUILD_EXTRAS=OFF", "-DWITH_OPENMP=ON",
    "-DNATIVE_SIZE=64", "-DMATHBACKEND=4", "-- NATIVEINT is set to 64",
    "-- MATHBACKEND is set to 4", "-DCMAKE_BUILD_TYPE=Debug", "-DCMAKE_PREFIX_PATH=",
):
    require(marker in text, f"configure evidence missing: {marker}")
require("cmake --install" in text and "OpenFHEConfigVersion.cmake" in text
        and "OpenFHETargets-release.cmake" in text,
        "OpenFHE build/install completion evidence missing")
cache = {"policy": "not configured on Windows", "result": "fresh build"}
if HOST == "linux":
    cache_key = f"openfhe-1.5.0-Linux-gcc-{PIN}-native64-backend4"
    require(text.count(f"key: {cache_key}") == 1
            and text.count(f"Cache not found for input keys: {cache_key}") == 1
            and "Cache restored" not in text,
            "Linux cache key/miss evidence mismatch")
    cache = {"key": cache_key, "result": "miss; OpenFHE built and installed in this job"}

# Decode every live CTest JSON block and bind it to frozen name/order/argv/backtrace.
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
            require(normalize_command(actual["command"]) == want["command"], f"live argv mismatch: {want['name']}")
            node = graph["nodes"][actual["backtrace"]]
            require(graph["commands"][node["command"]] == "add_test" and node["line"] == want["line"],
                    f"live add_test backtrace mismatch: {want['name']}")
            require(graph["files"][node["file"]].replace("\\", "/").endswith("CMakeLists.txt"),
                    "live backtrace file mismatch")
        listings.append({"line": index + 1, "count": count, "frozen_name_order_argv_backtrace": True})
require([item["count"] for item in listings] == [57, 60, 61], "live inventory sequence mismatch")

# Bind every Start to exactly one printed argv and one result.
active = None
accepted = []
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
                f"executed argv differs from frozen CMake: {active['name']}")
        active["command"] = argv
        active["command_line"] = index + 1
    elif passed or failed:
        end = passed or failed
        require(active is not None and active["command"] is not None, "result has no Start/command binding")
        require(int(end[3]) == active["number"] and end[4] == active["name"], "result identity mismatch")
        active.update(result="Passed" if passed else "Failed", result_line=index + 1,
                      ordinal=int(end[1]), group_size=int(end[2]), seconds=end[5])
        if active["number"] == 61:
            require(failed is not None and paper_invocation is None, "paper result not one unique failure")
            paper_invocation = active
        else:
            require(passed is not None, f"accepted invocation failed: {active['name']}")
            accepted.append(active)
        active = None
    elif active is not None:
        active["output"].append({"line": index + 1, "text": line})
require(active is None and paper_invocation is not None and len(accepted) == 123,
        "expected 123 accepted passes plus one paper failure")
groups = [[expected[2]], expected[55:57], expected[:57], [expected[57]], expected[58:60], expected[:60]]
offset = 0
group_summary = []
for group in groups:
    actual = accepted[offset:offset + len(group)]
    require([item["name"] for item in actual] == [item["name"] for item in group], "pass group order mismatch")
    require([item["ordinal"] for item in actual] == list(range(1, len(group) + 1)), "pass group ordinal mismatch")
    require(all(item["group_size"] == len(group) for item in actual), "pass group denominator mismatch")
    group_summary.append({"size": len(group), "first": group[0]["name"], "last": group[-1]["name"]})
    offset += len(group)
require(offset == 123, "pass partition mismatch")

# High-value accepted-test output remains bound to the corresponding CTest process.
io_records, repeated_records, h128_records = [], [], []
for invocation in accepted:
    output = invocation["output"]
    if invocation["number"] == 58:
        hits = [item for item in output if f"test={expected[57]['name']} result=PASS" in item["text"]]
        require(len(hits) == 1, "I/O numeric PASS missing or duplicated")
        record = dict(re.findall(r"(\w+)=([^\s]+)", hits[0]["text"]))
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
        io_records.append({"line": hits[0]["line"], **record})
    elif invocation["number"] == 59:
        records = [json.loads(item["text"].split(": ", 1)[1]) for item in output
                   if item["text"].startswith('59: {"test":')]
        require([(item["trial"], item["stage"]) for item in records] ==
                [(trial, stage) for trial in range(4) for stage in (1, 2)], "repeated trial/stage matrix mismatch")
        for record in records:
            d, m1, m2 = int(record["d"]), int(record["m1"]), int(record["m2"])
            require((d, m1, m2) == (1125899906843009, 1125899906840833, 1125899906844161),
                    "repeated divisor/moduli mismatch")
            wanted = Fraction(2**200, d*m1) if record["stage"] == 1 else Fraction(2**400, d*d*d*m1*m1*m2)
            require(Fraction(int(record["scale_numerator"]), int(record["scale_denominator"])) == wanted,
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

# Production, five public API, three accepted-contract and paper build completion.
api_targets = [f"{name}_api_contract_test" for name in ("relin2", "rs2", "mult2", "add", "sub")]
accepted_targets = [item["command"][0] for item in expected[57:60]]
target_lines = {}
production_lines = built_target(lines, "openfhe_2023_1788") or line_hits(
    lines, lambda line: bool(re.fullmatch(r"\[\d+/\d+\] Linking CXX static library libopenfhe_2023_1788\.a", line)))
require(production_lines, "production library completion missing")
for target in api_targets + accepted_targets:
    target_lines[target] = built_target(lines, target)
    require(target_lines[target], f"target completion missing: {target}")

if HOST == "linux":
    build_commands = line_hits(lines, lambda line: line ==
                               "##[group]Run cmake --build build --target paper_full_eight_square_contract_test --parallel 2")
else:
    build_commands = line_hits(lines, lambda line: line.startswith("\x1b[36;1mcmake --build ")
                               and "--target paper_full_eight_square_contract_test --parallel 2" in line)
object_lines = line_hits(lines, lambda line: "Building CXX object CMakeFiles/paper_full_eight_square_contract_test.dir/" in line)
link_lines = line_hits(lines, lambda line: bool(re.fullmatch(
    r"(?:\[100%\]|\[2/2\]) Linking CXX executable paper_full_eight_square_contract_test(?:\.exe)?", line)))
paper_built = built_target(lines, "paper_full_eight_square_contract_test")
require(len(build_commands) == len(object_lines) == len(link_lines) == 1,
        "paper build command/object/link evidence not unique")
if HOST == "linux":
    require(len(paper_built) == 1, "Linux paper Built-target completion missing/duplicated")
require(accepted[-1]["result_line"] < build_commands[0] < object_lines[0] < link_lines[0]
        < listings[-1]["line"] < paper_invocation["start_line"],
        "paper build/list/run ordering changed")
require(paper_invocation["ordinal"] == 1 and paper_invocation["group_size"] == 1,
        "paper CTest must be a single-test invocation")

# One process payload plus CTest's exact replay; replay is evidence, not a second run.
output = [item["text"] for item in paper_invocation["output"]]
prefixed = [line.split("61: ", 1)[1] for line in output if line.startswith("61: ")]
begin = [index for index, line in enumerate(prefixed) if line.startswith("BEGIN test=paper_full_eight_square_contract ")]
complete = [index for index, line in enumerate(prefixed) if line.startswith("COMPLETE test=paper_full_eight_square_contract ")]
require(len(begin) == len(complete) == 1 and begin[0] < complete[0], "paper process markers missing/duplicated")
payload = prefixed[begin[0]:complete[0] + 1]
begin_tokens = dict(re.findall(r"(\w+)=([^\s]+)", payload[0]))
require(begin_tokens == {
    "test":"paper_full_eight_square_contract", "source":SOURCE, "openfhe_pin":PIN,
    "native":"64", "backend":"4", "N":"32768", "M":"65536", "slots":"16384", "gap":"1",
    "h":"128", "nominal":"50", "P":"1152921504606584833", "P_root":"4443670208963",
    "chain_count":"1",
}, "paper BEGIN provenance/profile mismatch")
reason = "paper contract: accumulated numeric acceptance failures: 7"
require(payload[-1] == f"COMPLETE test=paper_full_eight_square_contract result=FAIL source={SOURCE} "
        f"openfhe_pin={PIN} reason={reason}", "paper failure reason mismatch")
result_index = paper_invocation["result_line"] - 1
replay_end = next(index for index in range(result_index + 1, len(lines))
                  if re.fullmatch(r"0% tests passed, 1 tests failed out of 1", lines[index]))
replay_region = lines[result_index + 1:replay_end]
require(replay_region.count("Errors while running CTest") == 1,
        "CTest error marker missing/duplicated in replay region")
replay = [line for line in replay_region if line and line != "Errors while running CTest"]
require(replay == payload, "unprefixed CTest replay differs from the live process payload")
require(text.count("BEGIN test=paper_full_eight_square_contract") == 2
        and text.count("COMPLETE test=paper_full_eight_square_contract") == 2,
        "paper live+replay marker count changed")

# Every one of the 835 source-defined fields must occur exactly once and in order.
anchors = ["0", "1", "256", "257", "512", "513", "768", "769", "1023", "16383"]
field_order = [
    "fresh.full_max_component_error", "fresh.codec_cross_precision",
    "fresh.expected_witness_real_difference", "fresh.actual_witness_real_difference",
    "diag.fresh.coefficient_max_over_scale",
    *(f"fresh.anchor_{anchor}_error" for anchor in anchors),
    "fresh.anchor_max_component_error", "fresh.anchor_vs_production",
]
for anchor in anchors:
    for part in ("w0", "E"):
        for component in ("real", "imag"):
            field_order.append(f"diag.fresh.anchor_{anchor}.{part}.{component}")
for round_ in range(1, 9):
    field_order.append(f"diag.round_{round_}.coefficient_max_over_scale")
    for anchor in anchors:
        for part in ("E", "I", "A", "L"):
            for component in ("real", "imag"):
                field_order.append(f"diag.round_{round_}.anchor_{anchor}.{part}.{component}")
    field_order.extend([
        f"diag.round_{round_}.I_anchor_max_component",
        f"diag.round_{round_}.A_anchor_max_component",
        f"diag.round_{round_}.L_anchor_max_component",
        *(f"round_{round_}.anchor_{anchor}_error" for anchor in anchors),
        f"round_{round_}.anchor_max_component_error",
    ])
field_order.extend([
    "final.full_max_component_error", "final.codec_cross_precision",
    "final.expected_witness_real_difference", "final.actual_witness_real_difference",
    "diag.final.coefficient_max_over_scale",
    *(f"final.anchor_{anchor}_error" for anchor in anchors),
    "final.anchor_max_component_error", "final.anchor_vs_production", "final.wrong_nominal100_error",
])
field_pairs = []
for line in payload:
    match = re.fullmatch(r"OBS field=(\S+) value=(\S+)", line)
    if match:
        try:
            value = Fraction(match[2])
        except (ValueError, ZeroDivisionError) as error:
            raise RuntimeError(f"non-finite/non-rational paper field {match[1]}") from error
        field_pairs.append((match[1], match[2], value))
require(len(field_order) == 835 and [item[0] for item in field_pairs] == field_order,
        "paper 835-field inventory/order/uniqueness mismatch")
fields = {name: value for name, value, _ in field_pairs}
numbers = {name: fraction for name, _, fraction in field_pairs}

# Derive and bind both complete 60-line profile passes from the tested source constants.
oracle_source = git_show(SOURCE, "tests/paper_full_eight_square_oracle.h")
q_body = re.search(r"constexpr std::array<std::uint64_t, 11> kQ\{\{(.*?)\}\};", oracle_source, re.S)[1]
root_body = re.search(r"constexpr std::array<std::uint64_t, 11> kRoots\{\{(.*?)\}\};", oracle_source, re.S)[1]
q_values = [1099510054913 if token == "kDiv" else int(token.removesuffix("ULL"))
            for token in re.findall(r"\d+ULL|kDiv", q_body)]
root_values = [int(token) for token in re.findall(r"\d+", root_body)]
require(len(q_values) == len(root_values) == 11, "could not derive Q/root arrays from tested oracle source")
profile_once = []
for family in range(8):
    size = 11 - family
    for tower in range(size):
        source_index = 10 if tower + 1 == size else tower
        profile_once.append(f"PROFILE family={family} tower={tower} q={q_values[source_index]} root={root_values[source_index]}")
profiles = [line for line in payload if line.startswith("PROFILE family=")]
require(len(profile_once) == 60 and profiles == profile_once + profile_once,
        "two complete family/tower profiles differ from source-derived inventory")

# Receipts: exact closed-form rational scales, family/level/tower/terminal state.
receipt_lines = [line for line in payload if line.startswith("RECEIPT operation=")]
require(len(receipt_lines) == 9, "receipt count changed")
receipts = []
q_div = q_values[10]
scales = [(2**100, 1)]
for round_ in range(1, 9):
    denominator = 1
    for j in range(1, round_ + 1):
        factor = q_div * q_values[10 - j]
        for _ in range(round_ - j):
            factor *= factor
        denominator *= factor
    numerator = 2 ** (100 * (1 << round_))
    common = gcd(numerator, denominator)
    scales.append((numerator // common, denominator // common))
for operation, line in enumerate(receipt_lines):
    record = dict(re.findall(r"(\w+)=([^\s]+)", line))
    expected_receipt = {
        "operation":str(operation), "family":str(7 if operation == 8 else operation),
        "local_level":str(2 if operation == 8 else 1), "towers":str(10-operation),
        "recorded_exp2":"100", "degree":"2", "exact_n":str(scales[operation][0]),
        "exact_d":str(scales[operation][1]), "terminal":str(int(operation == 8)),
    }
    require(record == expected_receipt, f"receipt {operation} state/exact scale mismatch")
    receipts.append(record)

# Validate the actual numeric relation that produced each accumulated miss.
gate80, gate120 = Fraction(1, 2**80), Fraction(1, 2**120)
require(0 <= numbers["fresh.full_max_component_error"] <= gate80, "fresh full-slot gate relation changed")
require(0 <= numbers["fresh.codec_cross_precision"] <= gate120, "fresh codec gate relation changed")
require(numbers["fresh.actual_witness_real_difference"] > Fraction(1, 2**76)
        and abs(numbers["fresh.actual_witness_real_difference"] - numbers["fresh.expected_witness_real_difference"]) <= 2*gate80,
        "fresh sub-binary64 witness gate relation changed")
require(numbers["fresh.anchor_max_component_error"] <= gate80
        and numbers["fresh.anchor_vs_production"] <= gate80, "fresh anchor gate relation changed")
for round_ in range(1, 9):
    relation = numbers[f"round_{round_}.anchor_max_component_error"] <= gate80
    require(relation == (round_ <= 3), f"round {round_} anchor gate relation changed")
require(numbers["final.full_max_component_error"] > gate80, "final full-slot miss relation changed")
require(0 <= numbers["final.codec_cross_precision"] <= gate120, "final codec gate relation changed")
require(numbers["final.actual_witness_real_difference"] > Fraction(1, 2**76)
        and abs(numbers["final.actual_witness_real_difference"] - numbers["final.expected_witness_real_difference"]) <= 2*gate80,
        "final sub-binary64 witness gate relation changed")
require(numbers["final.anchor_max_component_error"] > gate80
        and numbers["final.anchor_vs_production"] <= gate80, "final anchor relation changed")
require(numbers["final.wrong_nominal100_error"] > Fraction(1, 2**30), "wrong-normalization relation changed")
gate_failures = [line for line in payload if line.startswith("OBS numeric_gate=FAIL label=")]
expected_failures = [
    *(f"OBS numeric_gate=FAIL label=round_{round_} independent anchor 2^-80 gate" for round_ in range(4, 9)),
    "OBS numeric_gate=FAIL label=final full-slot 2^-80 gate",
    "OBS numeric_gate=FAIL label=final independent anchor 2^-80 gate",
]
require(gate_failures == expected_failures, "seven accumulated numeric failure labels/order changed")
require([line for line in payload if line.startswith("OBS numeric_gate_failures=")] ==
        ["OBS numeric_gate_failures=7"], "numeric failure total missing/duplicated")

rejections = [line for line in payload if line.startswith("OBS rejection=")]
require(rejections == [
    "OBS rejection=nonterminal_input result=PASS",
    "OBS rejection=nonterminal_first_square result=PASS",
    "OBS rejection=foreign_issuer result=PASS",
    "OBS rejection=foreign_client_binding result=PASS",
], "paper rejection set/order changed")
cleanup = "OBS lifecycle=paper_owner_cleanup owned_absent=8 unrelated_unchanged=2 result=PASS"
require(payload.count(cleanup) == 1, "paper ownership cleanup evidence missing/duplicated")

# Source ordering turns late output into reachability evidence without rerunning FHE locally.
paper_source = git_show(SOURCE, "tests/paper_full_eight_square_contract_test.cpp")
ordered_source_markers = [
    "const auto evaluation=Evaluate(setup.plan,source);",
    "for (std::size_t round=1;round<=8;++round) {",
    "const auto bound=client.BindRepeatedRcb(evaluation.result);",
    "const auto decoded=client.Decrypt(setup.rootSecret,bound);",
    "CheckFull(decoded,expected,\"final\",failures); Witness(decoded,expected,\"final\",failures);",
    "ForeignRejections(evaluation,foreign);",
    "CheckFamilies(setup.plan);",
    "OBS lifecycle=paper_owner_cleanup",
    "Require(numericFailures==0,\"accumulated numeric acceptance failures: \"",
]
positions = []
cursor = 0
for marker in ordered_source_markers:
    position = paper_source.index(marker, cursor)
    positions.append(position)
    cursor = position + len(marker)
require("for (std::size_t i=0;i<kSlots;++i)" in oracle_source
        and "PrecisionGate(maximum<=Pow2(-80),label+\" full-slot 2^-80 gate\",failures);" in oracle_source,
        "full-slot source contract changed")
require("if (!condition) {\n        ++failures;" in oracle_source
        and "Finite(z.real,field+\".real\")" in oracle_source,
        "finite/accumulated-miss diagnostic seam changed")

selected_names = [
    "fresh.full_max_component_error", "fresh.codec_cross_precision",
    "fresh.expected_witness_real_difference", "fresh.actual_witness_real_difference",
    "fresh.anchor_max_component_error", "fresh.anchor_vs_production",
    *(f"round_{round_}.anchor_max_component_error" for round_ in range(1, 9)),
    "final.full_max_component_error", "final.codec_cross_precision",
    "final.expected_witness_real_difference", "final.actual_witness_real_difference",
    "final.anchor_max_component_error", "final.anchor_vs_production", "final.wrong_nominal100_error",
]
bindings = [{key: value for key, value in invocation.items() if key != "output"} for invocation in accepted]
payload_bytes = ("\n".join(payload) + "\n").encode("utf-8")
field_bytes = ("\n".join(f"{name}={value}" for name, value, _ in field_pairs) + "\n").encode("utf-8")
result = {
    "audit": "independent retained signed-diagnostic terminal-run parser",
    "status": "PASS_EVIDENCE_AUDIT_WITH_OBSERVED_NUMERIC_FAILURE",
    "host": HOST,
    "repository_head": HEAD,
    "tested_source": SOURCE,
    "frozen_inventory_source": FROZEN,
    "openfhe_pin": PIN,
    "run_id": RUN,
    "run_attempt": ATTEMPT,
    "job_id": cfg["job"],
    "terminal": {
        "artifact": str(terminal_path), "artifact_sha256": hashlib.sha256(terminal_file).hexdigest(),
        "run_status": run["status"], "run_conclusion": run["conclusion"],
        "job_status": job["status"], "job_conclusion": job["conclusion"],
        "completed_at": job["completedAt"], "sole_failed_step": failed_steps[0]["name"],
        "expected_skipped_post_steps":[step["name"] for step in other_non_success],
    },
    "log": str(log_path), "bytes": len(raw), "sha256": hashlib.sha256(raw).hexdigest(),
    "connector_capture": str(capture_path), "connector_capture_file_sha256": hashlib.sha256(capture_file).hexdigest(),
    "connector_decoded_bytes": len(connector), "connector_decoded_sha256": hashlib.sha256(connector).hexdigest(),
    "connector_crlf_pairs": connector.count(b"\r\n"),
    "connector_transform": "identity" if HOST == "linux" else "CRLF-to-LF only",
    "runner": {"image":cfg["image"], "image_version":cfg.get("image_version"),
               "cmake":cfg["cmake"], "compiler":cfg["compiler"]},
    "openfhe_build": {"pin":PIN, "native":64, "backend":4, "cache":cache, "build_and_install":"observed"},
    "live_listings": listings,
    "accepted_pass_count": len(bindings),
    "execution_partition": [1, 2, 57, 1, 2, 60],
    "execution_groups": group_summary,
    "actual_pass_bindings": bindings,
    "production_library_build_lines": production_lines,
    "api_target_build_lines": {target: target_lines[target] for target in api_targets},
    "accepted_contract_build_lines": {target: target_lines[target] for target in accepted_targets},
    "io_numeric_records": io_records,
    "repeated_stage_records": repeated_records,
    "h128_invocations": h128_records,
    "paper": {
        "target_build_command_line": build_commands[0], "target_object_line": object_lines[0],
        "target_link_line": link_lines[0], "runtime_processes":1, "ctest_replay_copies":1,
        "result_line":paper_invocation["result_line"], "seconds":paper_invocation["seconds"],
        "live_payload_lines":len(payload), "live_payload_sha256":hashlib.sha256(payload_bytes).hexdigest(),
        "field_count":len(field_pairs), "field_inventory_order_match":True,
        "field_value_stream_sha256":hashlib.sha256(field_bytes).hexdigest(),
        "field_group_counts":{"fresh":57, "round_each":95, "round_total":760, "final":18},
        "selected_actual_values":{name:fields[name] for name in selected_names},
        "profile_lines":len(profiles), "profile_passes":2, "source_derived_profile_match":True,
        "receipts":receipts, "gate_2^-80":str(gate80), "numeric_failure_count":7,
        "numeric_failure_labels":[line.removeprefix("OBS numeric_gate=FAIL label=") for line in gate_failures],
        "rejections":[line.removeprefix("OBS rejection=").removesuffix(" result=PASS") for line in rejections],
        "final_binder_decrypt_fullslot_witness_reached":True,
        "foreign_reachability_and_owner_cleanup_reached":True,
        "failure_reason":reason,
    },
    "checks_not_run_by_this_parser":["compiler", "FHE runtime", "FFT/NTT", "CI", "browser", "network"],
}
print(json.dumps(result, indent=2, sort_keys=True))
