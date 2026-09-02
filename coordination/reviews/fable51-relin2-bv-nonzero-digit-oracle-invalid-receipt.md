# Fable 5.1 attempt receipt — BV nonzero-digit A-count oracle

Recorded: 2026-09-02 Asia/Shanghai

## Outcome

The terminal request completed naturally, but it is **not an accepted Fable
5.1 review**. The emitted model identity was `claude-fable-5`, rather than a
provider-advertised exact Fable 5.1 identity. The response also described files
and code that were absent from the sanitized source packet and contradicted the
packet's actual source. Its conclusions are excluded from design, code, test,
and acceptance evidence.

This is a nonblocking external-agent failure. The implementation continues from
official OpenFHE source, the exact project source commit, executable TDD, and
three independent read-only review gates. Do not retry the same alias
automatically. A later retry is permitted only when the terminal provider
advertises an exact Fable 5.1 identifier or the `fable` alias emits a verifiable
5.1 identity with fallback disabled.

## Request identity and handoff

- Decision task:
  `coordination/tasks/fable51-relin2-bv-nonzero-digit-oracle-01.md`
- Bound implementation source commit:
  `3411d65e752272a70d6dc147e8e7239014221196`
- Bound implementation tree:
  `572925d4819a32eefb258af1ed37b79deb551cc0`
- Official OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`
- Sanitized ZIP size: `50,740 bytes`
- Sanitized ZIP SHA-256:
  `774106f5b2f203aa2097d089399843fe46d3d6edb10ea9bffde565e16f86d12c`
- Included-file manifest SHA-256:
  `817075f2e1f2120dbdb613c4cd5d2d7477179be1dd4bce479c3adb63c423cc19`
- Gitleaks version: `8.30.1`
- Gitleaks result: exit `0`
- Targeted filename/content scan: `0` findings
- ZIP integrity check: passed
- Provider session: `a22b1c99-e4ff-46f8-b8d0-b9b12fb5a933`
- Terminal duration: `609,266 ms`
- Terminal exit: `0`
- Provider-reported model: `claude-fable-5`
- Provider-reported fallback-safe Fable 5.1 identity: absent

## Mechanical rejection evidence

The packet contained these source roots:

- `stage/current/`
- `stage/official-openfhe/`

The response instead claimed to inspect nonexistent roots and files including:

- `stage/openfhe-development/`
- `stage/double-ckks/`
- `relin2-validate.cpp`
- `relin2_validate_test.cpp`
- `throw_msg.h`

It also claimed the project guard throws `lbcrypto::config_error`. The bound
source actually centralizes project diagnostics in `Invalid(...)`, which throws
`std::invalid_argument` with the `DoubleCKKS:` prefix. This contradicts both the
task packet and the exact implementation source.

These are content-integrity failures independent of the model-name failure.
Even if the numerical count in the final prose happens to match the separately
derived result, the response is not usable evidence.

## Mainline disposition

The independently established oracle remains:

- ordered full-Q MSB manifest `{35, 31, 30, 31}`;
- BV digit size `10` decomposition count `15`;
- integer ceiling per complete-Q tower;
- a fixed manifest/count assertion plus a dynamically derived count;
- the existing exact project `std::invalid_argument` diagnostic;
- positive control before the isolated A-only malformed control.

The red test was subsequently committed and pushed at
`36d88dd731ec1e8c48cb90ca36e6ddc6f91c6486`; its hosted Linux/Windows result is
the executable acceptance evidence, not this rejected model response.
