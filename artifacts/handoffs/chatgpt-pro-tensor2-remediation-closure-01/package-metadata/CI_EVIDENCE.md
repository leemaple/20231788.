# Tensor2 remediation closure — hosted evidence

Prepared: 2026-09-01 Asia/Shanghai

All execution records below are raw retained GitHub Actions evidence. They are
not local execution by the reviewer.

## P3 public-seam red

- Run: `33436068864`, attempt 1, event `push`, overall `completed / cancelled`.
- Exact test-only head: `9d1d10a3414dce68b84d9887337254c275098d79`.
- Linux job `99632689793`: `completed / failure`. The strict build succeeded;
  five CTests passed; `dcp_rcb` alone failed because production emitted
  `DoubleCKKS: DCP input key tag does not match its ciphertext state` while
  the new public-DCP regression required the legacy `pair state` diagnostic.
- Windows job `99632689495`: `completed / cancelled` after Linux proved the
  intended red. No Windows build or CTest result is claimed.

Raw artifact hashes:

```text
a95452c531538488f5588e5868f96755ef88e8d1e74367ffd4d3938c715a7330  tdd/p3-diagnostic-red/run.json
0f3d58b38a9339396644b63b0259c949a2593dc6b1c97e42738a7ac8f04ca9a5  tdd/p3-diagnostic-red/jobs.json
ba3fbbeaa33a073d110ab2c3c597c687f694138cb6510b09ba5fe226c2a4a55f  tdd/p3-diagnostic-red/linux-gcc.log
9a0dfb389ee9b8051b00c329e2253d83e600e91e563380f7b0c94b855218e06c  tdd/p3-diagnostic-red/windows-mingw64.log
```

## Exact-current final green

- Run: `33436252725`, event `push`, overall `completed / success`.
- Exact source/test head: `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Linux job `99633299988`: `completed / success`, strict build and 6/6 CTests.
- Windows job `99633300315`: `completed / success`, Windows Server 2022,
  MSYS2 MINGW64, CMake 4.4.2, GCC 16.2.0, pristine OpenFHE and project builds,
  then 6/6 CTests.
- Both jobs bind pristine OpenFHE 1.5.0 commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

Raw artifact hashes:

```text
d283cf83067bd969b2bcc23334c4a9274be989de789bfa2efc0550bfaaee8743  ci/run.json
4bd699605dc681c2d2d2693552c973dd6c349ec10a5bf0a904c85f2f004ac425  ci/jobs.json
26be856772c3c4a481aeab23d131858f006052b97400e50faf75cb2bead2d8cd  ci/linux-gcc.log
b3e0d6535ec5328d28a6bcefbe2b72aaa0ae872127622a39e7fe5d7aab471707  ci/windows-mingw64.log
```

The three earlier Tensor2 intermediate runs and all 12 raw-file hashes are in
`cleanroom-project/artifacts/tdd/tensor2/hosted/README.md`; their raw files are
part of the exact Git export.
