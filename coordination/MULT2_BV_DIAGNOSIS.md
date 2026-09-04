# Mult2 BV coefficient-oracle failure

Observed2026-09-04. Source3087ff81d9cc86d277a23d12bf795a9c441556b2,
run33839781546, Linux job100919538142. Warning/default and Relin2/RS2 API
builds passed.42/44 tests passed; only BV REAL/COMPLEX fail at
pair relinearization error exceeded empirical E_Relin + h,0.04s each.
The HYBRID real and complex certificate cases passed. Full retained red output:
artifacts/tdd/mult2-coefficient-oracle/matrix-red-linux.txt.

## Feedback loop and scope

Actual loop already executed by hosted CTest: mult2_e2e_oracle_test bv_real
and bv_complex, via ctest --test-dir build --verbose --output-on-failure.
Focused replay is ctest --test-dir build -R '^mult2_e2e_bv_real$' --verbose.
The failed case, not the entire41-test baseline, is the focused signal. Two
different genuine random-key host cases failed identically on Linux; the exact
error magnitudes/distribution and second-platform outcome remain pending.
The logical pipeline is retained to exercise the named public Mult2 seam; no
further input/ring minimization is claimed yet. A deterministic seeded-crypto
reproduction is not claimed. No production fix is justified at this point.

## Ranked falsifiable hypotheses

1. Empirical bound misuse: ordinary Relinearize on the recombined tensor is a
   different execution from the high/low Relin2 paths; its measured max error
   is not a justified conservative E_Relin for those paths. Prediction: basis
   checks agree while the pair error is larger than this unrelated measurement;
   deriving/replaying the exact per-path errors may explain it without a
   production algorithm mismatch. This alone does not authorize deleting a gate.
2. BV decomposition/lift implementation mismatch. Prediction: after comparing
   each public Relin2 coefficient to a correct independent per-path equation,
   a discrepancy remains that cannot be explained by valid switch/rounding
   error. It should persist with correct bounds and may vary with BV digit size.
3. Independent decryption/CRT basis error. Prediction: ordered basis or direct
   c0+c1*s+c2*s^2 reconstruction disagrees on a BV input/intermediate before
   relinearization error is even formed; cross-check should fail upstream.

The next hosted revision adds only tagged [DEBUG-mult2-bv-bound] diagnostics
immediately before the unchanged failing check: q_div/q_l, both empirical
errors, h, Q_l and confirmation that existing basis-agreement checks reached
this point. No production, vector, parameter, divisor, threshold or acceptance
inequality changes. Remove the tagged instrumentation when diagnosis closes.
Owner Codex; external Pro/Fable5.1 escalation will include the exact red and
diagnostic data, complete pristine references and current source. Fable's prior
403 is not a successful review and must not be relabelled. Ordinary waits do not
block the parallel Add/Sub TDD track.

## Diagnostic reproduction on both platforms

Original matrix3087ff8 also failed on Windows job100919538008:42/44,1.18s,
same two BV cases. Diagnostic revisionbda8791 run33840176712 completed with
the same42/44 on Linux0.71s and Windows1.18s. Basis-agreement checks reached
the diagnostic point on all cases. The original and instrumented matrices thus
reproduced this failure in8 independently generated BV fixtures across two hosts;
this is a repeatable observed sample, not a probability/seed guarantee.

Measured ordinary-E / pair-error / h in diagnostic runs:
- Linux REAL:197331007675 /323602105437 /43;
- Linux COMPLEX:181218269350 /223094194606 /44;
- Windows REAL:190622642352 /254980207580 /47;
- Windows COMPLEX:200349662290 /316191854996 /50.

HYBRID stayed green. These data falsify using that one ordinary measurement as
this run's pair bound; they do not by themselves decide whether a correct
per-path bound would clear the production Relin2 implementation. Root-cause
adjudication remains pending. Tagged diagnostic logs remain until then.
