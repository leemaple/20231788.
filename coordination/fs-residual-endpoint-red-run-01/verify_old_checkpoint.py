#!/usr/bin/env python3
"""Fail-fast, read-only audit of the old checkpoints in run 33991083281.

The only write is the explicitly selected JSON output.  Source authority is
read with ``git show <tested-source>:<path>``; the current documentation HEAD
is deliberately neither required nor used as source authority.
"""

import argparse
import hashlib
import json
import re
import shlex
import subprocess
from collections import Counter
from pathlib import Path


SOURCE = "2fe655d493dcde5f05aa1515f41ca6823bba30bd"
SOURCE_TREE = "3102379bdc2c6fc25402809fd4d4db08971c71b2"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
RUN_ID = 33991083281
ATTEMPT = 1

SOURCE_FILES = {
    "CMakeLists.txt": {
        "blob": "82fc3cc581342d677f8b4cac157db8eee498913f",
        "sha256": "386a23eb61f083b109a18ca1d7b54ad0481ecd9ed0ccf40accf8bb2ba7e0eae1",
    },
    ".github/workflows/dcp-rcb.yml": {
        "blob": "20a663e947715e38a5c25b77b4c7b6b03be23b80",
        "sha256": "586fa02addebc175ec7e9266b2fa61b9084a50c3b311a799b20d0bf2775ef441",
    },
}

TERMINAL = {
    "name": "RUN_TERMINAL.json",
    "sha256": "4bb036872a6b9f179a654a433ca82635c60d9237bb9e4275c2806ada2d678599",
}

HOSTS = {
    "linux": {
        "job_id": 101373319837,
        "job_name": "linux-gcc",
        "log": "LINUX_RAW.log",
        "log_bytes": 460571,
        "log_sha256": "c2fe6cc6167d1648798cbc0d2a8f416369897f6d1458211cc0b447c70fa06b6e",
        "capture": "LINUX_CAPTURE.json",
        "capture_sha256": "60190170283ef54f999098f04b003f199e6598b32677f62b981925471541051f",
        "decoded_bytes": 460571,
        "decoded_sha256": "c2fe6cc6167d1648798cbc0d2a8f416369897f6d1458211cc0b447c70fa06b6e",
        "crlf_pairs": 0,
    },
    "windows": {
        "job_id": 101373319710,
        "job_name": "windows-mingw64",
        "log": "WINDOWS_LF.log",
        "log_bytes": 471229,
        "log_sha256": "3317309b60eacdee6175dceee7a62ad3c85eab4b76df4d79965b59aa01390764",
        "capture": "WINDOWS_CAPTURE.json",
        "capture_sha256": "25139215e8ed4ee2e546f8f278d4af144e15953d4e27ec8bb7c0d8c00426b389",
        "decoded_bytes": 477042,
        "decoded_sha256": "b49549088d34397f24dc9350b12f88c9b531a1bf6d65994bf5138f3e5f05733d",
        "crlf_pairs": 5813,
    },
}

OLD_SUCCESS_STEPS = [
    "Build warning-clean project",
    "Build Relin2 public API contract",
    "Build RS2 public API contract",
    "Run focused first-Mult2 precision contract",
    "Run focused Pair Add and Sub inputs to first Mult2",
    "Run legacy 57-test checkpoint",
    "Build Mult2 public API contract",
    "Build Add public API contract",
    "Build Sub public API contract",
    "Build production lossless client I/O contract",
    "Build repeated-Mult2 semantic contract",
    "Build fixed-Q h128 client keypair contract",
    "Run focused production lossless client I/O contract",
    "Run focused repeated-Mult2 and h128 contracts",
    "Run complete 60-test three-track suite",
]


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def git_bytes(repo, *args):
    return subprocess.check_output(["git", "-C", str(repo), *args])


def source_bytes(repo, path):
    return git_bytes(repo, "show", f"{SOURCE}:{path}")


def normalize_command(command):
    parts = list(command)
    parts[0] = parts[0].replace("\\", "/").rsplit("/", 1)[-1]
    if parts[0].endswith(".exe"):
        parts[0] = parts[0][:-4]
    return parts


def parse_shell_words(value):
    lexer = shlex.shlex(value, posix=True)
    lexer.whitespace_split = True
    lexer.commenters = ""
    lexer.escape = ""
    return list(lexer)


