# h128 Cycle-A GREEN accepted — 2026-09-05

Observed: the frozen N=256 valid-keypair diagnostic passes on both platforms.
This acceptance precedes applying Cycle-B RED; it does not close the full h128
slice, let alone the complete paper implementation.

- Exact source: `8aac5b7cf6530a9a2da14e8a4bdd5b65ab3c869f`.
- [Run 33944191280](https://github.com/leemaple/20231788./actions/runs/33944191280),
  automatic push, attempt 1, completed/success.
- Linux job `101247184972`; Windows MinGW64 job `101247184873`.
- Exact official OpenFHE `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

| Actual check | Linux | Windows MinGW64 |
|---|---|---|
| Provenance, native64/backend4, actual dependency build/install | PASS | PASS |
| Debug warning-clean project build / five explicit API builds | PASS / PASS | PASS / PASS |
| Prior first-Mult2 focus | 1/1, 0.32 s | 1/1, 0.22 s |
| Prior Pair Add/Sub focus | 2/2, 0.22 s | 2/2, 0.20 s |
| Legacy regression | 57/57, 1.26 s | 57/57, 2.97 s |
| New h128 focused diagnostic | 1/1, 0.01 s | 1/1, 0.14 s |
| Full suite | 58/58, 1.32 s | 58/58, 2.91 s |

## What the successful diagnostic actually checks

The exact frozen A test checks real returned private/public key objects, full
literal Q/moduli/roots/format/context/tag, a coefficient-format secret copy with
exactly 128 signed ternary nonzeros and the same support across all towers,
63–65 positive coefficients, official public-key Encrypt/private-key Decrypt,
and relinearized ordinary EvalMult using the returned secret's evaluation key.
It compares all 128 decoded slots to independent dyadic input/square literals
within the unchanged 1e-5 key-consistency smoke tolerance, and verifies caller
key/context/table immutability and owned cache cleanup. Secret coefficients
are not logged.

On each host, `valid-path assertions passed (diagnostic, not paper evidence)`
appears exactly twice: after the focused invocation and after test #58 in the
full suite. Root independently verified all actual legacy 57, focused #58 and
full 58 names, indices, normalized commands and order against the tested Git
CMake blob, required successful job steps, provenance and both markers. This
is actual h128 runtime evidence, not merely a green badge.

Independent reviewers `h128_candidate_spec` (Linux) and
`h128_profile_provenance` (Windows) separately verified full API logs/metadata,
legacy/full bindings and scope. Their raw log sizes and hashes match Root's.
Both parser audits completed successfully. Their checks did not run local
cryptography or repeat CI.

## Evidence and limits

Complete retained logs, final run metadata and per-host binding verification
are adjacent `CYCLE_A_GREEN_*` files. Exact raw/normalized byte counts and
SHA-256 are in `CYCLE_A_GREEN_LOG_IDENTITIES_01.json`; ignored JSON captures
losslessly retain original text including Windows CRLF. An initial low output
budget truncated the first API capture; it was rejected without creating any
evidence file. Bounded complete recapture, identity and normalization checks
then succeeded. Original runner whitespace is preserved; diff checks exclude
only the two specifically named log files, not code or other documents.

The RED test/profile/workflow remained byte-unchanged across this A GREEN;
the production patch is exactly original 0002, with only the pre-reviewed
build ordering overlay inherited from A RED. No Cycle-B code is yet applied.
Full unsupported-profile rejection and tag/cache lifecycle coverage remain
the next RED/GREEN cycle. This does not establish high-precision client I/O,
paper N=32768, shared-secret family projection, eight no-refresh squarings,
1000 trials, security or performance.

Next: commit/push this evidence, then apply only original 0003. Its first
noiseScale=2 counterexample must fail because A accepts an unsupported profile,
after the existing valid path passes. Do not change A literals/tolerances,
import B GREEN early, rerun this accepted CI or merge the default branch.

Pre-publication scan at 2026-09-05 12:39 Asia/Shanghai: gitleaks 8.30.1,
`gitleaks dir coordination/handoffs/paper-h128-keypair-pro-packet-01 --redact --no-banner`
scanned 1,116,645 bytes with no leaks; the separate ignored raw-capture directory
scan covered 546,314 bytes with no leaks. Raw captures remain local and ignored.
