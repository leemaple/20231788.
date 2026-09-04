# Reconstructed precision delivery and first hosted tracer bullet

Observed 2026-09-04 Asia/Shanghai. Owner: Codex.
Status: delivery/static continuity accepted for HOSTED RED validation;
no precision OpenFHE build or cryptographic runtime result is claimed yet.

## Source and conversation provenance

Completed Pro conversation: Double CKKS precision design
https://chatgpt.com/c/6a9a5891-6210-83ec-a24c-d6a5e0889fbd

The once-submitted delivery repair completed without stopping, refreshing,
reminding or resending the live response. This replaces the missing proposed
artifacts, not their historical evidence:
coordination/PRECISION_DELIVERY_REPAIR_BDA8791.md remains unchanged.
The author explicitly labels the replacement RECONSTRUCTED; its bytes are NOT
asserted to be the absent earlier candidate.

Downloaded ZIP:
 /Users/lifeng/Downloads/precision-delivery-repair-bda8791-reconstructed.zip
 91266 bytes
 SHA256 601c7bfbf195d383146ad63b508797322ed22cab12590fc470a241753da6f906
The relative-basename SHA sidecar agrees. A third UI link opened a verification
document preview; no local download of that separate record is claimed.

Exact safe text/source return copied without rewriting under
coordination/returns/precision-reconstructed-bda8791/.
Its original manifests and source/patch hashes remain authoritative for these
archival bytes, NOT a claim that every included proposed file is active code.

## Independent checks actually executed by Codex

- ZIP size/hash and sidecar verified; all 41 entries inspected: 34 regular
  nonempty files, 7 directories including the root. No duplicate/unsafe paths,
  symlinks, encrypted entries, compiled/cache/Git/dependency state; CRC passed.
- All 34 delivered files exist and are nonempty, including every requested
  patch, final source, document, verifier source and evidence record.
  MANIFEST.sha256 covers exactly the other 33 files; all hashes pass.
  MANIFEST.sizes-sha256.tsv covers exactly the other 32 payloads excluding
  both manifests; every size/hash passes. The record's seven source/patch
  identities agree with actual files. This checks presence, not only hashes.
- The selected 30-file baseline was compared byte-for-byte to clean-room Git
  bda879104c8a8b1ba6ac9301385b5b1919bef440, not an old local implementation.
- Read the entire delivered continuity verifier, contract, fixture header,
  green fixture, red fixture, design and oracle/cache review before use.
- Ran the read and inspected verifier on that exact 30-file snapshot:
  python3 tools/verify_contract_continuity.py
    /private/tmp/mult2-precision-handoff.oAeEtc/common/project
  It creates and removes only its own temporary replay copies.
  Exit 0, approximately 0.032 seconds. Both numbered patches and aggregate
  apply; aggregate equals sequential green; final copies equal that green;
  green changes ONLY tests/precision_dcp_rcb_fixture.cpp.
- Codex independently inspected pristine pinned plaintext access, Encrypt's
  direct GetElement path and FFTSpecialInv. The oracle uses exact secret/CRT
  recovery and direct polynomial evaluation, not a mirrored forward FFT.
- Gitleaks 8.30.1 initially scanned the extracted 257787-byte return: zero.
  A staged scan and byte-closure checks are required again before this commit.
- No provided C++ tool, OpenFHE build, project build or cryptographic test
  was executed on the Mac. Pro's standalone Boost arithmetic is a retained
  reviewer claim, not a Codex/OpenFHE runtime result.

Replay frozen hashes:
- original proposal CMake: 7daa3e85267dd1c52bc8c0ead56fc4cba58717968d6d66f9c2ab980a576d5cfc
- contract: 137612719a57f36316ee4a89d5c971300524ac9c6924c1513b1e57baeefcef01
- fixture header: 4b7b1c4f2670f5dc93e8d28f1ad585a47bb9cf4b81130bb45200a3af82e6b554
- RED fixture: f47a2b2f0446a97b62e77f41c1a1b36d759e07c7eb1b003e2b2b842383e23017
- proposed GREEN fixture: 09f1ff5c8f165a5296f3ad62e1bb9a8f41e3450fecaf371d5109c5d96b0f4907

## Controlled transplant to current combined source

Dedicated worktree:
 /Users/lifeng/Documents/20231788-openfhe-codex-precision-01
Branch: codex/precision-01
Parent: 0fc9632fe30619e160f8cd7456d333b9b2ae78a4, clean at creation.
That parent's active production/tests/build source equals tested combined
d73824c2d382013c3aadbd7cb29c57008e839714: retained run33854419062,
Linux53/53 and Windows53/53. Those are functional, NOT precision evidence.

Only the three new RED test files are transplanted byte-exact.
The proposal's NINE added CMake lines are inserted into the CURRENT CMake
instead of replacing it with the old bda8791 file. All53 existing CTest names
and command bindings, existing source/tests, warning options, API targets,
RS2 corrections and conditional BV labels are retained.
One CI branch allowlist entry admits codex/precision-01; jobs, dependencies,
pin, resource caps, warnings, test commands and cancellation policy unchanged.
Expected registration count is54, not an executed result.

