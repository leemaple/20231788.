"""Bounded final-review archive generation; no compile, crypto or network."""
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import stat
import subprocess
import sys
import zipfile

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve().parent
REPO = HERE.parents[2]
HEAD = "3eae38f518cba6d4bcf4e53229334571f9152eb6"
SOURCE = "5f26c77598a350bbdce9f572f64aada9d38c4117"
BASE = "4ccc8fd2e7617625d27e58a53eb3489e99466ed4"
PIN = "df495ba2e91739a6dc8f1de254fc5a41155ce504"
UPSTREAM = Path("/private/tmp/h128-pro-packet.Lo406g/official-source")
ROOT = "lossless-client-io-final-review-5f26c775"
OUTPUT = HERE / (ROOT + ".zip")
EVIDENCE = "coordination/handoffs/lossless-client-io-pro-packet-01/"
STAGES = [
    ("01-red-a", "12d8fae78cc0d0fed5038cf21cdbd2173fe1f1ef", 33948543866, "CYCLE_A_RED"),
    ("02-green-a", "084ffa0af3cb21623151df0c826736ca84954140", 33950923304, "CYCLE_A_GREEN"),
    ("03-red-first-modulus", "f1ea03f35a6a553d65db30c93e771738f6bc0e1d", 33952773643, "FIRST_MODULUS_RED"),
    ("04-green-first-modulus", "01c90e8eeec696b62b92a17be9a49d4a014664d8", 33953977794, "FIRST_MODULUS_GREEN"),
    ("05-red-b", "4648da463c6ec77f6f23acb1a56c5dce88c7732e", 33960255214, "CYCLE_B_RED"),
    ("06-green-b", SOURCE, 33961604938, "CYCLE_B_GREEN"),
]
helper = REPO / EVIDENCE / "build_source_correction_01.py"
spec = importlib.util.spec_from_file_location("source_correction_helpers", helper)
helpers = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helpers)

def git(*args, repo=REPO):
    return subprocess.check_output(["git", "-C", str(repo), *args])
def sha(b):
    return hashlib.sha256(b).hexdigest()
def blob(ref, p, repo=REPO):
    return git("show", ref + ":" + p, repo=repo)
def tree(ref, *paths):
    return git("ls-tree", "-r", "--name-only", ref, "--", *paths).decode().splitlines()
def jsonbytes(v):
    return (json.dumps(v, ensure_ascii=False, indent=2) + "\n").encode()
def scan(data, label):
    environment = os.environ.copy()
    for key in ("GITLEAKS_CONFIG", "GITLEAKS_CONFIG_TOML"):
        environment.pop(key, None)
    environment["GOMAXPROCS"] = "2"
    cmd = ["/opt/homebrew/bin/gitleaks", "stdin", "--ignore-gitleaks-allow",
           "--gitleaks-ignore-path", "/dev/null", "--max-decode-depth", "5",
           "--max-archive-depth", "1", "--redact", "--no-banner", "--no-color"]
    result = subprocess.run(cmd, input=data, capture_output=True, env=environment)
    assert result.returncode == 0, (label, result.stderr.decode())
    version = subprocess.check_output([cmd[0], "version"], text=True).strip()
    return dict(label=label, bytes=len(data), sha256=sha(data), command=cmd, version=version,
                exit=result.returncode, stdout=result.stdout.decode(), stderr=result.stderr.decode())

assert git("rev-parse", "HEAD").decode().strip() == HEAD
assert not git("status", "--porcelain")
assert not OUTPUT.exists(), "never overwrite an archive"
paths = ("src", "include", "tests", "CMakeLists.txt", ".github/workflows")
assert not git("diff", SOURCE, HEAD, "--", *paths)
payload, origins = {}, {}
def put(name, value, origin):
    helpers.validate_relative_path(name)
    assert name not in payload and name.casefold() not in {n.casefold() for n in payload}
    assert value and len(value) < 3000000
    assert not any(part.lower().startswith(".env") or part.lower().endswith(
        (".pem", ".p12", ".key", ".sqlite", ".db")) for part in Path(name).parts)
    payload[name] = value
    origins[name] = origin

