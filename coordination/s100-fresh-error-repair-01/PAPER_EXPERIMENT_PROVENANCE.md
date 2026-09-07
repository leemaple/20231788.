# Paper 2023/1788 Section 6.3 experiment provenance

## Answer

**No: the located primary public sources do not establish the exact Section 6.3/Table 3 input distribution, concrete \(\chi_{enc}\) or \(\chi_{err}\), or whether the proof-of-concept created its initial ciphertext through HEaaN public-key or secret-key encryption.** These are experimental-provenance unknowns, not parameters that should be inferred from the current OpenFHE implementation or from a different HEaaN release.

There is one narrower confirmed fact about encryption: Section 2.1 defines the paper's abstract `Enc` algorithm as public-key encryption, with

\[
ct=[v\cdot pk+(m+e_0,e_1)]_{Q_L},\qquad
v\leftarrow\chi_{enc},\ e_0,e_1\leftarrow\chi_{err}.
\]

That definition does **not** say that the Section 6.3 proof-of-concept invoked a public-key HEaaN API rather than a secret-key encryption API, nor does it instantiate either distribution.

## Exact paper source and version

- Supplied archive: `artifacts/handoffs/s100-fresh-error-repair-01/s100-fresh-error-repair-e6c4cc1.zip`, SHA-256 `58bdc872577c3e267dd2f1fe823bf1751242abf5520e211cceed2834be6e14aa`. This task independently recomputed the archive hash on 2026-09-07; it was not accepted solely from the prior manifest/report.
- Supplied member: `references/paper/PAPER-2023-1788.pdf`, SHA-256 `61d9b948b17b6a624d3bf3372462555288308011226d2893e9e6bc3d6d197eac`. This task streamed the member from the archive and recomputed its hash on 2026-09-07.
- The supplied PDF is byte-for-byte identical to both the [current IACR PDF](https://eprint.iacr.org/2023/1788.pdf) and IACR's sole archived PDF (`20231120:043222`, file timestamp URL `1700454742`), each with the same SHA-256. This task streamed and hashed both remote PDFs on 2026-09-07. IACR's [version history](https://eprint.iacr.org/archive/versions/2023/1788) listed only that one PDF update when checked on 2026-09-07.
- The [IACR record](https://eprint.iacr.org/2023/1788) listed PDF as the only available format and contained no code, artifact, data, supplement, or repository link when checked on 2026-09-07. It records receipt on 2023-11-20 and approval on 2023-11-24.
- Text inspection used the supplied `PAPER-2023-1788.txt` with binary-safe search because it contains a NUL byte. A normal `rg` search that stops at binary content is not evidence of absence.

## Confirmed by the exact paper

### Section 2.1 (generic CKKS description)

- Setup returns \(N,\Delta,h,Q_0,\ldots,Q_L,P,\chi_{enc},\chi_{err},B_{Dec}\).
- The secret polynomial has coefficients in \(\{-1,0,1\}\) and Hamming weight \(h\), sampled from a "prescribed distribution" that is not specified further.
- Public-key generation samples uniform \(a\in R_{Q_L}\) and \(e\leftarrow\chi_{err}\).
- Abstract encryption samples \(v\leftarrow\chi_{enc}\) and \(e_0,e_1\leftarrow\chi_{err}\) and uses `pk` in the formula quoted above.
- Section 2.1 never gives the support/weights of \(\chi_{enc}\), the family or numeric parameters of \(\chi_{err}\), or a HEaaN release/commit from which defaults could be recovered.

### Section 6 / Section 6.3 / Table 3 (experiment)

- The proof of concept was developed on the C++ CryptoLab HEaaN library and ran on Linux on an Intel Xeon Gold 6242 at 2.8 GHz with 503 GiB RAM.
- The computation used eight repeated squarings on one ciphertext; the reported infinity-norm errors were averaged over 1,000 executions. The user has removed 1,000 trials as a reproduction requirement, but this remains provenance for the paper's values.
- The high-precision setting used 100-bit plaintext/scale precision. For Mult2/Table 3: \(N=2^{15}\), \(h=128\), \(d_{num}=11\), \(\log_2(Q_LP)\approx680\), paired 50-bit base primes, eight 60-bit multiplication primes, a 40-bit division prime, and a 60-bit auxiliary prime. The reported precision after eight squarings was -81.8 bits.
- Section 6.3 does **not** state the plaintext/message values or a sampling law. In particular, it does not state real versus complex slots, populated slot count, range, fixed versus resampled messages, or whether the 1,000 executions resampled messages, keys, encryption noise, or some subset.
- Section 6.3 does **not** instantiate \(\chi_{enc}\) or \(\chi_{err}\), give a Gaussian standard deviation/tail bound, or identify the exact HEaaN source/package version.
- Section 6.3 says only "a single ciphertext" and does not identify the encryption call/path. Consequently, Section 2.1's public-key `Enc` formula supports the paper's abstract scheme description but is insufficient provenance for the experiment's public-versus-secret-key encryption mode.

## Bounded first-party artifact search

The following public first-party surfaces were inspected in this research turn on 2026-09-07. They are live observations as of that date, not claims that the same state held on 2023-11-20:

- IACR's paper record and sole version, described above.
- Jaehyung Kim's [official publication page](https://jaehyungkim0.github.io/publications/) links this work only to its ePrint. Wonhee Cho's [official publication page](https://wony0404.github.io/publications/) links it only to the ACM paper.
- The public repository inventories for [Jaehyung Kim](https://api.github.com/users/jaehyungkim0/repos?per_page=100), [Wonhee Cho](https://api.github.com/users/wony0404/repos?per_page=100), and [CryptoLab Inc.](https://api.github.com/orgs/CryptoLabInc/repos?per_page=100&type=public) contained no repository identified as this paper's artifact. GitHub repository searches for the [exact title](https://api.github.com/search/repositories?q=%22Homomorphic+Multiple+Precision+Multiplication%22), [`Double-CKKS`](https://api.github.com/search/repositories?q=Double-CKKS), and the [ACM article number](https://api.github.com/search/repositories?q=3623086) returned zero repositories. Negative repository searches are a bounded observation, not proof that no artifact has ever existed.
- The paper footnote URL, `https://www.heaan.it/`, now redirects to `https://ckks.org/software//` (HTTP 301) and serves the current [CKKS.org software listing](https://ckks.org/software/), whose response was last modified 2026-08-24. It links HEaaN to the current [heaan.io](https://heaan.io/) product site. The former `heaan.it/docs/heaan/` path now redirects to a 404. These current/migrated pages neither identify the 2023 build nor link an artifact for paper 2023/1788.

Current or unrelated HEaaN/HEAAN documentation and source may reveal defaults for *their own* releases. Without an explicit version or artifact link from paper 2023/1788, those defaults cannot be promoted to facts about the Table 3 experiment. In particular, a modern documentation statement about a ternary distribution used for a security estimate does not identify Table 3's \(\chi_{enc}\), \(\chi_{err}\), or encryption mode.

## Reproduction implication

The retained clean-room OpenFHE report `GREEN2_RESULT.md` is correctly bounded: it records one actual public encryption under an explicitly recorded OpenFHE profile and does not claim exact HEaaN equivalence. This provenance task inspected that retained report but did not rerun its cryptographic tests. The paper's `h=128` does not, by itself, determine \(\chi_{enc}\) or \(\chi_{err}\), and it does not justify silently changing OpenFHE's encryption mode, plaintext family, or sampler parameters to chase the paper's -81.8-bit value.

Until stronger provenance appears, any selected input family and encryption/noise configuration must be labeled as an explicit OpenFHE reproduction condition rather than "the paper's distribution/settings."

## One actionable next step

With explicit user authorization for external contact, send the paper's corresponding authors one concise reproducibility request asking for: (1) Section 6.3 message-generation code/range and slot layout; (2) the exact HEaaN package/release/commit; (3) concrete \(\chi_{enc}\) and \(\chi_{err}\), including numeric parameters; and (4) whether the initial ciphertext used public-key or secret-key encryption and what was resampled across executions. No contact was made in this task.

## Search stopping criterion

This bounded search stops after checking the exact sole IACR version and all of its links, the full binary-safe paper text, the two discoverable author-controlled publication pages, the discoverable author/CryptoLab public GitHub inventories plus exact-title/identifier repository searches, and the paper footnote's current redirect target. Resume only if a primary source explicitly tied to paper 2023/1788/Table 3 appears (artifact, source/config, versioned supplementary material, or an author response). Generic CKKS descriptions or unlinked HEaaN versions cannot close these provenance gaps.
