# Accepted Fable 5.1 matrix invocation

Recorded: 2026-09-02 Asia/Shanghai

The request was assembled and scanned outside every implementation checkout,
then extracted to this fresh canonical working directory:

```text
/private/tmp/relin2-fable51-test-matrix.J2kDf1/fresh/stage
```

Standard input was the exact retained `TASK.md`, SHA-256
`92958ac83c9f9899b4c578c7218621192d9fbdc65bb75881e21cb1c9851bfd14`.
The exact command was:

```sh
/tmp/claude-code-darwin-arm64-2.1.258-extract/package/claude \
  --settings /tmp/relin2-fable51-test-matrix.J2kDf1/fable51-minimal-auth-settings.json \
  --safe-mode \
  --restricted \
  --strict-mcp-config \
  --model claude-fable-5-1 \
  --effort max \
  --print \
  --verbose \
  --output-format stream-json \
  --input-format text \
  --no-session-persistence \
  --permission-mode dontAsk \
  --tools Read \
  < MATRIX_DECISION_TASK.md \
  > ../../fable51-matrix-output.jsonl \
  2> ../../fable51-matrix-stderr.txt
```

No fallback model was supplied. The mode-0600 settings file contained only
`env.ANTHROPIC_AUTH_TOKEN` and `env.ANTHROPIC_BASE_URL`, copied without printing
their values from the user's existing CLI settings. It contained no model,
tool, hook, plugin, permission, MCP, agent, or project setting. After the run,
the exact token and high-specificity scans were recorded without retaining the
token, and the ephemeral file was deleted. The user's original settings were
not modified.

The process completed naturally with shell exit 0. It was not interrupted,
prodded, resumed, or sent a duplicate task.

The initialization event records exact model `claude-fable-5-1`, Claude Code
2.1.258, the fresh directory above, `dontAsk`, exactly the `Read` tool, and zero
MCP servers/plugins. The terminal result records `success`, `is_error=false`,
`end_turn`, `completed`, session
`29b94085-4612-4706-857b-c557496b82da`, 53 turns, 831,434 ms, and model usage
only for canonical first-party `claude-fable-5-1`. Because
`--no-session-persistence` was used, the session is a run-correlation ID, not a
resumable conversation.