def derive_tests(cmake):
    tests = []
    for match in re.finditer(r"add_test\(\s*NAME\s+(\S+)\s+COMMAND\s+([^)]+)\)", cmake):
        tests.append({
            "name": match.group(1),
            "command": match.group(2).split(),
            "cmake_line": cmake.count("\n", 0, match.start()) + 1,
        })
    require(len(tests) == 61, f"tested CMake declares {len(tests)}, not 61, tests")
    require(len({item["name"] for item in tests}) == 61, "duplicate CMake test name")
    return tests


def derive_default_targets(cmake):
    targets = []
    for match in re.finditer(r"add_executable\((.*?)\)", cmake, re.S):
        tokens = match.group(1).split()
        require(tokens, "empty add_executable body")
        if "EXCLUDE_FROM_ALL" not in tokens[1:]:
            targets.append(tokens[0])
    return targets


def apply_selector(tests, mode, pattern):
    matched = [bool(re.search(pattern, item["name"])) for item in tests]
    return [item for item, hit in zip(tests, matched) if hit == (mode == "R")]


def derive_workflow_contract(workflow, tests):
    selectors = []
    for line in workflow.splitlines():
        if "ctest --test-dir" not in line or "--verbose" not in line:
            continue
        match = re.search(r" -(R|E) '([^']+)'\s*$", line)
        require(match is not None, f"unparsed verbose CTest command: {line}")
        selectors.append((match.group(1), match.group(2)))
    require(len(selectors) == 14 and selectors[:7] == selectors[7:],
            f"Linux/Windows verbose selector sequence differs: {selectors}")
    require(selectors[6] == ("R", "^paper_full_eight_square_contract$"),
            "paper selector is not the seventh per-host verbose CTest selector")
    groups = [apply_selector(tests, mode, pattern) for mode, pattern in selectors[:6]]
    sizes = [len(group) for group in groups]
    require(sizes == [1, 2, 57, 1, 2, 60], f"workflow-derived group sizes changed: {sizes}")

    target_sequences = []
    for job_text in re.split(r"\n  [a-zA-Z0-9_-]+:\n", workflow)[1:]:
        targets = re.findall(r"cmake --build [^\n]* --target (\w+) --parallel 2", job_text)
        if targets:
            target_sequences.append(targets)
    require(len(target_sequences) == 2 and target_sequences[0] == target_sequences[1],
            f"Linux/Windows explicit target sequence differs: {target_sequences}")
    targets = target_sequences[0]
    require(targets[-1:] == ["paper_full_eight_square_contract_test"],
            "paper target is not the final explicit target")
    old_targets = targets[:-1]
    api_targets = [target for target in old_targets if target.endswith("_api_contract_test")]
    require(api_targets == [f"{name}_api_contract_test" for name in ("relin2", "rs2", "mult2", "add", "sub")],
            f"five explicit API target sequence changed: {api_targets}")
    require(len(old_targets) == 9, f"old explicit target count changed: {old_targets}")
    return groups, old_targets, api_targets


def strip_log(raw):
    decoded = raw.decode("utf-8")
    lines = []
    for line in decoded.splitlines():
        line = re.sub(r"^\ufeff?\d{4}-\d\d-\d\dT\S+Z ?", "", line)
        line = re.sub(r"\x1b\[[0-9;]*m", "", line)
        lines.append(line)
    return lines


def parse_live_inventories(lines, expected, wanted_groups):
    listings = []
    decoder = json.JSONDecoder()
    for index, line in enumerate(lines):
        if line.strip() != "{" or index + 1 >= len(lines) or '"backtraceGraph"' not in lines[index + 1]:
            continue
        obj = decoder.raw_decode("\n".join(lines[index:]))[0]
        actual_tests = obj["tests"]
        count = len(actual_tests)
        require(count in (57, 60), f"unexpected reached live inventory size {count}")
        wanted = wanted_groups[2] if count == 57 else wanted_groups[5]
        require([item["name"] for item in actual_tests] == [item["name"] for item in wanted],
                f"live {count}-test name/order differs from tested workflow+CMake")
        graph = obj["backtraceGraph"]
        for actual, target in zip(actual_tests, wanted):
            require(normalize_command(actual["command"]) == target["command"],
                    f"live inventory argv mismatch: {target['name']}")
            node = graph["nodes"][actual["backtrace"]]
            require(graph["commands"][node["command"]] == "add_test"
                    and node["line"] == target["cmake_line"],
                    f"live inventory backtrace mismatch: {target['name']}")
            require(graph["files"][node["file"]].replace("\\", "/").endswith("CMakeLists.txt"),
                    f"live inventory source file mismatch: {target['name']}")
        listings.append({
            "log_line": index + 1,
            "count": count,
            "first": actual_tests[0]["name"],
            "last": actual_tests[-1]["name"],
            "name_order_argv_backtrace_match": True,
        })
    require([item["count"] for item in listings] == [57, 60],
            f"reached live inventory sequence differs: {listings}")
    return listings


