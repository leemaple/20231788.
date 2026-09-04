# Precision continuity return: delivery incomplete, repair dispatched

Observed 2026-09-04 Asia/Shanghai. This supersedes the live-status paragraph
in PRECISION_PRO_RETURN_AND_FOLLOWUP.md, not its historical evidence.
The current clean integration branch was codex/integration-01 at
a2c1164d5fab9c37d88ec5ea48c773bbe8b89494. No source, tests, build workflow
or precision acceptance threshold was modified by this receipt.

## Completed response and verified defect

Conversation "Double CKKS precision design":
https://chatgpt.com/c/6a9a5891-6210-83ec-a24c-d6a5e0889fbd.
The 14:43:08 CST continuity follow-up completed before the 17:01 Pair audit
dispatch. Its final response proposed a frozen 2^-80 DCP->RCB contract with
16 literal complex values, delta (2^-70, 2^-73), four fresh keys, 128-bit
non-wrap headroom and a test-owned fresh 2^100 plaintext fixture. It explicitly
did NOT execute candidate OpenFHE compilation, Encrypt/DCP/RCB, CTest or CI.

Downloaded once through the completed response:
 /Users/lifeng/Downloads/precision-tdd-continuity-review-bda8791.zip
35999 bytes, SHA256
3406367904b45131c7f3b7bd8a66cbe116aaa941799b082ab3582f5fcfaafaee.
The separately downloaded SHA sidecar matched. Safe-path/duplicate/symlink
inspection and all 17 manifest payload hashes plus own 18-file closure passed.
There are 26 ZIP entries (18 files, 8 directories).

Required delivery completeness nevertheless FAILED:
- no patch files and no final source files at all;
- missing PATCH_SERIES_AND_CONTINUITY.md, SOURCE-AND-DELIVERY-RECORD.json,
  standalone math C++ source, INPUT_INTEGRITY.txt and TEST_CONTRACT_HASH_LEDGER.txt;
- PATCH_APPLY_AND_CONTRACT_CHECK.txt and PINNED_SOURCE_HASHES.txt are empty;
- FINAL_CHANGED_FILES.sha256 contains only the empty-input hash for "-";
- a compiled __pycache__/verify_contract_continuity.cpython-313.pyc is present.

A self-consistent manifest does not establish deliverable completeness.
The final message/README's complete-patch and executed-patch-check claims are
not supported by the delivered bytes. No source was accepted or executed.
The claimed standalone numerical result cannot be independently reproduced
from this return because its claimed source was absent.

Seventeen safe text files, including the two genuinely empty evidence files,
are preserved byte-exact under coordination/returns/precision-continuity-incomplete-bda8791.
The forbidden cache was not extracted or uploaded. The historical MANIFEST
still names that omitted cache: this is a disclosed sanitized selection, not
a false claim of original manifest closure. RETURN-VALIDATION.json and
RETURN-ZIP-LISTING.txt separately preserve exact defect evidence.
Codex read all design/oracle/risk/matrix/claim documents and the supplied
continuity verifier source. No supplied script or binary was executed.

## Bounded complete-context repair, submitted once after completion

Archive:
 /private/tmp/precision-delivery-repair.dwkLpR/precision-delivery-repair-bda8791.zip
1151735 bytes, SHA256
d36d294976b230a0b769e8b683b58dad45d2b87b1b7800deced6091a32a50a62.
25 entries = 22 regular files + 3 directories. All 21 payload sizes/hashes
and equality to selected disk bytes were checked; manifest-self closure exact.
Nested archive listings, duplicates, traversal/symlink/excluded-class checks
and CRC tests passed recursively. No old user implementation, modified local
OpenFHE, credentials, browser state, build products or caches were included.

The payload contains the full earlier input ZIP, all 17 safe returned text
files, exact anomaly/listing records and the complete repair task.
PREVIOUS-FULL-CONTEXT.zip is 1114585 bytes, SHA256
10f5ee04e6f90047b175ddcf8b7431b4e9c01eb263823f9c80dd9661ab48de93,
including both original input and original return ZIPs, paper, pristine source
references and the original complete continuity brief. Proposal source remains
bda879104c8a8b1ba6ac9301385b5b1919bef440; pristine OpenFHE remains
df495ba2e91739a6dc8f1de254fc5a41155ce504.
Gitleaks 8.30.1 input and final-archive scans with max-archive-depth 4 found
zero leaks (~1.81 MB inspected). Clean scans are evidence, not guarantees.

Ego space122, tab6F1085C9C7D46593EFA146D644D7004B, same completed conversation.
Stop was absent and composer empty. A tiny unsent probe landed in the composer
and was fully cleared before the complete task was filled. Normalized full
readback matched (9081 rendered characters), attachment card and Pro label
were present, progress count zero and Send enabled. ONE Send occurred at
2026-09-04T09:19:01.335Z (17:19:01 CST), then empty composer and Stop/Pro thinking.
No running thought was stopped/refreshed/reminded or duplicated.

This requests artifact recovery, not an algorithmic restart. If original
proposed files are lost, any replacements must be labeled RECONSTRUCTED with
new hashes. The frozen contract cannot be weakened. Required-file presence,
nonempty actual evidence or honest NOT EXECUTED records, exact patch
continuity, final-file equality, manifest closure and fresh-extraction
rechecks are all explicit gates. The raw cache-containing defective ZIP was
NOT re-uploaded. Detailed task and outer manifest are retained in
coordination/tasks and coordination/handoffs.

## Current boundary and owner

Repair is LIVE, not accepted. Codex owns retrieval, completeness/hash/source
review and eventual isolated hosted red/green. No precision branch or test
run has been created from the incomplete return. Current independently
verified combined source d73824c2d382013c3aadbd7cb29c57008e839714 still has
53/53 on each host (run33854419062); that is functional, not precision proof.
Future reviewed new precision tests will start from that combined source,
not restore the historical bda8791 branch's two BV empirical failures.
Any eventual fixture red diagnoses insufficient fixture input, not a DCP or
upstream encoding defect; DCP->RCB green will not prove Mult2 precision,
repeated multiplication, Table 3, security or a production codec.

No Mac project/OpenFHE build or cryptographic test occurred. The separate
Pair and fixed-key BV Pro audits continued without interruption.