## Frozen acceptance and interpretation

CTest: precision_dcp_rcb_high_precision_contract.
Context N64,batch16,depth7,p50,first55,FIXEDMANUAL,HYBRID,COMPLEX;
UNIFORM_TERNARY,degree2,scale2^100,HEStd_NotSet diagnostic only.
Four fresh-key trials; all16 literal multiprecision slots; delta(2^-70,2^-73);
fixed all-slot and delta absolute tolerance2^-80; fixed state/basis assertions;
constant, X^32 and hard-coded sixteen X^2 canonical witnesses.

The 128-bit check measures headroom of the ACTUAL centered recovered
coefficients. It is not by itself a proof about every possible unwrapped
integer/noise history, universal Gaussian tails, or a later Mult2 integer lift.
Do not promote that diagnostic check to a general non-wrap theorem.

RED intentionally routes lossless values through binary64. An intended red
must reach an UNCHANGED positive 2^-80 assertion. A compile/state/witness/basis
or headroom failure is NOT the intended red and must be diagnosed accurately.
This does not allege that standard OpenFHE encoding, DCP or RCB is defective.

Only after retaining genuine hosted red may the fixture-only green be applied;
contract/header/CMake/workflow and all existing tests then remain byte-identical.
A green is accepted only after four fresh-key trials on both hosted platforms,
warning builds and the entire54-test suite, bound to exact commit/logs.

The green adapter is TEST ONLY: public DCRT injection leaves a stale binary64
placeholder cache that must never be read, serialized or promoted to a shipping
codec. No production Decrypt, hidden API, mutable pair factory, upstream fork,
security safeguard change or production source edit is part of this slice.

## Remaining full-goal work

High-precision first Mult2, meaningful conservative error/no-wrap evidence,
production lossless I/O, refresh/repeated multiplication, paper-scale parameters,
security/performance evaluation and final tri-party review remain open.
This precursor is necessary evidence, not a redefinition of completion.
Codex owns hosted red/green execution and return review reconciliation.
No user decision or external model availability blocks the present step.

Pre-commit checks completed: exact active RED file hashes pass; all53 existing
CTest name/command pairs retained and one new registration makes54; Ruby parses
both unchanged CI jobs. Current integrated CMake SHA256 is
ab5a9873f90f5ab7d292dca4e54684e1242102ac469e0810f71508b26e39c91b.
All34 archived files are byte-identical to the extracted return.
Staged Gitleaks scanned321321 bytes, zero findings. The unfiltered whitespace
check flags eight context-only blank lines inside the THREE original .patch
artifacts. Those exact files are preserved byte-for-byte; excluding only their
explicit paths, every remaining staged file passes git diff --cached --check.

## First hosted build failure and minimal pre-red correction

Initial RED source fe35a09940e3f3f5388aa735e93ef1a8c5a5deb4 was pushed and
remote SHA matched. Run33862006375 was observed live at that exact source.
Windows job100988349902 completed FAILURE at warning-clean project build,
not CTest: GNU16.2.0 reports four occurrences of
'lbcrypto::Format' has not been declared in contract lines145,512,531,614.
Retained complete project configure/build failure section:
artifacts/tdd/precision-dcp-rcb/compile-failure-windows.txt.
Linux job100988349689 was still installing dependencies when this correction
was prepared; no Linux outcome is presumed and the old run was not cancelled.

This is a test-candidate namespace error, NOT the intended positive precision
red, an upstream arithmetic defect or a high-precision failure. No CTest ran
on Windows. Existing source files compiled until the new contract object failed.

Codex used the diagnosing-bugs workflow with the observed warning-build command
as a deterministic compiler feedback loop. The compiler already pinpoints four
invalid type qualifications; probabilistic stress, broad bisection and repeated
costly minimization are unnecessary for this syntax error and were omitted.
Existing accepted DCP/RCB tests use Format::COEFFICIENT/EVALUATION successfully.
The smallest correction removes only the four lbcrypto:: prefixes.
Static byte comparison proves every constant, vector, witness, state assertion,
tolerance, error calculation, test name and trial count is otherwise unchanged.
No warning or assertion is disabled. The lossy RED fixture is unchanged.

Corrected pre-red contract SHA256:
ad677414499c3e98e7f798ed940d587cb35c6cc791c7b0f81166ca1e6917f854.
This supersedes the delivered contract hash for the forthcoming genuine
runtime red/green pair. The original returned bytes and failed commit remain
preserved. CMake/header hashes remain unchanged from the integrated RED.
The future GREEN fixture has the same invalid qualification in three places;
apply the same mechanical namespace correction ONLY when adopting that fixture.
Do not call the old delivered aggregate byte-identical to the corrected candidate.
Hosted verification must still reach the original frozen positive2^-80 assertion
before precision RED is accepted, then fixture-only green must preserve this
corrected contract hash.
