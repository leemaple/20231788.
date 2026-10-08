"""Package only task binaries with verified system-only dynamic dependencies."""
from pathlib import Path
import hashlib
import json
import shutil
import subprocess

source_commit = subprocess.check_output(["git", "rev-parse", "HEAD"], text=True).strip()
evidence = Path("artifacts/c7-cost")
green = json.loads((evidence / "green-receipt.json").read_text())
cases = {"wrong_lifecycle", "valid_arithmetic_state_immutability",
         "valid_wide_arithmetic_state_immutability", "mixed_tower_format",
         "untouched_public_pipeline", "declared_basis_mismatch", "terminal_rejections"}
expected = {(backend, case) for backend in ("reference", "reordered", "fused") for case in cases}
tests = green["tests"]
if (green["project_commit"] != source_commit or green["phase"] != "green" or
        green["passed"] is not True or len(tests) != 21 or
        {(t["backend"], t["case"]) for t in tests} != expected):
    raise RuntimeError("functional receipt identity or complete matrix mismatch")
for test in tests:
    stem = f"green-{test['backend']}-{test['case']}"
    if (test["passed"] is not True or test["returncode"] != 0 or test["expected_red"] is not False or
            (evidence / (stem + ".stdout.txt")).read_text() != f"RS2 case passed: {test['case']}\n" or
            (evidence / (stem + ".stderr.txt")).read_bytes()):
        raise RuntimeError("functional raw output or outcome mismatch")
probe = evidence / "probe-check"
started = json.loads((probe / "started.json").read_text())
completed = json.loads((probe / "completed.json").read_text())
if (started != {"source_commit": source_commit, "mode": "--check", "admission_sha256": "none"} or
        completed != {"source_commit": source_commit, "mode": "--check", "passed": True,
                      "sample_rows": 0, "functional_comparator_negative_controls": True} or
        (probe / "records.jsonl").read_bytes() or
        (evidence / "probe-check.stderr.txt").read_bytes() or
        (evidence / "probe-check.stdout.txt").read_text() !=
        "C7 --check completed; performance publication admission is separate\n"):
    raise RuntimeError("check-only probe receipt or raw output mismatch")
shape = json.loads((probe / "N32-shape.json").read_text())
if (shape["N"] != 32 or shape["slots"] != 16 or shape["components_per_member"] != 2 or
        (probe / "N32-public-inputs.bin").stat().st_size != 3 * 2 * 2 * len(shape["moduli"]) * 32 * 8):
    raise RuntimeError("check-only public input artifact is incomplete")

root = Path("artifacts/c7-cost/bundle")
root.mkdir(parents=True, exist_ok=False)
rows = []
for name in ("c7_cost_probe", "rs2_test"):
    source = Path(".ci/c7-cost-build") / name
    target = root / name
    shutil.copy2(source, target)
    file_output = subprocess.check_output(["file", str(target)], text=True)
    if "arm64" not in file_output or "Mach-O" not in file_output:
        raise RuntimeError("expected arm64 Mach-O executable")
    link_output = subprocess.check_output(["otool", "-L", str(target)], text=True)
    dependencies = [line.strip().split(" (")[0] for line in link_output.splitlines()[1:]]
    if not dependencies or any(not x.startswith(("/usr/lib/", "/System/Library/")) for x in dependencies):
        raise RuntimeError("bundle has a non-system dynamic dependency")
    rows.append({"name": name, "bytes": target.stat().st_size,
                 "sha256": hashlib.sha256(target.read_bytes()).hexdigest(),
                 "file": file_output.strip(), "dependencies": dependencies})
if rows[1]["sha256"] != green["binary_sha256"]:
    raise RuntimeError("packaged test binary does not match functional receipt")
manifest = {"source_commit": source_commit,
            "openfhe_commit": subprocess.check_output(["git", "-C", ".ci/openfhe-src", "rev-parse", "HEAD"], text=True).strip(),
            "classification": "engineering transport; performance execution requires separate admission",
            "files": rows}
(root / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
