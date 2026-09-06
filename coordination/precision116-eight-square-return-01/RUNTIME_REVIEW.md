# Independent hosted-runtime reconciliation

## Disposition

The retained Linux and Windows jobs are valid, bounded first numerical **PASS** observations for one exact experimental precision116 eight-square chain per host. Each satisfies the task's classification boundary: the candidate compiled and linked, the dedicated test started, established the frozen candidate and input identity, reached all eight operations and their numerical predicates, reported valid structural/oracle checks and cleanup, emitted one COMPLETE with `result=PASS`, and passed its one selected CTest.

No contradiction or actionable hosted-runtime blocker was found. The retained run-status record binds both jobs and the overall attempt to terminal SUCCESS. This is still not an overall project-completion or security verdict.

## Evidence and identity

- Run `34055816234`, attempt `1`: Linux job `101547399114` and Windows job `101547399237`.
- Retained raw log `LINUX_JOB_101547399114.log`: 544326 bytes, SHA-256 `883a5114c364953cf65f0aed9ff4f6559d49b5c15344dc697ae943b5d30a3486` (recomputed locally).
- Retained raw log `WINDOWS_JOB_101547399237.log`: 557035 bytes, SHA-256 `bfb5b7ba179842f7fa1bfa230ef508388f0558b614a55f9e905a9538556b502d` (recomputed locally).
- Retained `RUN_34055816234_STATUS.json`: 14607 bytes, SHA-256 `9d99816a8e903ba3d6b8ceaa5ab3665fb6efaa19153d4637b34cb327bd7e4b72` (recomputed locally). It records exact source `2b8b349...`, attempt1, overall `completed/success`, and both named job IDs as `completed/success`; each job's configure, build, and run steps for the new observation are successful.
- Tested project source: `2b8b349edf5575556347082c1b725f6696c743b6`. The log's clean-room provenance record, test START, and test COMPLETE all carry this exact SHA. The later worktree HEAD is documentation-only relative to that tested source.
- Frozen task SHA-256: `1fb9e2188ba8f4f90f3b4ac3d5fe7c4bdd248529363001366fe2e4cbfdf71a0d` (recomputed locally).
- The tested source commit's exact blobs match the reviewed integration slice: test `478954a08580d3d342a47ad1e54900f4dce455af1964a3d3e2d231cf61b748d2`, CMake `9cba634cedde1e8d96aff8561f5b4ed545b52fac8f1ce523e481a7833e1936f8`, and workflow `6095c87e3c79de388a73fbf816a5bfa772a6175e08631d219eab7f8d12672a05`.
- Review responsibility is the separate Codex task context `endpoint_interop_workflow`. Its exact model/backend identity is unknown/unattested; this is separate context, not a claim of provider diversity.

## Build, provenance, and prior regressions

Both logs record pristine OpenFHE checkout and verification at `df495ba2e91739a6dc8f1de254fc5a41155ce504`, with no dirty-status output. Each project provenance step checked `HEAD == GITHUB_SHA`, checked a clean worktree, and printed the tested project SHA, run, and attempt. Linux used GNU C++ 13.3.0; Windows used GNU C++ 16.2.0 under MINGW64; both report native64/backend4. On each host, the fresh option-enabled build compiled the four project production translation units and `experimental_precision116_eight_square_test.cpp`, then linked the dedicated executable successfully.

Before the new observation on each host, the final complete legacy CTest invocation contained exactly 60 Start records with 60 unique names and ended with all 60 passed. The earlier 57-test checkpoint also passed on each host. Each of the five public compile-only API targets (`relin2`, `rs2`, `mult2`, `add`, and `sub`) has exactly one successful build/link receipt per host. Focused invocations necessarily repeat some legacy tests elsewhere in each job; they are not additional experiments and do not undermine the 60-name uniqueness of the final suite.

## Dedicated test and profile binding

The option-enabled build registered the test with `TIMEOUT 1200`, `RUN_SERIAL TRUE`, and `OMP_NUM_THREADS=2`. Each hosted step selected the exact anchored name `^experimental_precision116_eight_square_contract$` with `--no-tests=error`; each log contains exactly one Start for it, records OMP2 and computed timeout 1200, and reports 1/1 passed with zero failures: Linux in 20.54 seconds and Windows in 21.89 seconds.

Per host there is exactly one candidate START, one COMPLETE, no ABORT, and no `numeric_gate=FAIL`. Both START records bind source `2b8b349...`, baseline `2759fa9...`, profile `experimental-s116-d56-b58-v1`, eight squares, 16384 slots, one requested chain, the pinned OpenFHE SHA, native64/backend4, and `security=UNRESOLVED`. Each candidate-establishment record binds `paper_full_test::Inputs`, N32768/M65536/gap1/h128, eight families, metadata exponent 58, recorded exponent 116, and tensor exponent 174. The hosts report different Boost versions (Linux 108300, Windows 109200), without changing the frozen profile or result classification.

On both hosts, all 11 logged Q modulus/root tuples and the reserved P/root exactly equal the frozen static certificate. Each host's nine returned-stage records has the required shape:

