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
