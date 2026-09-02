claude-fable-5-1

**Decision.** The API reviewer is right on both points. The valid `ExtraValid` fixture is the real two-entry vector from one `EvalMultKeysGen` call, and both cases must be honest runtime reds that require a normal `ReadyForRS2` return. They belong in the red commit that immediately precedes the arithmetic commit, never as scaffold-through greens.

**1. Facts with anchors**

Project source at `1e59e8b`:
- `project/src/double_ckks.cpp:605` binds `front()`. Lines 606 to 697 validate only that key and line 698 throws the scaffold. The only size test is `.empty()` at line 602. So a two-entry vector, null at index 1 or not, reaches line 698 today.
- `project/src/double_ckks.cpp:439` requires the fresh input factor inside `ValidatePair`, which has no lifecycle branch in lines 421 to 460, and line 702 makes `RCB` depend on it. A `ReadyForRS2` pair carrying `SF_T` cannot validate at this SHA. That is arithmetic-commit work.
- `project/tests/relin2_test.cpp:71` is the scaffold helper. Every call to it sits inside a negative-named test as a subordinate positive control, so no registered CTest closes a success row today.
- `project/tests/relin2_test.cpp:386` to 406 is `MakeContext`; it never sets `maxRelinSkDeg`. Lines 533, 588 and 880 mutate the public map directly under the RAII guard at lines 89 to 103. Lines 3152 to 3155 turn any unexpected exception into exit 1 with its text.
- `project/CMakeLists.txt:74` to 99 register the 26 tests.

Contracts:
- `contract/chatgpt-pro-relin2-01.md:262` to 264 and 282: extra later keys allowed, index zero only, never require exactly one key. Lines 502 to 505 define both cases and say "with successful Relin2". Line 405 forbids calling the scaffold a green. Lines 372 to 375 require red cases to reject the scaffold.
- `contract/relin2-preflight.md:204` to 206 states the same rule; lines 272 to 282 fix the red-then-green order.
- `contract/chatgpt-pro-relin2-remediation-06.md:258` to 259 are the two success rows. That table binds a `RunKeyMutation` wrapper structure the current file does not have, so the rows need mapping, not a refactor.

OpenFHE 1.5.0:
- `openfhe/src/pke/lib/schemebase/base-leveledshe.cpp:335` to 340: `RelinearizeInPlace` loops `j` from 2 below the component count using `evalKeyVec[j - 2]`. For three components only index 0 is dereferenced.
- `openfhe/src/pke/include/cryptocontext.h:2026` to 2031: public `Relinearize` checks only non-null input and `size() < components - 2`, then forwards the whole vector. Two entries pass, null included.
- `base-leveledshe.cpp:147` to 163: `EvalMultKeysGen` emits `maxRelinSkDeg - 1` keys with the secret power raised each iteration, so index i holds the key for s to the power i plus 2. `cryptocontext.h:1858` to 1859 documents first key for s², second for s³. Lines 136 to 144 of the same file show `EvalMultKeyGen` emits one s² key and never reads `maxRelinSkDeg`.
- `openfhe/src/pke/lib/cryptocontext.cpp:87` to 104: both generators insert only when the tag is absent. Lines 132 to 140 show `InsertEvalMultKey` throws on an existing tag despite the stale comment at `cryptocontext.h:717`. So the only public route to a real two-entry row is `EvalMultKeysGen` into an empty row.
- `openfhe/src/pke/include/scheme/gen-cryptocontext-params.h:380` to 382: public virtual `SetMaxRelinSkDeg`. `base-leveledshe.cpp:154` reads the getter through the base parameters pointer, so it is reachable from the module's parameters object.

**2. The valid `ExtraValid` fixture**

Generate `[s² key, s³ key]` with one `EvalMultKeysGen` call on a context with `maxRelinSkDeg` 3. Reasons:
- OpenFHE defines the vector by slot. A duplicated s² pointer at index 1 is shape-valid but role-invalid, cannot arise from any public path, and would silently decrypt wrong on a four-component input. It is not "an additional later valid key".
- Aliasing one pointee at two indices makes "index 1 not inspected" unobservable, because every check on index 1 is a check on index 0.
- The real vector is exactly the cache a lazy-relinearization user has, which is what the requirement protects.

`MalformedLater` uses the same context and the same generated vector, then assigns null to index 1 through the public map reference under the guard. Null is the strongest malformation because a dereference would crash rather than misbehave, so a normal return alone proves "not inspected or consumed".

Helper change: `MakeContext` gains a fifth defaulted parameter `maxRelinSkDeg = 2` and calls the setter unconditionally. All five existing call shapes are untouched, and existing tests are insensitive to the value because they use only the singular generator.

**3. Reds, not scaffold-through greens**

