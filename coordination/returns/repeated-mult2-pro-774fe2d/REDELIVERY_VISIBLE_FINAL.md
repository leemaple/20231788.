## Delivery repair result

The completed package **exists**. No reconstruction, design revision, source regeneration, patch modification, or scope change was performed.

### Observed original files

| Artifact | Actual path | Size | SHA-256 |
|---|---|---:|---|
| ZIP | `/mnt/data/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip` | **63,963 bytes** | `bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2` |
| SHA sidecar | `/mnt/data/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip.sha256` | **121 bytes** | `8e08b5f7a00f135f0aae238e0466cac9995197b0cec40c72c52a66ac88670619` |

Exact sidecar line:

```text
bee2b27ebf88c901b5b91bc3e79fe386231f07ea580b5228512bf380fdac2fd2  repeated-mult2-bounded-basis-routing-probe-774fe2d.zip
```

The reattached source archive was also confirmed independently:

```text
1,451,817 bytes
efc96137d3412bae57099b6e2f7f85a96bd175b4dd810b587083e1e3d324587d
```

### ZIP and manifest inspection

- ZIP CRC test: **PASS**
- Real ZIP members: **24 nonempty regular files**
- Corrupt members: **0**
- Archive root: `repeated-mult2-bounded-basis-routing-probe-774fe2d/`
- `MANIFEST.json`: **22 payload entries**
- Manifest self-exclusions: `MANIFEST.json`, `MANIFEST.sha256`
- Total manifest coverage: **24/24 files**
- Every recorded payload size and SHA-256: **PASS**
- Duplicate manifest paths: **none**
- `MANIFEST.sha256` verification: **PASS**

`MANIFEST.sha256` contains:

```text
cdeeab6eb0890218386c64cf50838b79a68b220144aed3f62d999264bca7de4b  MANIFEST.json
```

The CTest binding ledger has one header plus 57 bindings: the original **55 preserved bindings** and exactly **2 additive probe bindings**.

### Actual top-level contents

```text
ACTUAL_EXECUTION_LEDGER.md
DESIGN_DECISION.md
EXPECTED_CTEST_BINDINGS.tsv
FROZEN_SECOND_MULT2_CONTRACT.md
MANIFEST.json
MANIFEST.sha256
NEXT_PAPER_GATES.md
PATCH_REPLAY.md
PROBE_ACCEPTANCE.md
README.md
SOURCE_CLAIM_TEST_LEDGER.md
SOURCE_IDENTITY.json
SOURCE_LINE_INDEX.json
complete/
contracts/
evidence/
patches/
tools/
```

The two patches actually present are:

```text
patches/0001-freeze-current-second-mult2-boundary.patch
patches/0002-add-immutable-basis-key-routing-probe.patch
```

The package still identifies itself as **`BOUNDED_PUBLIC_API_PROBE`**. It does not claim C++ compilation, crypto runtime, GitHub Actions execution, semantic second Mult2, `h=128` setup, eight squarings, 1000 trials, performance, paper precision reproduction, or security validation.

## Re-delivered files

I created byte-for-byte redelivery copies under a fresh delivery path. `cmp` and SHA-256 both confirm they are identical to the existing completed artifacts.

[Download the verified completed ZIP](sandbox:/mnt/data/repeated-mult2-delivery-repair/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip)

[Download the matching SHA-256 sidecar](sandbox:/mnt/data/repeated-mult2-delivery-repair/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip.sha256)

Original-path links to the same bytes:

[Original ZIP path](sandbox:/mnt/data/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip)

[Original sidecar path](sandbox:/mnt/data/repeated-mult2-bounded-basis-routing-probe-774fe2d.zip.sha256)
