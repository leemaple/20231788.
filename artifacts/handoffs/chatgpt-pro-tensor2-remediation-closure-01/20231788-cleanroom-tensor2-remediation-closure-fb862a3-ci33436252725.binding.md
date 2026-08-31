# Final package binding — Tensor2 remediation closure

Generated: 2026-09-01 05:15 Asia/Shanghai

This external record was generated only after the final ZIP existed. The ZIP's
internal `HANDOFF_CONTENTS.md` describes the two-record mechanism and contains
all non-self-referential facts.

## Final attachment identity

- ZIP filename:
  `20231788-cleanroom-tensor2-remediation-closure-fb862a3-ci33436252725.zip`
- Final byte size: `9,334,115`.
- Final SHA-256:
  `54c4ab7ad202bfe8cb9c95a58816bd9d30af2132978b75ea3a074e1c4f561832`.
- ZIP central-directory entries: `2,310`.
- Manifest-listed files: `2,037`; total package files including the manifest:
  `2,038`.
- Exact source/test head:
  `fb862a3dfeeb0b79eb8f0e4218749d8a898e96c9`.
- Exact source Git tree:
  `759d5195739684748d5a9664edabe3fa719e1acf`.
- Exact final Actions run: `33436252725`, conclusion `success`; Linux and
  Windows each passed 6/6 CTests.
- Official OpenFHE 1.5.0 commit:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.
- Final-review task SHA-256:
  `71537ff615292b56a701866c22ecc722020d4175f2ba81b4f504364b833680b2`.
- Internal `HANDOFF_CONTENTS.md` SHA-256:
  `ba01c3fd5c8537d75223ae40f28684f85c22ce76f6abd0deade8ae3ebc10f5e3`.
- Internal `MANIFEST.sha256` SHA-256:
  `4fdc1087cf28fa058d013e2ff25f4b20bdb197bbf80a2eaa250bb3e1a2c4f95e`.

## Final verification results

- Pre-archive staged-tree scan: Gitleaks 8.30.1 scanned approximately 26.86 MB
  and reported `no leaks found`.
- Fresh-extraction scan: Gitleaks 8.30.1 scanned approximately 26.86 MB,
  including the manifest, and reported `no leaks found`.
- Archive integrity: `unzip -t` passed with no compressed-data errors.
- Manifest: all 2,037 entries verified successfully after fresh extraction.
- Targeted filename exclusions passed: no `.env`, credential/token JSON,
  PEM/private-key file, Cookie database, or browser `Login Data` file.
- Targeted directory exclusions passed: no `.git`, `node_modules`, generated
  build tree, `CMakeFiles`, cache, or runtime/browser-state directory.
- Entry-path safety passed: no absolute or `..` traversal path.
- Extracted-tree equality passed: recursive staged-versus-fresh comparison
  produced no difference.
- Exact Git-export equality passed: `cleanroom-project/` is byte-identical to
  a fresh `git archive` of tree `759d5195...`.
- The archive has exactly the 18 top-level entries listed by its internal
  handoff shape.

The package deliberately contains no `.git`; it supports auditing exported
diffs/histories and raw execution timestamps but does not independently prove
Git object ancestry or absence of history rewriting.

## Commands used

```sh
git archive --format=tar --prefix=cleanroom-project/ fb862a3...
git diff --binary --full-index 87c84b... fb862a3...
git diff --binary --full-index 55f3b43... fb862a3...
git log --reverse --format=fuller --stat <range>
shasum -a 256 <file>
shasum -a 256 -c MANIFEST.sha256
gitleaks detect --no-git --source <tree> --redact --verbose
zip -r <archive> .
unzip -t <archive>
unzip -q <archive> -d <fresh-directory>
find <tree> <targeted forbidden filename/directory predicates>
diff -qr <staged-tree> <fresh-tree>
diff -qr <cleanroom-project> <fresh-git-archive>
```

Upload this binding, the ZIP, and the standalone task to the same saved
ChatGPT Pro conversation. Any identity mismatch is an input blocker. The
package is for a bounded algorithm/OpenFHE engineering review, not a
network-security assessment.
