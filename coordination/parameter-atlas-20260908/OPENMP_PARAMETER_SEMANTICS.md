# Root primary-source check: configured versus private sampler state

2026-09-08, documentation research only. No compiler, sampler, key generation, cryptography or new experiment was run. This note is withheld from the active Pro thought and will be used to review its terminal document.

## Observed source

The independent randomness researcher flagged a parameter-effect distinction; root separately inspected both fixed OpenFHE files:

- [`keyswitch-hybrid.cpp`](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/pke/lib/keyswitch/keyswitch-hybrid.cpp#L93): line93 copies the crypto-parameter Gaussian generator into local `dgg`; line98 places `dgg` in an OpenMP `private(dug, dgg)` clause; line102 constructs the error polynomial using that loop's `dgg`.
- [`discretegaussiangenerator.h`](https://github.com/openfheorg/openfhe-development/blob/df495ba2e91739a6dc8f1de254fc5a41155ce504/src/core/include/math/discretegaussiangenerator.h#L93): the Gaussian generator constructor's default standard deviation is1.0.

## Independently checked language rule

Root browsed the official OpenMP5.2 specification, revision95b2e3a44, using a separate reference tab without stopping or refreshing Pro. [§5.3](https://www.openmp.org/spec-html/5.2/openmpse25.html#x68-700005.3) specifies initialization of newly privatized C++ items as a declaration without an initializer and requires an accessible default constructor for a class. [§5.4.3](https://www.openmp.org/spec-html/5.2/openmpsu37.html#x72-740005.4.3) applies those rules to `private`. In contrast, [§5.4.4](https://www.openmp.org/spec-html/5.2/openmpsu38.html#x73-750005.4.4) says `firstprivate` additionally initializes from the original object and ordinarily uses a C++ copy constructor. The reference tab was closed afterwards; the saved Pro tab remains live.

## Conditional inference to preserve in the atlas

Under conforming enabled OpenMP semantics for this exact loop, its private Gaussian object is default-constructed rather than copied from the configured generator. Thus the source-level expected private standard deviation is1.0, not automatically the context's configured value. A one-thread OpenMP region still has privatization semantics; setting the thread count to1 is not equivalent to compiling out OpenMP pragmas.

This is a source/language inference, **not a measurement of any retained run**, a security finding, or an explanation of original S100 failure. Actual deployed behavior also requires exact compiled-branch/binary provenance. The fresh PKE payload uses a different call path; do not carry this inference over to its e0/e1 without tracing it. Changing a context sigma must therefore be analyzed per consumer instead of assuming one global effective sigma.

The final reference should expose this branch and its evidence limits. Any later proposal to change the clause would require a separate justified task, a minimal fail-first check on the intended host, and review of affected keys/noise/security assumptions. **No such change or experiment is authorized or performed by this note.**
