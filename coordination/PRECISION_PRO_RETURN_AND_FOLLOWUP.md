# Precision return and frozen-contract follow-up

Observed2026-09-04, Asia/Shanghai. Source packet remains exact bda879104c8a8b1ba6ac9301385b5b1919bef440.
Conversation https://chatgpt.com/c/6a9a5891-6210-83ec-a24c-d6a5e0889fbd,
title Double CKKS precision design. Original Pro response completed55m8s;
Stop disappeared and final/download controls appeared before any follow-up.

## Verified original return, not implemented

/Users/lifeng/Downloads/mult2-precision-design-review-bda8791.zip:
83686bytes, SHA2564adb4832a47158b010932bc12cd1dc1c9b3790ad52ef68a1dc8d2ae521035cb6.
Codex matched size/hash to the visible final response, inspected all32 ZIP
entries(26 regular files,6 directories; no symlinks/unsafe paths), recomputed all
25 MANIFEST.sha256 entries and exact26-file closure. Gitleaks8.30.1 extracted
stage and archive-depth2 scans found zero secrets. No supplied tool was executed.

Pro marks candidate OpenFHE compilation, crypto, CTest and CI NOT EXECUTED.
Its standalone math result(about1.444e-30) is its reported arithmetic-kernel
evidence only, not a local/hosted OpenFHE result. The original final candidate
has NOT been copied into the source tree, compiled or claimed passed.

Source/design findings to investigate: Native64 degree2 lifts a base-rounded
integer rather than adding another block of information; standard canonical
input/output is binary64; REAL decode masking remains intact; a public-DCRT
fixture can carry freshly rounded2^100 coefficients but leaves the standard
private value cache stale. Such injection must remain test-only. Homogeneous
p50/50 would be a diagnostic bridge, not Table3's ordered40/60 regime.

Codex read README, DESIGN, RISK_AND_DECISIONS, TEST_FIRST_PROPOSAL and the first
two complete patches plus relevant third-patch hunks. Remaining detailed
precision diagnosis, experiment matrix and full final codec still need source
review before integration; package verification is not full code acceptance.

## Material pre-integration corrections requested

1. Original patch0002 asserts impossible sub-double preservation through the
   standard double API. Patch0003 deletes/renames that test and substitutes a
   new fixture adapter. Useful diagnostic facts, but not an unchanged-contract
   production red-green cycle. Preserve the same accepted test/threshold/values
   through red and green; distinguish a fixture limitation from an OpenFHE bug.
2. A paired port of inverse and forward special DFTs could hide matching phase
   or permutation mistakes. Require an independent direct canonical evaluation
   or discriminating literal coefficient/monomial witnesses.
3. Keep the next tracer bullet at agreed public DCP->RCB. Codec calibration is
   test fixture/oracle support, not a newly adopted shipping API. Exact-scale
   result, production codec/output, parameter factory and lifecycle remain
   proposals pending their own evidence/seam decision.
4. Check Boost floor/ceil conditional expression materialization portably;
   no unobserved compiler failure is asserted.

## Complete-context follow-up submitted once

Outer archive /private/tmp/precision-tdd-followup.MHDZJM/precision-tdd-continuity-bda8791.zip:
1114585bytes, SHA25610f5ee04e6f90047b175ddcf8b7431b4e9c01eb263823f9c80dd9661ab48de93.
It has exactly4 safe regular files: the full original input ZIP, full original
return ZIP, TASK.md and SOURCE-MANIFEST.json. Both nested archive hashes, task
hash82414d059912e8d3f4e9d4f85708e1b2083dafec77b49f508d704508fb1acc38,
all lengths and manifest closure were checked before upload; unzip-t passed.
Gitleaks8.30.1 recursive archive-depth3 scan found zero findings. Original
nested archives retain full paper/source/reference context; no prior memory is
assumed. Detailed task and manifest are retained under coordination/tasks and
coordination/handoffs with precision-tdd-continuity names.

Ego space122, same completed conversation. File card ready, no progressbar,
Pro visible in composer form, full normalized prompt readback matched exactly
before the single Send. Submitted2026-09-04 14:43:08CST; composer cleared,
Stop answering appeared, real conversation URL unchanged. No live thought was
stopped/refreshed/reminded or task duplicated. A tiny unsent composer probe was
fully removed before verified full text; it was not part of the sent request.

Current owner Codex: keep this live handle, finish BV conditional CI and the
remaining pair lifecycle/negative review track. Await revised frozen-contract
precision candidate; do not adopt the original full patch to manufacture green.
