#!/bin/zsh
set -euo pipefail

umask 077
exec 3>&2

fail() {
  print -u2 -- "Fable5 launch preflight failed: $*"
  print -u3 -- "FAIL: $*"
  exit 70
}

log() {
  print -u3 -- "$*"
}

sha256_of() {
  /usr/bin/shasum -a 256 "$1" | /usr/bin/awk '{print $1}'
}

require_sha256() {
  local candidate_file="$1"
  local expected="$2"
  local actual
  actual="$(sha256_of "${candidate_file}")"
  [[ "${actual}" == "${expected}" ]] || fail "SHA-256 mismatch for ${candidate_file}: ${actual}"
  log "sha256 ${actual}  ${candidate_file}"
}

require_readonly_regular_tree() {
  local root="$1"
  local label="$2"
  local audit_output
  local audit_exit
  set +e
  audit_output="$(
    /usr/bin/env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin \
      /usr/bin/python3 -I -S -E -c '
import os
import stat
import sys

root = sys.argv[1]
errors = []
entry_count = 0

try:
    root_status = os.lstat(root)
except OSError as exception:
    print(f"root_lstat_error={exception.__class__.__name__}")
    sys.exit(1)

if not stat.S_ISDIR(root_status.st_mode) or stat.S_ISLNK(root_status.st_mode):
    print("root_type_error=true")
    sys.exit(1)
if root_status.st_mode & 0o222:
    print("root_writable=true")
    sys.exit(1)

def walk_error(exception):
    errors.append(exception)

for directory, names, files in os.walk(root, topdown=True, followlinks=False, onerror=walk_error):
    for name in names + files:
        candidate = os.path.join(directory, name)
        try:
            candidate_status = os.lstat(candidate)
        except OSError as exception:
            errors.append(exception)
            continue
        entry_count += 1
        if stat.S_ISLNK(candidate_status.st_mode):
            errors.append(RuntimeError("symlink"))
        elif not (stat.S_ISDIR(candidate_status.st_mode) or stat.S_ISREG(candidate_status.st_mode)):
            errors.append(RuntimeError("non_regular_entry"))
        elif candidate_status.st_mode & 0o222:
            errors.append(RuntimeError("writable_entry"))

if errors:
    print(f"tree_error_count={len(errors)}")
    sys.exit(1)
print(f"readonly_entry_count={entry_count}")
' "${root}"
  )"
  audit_exit=$?
  set -e
  [[ ${audit_exit} -eq 0 ]] || fail "${label} read-only regular-tree audit failed: ${audit_output}"
  log "${label// /_}_${audit_output}"
}

targeted_scan() {
  local root="$1"
  local output="$2"
  local scan_exit
  set +e
  "${FABLE_RG}" --hidden --no-messages -n -I \
    -e '-----BEGIN[[:space:]]+(RSA[[:space:]]+|EC[[:space:]]+|OPENSSH[[:space:]]+|DSA[[:space:]]+)?PRIVATE KEY-----' \
    -e '(?i)(api[_-]?key|access[_-]?token|refresh[_-]?token|password|secret)[[:space:]]*[:=][[:space:]]*[A-Za-z0-9_./+=:-]{16,}' \
    -e '(?i)authorization[[:space:]]*:[[:space:]]*bearer[[:space:]]+[A-Za-z0-9._~+/-]{16,}' \
    -e 'AKIA[0-9A-Z]{16}' \
    -e 'gh[pousr]_[A-Za-z0-9]{20,}' \
    -e 'sk-ant-[A-Za-z0-9_-]{20,}' \
    -e 'sk-proj-[A-Za-z0-9_-]{20,}' \
    -e 'AIza[0-9A-Za-z_-]{35}' \
    -e 'xox[baprs]-[A-Za-z0-9-]{16,}' \
    "${root}" >/dev/null 2>&1
  scan_exit=$?
  set -e
  print -- "targeted_scan_exit=${scan_exit}" >"${output}"
  print -- "targeted_scan_root=${root}" >>"${output}"
  print -- "finding_content_retained=false" >>"${output}"
  [[ ${scan_exit} -eq 1 ]] || fail "targeted credential scan exit ${scan_exit} for ${root}"
  log "targeted_scan_exit=1(no matches) root=${root}"
}

exact_secret_scan() {
  local output="$1"
  shift
  local scan_exit
  set +e
  {
    print -rn -- "${fable_oauth_token}"
    print -rn -- $'\0'
    print -rn -- "${fable_refresh_token}"
    print -rn -- $'\0'
  } | /usr/bin/env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin \
    /usr/bin/python3 -I -S -E -c '
import os
import stat
import sys

parts = sys.stdin.buffer.read().split(b"\0")
if len(parts) != 3 or parts[2] != b"" or not parts[0] or not parts[1]:
    print("exact_secret_scan_exit=2")
    print("exact_secret_scan_error=invalid_secret_transport")
    sys.exit(2)

secrets = parts[:2]
matched_files = 0
scan_errors = 0

def root_files(raw_root):
    root_mode = os.lstat(raw_root).st_mode
    if stat.S_ISLNK(root_mode):
        raise RuntimeError("scan root is a symlink")
    if stat.S_ISREG(root_mode):
        yield raw_root
        return
    if not stat.S_ISDIR(root_mode):
        raise RuntimeError("scan root is not a regular file or directory")

    walk_errors = []
    for directory, names, files in os.walk(
        raw_root, topdown=True, followlinks=False, onerror=walk_errors.append
    ):
        retained_names = []
        for name in names:
            candidate = os.path.join(directory, name)
            try:
                candidate_mode = os.lstat(candidate).st_mode
            except OSError:
                walk_errors.append(candidate)
                continue
            if stat.S_ISDIR(candidate_mode) and not stat.S_ISLNK(candidate_mode):
                retained_names.append(name)
            else:
                walk_errors.append(candidate)
        names[:] = retained_names
        for name in files:
            candidate = os.path.join(directory, name)
            try:
                candidate_mode = os.lstat(candidate).st_mode
            except OSError:
                walk_errors.append(candidate)
                continue
            if stat.S_ISREG(candidate_mode) and not stat.S_ISLNK(candidate_mode):
                yield candidate
            else:
                walk_errors.append(candidate)
    if walk_errors:
        raise RuntimeError("scan traversal encountered an unreadable or non-regular entry")

def read_regular_no_follow(candidate):
    descriptor = os.open(candidate, os.O_RDONLY | getattr(os, "O_NOFOLLOW", 0))
    try:
        if not stat.S_ISREG(os.fstat(descriptor).st_mode):
            raise RuntimeError("scan candidate changed away from a regular file")
        with os.fdopen(descriptor, "rb", closefd=False) as source:
            return source.read()
    finally:
        os.close(descriptor)

for raw_root in sys.argv[1:]:
    try:
        for candidate in root_files(raw_root):
            try:
                content = read_regular_no_follow(candidate)
            except (OSError, RuntimeError):
                scan_errors += 1
                continue
            if any(secret in content for secret in secrets):
                matched_files += 1
    except (OSError, RuntimeError):
        scan_errors += 1

scan_exit = 0 if matched_files == 0 and scan_errors == 0 else 1
print(f"exact_secret_scan_exit={scan_exit}")
print(f"exact_secret_match_count={matched_files}")
print(f"exact_secret_scan_error_count={scan_errors}")
print("finding_content_retained=false")
sys.exit(scan_exit)
' "$@" >"${output}" 2>&1
  scan_exit=$?
  set -e
  [[ ${scan_exit} -eq 0 ]] || fail "exact OAuth-token scan failed"
  log "exact_oauth_token_scan_exit=0 roots=$*"
}

