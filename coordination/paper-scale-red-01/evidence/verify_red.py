#!/usr/bin/env python3
"""Strict retained-log audit for either exact hosted paper-scale API RED job."""
import hashlib
import json
import re
import shlex
import subprocess
import sys
from fractions import Fraction
from pathlib import Path

SOURCE = "448e9d3067796b64656f0746f4be5c4153b1271d"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
RUN = "33968576607"
ATTEMPT = "1"
ROOT = Path("/Users/lifeng/Documents/20231788-openfhe-paper-scale-implementation-20260905")
HERE = ROOT / "artifacts/handoffs/paper-scale-red-01"
HOST = (sys.argv[1] if len(sys.argv) > 1 else "linux").lower()
require_host = HOST in ("linux", "windows")
if not require_host:
    raise SystemExit("usage: verify_red.py [linux|windows]")
if HOST == "linux":
    JOB = "101312943097"
    LOG = HERE / "linux-job-101312943097.log"
    CAPTURE = HERE / "LINUX_CONNECTOR_CAPTURE.json"
    LOG_BYTES = 466976
    LOG_SHA = "bf6343f35f61b53905cdc9764392c56a04c96e059dc86900de0808954f799595"
    CONNECTOR_BYTES = 466976
    CONNECTOR_SHA = LOG_SHA
    COMPLETED_AT = "2026-09-05T13:26:01Z"
    PROCESS_EXIT = 2
else:
    JOB = "101312942973"
    LOG = HERE / "windows-job-101312942973-lf.log"
    CAPTURE = HERE / "WINDOWS_CONNECTOR_CAPTURE.json"
    LOG_BYTES = 474930
    LOG_SHA = "0fa3ece49d8a3205993b231570a213eb11d6582bb71b02b6d576b56a98e35199"
    CONNECTOR_BYTES = 480879
    CONNECTOR_SHA = "10a7f2a19a335979206f481b4223f1e0fb474cc9d43d616b1811363cb575bd05"
    COMPLETED_AT = "2026-09-05T13:30:28Z"
    PROCESS_EXIT = 1


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def git_show(path):
    return subprocess.check_output(
        ["git", "-C", str(ROOT), "show", f"{SOURCE}:{path}"]
    ).decode("utf-8")


def normalized(command):
    parts = list(command)
    parts[0] = parts[0].replace("\\", "/").rsplit("/", 1)[-1].removesuffix(".exe")
    return parts


require(subprocess.check_output(["git", "-C", str(ROOT), "rev-parse", "HEAD"]).decode().strip() == SOURCE,
        "worktree HEAD is not the audited source")
cmake = git_show("CMakeLists.txt")
expected = []
for match in re.finditer(r"add_test\(\s*NAME\s+(\S+)\s+COMMAND\s+([^)]+)\)", cmake):
    expected.append({
        "name": match.group(1),
        "command": match.group(2).split(),
        "line": cmake.count("\n", 0, match.start()) + 1,
    })
require(len(expected) == 61, "SOURCE must declare exactly 61 CTest entries")
require(expected[-4:][0]["name"] == "precision_client_io_first_mult2_contract", "entry 58 changed")
require(expected[-3:][0]["name"] == "repeated_mult2_semantic_two_square_contract", "entry 59 changed")
require(expected[-2:][0]["name"] == "paper_h128_client_keypair_contract", "entry 60 changed")
require(expected[-1]["name"] == "paper_full_eight_square_contract", "entry 61 missing or reordered")
by_name = {item["name"]: item for item in expected}

raw = LOG.read_bytes()
require(len(raw) == LOG_BYTES, "retained log byte count changed")
require(hashlib.sha256(raw).hexdigest() == LOG_SHA,
        "retained log SHA-256 changed")
require(raw.startswith(b"\xef\xbb\xbf"), "retained connector bytes lost UTF-8 BOM")
require(b"\r\n" not in raw, "retained connector bytes are not LF-only")
capture = json.loads(CAPTURE.read_text(encoding="utf-8"))
require(capture.get("isError") is False, "connector capture reports retrieval error")
connector_bytes = capture.get("structuredContent", {}).get("content", "").encode("utf-8")
require(len(connector_bytes) == CONNECTOR_BYTES and hashlib.sha256(connector_bytes).hexdigest() == CONNECTOR_SHA,
        "original connector decoded byte identity changed")
if HOST == "linux":
    require(connector_bytes == raw, "Linux connector decoded content is not byte-identical to retained log")
