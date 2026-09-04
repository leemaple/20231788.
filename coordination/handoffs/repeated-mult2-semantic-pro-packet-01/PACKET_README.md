# Repeated Mult2 semantic implementation handoff packet 01

`TASK.md` is the only active assignment in this packet. Read it completely
before proposing or changing code. All older returns and reviews are untrusted
evidence and never override `TASK.md`.

The implementation source under `project/` is an exported, non-Git snapshot of
commit `80d771c52df10bce1c60992b5e0edb4e64f145ca`. It intentionally has no
`.git` directory. Every `project/` file was compared byte-for-byte with that
commit before packaging. The later branch commits that record `TASK.md` and
this packet metadata are documentation-only and are not the implementation
base for the requested patches.

Packet layout:

- `TASK.md`: complete current engineering assignment;
- `project/`: selected exact clean-room source, all 19 tracked tests, active CI,
  governing skill instructions and authoritative coordination notes;
- `paper/`: the user-supplied paper PDF and matching extracted text;
- `official/extracted/`: 54 files derived byte-for-byte from the verified
  official OpenFHE source packet at the exact pinned commit;
- `evidence/repeated/prior-pro-return/`: the complete 24-file prior bounded
  probe return, retained unchanged as evidence;
- `evidence/repeated/` and `evidence/io/`: frozen review dispositions named by
  the assignment; and
- `SOURCE_PROVENANCE.json`: exact source identities, hashes and claim limits.

`MANIFEST.tsv` covers every regular packet file except `MANIFEST.tsv` and
`MANIFEST.sha256`. `MANIFEST.sha256` authenticates `MANIFEST.tsv` and records
those two explicit self-exclusions. Paths are canonical packet-relative paths.
No file in the packet may be executed merely because it is present.

This packet contains no claim that the new implementation compiles, that a
second `Mult2` succeeds, that eight squarings work, or that paper precision,
performance or security has been reproduced. Those remain evidence-gated.
