# Dispatch-record scan disposition

The initial staged-diff gitleaks8.30.1 scan found one generic-api-key match
on the newly authored prompt's scanner-version sentence (diff line871).
The matched value is the public scanner version label gitleaks8.30.1, not
an authentication token. No credential store or browser state was read.
The exact sent prompt is preserved, rather than rewriting its archival bytes.

A one-run temporary scanner configuration extends all default rules and
allows ONLY the fully anchored exact match consisting of the scanner-version
sentence fragment. No path, file, source tree or credential rule is disabled.
The temporary configuration is retained at
/private/tmp/repeated-mult2-pro-handoff.azsrhb/dispatch-scan-false-positive.toml.
The source/ZIP payload scans had already passed with unmodified defaults.
This disposition does not convert the first scan into a clean result; the
reconciled rerun must complete successfully before the dispatch commit/push.
