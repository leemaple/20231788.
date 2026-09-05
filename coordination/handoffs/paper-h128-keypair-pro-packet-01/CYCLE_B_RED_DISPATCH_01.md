# h128 Cycle-B RED hosted run — 2026-09-05

Observed automatic push run (no workflow dispatch or rerun):

- Source `43c2dca45c2305c9b6baf50ae1c32529d35e7f06`.
- Branch `codex/paper-h128-keypair-01`.
- [Run 33945243915](https://github.com/leemaple/20231788./actions/runs/33945243915),
  attempt 1, push, created 2026-09-05T04:41:35Z.
- Linux job `101250030746`; Windows MinGW64 job `101250030632`.
- Both in progress at the retained `CYCLE_B_RED_RUN_01.json` snapshot. Linux
  restored the exact-pin native64/backend4 install cache, so its dependency
  configure/build steps were skipped; this is not a fresh dependency build.
  Windows was installing its official toolchain.

No B runtime result yet. Apply the exact acceptance rule from
`CYCLE_B_RED_PREFLIGHT_01.md`: successful legacy/build gates followed by the
specific runtime rejection assertion, not a generic failure. Preserve terminal
logs and provenance before applying original 0004. Frozen tests/thresholds and
all other engineering files stay unchanged in that GREEN. No default merge.

The active continuation remains this concrete run. Do not repeat already
accepted A RED/GREEN or restart normal CI. The complete paper objective remains
open, including production I/O, family setup and paper-scale experiments.
