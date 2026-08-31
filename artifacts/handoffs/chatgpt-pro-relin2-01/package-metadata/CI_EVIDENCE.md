# Exact-current Tensor2 base CI evidence for Relin2

Prepared: 2026-09-01 Asia/Shanghai

## Binding

- Project branch: `agent/codex-tensor2-01` / inherited Relin2 base
  `agent/codex-relin2-01`.
- Exact source/test head:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Exact Git tree: `759d5195739684748d5a9664edabe3fa719e1acf`.
- GitHub Actions run: `33436252725`.
- URL:
  `https://github.com/leemaple/20231788./actions/runs/33436252725`.
- Overall state: `completed / success`.
- Pristine OpenFHE 1.5.0:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

## Jobs

- Linux job `99633299988`: `ubuntu-24.04`, CMake 3.31.6, GCC 13.3.0,
  warning-clean project build, 6/6 CTests passed.
- Windows job `99633300315`: Windows Server 2022, MSYS2 MINGW64, CMake 4.4.2,
  GCC 16.2.0, pristine OpenFHE and project builds, 6/6 CTests passed.

## Raw files

```text
e1ee03a699a6c9b6dfb3025586a59985eb314307ec44997165119c2ab7b57b7e  ci/33436252725/README.md
4bd699605dc681c2d2d2693552c973dd6c349ec10a5bf0a904c85f2f004ac425  ci/33436252725/jobs.json
26be856772c3c4a481aeab23d131858f006052b97400e50faf75cb2bead2d8cd  ci/33436252725/linux-gcc.log
d283cf83067bd969b2bcc23334c4a9274be989de789bfa2efc0550bfaaee8743  ci/33436252725/run.json
b3e0d6535ec5328d28a6bcefbe2b72aaa0ae872127622a39e7fe5d7aab471707  ci/33436252725/windows-mingw64.log
```

These are retained provider records, not new local executions during Relin2
packaging. ChatGPT Pro's accepted Tensor2 closure separately reports a reviewer-
local Linux strict build and 6/6 CTest; it correctly makes no reviewer-local
Windows claim.

