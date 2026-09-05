# Paper-scale slice — retained API RED accepted

Exact engineering source `448e9d3067796b64656f0746f4be5c4153b1271d`,
branch `codex/paper-scale-implementation-20260905`.
Automatic [run33968576607](https://github.com/leemaple/20231788./actions/runs/33968576607),
push/attempt1, created2026-09-05T13:19:46Z.

The two hosted jobs completed with the expected missing-public-API compile
failure: Linux101312943097 at13:26:01Z and Windows101312942973 at13:30:28Z.
The only failed step in each job is the explicit paper contract build.
**This is accepted compile-time API RED, not an executed paper numerical
failure and not paper correctness evidence.** The new executable never ran.

## What actually ran before RED

Each host completed123 bound Start/printed-argv/Passed records in the exact
sequence1+2+57+1+2+60. The live57 and60 listings match the exact source CMake
name/command/order and add_test backtrace lines. Five public API targets and
all three previously accepted contract targets built successfully. The final
existing suite is60/60 on each host, not61/61.

Retained I/O invocations each keep the original N64/16/gap2, exact rational
scale,2^-80/2^-120 gates, positive headroom and ordered negative/clone/live-basis
markers. Repeated has16 stage records per host, not32 per host, with exact
S1/S2, frozen diagnostic parameters and numeric gates. h128 has two runs per
host, all50 named rejects and owned-row uniqueness/isolation markers. None
of these diagnostic results is relabeled as the new N32768 test.

## Exact RED cause

Both compilers report the new required result type, RCBWithReceipt,
BindRepeatedRcb, CreatePaperRepeatedMult2Setup and RepeatedMult2Rcb origin
are missing from the exact source. The existing production headers/sources
at448e9d3 indeed contain none of those symbols; the new test requires them.
Remaining template/invalid-type/unused-function diagnostics are direct
cascades of the absent declarations. The lead read both complete extracted
error sets and found no independent unexpected diagnostic. Linux's explicit
build exits2; Windows's exits1. There are zero paper Start, runtime command,
BEGIN or COMPLETE records; the focused paper run was skipped.

The test now defines a real full single-chain behavior that must execute
after implementation. The missing-type assertions alone are not its future
GREEN proof. All gates, inputs, old tests and source pin stay frozen. Do not
fix RED by weakening the test, expecting an unsupported exception, stubbing
success or rerunning unchanged source.

## Retention and independent verification

Each job log was fetched once through the GitHub job-log connector. The
published Linux bytes are exactly its UTF-8 decoded content:466976 bytes,
SHA256 bf6343f35f61b53905cdc9764392c56a04c96e059dc86900de0808954f799595,
with BOM and LF.

Windows original connector-decoded content is480879 bytes,
SHA25610a7f2a19a335979206f481b4223f1e0fb474cc9d43d616b1811363cb575bd05,
with BOM and5949 CRLF pairs. That original is preserved in the ignored
`artifacts/handoffs/paper-scale-red-01/WINDOWS_CONNECTOR_CAPTURE.json`.
The published Windows log is the exact CRLF-to-LF-only transform:474930 bytes,
SHA2560fa3ece49d8a3205993b231570a213eb11d6582bb71b02b6d576b56a98e35199.
These are connector-decoded bytes, not raw HTTP transport bytes.

A separate GPT-5.6 Sol/medium review context authored and ran the strict
retained-source/log parser. The lead read the complete script and reran it
for both hosts using the bundled Python runtime; each full JSON output is
byte-identical to the independent result. ROOT_REEXECUTION_01.json records
those exact bytes/hashes and the246 total bindings. RUN_TERMINAL_01.json
retains fresh exact-source/job-step metadata. Published evidence files are
unchanged copies of the audited artifacts, with SHA256SUMS.

The script intentionally records the original absolute handoff paths and
source-HEAD gate used during this audit. Re-execution from a later docs or
implementation HEAD requires restoring that exact isolated audit setup; it
is not advertised as a portable standalone CI command. Original connector
captures stay in the ignored handoff directory. No data was re-fetched and
no hosted build/crypto was repeated for this audit.

## Handoff to implementation

The complete source448e9d3 plus frozen contract, independent oracle, full
paper and pinned official files was already delivered once to
[Implement Paper Scale Slice](https://chatgpt.com/c/6a9c1865-8274-83ec-ab61-0bc9128567af)
at13:25:56.304Z; detailed receipt is in
`coordination/paper-scale-pro-implementation-01/SEND_RECEIPT.md`.
It was submitted while CI was still in progress, without claiming a RED
result in the prompt. Pro is drafting production code uninterrupted.
This retained RED acceptance precedes applying any such patch.

Next: actually receive and inspect the production artifact; integrate only
the minimal accepted production change; commit/push an exact GREEN source;
run the configured one full chain per host plus regression checks; inspect
precision, lifecycle and independent-oracle evidence. No Mac build/FHE,
1,000-trial campaign, normal CI dispatch/rerun or default-branch merge occurred.
The complete paper implementation remains unfinished.
