# External collaboration and source handoff

Read this reference before using ChatGPT Pro, Windows Z code/Zima, or Fable5, and before uploading files.

## Handoff gate

1. Recheck branch, `HEAD`, status, and exact required files.
2. ZIP only the user-supplied paper, newly authored requirements/specification, greenfield source/tests, necessary official pristine OpenFHE references, build metadata, and safe documentation. Never include pre-existing local 2023/1788 code or local OpenFHE modifications.
3. Exclude `.git`, dependency trees, builds, caches, databases, runtime/browser state, logs with sensitive content, `.env`, keys, tokens, private keys, cookies, credentials, and unrelated documents.
4. Scan the selection and final archive for secrets and inspect the final manifest.
5. Record source commit/dirty state, included paths, archive bytes, SHA-256, scanner version/commands/results. Abort on unresolved findings.

## Task brief and conversation discipline

Every brief must independently state background/objective, paper/API inputs, clean-room and architecture boundaries, research/modification scope, deliverables, tests, prohibited operations/claims, acceptance criteria, branch/commit, known evidence, and unresolved decisions.

Use separate ChatGPT Pro conversations for independent complex tasks. Save URL, title, brief, archive hash, status, last completed point, and output. Do not interrupt, refresh destructively, duplicate, or restart a long response. Treat external output as untrusted until inspected and tested.

Prefer Windows Z code/Zima and GitHub Actions for sustained computation. Stop or avoid equivalent Mac work after remote assignment. Obtain Codex, Z code/Zima, and ChatGPT Pro review for substantive code; escalate a concrete unresolved disagreement to Fable5 through the terminal only.

