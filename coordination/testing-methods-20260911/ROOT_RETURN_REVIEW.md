# Pro return intake and root review checkpoint

2026-09-11 Asia/Shanghai. Conversation
https://chatgpt.com/c/6aa358da-b52c-83ec-9356-f0d8d742a0d9 (`执行诊断测试`).
One submission at 09:26:48.871 CST; first observed terminal response at 10:07:08
CST. UI reported `Worked for 38m 22s`. No stop, refresh, duplicate or restart.
Observed selection was 6 / Pro, highest Power 5/5; backend unattested.

## Transport and original bytes

Clicked the final ZIP button once. Browser-level download routing was not
supported by Ego's CDP surface; page-level routing returned success but the
download appeared in the ordinary Downloads directory. No second download was
needed. The actual file is
`/Users/lifeng/Downloads/testing-methods-diagnosis-20260911.zip`, 42,319 bytes,
SHA-256 `60f08159f81178ced012244e1da7d1e76a21045c27a33994756c19e17f666903`,
matching the final response.

`intake_return.py` exited 0 in approximately 0.089 seconds. Verified exact
13-member allow-list, regular files, CRC, UTF-8, safe paths, complete 12-row
self-excluding manifest, all file sizes/hashes, source base, official pin and
handoff identity. Original members retained unchanged under `pro/`. See
`RETURN_INTAKE.json` for all identities. No returned program executed at intake.

Decoded targeted scan and Gitleaks directory scan passed. Root additionally
used the already read `build_packet.py` strict scanner: Gitleaks 8.30.1,
`stdin --ignore-gitleaks-allow --gitleaks-ignore-path /dev/null
--max-decode-depth 5 --max-archive-depth 1 --redact --no-banner --no-color
--report-format json --report-path -`, working directory `/private/tmp`, clean
environment containing only GOMAXPROCS=2, LANG=C, LC_ALL=C and system PATH.
Exit 0, findings `[]`; 90,983 framed bytes with SHA-256
`8e35bb278d73384d15e151f6aba9cae2c4a8b43c48fcc387794849b40fe3c21d`.
A scan is evidence, not a guarantee.

## Final scope differs from the intermediate candidate

The final author selected `initial-phase-n256-h128-v1`, reusing the existing
N256/h128 fixture and the actual key adapter. It does not implement the N64
zero-core interception described in the earlier visible progress. Therefore
`SAME_NOISE_REVIEW.md` remains a review of that earlier candidate and is **not**
sign-off on this returned code. The final test reads actual EV coordinates by
an independent direct modular inverse and uses schoolbook negacyclic phase
calculation. Its output mutations act on copies of the same actual ciphertext.

Root read the complete C++ source, CMake append, diagnosis, coverage, test plan,
mutation plan, execution ledger, source index and static checker. No new
production defect was established. The source pin remains
`df495ba2e91739a6dc8f1de254fc5a41155ce504`; production base remains `33722b9`.

## Actually replayed on the Mac

After full source inspection, ran the returned stdlib-only `verify_static.py`
against the exact original input ZIP, with output directed to a new root
artifact, not over the author's output. Exit 0 in approximately 0.895 seconds.
Root output is byte-identical to `pro/STATIC_CHECKS.json` (SHA-256
`a03580911eb4b3a02404a5e4aca272bedcc27c44ea4fc42a077f768e024b10cb`).

Checks: 485 manifest records, 147 directly specified Git blobs, original
fixture exact-source inclusion, fixed scalar root orders/CRT coprimality,
binary32 sigma value, G=39/F=15015, mutation offset=30031 and scalar no-wrap
inequalities, bounded AST/text checks. No transform, sampling or FHE executed.

`git apply --check pro/initial-phase.patch` exited 0 against the current
documentation-only descendant. `git apply --numstat` lists exactly 18 CMake
additions and 423 lines in one new C++ test. No patch was applied by those
read-only commands.

## Independent review and remaining gate

- `/root/initial_phase_math_review`: independent mathematical/semantic review,
  requested GPT-6 Astra/xhigh, backend unattested. First pass from task/source
  and test, without author's conclusions.
- `/root/testing_coverage_gap_map`: independent C++/CMake/adversarial review,
  requested GPT-5.6 Sol/high, backend unattested. Final N256 code reviewed fresh,
  without author's conclusions. Earlier N64 review is not reused as approval.
- Root: integration, standards, runtime provenance and execution. Pro is the
  author, not its own independent reviewer. Fable/ZCode unavailable fallback
  remains explicit; there is no additional provider-diversity claim.

Both final first-pass notes are now complete. Root read them in full:
`INITIAL_PHASE_MATH_REVIEW.md` and `INITIAL_PHASE_ENGINEERING_REVIEW.md`.
Both give conditional acceptance for this exact one-shot Linux diagnostic;
neither reports a concrete blocking mathematical or static C++ defect. The
conditions are enforced by the runner plan: preserved test bytes, fresh pinned
dependency, explicit excluded target, one `--controls` CTest, exact final result
classification and no memory-dump artifacts. The raw public scheme seam and
one realized sample, rather than the higher plaintext/encoder wrapper or all
noise distributions, remain the acceptance boundary. The printed `source_base`
is the configured checkout HEAD; the fixed production base is recorded
separately and production `src/` and `include/` must remain unchanged.

Root integrated the test byte-for-byte (comparison PASS) and only the reviewed
18-line opt-in CMake append. The new one-shot workflow is independently under
runner review in `INITIAL_PHASE_RUNNER_REVIEW.md`; no tag has yet been created
and no C++ build or remote run has started at this checkpoint.

The runner guard was developed test-first: the first stdlib-only run failed
with the expected missing `runner_guard.py` error; after the minimal guard was
added, 3 unittest methods passed (including invalid event, count, command,
classification and source-identity cases). Root replayed them again at 10:28
CST: 3/3 passed in 0.001 seconds of unittest-reported time. This is CI guard
RED/GREEN, not a production algorithm bug and fix. Ruby Psych safely parsed the
workflow (the YAML 1.1 `on` boolean-key behavior was explicitly accounted for),
confirmed one exact tag push trigger and one job, and all six Bash run blocks
passed `bash -n`. Actionlint and PyYAML were unavailable, so no claim of their
validation is made. `git diff --check` passed.

A normal baseline plus two rejected copied-output mutations would close only
this small contract slice, not original S100, N32768, noise distribution or
paper Table 3. It cannot exclude coherent within-bound errors, missing small
noise terms, or sampling-distribution changes.
