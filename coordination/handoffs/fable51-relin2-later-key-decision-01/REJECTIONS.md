# Rejected attempts

Only `fable51-cleanroom-output.jsonl` and `ANSWER.md` are accepted decision
evidence. The files under `rejected/` are retained solely to prevent later
confusion.

## 01 — old client and old model

The installed Claude Code `2.1.239` was invoked with `--model fable`. Machine
output named `claude-fable-5`, not `claude-fable-5-1`. Anthropic documents
Claude Code `2.1.250` as the minimum Fable 5.1 client. This answer was rejected
before its prose was used.

## 02 — correct model, unconfined working directory

The verified `2.1.258` binary emitted canonical model
`claude-fable-5-1`, but its working directory was the project workspace.
`--safe-mode` disables customizations; it does not confine file tools. That
workspace contains a former implementation, and the single-result JSON did
not retain individual Read paths. The answer was therefore rejected regardless
of its content.

## 03 — restricted but unauthenticated

The next launch used the fresh directory and `--restricted`, but restricted
mode ignored the user-level gateway/auth settings. Its initialization event
showed `apiKeySource=none`; it returned HTTP 403 before model work, with zero
tokens and zero cost. This was a launch failure, not a review.

## Accepted retry

The accepted retry kept the strict fresh-directory boundary and supplied an
ephemeral minimal auth-only settings file. Its complete stream proves every
tool call was `Read` under the fresh directory. See `INVOCATION.md` and
`VERIFICATION.md`.