- The frozen rows say success true. At this SHA the only reachable outcome is the scaffold throw. A test that accepts it is green the instant it is written and was never observed red for the property it names. That is a tautology, and contract line 405 forbids reporting it as a Relin2 green.
- "Later entries not consumed" is a property of the arithmetic path, which does not exist yet. A scaffold-through variant would characterise only "preflight reaches line 698 with two entries", which is visible by inspection and is not a frozen requirement.
- The reviewers' plan writes each test twice and edits the oracle between red and green. That is exactly the oracle change the contract forbids, and it is waste.
- These reds are coupled to the arithmetic commit with no useful intermediate behaviour. The constraint therefore says: do not split them into long-lived reds. The resolution is placement, not weakening. They go in the red commit immediately before the arithmetic commit, in one delivery, and that red commit is never merged alone.
- The red is not manufactured. It is the existing line-698 throw, surfaced by `main` with its exact text.

**4. Minimal sequence**

R1, test-only red commit:
1. `MakeContext` gets the fifth parameter and setter call.
2. Add `CheckCompletesNormally(function, label)`, which converts any exception into a labelled `TestFailure`.
3. Add `CheckReadyForRS2Result(result, tensor, context, label)` asserting lifecycle `ReadyForRS2`, context identity, divisor, ordered moduli equal to the Tensor's, level 1, two components, Evaluation format, key tag, slots, noise-scale degree 3, recorded factor equal to the Tensor's, paper scale of `SF_T`, `q_div`, and the Tensor's high and recombined scales, and for each member the same context, CKKS packed encoding, two elements over exactly the Tensor's moduli in Evaluation format, level 1, degree 3, `SF_T`, tag and slots.
4. `TestExtraLaterValid`: `MakeContext(3, 8, HYBRID, 0, 3)`; fixture checks as in neighbouring tests plus `GetMaxRelinSkDeg() == 3`; empty cache; guard; `EvalMultKeysGen`; assert row size 2, both non-null, distinct pointers, both `EvalKeyRelinImpl`, bound context, Tensor tag, HYBRID A/B lengths, Evaluation format and `ParamsQP` basis for every entry; snapshot Tensor, both entries' A/B, cache, vector and pointer identities; `CheckCompletesNormally` around the Relin2 call; immediately `CheckTensorUnchanged`, `CheckKeyVectorUnchanged` for both entries, cache size 1, vector identity, size still 2, pointers, contexts and tags unchanged; then `CheckReadyForRS2Result`; cache empty after the guard.
5. `TestMalformedLaterIgnored`: same through generation; set `generatedRow->second.back() = nullptr`; assert size 2, front non-null, back null; snapshot Tensor, index-0 A/B and identities; `CheckCompletesNormally`; invariance including back still null and front pointer unchanged; `CheckReadyForRS2Result`; cache empty after the guard.
6. Register `key_extra_later_valid` and `key_malformed_later_ignored` in `ResolveTest` and as `relin2_key_extra_later_valid` and `relin2_key_malformed_later_ignored` in CMake.
7. Linux run must show exactly the two new cases failing after their fixture assertions pass, with this text, and the 26 others green. Record both observations.

```
Relin2 unexpected exception: DoubleCKKS: Relin2 is not implemented
```

G1, production green commit immediately after R1 in the same series: the smallest complete Relin2 per contract steps 4 to 11, including the `ValidatePair` lifecycle branch and scaffold removal, with no edit to either new test. One SHA green on Linux and Windows. Do not merge G1 without the preflight's arithmetic-oracle and result-state reds also present.

Mutations on the G1 tree, disposable, each restored with `git checkout -- src/double_ckks.cpp`, `git diff --exit-code`, an unchanged `git rev-parse HEAD`, and a green rerun:
- M1: after the empty check insert the snippet below. Expected: exactly `relin2_key_extra_later_valid` and `relin2_key_malformed_later_ignored` fail, everything else green. Both fail necessarily because any later entry means size 2. Report that plainly rather than isolating artificially.
- M2: after the index-0 null check at line 608 insert a loop rejecting any null entry at index 1 or later. Expected: exactly `relin2_key_malformed_later_ignored` fails. Placement after line 608 keeps `relin2_key_null_first` on its exact diagnostic.

```cpp
if (evaluationKey->second.size() != 1) {
    Invalid("Relin2 evaluation-key vector must contain exactly one key");
}
```

Follow-up, test-only: after G1, retire the scaffold tolerance in `CheckPassesCurrentScaffoldOrCompletes` so no positive control still accepts a `logic_error`.

Optional strengthening for `ExtraValid`: call Relin2 again under a fresh guard with only the index-0 pointer installed and assert both members compare equal. It makes "index 1 not consumed" a runtime fact. It assumes deterministic key switching, which the packet does not show, so treat it as optional.

Remaining unknowns, all outside the packet:
- OpenFHE's default `maxRelinSkDeg` lives in `cryptocontextparams-defaults.h`; believed 2. The explicit setter removes the dependency.
- Whether the CKKS `CCParams` override disables `SetMaxRelinSkDeg`, and whether `GenCryptoContext` copies it into the RLWE parameters. Detectable at the first Linux run by the size-2 fixture assertion. Fallback: `EvalMultKeyGen` plus a test-built s³ private key through public `KeySwitchGen`, appended through the public map.
- Whether `maxRelinSkDeg` alters CKKS modulus-chain generation. The tests assert four towers and `GetNumPartQ() == 2`, so drift fails the fixture instead of passing silently.
- Whether `KeySwitchGen` stamps the new key's tag. Only matters for the fallback.
