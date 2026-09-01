# Fable5 review — Relin2 Tensor validation slice at 84df651

## Role and exact decision

Act as an independent, read-only algorithm/API/TDD reviewer. Review the exact
clean-room OpenFHE 1.5.0 project state described below and answer two questions:

1. Does the retained red followed by commit `84df6518...` correctly establish
   that `DoubleCKKS::Relin2` validates the complete Tensor2 result before any
   future evaluation-key lookup or arithmetic, without implementing later
   behavior early?
2. Which single next runtime behavior should be implemented as the smallest
   independent red/green slice: the Relin2-only insufficient-active-basis
   rejection or the valid-Tensor missing-evaluation-key rejection? Give the
   exact test observation and maximum permitted production scope for that one
   slice.

Return one final Markdown answer. Do not edit files or return a patch.

## Background and objective

The project is a from-scratch implementation of the `t=2` double-precision
CKKS multiplication method in paper 2023/1788, built on official pristine
OpenFHE 1.5.0. The current implementation already has accepted DCP, RCB, and
Tensor2 behavior. Relin2 is being built through narrow vertical TDD slices so
that every nontrivial behavior has an independently retained red before its
minimal green.

The accepted Relin2 public seam is:

```cpp
CiphertextPair Relin2(const TensorCiphertextPair& tensor) const;
```

The current valid-input path deliberately remains:

```cpp
ValidateTensorResult(tensor);
throw std::logic_error("DoubleCKKS: Relin2 is not implemented");
```

This review is not an invitation to design or implement the full Relin2 core.

## Exact source and evidence identity

- implementation branch: `agent/codex-relin2-01`;
- current commit: `84df6518df47fc7e50b8f465e5aa294fe5fdf84d`;
- current tree: `028033ad13b90710eaa98e6f1436bdb2d8f49b86`;
- parent red commit: `f2deacbb9b1f1a291d91b6d9ac9eec5f363f0082`;
- accepted scaffold parent: `6f1645b97ce5b2175530cde5bfd0929370997634`;
- official pristine OpenFHE commit used by CI:
  `df495ba2e91739a6dc8f1de254fc5a41155ce504`.

Red run `33531734269`, Linux job `99936280030`:

- warning-clean default build passed;
- compile-only public API contract passed;
- all six inherited DCP/RCB/Tensor2 CTests passed;
- only `relin2_tensor_validation_order` failed because the scaffold threw
  `std::logic_error("DoubleCKKS: Relin2 is not implemented")` instead of the
  required exact invalid-argument diagnostic;
- Windows was cancelled during pristine OpenFHE build; no Windows claim.

Green run `33532645418`, Linux job `99939294149`:

- warning-clean default build and public API contract passed;
- exact `7/7` CTests passed, including the new Relin2 case;
- Windows was cancelled during pristine OpenFHE build; no Windows claim.

The raw receipts and exact source are present in this packet. Treat claims not
supported by those bytes as pending.

### Exact detached packet binding

The review ZIP is external to this task so this task can bind its final bytes
without a self-reference cycle:

- file: `fable5-relin2-validation-84df651-review-packet.zip`;
- size: exactly `845608` bytes;
- SHA-256:
  `98e51fa1a020bb49a97927282da4e767c6d85be165da79d9e5b794576f2eb19e`;
- central-directory entries: exactly `28`, all unencrypted regular `100644`
  files, with no duplicate or unsafe path;
- ordered member-list SHA-256:
  `1026987f2d5ad0c5cb8c66eee559bdf11ab4cf7469acf43161fbbc6656529303`;
- internal `MANIFEST.sha256`: exactly `27` entries, SHA-256
  `a00551b1faf346c824181b507a6ff92d370c869ae3b80c63c4a0b07808a38608`;
- staging and fresh-extraction Gitleaks 8.30.1: exit `0`, no findings;
- both targeted credential scans: no matches;
- fresh extraction: internal manifest check passed, byte-identical to staging,
  no symlinks, and zero writable file or directory entries under an exact
  `(mode & 0222) == 0` audit.

The exact 28 members are:

