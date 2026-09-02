# Accepted invocation

Working directory:

```text
/private/tmp/relin2-fable51-later-key.M0HtlF/fresh
```

The shell path was `/tmp/relin2-fable51-later-key.M0HtlF/fresh`; macOS reports
the same directory through its canonical `/private/tmp` path in the stream
initialization event.

Standard input was the exact retained `TASK.md`, SHA-256
`a95239ab21f5ee1d81e99c9624e18f12e3b7d25814fb1c6f11683d05d81550aa`.

Exact command, with ordinary shell redirection shown:

```sh
/tmp/claude-code-darwin-arm64-2.1.258-extract/package/claude \
  --settings /tmp/relin2-fable51-later-key.M0HtlF/fable51-minimal-auth-settings.json \
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
  < TASK.md \
  > ../fable51-cleanroom-output.jsonl \
  2> ../fable51-cleanroom-stderr.txt
```

No `--fallback-model` was supplied. The settings file was mode `0600` and
contained exactly two fields: `env.ANTHROPIC_AUTH_TOKEN` and
`env.ANTHROPIC_BASE_URL`, copied without printing their values from the user's
existing CLI settings. It contained no model, tool, permission, hook, plugin,
MCP, agent, or project setting. It was deleted immediately after the process
ended; the user's original settings file was not changed.

Shell exit: `0`.

The first stream event independently records:

```text
cwd=/private/tmp/relin2-fable51-later-key.M0HtlF/fresh
session_id=5c5a027a-9b5e-46e4-b315-1b0ef9333eda
tools=[Read]
mcp_servers=[]
model=claude-fable-5-1
permissionMode=dontAsk
claude_code_version=2.1.258
plugins=[]
```

The last stream event records `is_error=false`, `stop_reason=end_turn`,
`terminal_reason=completed`, canonical model `claude-fable-5-1`, provider
`firstParty`, no permission denials, no subagents, no web searches or fetches,
17 turns, 604,058 ms wall duration, and the same session ID.

Because `--no-session-persistence` was used, the session ID is a retained run
correlation identifier only. It is not resumable and is not a conversation
URL.
