# GitHub testing methods selected for this clean-room diagnosis

2026-09-11 Asia/Shanghai. User explicitly requested finding GitHub testing SKILLs and applying their methods to continue investigating original S100 precision. This is new bounded engineering authority, not the daily report and not permission to use old quarantined code.

## Primary-source discovery and selection

The [skills.sh leaderboard](https://skills.sh/) was inspected first in Ego Lite. Then `npx --yes skills find 'property based testing'`, `'mutation testing'`, and `'cpp testing'` were run. No skill was installed; no downloaded test engine was executed. Counts below are observations, not quality or correctness guarantees.

| Skill / exact Git source | Discovery signal | Scope adopted here |
| --- | --- | --- |
| [Trail of Bits property-based-testing](https://github.com/trailofbits/skills/blob/321ccfe628eca0d314b0ee4eaffcdd8a05639aaf/plugins/property-based-testing/skills/property-based-testing/SKILL.md) | CLI5.5K installs; owner repository7033 stars, CC-BY-SA-4.0 | Independent oracle, non-vacuous algebraic properties, explicitly generated valid boundary cases, and separating wrong properties from real implementation defects. Main fully read SKILL plus generating/reviewing/interpreting-failures references. |
| [Trail of Bits mutation-testing](https://github.com/trailofbits/skills/blob/321ccfe628eca0d314b0ee4eaffcdd8a05639aaf/plugins/mutation-testing/skills/mutation-testing/SKILL.md) | CLI3.4K installs; same repository | Full SKILL inspected, not adopted as a campaign: it configures mewt/muton and does not list C++ among its stated examples. Only the general question of whether a deliberate known error is detected is relevant. No framework installed or campaign started. |
| [ECC cpp-testing](https://github.com/affaan-m/ECC/blob/c9148d0bb239ed01a95724a5928b98cdf9c30658/skills/cpp-testing/SKILL.md) | CLI9.2K installs; repository255906 stars, MIT | Focused CTest first, deterministic fixtures, real public interfaces, scoped runtime instrumentation as a possible later runner check. No GoogleTest migration, global flags, blanket sanitizer campaign or local build. Main read complete SKILL. |
| [Matt Pocock diagnosing-bugs](https://github.com/mattpocock/skills/blob/3cca18b368ae95cdbdebbff572ccafa662551015/skills/engineering/diagnosing-bugs/SKILL.md) | Leaderboard580.2K installs; repository258874 stars, MIT | Establish a red-capable loop, minimise one failure, compare falsifiable hypotheses one variable at a time. Main read exact GitHub source and available local skill. |

Owner repo metadata was read with `gh api repos/<owner>/<repo>` and fixed HEADs with `gh api repos/<owner>/<repo>/commits/main`; source files were read through GitHub contents API at those exact commits. These are first-party skill documents, not OpenFHE mathematical authorities. Installation syntax, if ever explicitly requested, is `npx skills add trailofbits/skills@property-based-testing`; installation is not needed for this methodological reference task.

## Adaptation to CKKS, not mechanical copying

1. Do not apply exact associativity or exact encrypt/decrypt equality to approximate CKKS. Define the permitted error, domain, scale, basis and randomness relation from the paper/source first.
2. Use public-input deterministic property vectors and exact integer or independent high-precision oracles before any random campaign. Do not mock away production arithmetic or derive expected output with the same transform implementation.
3. A roundtrip alone may hide paired encoder/decoder bugs. A passing mutation must be classified: real surviving fault, equivalent mutation, unsupported input/property, build failure or timeout. A survivor is not automatically the cause of historical S100FAIL.
4. Generic1000-example/nightly and100-repeat stress suggestions are explicitly superseded by the user's no1000-run instruction. No new testing library or unrelated security scanning is proposed. Mac stays free of builds, FHE, FFT/NTT and sampler experiments.
5. Prefer one bounded test at an existing public seam. User previously delegated ordinary technical decisions, so seam selection is documented by the team rather than asking another routine confirmation. A new materially different public capability would require separate scope justification.

## Actual feedback-loop checkpoint

Ran existing `python3 -B -I coordination/s100-fresh-error-repair-01/check_fresh_ideal_propagation.py` using bundled Python. Exit0, approximately0.324s. Checker SHA2569fbf1e6747c621400a6992ecdba0fee37724aebf5719466010b5fa92e23d734f; fixedlog SHA256cbd1d44bb8bdfbf7f9ff74bc94e950ce8a7602cd5e8bb43d98da6f403b2a874d. Same three exceeding components:512imag,769real,1023real. Largest lower bound2.08324198737765211086267e-24, ratio2.518485027046137 to2^-80; slot0 remains below threshold. No new data sampling or ciphertext execution.

This is a fast retained-data replay, **not yet a live production defect regression test**: exit0 verifies the historical conditional calculation; it does not turn originalS100FAIL green or prove exact hidden phase containment. It establishes a small replayable witness useful in selecting the next experiment, while live-seam construction remains open. We explicitly do not claim diagnosing-bugs Phase1 is fully satisfied for a newly identified production bug.

## Next bounded collaboration

Main coordinates a completely briefed browserPro test-design consultation and owns source integration/execution. Independent Sol context maps existing coverage before seeing Pro's verdict. Fable5.1 has no observed recovery since its documented balance failure; no repeated quota probe, no false Fable sign-off. ZCode quota was unavailable this morning, no dispatch. Existing scientific conditions and failure labels remain frozen.
