# Independent seam audit, 2026-09-07

Requested agent /root/s100_fresh_audit, selector gpt-6-astra/high. Runtime identity requested-unverified; this is independent context, not verified provider diversity. Read-only; no tests or edits.

The public client currently has no deterministic encoding method. Encrypt owns the exact signed coefficient vector locally before official public encryption. Extract a shared computation, returning owned coefficients through one bounded inspection method; do not create a second encoder or use zero-secret/key encryption.

Existing independent consumers are SparseDecrypt in paper_full_eight_square_oracle.h, Observe(IntegerPolynomial,Scale) in paper_endpoint_observer_contract.h and Horner anchors. For the same m, p, z and independent O, separate encoding O(m)-z, aggregate public-encryption O(p-m), and production decoder residual Decrypt-O(p). Verify no wrap and componentwise reconstruction. Preserve extrema locations, signed component values, independent precision/anchor checks and frozen input.

Conditional nearest-integer coefficient-rounding bound: N/(2 Delta)=2^-86 at N32768/S100. It is about 25 times below retained fresh E0, conditional on transform/rounding correctness. Precision agreement alone is not proof. No concrete production bug established.

Pinned official CKKS decryption adds randomness only in noise-flooding mode, not frozen FIXED_NOISE_DECRYPT. Public encryption draws v,e0,e1, with aggregate (pk0+pk1*s)*v+e0+e1*s. Zero-key/secret controls do not preserve this experiment. The proposed diagnostic separates aggregate encryption error, not each individual sampler term.

Root incorporated this read-only challenge in the single initial Pro message. Remote evidence is still required; original S100 FAIL and qualified S116 PASS are unchanged.
