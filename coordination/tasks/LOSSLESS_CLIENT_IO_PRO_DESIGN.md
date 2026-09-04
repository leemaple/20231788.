# Production high-precision client I/O: concrete interface/design decision

## Goal and exact source
The user requests a complete clean-room implementation of paper2023/1788
t=2 Double-CKKS on pristine OpenFHE1.5.0. The single first-Mult2 diagnostic is
now empirically high-precision, but the only current lossless input fixture
exposes a zero stale binary64 Plaintext cache and actual-output testing uses
an independent schoolbook secret/CRT oracle. Neither is a shipping client I/O
contract. Decide a minimal usable high-precision client input/output module,
derived from the paper and pinned official source, without implementing a
parallel cryptographic backend or changing the evaluator.

New isolated branch codex/lossless-io-01 starts at
5cd9f37ca1361d22879aab1845b92341e4cfc34b.
Active code remains47907783a6141d0174da79eae264d779fc598f28.
Pin df495ba2e91739a6dc8f1de254fc5a41155ce504, pristine OpenFHE1.5.0.
Current complete source/test/build and relevant ledgers are provided; never
assume access to a local repo, hidden environment, previous chat or agent.
Verify archive/provenance/hash closure before assuming any supplied claim.

This is DESIGN DECISION ONLY at this stage. New public client-I/O test seams
have not been confirmed with the user. Do not author implementation/test
patches or claim TDD execution. Return concrete proposed interface declarations
as documentation, not compilable changes, and precise future acceptance
contracts. Only after the user explicitly confirms the proposed seam, and
Codex records that confirmation, may tests or implementation be authored.
Codex then asks Pro for the corresponding smallest code slice. Do not replace
this request with a generic list of future work.

## Actual accepted evidence and why a separate module is needed
Existing exact source4790778, run33873114880:
Linux101023587797 full55/55 in1.03s, focusedfirstMult2 1/1;
Windows101023588186 full55/55 in2.30s, focused1/1; both warning builds and
five explicit API targets successful.16 fresh-key first-Mult2 samples all
slot/delta errors<=2^-80 (not statistical or all-key proof). Worst observed
slot1.6696072195146116607129673424340031160212e-27 and
delta1.6958307879080880932103073456218202834178e-27.
Frozen inputscale2^100, outputlogicalscale2^200/(q_div*q_l),
q_div1125899906843009,q_l1125899906840833,
product1267650600226646386227681786497.
CurrentN64,batch16,depth7,p50,first55,FIXEDMANUAL,HYBRID,COMPLEX,
UNIFORM_TERNARY,HEStd_NotSet; this is not paper parameters or security.

Original Pro return, actual logs and reconciled final independent ZCode
GLM-5.3 Max LOCAL STATIC review are provided. That review accepted the narrow
test-only slice with noP0/P1, five nonblocking findings. Main correction note
is authoritative for disposition, not for substituting review by assertions:
margin is~495.43x(slot) and~487.77x(delta), NOT~49.5x; low ciphertext-component
remainder is centered, but decrypted low coefficients can be amplified by
secret convolution; observed centered headroom is not a universal no-wrap proof.
Do not repeat those corrected prose inaccuracies.

The fixture lives only in tests/precision_dcp_rcb_fixture.{h,cpp}; no production
source is allowed to depend on it. Existing <=2^-80 regressions, literals and
independent expected-value path must remain unchanged. A separate pending
user question concerns a narrow automated stale-cache CI safeguard; that is
NOT permission to reuse stale Plaintext as the shipping output contract.

## Paper requirements and fixed conceptual boundaries
Read paper2.1 canonical encoding/decoding and encryption/decryption definitions,
sections3/4 double representation and Mult2, and6.3/Table3.
The paper's can evaluates at powers-of-five primitive roots; Ecd rounds
Delta*can^-1, Dcd evaluates exact coefficient/scale values. Some rounding and
encryption error remain: call the client data path precision-preserving, not
mathematically lossless encryption or exact approximate-number multiplication.
The target paper experiment hasN32768,h128,dnum11,Base50x2,Mult60x8,Div40,P60,
100-bit fresh scale, eight repeated squarings without6.2refresh, mean
infinity-norm errors over1000 runs. Those remain separate empirical obligations.

Evaluator operations DCP,RCB,Add,Sub,Tensor2,Relin2,RS2,Mult2 must stay
homomorphic. It must never receive a secret key, call decrypt/re-encrypt, or use
test oracle plaintext to fabricate a result. Client decryption is legitimate;
keep ownership and interface separation explicit. Use official OpenFHE
cryptographic primitives; do not transplant test schoolbook RLWE decryption
or copy internal crypto algorithms into a new backend.

Keep KISS/YAGNI: a small deep module, simple interface, explicit invariants,
no generic plugin/codec framework, no automatic context/key factory, no
rotations/bootstrapping/multiparty features, no broad serialization format or
new persistence subsystem. Do not add try/catch unless a specified caller
boundary can resolve or deliberately translate the failure. Preserve upstream
pin/public behavior; no fork, hidden API, const_cast or shared context mutation.

## Concrete decisions required
1. Specify input/output representation that never first narrows through
   binary64, with enough precision for100-bit inputs and~80-bit post-evaluation
   results. Compare only genuinely useful alternatives (e.g. fixed
   multiprecision complex values vs textual decimal entry); choose one minimal
   public representation, finite/range handling and roundoff policy.
