# Tensor2 intermediate hosted TDD evidence

Captured: 2026-09-01 Asia/Shanghai

These files are raw retained GitHub Actions evidence downloaded from the public
repository after the exact-current ChatGPT Pro closure review identified that
the earlier handoff contained summaries but not the provider records. No
workflow was dispatched, rerun, cancelled, or modified while collecting them.

Acquisition used the GitHub REST endpoints for each Actions run, its jobs, and
each individual job log. Gitleaks 8.30.1 scanned the complete 637,139-byte
directory after download and reported no leaks.

## Boundaries

| TDD boundary | Run | Exact head | Linux | Windows |
|---|---:|---|---|---|
| API compile red | `33425868973` | `f3db12ef9fb0d13df0f779157eed168b8d582ea4` | job `99599155126`, intended compile failure | job `99599155483`, cancelled |
| Complete runtime red | `33426712752` | `482d27d0c43c22779aa548e00955ed90175dee97` | job `99601946465`, 1/6 passed and five intended Tensor2 failures | job `99601946625`, cancelled |
| First implementation green | `33427271692` | `1408d46217e97a1c14d43d49b64791da22f652da` | job `99603779665`, 6/6 passed | job `99603779368`, cancelled |

All three runs are attempt 1, event `push`, branch
`agent/codex-tensor2-01`, and bind pristine OpenFHE 1.5.0 commit
`df495ba2e91739a6dc8f1de254fc5a41155ce504`. The overall run conclusion is
`cancelled` because each intentionally unneeded Windows job was cancelled
after the Linux boundary became observable.

The API-red Linux log shows the existing production target build before the
compile-only public-contract target fails on the missing
`TensorCiphertextPair`, `TensorScaleDescriptor`, and `DoubleCKKS::Tensor2`.
The runtime-red Linux log records all five Tensor2 cases independently failing
on the immediate `DoubleCKKS: Tensor2 is not implemented` scaffold while the
existing DCP/RCB test passes. The first-green Linux log records 6/6 CTest
entries passing.

## File hashes

```text
383a723b036d4c42eba114ae2ecd9a68629521bcc89abd87952eff0fb3624408  33425868973/jobs.json
16075c494e1ab55f92b2b5039e41317edbee41efeba982cf73cde5d2f1c521f0  33425868973/linux-gcc.log
d68678cf9a0ace72d8fc72aefd948f2d2d0650e8b0f644302c2a127b34aca016  33425868973/run.json
97b021edbdf0a79d1ebf8c5c66d38bf0463ac183a8102980505fec680904e125  33425868973/windows-mingw64.log
ad3838350f4c0edaeb9ab36acbe249269bbd726a07deca27d5094f147b451096  33426712752/jobs.json
2aab913ab00ca5ff696cdd401bdd74737f32646dfbc7ccde2c6f1f337eee2eaa  33426712752/linux-gcc.log
0ddffa00b2202a7abe99fb1ede5ae763cb9b89cb5b57b57e816dd9c7367b3934  33426712752/run.json
8a217c3b5f6bc5523455eb79b8bb902c6de91c0a6aa5c1f8c08003477802afca  33426712752/windows-mingw64.log
da09b2d882b9010d32a9325c97e14f6a58273dea1365c661d391299ed60fa14f  33427271692/jobs.json
30a4472946941fcc342dda2242aee29397f47a6a5fd5d5a138a02dcf1d1b3c29  33427271692/linux-gcc.log
5ff25644275ddded22eb6153e5918d19885fb8bb866717cafb143c2baa133042  33427271692/run.json
aab5594a9d6bc72715d6ce566f2ce88fbd19e4d3683b5df14d5a9232a778ff54  33427271692/windows-mingw64.log
```

These are retained remote records, not local execution. The final exact-head
Linux/Windows evidence remains separately bound to Actions run `33428194982`.