def parse_pass_bindings(lines, expected, groups):
    by_name = {item["name"]: item for item in expected}
    active = None
    passed = []
    for index, line in enumerate(lines):
        start = re.match(r"\s*Start\s+(\d+):\s+(\S+)\s*$", line)
        command = re.match(r"(\d+): Test command:\s+(.+)$", line)
        result = re.match(r"\s*(\d+)/(\d+) Test\s*#\s*(\d+):\s+(\S+)\s+\.+\s+(Passed|\*\*\*Failed)\s+([\d.]+) sec", line)
        if start:
            require(active is None, f"nested Start at log line {index + 1}")
            number, name = int(start.group(1)), start.group(2)
            require(1 <= number <= len(expected) and expected[number - 1]["name"] == name,
                    f"Start number/name mismatch at log line {index + 1}")
            active = {"number": number, "name": name, "start_line": index + 1, "command": None}
        elif command:
            require(active is not None and int(command.group(1)) == active["number"]
                    and active["command"] is None,
                    f"unbound/duplicate Test command at log line {index + 1}")
            argv = parse_shell_words(command.group(2))
            require(normalize_command(argv) == by_name[active["name"]]["command"],
                    f"executed argv mismatch: {active['name']}")
            active["command"] = argv
            active["command_line"] = index + 1
        elif result:
            require(active is not None and active["command"] is not None,
                    f"result lacks Start/command at log line {index + 1}")
            require(int(result.group(3)) == active["number"] and result.group(4) == active["name"],
                    f"result identity mismatch at log line {index + 1}")
            require(result.group(5) == "Passed", f"old invocation failed: {active['name']}")
            active.update({
                "ordinal": int(result.group(1)),
                "group_size": int(result.group(2)),
                "result": "Passed",
                "result_line": index + 1,
                "seconds": result.group(6),
            })
            passed.append(active)
            active = None
    require(active is None, "unterminated old Start binding")

    offset = 0
    group_results = []
    for wanted in groups:
        actual = passed[offset:offset + len(wanted)]
        require([item["name"] for item in actual] == [item["name"] for item in wanted],
                f"old pass group order mismatch at offset {offset}")
        require([item["ordinal"] for item in actual] == list(range(1, len(wanted) + 1)),
                f"old pass group ordinal mismatch at offset {offset}")
        require(all(item["group_size"] == len(wanted) for item in actual),
                f"old pass group denominator mismatch at offset {offset}")
        group_results.append({
            "size": len(wanted),
            "first": wanted[0]["name"],
            "last": wanted[-1]["name"],
            "first_start_line": actual[0]["start_line"],
            "last_result_line": actual[-1]["result_line"],
        })
        offset += len(wanted)
    require(offset == len(passed) == 123, f"old invocation count differs: {len(passed)}")
    frequencies = Counter(item["name"] for item in passed)
    require(set(frequencies) == {item["name"] for item in expected[:60]},
            "the 123 passes do not cover exactly the 60 old tests")
    return passed, group_results, dict(sorted(frequencies.items()))


def completion_lines(lines, target, host):
    if host == "linux":
        pattern = re.compile(r"\[\s*\d+%\] Built target " + re.escape(target) + r"$")
    else:
        pattern = re.compile(r"\[\d+/\d+\] Linking CXX executable " + re.escape(target) + r"\.exe$")
    return [index + 1 for index, line in enumerate(lines) if pattern.fullmatch(line)]


