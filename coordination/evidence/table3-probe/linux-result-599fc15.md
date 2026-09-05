# Linux first observed profile-discovery result

Observed 2026-09-05 09:22–09:24 Asia/Shanghai. Run
https://github.com/leemaple/20231788./actions/runs/33935796427,
attempt 1, Linux job `101223371509`, exact in-band source
`599fc158b6b67d3e752c39cb53069c29dc60fc6f`.
GitHub marks this job completed/success. Windows job `101223371775` was
still building pristine OpenFHE at the paired snapshot; dual-platform
acceptance remains **pending**.

## Actually observed and independently checked

- Ubuntu GCC 13.3.0, CMake 3.31.6; exact pristine OpenFHE pin verified.
- Default warning-clean project build and Relin2/RS2/Mult2/Add/Sub API targets
  completed. The explicit parameter-probe target also compiled warning-clean.
- Focused first-Mult2 regression: 1/1, 0.22 s.
- Focused Pair Add/Sub composition regressions: 2/2, 0.20 s.
- Full existing CTest suite: 57/57, 1.32 s; no new CTest was added.
- Probe reports NATIVEINT=64, MATHBACKEND=4 and all 18 A/B families passed.
- Root parsed **57 actual ordered Q/P/QP basis records** (19 cases x 3)
  from the retained job log, independently generated each expected family
  from the pre-recorded candidate JSON, and verified size, modulus/root
  sequence and dnum=L/alpha=1/partition-count=L for all 18 ordinary families
  plus the separate collision control. All records match exactly.
- All 12 explicit official RootOfUnity records match the pre-recorded roots.
  The collision control's P is `1152921504598720513`, root
  `100545759574150`, as expected when reserved P replaces Mult0 in full Q.
- Final marker: `PASS families=18 collision-control=exact no-keygen no-ciphertext`.
  No `FAIL field=` marker exists in this job log.

Retained connector-decoded job log and identity/validation wrapper:
`linux-job-101223371509.json`, 141,936 bytes, SHA-256
`c610caed3d36df1afff7b549a67b3e7b4144323fd3af1d24b009a406e7143a29`.
The JSON wrapper is not a raw HTTP download; its content field preserves the
decoded connector log. Gitleaks 8.30.1 scanned the evidence directory
(approximately 147,524 bytes), zero findings. No Mac build, NTT or crypto
execution was performed to obtain this evidence.

## Boundary and next action

This is **first-observed passing upstream parameter discovery**, not a new
production RED/GREEN or proof of the full paper. It supports this direct
public-constructor parameter/getter path on Linux only. No h128 key generation,
security estimate, client I/O, repeated Mult2, eight-squaring precision or
performance experiment was run by this probe. It did perform the official
CRT/NTT table precomputation. Keep all three current Pro implementation tasks
unchanged, wait for the existing Windows job without re-dispatch, and audit
the same source/expected records before any subsequent adoption decision.
