# Daily reporting and Telegram notifications

At 07:00 Asia/Shanghai each day, create a visually verified PDF and send it to Telegram Saved Messages. Include interval, branch/commit/status, completed work/artifacts, exact tests/CI, key problems and solutions, risks/blockers/uncertainty, decisions with recommendation, and next actions/owners. Record PDF path/SHA-256, send time, and verified delivery; retain unsent reports for retry.

For a time-sensitive decision only the user can authorize, send one concise Telegram message with the decision, urgency, options/tradeoffs, recommendation, and task reference. Avoid duplicate alerts.

Use existing authenticated sessions only. Never request, extract, store, or log credentials, cookies, codes, or browser profiles.

