# Clean-room baseline

Recorded: 2026-08-31 (Asia/Shanghai)

- Destination: `https://github.com/leemaple/20231788.` (the trailing period is part of the repository name)
- Branch: `cleanroom/reimplement-mult2-20260831`
- Starting state: empty Git repository; no inherited source, tests, build files, interfaces, or OpenFHE patches.
- Authoritative inputs: user-supplied paper 2023/1788 and official pristine OpenFHE 1.5.0 source/documentation.
- Quarantined and prohibited: destination repository's previous implementation/history as an engineering input; all existing local related code, builds, tests, specs, patches, OpenFHE modifications/installations, and prior diagnostic conclusions.
- Compute policy: Windows Z code/Zima or GitHub Actions for sustained builds/tests; Mac only for lightweight orchestration and bounded checks.
- First accepted slice: greenfield `t=2` DCP, RCB, Tensor2, Relin2, RS2, and Mult2 for one correct multiplication, plus a tested next-multiplication or explicit refresh boundary.

No implementation test exists yet. The first implementation evidence must be a newly authored independent-oracle test with retained red output.
