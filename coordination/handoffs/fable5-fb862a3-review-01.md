# Fable5 fb862a3 independent-review handoff

Prepared: 2026-09-01 21:36 Asia/Shanghai
Updated: 2026-09-01 22:48 Asia/Shanghai

Status: **the single provider-capable process terminated naturally with exit 1
before any stream event; no Fable5 verdict exists; the one-use allowance is
`consumption-unknown (operationally exhausted)` and may not be retried**. The
complete raw/parsed receipt is retained under
`coordination/handoffs/fable5-fb862a3-review-01-receipt/`.

## Exact review target

- Candidate commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Candidate tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`.
- Pristine OpenFHE commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Final same-candidate hosted run: `33436252725`, Linux 6/6 and Windows 6/6.
- Historical diagnostic-red run: `33436068864`, complete Linux failure log and
  cancelled-Windows record retained.
- Registration-evidence run: `33513310345`, success on Linux and Windows after
  each job explicitly checked out and verified the candidate; both complete
  CTest JSON artifacts and unfiltered 6/6 logs retained.

## Frozen task and packet

- Detached task:
  `coordination/tasks/fable5-fb862a3-review-01.md`.
- Task size: 27,130 bytes.
- Task SHA-256:
  `bd6a459b64fef9513bf672b4f2d742ef97d396e78580095000e3aa302a660f04`.
- ZIP path:
  `/var/tmp/fable5-fb862a3.gBL1Wt/fable5-fb862a3-review-packet.zip`.
- ZIP size: 9,327,133 bytes.
- ZIP SHA-256:
  `b14d33341edda9ae9e0a38a3ebe5879170b07dfa592fca2d7088099ff66b66a5`.
- ZIP members: 2,360 total entries, 2,095 regular files.
- Internal manifest: 2,094 lines, covering every other regular file.
- Fresh extraction:
  `/var/tmp/fable5-fb862a3.gBL1Wt/fresh-extraction`.

The archive had zero duplicate, unsafe, encrypted, symlink, or bad-mode
members. `unzip -t`, the internal manifest, and byte-for-byte stage/fresh
comparison passed. Gitleaks 8.30.1 reported zero findings independently on
staging and the fresh extraction, scanning approximately 27.63 MB each.
Targeted sensitive-filename, forbidden-directory, symlink, and credential-path
counts were zero. The fresh extraction has zero writable filesystem entries.

The packet now contains a connected 70-row parent/commit/tree graph, 69 ordered
full-index patches from the first DCP test through the candidate, 35 complete
anchor tree/blob listings, the cumulative diff, all named raw TDD artifacts,
the complete historical red run, and full Linux/Windows registration evidence.

## Fixed CLI and sandbox

- Resolved non-symlink binary:
  `/Users/lifeng/.local/share/claude/versions/2.1.239`.
- Version: `2.1.239 (Claude Code)`.
- Binary size: 324,973,552 bytes.
- Binary SHA-256:
  `2b4f7aafdaa65bcc2335f56a4b276317837203f2c5587b1f2a17ca78ad14e36f`.
- Production sandbox:
  `coordination/handoffs/fable5-fb862a3-review-01.sb`, 2,095 bytes,
  SHA-256
  `c8d4b416d9d92f667a0ca7ac0494bf12158591e3ef5682555667e9d70642d5bb`.
- Probe sandbox:
  `coordination/handoffs/fable5-fb862a3-review-01-probe.sb`, 2,153 bytes,
  SHA-256
  `384f0f3f7a2dd0ff7c173ee4003c55d834b40b4883717ffe0a7cfbeefae27793`.
- Imported Apple sandbox baseline: macOS 26.3.1, build `25D2128`.
- Canonical `/System/Library/Sandbox/Profiles/system.sb`: 11,847 bytes,
  SHA-256
  `8e6c396a0a4a6db758b49104e045d39a4af0ca28c61300a683f4de88c393e7f6`.
- Canonical `/System/Library/Sandbox/Profiles/dyld-support.sb`: 2,655 bytes,
  SHA-256
  `06215a5d32689aefe395c29710e182eb54ba22162f50df8b4842290f8a19bf1c`.
- Non-sensitive `/Users` read canary:
  `coordination/handoffs/fable5-sandbox-user-read-canary.txt`, SHA-256
  `a86d8f857976b9ff429e7a367fb20a11c0b45250450dad3ce7f547f13ab245c5`.

Literal no-provider preflight results against the final fresh extraction:

| Probe | Expected | Actual |
|---|---:|---:|
| sandboxed fixed-binary `--version` | 0 | 0 |
| exact full launch-flag parse with `--help` | 0 | 0 |
| read `PACKET_SCOPE.md` | 0 | 0 |
| ordinary read of non-sensitive `/Users` canary | 0 | 0 |
| ordinary read of empty `/var/folders` canary | 0 | 0 |
| ordinary read of one System.keychain byte to `/dev/null` | 0 | 0 |
| actual create/check/remove in disposable write directory | 0 | 0; control target absent afterward |
| read non-sensitive regular `/Users` canary | 1 | 1, empty stdout |
| read empty regular `/var/folders` canary | 1 | 1, empty stdout |
| read one ordinarily readable System.keychain byte to `/dev/null` | 1 | 1, empty stdout |
| write in pre-proven POSIX-writable disposable directory | 1 | 1, empty stdout; target absent |

The final packet-external probe receipt is
`/var/tmp/fable5-fb862a3.gBL1Wt/sandbox-probe-receipt-final`. Its
`preflight-summary.txt` is 2,537 bytes with SHA-256
`3f243e2464ab414f2a0b36699b1467ff0faf36cbce395281c5ef4f0524c8f191`;
`preflight-summary.identity.txt` is 119 bytes with SHA-256
`7f3291cd2ec9159b8372d2fdf78e5beb0a618a3de2d597dc5f7c53fce171300e`.
The 20-row manifest binds every retained positive/deny stdout/stderr plus both
summary files; it is 1,789 bytes with SHA-256
`cdbef34cef6c3b3f329601b11d0239c27235fe348dc585a493b5703880c16784`.
It records all ten exit values, bytes/SHA-256 for every raw capture, both
system-profile identities, and both post-probe target-existence values. The
first enhanced control attempt, which correctly failed when sandboxed `cat`
was pointed directly at a temporary-path regular file, remains separately
retained; the frozen task now requires parent-owned pipes for positive and
negative sandbox captures. The ordinary keychain-read control uses
`status=none`, eliminating `dd`'s nondeterministic transfer-rate stderr; two
complete consecutive executions reproduced the three bound receipt hashes
exactly.

The production profile imports Apple's positive system-runtime whitelist, then
adds only the exact CLI binary, packet extraction, and minimal TLS/DNS reads. It
explicitly denies all `/Users`, mounted volumes, keychains, ordinary temporary
paths, macOS user temporary/browser state, staging/archive paths, Darwin
directory data, and `master.passwd`. It grants no persistent filesystem write,
permits provider egress, and restricts execution to the fixed binary.
The macOS build and both imported policy files are checked by path, non-symlink
status, byte size, and SHA-256 before the probes, immediately before the only
provider-capable launch, and after termination. Any pre-launch mismatch fails
closed; any post-run mismatch invalidates the receipt without permitting a
retry.

## Authentication and launch contract

The parent process reads the existing settings values without printing them.
With `--bare`, inherited `ANTHROPIC_AUTH_TOKEN` alone correctly produced no
local authenticated state; therefore the final isolated environment supplies
the same unlogged secret under both `ANTHROPIC_API_KEY` and
`ANTHROPIC_AUTH_TOKEN`, plus the unlogged `ANTHROPIC_BASE_URL`. A sandboxed,
no-provider `--bare auth status --json` then returned only the filtered facts
`loggedIn=true`, `authMethod=api_key`, and `apiProvider=firstParty`. No token,
base URL, settings contents, or other environment value was logged.

The final process must use exactly one `claude -p` launch from the read-only
fresh extraction with model `claude-fable-5`, effort `max`, permission mode
`plan`, `--bare`, `--safe-mode`, `--no-session-persistence`, `--no-chrome`,
disabled slash commands and prompt suggestions, strict empty MCP config,
`Read,Glob,Grep` only, explicit write/shell/web/delegation denials, verbose
`stream-json`, task stdin opened by the parent, and raw output/stderr connected
to parent-owned pipes whose unsandboxed `tee` processes write the separate
packet-external receipt. There is no fallback model.

Immediately before that one process, the parent must assert both authentication
variables are nonempty without printing them, prove the task readable and fresh
extraction present, create and actually write/remove a receipt control file,
assert both final output paths absent, `cd` to the exact fresh extraction and
assert `$PWD`, and reverify the imported sandbox policy identities. Only after
all assertions pass may the provider-capable command start.

The allowance is consumed only when the provider-capable production process
starts without `--help`. Local `--version`, local `auth status`, and `--help`
short-circuit probes do not contact the provider and do not consume it. Every
production terminal outcome forbids a retry, resume, follow-up, second process,
or new session. Transport/output uncertainty is recorded as
`consumption-unknown (operationally exhausted)`.

## Terminal result

The exact process started at `2026-09-01T14:46:10Z` and ended naturally at
`2026-09-01T14:46:11Z`. Claude Code exited 1, the parent `tee` wait exited 0,
and the post-identity gate exited 0. Raw JSONL is empty: 0 bytes, SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
The 56-byte stderr has SHA-256
`95d184c527424ea77100afcd245bdce94b0118dc3a56abafc4dbc785a5c9da4b`
and contains only the observed local startup error:

```text
EPERM: operation not permitted, mkdir '/tmp/claude-501'
```

There were zero stream events, tool calls, read paths, web/browser calls,
subagents, turns, or terminal-answer bytes. No session ID, API duration,
provider cost, or Fable verdict was emitted. Provider acceptance therefore
cannot be established. Per the frozen contract, do not retry, resume,
follow-up, or launch a second process. The failed invocation does not replace
source/test evidence and contributes no review finding. The retained
`post_identity_exit=0` is a combined comparison result; the wrapper did not
emit separate per-item post-run hashes, so no independent per-item post
snapshot is claimed.

The parsed terminal-answer artifact is explicitly retained as 0 bytes with
SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
The six-file receipt manifest is 536 bytes with SHA-256
`81cf9d05b5505791c8dc72421497696dbfa2ce5f6075c297e6ab29ce87e5ebf5`.
