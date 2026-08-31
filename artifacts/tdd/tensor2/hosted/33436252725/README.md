# Tensor2 exact-head final hosted CI evidence

Retrieved: 2026-09-01 04:36 Asia/Shanghai

This directory retains the raw GitHub Actions API response and both job logs
for the final Tensor2 diagnostic-compatibility fix. It is stored on the
coordination branch so the tested source branch remains frozen at the exact
source/test commit below.

- Repository: `leemaple/20231788.`
- Workflow run: `33436252725`
- Run URL: <https://github.com/leemaple/20231788./actions/runs/33436252725>
- Exact source/test commit: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`
- Pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Run result: `completed / success`
- Linux job `99633299988`: `completed / success`, 6/6 CTests
- Windows job `99633300315`: `completed / success`, 6/6 CTests

The Linux log records CMake 3.31.6 and the Windows log records MSYS2 MINGW64,
CMake 4.4.2, and GCC 16.2.0. Both logs bind the exact project and OpenFHE
commits above.

## Raw artifact identity

| File | Bytes | SHA-256 |
|---|---:|---|
| `run.json` | 11,904 | `d283cf83067bd969b2bcc23334c4a9274be989de789bfa2efc0550bfaaee8743` |
| `jobs.json` | 6,387 | `4bd699605dc681c2d2d2693552c973dd6c349ec10a5bf0a904c85f2f004ac425` |
| `linux-gcc.log` | 47,801 | `26be856772c3c4a481aeab23d131858f006052b97400e50faf75cb2bead2d8cd` |
| `windows-mingw64.log` | 126,032 | `b3e0d6535ec5328d28a6bcefbe2b72aaa0ae872127622a39e7fe5d7aab471707` |

Gitleaks 8.30.1 scanned all four raw files (192,124 bytes) with `--no-git
--redact --verbose` and reported no leaks before they were staged.
