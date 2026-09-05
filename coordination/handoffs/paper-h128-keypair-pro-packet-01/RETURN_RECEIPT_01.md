# Actual h128 return received — 2026-09-05

Status: **archive/static preparation accepted; engineering integration and runtime NOT RUN**. No production/test/workflow bytes in the real worktree have been changed.

## Acquisition and identity

Saved Pro conversation **Implement Keypair Adapter Package**, https://chatgpt.com/c/6a9b5ebc-6aa0-83ec-ab39-5eaf91ca6da5, was visibly terminal (`Worked for 45m 5s`, Stop absent, 6 Pro). A stale Too many requests dialog was acknowledged once; no refresh, stop or repeated implementation prompt occurred. Actual ZIP download was requested at 11:41:09 CST and completed under Downloads. The initial rapid sidecar click did not produce a file; after checking its absence the sidecar alone was requested once more at 11:41:53 CST and then observed. The ZIP was not downloaded twice.

- Return ZIP: `paper-h128-keypair-four-patch-9d21c3a.zip`, **96,840 bytes**.
- SHA-256: `ccf2ecad2b6db7d0c6306dcedcd64b21e6e57aa677fa33e0eab83b964aa5df5a`.
- Real sidecar: **108 bytes**, SHA-256 `c260ef51025df263902f26652e61049b6d07ea04e482cb77abfebd03005e41b0`; exact archive basename/digest content independently verified.
- Exact input source: `9d21c3a5aea79c31745aca790712a9fd8c7743b2`; official pin `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Real worktree branch `codex/paper-h128-keypair-01`, pre-receipt HEAD `cfe2fbc072bbd2154443ef71a641eabe449f34f5`, initially clean; engineering still baseline `4ecbd972429884489918d9f82dfc3fe9f702ef4a`.

Durable ignored retention under this worktree: `artifacts/handoffs/h128-return-01/` contains the actual return ZIP/sidecar, original input ZIP/sidecar, and fresh `extracted/paper-h128-keypair-four-patch-9d21c3a/`. Both copied ZIP hashes were rechecked; gitignore confirmed. No paper/source bulk is added to public Git. Original Downloads files remain preserved.

## Checks actually executed

Root's independent in-memory gate verified exact outer identity, CRC, one-root safe relative regular unencrypted paths, no duplicate or NFC-casefold collision, 38 nonempty regular members, 282,974 expanded bytes, 36 closed manifest payloads and every size/hash/origin attribution. The two explicitly excluded root metadata files authenticate without a circular hash. Manifest SHA-256: `dd9dfec7854a8ebec28f4ff0af5fe6ba4625dbb641f5a5c4c67bc954611ca6dc`.

Gitleaks **8.30.1** scanned the actual ZIP (`gitleaks dir <ZIP> --max-archive-depth 1 --no-banner --redact`) and fresh extraction (`gitleaks dir <extracted> --no-banner --redact`): each 282,974 bytes, **zero findings**. This is a bounded secret scan, not a guarantee.

Root completely read both returned offline verifier scripts before invoking them with the retained exact input/return/sidecar. `ROOT_RETURN_REPLAY_01.json` retains the actual exit-0 output: all four ordered `git apply --check` and applications succeeded only in a fresh temporary tree; all six final files match complete/project byte-for-byte; other supplied project files and original 57 normalized CTest bindings/order are preserved; only the required 58th test is appended. Both GREEN stages preserve their respective RED test, and the frozen profile stays identical. This is static replay, not executed semantic RED/GREEN.

`ROOT_RETURN_ARITHMETIC_01.json` retains the actual exit-0 integer-only certificate verifier output: **39 prime certificates, 213 rejected composites, four minimum primitive roots**. This validates the supplied certificate arithmetic, not yet an independent review that those formulas/profile select the correct official runtime parameters. No OpenFHE or cryptographic experiment ran locally.

Independent agent `h128_next_gate_inventory` separately verified actual archive/sidecar, full manifest, original 96-payload input closure, all four copied input metadata files, and the 51 official-origin rows. Independently parsed baseline/final CMake and exact expected58 ledger; four patch scopes are A RED workflow/CMake/profile/test, A GREEN CMake/header/source, B RED new test only, B GREEN source only. Their union equals six complete files, with no evaluator path. No concrete blocker within this static audit. Agent did not run supplied scripts or replay patches. Both parties distinguish provenance consistency from independently checked Git object membership, which remains pending.

## Next gate — not yet performed

1. Verify supplied project/official objects against exact clean-room Git and pristine official pin; inspect full design, frozen profile, source/test and workflow diffs independently against the task. Reconcile concrete standards/spec findings; arithmetic certificate PASS alone is not a source-mapping verdict.
2. If preflight is sound, apply **only Cycle-A RED** to the real branch, commit/push, and retain exact-SHA Linux/Windows genuine missing-adapter failure after legacy/API success.
3. Only then integrate A GREEN and run focused/full CI. Subsequently repeat B RED then B GREEN; do not collapse the two cycles or change RED oracle/thresholds to follow implementation.
4. Final dual-host warning-clean/five-API/focus/full58 and external review remain required before integration acceptance. No default-branch merge now.

No N32768, 80-bit precision, eight-square, 1000-trial, production I/O, h-aware security, performance or full-project result is claimed by this bounded N256 key-consistency candidate. Repeated Mult2 final Pro review is already running independently; a Windows input-channel issue does not block this next engineering slice.