else:
    require(connector_bytes.startswith(b"\xef\xbb\xbf") and connector_bytes.count(b"\r\n") == 5949,
            "Windows connector decoded content lost BOM/CRLF structure")
    require(connector_bytes.replace(b"\r\n", b"\n") == raw,
            "Windows retained LF log is not the exact CRLF-to-LF transform of connector content")

terminal = json.loads((HERE / "RUN_TERMINAL_01.json").read_text(encoding="utf-8"))
require(terminal["databaseId"] == int(RUN) and terminal["attempt"] == int(ATTEMPT) and
        terminal["headSha"] == SOURCE and terminal["conclusion"] == "failure", "run terminal metadata mismatch")
jobs = [job for job in terminal["jobs"] if job["databaseId"] == int(JOB)]
require(len(jobs) == 1, "job terminal metadata missing or duplicated")
terminal_job = jobs[0]
require(terminal_job["status"] == "completed" and terminal_job["conclusion"] == "failure" and
        terminal_job["completedAt"] == COMPLETED_AT, "job terminal state/time mismatch")
failed_steps = [step for step in terminal_job["steps"] if step["conclusion"] == "failure"]
require(len(failed_steps) == 1 and failed_steps[0]["name"] == "Build paper full eight-square contract",
        "paper explicit build is not the sole failed step")

decoded = raw.decode("utf-8")
lines = [re.sub(r"^\ufeff?\d{4}-\d\d-\d\dT\S+Z ?", "", line) for line in decoded.splitlines()]
text = "\n".join(lines)
provenance = f"PROJECT_SOURCE_COMMIT={SOURCE} GITHUB_RUN_ID={RUN} GITHUB_RUN_ATTEMPT={ATTEMPT}"
require(text.count(provenance) == 1, "exact run provenance missing or duplicated")
require(PIN in text and "-DNATIVE_SIZE=64" in text and "-DMATHBACKEND=4" in text,
        "pin or native/backend configure evidence missing")

# Live CTest JSON is checked against the exact SOURCE CMake names, argv and add_test backtraces.
listings = []
for index, line in enumerate(lines):
    if line.strip() == "{" and index + 1 < len(lines) and '"backtraceGraph"' in lines[index + 1]:
        obj = json.JSONDecoder().raw_decode("\n".join(lines[index:]))[0]
        count = len(obj["tests"])
        require(count in (57, 60), f"unexpected live listing size {count}")
        require([item["name"] for item in obj["tests"]] == [item["name"] for item in expected[:count]],
                f"live {count} names/order differ from SOURCE")
        graph = obj["backtraceGraph"]
        for actual, want in zip(obj["tests"], expected):
            require(normalized(actual["command"]) == want["command"], f"listing argv mismatch: {want['name']}")
            node = graph["nodes"][actual["backtrace"]]
            require(graph["commands"][node["command"]] == "add_test", "backtrace command is not add_test")
            require(node["line"] == want["line"], f"backtrace line mismatch: {want['name']}")
            require(graph["files"][node["file"]].replace("\\", "/").endswith("CMakeLists.txt"),
                    "backtrace does not resolve to CMakeLists.txt")
        listings.append({"line": index + 1, "count": count, "source_backtraces_match": True})
require([item["count"] for item in listings] == [57, 60], "expected exactly live 57 then live 60 listings")

# Bind every Start, printed argv and terminal Passed record.
active = None
invocations = []
for index, line in enumerate(lines):
    begin = re.match(r"\s*Start\s+(\d+):\s+(\S+)\s*$", line)
    command = re.match(r"(\d+): Test command:\s+(.+)$", line)
    end = re.match(r"\s*(\d+)/(\d+) Test\s*#\s*(\d+):\s+(\S+)\s+\.+\s+Passed\s+([\d.]+) sec", line)
    if begin:
        require(active is None, "nested Start records")
        number, name = int(begin[1]), begin[2]
        require(expected[number - 1]["name"] == name, "Start number/name differs from SOURCE")
        active = {"number": number, "name": name, "start_line": index + 1, "command": None, "output": []}
    elif command:
        require(active is not None and int(command[1]) == active["number"] and active["command"] is None,
                "command not uniquely bound to active Start")
        lexer = shlex.shlex(command[2], posix=True)
        lexer.whitespace_split = True
        lexer.commenters = ""
        lexer.escape = ""
        parts = list(lexer)
        require(normalized(parts) == by_name[active["name"]]["command"], "executed argv differs from SOURCE")
        active["command"] = parts
        active["command_line"] = index + 1
    elif end:
        require(active is not None and active["command"] is not None, "Passed record has no bound command")
        require(int(end[3]) == active["number"] and end[4] == active["name"], "Passed identity mismatch")
        active.update(result="Passed", result_line=index + 1, ordinal=int(end[1]),
                      group_size=int(end[2]), seconds=end[5])
        invocations.append(active)
        active = None
    elif active is not None:
        active["output"].append({"line": index + 1, "text": line})
