## Delivery

The package is classified **`BOUNDED_PUBLIC_API_PROBE`**, not a completed second-Mult2 implementation and not a Section 6.3 reproduction.

**Selected design:** use an immutable ordered-basis context/key family:

\[
B_i=[A_i,q_{\mathrm{div}}],
\]

where each operation family owns its exact ordered Q/root set, generated P/QP tables, HYBRID partition tables, evaluation-key rows, and unique key tag. After RS2 consumes the current trailing multiplication prime, the result is copied into a fresh ciphertext bound to the next family; caller ciphertexts and existing contexts/keys are not mutated.

Important decisions frozen in the package:

- `numPartQ=|B_i|` for each smaller family, retaining `alpha=1`; the paper’s initial `dnum=11` is not silently redefined.
- Cross-family P equality is **not** required or assumed. Every key is generated and used only with its own family’s Q/P/QP tables.
- Secret projection is permitted only during client setup. All private-key objects leave scope before evaluator operations.
- Guard removal, level relabeling, `m_params` mutation, one full-context key for arbitrary subsets, evaluator decryption/re-encryption, and FLEXIBLEAUTO as a scale workaround are rejected.
- The exact logical-scale recurrence uses the actual primes:
  \[
  S_i=\frac{S_{i-1}^2}{q_{\mathrm{div}}q_{\ell,i}},
  \]
  rather than trusting nominal `2^p` metadata.
- The probe uses ring dimension 256 with `SPARSE_TERNARY`, exercising normal OpenFHE’s `h=192` setup branch. A complete matching `h=128` public/evaluation-key family remains a separate mandatory gate.

## Staged executable increment

The archive contains:

1. **Stage 1 patch:** preserves the existing first-Mult2 high-precision oracle and freezes the exact current rejection of the second call. It does not manufacture or claim an executed RED.
2. **Stage 2 patch:** a pristine-public-API construction/key-routing probe covering immutable contexts, alpha-one HYBRID rows, tag-collision preflight, setup-only secret projection, active/full-basis relinearization, context rehoming, and source immutability.
3. **Future semantic contract:** exact 16-slot dyadic X/Y/Z/W vectors for
   \[
   Z=\operatorname{Mult2}(X,Y),\qquad
   W=\operatorname{Mult2}(Z,Z),
   \]
   including sub-binary64 distinguishing deltas and the frozen `≤2^-80` second-stage threshold.
4. The required design decision, source claim ledger, exact paper/source line index, complete changed/new files, two staged patches, 55 preserved CTest bindings plus two additive bindings, execution ledger, paper gates, patch-replay evidence, and complete per-file manifest.

## Verification boundary

Completed in this environment:

- supplied archive identity: 1,451,817 bytes, SHA-256 `efc96137d3412bae57099b6e2f7f85a96bd175b4dd810b587083e1e3d324587d`;
- 156 safe regular ZIP members and exact 155-entry non-self manifest closure;
- project and pinned upstream source identity checks;
- exact rational-vector/oracle verification;
- Stage 1 then Stage 2 patch application and byte-exact final-file replay;
- preservation of all 55 prior CTest name/command pairs;
- output manifest, ZIP CRC, and SHA sidecar closure.

Not run or claimed:

- OpenFHE C++ configure/build;
- either new CTest runtime;
- Linux or Windows GitHub Actions;
- semantic second Mult2;
- matching `h=128` keypair setup;
- eight squarings, 1000 trials, performance, 81.8-bit reproduction, or security validation.

[Download the repeated-Mult2 bounded design and executable probe ZIP](sandbox:/mnt/data/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip)

[Download the matching SHA-256 sidecar](sandbox:/mnt/data/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip.sha256)