2. Follow exact official source for encryption of a public DCRT element and
   decryption to an UNDECODED Poly/DCRT result. Determine whether public
   scheme-level calls are supported and which validation/metadata that the
   high-level context wrapper normally performs must be retained.
   Never assume a standard Decrypt output still contains untouched coefficients
   if standard Decode has already cleared or rounded them. Distinguish public
   scheme Decrypt(Poly*) from DecryptCore: preserve configured polynomial
   flooding and explicitly identify ordinary Decode postprocessing that a new
   client path would omit. Do not claim equivalent decryption-output protection
   or silently disable configured protection to satisfy a precision gate.
3. Propose smallest client interface that does not expose an incoherent
   ordinary Plaintext/cache. Decide whether direct Encode+Encrypt returning
   ciphertext and Decrypt+Decode returning multiprecision slots avoids that
   leak. If another interface is better, give exact source-backed reason.
   Include context/key identity, ordered basis, level, slots, format, encoding
   type, component count, required features and metadata invariants; do not
   bypass cryptographic validation because a lower-level overload is public.
4. Resolve exact logical scale ownership. Binary64 scaling metadata and current
   long-double PaperScaleDescriptor are state observations, not precision
   normalization. A rational scale can be supplied/tracked independently;
   explain numerator/denominator validation and how it remains attached to the
   correct operation/state. Do not freeze a conflicting new scale type before
   considering the repeated-Mult2 design dependency described below.
5. Derive canonical transform/order and partial/full slot behavior.
   Existing direct Horner is independent test evidence atN64, but a full
   N32768*16384 coefficient-by-slot loop is not a viable default production
   path for1000 paper runs. Give a defensible O(NlogN)/O(slotslogslots)
   multiprecision route and independent small-N monomial/literal checks.
   Do not benchmark or claim runtime from algorithmic complexity alone.
6. Explain finite precision, CRTcentering, exact coefficient conversion,
   shape/magnitude/headroom checks, key/context mismatch, input immutability
   and output cache ownership. Reject unsupported configurations explicitly.
   Distinguish actual centered representative checks from unknown integer/noise
   history. Do not invent an all-key theorem as an extra completion condition.
7. Freeze one small proposed public-seam tracer-bullet contract in prose:
   precise fixed vectors including sub-binary64 witness, expected scale,
   independent output expectation, predicted genuine initial failure,
   minimal implementation scope and hosted build/test commands. Tests must
   observe the proposed public client interface and retained DoubleCKKS seams,
   never private helpers. Then outline only the next justified slices.
   Do not write tests or implementation until the user explicitly confirms
   this seam and Codex records that confirmation.
8. Define a clean handoff with the repeated-Mult2 task. Its code is NOT available
   yet and it is live separately; do not contact, stop, refresh, remind or assume
   its content. Current known primary-source constraints and full frozen task
   are included. Identify which client decisions can be adopted independently
   and which exact scale/state/context details must be reconciled after return.

## Test, execution and scope constraints
Allowed in Pro environment: read supplied files; bounded source/signature and
integer/rational checks; verify hashes/manifests and consistency. Report actual
commands/outcomes and environment limitations. No invented compilation,
Mac OpenFHE/project builds, crypto/CTest, benchmark, dependency install,
credentials/browser access, Git mutation, CI action or other-agent dispatch.
Existing hosted results are supplied evidence, not your environment's run.

Codex future hosted commands (after user-confirmed seam and reviewed draft):
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=<pristine-install>
cmake --build build --parallel 2
ctest --test-dir build --verbose --output-on-failure
plus focused new public-seam CTest, all old55 tests, warning-as-error settings
and all5 explicit Relin2/RS2/Mult2/Add/Sub API targets onLinuxGCC/WindowsMinGW64.
A future codec roundtrip alone is insufficient: matching wrong transforms
can cancel. Require independent literals/monomial/order witnesses and retained
first-Mult2 highprecision evidence at the public operation integration seam.
Do not change existing frozen precision thresholds to manufacture success.

## Required deliverables
One downloadable ZIP and matching SHA256 sidecar. Exactly documented files:
- DESIGN_DECISION.md: exact selected minimal design with rejected alternatives,
  invariants/errors/ownership, source citations and decisions1-8;
- PROPOSED_PUBLIC_INTERFACE.md: concise declaration-shaped documentation,
  precise user-confirmation question and supported initial configuration;
- PROPOSED_FIRST_TDD_CONTRACT.md: one concrete contract, expected RED cause,
  independent reference, acceptance, then ordered next slices; NO test code;
- SOURCE_CLAIM_LEDGER.md: file/line/pin for each nontrivial upstream claim,
  observed vs inference vs pending;
- REPEATED_MULT2_HANDSHAKE.md: independent decisions vs exact dependencies;
- EXECUTION_LEDGER.md: only actual checks, no claimed build/crypto run;
- MANIFEST.json with per-file sizes/SHA256 and explicit self-exclusion.
Verify all named files exist/nonempty, ZIPsafe, manifestclosed, hashcorrect.
Final chat must agree with actual files and not promise an implementation.
The scope remains the complete paper-directed project; this design increment
does not waive repeated multiplication, paper configuration or actual evidence.