require(active is None and len(invocations) == 123, "expected 123 completely bound passing invocations")
groups = [[expected[2]], expected[55:57], expected[:57], [expected[57]], expected[58:60], expected[:60]]
offset = 0
for group in groups:
    actual = invocations[offset:offset + len(group)]
    require([item["name"] for item in actual] == [item["name"] for item in group], "execution group order changed")
    require([item["ordinal"] for item in actual] == list(range(1, len(group) + 1)), "group ordinal mismatch")
    require(all(item["group_size"] == len(group) for item in actual), "group denominator mismatch")
    offset += len(group)
require(offset == 123, "group partition did not consume every invocation")

def built_target(target):
    return any(line == "[100%] Built target " + target or
               re.fullmatch(r"\[(\d+)/\1\] Linking CXX executable " + re.escape(target) + r"\.exe", line)
               for line in lines)

api_targets = [f"{prefix}_api_contract_test" for prefix in ("relin2", "rs2", "mult2", "add", "sub")]
accepted_targets = [item["command"][0] for item in expected[57:60]]
for target in api_targets + accepted_targets:
    require(built_target(target), f"explicit target completion missing: {target}")

# Preserve the original strict I/O, repeated-Mult2 and h128 execution semantics.
io_records, repeated_records, h128_records = [], [], []
for invocation in invocations:
    output = invocation["output"]
    if invocation["number"] == 58:
        numeric = [item for item in output if f"test={expected[57]['name']} result=PASS" in item["text"]]
        require(len(numeric) == 1, "I/O numeric PASS line missing or duplicated")
        record = dict(re.findall(r"(\w+)=([^\s]+)", numeric[0]["text"]))
        require(record["source"] == SOURCE and record["openfhe_pin"] == PIN, "I/O source/pin mismatch")
        for key, value in {"N":"64", "S":"16", "gap":"2", "native":"64", "backend":"4", "depth":"7",
                           "first_bits":"55", "malformed_key_rejections":"32"}.items():
            require(record[key] == value, f"I/O frozen field mismatch: {key}")
        for key in ("max_fresh_public_error", "max_fresh_oracle_error", "max_product_public_error",
                    "max_product_oracle_error", "input_public_delta_error", "input_oracle_delta_error",
                    "product_public_delta_error", "product_oracle_delta_error"):
            require(0 <= Fraction(record[key]) <= Fraction(1, 2**80), f"I/O error threshold failed: {key}")
        for key in ("max_horner_component_disagreement", "cross_precision_error"):
            require(0 <= Fraction(record[key]) <= Fraction(1, 2**120), f"I/O exact threshold failed: {key}")
        require(Fraction(int(record["exact_scale_numerator"]), int(record["exact_scale_denominator"])) ==
                Fraction(2**200, int(record["q_div"]) * int(record["q_l"])), "I/O exact scale mismatch")
        require(int(record["centered_headroom"]) > 0, "I/O centered headroom is not positive")
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
            "shared_params_drift_contract passed boundaries=3 owned_eval_tag_cleanup=1",
        ]
        anchor_lines = [numeric[0]["line"]]
        for marker in markers:
            hits = [item["line"] for item in output if item["text"] == "58: " + marker]
            require(len(hits) == 1, f"I/O marker missing or duplicated: {marker}")
            anchor_lines.extend(hits)
        require(anchor_lines == sorted(anchor_lines) and anchor_lines[-1] < invocation["result_line"],
                "I/O markers are not ordered before actual PASS")
        io_records.append({"numeric": record, "ordered_anchor_lines": anchor_lines})
    elif invocation["number"] == 59:
        records = [json.loads(item["text"].split(": ", 1)[1]) for item in output
                   if item["text"].startswith('59: {"test":')]
        require([(item["trial"], item["stage"]) for item in records] ==
                [(trial, stage) for trial in range(4) for stage in (1, 2)], "repeated stage/trial matrix mismatch")
        for record in records:
            require(record["test"] == expected[58]["name"] and record["N"] == 64 and record["batch"] == 16,
                    "repeated frozen geometry mismatch")
            require(record["depth"] == 9 and record["secret_distribution"] == "UNIFORM_TERNARY",
                    "repeated depth/distribution mismatch")
            d, m1, m2 = int(record["d"]), int(record["m1"]), int(record["m2"])
            require((d, m1, m2) == (1125899906843009, 1125899906840833, 1125899906844161),
                    "repeated frozen divisor/moduli mismatch")
            want = Fraction(2**200, d*m1) if record["stage"] == 1 else Fraction(2**400, d*d*d*m1*m1*m2)
            require(Fraction(int(record["scale_numerator"]), int(record["scale_denominator"])) == want,
                    "repeated exact scale mismatch")
            require(record["evaluation_family"] == record["stage"] - 1 and record["result_family"] == 1,
                    "repeated family routing mismatch")
            require(int(record["m"]) == (m1 if record["stage"] == 1 else m2), "repeated active modulus mismatch")
            require(record["tag"].endswith("-mult2-family-1") and record["observed_product_headroom_bits"] > 0,
                    "repeated tag/headroom mismatch")
            for key in ("max_slot_error", "delta_error"):
                require(0 <= Fraction(record[key]) <= Fraction(1, 2**80), f"repeated threshold failed: {key}")
        repeated_records.extend(records)
    elif invocation["number"] == 60:
        markers = ["valid-path assertions passed (diagnostic, not paper evidence)",
                   "50 named rejection assertions passed (no injected tag collisions)",
                   "two-call uniqueness and owned-tag cache-isolation assertions passed"]
        found = []
        for marker in markers:
            hits = [item["line"] for item in output if item["text"] == "60: " + marker]
            require(len(hits) == 1, f"h128 marker missing or duplicated: {marker}")
            found.extend(hits)
        require(found == sorted(found) and found[-1] < invocation["result_line"], "h128 markers not ordered before PASS")
        h128_records.append({"marker_lines": found, "named_rejections": 50,
                             "scope": "fixed-Q N256 diagnostic; not paper-scale evidence"})
