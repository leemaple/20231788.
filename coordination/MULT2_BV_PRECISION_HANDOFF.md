# Separate BV diagnosis and precision-design handoffs

Observed2026-09-04, Asia/Shanghai. Current source for both packets:
bda879104c8a8b1ba6ac9301385b5b1919bef440, codex/mult2-01.
Current working-tree changes at packet creation were only subsequent evidence
and dispatch documentation; project/ selections came from the exact Git commit.
No old implementation, modified local OpenFHE or browser/runtime/credential data.

## Independently verified source packages

Both packets have51 payload files plus SOURCE-MANIFEST.json,66 ZIP entries
including directories. All30 selected project files were byte-compared against
git show atbda8791; all CMake-referenced source inputs are present.16 official
OpenFHE references are pinned todf495ba2e91739a6dc8f1de254fc5a41155ce504:
the existing8 verified references, the separately verified dcrtpoly-impl.h,
and7 fresh GitHub blob-verified plaintext/DFT/parameter/BV/HYBRID sources.
The user-supplied paper PDF/text hashes match their prior verified manifests.
Supplemental logs are explicitly later results, not silently substituted source.

- BV: /private/tmp/mult2-precision-handoff.oAeEtc/mult2-bv-diagnosis-bda8791.zip,
  1024988bytes; SHA-256cfd92c7181253a92dc0e8fac2a7b18bafba19eadb2fbe86416d638963eb0009d.
- Precision: /private/tmp/mult2-precision-handoff.oAeEtc/mult2-precision-design-bda8791.zip,
  1025333bytes; SHA-25610fdc5a5d7327eca2ab03c8b9b5cb3778ca69415179277b8dd3c73495ce6e251.

Gitleaks8.30.1 scans of both full staged selections and both archive-depth2
contents:zero findings. Excluded-path, duplicate-path and symlink checks:zero.
Fresh extraction independently matched all51 lengths/hashes and exact52-file
closure for each archive. No supplied verification script was executed.
Full manifests are retained in coordination/handoffs/mult2-{bv,precision}-bda8791-manifest.json.
Exact full dispatch text is in coordination/tasks/chatgpt-pro-mult2-{bv,precision}-bda8791.md.

## ChatGPT Pro: independent conversations, submitted once each

Ego task space122, verified form selector Pro, ZIP card ready and normalized
composer text matched the saved full prompt before each single Send action.
After each send, composer cleared, Stop answering appeared and the real
conversation URL was verified. Neither existing nor new reasoning was stopped,
refreshed, reminded or submitted twice.

- BV task: title **Diagnose BV Relin2 Failure**,
  https://chatgpt.com/c/6a9a5824-3e5c-83ec-83ed-a73acf3dc062,
  tab8D68362B6373C1779E061DE35CD68AEE, submitted about13:33CST,
  full normalized prompt8773characters. Live: inspecting ZIP and manifest.
  The prompt explicitly adds the newly completed Windows42/44 original-matrix
  result to the archived brief's earlier in-progress observation.
- Precision task: title **Double CKKS precision design**,
  https://chatgpt.com/c/6a9a5891-6210-83ec-a24c-d6a5e0889fbd,
  tab6F1085C9C7D46593EFA146D644D7004B, submitted about13:35CST,
  full normalized prompt9425characters. Live: archive/manifest verification.

BV must adjudicate an empirical-bound defect versus production/CRT defect,
preserving all BV coverage and genuine red evidence. Precision must diagnose
encoding-rounding/REAL decoding-noise/host-type limits and propose the minimal
test-first route toward the paper's real precision, not declare the existing
degree2 or100-bit metadata sufficient. New public APIs remain proposals until
the seam is agreed. Independent current tasks must not overwrite each other.

## Fresh terminal Fable5.1 escalation: unavailable, not a completed review

After the new concrete BV failure and diagnostic values, one bounded terminal
invocation used the full verified BV stage, CLI claude2.1.258, exact requested
claude-fable-5-1, high effort, max-budget5USD, safe/restricted mode, Read-only
tools, strict empty MCP, no session persistence or fallback. Session
00294377-82dd-41ac-b2df-867a8efcb169, execution handle92801.
Init selected claude-fable-5-1; then synthetic error response and result reported
Failed to authenticate. API Error:403 Request not allowed, api_error,
zero input/output tokens,0USD, empty modelUsage, exit1. No usable5.1 inference
or mathematical verdict exists. Filtered visible/error fields are retained in
coordination/returns/fable51-mult2-bv-terminal.jsonl; thinking/tool chatter omitted.
No retry loop, auth change, fallback model or successful-review claim.

## Shared ZCode quota read-only refresh

Official logged-in page https://bigmodel.cn/coding-plan/personal/usage,
page refresh2026.09.04 13:35: five-hour12% used/88% remaining, reset15:29;
weekly64% used/36% remaining, reset2026-09-09 10:00;
MCP monthly16% used/84% remaining, reset2026-09-25 10:00.
The1 unused quota reset was NOT redeemed. These limits are shared by all ZCode
sessions. No new ZCode task was dispatched at this checkpoint; it is available
for a bounded independent review at the next reachable boundary. Previous
Windows task-text input failed; do not fabricate a Windows-agent review.

Current owner Codex: keep both Pro handles live, continue Sub red-green and
retain all CI outcomes. The full goal is unfinished; no conservative theorem,
full matrix green, repeated-multiplication or high-precision acceptance yet.