1. `MANIFEST.sha256`
2. `PACKET_SCOPE.md`
3. `evidence/green/84df651.diff`
4. `evidence/green/coordination-receipt.md`
5. `evidence/green/hosted-receipt.md`
6. `evidence/green/jobs-terminal.json`
7. `evidence/green/linux-job.log`
8. `evidence/green/run-terminal.json`
9. `evidence/red/coordination-receipt.md`
10. `evidence/red/f2deacb.diff`
11. `evidence/red/hosted-receipt.md`
12. `evidence/red/jobs-terminal.json`
13. `evidence/red/linux-job.log`
14. `evidence/red/run-terminal.json`
15. `paper/2023.1788.pdf`
16. `paper/2023.1788.txt`
17. `source/.github/workflows/dcp-rcb.yml`
18. `source/CMakeLists.txt`
19. `source/README.md`
20. `source/include/openfhe_2023_1788/double_ckks.h`
21. `source/src/double_ckks.cpp`
22. `source/tests/dcp_rcb_test.cpp`
23. `source/tests/relin2_api_contract_test.cpp`
24. `source/tests/relin2_test.cpp`
25. `source/tests/tensor2_api_contract_test.cpp`
26. `source/tests/tensor2_test.cpp`
27. `spec/original-relin2-contract.md`
28. `spec/relin2-preflight.md`

Before the provider process starts, the parent must verify the ZIP size/hash,
exact 28-member inventory, internal manifest hash, `shasum -a 256 --check
MANIFEST.sha256`, staging/extraction equality, read-only modes, and scans above.
Those raw parent-side checks belong in the receipt. As reviewer, use
Read/Glob/Grep only to confirm that every listed packet path is present and
readable; no shell tool is available. If a listed member is absent or
unreadable, return `BLOCKED` with the exact mismatch and make no source verdict.
The final detached task size/SHA is deliberately recorded by the launcher and
receipt rather than self-declared inside this file.

## Current architecture and boundaries

- Upstream OpenFHE source is unchanged and must remain unchanged.
- `TensorCiphertextPair` is the distinct three-component input type.
- `ValidateTensorResult` validates bound context, divisor, three-component
  evaluation shape, level-one ordered prefix basis, exact Tensor2 recorded
  factor/noise degree/key tag, both paper logical scales, and both ciphertext
  members.
- Relin2 must validate the complete Tensor input before non-validation raw
  member access, evaluation-key lookup, cloning, tower construction, or
  arithmetic. The validator itself necessarily reads members to validate them.
- The current test does not call `EvalMultKeyGen`; it corrupts only
  `approximateHighLogicalScalingFactor` and requires full-string equality with
  `DoubleCKKS: Tensor2 result paper-scale descriptor is inconsistent`.
- The next insufficient-basis behavior is Relin2-specific: a degree-three
  Tensor needs at least three active `Q_l` towers before raising; it must reject
  earlier than key lookup.
- The next missing-key behavior uses an otherwise valid Tensor and must reject
  a missing exact key-tag entry with a project-owned diagnostic before
  arithmetic.
- Every next negative case is already constrained to `std::invalid_argument`,
  the exact `DoubleCKKS: ` prefix, and a stable field-specific full message.
  You may recommend the message suffix, but not change the exception type,
  prefix, or field-specific requirement.
- KISS, YAGNI, fail-fast behavior, no speculative try/catch, and test-first
  boundaries are mandatory.

## Review scope

Inspect every packet file relevant to these questions, especially:

- `source/include/openfhe_2023_1788/double_ckks.h`;
- `source/src/double_ckks.cpp`;
- `source/tests/relin2_test.cpp`;
- `source/CMakeLists.txt`;
- `spec/relin2-preflight.md`;
- the two coordination receipts and their evidence-side receipts;
- the paper PDF/text where needed for algorithm context.

Check exception type and exact diagnostic matching, fixture reachability,
absence of evaluation-key generation in the red/green test, validation order,
scope containment, and whether the proposed next behavior can be made red and
green without implementing a later behavior.

Do not inspect any path outside the extracted packet. Do not use network,
browser, shell/code execution, build, test, write, edit, Git mutation, or
delegation tools. Read, Glob, and Grep are the only available tools. Archive
identity and manifest execution are parent-side pre-launch gates, not reviewer
tool calls.

## Exact Terminal launch and receipt contract

- fixed CLI binary: `/Users/lifeng/.local/share/claude/versions/2.1.239`;
- CLI version/hash: `2.1.239 (Claude Code)` /
  `2b4f7aafdaa65bcc2335f56a4b276317837203f2c5587b1f2a17ca78ad14e36f`;
