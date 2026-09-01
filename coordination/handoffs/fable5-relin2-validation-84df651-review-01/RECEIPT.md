# Fable5 Relin2 validation attempt receipt

## Outcome

No usable Fable5 review was produced. All three observed failures were local
wrapper or CLI startup failures, and no attempt emitted a `stream-json` event,
session ID, model identity, tool call, usage record, cost, terminal answer, or
verdict. Nothing in these attempts is algorithm-review evidence.

The first wrapper attempt, `receipt-attempt-10742`, stopped before the provider
guard because an inherited zsh `TRAPEXIT` deleted credential-adjacent temporary
stderr files from a command-substitution subshell. Its exact-token scan had
already reported zero matches. The entry lock was renamed to
`launcher-entry.failed-10742`; the receipt remains unchanged.

The next provider-capable attempt, `receipt-attempt-11888`, used launcher commit
`932ab8ddd8e3705f1f3a1ded3adb9f7e5a0453ef`. It ended locally with:

```text
Error: Invalid MCP configuration:
mcpServers: Invalid input: expected record, received undefined
```

Its raw stdout was empty. The exact empty MCP configuration was corrected from
`{}` to `{"mcpServers":{}}` and committed as
`5c1a5f5f02075b508dc820b7faec62cff2210591` after local parse testing and three
read-only regression PASS verdicts. The failed guard, lock, HOME, and TMP were
renamed with suffix `.failed-11888`; none was deleted or reused.

The final attempt, `receipt-attempt-13408`, used the corrected launcher and
ended locally with:

```text
EPERM: operation not permitted, mkdir '/tmp/claude-501'
```

Its raw stdout was also empty. Per the nonblocking allocation, the current
provider guard remains bound to this final receipt and no further process is
started for this task. Fable5 is therefore unavailable for this exact boundary;
Codex, the existing independent reviewers, executable TDD, GitHub Actions, and
direct Windows work remain the critical path. ZCode remains outside that path
until a fresh service/quota check permits its return.

## Frozen identities

- Coordination commit at final attempt:
  `5c1a5f5f02075b508dc820b7faec62cff2210591`.
- Coordination tree:
  `314a4c19f31415e801978742acc6418ad06be57c`.
- Reviewed implementation commit:
  `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`.
- Reviewed implementation tree:
  `028033ad13b90710eaa98e6f1436bdb2d8f49b86`.
- Task: 18,110 bytes; SHA-256
  `2472f02f950d6450183381d5eb64fdcb4af9600db1aca9185f5080c2808e0454`.
- Final launcher: 62,739 bytes; SHA-256
  `abb47e044fc6164f1d38c85212f06b33c1e765cba7777e5bed90dc0fed9d8adc`.
- Sandbox: 2,071 bytes; SHA-256
  `680fec15a149b95801cbc8dccf377661f5c756f28336e8565ce268359b8640b8`.
- Packet: 845,608 bytes; SHA-256
  `98e51fa1a020bb49a97927282da4e767c6d85be165da79d9e5b794576f2eb19e`.
- Packet member count: 28; ordered-list SHA-256
  `1026987f2d5ad0c5cb8c66eee559bdf11ab4cf7469acf43161fbbc6656529303`.
- Packet manifest SHA-256:
  `a00551b1faf346c824181b507a6ff92d370c869ae3b80c63c4a0b07808a38608`.

## Complete final receipts

| Attempt | UTC interval | Provider exit | Raw stdout | Raw stderr | Events / usage / answer | Manifest |
|---|---|---:|---|---|---|---|
| `11888` | `20:04:34`–`20:04:51` | 1 | 0 bytes, SHA-256 `e3b0c442…b855` | 97 bytes, SHA-256 `527402a1…f59b` | 0 / none / 0 bytes | 65 checked entries; manifest SHA-256 `a95e07a3…7177` |
| `13408` | `20:09:03`–`20:09:20` | 1 | 0 bytes, SHA-256 `e3b0c442…b855` | 56 bytes, SHA-256 `95d184c5…da4b` | 0 / none / 0 bytes | 65 checked entries; manifest SHA-256 `8a254a14…2efd` |

Both complete receipts passed their post-provider exact OAuth-token scan,
Gitleaks scan, targeted scan, repository/packet identity checks, and independent
`shasum --check` replay. Their full unchanged runtime directories are retained
at:

- `/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/receipt-attempt-11888`;
- `/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/receipt-attempt-13408`.
