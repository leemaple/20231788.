# FS-RESIDUAL-ENDPOINT-01 GREEN packet selection plan

Status: builder prepared but **NOT RUN**. No ZIP has been created. Root must read the complete builder and provide the final task brief before execution.

## Interface

Planned task path:

`artifacts/handoffs/fs-residual-endpoint-green-01/TASK.md`

Future command, not executed during preparation:

`python3 artifacts/handoffs/fs-residual-endpoint-green-01/build_packet.py --task artifacts/handoffs/fs-residual-endpoint-green-01/TASK.md`

The task path is explicit and required. The output defaults to the same ignored handoff directory, is created exclusively, and cannot overwrite an existing packet.

## Planned archive selection

| Class | Payloads | Archive prefix |
|---|---:|---|
| Root-authored task | 1 | `TASK.md` |
| Current project sources/configuration | 39 | `project/` |
| Frozen requirements and seams | 5 | `requirements/` |
| Adopted endpoint RED return context | 9 | `context/endpoint-red-return/` |
| Current endpoint RED run evidence | 13 | `evidence/current-endpoint-red-run/` |
| Scientific disposition and endpoint proposals | 9 | `context/scientific-disposition/` |
| Historical original-E80 evidence | 15 | `evidence/historical-original-e80/` |
| Official OpenFHE source at the pinned commit | 77 | `references/official-full/` |
| Boost 1.83.0 references | 4 | `references/boost-1.83.0/` |
| Original paper PDF and text | 2 | `references/paper/` |

Expected total: 174 payloads plus one self-excluded `MANIFEST.json`, or 175 regular ZIP members.

The builder admits one unambiguous `project/` tree and does not copy the obsolete project tree or nested packet from the historical archive. It verifies the historical archive's manifest closure before selecting only approved reference/evidence bytes.

## Fixed identities and gates

- Documentation HEAD: `29e12670150f083be396686f0f4b92136758956f`
- Tested endpoint RED source: `2fe655d493dcde5f05aa1515f41ca6823bba30bd`
- Production source: `b1b024e3134fbb4e8cac7c0d59cf790a37e4ed89`
- Official OpenFHE pin: `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Prior verified clean-room ZIP: 2,046,500 bytes, SHA-256 `1584a5b7362c9568d3f8f7fa8acfea9f28a4a934cabe2dcdd283aa6f02e9b7da`
- Builder: 26,306 bytes, SHA-256 `60a336c87719e863d5a1dfa83027bbd0b47ebc96d05a13046d7ebde701f885f3`

The future run fails fast on branch/HEAD/cleanliness, Git blob equality, source and reference identities, path safety and uniqueness, ZIP CRC and manifest closure, expected member counts, byte/hash mismatches, credential-like filenames, and strict Gitleaks 8.30.1 scans of both the selected bytes and decoded final archive. Requested/configured Sol/high is recorded only as a selector; its actual backend identity remains unattested.

## Remaining input

The only missing input is the root-authored final `TASK.md` supplied through `--task`. No other missing source, specification, reference, or evidence input was observed. Gitleaks availability and exact version are runtime preconditions that the builder will verify only when root authorizes execution.
