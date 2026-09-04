# Final Pair audit of the combined 53-test boundary

Observed 2026-09-04 Asia/Shanghai. Source selection was clean at
f550eac1251f2005222e60aa4f07cc2e57380c46, codex/integration-01. Production,
tests, CMake and workflow were byte-checked unchanged from tested merge
d73824c2d382013c3aadbd7cb29c57008e839714. Exact run 33854419062 passed
Linux10096429980253/53,0.68s and Windows10096429959353/53,2.27s, all warning
and Relin2/RS2/Mult2/Add/Sub API builds green. These are scoped functional
results; precision, repeated multiplication and general error bounds remain open.

## Complete safe packet

Archive /private/tmp/pair-combined-review.5lwGtO/pair-combined-review-d73824c.zip
is 1305833 bytes, SHA256
e3dd499889e66a3406fa8ca755b559505db802c2d4cd7c8e1615d74900225fce.
Exactly 91 entries: 71 regular files and 20 directories. The 70-payload manifest
plus itself gives exact closure. Every payload was read from the final ZIP and
checked for size, SHA256 and equality to the selected local bytes. No duplicates,
unsafe/excluded paths, symlinks or CRC errors were found.

54 selected project files independently match exact Git f550eac. All 15
CMake-referenced test sources, current header/source, build workflow, relevant
ledgers, original Pair red/green/coverage and both combined CI log sets are
included. Eight paper/pristine-reference files match the earlier verified
precision-stage manifest. Four further official files (ciphertext.h, metadata.h,
cryptoobject.h, evalkeyrelin.h) were obtained from the official repository at
exact df495ba2e91739a6dc8f1de254fc5a41155ce504; GitHub blob SHA1, byte size and
SHA256 were independently recomputed. Their upstream paths/URLs/blob identities
are retained in the supplemental provenance JSON.

The full original Pair Pro ZIP is included, not just selected return documents:
212032 bytes, SHA256 735dea4e6c164ced95c2829ea8eb5316201eb900fd5d77b1aad171e94e2676c4,
88 entries / 66 regular files, safe listing and CRC checked. Its historical
clean-room candidate is not the current tested source. The narrow production
delta also byte-matches git diff from clean-room base7041a48 to Pair4b17018.
No old user implementation or modified local OpenFHE was used.

Gitleaks8.30.1 selected-input and final-archive scans, max-archive-depth3:
zero findings, about2.83MB and5.68MB inspected. No credentials, browser/runtime
state, cache, dependency tree or build output included. Clean scans are evidence,
not absolute guarantees. Full brief and manifest:
coordination/tasks/pair-combined-review-d73824c.md;
coordination/handoffs/pair-combined-review-d73824c-manifest.json.

## Independent ZCode seat dispatched once

Dedicated NEW local folder
/Users/lifeng/Documents/20231788-openfhe-zcode-pair-review-20260904.
The archive copy's bytes/hash were verified. LOCAL-REVIEW-TASK.md supplies the
full in-ZIP task and restricts work to static inspection, safe extraction there,
output/REVIEW.md and output/MANIFEST.sha256. No source/build/crypto/Git/CI or
other-agent operation is permitted. This uses the authorized local static
fallback because the prior Windows task-input path remains unverified; it is
NOT a Windows review and introduces no Mac compilation load.

The exact /Applications/ZCode.app Add project/Open folder flow selected this
new directory. Choose workspace visibly confirmed it with Value1. GLM-5.3
and Max were visibly selected. Two lowercase Return-key spellings failed at
the folder dialog before any submission; after reading the current CUA API,
the documented Return key and Open button completed the navigation. No task
was sent to an unrelated project and no existing task was interrupted.

Real keyboard entry (not the ineffective contenteditable setValue) populated
the short launcher text. It was read back in the correct composer with the
correct model before ONE Send. The resulting heading was initially
"Static final Pair Add/Sub review. Read LOCAL-REVIEW-TASK....", project label
matched, Working for1s and Stop appeared, and the follow-up composer was empty.
This occurred before the17:01Pro submission below. Review is LIVE, not accepted.
Owner Codex: retain its final actual title, verify output hashes and all findings.

## Pro final audit dispatched once

The completed source-proposal conversation "实现 Pair 加减补丁":
https://chatgpt.com/c/6a9a4269-665c-83ec-b130-8e40fd86f2d7,
Ego space122, tabD8178F5CA273531DF5B0AC24424CFA0D, visible Pro mode.
Stop was absent and composer empty before follow-up preparation. The exact
same complete packet was uploaded. A tiny unsent probe was verified/cleared;
the full task plus hash/size was entered and normalized readback matched
11925 rendered characters. Attachment card was present, upload progress zero,
Send enabled. ONE Send was observed at2026-09-04T09:01:29.654Z
(17:01:29CST), followed by Pro thinking, Stop and empty composer.
No stop, refresh, reminder or duplicate submission occurred. Review is LIVE.

Both seats must independently audit equations, validation order, exact CRT
oracles, aliasing, metadata policy, specified deep snapshots, key independence,
Codex's fixture corrections, real red-green continuity and the merged53-test
wiring. They must report blind spots and actual model identity, not equate
static approval or functional CI with full paper/precision acceptance.

## Quota, other tasks and next boundary

The official shared quota page was refreshed independently of all Pro tabs.
Page refresh2026.09.04 16:40: every5hours9%used/91%remaining, reset20:29;
week67%used/33%remaining, reset2026-09-09 10:00; MCPmonth16%used/84%remaining,
reset2026-09-25 10:00. One unused reset was not redeemed. All ZCode sessions
share this pool. Exactly one bounded new static review was dispatched here.

The BV fixed-key Pro follow-up is independently live and has verified its input
packet. Fable5.1's latest bounded terminal escalation remains403beforeinference,
not a usable review; no retry loop. The separate precision Pro follow-up has
now completed; its final text explicitly says OpenFHE/Encrypt/DCP/RCB/CTest were
NOT EXECUTED. It proposes a fixed2^-80 contract and a test-owned fresh2^100
fixture; Codex has not integrated or accepted it. Next: retrieve and verify the
complete return, inspect the unchanged contract/oracle and run an isolated
hosted red/green slice. The full project goal remains unchanged and ACTIVE.

Additional read-only CI sweep found the latest documentation-only branch runs
33853979501,33853884412,33853883551,33853055180,33853055054 all success;
the earlier expected BV probe red33844736013 remains preserved. These extra
status observations are not relabeled as new implementation tests or precision.
