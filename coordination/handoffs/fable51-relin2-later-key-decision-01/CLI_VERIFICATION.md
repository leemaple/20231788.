# Claude Code 2.1.258 verification

The wrapper and native metadata files beside this document were fetched from
the official npm registry over HTTPS. Their relevant immutable fields are:

```text
@anthropic-ai/claude-code  2.1.258
tarball=https://registry.npmjs.org/@anthropic-ai/claude-code/-/claude-code-2.1.258.tgz
integrity=sha512-Zis1AYrHuCcK4V1tXJUkzJdklFsTvvqIcj7gk4K8lyEeJOW99ZoQH/E+WxugMqDEO7xncYZ41gydxlkSTmj/2Q==
shasum=55f84789ed34ca70702c043e7bcba88dde2daea3

@anthropic-ai/claude-code-darwin-arm64  2.1.258
tarball=https://registry.npmjs.org/@anthropic-ai/claude-code-darwin-arm64/-/claude-code-darwin-arm64-2.1.258.tgz
integrity=sha512-9gYzEZjYLY/3N49jkgk9BeXPGDQNCE/6pO3/tFHJP054YnXCvKcyjWbx1M5ISgiKnEtmTRDXnAX4cXe1SGZ+Eg==
shasum=bd420beada4c146a60e4c5859da3361151789eaa
```

The downloaded native tarball was 86,968,654 bytes. This command produced the
registry value above exactly:

```sh
openssl dgst -sha512 -binary claude-code-darwin-arm64-2.1.258.tgz | openssl base64 -A
```

The extracted executable verification was:

```text
file: Mach-O 64-bit executable arm64
bytes: 199027600
mode: -rwxr-xr-x
SHA-256: b63136194160791c27cfa7b0403060d85eb0752991625fde8c09f9acacb17c78
codesign --verify --deep --strict: valid on disk; satisfies its Designated Requirement
Identifier=com.anthropic.claude-code
Hash type=sha256 size=32
CDHash=de33e9f3cf698e24fa5be31f9435dde9d8cde5fb
Authority=Developer ID Application: Anthropic PBC (Q6L2SF6YDW)
TeamIdentifier=Q6L2SF6YDW
version=2.1.258 (Claude Code)
```

The executable and tarballs are not retained in Git. The registry metadata is
retained, and the exact executable/tarball identities above bind the launch.