- fixed credential transporter/scanner: `/usr/bin/python3`, exact version
  `Python 3.9.6`, SHA-256
  `a961f78075d8e7621ef4f5d764c64ef8a41bf66c0a98ab5cb6ca39b85ce31c93`;
- model: exact `claude-fable-5`, effort `max`, with no fallback model;
- exact CWD:
  `/private/var/tmp/fable5-relin2-84df651.ZLu6Uh/fresh-extraction`;
- noninteractive print mode, `plan` permissions, safe mode, no Chrome, no
  session persistence, slash commands disabled, prompt suggestions disabled,
  strict empty MCP configuration, and a bounded API budget;
- tools: only Read, Glob, and Grep; shell/Bash, edit/write, web/browser, and
  delegation tools are absent;
- exact audited launcher:
  `coordination/handoffs/fable5-relin2-validation-84df651-review-01/launch.sh`;
  its final SHA-256 is bound by the clean coordination commit and raw parent
  receipt, while the launcher hardcodes and checks this task's final SHA-256;
- exact Apple sandbox profile:
  `coordination/handoffs/fable5-relin2-validation-84df651-review-01/sandbox.sb`,
  SHA-256
  `680fec15a149b95801cbc8dccf377661f5c756f28336e8565ce268359b8640b8`;
- the sandbox denies all filesystem reads and writes, then permits only the
  exact CLI/task/packet and isolated runtime TMP plus narrowly enumerated
  read-only macOS runtime binaries, libraries, root certificates, hosts, and
  resolver files. The isolated runtime HOME is child-write-only. The profile
  denies the real user configuration, user/system credential stores,
  browser-state, unrelated workspace, and broad `/Library`, `/private/etc`,
  and `/private/var/db` reads. It explicitly denies `/System/Volumes/Data` and
  probes the corresponding user-config, browser, and system-keychain aliases.
  It also denies the enumerated macOS Security/Keychain Mach services and
  requires a sandboxed `security list-keychains` negative canary. Before those
  denials are trusted, the parent proves every named sensitive path is present
  and readable and proves the same `env -i` keychain command succeeds without
  the sandbox; the receipt records both positive controls without file content;
- before entering the sandbox, the launcher retrieves the existing OAuth
  access and refresh tokens from macOS Keychain. It transports the values to a
  fixed-hash Python child over an anonymous NUL-delimited pipe, not process
  arguments. Python runs with `env -i -I -S -E`, reads no user startup/site
  code, constructs an exact seven-name environment containing only
  `CLAUDE_CODE_OAUTH_TOKEN`, `CLAUDE_CODE_OAUTH_REFRESH_TOKEN`, HOME, TMPDIR,
  PATH, LANG, and LC_ALL, mechanically checks that name set, then directly
  `execve`s the sandbox wrapper. The launcher never prints, stages, packages,
  or deliberately writes either value, and no parent API key, proxy, hook,
  socket, shell-startup, or config-path variable is inherited. The same
  isolated/no-site Python performs suppressed exact-byte scans that reject
  unreadable, symlinked, or non-regular entries. Those scans of isolated HOME,
  TMP, and the receipt must pass before auth, after auth, after flag parsing,
  and again immediately after the provider exits, before the parent clears its
  in-memory copies;
- the launcher is the sole preflight-and-capture wrapper. It creates a unique
  attempt receipt, opens `00-preflight.txt` plus a dedicated wrapper-error
  record, and prints that safe receipt path before checking canonical CWD, a
  prior provider guard, or the atomic entry lock. Explicit failures and a
  phase/step-aware exit trap append their reason, phase, step, and exact exit
  before the wrapper terminates. Git remote checks retain safe stdout, stderr,
  and exact exits; sandbox probing retains its observed matrix, stderr, and
  exit before comparison; Keychain stdout is never persisted, while its
  stderr/exit and every credential-JSON parse stderr/exit are retained and
  exact-token-scanned. The entry lock is acquired before any shared
  HOME/TMP/auth activity. Before a distinct
  one-use provider-start guard it fail-closes on exact
  task/CLI/profile/ZIP/manifest and source identities, Git cleanliness/remote
  equality, member inventory/modes, staging/extraction equality, Gitleaks,
  targeted and exact-value scans, canonical CWD, exact flag parsing,
  authenticated method `oauth_token`, allowed packet/task reads, denied real
  config/browser/system-keychain and Keychain-service access, denied packet
  writes, and an allowed isolated-TMP write. Isolated HOME and TMP must start
  empty at their exact realpaths, with no startup file, symlink, or non-regular
  entry. Auth and flag-probe stdout, stderr, and exact exit are retained before
  the provider guard. These startup probes do not contact the Fable provider;
