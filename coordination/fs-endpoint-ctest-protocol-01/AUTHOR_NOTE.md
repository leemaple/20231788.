# Endpoint CTest protocol RED author note

Date: 2026-09-06

Requested model/routing: Sol/high. Backend identity remains
requested-unverified; no ranking, quota, or model probe was performed.

## Source boundary

The isolated worktree was created at exact base
`18f4d32eaa70a0b182fc876e1589856fbbae8552` on branch
`codex/endpoint-ctest-protocol-test-20260906`. Before creation, that branch and
path were absent. Untracked CI receipts in the main worktree were observed and
left untouched.

The implementation was grounded in these exact base files:

- `tests/run_paper_endpoint_once.sh`:
  `148c8400a8e520b2161f1e837eb12484520d0d53daf58be75b2986e37b08d286`
- `tests/paper_endpoint_finalizer.py`:
  `b3061c1b0283598e53e10d60241de77d5e4765743195e80782e4f2a7da507705`
- `.github/workflows/dcp-rcb.yml`:
  `d120c6765bf3bd4f0b434658515e14972f0c3ae1511dc0846d6e52760971e165`
- `coordination/fs-endpoint-wrapper-01/INTEGRATION_CONTRACT.md`:
  `98b2ad68b789e88c8b0923177019b2004018f740c82c39cd52052a257816c6f8`
- adopted `ENDPOINT_SPEC.md`:
  `e98f8cda4a04d92be6b42077c6a4cf40083e0616b935954d44a4724e1fb5662c`

No old implementation, quarantine file, OpenFHE production change, or modified
external OpenFHE tree was read.

## Action pins

The major action refs were resolved read-only from their official GitHub
repositories on 2026-09-06 and then frozen to these commits:

- `actions/checkout@v4` -> `11d5960a326750d5838078e36cf38b85af677262`
- `actions/setup-python@v5` -> `a26af69be951a213d495a4c3e4e4022e16d87065`
- `actions/upload-artifact@v4` -> `ea165f8d65b6e75b540449e92b4886f43607fa02`
- `actions/download-artifact@v4` -> `d3f86a106a0bac45b974a628896c90dbdf5c8093`
- `msys2/setup-msys2@v2` -> `66cd2cce69caa17b53920067426061ca1de3a884`

These pins record the reviewed workflow dependency identities; they are not a
claim that the new workflow has executed.

The official pinned-action interface exposes setup-python's absolute
`python-path`. The official setup-msys2 documentation says its `path-type`
default is `minimal`, which does not inherit arbitrary Windows PATH additions.
The workflow therefore selects `path-type: inherit` explicitly and fails unless
MSYS `command -v python`, converted to native spelling, equals the setup-python
output. This is an explicit hosted premise, not an assumption about the default.

## Static evidence only

The following commands were run locally and succeeded:

```text
python3 ast.parse(tests/paper_endpoint_ctest_protocol_gate.py)
PYTHON_AST_OK_NO_ASSERT

ruby YAML.parse_file(.github/workflows/endpoint-ctest-protocol-gate.yml)
YAML_SYNTAX_OK

bounded workflow-token check
WORKFLOW_BOUNDARY_TEXT_OK

pure string MAX_PATH review (D:/a/_temp, 11-digit run, one-digit attempt)
original deepest staged candidate: 296 characters
short RUNNER_TEMP/fsew deepest staged candidate: 242 characters
```

The workflow-token check required the one exact trigger branch, both synthetic
artifact names, always-run steps, and the final TIMEOUT gate. It rejected
`workflow_dispatch`, `pull_request`, `continue-on-error`, and a live artifact
name.

The hosted prepare command repeats the same check using the actual Windows
runner temp, SHA, run ID, and attempt before allocating anything and requires
the deepest publication staging candidate to be at most 247 characters. It
does not use a `\\?\` prefix or depend on Windows long-path enablement.

No local CMake, CTest, wrapper, finalizer, fixture, timeout, replay, crypto, or
full test was run. No workflow was pushed or dispatched. Therefore no RED
outcome, CTest timeout grammar, Windows line ending, MSYS/native path behavior,
artifact upload, or GitHub-hosted result is claimed yet.

## Expected first hosted observation

Both jobs should reach a real nonzero wrapper exit, an escaped primary-log
receipt containing the actual `61: ` sentinel, successful status-only selection
and artifact round trip, and then fail the final assertion because the current
CLI path does not convey the observed CTest timeout and publishes
`CTEST_FATAL`, not `TIMEOUT`. Any earlier inspector/selector/upload/download
failure is a different setup or transport result and must not be reported as
the intended RED.

The output renders captured CTest bytes as bounded data. It is not trusted as
instructions and is never evaluated. The artifact is synthetic, status-only,
one-day retention, and contains no canonical TSV, gzip, full primary log,
selector manifest, secret, ciphertext, or polynomial.
