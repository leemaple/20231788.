# Fable5 fb862a3 invocation receipt

## Outcome

The single authorized provider-capable process started at
2026-09-01 22:46:10 Asia/Shanghai and terminated naturally one second later.
Claude Code exited 1 before emitting any `stream-json` event. Its only stderr
was:

```text
EPERM: operation not permitted, mkdir '/tmp/claude-501'
```

This is an observed local startup failure at the frozen persistent-write
sandbox boundary. Zero stream events means provider acceptance cannot be
established. Per the frozen task, the allowance state is therefore
`consumption-unknown (operationally exhausted)`: no retry, resume, follow-up,
second process, or new session is permitted. No Fable5 review or verdict is
claimed, and the failed invocation has no effect on source acceptance.
The raw `launch-post.txt` field `allowance_state=consumed` records the earlier
process-start accounting rule; this parsed receipt applies the task's more
specific no-provider-acceptance state without altering that raw file.

## Bound invocation

- Coordination launch commit:
  `6a31d94ba481127e8bd2e87f00759c7c553ab64e`.
- Reviewed implementation commit:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Reviewed implementation tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`.
- Task: 27,130 bytes, SHA-256
  `bd6a459b64fef9513bf672b4f2d742ef97d396e78580095000e3aa302a660f04`.
- Packet: 9,327,133 bytes, SHA-256
  `b14d33341edda9ae9e0a38a3ebe5879170b07dfa592fca2d7088099ff66b66a5`.
- Binary: Claude Code 2.1.239, 324,973,552 bytes, SHA-256
  `2b4f7aafdaa65bcc2335f56a4b276317837203f2c5587b1f2a17ca78ad14e36f`.
- Model/flags: `claude-fable-5`, effort `max`, permission mode `plan`,
  `--bare`, `--safe-mode`, no persistence/Chrome/slash commands, strict empty
  MCP, and only `Read,Glob,Grep` enabled.
- Production profile SHA-256:
  `c8d4b416d9d92f667a0ca7ac0494bf12158591e3ef5682555667e9d70642d5bb`.
- Probe profile SHA-256:
  `384f0f3f7a2dd0ff7c173ee4003c55d834b40b4883717ffe0a7cfbeefae27793`.
- macOS 26.3.1 build `25D2128`; imported `system.sb` SHA-256
  `8e6c396a0a4a6db758b49104e045d39a4af0ca28c61300a683f4de88c393e7f6`;
  imported `dyld-support.sb` SHA-256
  `06215a5d32689aefe395c29710e182eb54ba22162f50df8b4842290f8a19bf1c`.

Pre-launch, the packet's 2,094-file internal manifest passed, the extraction
had zero writable entries, exact local/upstream/remote coordination identity
matched, implementation commit/tree were clean, isolated auth status was
valid, and the full flag vector parsed with a non-provider `--help` command.
The deterministic positive/deny probe receipt remained bound by SHA-256
`cdbef34cef6c3b3f329601b11d0239c27235fe348dc585a493b5703880c16784`.

## Terminal evidence

| Field | Value |
|---|---|
| Start UTC | `2026-09-01T14:46:10Z` |
| End UTC | `2026-09-01T14:46:11Z` |
| CLI exit | `1` |
| Parent `tee` wait | `0` |
| Post-identity gate | `0` |
| Raw JSONL | 0 bytes; SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |
| stderr | 56 bytes; SHA-256 `95d184c527424ea77100afcd245bdce94b0118dc3a56abafc4dbc785a5c9da4b` |
| launch-pre | 932 bytes; SHA-256 `ceaffbe682e8c0940acbef6c179f73001458034cd03dfbc02f7fec3866c095ea` |
| launch-post | 140 bytes; SHA-256 `5b540c1b80395deb3d842696ab4e36a8ebb187b38861489fef560aaf9429a524` |
| Stream events | `0` |
| Session ID | absent |
| Turns | `0` |
| API duration/cost | absent |
| Tool calls | `0` |
| Read/Glob/Grep paths | `0` |
| Web/browser/subagent calls | `0` |
| Parsed terminal answer | 0 bytes; SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |
| Fable verdict | absent; not claimed |

The raw/parsed files contain no credential or base-URL value. The wrapper's
combined post-identity comparison exited 0 after checking the binary, both
project profiles, macOS build, and both imported system profiles against their
frozen values. It did not emit separate per-item post-run hashes, so no claim is
made that an independent per-item post snapshot was retained. A later read-only
recheck also matched, but is kept distinct from the immediate combined gate.