- this detached task is opened once without symlink following by the fixed
  transporter, verified for the frozen size/SHA on that same descriptor, then
  rewound and supplied on provider standard input;
- after the guard is created exactly once, the wrapper starts one provider
  process; verbose `stream-json` stdout and stderr are captured byte-for-byte
  outside the packet, with exit/start/end and post-identities retained, then
  the terminal answer is parsed without editing it. A successful exit is
  accepted only for one nonempty `success`/non-error result, one session, no
  permission denial, exact emitted `claude-fable-5` identity, and at least one
  fixed-schema Read/Glob/Grep call. Every required/optional field is checked for
  allowed key, type, numeric range, boolean type, or enum as applicable; every
  path/pattern field rejects parent traversal and expansion syntax and is
  lexically confined to the extraction root. Every counted tool call must have
  exactly one successful same-session tool result, and vice versa. Every event
  carrying a session or model field must match the accepted identity, every
  assistant must explicitly carry the exact model, and every present
  `modelUsage` value must be a nonempty object keyed only by that model. The
  unique init must report the exact extraction CWD, `plan` permission mode,
  exactly Read/Glob/Grep, and an empty MCP-server list. Each matched tool result
  must carry protocol content of string or array type; an explicit null
  tool-result error field is rejected rather than treated as success;
- the parent receipt binds start/end time, CLI/task/packet identities, raw
  exits, output hashes, session/usage fields when present, and observed tool
  calls/paths. After provider exit it reruns the frozen packet
  inventory/mode/encryption/manifest/staging gates and requires unchanged
  launcher, repository HEAD/tree/upstream/remote, and source identities. The
  final receipt must be exactly the frozen top-level regular-file set, with no
  extra directory, symlink, nested path, or other entry;
- a slow response must run naturally. Do not interrupt, resend, resume, or
  start a duplicate request. A transport/startup failure is recorded exactly
  and does not block local TDD work.

## Required deliverable

Return exactly these sections:

1. `Verdict`: `PASS`, `CHANGES NEEDED`, or `BLOCKED` for the current slice.
   Use `BLOCKED` only for missing/mismatched packet identity or inability to
   inspect the provided bytes; do not guess through an identity failure.
2. `Observed findings`: each with severity `P1`, `P2`, or `P3`, exact packet
   file and line, why it matters, and the smallest correction. Write `none` if
   there are no findings. For `BLOCKED`, report only the identity/readability
   problem here and do not make a source verdict.
3. `Next single slice`: choose exactly one of `insufficient active basis` or
   `missing evaluation key`; state why it is the smaller dependency-ordered
   behavior. For `BLOCKED`, write `not assessed`.
4. `Next red contract`: exact CTest name, fixture preconditions, exception
   type fixed to `std::invalid_argument`, full exact diagnostic with the fixed
   `DoubleCKKS: ` prefix and stable field-specific suffix you recommend, and
   the observation proving the current `84df651` state fails for the intended
   reason. For `BLOCKED`, write `not assessed`.
5. `Maximum green scope`: enumerate what the minimal production change may do
   and what valid-input not-implemented seam must remain afterward. For
   `BLOCKED`, write `not assessed`.
6. `Claims`: separate observed, inferred, and pending facts; explicitly state
   that you did not build, test, edit, access Windows, or verify an unprovided
   upstream OpenFHE checkout.

## Acceptance criteria

- For `PASS` or `CHANGES NEEDED`, every current-slice conclusion cites packet
  source/test/evidence bytes; for `BLOCKED`, only the exact identity or
  readability failure and the required claims are assessed.
- For `PASS` or `CHANGES NEEDED`, the chosen next slice is one independently
  observable behavior, not a bundle.
- For `PASS` or `CHANGES NEEDED`, its red can run against `84df651` without
  test-only friendship, setters, or production changes.
- For `PASS` or `CHANGES NEEDED`, its minimal green does not access or implement
  any behavior that belongs to a later slice.
- No unsupported Linux, Windows, Fable-provider, precision, performance, or
  full-Relin2 claim is made.
