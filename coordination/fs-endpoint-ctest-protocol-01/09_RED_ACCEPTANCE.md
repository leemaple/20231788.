# Accepted actual CTest protocol RED and retained artifacts

Source `0e3b82bdf71a49f21b497f4ad27a69792e18159a`, run [34018316144](https://github.com/leemaple/20231788./actions/runs/34018316144), attempt 1. Linux job 101446076946 and Windows job 101446076882 both reached the intended RED: wrapper exit 8 and final TIMEOUT assertion failure, with inspector, selector, allowlist validation, upload and download successful. Neither terminal run was restarted or rerun.

Both selected precisely Test #61 once; the sentinel and exclusive marker were validated. Actual CTest killed the non-crypto emitter after 2 seconds and recorded the same timeout result line and final failure summary on both platforms. Both primary logs are LF-only: Linux 1515 bytes/28 LF/0 CRLF, Windows 1240 bytes/26 LF/0 CRLF. Full bounded byte inspections are retained under red-artifacts and checked reconstructed fixtures under tests/fixtures. Windows native Python/path setup succeeded; a speculative MSYS .exe-path concern was not observed and is not an outstanding blocker.

The downloaded exact one-file artifacts were independently fetched by immutable artifact ID, checked against the service ZIP digest/size, and inspected in memory with an exact single-entry allowlist. No general archive extraction occurred. Raw status files are retained under red-artifacts, not republished as live evidence:

| Host | Artifact ID | ZIP bytes | Status bytes | Status SHA-256 |
| --- | ---: | ---: | ---: | --- |
| Linux | 9984627144 | 1355 | 1037 | bd0ddeef607a5a59daf7c877f8c85b9957c1be57164c79f44ede87a61efdb7bf |
| Windows | 9984644193 | 1363 | 1041 | f6248b1e2707a56caf186dfce2acdbd2a8fb97b4f6856c33e5c2c98394153a93 |

Both strict status objects retain ctest_exit_code=8, FATAL/CTEST_FATAL, observer NOT_OBSERVED, null numerical facts and payload facts, row_count=0, packer FAIL. ZIP size is not status size. The workflow's final verifier reached the reason assertion only after exact download file/name/byte equality. Root retained and hashed the status payloads independently as well; see 08_red_artifact_receipt.json for actual commands/results.

Independent reviewer /root/endpoint_canonical_writer accepted the exact evidence identity, LF framing, real shell/native path handoff and cause attribution after reading source/logs. It did not run anything or claim a new provider's review. Root's small public-CLI TDD fix is separately recorded in ../fs-endpoint-timeout-green-01; it is not yet hosted GREEN as of this acceptance note.

No C++ build, OpenFHE/crypto, full-slot replay or paper chain ran in this protocol gate. The frozen paper E80 FAIL and A_NOT_ADOPTED remain unchanged. The real 1200-second live timeout and complete gzip transport are not proved by this status-only 2-second probe.
