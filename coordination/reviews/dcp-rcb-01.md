# DCP/RCB review disposition 01

Recorded: 2026-08-31 Asia/Shanghai

## Scope and identities

- Reviewed pre-hardening checkpoint: `e1153122be529ef21e9e5bce1ace877015410304`.
- Current production checkpoint: `4971d2292b5af0ddbbe0c7dbe5a2e87f45102ff1`.
- Cross-platform build-portability checkpoint: `e236a6ef3361169363fd17a74ab1a8dafc539d57`.
- Current cross-platform evidence checkpoint before this update: `e361f1e3a75ad860ef8a34998d8c29be2d5379ae`.
- Branch: `agent/codex-dcp-rcb-01`; it remains isolated from the default integration branch.
- OpenFHE: official 1.5.0 commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Latest Linux strict-build/CTest result: [GitHub Actions 33404816846](https://github.com/leemaple/20231788./actions/runs/33404816846), success.
- Windows/MinGW64: strict build and 1/1 CTest passed in [GitHub Actions 33404816846](https://github.com/leemaple/20231788./actions/runs/33404816846). This is OpenFHE 1.5.0's officially supported Windows path; VC++/MSVC is explicitly unsupported upstream and is not claimed.

The Standards and Spec reviews below are intentionally reported as separate axes.
Judgment-only design observations are also kept separate from documented
requirements. ChatGPT Pro's independent review is a third section rather than
being merged into either internal review.

## Standards review

### Documented findings

| Finding | Disposition | Evidence |
| --- | --- | --- |
| Negative tests accepted any `std::exception`, so an upstream failure could false-pass. | Accepted and fixed. The helper now requires `std::invalid_argument`, the module prefix, and a case-specific substring; other exceptions fail the test. | Commit `2b546641fdbda916bc91a4c9ce50f53c64d81dbd`; [run 33393282342](https://github.com/leemaple/20231788./actions/runs/33393282342). |
| Unimplemented future lifecycle values violated the current YAGNI boundary. | Accepted and fixed. Only `ReadyForFirstMult` remains. | Commit `d7412b8d42299fd07225c23e10c9c11c867895a7`; [run 33393658006](https://github.com/leemaple/20231788./actions/runs/33393658006). |
| The original slash-prefixed local branch name did not follow the project agent-branch convention. | Accepted and fixed without rewriting or deleting recovery history. Active work now uses `agent/codex-dcp-rcb-01`; the old remote branch remains recoverable. | Remote branch equality is checked after every push. |
| The retained hardening red stopped at a compile failure, so it did not independently demonstrate every later runtime assertion. | Accepted as an evidence limitation. The record is retained transparently and no historical behavior is fabricated. Later focused behavioral reds and green runs provide the missing evidence where required. | `artifacts/tdd/dcp-rcb/red-hardening-01.txt` plus the focused minimum-basis, precomputation-access, and encoding-metadata red/green records. |

### Judgment calls, not required refactors

- Real/complex CKKS value tests and repeated multiplication belong to later
  Tensor2/Relin2/RS2/Mult2 slices, not the bounded DCP/RCB acceptance scope.
- Pair manifest duplication is deliberate cross-validation of a read-only deep
  object, not accidental duplication to remove.
- Feature-envy, data-clump, and message-chain observations did not identify a
  concrete contract violation and therefore did not justify a speculative API
  rewrite in this slice.

## Spec review

### Findings against the accepted DCP/RCB scope

| Finding | Disposition | Evidence |
| --- | --- | --- |
| Several retained green/red records omitted the exact workflow commands. | Accepted and fixed. The records now include configure, strict build, and CTest commands. | Commit `b07fbf06448841f26071e4e31e776f4c30e9d24f`. |
| Requiring four Q towers rejected the true minimum first-Mult2 basis `[q0, q_l, q_div]`. | Accepted and fixed test-first. A three-tower fixture failed against the old constructor, then passed after the minimum changed to three. | Red `61325ee41b94f0be355a357392cfae2c5bf5d0c7`, [run 33387083987](https://github.com/leemaple/20231788./actions/runs/33387083987); production `bcf50df`; green [run 33387865378](https://github.com/leemaple/20231788./actions/runs/33387865378). |
| Unused future states combined with an initial-scale assumption could become incorrect once later operations exist. | Accepted for this slice by removing the unused states; later state transitions must be derived and tested when implemented. | Commit `d7412b8d42299fd07225c23e10c9c11c867895a7`. |

ZIP, patch, and `DESIGN.md` deliverables were requirements of the bounded external
ChatGPT Pro and Windows handoffs, not deliverables of the Codex implementation
branch itself. They are therefore not classified as implementation-spec defects.

## ChatGPT Pro independent review

- Conversation: <https://chatgpt.com/c/6a95607d-31ec-83ec-b35b-eedc17c5bf38>.
- Reviewed archive SHA-256: `e32ba8a4b59ef7e377a01e6dfcd426bd3dab8e6db5ecad01c0510fefdc4c6fcc`.
- Returned review archive SHA-256: `b559db7f1a1e5feecad065f467f71d420d49a4995a77782fab26c5024c884293`.
- Reviewer verdict at `e115312`: `ACCEPTABLE ONLY AFTER NAMED FIXES`.
- Arithmetic conclusion: no centered-division, low-construction, pair-scale, or
  RCB-recombination algorithm defect was found.

| Finding | Disposition | Evidence |
| --- | --- | --- |
| OpenFHE precomputation row zero was obtained through unchecked outer-vector access before the code could inspect row lengths. | Accepted, independently reproduced as a behavioral segfault, and fixed without changing OpenFHE. The implementation locally derives `q_div^-1 mod q_i` and its negative before calling the same upstream arithmetic primitive. | Valid red `9a81c3cf78f0d7125a1a251f9f4632e7b4711034`, [run 33388548297](https://github.com/leemaple/20231788./actions/runs/33388548297); production `59bba42d386dd043bdcb4371014c42bb965befd9`; green [run 33388778949](https://github.com/leemaple/20231788./actions/runs/33388778949). |
| Generic exception acceptance weakened fail-fast attribution. | Accepted and fixed as recorded in the Standards section. | Commit `2b546641fdbda916bc91a4c9ce50f53c64d81dbd`. |
| Source immutability execution evidence covered elements and level but not all then-checked metadata; RCB pair-member immutability was also incomplete. | Accepted and fixed for the fields that were in the pair contract at that checkpoint. Later slot-count hardening below closes a distinct observable field that this statement had described too broadly. | Commit `6a954c6847ec4ca252257e26a9f116936c6feb97`; [run 33393525763](https://github.com/leemaple/20231788./actions/runs/33393525763). |
| README state and DCP scale terminology were stale/ambiguous. | Accepted and fixed. Documentation now distinguishes the recombined pair scale from the high quotient component descriptor. | Commit `f82c0cecae20873cb931642affc249533a7fe819`. |

ChatGPT Pro could not locally compile its supplied OpenFHE archive because cereal
submodule content was absent, so its response made no build, CTest, precision,
performance, or Windows claim. The Linux and Windows results above are independent
GitHub Actions results produced after Codex reproduced and fixed the findings.

## Additional Codex hardening after review

Codex added a focused adversarial check that an RCB pair member cannot silently
change from CKKS packed encoding metadata. The unchanged validator failed the new
test at commit `dd3a254a2a0505f85267b55dc0e4fab083e69655` in
[run 33394331645](https://github.com/leemaple/20231788./actions/runs/33394331645).
The shared ciphertext validator was fixed at
`b0cd3adba690b71b6446ade8c038002efea4b6ec`; the strict build and full test then
passed in [run 33394619792](https://github.com/leemaple/20231788./actions/runs/33394619792).

The first strict Windows consumer compile then exposed OpenFHE's public use of
the non-standard `M_E` macro under MinGW's strict C++17 mode at commit
`91fd77bd263799ce5b1cd2c4e8f203b92e8c8a78` in
[run 33398384157](https://github.com/leemaple/20231788./actions/runs/33398384157).
A MinGW-only public `_USE_MATH_DEFINES` consumer definition fixed that upstream
header portability boundary without modifying OpenFHE or enabling GNU language
extensions. Commit `e236a6ef3361169363fd17a74ab1a8dafc539d57` passed the strict
Linux and Windows builds and 1/1 CTest in
[run 33399245184](https://github.com/leemaple/20231788./actions/runs/33399245184).

A later audit found that the pair manifest did not bind OpenFHE's CKKS slot
count, even though that field controls the decoded output length. Test-only
commit `d0cbc97190c9cc5be2164c7bcbff82109fd2ca55` built strictly and then failed
with the intended `tampered pair slot metadata did not fail fast` diagnostic in
[run 33404277096](https://github.com/leemaple/20231788./actions/runs/33404277096).
Production commit `4971d2292b5af0ddbbe0c7dbe5a2e87f45102ff1` records the slot count in the
private pair manifest and cross-validates both stored ciphertexts before raw RCB
arithmetic. The strict Linux and Windows/MinGW64 builds and 1/1 CTest passed in
[run 33404816846](https://github.com/leemaple/20231788./actions/runs/33404816846).

## Current integration decision

The DCP/RCB source now has strict Linux/GCC and Windows/MinGW64 build/test
evidence against the same exact slot-hardened production commit. It is not yet
ready to merge into the default branch because the preserved Windows Z
code/Zima session and ChatGPT Pro have not performed the required final review
of that same green commit. The currently running ChatGPT Pro review is bound to
the earlier `a3df1c5` checkpoint, so even a favorable verdict there requires a
bounded remediation review. Later operations remain separate TDD slices and are
not implied by this decision.
