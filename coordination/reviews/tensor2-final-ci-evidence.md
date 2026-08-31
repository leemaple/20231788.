# Tensor2 exact-head hosted CI evidence

Recorded: 2026-09-01 Asia/Shanghai

## Binding

- Branch: `agent/codex-tensor2-01`.
- Exact head: `55f3b43c47b5b2464625afcc6a1f244724336d5b`.
- Exact Git tree: `2269bee6bac5e7cd1124ab78c49a750af9a38942`.
- GitHub Actions run: `33428194982`.
- Run URL:
  `https://github.com/leemaple/20231788./actions/runs/33428194982`.
- Event/attempt: `push`, attempt `1`.
- Run state: `completed`; conclusion: `success`.
- OpenFHE version selected by the workflow: `v1.5.0`, exact upstream commit
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

The downloaded run JSON binds `head_sha` to the exact head above. The jobs
JSON lists exactly two completed successful jobs.

## Linux job

- Job: `linux-gcc`, ID `99606845597`.
- URL:
  `https://github.com/leemaple/20231788./actions/runs/33428194982/job/99606845597`.
- Runner/workflow environment: `ubuntu-24.04`.
- Recorded tools: CMake `3.31.6`, GCC `13.3.0`.
- The strict configure/build completed successfully.
- CTest passed all 6 tests, including the existing `dcp_rcb` test and all five
  independently registered Tensor2 cases.
- Total CTest time: `0.19 sec`.

## Windows job

- Job: `windows-mingw64`, ID `99606845245`.
- URL:
  `https://github.com/leemaple/20231788./actions/runs/33428194982/job/99606845245`.
- Runner/toolchain: Windows Server 2022 with MSYS2 `MINGW64`.
- Recorded tools: CMake `4.4.2`, GCC `16.2.0`.
- The job built pristine OpenFHE and then the project under the strict warning
  policy.
- CTest passed the same 6 tests.
- Total CTest time: `0.20 sec`.

## Raw attachment hashes

The exact-current ChatGPT Pro review package contains the complete downloaded
API records and job logs with these SHA-256 values:

```text
158bfc8b4882ad80e8fe5dae570e5ec81e20da5aad37f958e4c9dff4bbf77445  ci/run.json
7828285ad361c05f9c90ebedf09f088905d1a033b568b843461054fed7e81e02  ci/jobs.json
07d0cfcf795bc210131729647fce0c552d3f0366711f6bb4a5a7db855d5269b0  ci/linux-gcc.log
a7e10fcb961b06ab5a7a1d51037909ba20f25739d160f88d0bf429c2903f0773  ci/windows-mingw64.log
```

This is retained remote execution evidence. It is not a claim that the same
commands were rerun locally during review packaging.