run_fable_cli() {
  local operation="$1"
  {
    print -rn -- "${fable_oauth_token}"
    print -rn -- $'\0'
    print -rn -- "${fable_refresh_token}"
    print -rn -- $'\0'
  } | /usr/bin/env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin \
    /usr/bin/python3 -I -S -E -c '
import hashlib
import os
import stat
import sys

parts = sys.stdin.buffer.read().split(b"\0")
if len(parts) != 3 or parts[2] != b"" or not parts[0] or not parts[1]:
    sys.exit(74)

try:
    access_token = parts[0].decode("utf-8", "strict")
    refresh_token = parts[1].decode("utf-8", "strict")
except UnicodeDecodeError:
    sys.exit(75)

operation = sys.argv[1]
expected_task_sha256 = sys.argv[2]
expected_task_bytes = int(sys.argv[3])
sandbox_exec = "/usr/bin/sandbox-exec"
sandbox_profile = "/Users/lifeng/Documents/20231788-openfhe-cleanroom-20260831/coordination/handoffs/fable5-relin2-validation-84df651-review-01/sandbox.sb"
cli = "/Users/lifeng/.local/share/claude/versions/2.1.239"
task = "/Users/lifeng/Documents/20231788-openfhe-cleanroom-20260831/coordination/tasks/fable5-relin2-validation-84df651-review-01.md"
common = [
    "-p", "--model", "claude-fable-5", "--effort", "max",
    "--permission-mode", "plan", "--safe-mode", "--no-chrome",
    "--no-session-persistence", "--disable-slash-commands",
    "--prompt-suggestions", "false", "--strict-mcp-config",
    "--mcp-config", "{}", "--tools", "Read,Glob,Grep",
    "--max-budget-usd", "5.00", "--output-format", "stream-json",
    "--verbose",
]

if operation == "auth":
    arguments = [sandbox_exec, "-f", sandbox_profile, cli, "auth", "status"]
elif operation == "flags":
    arguments = [sandbox_exec, "-f", sandbox_profile, cli, *common, "--help"]
elif operation == "provider":
    descriptor = os.open(task, os.O_RDONLY | getattr(os, "O_NOFOLLOW", 0))
    task_status = os.fstat(descriptor)
    if not stat.S_ISREG(task_status.st_mode) or task_status.st_size != expected_task_bytes:
        os.close(descriptor)
        sys.exit(76)
    task_digest = hashlib.sha256()
    while True:
        task_chunk = os.read(descriptor, 1024 * 1024)
        if not task_chunk:
            break
        task_digest.update(task_chunk)
    if task_digest.hexdigest() != expected_task_sha256:
        os.close(descriptor)
        sys.exit(76)
    os.lseek(descriptor, 0, os.SEEK_SET)
    os.dup2(descriptor, 0)
    os.close(descriptor)
    arguments = [sandbox_exec, "-f", sandbox_profile, cli, *common]
else:
    sys.exit(77)

environment = {
    "CLAUDE_CODE_OAUTH_TOKEN": access_token,
    "CLAUDE_CODE_OAUTH_REFRESH_TOKEN": refresh_token,
    "HOME": "/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/claude-home",
    "TMPDIR": "/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/claude-tmp",
    "PATH": "/usr/bin:/bin:/usr/sbin:/sbin",
    "LANG": "en_US.UTF-8",
    "LC_ALL": "en_US.UTF-8",
}
expected_names = {
    "CLAUDE_CODE_OAUTH_TOKEN", "CLAUDE_CODE_OAUTH_REFRESH_TOKEN", "HOME",
    "TMPDIR", "PATH", "LANG", "LC_ALL",
}
if set(environment) != expected_names:
    sys.exit(78)
os.execve(sandbox_exec, arguments, environment)
' "${operation}" "${EXPECTED_TASK_SHA256}" "${EXPECTED_TASK_BYTES}"
}

readonly FABLE_COORDINATION="/Users/lifeng/Documents/20231788-openfhe-cleanroom-20260831"
readonly FABLE_IMPLEMENTATION="/Users/lifeng/Documents/20231788-openfhe-codex-relin2-01"
readonly FABLE_CLI="/Users/lifeng/.local/share/claude/versions/2.1.239"
readonly FABLE_TASK="${FABLE_COORDINATION}/coordination/tasks/fable5-relin2-validation-84df651-review-01.md"
readonly FABLE_HANDOFF="${FABLE_COORDINATION}/coordination/handoffs/fable5-relin2-validation-84df651-review-01"
readonly FABLE_LAUNCHER="${FABLE_HANDOFF}/launch.sh"
readonly FABLE_SANDBOX="${FABLE_HANDOFF}/sandbox.sb"
readonly FABLE_ROOT="/private/var/tmp/fable5-relin2-84df651.ZLu6Uh"
readonly FABLE_PACKET="${FABLE_ROOT}/fable5-relin2-validation-84df651-review-packet.zip"
readonly FABLE_STAGING="${FABLE_ROOT}/staging"
readonly FABLE_EXTRACTION="${FABLE_ROOT}/fresh-extraction"
readonly FABLE_HOME="${FABLE_ROOT}/claude-home"
readonly FABLE_TMP="${FABLE_ROOT}/claude-tmp"
readonly FABLE_RECEIPT="${FABLE_ROOT}/receipt-attempt-${$}"
readonly FABLE_SCAN_TMP="${FABLE_ROOT}/receipt-scan-${$}"
readonly FABLE_ENTRY_LOCK="${FABLE_ROOT}/launcher-entry.lock"
readonly FABLE_STARTED_GUARD="${FABLE_ROOT}/provider-started.once"
readonly FABLE_GITLEAKS="/opt/homebrew/bin/gitleaks"
readonly FABLE_RG="/Applications/ChatGPT.app/Contents/Resources/rg"
readonly FABLE_PYTHON="/usr/bin/python3"

readonly EXPECTED_TASK_SHA256="e95db7704faffc51cfc70b3f4ca85b6182cc62bf6f077e334655bb70b158b99b"
readonly EXPECTED_TASK_BYTES="16794"
readonly EXPECTED_SANDBOX_SHA256="680fec15a149b95801cbc8dccf377661f5c756f28336e8565ce268359b8640b8"
readonly EXPECTED_CLI_SHA256="2b4f7aafdaa65bcc2335f56a4b276317837203f2c5587b1f2a17ca78ad14e36f"
readonly EXPECTED_GITLEAKS_SHA256="f414bc2fb952be6c9072b75cb411e3368614ef4b16d48dbd9ad238034afd2302"
readonly EXPECTED_RG_SHA256="b4cecd1ca4dd88c0636d4847ab83fdca328a0930df2c001963fe84f0d23476fe"
readonly EXPECTED_PYTHON_SHA256="a961f78075d8e7621ef4f5d764c64ef8a41bf66c0a98ab5cb6ca39b85ce31c93"
readonly EXPECTED_PACKET_SHA256="98e51fa1a020bb49a97927282da4e767c6d85be165da79d9e5b794576f2eb19e"
readonly EXPECTED_PACKET_BYTES="845608"
readonly EXPECTED_MEMBER_LIST_SHA256="1026987f2d5ad0c5cb8c66eee559bdf11ab4cf7469acf43161fbbc6656529303"
readonly EXPECTED_MANIFEST_SHA256="a00551b1faf346c824181b507a6ff92d370c869ae3b80c63c4a0b07808a38608"
readonly EXPECTED_IMPLEMENTATION_HEAD="84df6518df47fc7e50b8f465e5aa294fe5fdf84d"
readonly EXPECTED_IMPLEMENTATION_TREE="028033ad13b90710eaa98e6f1436bdb2d8f49b86"
readonly EXPECTED_IMPLEMENTATION_BRANCH="agent/codex-relin2-01"
readonly EXPECTED_COORDINATION_BRANCH="cleanroom/reimplement-mult2-20260831"

