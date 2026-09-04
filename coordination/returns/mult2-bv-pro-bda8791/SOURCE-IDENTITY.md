# Source identity and evidence boundary

## Verified archive identity

| Item | Verified value |
|---|---|
| Input archive | `mult2-bv-diagnosis-bda8791.zip` |
| Bytes | `1024988` |
| SHA-256 | `cfd92c7181253a92dc0e8fac2a7b18bafba19eadb2fbe86416d638963eb0009d` |
| Root manifest SHA-256 | `d68de3d44065fdbe966cc1f6060ee33cb6b86219d537a9b680844a9a3e1ad7e2` |
| Project source commit | `bda879104c8a8b1ba6ac9301385b5b1919bef440` |
| Branch | `codex/mult2-01` |
| Pinned OpenFHE commit | `df495ba2e91739a6dc8f1de254fc5a41155ce504` |
| Manifest-bound payload count | `51` |
| ZIP regular-file count including manifest | `52` |
| Project files | `30` |
| Pinned official-reference files | `16` |
| Supplemental logs | `2` |
| Root payloads (`TASK`, paper PDF/text) | `3` |

The root manifest's 51 paths exactly matched the non-manifest regular files in the ZIP. Every listed byte count and SHA-256 matched the extracted file. No additional unlisted payload file, missing payload, unsafe path, or symlink was found. `unzip -t` reported no compressed-data error. The machine-readable retained output is `evidence/source-identity-checks.txt`; the exact input manifest is copied as `evidence/input-SOURCE-MANIFEST.json`.

## Git and source boundary

The review treats the 30 `project/` files as the exact selected archive for commit `bda879104c8a8b1ba6ac9301385b5b1919bef440`. It does not assume access to another checkout, another branch, another conversation, a modified OpenFHE tree, or a prior implementation. The only final candidate source change is test-only and is supplied as ordered patches against that exact project root.

The official files are treated as pinned references for OpenFHE commit `df495ba2e91739a6dc8f1de254fc5a41155ce504`; no upstream mutable page or fork was substituted.

## CI evidence boundary

### Retained Linux diagnostic

- Run: `33840176712`
- Job: `100920696884`
- Result: `42/44`; only BV REAL and BV COMPLEX fail at the old certificate assertion
- Source: `evidence/bv-diagnostic-linux.txt`

### Authoritative Windows update

- Run: `33839781546`
- Job: `100919538008`
- Result: `42/44`
- Total test time: `1.18 s`
- Same two BV failures
- Source: `evidence/matrix-red-windows.txt`

These are supplied retained logs. They were inspected but not produced or rerun in this review environment.

## Security-scan and external-review status

The request states that Gitleaks 8.30.1 stage/archive/fresh-manifest checks passed. That status is recorded as **user-supplied evidence**; Gitleaks was not independently rerun here.

The request and packet state that the previous and fresh Fable 5.1 attempts returned HTTP 403 before inference. Therefore:

```text
successful_Fable_review = false
```

No Fable conclusion is inherited or represented as technical evidence.

## Paper inspection

The supplied PDF and text were both inspected. PDF pages covering ordinary relinearization, Definition 4.3, Lemma 4.4, its proof, and Theorem 4.8 were rendered and visually checked against the extracted text. No external paper copy was substituted.
