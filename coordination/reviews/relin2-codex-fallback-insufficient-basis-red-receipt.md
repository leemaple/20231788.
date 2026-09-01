# Relin2 Codex fallback insufficient-basis-red coordination receipt

Recorded: 2026-09-02 Asia/Shanghai

Status: **public-API-reachable insufficient-active-basis red accepted; production
remains the Tensor-validation-only Relin2 scaffold**.

## Implementation boundary

- branch: `agent/codex-relin2-01`;
- commit: `8642a9450bd9315f1d228a537ccdce7b3d9614a5`;
- parent: `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`;
- tree: `f7cf880d6682f5ab194aefd2f5df59120e7e034b`;
- local, upstream, and remote branch SHA matched after the non-force push.

Only `CMakeLists.txt` and `tests/relin2_test.cpp` changed. The public fixture
starts with three towers at multiplicative depth two, proves RCB remains
supported, then reaches Tensor2 with two active towers and noise-scale degree
three. It requires exact `std::invalid_argument` text. Production `include/`
and `src/` bytes are unchanged. Spec, TDD, and Delivery/CI read-only reviews
each returned `PASS` before the red commit.

## Hosted observation

- workflow run: `33554777953`, attempt `1`, event `push`;
- URL: `https://github.com/leemaple/20231788./actions/runs/33554777953`;
- exact head SHA: `8642a9450bd9315f1d228a537ccdce7b3d9614a5`;
- Linux job `100012796036`: warning-clean build and compile-only Relin2 API
  contract succeeded; inherited tests passed `7/7`; only the new eighth test
  failed, giving aggregate `7/8` and CTest exit `8`;
- exact attribution: production threw
  `DoubleCKKS: Relin2 is not implemented`, proving the fixture reached the old
  seam and the requested guard did not exist;
- after Linux log capture, the non-final run was cancelled; Windows job
  `100012795842` stopped during pristine OpenFHE build near 9% and supplies no
  project-build or test claim.

## Remote evidence binding

- evidence branch: `evidence/relin2-hosted-8642a94`;
- evidence commit/local upstream/remote SHA:
  `d5cc21215fbc10c97d03342f7a67a716dbecc9dc`;
- evidence tree: `021cd20d9cd1f352c79e68ff5b79d9372f3d5e00`;
- 19-entry `MANIFEST.sha256` SHA-256:
  `26983d0707c7e9c5576044aa45e2a37f6b14455fa25ceb7d16b5c254ef7b6ca1`;
- evidence `RECEIPT.md` SHA-256:
  `e62dd8fcb2c83a7694b2a7cfbe73ba905d64e78f920057498c26989f02d3cea0`;
- terminal complete-logs ZIP SHA-256:
  `86910c580970791b364fae89a158f43db379c5d6337b6f83683903d37b105469`;
- exact Linux log SHA-256:
  `1253eebed135d3bdd541ab748ae2176530d809e7ad5e838b61962ccf312385d6`;
- the ZIP passed `unzip -t`; artifact count was zero; Gitleaks 8.30.1 and
  targeted scans reported no credential findings in retained or expanded data.

## Next authorized boundary

Production may add only the active-tower-count guard after complete Tensor
validation and before all later key or arithmetic work. Adequate-basis input
must continue to reach the exact not-implemented seam.
