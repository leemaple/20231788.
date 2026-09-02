# Fable 5.1 matrix evidence verification

## Bound inputs

- Implementation commit/tree:
  `1e59e8b36d5119ceb2b463922f1053e03a029bd4` /
  `4e3a8b4857aeb8f5f7ef07dd2f01b5f74079ba77`.
- Pristine OpenFHE 1.5.0:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Request ZIP: 113,283 bytes, SHA-256
  `42ddcbe09df14c52fd7e38522da5e2dd259a2989c23b1b7a75ebac963da3808d`.
- Task SHA-256:
  `92958ac83c9f9899b4c578c7218621192d9fbdc65bb75881e21cb1c9851bfd14`.
- Packet manifest SHA-256:
  `2774896dc34a1555673cef9310f99a7efaa6f081eb6b9653a6db59b938ce0770`.

The ZIP contains 16 file members, no duplicate or unsafe path, and no encrypted
member. `unzip -t` exited 0. A fresh extraction verified the 14-row matrix
manifest with raw exit 0; the two manifest files are intentionally outside that
non-self-referential payload list. Gitleaks 8.30.1 scanned both the staged
payload and archive before submission with observed exit 0. After the run, the
same byte-exact archive was scanned again; that replay scan exited 0 and its
`[]` report plus stdout/stderr are retained under `scans/`.

## Bound output

- Raw JSONL: 1,150,987 bytes, SHA-256
  `cecf245ba36ad685b58d0f8d25eaffadc27bd73b3807b8bbd4d12aa7b7abd166`.
- Extracted answer: 11,102 bytes, SHA-256
  `dc3e173bd82cd27fb9fdeda41fd9af4693111c294f0327b5b6b3cd1e79aedc19`.
- Stderr: 0 bytes, SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Tool calls: 52 rows, SHA-256
  `2e0fcdc173e0407fa6f772b0af0fff6139622fa5a180e88caf3b2eb034edd524`.

Every tool-call row is `Read` and every requested path is the fresh directory
or a descendant. Calls to absent filenames/directories remained inside that
boundary. The initialization event reports no MCP server or plugin; the final
usage reports zero web search/fetch requests, no permission denial, and no
subagent.

The raw output Gitleaks scan exited 0 and retained `[]`. The targeted receipt in
`scans/output-targeted-scan.txt` binds the raw output/stderr hashes, the
auth-token SHA-256 without the token, the predicate classes, process exit 0,
and zero observed matches. Because the ephemeral target was deliberately
deleted, that fixed-string check is a contemporaneous non-replayable
supplementary observation, not acceptance evidence. The retained Gitleaks scan
is the mechanically replayable output credential gate.

## Machine result and accepted scope

The terminal event binds exact canonical model `claude-fable-5-1`, provider
`firstParty`, success, and normal completion. The accepted answer chooses
matrix **B**: 26→36 core red/green, then 36→37 lifecycle red/green. It does not
claim a build, runtime test, Windows result, or implementation change. All such
evidence remains downstream.