engineering_paths = set(tree(SOURCE, *paths))
for p in tree(HEAD, *paths, ".gitignore", "README.md", ".agents/skills/openfhe-2023-1788-workflow"):
    # Reporting instructions are not part of a source-review assignment.
    if p.endswith("/references/reporting.md"):
        continue
    ref = SOURCE if p in engineering_paths else HEAD
    put("project/" + p, blob(ref, p), dict(kind="project-git", commit=ref, path=p))
for p in tree(BASE, *paths, ".gitignore"):
    put("history/base/project/" + p, blob(BASE, p), dict(kind="project-git", commit=BASE, path=p))
docs = [
    "coordination/CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md",
    "coordination/TEST_SEAMS.md",
    "coordination/tasks/LOSSLESS_CLIENT_IO_PRO_IMPLEMENTATION_01.md",
    "coordination/LOSSLESS_CLIENT_IO_ACTUAL_RETURN.md",
    "coordination/LOSSLESS_CLIENT_IO_RETURN_SPEC_REVIEW.md",
    "coordination/LOSSLESS_REPEATED_H128_HANDSHAKE_REVIEW.md",
    "coordination/PRODUCTION_IO_API_INDEPENDENT_RECHECK.md",
    "coordination/PRODUCTION_IO_API_MAIN_DISPOSITION.md",
    "coordination/PRODUCTION_IO_OFFICIAL_API_AUDIT.md",
    "coordination/evidence/production-io-api/main-source-verification-20260904.json",
]
docs += [p for p in tree(HEAD, "coordination/returns/lossless-client-io-pro-b64a980")
         if p.endswith(".md") or p.endswith("/MANIFEST.json")]
docs += [EVIDENCE + n for n in [
    "DISPATCH_RECEIPT.md", "RECOVERY_RECEIPT_01.md", "SOURCE_CORRECTION_RECEIPT_01.md",
    "CODEX_CYCLE_A_FALLBACK_01.md", "CYCLE_A_GREEN_CANDIDATE_PREFLIGHT_01.md",
    "CYCLE_B_GREEN_PREFLIGHT_01.json"]]
for p in docs:
    put("project/" + p, blob(HEAD, p), dict(kind="project-git", commit=HEAD, path=p))
previous = BASE
stages = []
all_evidence = tree(HEAD, EVIDENCE)
for label, commit, run, prefix in STAGES:
    diff = git("diff", "--binary", "--full-index", previous, commit, "--", *paths)
    put("history/" + label + ".patch", diff,
        dict(kind="engineering-diff", before=previous, after=commit, paths=paths))
    members = [p for p in all_evidence if Path(p).name.startswith(prefix + "_") and (
        p.endswith("_ACCEPTANCE_01.md") or p.endswith("_JOB_01.log") or
        p.endswith("_ROOT_VERIFICATION_01.json") or p.endswith("_RUN_FINAL_01.json") or
        p.endswith("_DISPATCH_01.md"))]
    assert any(p.endswith("_ACCEPTANCE_01.md") for p in members)
    assert sum(p.endswith("_JOB_01.log") for p in members) == 2
    assert any(p.endswith("_RUN_FINAL_01.json") for p in members)
    for p in members:
        put("project/" + p, blob(HEAD, p), dict(kind="project-git", commit=HEAD, path=p))
    stages.append(dict(stage=label, commit=commit, run=run, event="push", attempt=1,
        patch="history/" + label + ".patch",
        files=[dict(path=p, bytes=len(blob(commit, p)), sha256=sha(blob(commit, p)))
               for p in tree(commit, *paths)], evidence=["project/" + p for p in members]))
    previous = commit
put("history/STAGES.json", jsonbytes(stages), dict(kind="generated-git-provenance"))

old = REPO / "artifacts/handoffs/io-source-correction-01/lossless-client-io-implementation-01-4ccc8fd-source-correction-01.zip"
assert len(old.read_bytes()) == 3132684
assert sha(old.read_bytes()) == "4c8295d56ca59d39441adbdc2fe24e87bb2bafaab4128b19265ba159337f329c"
with zipfile.ZipFile(old) as archive:
    assert archive.testzip() is None
    members = [n for n in archive.namelist() if "/official-full/" in n]
    assert len(members) == 64
    official_paths = [n.split("/official-full/", 1)[1] for n in members]
    for n, p in zip(members, official_paths):
        value = blob(PIN, p, UPSTREAM)
        assert value == archive.read(n)
        put("official-full/" + p, value, dict(kind="official-git", commit=PIN, path=p))