typeset -a expected_members
expected_members=(
  MANIFEST.sha256
  PACKET_SCOPE.md
  evidence/green/84df651.diff
  evidence/green/coordination-receipt.md
  evidence/green/hosted-receipt.md
  evidence/green/jobs-terminal.json
  evidence/green/linux-job.log
  evidence/green/run-terminal.json
  evidence/red/coordination-receipt.md
  evidence/red/f2deacb.diff
  evidence/red/hosted-receipt.md
  evidence/red/jobs-terminal.json
  evidence/red/linux-job.log
  evidence/red/run-terminal.json
  paper/2023.1788.pdf
  paper/2023.1788.txt
  source/.github/workflows/dcp-rcb.yml
  source/CMakeLists.txt
  source/README.md
  source/include/openfhe_2023_1788/double_ckks.h
  source/src/double_ckks.cpp
  source/tests/dcp_rcb_test.cpp
  source/tests/relin2_api_contract_test.cpp
  source/tests/relin2_test.cpp
  source/tests/tensor2_api_contract_test.cpp
  source/tests/tensor2_test.cpp
  spec/original-relin2-contract.md
  spec/relin2-preflight.md
)

/bin/mkdir "${FABLE_RECEIPT}" || fail "unique receipt directory creation failed"
exec 3>"${FABLE_RECEIPT}/00-preflight.txt"
print -u2 -- "Fable5 attempt receipt: ${FABLE_RECEIPT}"
log "preflight_started_utc=$(/bin/date -u '+%Y-%m-%dT%H:%M:%SZ')"
log "invocation_cwd=$(pwd -P)"
[[ "$(pwd -P)" == "${FABLE_EXTRACTION}" ]] || fail "canonical CWD is not ${FABLE_EXTRACTION}"
[[ ! -e "${FABLE_STARTED_GUARD}" ]] || fail "one-use provider-start guard already exists"
/bin/mkdir "${FABLE_ENTRY_LOCK}" || fail "exclusive launcher-entry lock creation failed"
print -- "pid=${$}" >"${FABLE_ENTRY_LOCK}/pid.txt"
print -- "entry_utc=$(/bin/date -u '+%Y-%m-%dT%H:%M:%SZ')" >"${FABLE_ENTRY_LOCK}/entry-utc.txt"

typeset -a receipt_outputs
receipt_outputs=(
  00-preflight.txt
  00b-exact-pre-auth-secrets.txt
  01-auth-status.json
  01a-auth-stderr.txt
  01b-auth-exit.txt
  01c-flags-stdout.txt
  01d-flags-stderr.txt
  01e-flags-exit.txt
  01f-exact-post-auth-secrets.txt
  02-sandbox-probes.txt
  03-staging-gitleaks.txt
  04-extraction-gitleaks.txt
  05-runtime-home-gitleaks.txt
  06-handoff-gitleaks.txt
  07-task-gitleaks.txt
  08-targeted-staging.txt
  09-targeted-extraction.txt
  10-targeted-runtime-home.txt
  10b-runtime-tmp-gitleaks.txt
  10c-targeted-runtime-tmp.txt
  10d-exact-pre-provider-secrets.txt
  11-members.txt
  12-manifest-check.txt
  13-start.txt
  14-fable.raw.jsonl
  15-fable.stderr.txt
  16-exit.txt
  17-postflight.txt
  18-exact-post-provider-secrets.txt
  20-post-runtime-home-gitleaks.txt
  21-post-runtime-home-targeted.txt
  21b-post-runtime-tmp-gitleaks.txt
  21c-post-runtime-tmp-targeted.txt
  22-events.json
  23-terminal-answer.md
  24-session-usage.json
  25-tool-uses.json
  25b-tool-results.json
  26-stream-summary.txt
  27-final-receipt-gitleaks.txt
  28-final-receipt-targeted.txt
  29-receipt-manifest.sha256
)
[[ "$(/usr/bin/printf '%s\n' "${receipt_outputs[@]}" | /usr/bin/sort -u | /usr/bin/wc -l | /usr/bin/tr -d ' ')" == "${#receipt_outputs[@]}" ]] ||
  fail "receipt output list contains duplicate names"
for output_name in "${receipt_outputs[@]}"; do
  [[ "${output_name}" == "00-preflight.txt" ]] && continue
  [[ ! -e "${FABLE_RECEIPT}/${output_name}" ]] || fail "receipt output already exists: ${output_name}"
done

log "canonical_cwd=$(pwd -P)"
log "entry_lock=${FABLE_ENTRY_LOCK}"
preflight_launcher_sha="$(sha256_of "${FABLE_LAUNCHER}")"
log "launcher_sha256=${preflight_launcher_sha}"

[[ -f "${FABLE_TASK}" && -f "${FABLE_SANDBOX}" && -x "${FABLE_CLI}" && -x "${FABLE_PYTHON}" ]] || fail "task, sandbox, CLI, or credential transporter is unavailable"
[[ -d "${FABLE_STAGING}" && -d "${FABLE_EXTRACTION}" && -d "${FABLE_HOME}" && -d "${FABLE_TMP}" ]] || fail "staging, extraction, isolated HOME, or isolated TMP is unavailable"
[[ "$(cd "${FABLE_HOME}" && pwd -P)" == "${FABLE_HOME}" ]] || fail "isolated HOME realpath mismatch"
[[ "$(cd "${FABLE_TMP}" && pwd -P)" == "${FABLE_TMP}" ]] || fail "isolated TMP realpath mismatch"
[[ -z "$(/usr/bin/find "${FABLE_HOME}" "${FABLE_TMP}" -mindepth 1 -print -quit)" ]] || fail "isolated HOME or TMP is not initially empty"
[[ -z "$(/usr/bin/find "${FABLE_HOME}" "${FABLE_TMP}" -type l -print -quit)" ]] || fail "isolated HOME or TMP contains a symlink"
[[ -z "$(/usr/bin/find "${FABLE_HOME}" "${FABLE_TMP}" ! -type d ! -type f -print -quit)" ]] || fail "isolated HOME or TMP contains a non-regular entry"
for startup_name in .zshenv .zprofile .zshrc .zlogin .zlogout; do
  [[ ! -e "${FABLE_HOME}/${startup_name}" ]] || fail "isolated HOME contains a shell startup file: ${startup_name}"
done
log "isolated_home_tmp_realpaths=exact initial_entries=0 startup_files=absent symlinks=0 non_regular_entries=0"
[[ "$(/usr/bin/stat -f '%z' "${FABLE_TASK}")" == "${EXPECTED_TASK_BYTES}" ]] || fail "task byte size mismatch"
[[ "$(/usr/bin/stat -f '%z' "${FABLE_PACKET}")" == "${EXPECTED_PACKET_BYTES}" ]] || fail "packet byte size mismatch"
require_sha256 "${FABLE_TASK}" "${EXPECTED_TASK_SHA256}"
require_sha256 "${FABLE_SANDBOX}" "${EXPECTED_SANDBOX_SHA256}"
require_sha256 "${FABLE_CLI}" "${EXPECTED_CLI_SHA256}"
require_sha256 "${FABLE_GITLEAKS}" "${EXPECTED_GITLEAKS_SHA256}"
require_sha256 "${FABLE_RG}" "${EXPECTED_RG_SHA256}"
require_sha256 "${FABLE_PYTHON}" "${EXPECTED_PYTHON_SHA256}"
require_sha256 "${FABLE_PACKET}" "${EXPECTED_PACKET_SHA256}"
[[ "$("${FABLE_CLI}" --version)" == "2.1.239 (Claude Code)" ]] || fail "CLI version mismatch"
[[ "$("${FABLE_GITLEAKS}" version)" == "8.30.1" ]] || fail "Gitleaks version mismatch"
[[ "$("${FABLE_PYTHON}" --version 2>&1)" == "Python 3.9.6" ]] || fail "credential transporter version mismatch"

for repository in "${FABLE_COORDINATION}" "${FABLE_IMPLEMENTATION}"; do
  [[ -z "$(/usr/bin/git -C "${repository}" status --porcelain=v1)" ]] || fail "dirty repository: ${repository}"
  /usr/bin/git -C "${repository}" diff --check >&3
  /usr/bin/git -C "${repository}" diff --cached --check >&3