def audit_host(host, cfg, coord_dir, capture_dir, terminal, tests, groups,
               default_targets, old_targets, api_targets):
    log_path = coord_dir / cfg["log"]
    capture_path = capture_dir / cfg["capture"]
    raw = log_path.read_bytes()
    require(len(raw) == cfg["log_bytes"] and sha256(raw) == cfg["log_sha256"],
            f"{host}: retained LF log identity changed")
    require(raw.startswith(b"\xef\xbb\xbf") and b"\r" not in raw,
            f"{host}: retained log is not UTF-8-BOM LF-only")

    capture_file = capture_path.read_bytes()
    require(sha256(capture_file) == cfg["capture_sha256"], f"{host}: capture file identity changed")
    capture = json.loads(capture_file.decode("utf-8"))
    require(capture["run_id"] == RUN_ID and capture["job_id"] == cfg["job_id"]
            and "not original HTTP transport bytes" in capture["description"],
            f"{host}: connector capture provenance/representation mismatch")
    connector = capture["content"].encode("utf-8")
    require(len(connector) == cfg["decoded_bytes"] and sha256(connector) == cfg["decoded_sha256"],
            f"{host}: connector decoded content identity changed")
    require(connector.startswith(b"\xef\xbb\xbf")
            and connector.count(b"\r\n") == cfg["crlf_pairs"]
            and connector.count(b"\r") == cfg["crlf_pairs"],
            f"{host}: connector newline/BOM structure changed")
    normalized = connector if host == "linux" else connector.replace(b"\r\n", b"\n")
    require(normalized == raw, f"{host}: retained log differs beyond declared newline normalization")

    jobs = [job for job in terminal["jobs"]["jobs"] if job["id"] == cfg["job_id"]]
    require(len(jobs) == 1, f"{host}: terminal job missing/duplicated")
    job = jobs[0]
    require(job["run_id"] == RUN_ID and job["run_attempt"] == ATTEMPT
            and job["head_sha"] == SOURCE and job["name"] == cfg["job_name"]
            and job["status"] == "completed" and job["conclusion"] == "failure",
            f"{host}: terminal job identity/status mismatch")
    steps = {step["name"]: step for step in job["steps"]}
    for name in OLD_SUCCESS_STEPS:
        require(name in steps and steps[name]["status"] == "completed" and steps[name]["conclusion"] == "success",
                f"{host}: old workflow step not successful: {name}")

    lines = strip_log(raw)
    text = "\n".join(lines)
    provenance = f"PROJECT_SOURCE_COMMIT={SOURCE} GITHUB_RUN_ID={RUN_ID} GITHUB_RUN_ATTEMPT={ATTEMPT}"
    require(text.count(provenance) == 1, f"{host}: exact project provenance missing/duplicated")
    require(text.count(f"HEAD is now at {PIN[:7]}") == 1,
            f"{host}: exact OpenFHE checkout completion missing/duplicated")
    require(any(f"branch            {PIN} -> FETCH_HEAD" in line for line in lines),
            f"{host}: exact OpenFHE fetched ref missing")
    require(any(line == f"  OPENFHE_COMMIT: {PIN}" for line in lines),
            f"{host}: exact OpenFHE workflow pin missing")

    listings = parse_live_inventories(lines, tests, groups)
    passed, group_results, frequencies = parse_pass_bindings(lines, tests, groups)

    default_completion = {target: completion_lines(lines, target, host) for target in default_targets}
    for target, hits in default_completion.items():
        require(hits, f"{host}: default old target completion absent: {target}")
    explicit_completion = {target: completion_lines(lines, target, host) for target in old_targets}
    for target, hits in explicit_completion.items():
        require(hits, f"{host}: old explicit target completion absent: {target}")
    production_lines = [
        index + 1 for index, line in enumerate(lines)
        if (host == "linux" and re.fullmatch(r"\[\s*\d+%\] Built target openfhe_2023_1788", line))
        or (host == "windows" and re.fullmatch(r"\[\d+/\d+\] Linking CXX static library libopenfhe_2023_1788\.a", line))
    ]
    require(production_lines, f"{host}: production library completion absent")

    bindings = []
    for item in passed:
        binding = {key: value for key, value in item.items() if key != "command"}
        binding["actual_argv"] = item["command"]
        binding["normalized_argv"] = normalize_command(item["command"])
        binding["expected_argv"] = next(test["command"] for test in tests if test["name"] == item["name"])
        bindings.append(binding)
    return {
        "status": "PASS_OLD_CHECKPOINT_EVIDENCE",
        "host": host,
        "job_id": cfg["job_id"],
        "log": {"path": str(log_path), "bytes": len(raw), "sha256": sha256(raw)},
        "capture": {
            "path": str(capture_path),
            "file_sha256": sha256(capture_file),
            "decoded_bytes": len(connector),
            "decoded_sha256": sha256(connector),
            "crlf_pairs": connector.count(b"\r\n"),
            "transform": "identity" if host == "linux" else "CRLF-to-LF only",
        },
        "source_provenance_occurrences": 1,
        "openfhe_pin": PIN,
        "live_inventories": listings,
        "old_pass_invocations": len(passed),
        "old_unique_tests": len(frequencies),
        "workflow_derived_partition": [len(group) for group in groups],
        "groups": group_results,
        "test_invocation_frequency": frequencies,
        "bindings": bindings,
        "five_api_target_completion_lines": {
            target: explicit_completion[target] for target in api_targets
        },
        "other_old_explicit_target_completion_lines": {
            target: explicit_completion[target] for target in old_targets if target not in api_targets
        },
        "default_target_completion_lines": default_completion,
        "production_library_completion_lines": production_lines,
        "old_terminal_steps_successful": OLD_SUCCESS_STEPS,
    }


