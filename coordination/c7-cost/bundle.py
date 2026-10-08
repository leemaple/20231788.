"""Package only task binaries with verified system-only dynamic dependencies."""
from pathlib import Path
import hashlib
import json
import shutil
import subprocess

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
manifest = {"source_commit": subprocess.check_output(["git", "rev-parse", "HEAD"], text=True).strip(),
            "openfhe_commit": subprocess.check_output(["git", "-C", ".ci/openfhe-src", "rev-parse", "HEAD"], text=True).strip(),
            "classification": "engineering transport; performance execution requires separate admission",
            "files": rows}
(root / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