done

coordination_branch="$(/usr/bin/git -C "${FABLE_COORDINATION}" branch --show-current)"
coordination_head="$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse HEAD)"
coordination_tree="$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse HEAD^{tree})"
coordination_upstream="$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse '@{u}')"
coordination_remote="$(/usr/bin/git -C "${FABLE_COORDINATION}" ls-remote --exit-code origin "refs/heads/${coordination_branch}" | /usr/bin/awk '{print $1}')"
[[ "${coordination_branch}" == "${EXPECTED_COORDINATION_BRANCH}" ]] || fail "coordination branch mismatch"
[[ "${coordination_head}" == "${coordination_upstream}" && "${coordination_head}" == "${coordination_remote}" ]] || fail "coordination local/upstream/remote mismatch"
log "coordination_branch=${coordination_branch}"
log "coordination_head=${coordination_head}"
log "coordination_tree=${coordination_tree}"
log "coordination_upstream=${coordination_upstream}"
log "coordination_remote=${coordination_remote}"

implementation_branch="$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" branch --show-current)"
implementation_head="$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse HEAD)"
implementation_tree="$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse HEAD^{tree})"
implementation_upstream="$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse '@{u}')"
implementation_remote="$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" ls-remote --exit-code origin "refs/heads/${implementation_branch}" | /usr/bin/awk '{print $1}')"
[[ "${implementation_branch}" == "${EXPECTED_IMPLEMENTATION_BRANCH}" ]] || fail "implementation branch mismatch"
[[ "${implementation_head}" == "${EXPECTED_IMPLEMENTATION_HEAD}" ]] || fail "implementation HEAD mismatch"
[[ "${implementation_tree}" == "${EXPECTED_IMPLEMENTATION_TREE}" ]] || fail "implementation tree mismatch"
[[ "${implementation_head}" == "${implementation_upstream}" && "${implementation_head}" == "${implementation_remote}" ]] || fail "implementation local/upstream/remote mismatch"
log "implementation_branch=${implementation_branch}"
log "implementation_head=${implementation_head}"
log "implementation_tree=${implementation_tree}"
log "implementation_upstream=${implementation_upstream}"
log "implementation_remote=${implementation_remote}"

