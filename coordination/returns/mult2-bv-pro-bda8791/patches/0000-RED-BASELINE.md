# 0000 RED — immutable historical BV failure

## Why this is a record rather than a synthetic code patch

The exact source at project commit `bda879104c8a8b1ba6ac9301385b5b1919bef440` already contains a real fatal assertion and the packet contains actual Linux and Windows failures. Creating a second artificial red would add no information and could obscure the authentic evidence.

## Baseline assertion

In the unmodified `project/tests/mult2_e2e_oracle_test.cpp`, the oracle:

1. recombines the Tensor pair on `Q_l`;
2. invokes one ordinary `context->Relinearize` on that combined degree-three ciphertext;
3. independently decrypts it and measures `empiricalRelinError`;
4. independently decrypts/recombines the production `Relin2` pair and measures `empiricalPairRelinError`; and
5. asserts:

```text
empiricalPairRelinError <= empiricalRelinError + h
```

with failure text:

```text
pair relinearization error exceeded empirical E_Relin + h
```

Relevant baseline source: `project/tests/mult2_e2e_oracle_test.cpp:654-736`.

## Linux diagnostic red

Run `33840176712`, job `100920696884`:

```text
BV REAL:
  ordinary E = 197331007675
  pair error = 323602105437
  h          = 43
  failure margin = 126271097719

BV COMPLEX:
  ordinary E = 181218269350
  pair error = 223094194606
  h          = 44
  failure margin = 41875925212
```

The suite reports `42/44`; only tests 43 and 44 fail. Exact retained source: `evidence/bv-diagnostic-linux.txt:431-463`.

## Windows red

Run `33839781546`, job `100919538008` completed `42/44` in `1.18 s` with the same two test names and same failure message. Exact retained source: `evidence/matrix-red-windows.txt:456-485`.

## What the red proves and does not prove

It proves that the numerical inequality is false for the retained BV executions. From the reported maxima alone, the paper-additivity residual is at least:

```text
126271097762 for BV REAL, versus h=43
 41875925256 for BV COMPLEX, versus h=44.
```

It does **not** by itself prove a production `Relin2` wrapper defect, because the failed inequality compares different ordinary key-switch executions and depends on the paper proof's near-additivity property. It also does not prove the independent CRT oracle correct. Those are the purposes of patch 0001.

The BV tests abort at this assertion, so the later output-coefficient and decoded-slot checks are not historical passes and must not be reported as such.
