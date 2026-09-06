# Independent full-slot scientific review return

Read `DECISION.md` first; `NEXT_STEP.md` contains the single selected engineering action. `FINDINGS.md` provides original path/line/page/slot citations. The Chinese explanation is in `DECISION.md` Section5. There is **no production patch**.

This package is self-contained for both endpoint replays, the receipt checks, scalar/unit tests, and inspection of the cited sources. `evidence/` contains 65 exact input copies, including both actual gzip/status pairs, complete retained logs, the supplied readers, relevant production/official sources and the paper. It is a review evidence subset, **not** a complete OpenFHE build distribution. The original input ZIP is needed only to repeat verification of that original archive's own byte identity/218-member closure; its verified results are retained here.

From the extracted return directory:

```sh
python3 -B -m unittest -v test_independent_endpoint_check
python3 -B independent_endpoint_check.py evidence linux
python3 -B independent_endpoint_check.py evidence windows
python3 -B run_supplied_full_replay.py evidence linux
python3 -B run_supplied_full_replay.py evidence windows
python3 -B evidence/project/coordination/fs-endpoint-live-run-01/verify_live_evidence.py linux
python3 -B evidence/project/coordination/fs-endpoint-live-run-01/verify_live_evidence.py windows
```

Python 3.12+ and its standard library suffice; this review used Python3.13.5. No C++, OpenFHE installation, transforms, credentials or network are needed. The first replay is independently authored integer-interval arithmetic; the second is a review-authored driver of the **supplied** Decimal256 arithmetic. They are intentionally labeled separately. Elapsed-time fields can differ on reproduction; scientific fields should match.

`results/` retains actual outputs and two initial reviewer-tool failure logs, subsequently corrected. Those failures are not production or CI failures. `EXECUTION_LEDGER.md` separates the work performed here from the historical hosted runs and unexecuted integration proposal.

The root `MANIFEST.json` excludes only itself and hashes every other return file; input-origin records identify unchanged supplied copies. Verify a ZIP using `python3 -B verify_return.py PATH_TO_RETURN.zip`. The original evidence manifest remains at `evidence/MANIFEST.json`; its 217-entry closure refers to the original input archive, not to this selected subset.
