# Fixed binary512 to binary768 widening diagnosis

## Retained failure

Linux run `34007548849`, job `101417430759`, source
`8d7e6f072a399e572de5c4762d8399cbed0bc15b` built the unchanged old suite and
then failed while compiling `paper_endpoint_diagnostics.cpp`. The retained
excerpt is
`coordination/fs-endpoint-diagnostic-draft-01/COMPILE_FIX_LINUX_BUILD_RED.json`.
It reports GCC 13 `-Werror=array-bounds` in Boost 1.83 `copy_and_round`; the
observer self-test and paper live test were skipped. This is a compile failure,
not evidence about endpoint numerical behavior. Windows on the same source
compiled and ran the synthetic self-test successfully; that platform difference
does not make the Linux failure a false positive.

## Source path and cause

The clean-room input archive
`artifacts/handoffs/paper-scale-precision-adjudication-01/paper-scale-precision-adjudication-9f6c8eae.zip`
contains the official Boost 1.83 source at
`boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp`, SHA-256
`bcd782b34bd90d894c416fa595cd31c9e7114f343eb4dd1aad24bb722ab3dfc5`.
The same source is available upstream at
<https://github.com/boostorg/multiprecision/blob/boost-1.83.0/include/boost/multiprecision/cpp_bin_float.hpp>.
The instantiated shift is in
`boost-1.83.0/include/boost/multiprecision/cpp_int/bitwise.hpp`, SHA-256
`3e6b6fbf1720d1b51248297c6ed92df9dafd45ae224757cd775323cf7df4b11c`,
upstream at
<https://github.com/boostorg/multiprecision/blob/boost-1.83.0/include/boost/multiprecision/cpp_int/bitwise.hpp>.

`paper_full_test::Real` has a fixed 512-bit `Allocator=void` backend, while the
observer `Binary768` is allocator-backed. The cross-precision assignment at
Boost lines 251–276 copies the fixed source representation and instantiates
`copy_and_round`. Lines 617–694 contain both widening and rounding branches;
the latter instantiates a right shift on the fixed source representation through
`cpp_int/bitwise.hpp` lines 568–644. GCC's array analysis reports offsets beyond
that fixed object and `-Werror` correctly stops this build. The correction avoids
this cross-backend assignment template;
it does not suppress the warning or modify Boost.

## Exact bridge

`internal::WidenRepresented512` uses public `frexp` and `ldexp` operations in
the original fixed binary512 type to expose all 512 represented significand
bits as an integer. It checks the integer conversion by reconstructing the
scaled value in the same source type. A decimal string is used only for that
exact integer, which fits losslessly in binary768; a final power-of-two shift
restores the original exponent. The destination integer conversion and inverse
power-of-two shift are checked as well. Ordinary decimal formatting of the unscaled
binary value is deliberately not used because round-trip digits at precision
512 do not identify the exact dyadic when parsed at precision 768.

The original `paper_full_test::R(Int)` decimal-string calculation remains first;
only its already represented result crosses the bridge. Horner anchors reuse the
same bridge. Nonfinite bridge inputs are `NONFINITE`; finite values outside the
checked binary exponent envelope are `MODEL_UNSUPPORTED`.
