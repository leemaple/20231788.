# h128 Cycle-B GREEN preflight — 2026-09-05

Real dual-platform B RED acceptance was committed and pushed first at
`4672b3b928623cbfa532a580377259f0d021bb5c`. Its run 33945243915 at source
`43c2dca45c2305c9b6baf50ae1c32529d35e7f06` failed only at the intended public
rejection assertion after successful legacy/build gates.

Then only original `0004-green-paper-h128-client-keypair-guards.patch` was
applied. Original patch: 9,708 bytes, SHA-256
`f5b6604ce760252e2d3862acb483cc9c55046b2db8f18ad2f5eca18c05a69ac7`.
Actual engineering diff: one production source, +125/-0, now 219 lines. Result:
`679d4fa226b95770282fd5c877bf47044a290949bbaed501bd8f662744a6f22e`.

The patch adds fail-fast profile, actual implementation, public basis/table and
fresh-tag checks before key publication. It does not repair/rebuild contexts,
change key generation algebra, clear caches or catch unexpected exceptions.
Its public structural checks are not a theorem about every hidden upstream
table; callers still supply an already finalized context. Random tag collision
branches are source-reviewed, not forced/tested through a new injection seam.

Frozen B test remains exactly
`c2698109d0a45621f6c705bcdfd2d0da3ae748db9802db1287de8517077fb81f`.
Header, profile, CMake/workflow and all other source/tests remain byte-unchanged.
No oracle/threshold, 57 legacy bindings or five explicit API targets changed.
Root source hash and whitespace/path-scope gates passed. This is static
integration evidence, not B GREEN runtime acceptance.

Required next hosted gates on both platforms: warning-clean build, five APIs,
prior focused and legacy 57/57, new target build, new focus 1/1 and full 58/58.
In the actual focused and full new test require all three completion markers:
valid path, 50 named rejections, and two-call uniqueness/owned-tag cache
isolation. Preserve complete logs and precise source/run identity. A failure
must be investigated without changing the frozen test to follow implementation.

No Mac build/cryptography, default merge or paper-completion claim. Full paper
I/O/family setup/N32768/eight squarings/1000 trials and applicable security and
performance evidence remain outside this diagnostic acceptance.

Independent actual-worktree reviewer h128_candidate_standards: PASS; source
also matches original return complete/project byte-for-byte. At 12:56
Asia/Shanghai, gitleaks 8.30.1 scanned src (63,587 bytes) and handoff evidence
(1,543,309 bytes), no leaks. Root diff checks passed; source size 11,625 bytes.
