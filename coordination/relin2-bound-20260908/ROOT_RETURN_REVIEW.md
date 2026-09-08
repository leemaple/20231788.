# Root review of the returned Relin2 contract

2026-09-09 Asia/Shanghai. Runtime source remains a4b815a; official OpenFHE pin df495ba2. This is root's independent source/algebra/replay review, separate from the author's verification. Final adoption also considers RETURN_MATH_REVIEW.md from the independent source-map reviewer.

## Integrity and actual execution

Return ZIP SHA256 `68fbad707507d225e05f35ef4a2039b8ef693a615c2bcc68e7cd386f8dad6b4d`, 189392 bytes, 24 regular members; manifest SHA256 `5a35ca6b224c6737af831f33a9e714112af903fb1b7103376ecd7647c7fc6026`. RETURN_INTAKE.json records path/CRC/hash closure and zero findings from targeted and Gitleaks8.30.1 decoded scans. Temporary browser rate limiting was respected; one download click succeeded after waiting. No author task was restarted or resubmitted. The final UI reported 64m15s; terminal observation was 2026-09-08T16:26:49.473Z.

Root fully read the 640-line proof, all 30 claims and the assumption registry, all 50 source-map rows, NEXT_ACTION, execution/check documentation, and all four returned Python scripts before executing them. The scripts use standard-library integer/Fraction models only; their only writes were directed to a new replay directory outside immutable pro/.

The author's SOURCE_MAP.tsv uses CRLF. Default staged `git diff --check` flags those 51 line endings; preserve the manifest-bound bytes rather than silently normalizing them. A command-local `core.whitespace=cr-at-eol` check is used for this imported artifact, without changing repository/global Git configuration.

ROOT_REPLAY.json records four exit0 commands with empty stderr. All 13 synthetic model groups and 8 exact public-parameter family calculations passed. Input verification, model results, public bounds JSON and TSV are byte-identical to their returned counterparts. The source-map hashes and text intervals resolve against the original fixed input archive. This is not production FHE RED/GREEN, a historical endpoint replay, or a proof of the entire underlying NTT implementation.

## Source/algebra reconciliation

- C03–C08: the one-prime digit lift, two separate nonnegative P remainders, key equation and integral noise quotient agree with root's pre-return draft. The source uses both ApproxModDown outputs; omitting the first remainder is unjustified.
- C09–C12: digit carry introduces G_i before quotient extraction; the residual output carry alone is not the whole coordinate correction. The integer subtraction identity and the two-coordinate decoded bound follow without an independence assumption. The general multi-prime discussion remains a primitive model, not a theorem for all HYBRID contexts.
- C13–C17: source lines keyswitch-hybrid.cpp:308–438 and double_ckks.cpp:1009–1060 support zero extra d digit, the same-family P-tower offset, and recombination cancellation. This sharpens root's two-separate-call formulation. Full and restricted error lifts have the same nonzero digits and P residues; they are not errors from independently generated family keys.
- C18–C19: root checked DGG header:79–106, implementation:52–114 and DCRT constructor:126–150. Successful Peikert outputs lie in its finite vector's index range. The conservative support39 covers both stated standard deviations; it is not a historical sigma measurement, Gaussian-distribution equivalence or security proof.
- All eight Pro normalization values are below root's pre-return bounds. Their exact difference is `N²*39*(d−1)/P * d/S_f²`: root had conservatively allowed noise from the raised d digit, which C13 proves is zero. ROOT_REPLAY.json verifies this exact identity, not merely matching decimal digits.
- C20–C24: direct low-part compensation, Tensor positive cross terms, one recombined RS rounding term and the exact scale recurrence match the source. Input lift terms and W remain explicit. The sufficient NW condition is not circular and is not a claim about observed historical headroom.

## Independent check of C25 (no phase nonwrap assumption needed)

Let Ht=h_a*h_b and Lt=h_a*l_b+l_a*h_b be integer tensor-phase lifts, and let rho_d be the phase of the two centered d remainders. Define

    X = Ht + (nu_H-rho_d)/d,
    Y = Lt + nu_L + rho_d.

X is integral: the full raised-high phase and the d-remainder phase agree modulo d. X and Y represent the actual high/low phases modulo Q. For the two actual rescale remainder terms eps_H and eps_C, each with coefficient norm at most R_m, the next high and low phases have integral representatives

    X/m + eps_H,
    Y/m + eps_C - d*eps_H.

Integrality follows from the respective ciphertext residue phases modulo m. Changing a lift by Q times an integer changes these representatives by Q/m times an integer, so final centering removes it. Centering an **integer coefficient** cannot increase its absolute value. Bounding X and Y, followed by negacyclic convolution, gives the two C25 inequalities. This argument does not assert canonical-norm contraction under centering and does not establish the original chain's numerical nonwrap.

## Required wording qualification

The proof's wording “only when all kappa_j=0” is to be adopted as **a sufficient special case**, consistent with CLAIMS.json C11's “When kappa=0”. It is not a necessary condition for small corrections: other G terms may vanish or cancel for special keys/inputs. Likewise, alpha=0 and W=0 are a sufficient route to the simplified local numerical inequality, not an iff characterization of every case in which that inequality may happen to hold. The main C16/C23 formulas do not require this stronger necessity claim. Preserve immutable pro/; put this qualification in the adoption record rather than silently changing author bytes.

## Boundaries and disposition

Root finds no unsupported production patch in this return. Adoptable scope is a conditional source-specific mathematical contract, not “code is bug-free”, not “original S100 passes”, and not “the paper is fully reproduced”. Coarse RS and low-product upper bounds above T are not measured lower bounds. The old original S100 FAIL and changed-condition S116/annulus PASS remain distinct.

Do not lower noise, switch public payload encryption to secret-key encryption, or tune input conditions merely to turn the old test green. As a limited cross-check, the supplied paper TXT §2.1 explicitly describes public-key encryption `v*pk+(m+e0,e1)`; §6.3 reports an empirical average over executions, not a universal per-input accuracy theorem. The actual PDF remains the formula authority. These source observations do not identify the missing HEaaN experiment choices or authorize changing the accepted original test.

Pending final gate at writing: read and resolve the independent return mathematics review. No new FHE run, compile, FFT/NTT, sampler call, CI dispatch or production modification has occurred in this return review.
