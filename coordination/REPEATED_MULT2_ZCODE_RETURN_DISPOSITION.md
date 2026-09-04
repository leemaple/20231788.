# Actual ZCode review return — 2026-09-05

## Observed delivery and preservation

Root revalidated `/Applications/ZCode.app` at `2026-09-04T16:04:37.802Z`
(2026-09-05 00:04:37.802 Asia/Shanghai). The correct task was
`Repeated Mult2 candidate review per TASK.md`, under the dedicated
`20231788-openfhe-zcode-repeated-probe-review-20260904` project. It showed
**Worked for 24m 16s**, no Stop control, a completed final response and
two changed files. This is actual Mac LOCAL STATIC review, not Windows
runtime. Root observed the client model label **GLM-5.3 / Max**; the reviewer's
own report accurately says it could not observe model identity inside its
session. These are separately attributed observations, not Fable 5.1 output.

Original reports are preserved byte-for-byte at
`returns/repeated-mult2-zcode-05c8cd8/output/`:

| File | Bytes | SHA256 |
| --- | ---: | --- |
| `REVIEW.md` | 27081 | `fb0cda07d7c8567d99353dc586bda30543fb7f2a5219c910418611e146f81021` |
| `EXECUTION_LEDGER.md` | 15889 | `e859e846caf1f5aeedb821eec3844d7b48c41034dd615106cd2167ab9ecd2706` |

Root fully read both reports and independently rehashed all **196** original
input-manifest entries after completion: unchanged. Only the two expected
files were present in `output/`. Root's gitleaks 8.30.1 scan of those outputs
(ambient config overrides unset, ignore-inline-allow, ignore file `/dev/null`,
decode depth 5, GOMAXPROCS=2) scanned **42970 bytes**, zero findings, exit 0.
The original report's Sep4 date refers to its session, which crossed local
midnight; root's terminal observation above is Sep5. No report bytes were
rewritten to harmonize dates or reviewer identity.

## Reconciled disposition

The external verdict is **REQUEST_CHANGES on the incoming static candidate**.
The packet is an allowed, accurately labelled bounded construction/key-routing
probe; it is not a semantic second-Mult2 implementation. Root retains the
existing [R1–R6 disposition](REPEATED_MULT2_ACTUAL_RETURN_DISPOSITION.md):

- R1/V1: confirmed P1 unconditional MSVC/GCC warning flags; correct before
  any authorized hosted adoption. Static defect, not an observed failed build.
- R2/V2: confirmed missing actual CKKS scheme identity; pass the scheme ID
  during construction and validate the returned context, not mutate it later.
- R3/V3: confirmed requested-versus-returned parameter validation and
  shape-only immutability evidence. No runtime mutation is alleged.
- R4/V4: confirmed reused first-precision result label in the new rejection
  fixture. Original first-precision regression must remain unchanged.
- R5/V5: confirmed text inconsistency: h128 gates the paper setup, not the
  permitted low-N semantic second-operation diagnostic.
- R6: duplication remains a KISS recommendation, not authority for a broad
  refactor or a new test seam.
- The ConstCiphertext-reference objection remains disproved by the pinned
  alias; all-key/HEaaN-law theorem requirements are not added.

Additional review findings verified by root:

- **V6, accepted documentation correction needed.** The returned
  `SOURCE_LINE_INDEX.json` mislabels §6.1 (1472–1517) as §6.3 and points
  Table 3 to an introduction mention (169–214). Root read the exact paper
  bytes: §6.3 is 1562–1590, Table 3 caption/data 1580–1590. Preserve the
  received index as historical evidence; correct a later candidate/errata.
- **V7, accepted citation correction needed.** The full-basis tower append
  is in `double_ckks.cpp:925–942`, and `SetPrivateElement` is at
  `privatekey.h:139–151`. This is citation drift, not a new source defect.
- **V8 and V2's enum evidence gap, now closed by root source retrieval.**
  The three exact official declaration files and hashes are recorded in
  [supplemental provenance](evidence/repeated-mult2-review-pristine/PROVENANCE.md).
  They prove public A/B-vector getters and the enum spelling; no build or
  cryptographic operation was executed to close this source-only gap.

## Next boundary

No candidate patch, source, test, build configuration or CI was changed by
this receipt/review work. The repeated-operation client-setup/Mult2 seam
confirmation remains unanswered, so no new test is adopted at that boundary.
Separately, Pro's I/O design has now returned and is being reconciled with the
source-reviewed h128 proposal; its delivery is not an implementation.

The next implementation gate remains actual semantic second Mult2, then
eight operations and the paper parameter/1000-trial experiment. Passing the
received rejection freeze or random-tensor shape probe cannot satisfy it.
