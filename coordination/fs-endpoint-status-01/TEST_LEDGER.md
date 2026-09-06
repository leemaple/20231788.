# Status byte codec — actual bounded Python TDD

Root author, clean-room main base9f12e1759055a0c4c0f9a794278768a6101f277e.
No C++/crypto/FFT/NTT, full-slot replay, CI invocation or file publication was
performed by these checks. Synthetic fake hashes/byte counts test the wire
contract only; no fixture is live evidence.

Public command used for each saved receipt:

`/Users/lifeng/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/bin/python3 -m unittest discover -s tests -p test_paper_endpoint_status.py -v`

| Receipt | Observed result | Change |
| --- | --- | --- |
| evidence/01-import-red.json | Missing module, exit1 | First public complete/failing-E80 fixture before implementation |
| evidence/02-roundtrip-green.json | 1 PASS, exit0 | Minimal canonical JSON roundtrip |
| evidence/03-schema-red.json | 3 discovered, 37 assertion failures, exit1 | Closed keys/types/identity/complete-disposition mutations expose absent validation |
| evidence/04-schema-green.json | 3 PASS, 0.001s, exit0 | Exact schema and complete count/CTest/null/digest/filename rules |
| evidence/05-incomplete-red.json | 5 discovered, 13 errors, exit1 | Truthful incomplete records rejected by complete-only validation |
| evidence/06-incomplete-green.json | 5 PASS, 0.001s, exit0 | Explicit first-cause class table/nulls/retained numeric evidence shape |
| evidence/07-bytes-red.json | 7 discovered, 11 failures + 2 errors, exit1 | Alternate/duplicate/malformed/unbounded JSON accepted or wrong exception boundary |
| evidence/08-bytes-green.json | 7 PASS, 0.002s, exit0 | Bounded immutable ASCII bytes and exact canonical re-encoding; parser-boundary failure translation |

Final tested environment: CPython3.12.14. Source
`bc5fa49aa104ec331df73335b7f2f02f0afb10b9f05c6b50f23111e62d5a460d`;
tests `a9bca424f907bae75ce37aced4e34724dfeb9ac9812af5a85b91f014706a423c`.
The actual focused test process was subsecond. Two generated CPython cache files
were removed after inspection; they are reproducible cache only, not source or
evidence. Future local checks use `-B` to avoid creating them.

Independent source-first review assigned to `/root/endpoint_canonical_writer`
(requested GPT-5.6 Sol/high, backend requested-unverified) in a separate context
from root authorship. Review result is pending. No claim of full packer,
first-cause *detection*, actual hash correspondence, transaction, upload,
original E80 PASS or project completion follows from this codec.

## Independent review and targeted follow-up

The reviewer completed its source-first pass and challenged retained count/E80
versus exit disagreement in an incomplete status. Root cited the distinction
between the COMPLETE-only consistency requirement and incomplete observed facts:
an original COMPLETE PASS/count0 can be followed by teardown/signal failure;
an inconsistent primary FAIL/exit0 must remain a visible INTEGRITY failure.
After checking the exact specification, the reviewer explicitly retracted this
as a defect and agreed the incomplete record should preserve those facts.
Neither pathway permits a COMPLETE evidence claim. Two public tests now
discriminate these cases; no changed E80 rule was introduced.

Reviewer-requested nested values and exact 256 KiB / 256 KiB+1 boundaries were
added; their first executions passed against unchanged implementation, not a
new behavioral RED. A separate scalar-length precedence assertion failed
against that source (evidence09: 11 discovered, 1 failure, 0.006s). After adding
an upfront scalar-string bound before stem and JSON allocation, evidence10
records **11 PASS in 0.006s**, Python3.12.14, exit0. The output remains bounded
to256KiB; input strings are already caller-owned and the guard bounds derived
allocations without inventing a smaller run-ID semantic range.

Current source SHA-256:
`f443ff51dbe4aa24b21bfd5d1eff5986a7bcfd064ee1d7bae55ffa6cce283c34`.
Current tests SHA-256:
`fce18d0a851acfd61191dcc53c7f12125eb570a4a9d268ccc7976e93c626bce3`.
Evidence10 includes the exact command and immediately preceding red-source/test
hash receipt. Early receipts01–07 lack contemporaneous command/hash fields;
the table and conversation associate them with the sequence, but those files
alone do not independently prove exact source/authoring order. That limitation
is retained rather than filled with retrospective hashes. Tool-reported
wall_time_seconds is not used as timing evidence; the times here are unittest's
own printed values. Final follow-up review of the added guards/tests is pending.