extra = [
    "src/core/include/math/hal/bigintbackend.h",
    "src/core/include/math/hal/bigintdyn/backenddyn.h",
    "src/core/include/math/math-hal.h",
    "src/core/include/lattice/hal/default/poly-impl.h",
    "src/pke/include/key/evalkey.h",
    "src/pke/include/key/evalkeyrelin.h",
    "src/pke/include/schemebase/base-leveledshe.h",
    "src/core/include/utils/inttypes.h",
    "src/core/include/lattice/hal/poly-interface.h",
    "src/pke/include/constants-defs.h",
    "src/pke/include/metadata.h",
    "src/pke/include/schemebase/decrypt-result.h",
    "src/pke/lib/schemerns/rns-leveledshe.cpp",
]
for p in extra:
    if "official-full/" + p not in payload:
        put("official-full/" + p, blob(PIN, p, UPSTREAM), dict(kind="official-git", commit=PIN, path=p))
for suffix in ("pdf", "txt"):
    path = Path("/Users/lifeng/.zcode/workspace/default/2023.1788." + suffix)
    put("paper/PAPER-2023-1788." + suffix, path.read_bytes(), dict(kind="user-supplied-paper"))
brief = HERE / "TASK.md"
put("TASK.md", brief.read_bytes(), dict(kind="current-final-review-task", owner="Codex"))
put("SOURCE_IDENTITY.json", jsonbytes(dict(project_commit=HEAD, tested_source=SOURCE,
    base=BASE, openfhe_commit=PIN, branch="codex/lossless-io-implementation-01",
    dirty_state="clean", scope="Final N64/S16 first-Mult2 source review; no1000 trials",
    historical_document_rule="Earlier unconfirmed-seam, pending-B and1000-run statements are historical, superseded by TASK.md and current scope.",
    upstream_citation_rule="Map every older official-openfhe/<basename> citation to the unique matching basename under official-full/, then verify original canonical Git path.")),
    dict(kind="generated-provenance"))
selection = b"\n".join(n.encode() + b"\n" + payload[n] for n in sorted(payload))
source_scan = scan(selection, "source-selection")
manifest = jsonbytes(dict(self_excluded=["MANIFEST.json"], files=[
    dict(path=n, bytes=len(payload[n]), sha256=sha(payload[n]), origin=origins[n])
    for n in sorted(payload)]))
payload["MANIFEST.json"] = manifest
with zipfile.ZipFile(OUTPUT, "x", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
    for n in sorted(payload):
        i = zipfile.ZipInfo(ROOT + "/" + n, (2026, 9, 5, 0, 0, 0))
        i.create_system = 3
        i.external_attr = (stat.S_IFREG | 0o644) << 16
        i.compress_type = zipfile.ZIP_DEFLATED
        archive.writestr(i, payload[n], compresslevel=6)
with zipfile.ZipFile(OUTPUT) as archive:
    assert archive.testzip() is None
    assert len(archive.namelist()) == len(payload)
    for i in archive.infolist():
        helpers.validate_zip_info(i, expected_root=ROOT)
        n = i.filename[len(ROOT) + 1:]
        assert archive.read(i) == payload[n]
    final_scan = scan(b"\n".join(i.filename.encode() + b"\n" + archive.read(i)
        for i in archive.infolist()), "final-archive-decoded-content")
print(json.dumps(dict(archive=str(OUTPUT), bytes=OUTPUT.stat().st_size,
    sha256=sha(OUTPUT.read_bytes()), root=ROOT, members=len(payload),
    payload_bytes=sum(len(b) for b in payload.values()), manifest_sha256=sha(manifest),
    source_scan=source_scan, final_scan=final_scan,
    official_files=len([p for p in payload if p.startswith("official-full/")]),
    source_commit=HEAD, tested_source=SOURCE, git_status="clean",
    included_manifest=json.loads(manifest), status="built-and-scanned-NOT-UPLOADED"),
    ensure_ascii=False, indent=2))
