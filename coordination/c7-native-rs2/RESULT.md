# Native functional result, 2026-10-08

Implementation commit: 93f128b49e6eb03c5e1d541763b49f8f269ac378. Branch: experiment/c7-native-rs2-20261008. Base: 33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1. No integration or default promotion.

The first engineering CI [37800381982](https://github.com/leemaple/20231788./actions/runs/37800381982) built successfully, witnessed the exact preimplementation failures, and passed all seven Reference cases. Reordered then failed: SetLevel checked two remaining towers against the old noise degree of three. This run is retained as a failure; Fused was not reached.

The only repair moves SetNoiseScaleDeg before SetLevel, matching pinned OpenFHE ModReduceInternalInPlace and CiphertextImpl::SetLevel. Tests, arithmetic and workflow were unchanged.

Repair CI [37802782406](https://github.com/leemaple/20231788./actions/runs/37802782406), attempt1, passed all21 green checks (three backends, seven named cases) and all3 expected red outcomes at commit0fc6449cc7ee7aaa10641a437f57cf40ace561da. The original RS2 public-signature executable also built and ran.

Artifact11560839145 SHA256 8769f9f37974cb8c82fe528edcd63e2a9e1e3edaf35f7ad9451268fc21a9f2f2; downloaded archive matched the API digest, and every green raw output/return code was checked. Green binary SHA256 ef6130f2b7ada2cf3d0d121d67c770d981076c88bc73ba3ae7abdfa0e332b294. First artifact11560487413 SHA256 7754d80fbc21507ed968c3ab889434a23467161f26a8d07051686c31184689cb.

OpenFHE1.5.0 pin df495ba2e91739a6dc8f1de254fc5a41155ce504; native64/backend4; Ubuntu24.04 x86_64, GCC13.3.0, OMP1. N32 functional fixtures cover full-integer CRT/rescale, metadata/alias/immutability and rejection contracts. The wide controlled test uses 58-bit HYBRID; genuine HYBRID/BV public pipelines use30-bit moduli.

This is engineering evidence only. No native timing comparison, security-level qualification, S100 precision repair, wide-BV/repeated-path result, full Mult2 speedup, novelty or paper-readiness claim follows. Continue only with a separately admitted controlled cost comparison against the ordinary reordered baseline.
