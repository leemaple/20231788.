# RECONSTRUCTED precision contract patches

These are newly drafted replacements because the prior delivered artifact
contained no patch or final-source bytes. They are not claimed byte-identical to
the absent prior candidate.

Apply either:

1. patch 0001 and observe the genuine hosted red, then patch 0002 and observe the
   unchanged-contract hosted result; or
2. `candidate-final.patch` once on a fresh exact baseline.

Do not apply the aggregate patch after the numbered series.

The exact base is `bda879104c8a8b1ba6ac9301385b5b1919bef440` selected source.
Patch 0002 changes only `tests/precision_dcp_rcb_fixture.cpp`. No production file
or public API is changed. See `../PATCH_SERIES_AND_CONTINUITY.md` and the static
verifier for hashes and continuity evidence.
