# Relin2 remediation 07 downstream quarantine preflight

Recorded: 2026-09-01 20:44 Asia/Shanghai

Status: **environment gate ready; delivery still pending**. No remediation-07
ZIP has been received, inspected, decoded, replayed, executed, or applied.

## Repository state before preparation

- Coordination branch, local upstream, and remote were exact
  `09b2a6e2a7da2db45f84d99e3998bafce8e3da3a` with an empty worktree before
  this record.
- The implementation worktree, upstream, and remote were exact clean
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`, tree
  `759d5195739684748d5a9664edabe3fa719e1acf`.
- No Relin2 patch has been applied and no OpenFHE/project build or candidate
  script was run on the Mac.

## Actual downstream executables

The remediation-07 receipt requires actual Gitleaks 8.30.1 and actual CPython
3.9/3.10. The following identities were observed directly:

| Tool | Command path | Version | Resolved binary SHA-256 |
|---|---|---|---|
| Gitleaks | `/opt/homebrew/bin/gitleaks` | `8.30.1` | `f414bc2fb952be6c9072b75cb411e3368614ef4b16d48dbd9ad238034afd2302` |
| CPython 3.9 | `/Users/lifeng/.local/bin/python3.9` | `Python 3.9.25` | `4b84934f5f38de73d19e60a4a359ce7482b891de5b5d31f9a0b36b2aeec8c603` |
| CPython 3.10 | `/Users/lifeng/.local/bin/python3.10` | `Python 3.10.20` | `53815ef772c33a6e786b9702089e7f89bfbc5d43e1759daef3aafeb798a185bd` |

CPython 3.9 was initially absent. The already installed
`/Users/lifeng/.local/bin/uv`, version
`uv 0.11.16 (135a36367 2026-05-21 aarch64-apple-darwin)`, installed the
isolated 17.6 MiB CPython 3.9.25 distribution in 2.30 seconds. This did not
compile OpenFHE or consume the Windows runner.

## Pending fail-closed sequence

Only a natural ChatGPT Pro response beginning `ready for quarantine` and
followed by the exact mandatory STOP line may supply the expected ten-file ZIP.
After immutable response/ZIP capture, Codex must still perform, in order:

1. safe central-directory/path/mode/encryption checks and fresh extraction;
2. actual Gitleaks plus an independent targeted scan over that extraction;
3. non-executing manifest verification, exact-base seven-patch replay, tracked
   tree export, and strict Base64 decoding;
4. actual Gitleaks plus targeted scans over the exported tree and every decoded
   byte;
5. only then content review, exact 26,667-byte driver identity verification,
   `py_compile` under CPython 3.9 and 3.10, the complete immutable 68-mode run
   under CPython 3.9, and every shipped audit under both interpreters;
6. all remaining replay, mutation, build, registration, 37/37 CTest, evidence,
   and independent review gates before any application.

The presence of these executables proves only environment readiness. It is not
a Gitleaks, Python compatibility, replay, build, test, or Relin2 acceptance
result.

## External-state boundary

The saved Ego Lite task space is currently user-controlled/inactive from the
agent's perspective. The ego-browser workflow forbids automatic takeover or an
alternate-browser workaround. Codex has not retried, reclaimed, refreshed,
prodded, resent, or duplicated remediation 07. Receipt inspection resumes only
after the user explicitly says `continue`.
