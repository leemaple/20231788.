# Confirmed public test seams

Recorded: 2026-08-31

The user-provided objective explicitly names DCP, RCB, Tensor2, Relin2, RS2, Mult2, and the necessary pair addition/subtraction as required behaviors. Those named operations are therefore the agreed public seams for TDD.

Tests must exercise only the module interface for these operations:

- `DCP`: ordinary OpenFHE ciphertext to a two-ciphertext pair.
- `RCB`: pair to an ordinary OpenFHE ciphertext.
- pair `Add` and `Sub`: componentwise operations with strict compatibility checks.
- `Tensor2`: two two-component pairs to a pair whose ciphertexts each contain three RLWE components.
- `Relin2`: Tensor2 output to a two-component ciphertext pair.
- `RS2`: relinearized pair to the next supported modulus level.
- `Mult2`: the composed first-multiplication behavior and the explicit next-multiplication boundary.

The module interface includes the invariants callers must know: pristine OpenFHE 1.5.0 context identity, divisor role, tower order, component counts, coefficient/evaluation format, level, scale, key tag, required evaluation key, supported initial level, and the fail-fast refresh boundary.

Private DCRT tower-copying, signed-remainder construction, tensor loops, metadata helpers, and key-switch plumbing are implementation details, not test seams. Tests may use independent plaintext/big-integer/CRT adapters only to compute expected results; they must not call implementation helpers or reproduce their algorithm.

Each vertical slice must retain its first failing output before the smallest implementation change is written.

## Confirmed repeated/client seam — 2026-09-05

The user answered that the reviewers may confirm the proposed interface as
reasonable and should continue. Codex accepts it: it preserves the paper's
evaluation model, keeps secret material client-side, hides basis-family routing
from the caller, and exposes behavior rather than implementation helpers. This
confirms the following end-to-end seam without requiring Pro's suggested token:

- client-owned setup, high-precision input encryption and final decryption;
- evaluator-only repeated use of the same public `Mult2` interface, first for a
  genuine two-operation tracer and ultimately all eight paper squarings;
- no evaluator secret-key access, intermediate decryption, re-encryption,
  bootstrapping, or Section 6.2 refresh substitution;
- implementation-owned immutable context/key-family, basis, level and exact
  scale transitions, with observable state validation at the public seam.

The concrete C++ types may be kept minimal by the implementation and reviewed
for KISS/YAGNI. Tests observe only this confirmed behavior. The second-operation
test is the first new RED slice; eight operations, high-precision client I/O and
paper parameters follow as vertical slices rather than being claimed by it.

## Confirmed fixed-Q h=128 client setup seam — 2026-09-05

The user delegated routine technical decisions to the reviewers and asked that
work continue without another confirmation round. After the pinned-source and
algebra reviews recorded in `PAPER_H128_SETUP_CANDIDATE_B_DECISION.md`, Codex
accepts one narrow client-owned public seam:

- `CreateFixedQH128ClientKeyPair(context)` accepts an already finalized CKKS
  context and returns a fresh matching OpenFHE private/public key pair whose
  secret has exactly 128 signed ternary coefficients;
- the adapter composes the official h-aware DCRT sampler, fresh key objects and
  public scheme `EncryptZeroCore` primitive; it does not implement RLWE itself,
  build or mutate contexts, use the debugging-only multiparty helper, or retain
  a secret on the evaluator side;
- the first vertical slice is an N=256 fixed-Q diagnostic. Tests observe the
  public adapter, returned public key getters and official Encrypt/Decrypt plus
  ordinary EvalMult compatibility. They also cover the frozen profile's
  fail-fast rejection cases and task-owned tag isolation;
- paper N=32768, shared-secret projection across basis families, specialized
  repeated Mult2, h-aware security analysis and 1000-trial evidence remain
  later gates. Passing the diagnostic must not be described as any of those.

The implementation may choose the smallest declaration shape consistent with
this behavior. Secret coefficients may be inspected only inside the independent
test oracle and must never be printed or committed as evidence. The exact
diagnostic parameters, literals, tolerance and expected missing-API failure are
frozen before the RED commit and may not change in GREEN.
