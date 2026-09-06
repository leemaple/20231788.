# First candidate full-eight hosted observation

## Activation observed

- Exact source: `2b8b349edf5575556347082c1b725f6696c743b6`.
- Working ref `codex/precision116-eight-square-20260907` was pushed and verified first; it does not trigger this workflow.
- Dedicated ref `codex/precision116-eight-square-observation-20260907` was observed absent before activation, pushed once, and `git ls-remote` confirmed the exact source SHA.
- [Run 34055816234](https://github.com/leemaple/20231788./actions/runs/34055816234), created `2026-09-06T19:44:09Z` / September 7 03:44:09 Asia/Shanghai, workflow `OpenFHE 2023/1788 TDD`.
- Initial `gh run list` result: exact source, `in_progress`, empty conclusion. No rerun or manual dispatch.
- Subsequent `gh run view` confirmed attempt 1: Linux job `101547399114` started `19:44:13Z`, Windows job `101547399237` started `19:44:11Z`. At this checkpoint Linux was building pristine OpenFHE and Windows was installing its official toolchain. Neither had reached the new candidate target or numerical test.

This is one Linux observation and one Windows observation, not a trial batch. Compilation and eight-square numerical outcomes remain pending; do not infer PASS from workflow creation or static review. Fetch and bind the final run/attempt and full job logs before interpreting the outcome. Original S100 E80 FAIL and experimental security UNRESOLVED remain unchanged.

## Linux completed — Windows still pending

Linux job `101547399114` completed successfully. Its raw log was downloaded once using the job-log API and retained as `LINUX_JOB_101547399114.log`: 544326 bytes, SHA-256 `883a5114c364953cf65f0aed9ff4f6559d49b5c15344dc697ae943b5d30a3486`.

The new CTest passed in 20.54 seconds. Its sole START and COMPLETE bind the exact source `2b8b349...`, baseline `2759fa9...`, experimental profile, native64/backend4, one chain/eight squares/16384 slots. COMPLETE reports result PASS, zero numeric gate failures, structural/oracle checks VALID and cleanup PASS. Root extracted 144 unique measurement fields, confirmed all eight round-anchor maxima, and checked reported error/codec values against their frozen thresholds using bounded decimal arithmetic, without cryptographic replay.

- Fresh full-slot maximum: `5.7033521175851448589869119502284064048808222512142101991079408379372948327e-30`.
- Final full-slot maximum: `2.5905123324714234303144832599280586708902635685451002239719941962510645829603444214877194832613040822e-26`, at slot12568/real.
- Frozen E80 limit: `8.2718061255302767487140869206996285356581211090087890625e-25`; threshold divided by final observed error is approximately31.93116. This is one draw's margin, not a probabilistic guarantee.
- Final independent-anchor maximum: `2.3337989099645534575245041844589572505638286323645888529567336866471394525546647150275618303159731191e-27`.
- Wrong nominal scale anchor0 error: approximately `1.53649086e-6`, above the required `2^-30` falsifier threshold.

An independent source/log reconciliation is in progress. Windows and overall run acceptance remain pending at this checkpoint. No extra observation was dispatched.

## Both hosts completed successfully

The final run status confirms attempt1/source`2b8b349...` completed SUCCESS on both hosts. Windows job`101547399237` completed at `2026-09-06T19:56:53Z` (September7 03:56:53 Asia/Shanghai). The read-only watch process exited0; no observation remains running and no rerun is needed.

- Windows raw log `WINDOWS_JOB_101547399237.log`:557035bytes, SHA-256`bfb5b7ba179842f7fa1bfa230ef508388f0558b614a55f9e905a9538556b502d`.
- Exact final status `RUN_34055816234_STATUS.json`:14607bytes, SHA-256`9d99816a8e903ba3d6b8ceaa5ab3665fb6efaa19153d4637b34cb327bd7e4b72`.
- Windows new CTest:21.89seconds, COMPLETE PASS, zero numeric misses, structural/oracle checks VALID and cleanup PASS. Final full-slot maximum `3.4805603371613676977710923112292752103286627796357395987548688159063674576870987628082160966009172080e-26`, approximately1/23.76573 of E80.
- Both logs show124 passing CTest invocations covering61 unique tests: the existing60 plus exactly one new eight-square test. The old original-profile experiment and old experimental one-operation seam were not rerun.

`ROOT_RUN_RECEIPT.json` records root's bounded scalar/log reconciliation: one START/COMPLETE and no ABORT on each host; exact source/profile/chain/output classification;144 unique measurement fields per host; all9 returned stages; reported component errors, codec consistency, witness deltas, actual domain and wrong-scale falsifier within their respective frozen predicates. These checks consumed retained logs only, not another encrypted sample.

This establishes an observed PASS for the stated experimental profile on these two draws; it does not establish exact original-table replication, all-key correctness or cryptographic security. Independent runtime review and final delivery/mainline integration remain to be closed. Original S100 FAIL is preserved.
