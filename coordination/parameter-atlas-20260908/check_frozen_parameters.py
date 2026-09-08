"""Static integer/document checks only; never imports or executes OpenFHE."""
import hashlib
import json
import math
from decimal import Decimal, localcontext
from pathlib import Path
import re
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[2]
PACKET = ROOT / 'artifacts/handoffs/parameter-atlas-20260908/openfhe-parameter-atlas-a4b815a.zip'
PACKET_SHA = 'abc4df778754a2d3713f1442fbfe34d69af274f3638e65d4d362f35aa93fdd68'


def main():
    blob = PACKET.read_bytes()
    assert hashlib.sha256(blob).hexdigest() == PACKET_SHA
    with zipfile.ZipFile(PACKET) as z:
        src = z.read('project/src/repeated_mult2.cpp')
        assert src == (ROOT / 'src/repeated_mult2.cpp').read_bytes()
        base = z.read('official/src/pke/include/scheme/gen-cryptocontext-params.h').decode()
        ckks = z.read('official/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h').decode()
    text = src.decode()
    pair = r'\{(\d+)ULL,(\d+)ULL\}'
    paper = [tuple(map(int, p)) for p in re.findall(pair, text.split('kPaperQ{{', 1)[1].split('}};', 1)[0])]
    special = [tuple(map(int, p)) for p in re.findall(pair, text.split('kExperimentalPrecision116Profile{{{', 1)[1].split('}},kPaperP,58};', 1)[0])]
    auxiliary = tuple(map(int, re.search(r'kPaperP\{(\d+)ULL,(\d+)ULL\}', text).groups()))
    assert len(paper) == 11 and len(special) == 3
    candidate = special[:2] + paper[2:10] + special[2:]
    setters = re.findall(r'^\s*virtual\s+void\s+(Set\w+)\(', base, re.M)
    disabled = re.findall(r'void\s+(Set\w+)\([^)]*\)\s+override\s*\{\s*DISABLED_FOR_CKKSRNS;', ckks)
    assert len(setters) == len(set(setters)) == 32
    assert len(disabled) == len(set(disabled)) == 8 and set(disabled) <= set(setters)
    profiles = {}
    for label, primes, b in [('S100', paper, 50), ('S116', candidate, 58)]:
        q = [p for p, _ in primes]
        all_primes = primes + [auxiliary]
        assert len({p for p, _ in all_primes}) == 12
        for p, r in all_primes:
            assert p % 65536 == 1 and 0 < r < p
            assert pow(r, 32768, p) == p - 1 and pow(r, 65536, p) == 1
        assert all(math.gcd(a, c) == 1 for i, a in enumerate(q + [auxiliary[0]]) for c in (q + [auxiliary[0]])[i + 1:])
        current = q[:]
        numerator_power = 2 * b
        denominator = 1
        rounds = []
        factors = {}
        for family in range(8):
            consumed = current[-2]
            numerator_power *= 2
            denominator = denominator * denominator * q[-1] * consumed
            factors = {p: exponent * 2 for p, exponent in factors.items()}
            for p in (q[-1], consumed):
                factors[p] = factors.get(p, 0) + 1
            assert math.prod(p ** e for p, e in factors.items()) == denominator
            with localcontext() as ctx:
                ctx.prec = 50
                excess = Decimal(2 ** (numerator_power - 2 * b)) / Decimal(denominator) - 1
            rounds.append({'round': family + 1, 'family': family, 'family_Q_count': len(current),
                           'input_pair_Q_count': len(current) - 1, 'output_pair_Q_count': len(current) - 2,
                           'consumed_Mult': consumed, 'Div': q[-1], 'recorded_scale_pow2': 2 * b,
                           'exact_scale_numerator_pow2': numerator_power,
                           'exact_scale_denominator_factors': [{'integer': p, 'exponent': e} for p, e in sorted(factors.items())],
                           'approx_exact_to_recorded_ratio_minus_one_50digits': str(excess)})
            del current[-2]
        assert current == [q[0], q[1], q[-1]]
        profiles[label] = {'ordered_Q_and_roots': primes, 'P_and_root': auxiliary,
                           'Q_bit_length': math.prod(q).bit_length(),
                           'QP_bit_length': (math.prod(q) * auxiliary[0]).bit_length(), 'rounds': rounds}
    result = {'check': 'PASS', 'meaning': 'source-bound lexical and small-integer constant/scale checks only',
              'not_checked': ['primality certification', 'samplers', 'FFT', 'OpenFHE runtime', 'FHE precision', 'security'],
              'source_commit': 'a4b815a733efe81897325e2a8e4c826a4ebfa439',
              'official_pin': 'df495ba2e91739a6dc8f1de254fc5a41155ce504', 'packet_sha256': PACKET_SHA,
              'python': sys.version, 'base_setters': setters, 'CKKS_disabled_setters': disabled, 'profiles': profiles}
    output = Path(__file__).with_name('ROOT_STATIC_PARAMETER_CHECK.json')
    with output.open('x') as f:
        json.dump(result, f, ensure_ascii=False, indent=2)
        f.write('\n')
    print(json.dumps({'result': str(output), 'profiles_QP_bit_length': {k: v['QP_bit_length'] for k, v in profiles.items()},
                      'setters': len(setters), 'disabled': len(disabled), 'check': 'PASS', 'FHE_run': False}))


if __name__ == '__main__':
    main()