require(len(io_records) == 2 and len(repeated_records) == 16 and len(h128_records) == 2,
        "accepted semantic invocation counts mismatch")

# RED must be the explicit entry-61 build, caused by absent production API, before any paper runtime.
last_pass_line = invocations[-1]["result_line"]
if HOST == "linux":
    build_groups = [index + 1 for index, line in enumerate(lines)
                    if line == "##[group]Run cmake --build build --target paper_full_eight_square_contract_test --parallel 2"]
else:
    build_groups = [index + 1 for index, line in enumerate(lines)
                    if line == '##[group]Run build="$(cygpath -u "$PROJECT_BUILD")"' and
                    any("cmake --build" in following and
                        "--target paper_full_eight_square_contract_test --parallel 2" in following
                        for following in lines[index + 1:index + 4])]
require(len(build_groups) == 1 and build_groups[0] > last_pass_line, "paper build is not uniquely after accepted 60 PASS")
failure_requirements = {
    "RepeatedMult2Result": "RepeatedMult2Result' was not declared in this scope" if HOST == "windows" else "RepeatedMult2Result’ was not declared in this scope",
    "DoubleCKKS::RCBWithReceipt": "DoubleCKKS' has no member named 'RCBWithReceipt'" if HOST == "windows" else "DoubleCKKS’ has no member named ‘RCBWithReceipt’",
    "HighPrecisionClientIO::BindRepeatedRcb": "HighPrecisionClientIO' has no member named 'BindRepeatedRcb'" if HOST == "windows" else "HighPrecisionClientIO’ has no member named ‘BindRepeatedRcb’",
    "ClientCiphertextOrigin::RepeatedMult2Rcb": "RepeatedMult2Rcb' is not a member of 'openfhe_2023_1788::client_io::ClientCiphertextOrigin'" if HOST == "windows" else "RepeatedMult2Rcb’ is not a member of ‘openfhe_2023_1788::client_io::ClientCiphertextOrigin’",
    "CreatePaperRepeatedMult2Setup": "CreatePaperRepeatedMult2Setup' was not declared in this scope" if HOST == "windows" else "CreatePaperRepeatedMult2Setup’ was not declared in this scope",
}
failure_lines = {}
for api, diagnostic in failure_requirements.items():
    hits = [index + 1 for index, line in enumerate(lines) if diagnostic in line]
    require(hits and min(hits) > build_groups[0], f"missing primary absent-API diagnostic: {api}")
    failure_lines[api] = hits
production = "\n".join(git_show(path) for path in (
    "include/openfhe_2023_1788/repeated_mult2.h",
    "include/openfhe_2023_1788/double_ckks.h",
    "include/openfhe_2023_1788/high_precision_client_io.h",
    "src/repeated_mult2.cpp", "src/double_ckks.cpp", "src/high_precision_client_io.cpp"))