- round0: family0 Input, level1, 10 towers, nonterminal;
- rounds1--7: matching families1--7 Reentry, level1, towers9 down to3, nonterminal;
- round8: family7 Rescaled, level2, two towers, terminal.

Every stage records exponent116/degree2, and on both hosts every exact scale numerator/denominator is integer-equal to the corresponding independently closed-product value in `STATIC_CERTIFICATE.json`. The tested source checks the exact phase/family/operation/terminal state, basis/root/tag/lifecycle, exact reduced scale, and parent identity for each stage. It then requires an acyclic terminal ancestry containing exactly 32 distinct receipt objects and binds the terminal wrapper receipt to the final returned receipt. Because those identities are not serialized individually, “32” here is a successfully executed source assertion supported by each COMPLETE/pass disposition, not a log-level reconstruction of 32 retained objects.

The frozen new-prime Proth-form and witness checks execute before candidate setup in the bound source. As with the 32-node ancestry, the raw logs do not serialize each witness result; their success is an executed source assertion implied by reaching candidate establishment and COMPLETE without ABORT, not an independent primality recertification in this review.

## Numerical and structural reconciliation

I evaluated the exact decimal strings from both logs against the frozen scalar predicates without binary floating-point conversion; shortened values below are explicitly marked “about”:

- `2^-80 = 8.2718061255302767487140869206996285356581211090087890625e-25`.
- Linux fresh/final full-slot maximum component errors are `5.7033521175851448589869119502284064048808222512142101991079408379372948327e-30` and `2.5905123324714234303144832599280586708902635685451002239719941962510645829603444214877194832613040822e-26`. Windows fresh/final values are `5.8341169312125851949799678538169857096346801677706643002334422404867839248e-30` and `3.4805603371613676977710923112292752103286627796357395987548688159063674576870987628082160966009172080e-26`. All four are below `2^-80`.
- Linux fresh/final codec disagreements are about `4.32834e-165` and `8.35048e-166`; Windows values are about `4.32790e-165` and `8.35304e-166`. All are below `2^-120`.
- Linux fresh/final actual slot1-minus-slot0 real values are about `2.64698e-23` and `6.91106e-22`; Windows values are about `2.64698e-23` and `6.91104e-22`. All exceed `2^-76`. Linux disagreements from the independent expected deltas are about `1.63808e-30` and `1.90412e-27`; Windows values are about `9.84865e-31` and `1.56617e-29`. All are below `2*2^-80`.
- On both hosts the exact final expected witness is about `6.91104e-22`, strictly between `2^-71` and `2^-70`. The source also asserts the frozen slot0 expected scalars and every ideal final-domain interval before COMPLETE.
- Actual centered headroom is positive at both endpoints on both hosts. Each reports `failing_slots=0`; the Linux and Windows final minimum actual squared magnitudes are both about `9.67816e-3`, above `.09^2`.
- On each host fresh, round0, rounds1--8, and final ten-anchor maxima are all below `2^-80`. The largest Linux and Windows reported round/terminal maxima are respectively about `2.33380e-27` and `1.94596e-27`. Fresh/final independent polynomial-versus-production anchor disagreements remain around `10^-102` on both hosts.
- The deliberately wrong nominal-`2^116` terminal normalization produces anchor0 component error about `1.53649e-6` on each host, above `2^-30`; this is a discriminating falsifier, not merely a scale-inequality assertion.

The source traverses all 16384 decoded slots at fresh and final, checks both real and imaginary component errors against the independent ideal, and checks the final actual-domain predicate for every slot. The raw logs retain maxima, witness values, the minimum-domain value, and failure counts rather than all per-slot values. Accordingly, this review reconciles each host's executed all-slot endpoint assertions and summaries; it does **not** independently replay or reconstruct all slot values.

The source-side independent oracle uses the frozen sparse signed-h128 secret, inverse NTT, exact CRT and binary512 Horner outside `Evaluate`; both runtimes reach `structural_oracle_checks=VALID`. Each evaluator execution contains one DCP, exactly eight `Mult2(pair,pair)` operations, and one successful terminal RCB. Successful completion also covers the required nonterminal/foreign-plan rejections, input/key/cipher immutability assertions, candidate-row release, preservation of the concurrent small diagnostic setup, and restoration of global caches. Each COMPLETE reports `cleanup=PASS`, `numeric_gate_failures=0`, and `result=PASS`.

## Preserved limits

- Security remains explicitly `UNRESOLVED`; no security estimate or inherited 128-bit claim follows.
- The original S100 profile remains recorded FAIL and was not rerun or reclassified by these jobs.
- Only ten anchors are observed at intermediate rounds. The test itself records that these do not prove all-slot intermediate accuracy/nonwrap and that Tensor/Relin lift safety is `NOT_PROVED`.
- This reconciliation performed no compilation, cryptography, CI polling/download, full-slot replay, or new experiment. It used the two retained raw logs, the retained terminal run-status JSON, exact tested Git blobs, source control flow, integer tuple/scale equality, CTest-name counts, and exact decimal threshold comparisons.
