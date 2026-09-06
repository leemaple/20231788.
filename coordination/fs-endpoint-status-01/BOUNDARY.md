# Endpoint status JSON boundary

Root owns a pure standard-library byte codec in `tests/paper_endpoint_status.py`
and its public-boundary tests. The user delegates ordinary seam choices. This
slice implements section8 of the adopted ENDPOINT_SPEC only: `encode_status`
validates an explicit record against independently supplied expected source,
host/run/attempt and emits canonical JSON; `decode_status` validates bounded
bytes and returns the checked record. No file read/write, CTest invocation,
primary parsing, replay, compression, publication or upload is in this slice.

Required evidence distinctions: COMPLETE status means complete *diagnostic
evidence*, not E80 PASS; finite E80 FAIL preserves actual nonzero CTest status.
Incomplete status has null chain_count and gzip information, and an exact
first-cause reason/state. Reliable numeric count may survive an incomplete
packer result but this codec does not establish that the primary log warrants
it; the future finalizer must supply that evidence.

All fields and enum rules come from the adopted specification. Test fixtures
are pure synthetic bytes with explicit dummy source identity and fake digests,
never files selected for live upload. Root uses actual bounded Python3.12
red/green checks; C++ and full replay remain hosted-only. Tests cover schema,
types, sorted canonical bytes, identity/filename bindings, and complete versus
incomplete null/disposition/status consistency. A codec PASS cannot prove the
actual canonical or compressed data matches its claimed digest.

An incomplete record may preserve a reliable primary E80 observation that
disagrees with the eventual CTest exit (e.g. teardown/signal after COMPLETE PASS,
or a primary/exit integrity contradiction). The finalizer must classify that
failure; it must not erase observed facts to manufacture agreement. Only
COMPLETE status demands E80/count/CTest agreement. This interpretation was
independently challenged and confirmed against section8.
