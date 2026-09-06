# Decoded log transport

Both source commands were `gh api repos/leemaple/20231788./actions/jobs/<job>/logs`, exit0. Full output was concatenated across tool chunks; an earlier truncated Linux preview was not used as evidence. No CI rerun occurred.

- Linux job101502439156: retained decoded UTF-8/BOM log987346 bytes, SHA256 `ba76bd3b955fb6bd5e022dcd1d1d6aeb6a8ac657ea8188cef75ab48c0d9d016c`.
- Windows job101502439304: original decoded UTF-8/BOM output1006484 bytes, 8151 CRLFs, SHA256 `2a98a4889302bc911348ed8a6c9b056611d8ec3337c33730f145663ad71ae87f`. The tracked LF-normalized copy998333 bytes has SHA256 `92201a4bb4a628a330df2d11f2bbb1ce885d651e4ddee946fcdc9284d7754384`. Root actually checked original.replace(CRLF,LF)==tracked bytes; all other content including BOM is preserved. Original decoded capture JSON remains ignored under artifacts/handoffs/fs-endpoint-live-run-01/windows-log-capture.json.

The selected CTest61 payload byte/hash fields are derived from these job logs. They do not claim the original runner primary.ctest.log's file identity, and no synthetic BEGIN/COMPLETE framing was inserted.
