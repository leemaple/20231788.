# Paper-precision parameter/lifecycle gates — Codex source cross-check

Status: source observations and next acceptance obligations only.
Observed2026-09-04; current code f4e661eb3a86819ee595e88e0e3227ee0620ce7d,
active tested code bd141806bd1e0b1dad80c7ad47bfd92fc334db55.
The next Pro first-Mult2 task is already running with these complete sources;
this note does not interrupt it or introduce an additional assignment.

## Observed from the supplied paper
PAPER-2023-1788.txt:1512-1531 (Section6.2) describes recombine/decompose refresh
between levels6/7 and12/13 for its18-level capacity experiment. It consumes
three Div primes: one initial, two refreshing. Refresh is not a free relabel.
PAPER-2023-1788.txt:1557-1603 (Section6.3/Table3) instead uses100-bit scale,
40-bit Div versus60-bit Mult. It explicitly reports eight repeated squarings
without needing the6.2 refreshing strategy and averages errors over1000
executions. Its t2 N2^15,h128,dnum11,auxP60,Base50x2 and reported~81.8bits
are separate from our N64,p50/50,UNIFORM_TERNARY diagnostic.
Thus first-Mult2 acceptance is not eight-layer paper reproduction; nor should
we impose repeated bootstrapping as an invented prerequisite for Section6.3.

## Observed current barriers, not proof of impossibility
Current include/openfhe_2023_1788/double_ckks.h exposes approximate long-double
scale descriptors and only ReadyForFirstMult/ReadyForRS2/RefreshRequired states.
src/double_ckks.cpp:241-278 binds q_div to the last full Q tower and fresh input
scale to GetScalingFactorReal(0)^2. Lines357-374 restrict DCP to level0,degree2.
Lines440-557 validate only pair levels1/2 with first-lifecycle scale formulas.
Lines766-768 reject Tensor2 inputs not ReadyForFirstMult. RS2 returns
RefreshRequired; public RCB is not authorization to restart DCP at arbitrary
levels or consume arbitrary next primes. Removing these guards would not
implement a justified repeated lifecycle.

Pinned official OpenFHE1.5.0 df495ba2e91739a6dc8f1de254fc5a41155ce504:
- ckksrns-parametergeneration.cpp:162-192 creates the entire Q vector,
  SetElementParams and then PrecomputeCRTTables. Parameters and precomputed
  key-switch/rescale tables are connected, not independent labels.
- SinglePrimeModuliGen:415-449 starts with a final dcrtBits prime and populates
  intermediate FIXEDMANUAL primes around that size. Merely setting p60 does
  not introduce a separate final40-bit Div and50x2 Base arrangement.
- ckksrns-cryptoparameters.cpp:46-82 computes rescale tables using the actual
  ordered Q vector. Lines86-176 distinguish flexible/composite scale arrays
  from FIXEDMANUAL's pow(2,p) approximate base scale.
- ParameterGeneration:122-156 and194-205 includes HE-standard checks on
  estimated and actual Q(P); bypassing them or using HEStd_NotSet cannot
  become a claimed128-bit security result.
Source SHA/size provenance and complete files are in the verified first-Mult2
handoff under official-openfhe/; hashes are retained in
coordination/handoffs/first-mult2-precision-c9ee28d-source-provenance.json.

## Inferred required next gates (not adopted code/design)
1. Execute the independently normalized first-Mult2 precision test first.
   Exact ideal output scale for this existing path must be derived from
   integer2^200 and actualq_div*q_l, never from an approximate descriptor.
2. Design a minimal validated parameter-construction boundary that makes the
   intended Div/Mult/Base roles, ordered RNS bases and prerequisite tables
   explicit. Public SetElementParams exists in upstream code; this fact alone
   does NOT establish that mutating a live shared context is safe or sufficient.
   No such mutation, custom-context experiment or policy change was attempted.
3. Extend a justified level/scale/domain lifecycle for repeated multiplication.
   Freeze a second multiplication oracle before changing the first-lifecycle
   rejection; preserve invalid-state rejection and input/key immutability.
   Section6.3 may reach eight steps without mid-computation refresh. A later
   capacity/refresh slice must explicitly account for additional Div primes.
4. Define usable lossless production I/O separately from the accepted test-only
   stale-cache injection/secret-key oracle. Do not present the latter as safe
   production decryption, serializable plaintext or a completed codec.
5. Increase to paper parameters and evaluate security/precision/performance
   with documented assumptions, actual samples and hosted resources. The
   mean error from1000 executions is not a worst-case or all-key guarantee.

No source/test/API change, compile, crypto run, benchmark, security analysis,
or new precision result is performed by this note. Pro's concrete design and
candidate are pending; Codex will reconcile against these source obligations.
