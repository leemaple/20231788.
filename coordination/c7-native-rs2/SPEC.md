# C7 native RS2 engineering contract

Base: 33722b9c9ba9d32b36e672ee7cdaa3c3fb2a95d1, accepted clean-room source.
Official OpenFHE 1.5.0: df495ba2e91739a6dc8f1de254fc5a41155ce504; native64/backend4.
User asks to improve this implementation toward an original paper. This engineering experiment does not establish novelty, speed, security parameters or publication readiness.

1. Preserve original RS2 public signature and default Reference behavior; add explicit opt-in Reordered and Fused backends. No changed modulus chains, rounding, scales or key semantics.
2. Reordered baseline materializes d*h+l, performs two ordinary in-place rescalings using (x-NTT(ctr_q(x_q)))*q_inverse, then subtracts d*H per tower without a full extra scaled-H ciphertext.
3. Fused uses two independent centered dropped-tower residues r_h=ctr_q(h_q), r_c=ctr_q(d*h_q+l_q), and computes H=(h-r_h)/q, L=(l+d*r_h-r_c)/q in each surviving tower. Center only in coefficient format. Use native modular arithmetic rather than signed-word products.
4. Validate input before mutation, preserve output basis/root/order/format, logical and recorded scaling, noise degree, slots/key tag/context, lifecycle/receipt, and source-derived metadata. Inputs and shared keys/parameters must remain unchanged. Both output metadata maps derive from high and must not alias.
5. Test first with a real unimplemented-backend failure retained at a separate commit. Green must pass the existing independent full-integer oracle and all existing RS2 state/rejection fixtures, plus 58-bit modulus boundaries. Build existing API signature tests. Both HYBRID and BV genuine public inputs are included by the existing fixture; N=32 is functional only, not a secure parameter claim.
6. One bounded Linux GitHub workflow, 2 compile threads, fixed dependency, functional output only. CI timing is not a scientific benchmark. At most one evidence-driven repair; retain all failures. No sustained local OpenFHE build.
7. No default promotion or integration merge. Full Mult2/repeated-path integration and meaningful fixed-resource performance remain follow-up gates. Ordinary algebraic rearrangement and fewer wrappers alone are not a new research contribution.
