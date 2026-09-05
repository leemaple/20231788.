# h128 four-stage TDD: final dual-host GREEN — 2026-09-05

Observed: the frozen N=256 fixed-Q h=128 client-keypair diagnostic passes its
valid-path, 50 named rejection and client-owned lifecycle checks on both hosts.
This closes the four-stage runtime sequence, not the complete paper project
or all final external-review/integration gates.

- Tested source `1192200f558c69c0967e8306ed1a8bddf786ca34`.
- [Run 33945897881](https://github.com/leemaple/20231788./actions/runs/33945897881),
  automatic push, attempt 1, completed/success.
- Linux `101251800888`, complete 04:59:36 UTC; Windows MinGW64
  `101251800950`, complete 05:05:35 UTC.
- Pristine OpenFHE `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

| Actual check | Linux | Windows MinGW64 |
|---|---|---|
| Official dependency | exact-pin native64/backend4 cache | actual native64/backend4 build/install |
| Warning-clean project / five API builds | PASS / PASS | PASS / PASS |
| Existing first-Mult2 focus | 1/1, 0.29 s | 1/1, 0.24 s |
| Existing Pair focus | 2/2, 0.16 s | 2/2, 0.18 s |
| Legacy suite | 57/57, 1.28 s | 57/57, 2.55 s |
| h128 focus | 1/1, 0.04 s | 1/1, 0.12 s |
| Full suite | 58/58, 1.29 s | 58/58, 2.56 s |

All three completion markers appear once in each focused/full new invocation:
valid-path assertions; 50 named rejections (no injected tag collisions);
two-call uniqueness and owned-tag cache isolation. Independent reviewers
matched each host's actual legacy 57/full58 names, normalized commands and order
to the tested CMake. Root independently fetched the complete Linux API log and
matched it to the retained independent capture, and parsed all 119 Windows
starts/commands, all three markers and required successful job steps. Root's
initial API read yielded partial stdout while its shell handle was running;
those partial bytes were not saved as evidence. The final complete capture was
obtained by resuming its known handle, not by restarting CI.

## Ordered runtime evidence

| Stage | Tested source | Run | Observed dual-host result |
|---|---|---|---|
| A RED | a21216f0a8f854f478129d02fd32f496bd80f71c | 33943456483 | missing public header after old gates |
| A GREEN | 8aac5b7cf6530a9a2da14e8a4bdd5b65ab3c869f | 33944191280 | valid path focus/full58 PASS |
| B RED | 43c2dca45c2305c9b6baf50ae1c32529d35e7f06 | 33945243915 | intended unsupported-profile assertion |
| B GREEN | 1192200f558c69c0967e8306ed1a8bddf786ca34 | 33945897881 | focus/full58 and all markers PASS |

B RED evidence was committed/pushed first at
`4672b3b928623cbfa532a580377259f0d021bb5c`. Only original 0004 then changed
production source: 219 lines, 11,625 bytes, SHA-256
`679d4fa226b95770282fd5c877bf47044a290949bbaed501bd8f662744a6f22e`,
byte-identical to the original return's final source. B test remains
`c2698109d0a45621f6c705bcdfd2d0da3ae748db9802db1287de8517077fb81f`;
header, profile, CMake/workflow and all other engineering paths are unchanged.
No threshold/oracle was changed to obtain GREEN.

## Complete retained logs

- Linux raw/retained: 186,415 bytes, SHA-256
  `fd5ea99e4423fe53ebededaefe9753508b3608aede53b55626f3243a84810dcb`.
- Windows raw: 271,952 bytes, SHA-256
  `0b51f2d5ec69bbcbfd9ca5bafe1cd0c1849c4c3f2bf6c5d6cec287a07d2046d1`.
- Windows retained LF: 269,335 bytes, SHA-256
  `da965569bf771a14e6e5cd1988d5dfd8edd55de4a312de3c605cd9bf99a82f49`.

Adjacent full logs, per-host verification JSON and final run metadata preserve
the evidence. Ignored artifacts/handoffs/h128-cycle-b-green-01/
windows-api-capture.json losslessly retains original CRLF. Independent Windows
hashes match Root's. Preserve raw log whitespace; exclude only these two exact
log paths from docs whitespace checks.

This proves the frozen diagnostic's actual h=128 keys, official roundtrip and
ordinary EvalMult compatibility, 50 public rejection cases and owned-cache
lifecycle. It is not an 80-bit h128 experiment, forced-collision test, arbitrary
concurrency guarantee or all-hidden-table theorem. Callers supply finalized
contexts; secret coefficients/key material are not logged.

Next: final exact-source review reconciliation, production I/O, shared-family
and paper-parameter integration. No default merge or Mac build was performed.
N32768, eight no-refresh squarings, 1000 trials and applicable security and
performance evidence remain unclosed paper gates.

Pre-publication scan at 13:13 Asia/Shanghai: gitleaks 8.30.1, handoff evidence
2,104,726 bytes and separate ignored raw capture 279,619 bytes, both no leaks.