for symbol in ("RepeatedMult2Result", "RCBWithReceipt", "BindRepeatedRcb", "RepeatedMult2Rcb",
               "CreatePaperRepeatedMult2Setup"):
    require(symbol not in production, f"claimed missing API unexpectedly exists at exact SOURCE: {symbol}")
paper_test = git_show("tests/paper_full_eight_square_contract_test.cpp")
for symbol in ("RepeatedMult2Result", "RCBWithReceipt", "BindRepeatedRcb", "RepeatedMult2Rcb",
               "CreatePaperRepeatedMult2Setup"):
    require(symbol in paper_test, f"paper test does not require absent symbol: {symbol}")
process_errors = [index + 1 for index, line in enumerate(lines)
                  if line == f"##[error]Process completed with exit code {PROCESS_EXIT}."]
require(len(process_errors) == 1 and process_errors[0] > max(min(v) for v in failure_lines.values()),
        f"explicit paper build did not terminate with exit {PROCESS_EXIT} after API diagnostics")
allowed_cascade_fragments = (
    "RepeatedMult2Result", "template argument", "declval", "does not name a type", "RCBWithReceipt",
    "could not convert", "has no member named 'result'", "has no member named ‘result’", "BindRepeatedRcb",
    "RepeatedMult2Rcb", "CreatePaperRepeatedMult2Setup", "defined but not used",
    "forbids declaration of ‘type name’", "parse error in template argument list",
)
compiler_error_lines = [{"line": index + 1, "text": line} for index, line in enumerate(lines)
                        if build_groups[0] < index + 1 < process_errors[0] and ": error:" in line]
require(compiler_error_lines, "paper build contains no compiler diagnostics")
unexpected_errors = [item for item in compiler_error_lines
                     if not any(fragment in item["text"] for fragment in allowed_cascade_fragments)]
require(not unexpected_errors, f"independent unexpected compiler diagnostics: {unexpected_errors}")
require(not any(re.match(r"\s*Start\s+61:\s+paper_full_eight_square_contract", line) for line in lines),
        "paper test unexpectedly started")
require(not any("Test command:" in line and "paper_full_eight_square_contract_test" in line for line in lines),
        "paper runtime command unexpectedly printed")
require("BEGIN test=paper_full_eight_square_contract" not in text and
        "COMPLETE test=paper_full_eight_square_contract" not in text,
        "paper runtime marker unexpectedly present")

for invocation in invocations:
    del invocation["output"]
result = {
    "audit": "independent exact-source retained hosted API-RED parser",
    "host": HOST,
    "source": SOURCE,
    "run_id": RUN,
    "run_attempt": int(ATTEMPT),
    "job_id": JOB,
    "log": str(LOG),
    "bytes": len(raw),
    "sha256": hashlib.sha256(raw).hexdigest(),
    "connector_capture": str(CAPTURE),
    "connector_decoded_bytes": len(connector_bytes),
    "connector_decoded_sha256": hashlib.sha256(connector_bytes).hexdigest(),
    "connector_transform": "identity" if HOST == "linux" else "CRLF-to-LF only; 5949 CRLF pairs",
    "encoding": "retained UTF-8 BOM, LF-only; connector decoded content, not HTTP raw bytes",
    "terminal": {"status": terminal_job["status"], "conclusion": terminal_job["conclusion"],
                 "completed_at": terminal_job["completedAt"], "sole_failed_step": failed_steps[0]["name"]},
    "status": "PASS_EXPECTED_COMPILE_TIME_API_RED",
    "live_listings": listings,
    "actual_pass_bindings": invocations,
    "execution_partition": [1, 2, 57, 1, 2, 60],
    "api_targets_built": api_targets,
    "accepted_contract_targets_built": accepted_targets,
    "io_records": io_records,
    "repeated_stage_records": repeated_records,
    "h128_invocations": h128_records,
    "paper_red": {
        "explicit_build_group_line": build_groups[0],
        "primary_absent_api_diagnostic_lines": failure_lines,
        "process_exit_code": PROCESS_EXIT,
        "process_error_line": process_errors[0],
        "paper_runtime_invocations": 0,
        "compiler_error_lines": len(compiler_error_lines),
        "independent_unexpected_compiler_errors": [],
        "interpretation": "expected compile-time RED from intentionally missing production APIs; no paper numerical execution",
    },
    "scope": f"retained {HOST} diagnostic regressions plus entry-61 compile-time API RED; not paper-scale correctness evidence",
    "mac_crypto": "NOT_RUN",
}
print(json.dumps(result, indent=2))
