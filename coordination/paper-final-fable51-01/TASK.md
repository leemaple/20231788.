# One bounded Fable 5.1 consultation: paper-scale production integration
Requested model: claude-fable-5-1 only; do not substitute another model.
This is a design/source consultation, not implementation. Answer in <=1800 words.
Read only the supplied sanitized packet; no command/code/crypto execution, web, writes or external actions.

## Identity and authority
Engineering source: 9c4d83b5cde16e5c5af89886bd73fe5252a99002.
Documents: 07d0d1f130b2c9b041a61b97d4546e7058066587.
Branch codex/three-track-integration-20260905. Official OpenFHE1.5.0 pin:
df495ba2e91739a6dc8f1de254fc5a41155ce504, native64/backend4.
Only this fresh clean-room implementation, the user paper, and selected pristine
official files are inputs. No old local implementation or modified OpenFHE.
The user's latest scope cancels1000 trials: keep meaningful correctness tests,
but no statistics/benchmark/security-theorem gate. Current co-build is NOT
paper-scale end-to-end acceptance. Existing diagnostics must remain strict.

## Complete packet/read order
TASK.md and MANIFEST.json identify every actual selected file and its SHA256.
Read current project/include/openfhe_2023_1788/{double_ckks,repeated_mult2,
high_precision_client_io,paper_h128_client_keypair}.h and matching src/*.cpp;
all current tests/helpers and CMake/workflow are supplied for exact contracts.
Read project/coordination/TEST_SEAMS.md and
CORRECTNESS_ACCEPTANCE_SCOPE_20260905.md; paper/PAPER-2023-1788.txt
section6.3/Table3 (around text lines1560-1600), with full PDF also supplied.
Read historical-probe/table3_profile_probe.cpp at exact commit
599fc158b6b67d3e752c39cb53069c29dc60fc6f. It supplies mixed-prime/roots/basis
observations only: no keygen, encryption, decryption or evaluator evidence.
Its EncodingParamsImpl(100,kBatchSize) MUST NOT be inherited as the production
FIXEDMANUAL compatibility metadata contract.
official-full/ contains77 complete exact-pin source files, not snippets.
These include math aliases, CKKS parameters/factory/PKE/leveled-SHE, DCRT and
ciphertext/key definitions. If one directly decisive definition is absent,
identify that exact missing symbol/path; continue the unaffected analysis.

## Goal and proposed scope
Prefer going directly to N32768/M65536/S16384/gap1, one h128 root secret and
eight sequential squarings of ONE high-precision encrypted vector, then final
production client decryption. A second-square checkpoint may be observed in
the same trace; do not make a new N256 two-square profile a mandatory project.
No evaluator secret, intermediate decrypt/re-encrypt, bootstrap, or6.2 refresh.
Existing N64 I/O, N64 repeated two-operation, and N256 fixed-Q h128 diagnostics
stay unchanged; a new explicitly named paper profile may share implementation.

Paper Table3 t=2: exact S0=2^100; Base two50-bit primes; eight60-bit Mult primes;
40-bit Div;60-bit auxiliary P; h128; N32768. Use actual ordered prime/root
identities, not nominal powers as moduli. B0..B7 have11..4 fullQ towers;
after the eighth result, terminal two-tower basis must be the B0 prefix.

## Observed blockers and competing proposals to judge
1. I/O cpp:46-49 fixes16/N64/M128/gap2;97-114 fixes geometry;169-210 fixes
UNIFORM, depth7, Q8, two named primes and three partitions. Transform arrays/
stride (305-406) and encode/decode (448-595) are fixed-size. BindFirstMult2Rcb
(503-544) accepts only same-context fresh parents and outputlevel2.
2. Repeated cpp:34-118 fixes N64/M128, UNIFORM, alpha1 and nominal50;
200-235 requires exactlytwo families;264-290 startsQ10/depth9/50/55.
3. h128 cpp:163-184 accepts suitable finalized N>=128 SPARSE contexts.
CreateFixedQH128ClientKeyPair:193+ already generates a fresh matching h128
SK/PK using official EncryptZeroCore. Invoke exactlyonce at B0, then project
that SAME signed ternary secret by modulus/root/order to each actual Bi and
generate its own evalrow; do not freshly sample each family or retain SK
inside the public plan. Existing repeated setup:311-326 shows this projection.
4. Receipt exact recurrence is S_i=S_(i-1)^2/(d*m_i). Public current receipt
has family/operation/phase/parent/exactscale; no general output adoption seam.
Current DoubleCKKS RCB returns raw ciphertext and loses receipt.
Reenter (repeated.cpp:371-398) changes context/tag/locallevel, not coefficients.
Terminal result must be wrapped back to B0 originaltag/absolutelevel9 only
after exact original-prefix and same-secret family relation validation.
5. Proposed SMALL interface: explicit paper client setup -> {plan,PK,rootSK};
an I/O constructor/factory bound to that validated plan; evaluator RCBWithReceipt
returning an owned immutable issued result; I/O BindRepeatedRcb consumes it.
Do not accept arbitrary caller scale or claim cryptographic lineage proof.
Keep legacy constructors/firstbinder strict. Is there a smaller sound seam?
6. Critical nominal-vs-actual question: current DoubleCKKS ctor:273-274 expects
fresh recorded=base^2; Tensor:847-861 divides raw factor by nominalbase;
RS2:1098-1108 divides by GetModReduceFactor. Repeated:204-231 fixes initial
recorded2^100. Proposed production compatibility is nominal50 (fresh recorded
2^100) while actual logical scale uses d40/m60 primes. It may permit unchanged
existing metadata mechanics with a separate exact rational authority.
DO NOT default to old probe100 or set nominal60 merely because Mult is60.
If nominal60 with initial100 were forced, exponent recurrence e'=2e-120
would underflow at step6; ctor also initially disagrees. Check the exact source
whether nominal50 has hidden semantic side effects in PKE/leveled-SHE/
HYBRID/precomputation. Give counterexample or concrete approval conditions.
7. Inputs/precision must be frozen BEFORE execution. Full16384-slot expected
plaintext z^(256) must be independently computed, with a sub-binary64 input/
output witness and nontrivial complex packing; avoid allzero/decay-tozero or
vectors whose final encoded coefficients wrap the two-Base centered modulus.
Old 16 vectors are frozen historical diagnostics, not automatically suitable
for eight squarings. Propose a finite deterministic vector generator and
defensible absolute/relative/anchor bounds. Existing gates2^-80 and2^-120
must not be weakened; paper's average81.8bits is not a universal run bound.
8. Do not scale the current test O(N^2) schoolbook convolution or all-slot
Horner to N32768. Keep ALL slots' production plaintext error check. Options:
independent exact sparse h128 c1*s in O(128N), then a few predetermined O(N)
MP Horner anchors; independent expected plaintext arithmetic O(8S).
Discuss which matching-wrong-codec blind spots remain and minimal controls.
9. Rough memory inference only: alpha1, P1 and familiesQ11..4 give eval A/B
native coefficient payload about276MiB; existing sealed A/B copies another
276MiB, excluding contexts/tables/temporaries/allocator. Actual peak/time is
NOT measured. Current MP root generation has many transcendentals; don't use
paper HEaaN179ms as an OpenFHE/GH estimate. A single bounded hosted run may be
viable; use N256 only if a concrete resource/diagnosis need demonstrates it.

## Specific requested decision / deliverables
Give a direct accept/change recommendation, exact source/paper anchors and
concrete counterexamples (where applicable) for:
A. minimal complete production interface/owned receipt/terminal rehome;
B. nominal50 compatibility with d40/m60 exact scale and actual mixed-Q setup;
C. deterministic full-slot input domain, pre-run numerical acceptance and
nonquadratic independent oracle for direct eight-square correctness.
Separate observed source facts, inferred safety, and unverified runtime.
Identify a single genuine end-to-end RED assertion (expected actual output),
not static_assert/type-shape self-proof. If initially only a missing declaration
fails compilation, label it API RED, not an executed semantic RED. No fake
test-result, no expected-unsupported exception counted as final GREEN.
List only substantive blockers and the smallest repair; no generic framework,
no new interface-confirmation token or user decision request.
No code changes, builds, cryptography, CI dispatch,1000 runs, external messages,
or new security/performance claims. Output one visible <=1800word answer.