typeset -a actual_members
actual_members=("${(@f)$(/usr/bin/unzip -Z1 "${FABLE_PACKET}")}")
[[ ${#actual_members[@]} -eq ${#expected_members[@]} ]] || fail "packet member count mismatch"
for ((member_index = 1; member_index <= ${#expected_members[@]}; ++member_index)); do
  [[ "${actual_members[member_index]}" == "${expected_members[member_index]}" ]] || fail "packet member mismatch at ${member_index}: ${actual_members[member_index]}"
done
/usr/bin/printf '%s\n' "${actual_members[@]}" >"${FABLE_RECEIPT}/11-members.txt"
member_list_sha="$(/usr/bin/unzip -Z1 "${FABLE_PACKET}" | /usr/bin/shasum -a 256 | /usr/bin/awk '{print $1}')"
[[ "${member_list_sha}" == "${EXPECTED_MEMBER_LIST_SHA256}" ]] || fail "ordered member-list SHA mismatch"
typeset -a member_modes
member_modes=("${(@f)$(/usr/bin/zipinfo -l "${FABLE_PACKET}" | /usr/bin/awk '$1 ~ /^-/ {print $1}')}")
[[ ${#member_modes[@]} -eq 28 ]] || fail "packet regular-member mode count mismatch"
for member_mode in "${member_modes[@]}"; do
  [[ "${member_mode}" == "-rw-r--r--" ]] || fail "unexpected packet member mode: ${member_mode}"
done
[[ "$(/usr/bin/zipinfo -v "${FABLE_PACKET}" | "${FABLE_RG}" -c 'file security status:.*not encrypted')" == "28" ]] || fail "packet encryption-status count mismatch"
log "packet_members=28 member_list_sha256=${member_list_sha} modes=100644 encryption=none"

[[ "$(/usr/bin/find "${FABLE_STAGING}" -type f | /usr/bin/wc -l | /usr/bin/tr -d ' ')" == "28" ]] || fail "staging regular-file count mismatch"
[[ "$(/usr/bin/find "${FABLE_EXTRACTION}" -type f | /usr/bin/wc -l | /usr/bin/tr -d ' ')" == "28" ]] || fail "extraction regular-file count mismatch"
[[ -z "$(/usr/bin/find "${FABLE_STAGING}" -type l -print -quit)" ]] || fail "staging contains a symlink"
[[ -z "$(/usr/bin/find "${FABLE_EXTRACTION}" -type l -print -quit)" ]] || fail "extraction contains a symlink"
require_readonly_regular_tree "${FABLE_EXTRACTION}" "extraction"
/usr/bin/diff -qr "${FABLE_STAGING}" "${FABLE_EXTRACTION}" >&3 || fail "staging/extraction byte mismatch"
(
  cd "${FABLE_EXTRACTION}"
  [[ "$(sha256_of MANIFEST.sha256)" == "${EXPECTED_MANIFEST_SHA256}" ]] || exit 71
  /usr/bin/shasum -a 256 MANIFEST.sha256
  /usr/bin/shasum -a 256 --check MANIFEST.sha256
) >"${FABLE_RECEIPT}/12-manifest-check.txt" 2>&1 || fail "internal manifest check failed"
log "staging_extraction_identical=true extraction_writable_entries=0 symlinks=0"

"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_STAGING}" >"${FABLE_RECEIPT}/03-staging-gitleaks.txt" 2>&1 || fail "staging Gitleaks scan failed"
"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_EXTRACTION}" >"${FABLE_RECEIPT}/04-extraction-gitleaks.txt" 2>&1 || fail "extraction Gitleaks scan failed"
"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_HANDOFF}" >"${FABLE_RECEIPT}/06-handoff-gitleaks.txt" 2>&1 || fail "handoff Gitleaks scan failed"
"${FABLE_GITLEAKS}" detect --no-banner --redact --no-git --source "${FABLE_TASK}" >"${FABLE_RECEIPT}/07-task-gitleaks.txt" 2>&1 || fail "task Gitleaks scan failed"
targeted_scan "${FABLE_STAGING}" "${FABLE_RECEIPT}/08-targeted-staging.txt"
targeted_scan "${FABLE_EXTRACTION}" "${FABLE_RECEIPT}/09-targeted-extraction.txt"

typeset -a parent_read_probe_paths
parent_read_probe_paths=(
  "/Users/lifeng/.claude.json"
  "/Users/lifeng/Library/Application Support/Google/Chrome/Default/Cookies"
  "/Library/Keychains/System.keychain"
  "/System/Volumes/Data/Users/lifeng/.claude.json"
  "/System/Volumes/Data/Users/lifeng/Library/Application Support/Google/Chrome/Default/Cookies"
  "/System/Volumes/Data/Library/Keychains/System.keychain"
)
for probe_candidate in "${parent_read_probe_paths[@]}"; do
  [[ -f "${probe_candidate}" && -r "${probe_candidate}" ]] ||
    fail "sandbox negative-probe parent read control is unavailable"
  /usr/bin/head -c 1 "${probe_candidate}" >/dev/null 2>&1 ||
    fail "sandbox negative-probe parent read control failed"
done
/usr/bin/env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8 \
  /usr/bin/security list-keychains >/dev/null 2>&1 ||
  fail "unsandboxed keychain-service positive control failed"

print -- "isolated HOME read-denial canary" >"${FABLE_HOME}/.fable-read-denial-canary"
/bin/chmod 600 "${FABLE_HOME}/.fable-read-denial-canary"
sandbox_probe_output="$(
  /usr/bin/env -i PATH=/usr/bin:/bin:/usr/sbin:/sbin LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8 \
    /usr/bin/sandbox-exec -f "${FABLE_SANDBOX}" /bin/zsh -c '
      probe_read() {
        if /usr/bin/head -c 1 "$1" >/dev/null 2>&1; then
          printf "%s=allowed\n" "$2"
        else
          printf "%s=denied\n" "$2"
        fi
      }
      probe_read "/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/fresh-extraction/MANIFEST.sha256" packet_read
      probe_read "/Users/lifeng/Documents/20231788-openfhe-cleanroom-20260831/coordination/tasks/fable5-relin2-validation-84df651-review-01.md" task_read
      probe_read "/Users/lifeng/.claude.json" real_claude_config_read
      probe_read "/Users/lifeng/Library/Application Support/Google/Chrome/Default/Cookies" chrome_cookie_read
      probe_read "/Library/Keychains/System.keychain" system_keychain_read
      probe_read "/System/Volumes/Data/Users/lifeng/.claude.json" data_alias_claude_config_read
      probe_read "/System/Volumes/Data/Users/lifeng/Library/Application Support/Google/Chrome/Default/Cookies" data_alias_chrome_cookie_read
      probe_read "/System/Volumes/Data/Library/Keychains/System.keychain" data_alias_system_keychain_read
      probe_read "/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/claude-home/.fable-read-denial-canary" isolated_home_read
      probe_read "/private/etc/hosts" hosts_read
      if /usr/bin/security list-keychains >/dev/null 2>&1; then
        printf "keychain_service=allowed\n"
      else
        printf "keychain_service=denied\n"
      fi
      if /usr/bin/touch "/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/fresh-extraction/.sandbox-write-probe" 2>/dev/null; then
        printf "packet_write=allowed\n"
      else
        printf "packet_write=denied\n"
      fi
      if /usr/bin/touch "/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/claude-tmp/sandbox-write-probe" 2>/dev/null; then
        printf "isolated_tmp_write=allowed\n"
      else
        printf "isolated_tmp_write=denied\n"
      fi
    '
)"
readonly expected_sandbox_probe_output=$'packet_read=allowed\ntask_read=allowed\nreal_claude_config_read=denied\nchrome_cookie_read=denied\nsystem_keychain_read=denied\ndata_alias_claude_config_read=denied\ndata_alias_chrome_cookie_read=denied\ndata_alias_system_keychain_read=denied\nisolated_home_read=denied\nhosts_read=allowed\nkeychain_service=denied\npacket_write=denied\nisolated_tmp_write=allowed'
[[ "${sandbox_probe_output}" == "${expected_sandbox_probe_output}" ]] || fail "sandbox probe matrix mismatch"
{
  print -- "parent_sensitive_read_positive_controls=${#parent_read_probe_paths[@]}"
  print -- "parent_env_i_keychain_service_positive_control=passed"
  print -r -- "${sandbox_probe_output}"
} >"${FABLE_RECEIPT}/02-sandbox-probes.txt"
/bin/rm -f "${FABLE_TMP}/sandbox-write-probe"
/bin/rm -f "${FABLE_HOME}/.fable-read-denial-canary"
[[ ! -e "${FABLE_EXTRACTION}/.sandbox-write-probe" ]] || fail "packet write probe unexpectedly created a file"
[[ ! -e "${FABLE_HOME}/.fable-read-denial-canary" ]] || fail "isolated HOME read-denial canary cleanup failed"

typeset fable_oauth_token fable_refresh_token
fable_credentials="$(/usr/bin/security find-generic-password -s 'Claude Code-credentials' -w)"
fable_oauth_token="$(print -rn -- "${fable_credentials}" | /usr/bin/jq -er '.claudeAiOauth.accessToken | select(type == "string" and length > 0)')"
fable_refresh_token="$(print -rn -- "${fable_credentials}" | /usr/bin/jq -er '.claudeAiOauth.refreshToken | select(type == "string" and length > 0)')"
fable_access_expires_ms="$(print -rn -- "${fable_credentials}" | /usr/bin/jq -er '.claudeAiOauth.expiresAt | numbers')"
fable_refresh_expires_ms="$(print -rn -- "${fable_credentials}" | /usr/bin/jq -er '.claudeAiOauth.refreshTokenExpiresAt | numbers')"
unset fable_credentials
now_ms=$(( $(/bin/date +%s) * 1000 ))
refresh_remaining_seconds=$(( (fable_refresh_expires_ms - now_ms) / 1000 ))
access_remaining_seconds=$(( (fable_access_expires_ms - now_ms) / 1000 ))
(( refresh_remaining_seconds > 14400 )) || fail "OAuth refresh token has less than four hours remaining"
log "oauth_access_remaining_seconds=${access_remaining_seconds}"
log "oauth_refresh_remaining_seconds=${refresh_remaining_seconds}"

exact_secret_scan "${FABLE_RECEIPT}/00b-exact-pre-auth-secrets.txt" "${FABLE_HOME}" "${FABLE_TMP}" "${FABLE_RECEIPT}"
set +e
run_fable_cli auth >"${FABLE_RECEIPT}/01-auth-status.json" 2>"${FABLE_RECEIPT}/01a-auth-stderr.txt"
auth_exit=$?
set -e
print -- "auth_exit=${auth_exit}" >"${FABLE_RECEIPT}/01b-auth-exit.txt"
exact_secret_scan "${FABLE_RECEIPT}/01f-exact-post-auth-secrets.txt" "${FABLE_HOME}" "${FABLE_TMP}" "${FABLE_RECEIPT}"
[[ ${auth_exit} -eq 0 ]] || fail "isolated OAuth status command failed with exit ${auth_exit}"
/usr/bin/jq -e '.loggedIn == true and .authMethod == "oauth_token" and .apiProvider == "firstParty"' "${FABLE_RECEIPT}/01-auth-status.json" >/dev/null || fail "isolated OAuth status mismatch"

set +e
run_fable_cli flags >"${FABLE_RECEIPT}/01c-flags-stdout.txt" 2>"${FABLE_RECEIPT}/01d-flags-stderr.txt"
flags_exit=$?
set -e
print -- "flags_exit=${flags_exit}" >"${FABLE_RECEIPT}/01e-flags-exit.txt"
exact_secret_scan "${FABLE_RECEIPT}/10d-exact-pre-provider-secrets.txt" "${FABLE_HOME}" "${FABLE_TMP}" "${FABLE_RECEIPT}"
[[ ${flags_exit} -eq 0 ]] || fail "exact provider flag parse failed with exit ${flags_exit}"
"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_HOME}" >"${FABLE_RECEIPT}/05-runtime-home-gitleaks.txt" 2>&1 || fail "post-auth isolated HOME Gitleaks scan failed"
targeted_scan "${FABLE_HOME}" "${FABLE_RECEIPT}/10-targeted-runtime-home.txt"
"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_TMP}" >"${FABLE_RECEIPT}/10b-runtime-tmp-gitleaks.txt" 2>&1 || fail "post-auth isolated TMP Gitleaks scan failed"
targeted_scan "${FABLE_TMP}" "${FABLE_RECEIPT}/10c-targeted-runtime-tmp.txt"
log "exact_flag_parse_exit=0"
log "authenticated_method=oauth_token provider=firstParty"
log "minimal_child_environment=CLAUDE_CODE_OAUTH_TOKEN,CLAUDE_CODE_OAUTH_REFRESH_TOKEN,HOME,TMPDIR,PATH,LANG,LC_ALL"
log "preflight_completed_utc=$(/bin/date -u '+%Y-%m-%dT%H:%M:%SZ')"
exec 3>&-
exec 3>&2

provider_start_utc="$(/bin/date -u '+%Y-%m-%dT%H:%M:%SZ')"
/bin/mkdir "${FABLE_STARTED_GUARD}" || fail "atomic one-use provider-start guard creation failed"
{
  print -- "provider_start_utc=${provider_start_utc}"
  print -- "guard=${FABLE_STARTED_GUARD}"
  print -- "model=claude-fable-5"
  print -- "effort=max"
  print -- "fallback=none"
  print -- "task_sha256=${EXPECTED_TASK_SHA256}"
  print -- "launcher_sha256=$(sha256_of "${FABLE_LAUNCHER}")"
  print -- "sandbox_sha256=${EXPECTED_SANDBOX_SHA256}"
  print -- "packet_sha256=${EXPECTED_PACKET_SHA256}"
} >"${FABLE_RECEIPT}/13-start.txt"
print -- "${provider_start_utc}" >"${FABLE_STARTED_GUARD}/started-utc.txt"
print -- "${FABLE_RECEIPT}" >"${FABLE_STARTED_GUARD}/receipt-path.txt"

exec 3>&-
set +e
run_fable_cli provider >"${FABLE_RECEIPT}/14-fable.raw.jsonl" 2>"${FABLE_RECEIPT}/15-fable.stderr.txt"
provider_exit=$?
set -e
provider_end_utc="$(/bin/date -u '+%Y-%m-%dT%H:%M:%SZ')"
exec 3>&2
{
  print -- "provider_exit=${provider_exit}"
  print -- "provider_start_utc=${provider_start_utc}"
  print -- "provider_end_utc=${provider_end_utc}"
  print -- "raw_stdout_bytes=$(/usr/bin/stat -f '%z' "${FABLE_RECEIPT}/14-fable.raw.jsonl")"
  print -- "raw_stdout_sha256=$(sha256_of "${FABLE_RECEIPT}/14-fable.raw.jsonl")"
  print -- "raw_stderr_bytes=$(/usr/bin/stat -f '%z' "${FABLE_RECEIPT}/15-fable.stderr.txt")"
  print -- "raw_stderr_sha256=$(sha256_of "${FABLE_RECEIPT}/15-fable.stderr.txt")"
} >"${FABLE_RECEIPT}/16-exit.txt"
exact_secret_scan "${FABLE_RECEIPT}/18-exact-post-provider-secrets.txt" "${FABLE_HOME}" "${FABLE_TMP}" "${FABLE_RECEIPT}"
unset fable_oauth_token fable_refresh_token

{
  print -- "postflight_utc=${provider_end_utc}"
  print -- "task_sha256=$(sha256_of "${FABLE_TASK}")"
  print -- "launcher_sha256=$(sha256_of "${FABLE_LAUNCHER}")"
  print -- "sandbox_sha256=$(sha256_of "${FABLE_SANDBOX}")"
  print -- "cli_sha256=$(sha256_of "${FABLE_CLI}")"
  print -- "packet_sha256=$(sha256_of "${FABLE_PACKET}")"
  print -- "manifest_sha256=$(sha256_of "${FABLE_EXTRACTION}/MANIFEST.sha256")"
  print -- "coordination_head=$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse HEAD)"
  print -- "coordination_tree=$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse HEAD^{tree})"
  print -- "implementation_head=$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse HEAD)"
  print -- "implementation_tree=$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse HEAD^{tree})"
  print -- "coordination_status_begin"
  /usr/bin/git -C "${FABLE_COORDINATION}" status --porcelain=v1
  print -- "coordination_status_end"
  print -- "implementation_status_begin"
  /usr/bin/git -C "${FABLE_IMPLEMENTATION}" status --porcelain=v1
  print -- "implementation_status_end"
} >"${FABLE_RECEIPT}/17-postflight.txt"
exec 3>>"${FABLE_RECEIPT}/17-postflight.txt"

typeset -a post_actual_members
post_actual_members=("${(@f)$(/usr/bin/unzip -Z1 "${FABLE_PACKET}")}")
[[ ${#post_actual_members[@]} -eq ${#expected_members[@]} ]] || fail "post-run packet member count mismatch"
for ((member_index = 1; member_index <= ${#expected_members[@]}; ++member_index)); do
  [[ "${post_actual_members[member_index]}" == "${expected_members[member_index]}" ]] ||
    fail "post-run packet member mismatch at ${member_index}: ${post_actual_members[member_index]}"
done
[[ "$(/usr/bin/unzip -Z1 "${FABLE_PACKET}" | /usr/bin/shasum -a 256 | /usr/bin/awk '{print $1}')" == "${EXPECTED_MEMBER_LIST_SHA256}" ]] ||
  fail "post-run ordered member-list SHA mismatch"
typeset -a post_member_modes
post_member_modes=("${(@f)$(/usr/bin/zipinfo -l "${FABLE_PACKET}" | /usr/bin/awk '$1 ~ /^-/ {print $1}')}")
[[ ${#post_member_modes[@]} -eq 28 ]] || fail "post-run packet regular-member mode count mismatch"
for member_mode in "${post_member_modes[@]}"; do
  [[ "${member_mode}" == "-rw-r--r--" ]] || fail "post-run unexpected packet member mode: ${member_mode}"
done
[[ "$(/usr/bin/zipinfo -v "${FABLE_PACKET}" | "${FABLE_RG}" -c 'file security status:.*not encrypted')" == "28" ]] ||
  fail "post-run packet encryption-status count mismatch"
[[ "$(sha256_of "${FABLE_EXTRACTION}/MANIFEST.sha256")" == "${EXPECTED_MANIFEST_SHA256}" ]] ||
  fail "post-run manifest SHA-256 mismatch"
(
  cd "${FABLE_EXTRACTION}"
  /usr/bin/shasum -a 256 --check MANIFEST.sha256
) >&3 2>&1 || fail "post-run extracted-member manifest check failed"
[[ "$(/usr/bin/find "${FABLE_STAGING}" -type f | /usr/bin/wc -l | /usr/bin/tr -d ' ')" == "28" ]] || fail "post-run staging regular-file count mismatch"
[[ "$(/usr/bin/find "${FABLE_EXTRACTION}" -type f | /usr/bin/wc -l | /usr/bin/tr -d ' ')" == "28" ]] || fail "post-run extraction regular-file count mismatch"
[[ -z "$(/usr/bin/find "${FABLE_STAGING}" -type l -print -quit)" ]] || fail "post-run staging contains a symlink"
[[ -z "$(/usr/bin/find "${FABLE_EXTRACTION}" -type l -print -quit)" ]] || fail "post-run extraction contains a symlink"
require_readonly_regular_tree "${FABLE_EXTRACTION}" "post_run_extraction"
/usr/bin/diff -qr "${FABLE_STAGING}" "${FABLE_EXTRACTION}" >&3 || fail "post-run staging/extraction byte mismatch"
log "post_packet_inventory_modes_encryption=passed post_extracted_manifest_check=passed"
log "post_staging_extraction_identical=true post_extraction_writable_entries=0 post_symlinks=0"

require_sha256 "${FABLE_TASK}" "${EXPECTED_TASK_SHA256}"
require_sha256 "${FABLE_SANDBOX}" "${EXPECTED_SANDBOX_SHA256}"
require_sha256 "${FABLE_CLI}" "${EXPECTED_CLI_SHA256}"
require_sha256 "${FABLE_GITLEAKS}" "${EXPECTED_GITLEAKS_SHA256}"
require_sha256 "${FABLE_RG}" "${EXPECTED_RG_SHA256}"
require_sha256 "${FABLE_PYTHON}" "${EXPECTED_PYTHON_SHA256}"
require_sha256 "${FABLE_PACKET}" "${EXPECTED_PACKET_SHA256}"
[[ "$(sha256_of "${FABLE_LAUNCHER}")" == "${preflight_launcher_sha}" ]] || fail "launcher changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_COORDINATION}" branch --show-current)" == "${coordination_branch}" ]] || fail "coordination branch changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse HEAD)" == "${coordination_head}" ]] || fail "coordination HEAD changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse HEAD^{tree})" == "${coordination_tree}" ]] || fail "coordination tree changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_COORDINATION}" rev-parse '@{u}')" == "${coordination_upstream}" ]] || fail "coordination upstream changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_COORDINATION}" ls-remote --exit-code origin "refs/heads/${coordination_branch}" | /usr/bin/awk '{print $1}')" == "${coordination_remote}" ]] || fail "coordination remote changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" branch --show-current)" == "${implementation_branch}" ]] || fail "implementation branch changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse HEAD)" == "${implementation_head}" ]] || fail "implementation HEAD changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse HEAD^{tree})" == "${implementation_tree}" ]] || fail "implementation tree changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" rev-parse '@{u}')" == "${implementation_upstream}" ]] || fail "implementation upstream changed during provider run"
[[ "$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" ls-remote --exit-code origin "refs/heads/${implementation_branch}" | /usr/bin/awk '{print $1}')" == "${implementation_remote}" ]] || fail "implementation remote changed during provider run"
[[ -z "$(/usr/bin/git -C "${FABLE_COORDINATION}" status --porcelain=v1)" ]] || fail "coordination repo changed during provider run"
[[ -z "$(/usr/bin/git -C "${FABLE_IMPLEMENTATION}" status --porcelain=v1)" ]] || fail "implementation repo changed during provider run"

"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_HOME}" >"${FABLE_RECEIPT}/20-post-runtime-home-gitleaks.txt" 2>&1 || fail "post-run isolated HOME Gitleaks scan failed"
targeted_scan "${FABLE_HOME}" "${FABLE_RECEIPT}/21-post-runtime-home-targeted.txt"
"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_TMP}" >"${FABLE_RECEIPT}/21b-post-runtime-tmp-gitleaks.txt" 2>&1 || fail "post-run isolated TMP Gitleaks scan failed"
targeted_scan "${FABLE_TMP}" "${FABLE_RECEIPT}/21c-post-runtime-tmp-targeted.txt"

/usr/bin/jq -s '.' "${FABLE_RECEIPT}/14-fable.raw.jsonl" >"${FABLE_RECEIPT}/22-events.json" || fail "raw stream-json contains a non-JSON event"
event_count="$(/usr/bin/jq -r 'length' "${FABLE_RECEIPT}/22-events.json")"
result_count="$(/usr/bin/jq -r '[.[] | select(.type == "result" and (.result | type == "string"))] | length' "${FABLE_RECEIPT}/22-events.json")"
if [[ ${provider_exit} -eq 0 ]]; then
  /usr/bin/jq -e --arg model "claude-fable-5" '
    def nonblank:
      type == "string" and (gsub("\\s"; "") | length > 0);
    ([.[] | select(.type == "system" and .subtype == "init")]) as $inits |
    ([.[] | select(.type == "result")]) as $results |
    ([.[] | select(.type == "assistant")]) as $assistants |
    ([.[] | select(.type == "assistant") | .message.model? // empty] | unique) as $assistant_models |
    ([.[] | .modelUsage? | objects | keys[]] | unique) as $usage_models |
    ($results[0].session_id) as $session |
    ($inits | length) == 1 and
    ($inits[0].model == $model) and
    ($results | length) == 1 and
    ($results[0].subtype == "success") and
    ($results[0].is_error == false) and
    ($results[0].result | nonblank) and
    (($session | type) == "string" and ($session | length) > 0) and
    ($results[0] | has("permission_denials")) and
    (($results[0].permission_denials | type) == "array" and ($results[0].permission_denials | length) == 0) and
    all(.[];
      if (.type == "system" or .type == "assistant" or .type == "user" or .type == "result")
      then ((.session_id | type) == "string" and (.session_id | length) > 0 and .session_id == $session)
      else true
      end) and
    all(.[];
      if has("session_id")
      then ((.session_id | type) == "string" and (.session_id | length) > 0 and .session_id == $session)
      else true
      end) and
    all(.[];
      if has("permission_denials")
      then ((.permission_denials | type) == "array" and (.permission_denials | length) == 0)
      else true
      end) and
    all(.[]; .subtype? != "model_refusal_fallback") and
    all(.[]; if has("model") then .model == $model else true end) and
    all(.[];
      if ((.message? | type) == "object" and (.message | has("model")))
      then .message.model == $model
      else true
      end) and
    ($assistants | length) > 0 and
    all($assistants[];
      (.message | type) == "object" and (.message | has("model")) and .message.model == $model) and
    ($assistant_models | length) > 0 and
    all($assistant_models[]; . == $model) and
    all($usage_models[]; . == $model)
  ' "${FABLE_RECEIPT}/22-events.json" >/dev/null ||
    fail "successful provider exit violated the exact success, session, permission, or Fable5 model contract"
  accepted_session_id="$(/usr/bin/jq -r '.[] | select(.type == "result") | .session_id' "${FABLE_RECEIPT}/22-events.json")"
else
  accepted_session_id=""
fi
/usr/bin/jq -j -r '[.[] | select(.type == "result" and (.result | type == "string")) | .result] | if length == 1 then .[0] else "" end' \
  "${FABLE_RECEIPT}/22-events.json" >"${FABLE_RECEIPT}/23-terminal-answer.md"
/usr/bin/jq '[.[] | select(.type == "system" or .type == "result" or has("session_id") or has("usage") or has("modelUsage")) |
  {type: (.type // null), subtype: (.subtype // null), session_id: (.session_id // null),
   is_error: (.is_error // null), duration_ms: (.duration_ms // null),
   duration_api_ms: (.duration_api_ms // null), num_turns: (.num_turns // null),
   total_cost_usd: (.total_cost_usd // null), usage: (.usage // null),
   modelUsage: (.modelUsage // null), permission_denials: (.permission_denials // null),
   model: (.model // .message.model // null)}]' \
  "${FABLE_RECEIPT}/22-events.json" >"${FABLE_RECEIPT}/24-session-usage.json"
/usr/bin/jq '[.[] as $event | $event.message.content[]? | select(.type == "tool_use") |
  {event_type: ($event.type // null), event_session_id: ($event.session_id // null),
   id: (.id // null), name: (.name // null), input: (.input // null)}]' \
  "${FABLE_RECEIPT}/22-events.json" >"${FABLE_RECEIPT}/25-tool-uses.json"
/usr/bin/jq '[.[] as $event | $event.message.content[]? | select(.type == "tool_result") |
  {event_type: ($event.type // null), event_session_id: ($event.session_id // null),
   tool_use_id: (.tool_use_id // null), has_is_error: has("is_error"),
   is_error: (if has("is_error") then .is_error else null end)}]' \
  "${FABLE_RECEIPT}/22-events.json" >"${FABLE_RECEIPT}/25b-tool-results.json"
if [[ ${provider_exit} -eq 0 ]]; then
  /usr/bin/jq -e 'length > 0 and all(.[]; .name == "Read" or .name == "Glob" or .name == "Grep")' \
    "${FABLE_RECEIPT}/25-tool-uses.json" >/dev/null || fail "successful provider result has no allowed review tool use"
  stream_contract_status=passed
else
  /usr/bin/jq -e 'all(.[]; .name == "Read" or .name == "Glob" or .name == "Grep")' \
    "${FABLE_RECEIPT}/25-tool-uses.json" >/dev/null || fail "provider emitted a tool outside Read/Glob/Grep"
  stream_contract_status=not_applicable_provider_exit_nonzero
fi
/usr/bin/jq -e --arg root "${FABLE_EXTRACTION}" '
  def nonnegative_integer:
    type == "number" and . >= 0 and floor == .;
  def positive_integer:
    type == "number" and . > 0 and floor == .;
  def optional_nonnegative_integer($object; $key):
    ((($object | has($key)) | not) or ($object[$key] | nonnegative_integer));
  def optional_positive_integer($object; $key):
    ((($object | has($key)) | not) or ($object[$key] | positive_integer));
  def optional_boolean($object; $key):
    ((($object | has($key)) | not) or (($object[$key] | type) == "boolean"));
  def optional_nonblank_string($object; $key):
    ((($object | has($key)) | not) or
     (($object[$key] | type) == "string" and ($object[$key] | length) > 0));
  def page_range_ok:
    type == "string" and
    test("^[1-9][0-9]*(-[1-9][0-9]*)?$") and
    (split("-") | if length == 2 then (.[1] | tonumber) >= (.[0] | tonumber) else true end);
  def lexical_path_ok($root):
    . as $value |
    ($value | type == "string") and
    ($value | length > 0) and
    (($value | startswith("~")) | not) and
    ($value | split("/") | all(.[]; . != "..")) and
    (if ($value | startswith("/"))
     then ($value == $root or ($value | startswith($root + "/")))
     else true
     end);
  def pattern_path_ok($root):
    . as $value |
    ($value | lexical_path_ok($root)) and
    (["..", "{", "}", "(", ")", "[", "]", "|", "!", "\\"] |
     all(.[]; . as $bad | (($value | contains($bad)) | not)));
  def object_keys_only($allowed):
    (.input | type == "object") and
    (((.input | keys) - $allowed) | length == 0);
  all(.[];
    if .name == "Read" then
      .input as $input |
      object_keys_only(["file_path", "limit", "offset", "pages"]) and
      ($input.file_path | lexical_path_ok($root)) and
      optional_nonnegative_integer($input; "offset") and
      optional_positive_integer($input; "limit") and
      ((($input | has("pages")) | not) or ($input.pages | page_range_ok))
    elif .name == "Glob" then
      .input as $input |
      object_keys_only(["path", "pattern"]) and
      ($input.pattern | pattern_path_ok($root)) and
      ((($input | has("path")) | not) or ($input.path | lexical_path_ok($root)))
    elif .name == "Grep" then
      .input as $input |
      object_keys_only(["-A", "-B", "-C", "-i", "-n", "-o", "glob", "head_limit", "multiline", "offset", "output_mode", "path", "pattern", "type"]) and
      (($input.pattern | type) == "string" and ($input.pattern | length) > 0) and
      ((($input | has("path")) | not) or ($input.path | lexical_path_ok($root))) and
      ((($input | has("glob")) | not) or ($input.glob | pattern_path_ok($root))) and
      optional_nonnegative_integer($input; "-A") and
      optional_nonnegative_integer($input; "-B") and
      optional_nonnegative_integer($input; "-C") and
      optional_nonnegative_integer($input; "head_limit") and
      optional_nonnegative_integer($input; "offset") and
      optional_boolean($input; "-i") and
      optional_boolean($input; "-n") and
      optional_boolean($input; "-o") and
      optional_boolean($input; "multiline") and
      optional_nonblank_string($input; "type") and
      ((($input | has("output_mode")) | not) or
       ($input.output_mode == "content" or $input.output_mode == "files_with_matches" or
        $input.output_mode == "count"))
    else false
    end)' \
  "${FABLE_RECEIPT}/25-tool-uses.json" >/dev/null || fail "provider emitted an explicit tool path outside the packet root"
if [[ ${provider_exit} -eq 0 ]]; then
  /usr/bin/jq -e --arg session "${accepted_session_id}" \
    --slurpfile outcomes "${FABLE_RECEIPT}/25b-tool-results.json" '
      . as $uses |
      $outcomes[0] as $results |
      (length > 0) and
      all(.[];
        .event_type == "assistant" and
        ((.event_session_id | type) == "string" and .event_session_id == $session) and
        ((.id | type) == "string" and (.id | length) > 0) and
        (.input | type) == "object") and
      (([.[].id] | unique | length) == length) and
      (($results | length) == length) and
      all($results[];
        .event_type == "user" and
        ((.event_session_id | type) == "string" and .event_session_id == $session) and
        ((.tool_use_id | type) == "string" and (.tool_use_id | length) > 0) and
        ((.has_is_error | type) == "boolean") and
        ((.has_is_error == false and .is_error == null) or
         (.has_is_error == true and (.is_error | type) == "boolean" and .is_error == false))) and
      all($uses[]; . as $use |
        ([$results[] | select(.tool_use_id == $use.id)] | length) == 1) and
      all($results[]; . as $result |
        ([$uses[] | select(.id == $result.tool_use_id)] | length) == 1)
    ' "${FABLE_RECEIPT}/25-tool-uses.json" >/dev/null ||
    fail "successful provider tool calls are not uniquely bound to successful same-session results"
  successful_tool_result_count="$(/usr/bin/jq -r 'length' "${FABLE_RECEIPT}/25b-tool-results.json")"
else
  successful_tool_result_count=not_applicable
fi
tool_use_count="$(/usr/bin/jq -r 'length' "${FABLE_RECEIPT}/25-tool-uses.json")"
session_ids="$(/usr/bin/jq -r '[.[] | .session_id? // empty] | unique | join(",")' "${FABLE_RECEIPT}/22-events.json")"
models="$(/usr/bin/jq -r '[.[] | (.model? // .message.model? // empty)] | unique | join(",")' "${FABLE_RECEIPT}/22-events.json")"
{
  print -- "provider_exit=${provider_exit}"
  print -- "event_count=${event_count}"
  print -- "string_result_count=${result_count}"
  print -- "tool_use_count=${tool_use_count}"
  print -- "successful_tool_result_count=${successful_tool_result_count}"
  print -- "session_ids=${session_ids}"
  print -- "models=${models}"
  print -- "stream_success_session_permission_model_contract=${stream_contract_status}"
  print -- "tool_schema_and_canonical_path_contract=passed"
  print -- "terminal_answer_bytes=$(/usr/bin/stat -f '%z' "${FABLE_RECEIPT}/23-terminal-answer.md")"
  print -- "terminal_answer_sha256=$(sha256_of "${FABLE_RECEIPT}/23-terminal-answer.md")"
} >"${FABLE_RECEIPT}/26-stream-summary.txt"

/bin/mkdir "${FABLE_SCAN_TMP}" || fail "final receipt-scan staging directory creation failed"
"${FABLE_GITLEAKS}" dir --no-banner --redact --exit-code 1 "${FABLE_RECEIPT}" >"${FABLE_SCAN_TMP}/gitleaks.txt" 2>&1 || fail "final receipt Gitleaks scan failed"
targeted_scan "${FABLE_RECEIPT}" "${FABLE_SCAN_TMP}/targeted.txt"
/bin/mv "${FABLE_SCAN_TMP}/gitleaks.txt" "${FABLE_RECEIPT}/27-final-receipt-gitleaks.txt"
/bin/mv "${FABLE_SCAN_TMP}/targeted.txt" "${FABLE_RECEIPT}/28-final-receipt-targeted.txt"

: >"${FABLE_RECEIPT}/29-receipt-manifest.sha256"
[[ -z "$(/usr/bin/find "${FABLE_RECEIPT}" -mindepth 1 ! -type f -print -quit)" ]] ||
  fail "final receipt contains a directory, symlink, or non-regular entry"
[[ "$(/usr/bin/find "${FABLE_RECEIPT}" -mindepth 1 -maxdepth 1 -type f | /usr/bin/wc -l | /usr/bin/tr -d ' ')" == "${#receipt_outputs[@]}" ]] ||
  fail "final receipt entry count does not match the frozen output inventory"
for output_name in "${receipt_outputs[@]}"; do
  [[ -f "${FABLE_RECEIPT}/${output_name}" && ! -L "${FABLE_RECEIPT}/${output_name}" ]] ||
    fail "missing or non-regular frozen receipt output: ${output_name}"
  [[ "${output_name}" == "29-receipt-manifest.sha256" ]] && continue
  (
    cd "${FABLE_RECEIPT}"
    /usr/bin/shasum -a 256 "${output_name}"
  ) >>"${FABLE_RECEIPT}/29-receipt-manifest.sha256"
done
exec 3>&-

exit "${provider_exit}"
