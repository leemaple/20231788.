# Evidence verification

## Request archive

Retained results in `scans/zip-verification-summary.txt`:

```text
zip_bytes=105324
zip_sha256=4454f15e34bc48f73f23af8321ae7e26e315a42637447ccfb066b31d98fb2dcf
member_count=26
duplicate_count=0
unsafe_path_count=0
encrypted_count=0
filename_scan_exit=1
content_scan_exit=1
fresh_manifest_exit=0
unzip_test_exit=0
```

For both `rg` scans, exit 1 means no match. The filename scan covered common
environment, private-key, browser-state, credential, token, and secret names.
The content scan covered bearer authorization, private-key headers, and
credential-like API/access/refresh-token assignments. Matching text was never
retained.

The fresh manifest check used:

```sh
verify_dir=$(mktemp -d /tmp/relin2-fable51-retained-verify.XXXXXX)
unzip -q request.zip -d "$verify_dir"
(cd "$verify_dir" && shasum -a 256 -c PACKET_MANIFEST.sha256)
```

`request.zip` was also scanned directly with Gitleaks 8.30.1 archive traversal:

```sh
gitleaks dir --no-banner --no-color --redact --max-archive-depth 2 \
  --report-format json --report-path scans/retained-zip-gitleaks.json \
  request.zip
```

Exit was 0 and the retained JSON report is `[]`. The original stage and fresh
extraction reports are also retained; each is `[]`, and their stderr records
Gitleaks scanning approximately 599.58 KB with no leaks.

The retained packet `TASK.md` hashes to `a95239ab...`, exactly the first row of
`PACKET_MANIFEST.sha256`. The coordination task file acquired a status/link
annotation only after the run. It is not claimed to be the byte-identical
request; the retained handoff `TASK.md` is the request identity.

## Model output and file-tool boundary

The accepted raw stream is 1,541,999 bytes with SHA-256
`5cf7fdf279cc6eda4306b7bb53511d368e1955390b58df0af022e954c19c4e03`.
The extracted answer SHA-256 is
`56fc4e76deff61b0d58864ba29ba1f74cb877c354b0b1c9a7b6d23210baba98f`.
Stderr is zero bytes with the standard empty-file SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.

Tool calls were extracted without interpretation using:

```sh
jq -r 'select(.type=="assistant") |
  .message.content[]? | select(.type=="tool_use") |
  [.name, (.input.file_path // .input.path // "")] | @tsv' \
  fable51-cleanroom-output.jsonl > tool-calls.tsv
```

There are exactly 16 rows. Every tool name is `Read`; every path begins with
`/private/tmp/relin2-fable51-later-key.M0HtlF/fresh/`; the TSV SHA-256 is
`289a7b837fdd2e7e25542fbdab3848e6d36a6efde123fcb225d2aef43fb5b821`.
The initialization event independently names only `Read`, with no MCP server
or plugin. The final event records zero web search/fetch requests and no
subagents.

Before the ephemeral auth-only settings file was deleted, the exact auth-token
value was searched as a fixed string across stdout and stderr without printing
the value or any match; the contemporaneous observation was no match. A
separate high-specificity credential scan had the same observed result. Those
two supplementary checks did not retain a target hash, predicate, or raw exit,
so they are not independently replayable evidence and are not part of the
mechanically verified clean-scan claim below. The retained, replayable output
scan was:

```sh
gitleaks dir --no-banner --no-color --redact \
  --report-format json \
  --report-path scans/fable51-cleanroom-output-gitleaks.json \
  fable51-cleanroom-output.jsonl
```

Exit was 0 and the retained report is `[]`.

## Retained-file manifest

The directory contains 39 files: 38 evidence files plus `FILES.sha256`. The
manifest verifies every non-manifest file. Its final hash is recorded outside
this manifest-covered directory in the coordination review receipt, avoiding a
circular self-reference. The retained `final-retained-gitleaks.*` triplet is a
defence-in-depth scan produced while this directory was being assembled; it is
not evidence that the byte-final directory recursively scanned itself. The
authoritative mechanically bound scan evidence is instead the immutable request
ZIP, its fresh extraction, the pre-run stage, and the accepted raw JSONL, each
with a retained report/stdout/stderr triplet or exact summarized raw exits.