def main():
    here = Path(__file__).resolve().parent
    default_repo = here.parents[2]
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", type=Path, default=default_repo)
    parser.add_argument("--coordination-dir", type=Path,
                        default=default_repo / "coordination/fs-residual-endpoint-red-run-01")
    parser.add_argument("--capture-dir", type=Path, default=here)
    parser.add_argument("--output", type=Path, default=here / "OLD_CHECKPOINT_VERIFICATION.json")
    args = parser.parse_args()

    require(git_bytes(args.repo, "cat-file", "-t", SOURCE).strip() == b"commit",
            "tested source is not an available Git commit")
    require(git_bytes(args.repo, "rev-parse", f"{SOURCE}^{{tree}}").decode().strip() == SOURCE_TREE,
            "tested source tree identity changed")
    source_evidence = {}
    for path, wanted in SOURCE_FILES.items():
        data = source_bytes(args.repo, path)
        blob = git_bytes(args.repo, "rev-parse", f"{SOURCE}:{path}").decode().strip()
        require(blob == wanted["blob"] and sha256(data) == wanted["sha256"],
                f"tested Git blob/content mismatch: {path}")
        source_evidence[path] = {"blob": blob, "bytes": len(data), "sha256": sha256(data)}

    cmake = source_bytes(args.repo, "CMakeLists.txt").decode("utf-8")
    workflow = source_bytes(args.repo, ".github/workflows/dcp-rcb.yml").decode("utf-8")
    require(re.search(r"^\s*OPENFHE_COMMIT:\s*" + PIN + r"\s*$", workflow, re.M),
            "tested workflow OpenFHE pin changed")
    tests = derive_tests(cmake)
    groups, old_targets, api_targets = derive_workflow_contract(workflow, tests)
    default_targets = derive_default_targets(cmake)

    terminal_path = args.coordination_dir / TERMINAL["name"]
    terminal_file = terminal_path.read_bytes()
    require(sha256(terminal_file) == TERMINAL["sha256"], "terminal metadata identity changed")
    terminal = json.loads(terminal_file.decode("utf-8"))
    run = terminal["run"]
    require(run["id"] == RUN_ID and run["run_attempt"] == ATTEMPT
            and run["head_sha"] == SOURCE and run["event"] == "push"
            and run["status"] == "completed" and run["conclusion"] == "failure",
            "terminal run identity/status mismatch")
    require(terminal["jobs"]["total_count"] == 2 and len(terminal["jobs"]["jobs"]) == 2,
            "terminal job count differs")

    hosts = {
        host: audit_host(host, cfg, args.coordination_dir, args.capture_dir, terminal,
                         tests, groups, default_targets, old_targets, api_targets)
        for host, cfg in HOSTS.items()
    }
    result = {
        "audit": "independent old-checkpoint verifier for FS residual endpoint RED run",
        "status": "PASS_OLD_CHECKPOINT_EVIDENCE_BOTH_HOSTS",
        "run_id": RUN_ID,
        "run_attempt": ATTEMPT,
        "tested_source": SOURCE,
        "tested_source_tree": SOURCE_TREE,
        "current_repository_head_required": False,
        "source_files": source_evidence,
        "terminal": {"path": str(terminal_path), "bytes": len(terminal_file), "sha256": sha256(terminal_file)},
        "derived_cmake_test_count": len(tests),
        "derived_old_unique_test_count": 60,
        "derived_old_invocation_partition": [len(group) for group in groups],
        "derived_old_invocation_count": sum(len(group) for group in groups),
        "derived_old_explicit_targets": old_targets,
        "derived_five_explicit_api_targets": api_targets,
        "derived_default_targets": default_targets,
        "hosts": hosts,
        "scope": "old checkpoints only; no paper target failure or new self-test adjudication",
        "not_run": ["C++ compiler", "CMake", "OpenFHE", "FHE", "FFT", "NTT", "crypto", "CI", "network"],
        "model_note": "requested Sol/high; backend identity unattested",
    }
    args.output.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"PASS old checkpoints: hosts=2 unique=60 invocations=123 groups={[len(group) for group in groups]} output={args.output}")


if __name__ == "__main__":
    main()
