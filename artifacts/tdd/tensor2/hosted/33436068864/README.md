# DCP legacy-diagnostic hosted red evidence

Retrieved: 2026-09-01 04:47 Asia/Shanghai

This directory retains the raw GitHub Actions API response and both job logs
for the test-only commit that introduced the public DCP empty-key-tag
diagnostic regression assertion before the production fix.

- Repository: `leemaple/20231788.`
- Workflow run: `33436068864`, attempt 1
- Run URL: <https://github.com/leemaple/20231788./actions/runs/33436068864>
- Exact test-only commit: `9d1d10a3414dce68b84d9887337254c275098d79`
- Pristine OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Run result: `completed / cancelled`
- Linux job `99632689793`: `completed / failure`; strict build passed,
  five CTests passed, and `dcp_rcb` alone failed because production reported
  `DoubleCKKS: DCP input key tag does not match its ciphertext state` instead
  of the required legacy `pair state` diagnostic.
- Windows job `99632689495`: `completed / cancelled` after Linux had proved
  the intended red; no Windows build or CTest result is claimed for this run.

## Raw artifact identity

| File | Bytes | SHA-256 |
|---|---:|---|
| `run.json` | 11,916 | `a95452c531538488f5588e5868f96755ef88e8d1e74367ffd4d3938c715a7330` |
| `jobs.json` | 6,391 | `0f3d58b38a9339396644b63b0259c949a2593dc6b1c97e42738a7ac8f04ca9a5` |
| `linux-gcc.log` | 47,504 | `ba3fbbeaa33a073d110ab2c3c597c687f694138cb6510b09ba5fe226c2a4a55f` |
| `windows-mingw64.log` | 30,911 | `9a0dfb389ee9b8051b00c329e2253d83e600e91e563380f7b0c94b855218e06c` |

Gitleaks 8.30.1 scanned all four raw files (96,722 bytes) with `--no-git
--redact --verbose` and reported no leaks before they were staged.
