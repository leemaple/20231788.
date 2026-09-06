# Daily reporting and Telegram notifications

At 07:30 Asia/Shanghai each day, create a visually verified PDF and send it to Telegram Saved Messages. Include interval, branch/commit/status, completed work/artifacts, exact tests/CI, key problems and solutions, risks/blockers/uncertainty, decisions with recommendation, and next actions/owners. Record PDF path/SHA-256, send time, and verified delivery; retain unsent reports for retry.

For a time-sensitive decision only the user can authorize, send one concise Telegram message with the decision, urgency, options/tradeoffs, recommendation, and task reference. Avoid duplicate alerts.

Use existing authenticated sessions only. Never request, extract, store, or log credentials, cookies, codes, or browser profiles.

## Plain-language report contract (user update, 2026-09-06)

Write for a reader who does not know the paper or cryptography. Lead with what works, what does not, and the practical meaning; never lead with commit lists, model coordination, or unexplained metrics.

The main body must answer, in order:
1. What problem the paper solves, what it actually builds, and why that matters. Explain each essential term on first use; label analogies as analogies. Keep authors' results separate from our results.
2. How this clean-room reproduction maps to the paper, what OpenFHE supplies, and which parts we implemented. Explain the end-to-end calculation and non-negotiable boundaries.
3. What changed since the previous report, what did not change, and the actual effect. Separate authored drafts, compiled/running code, passing tests, and final acceptance. Compare error with its threshold in understandable terms.
4. Priorities, hard problems, and critical judgments: observed evidence, likely cause versus unresolved alternatives, fixes made, outstanding difficulties, and why the next experiment can distinguish them.
5. Next concrete deliverables, owners, completion checks, and conditional remaining-time ranges. State assumptions and when to re-estimate; never imply that writing drafts removes verification time.

Put exact branch/commit/Git status, CI outcomes, source links, quota observations, and supporting measurements in a compact appendix or clearly separated resource section. No 1000-run completion gate; report only tests actually needed and executed. Ordinary technical decisions stay with the team; ask the user only when new authority is genuinely required.

## Delivery and revised editions

Use the ego-browser skill and the existing logged-in Telegram Web session in Ego Lite as the preferred delivery route. Confirm the Saved Messages destination, upload the PDF once, and visually confirm its file message. Do not read unrelated chats or expose login data. If Web delivery is unavailable, the previously authorized /Applications/Telegram.app is a fallback; never select an updater's temporary copy.

For scheduled reports, check the delivery ledger first: a Confirmed record for today's Asia/Shanghai report date means no rebuilding, rerendering, resending, or duplicate ledger entry. An explicit user request to rewrite and send today authorizes a separate versioned manual edition, not overwriting the original or bypassing future scheduled idempotency. Record its edition, path, SHA-256, size, actual send time, destination, and verified or failed status separately.
