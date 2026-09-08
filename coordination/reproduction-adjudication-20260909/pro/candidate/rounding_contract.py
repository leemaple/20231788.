"""Scalar binding / half-down rounding cells. No trigonometry or transforms."""
from __future__ import annotations
from fractions import Fraction
import hashlib
import json
from pathlib import Path
import re
from interval_core import Interval, UNIT

N = 32768
SCALE = 1 << 100
PAYLOAD_SHA256 = '7cac9765f0c5e567840ebc347a59e50d039107c7b6a92c037152b69bd8edd94f'
STREAM_SHA256 = '66c36716c1445b4b2c6c47e06996c2b080792b0b9251fe075033f542acd540be'
BASE = Path(__file__).resolve().parents[1]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def sha256_file(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def unique_object(pairs: list[tuple[str, object]]) -> dict:
    result = {}
    for key, value in pairs:
        require(key not in result, 'duplicate JSON member')
        result[key] = value
    return result


def load_bound_fixture() -> tuple[dict, list[int]]:
    """Reads retained public integers, never generates slots or encodes anything."""
    bindings = json.loads((BASE/'source/SOURCE_BINDINGS.json').read_text())
    for row in bindings:
        path = BASE/row['delivery_path']
        require(path.is_file() and not path.is_symlink(), 'missing/unsafe source fixture')
        require(path.stat().st_size == row['bytes'] and sha256_file(path) == row['sha256'],
                'source fixture identity mismatch: '+row['delivery_path'])
    path = BASE/'fixtures/public_s100_encoding.json'
    raw = path.read_bytes()
    require(hashlib.sha256(raw).hexdigest() == PAYLOAD_SHA256, 'public payload mismatch')
    data = json.loads(raw, object_pairs_hook=unique_object)
    require(data['n'] == N and data['slots'] == N//2 and data['gap'] == 1
            and data['cyclotomic_order'] == 2*N, 'geometry mismatch')
    require(data['scale_num'] == str(SCALE) and data['scale_den'] == '1', 'scale mismatch')
    require(data['encoding_calls'] == 1 and data['crypto_calls'] == 0, 'retained run counters mismatch')
    require(data['build_source_commit'] == '28bab40431eebc85d72521c6d9dc840ecd675cd7',
            'build identity mismatch')
    strings = data['coefficients']
    require(len(strings) == N, 'coefficient count mismatch')
    require(all(isinstance(v,str) and re.fullmatch(r'0|-?[1-9][0-9]*', v)
                for v in strings), 'noncanonical integer spelling')
    require(hashlib.sha256(('\n'.join(strings)+'\n').encode()).hexdigest() == STREAM_SHA256,
            'coefficient stream mismatch')
    return data, [int(v) for v in strings]


def classify_cell(enclosure: Interval, p: int) -> str:
    """Nearest, ties to the lower integer: exact preimage of p is (p-1/2,p+1/2].

    A REFUTED interval must lie WHOLLY outside that cell. An intersection is
    not a certificate. Equality at the open lower boundary is a refutation.
    """
    require(type(p) is int, 'integer coefficient required')
    left, right = p*UNIT-UNIT//2, p*UNIT+UNIT//2
    if enclosure.lo > left and enclosure.hi <= right:
        return 'CERTIFIED'
    if enclosure.hi <= left or enclosure.lo > right:
        return 'REFUTED'
    return 'INCONCLUSIVE'


def cell_margin(enclosure: Interval, p: int) -> int:
    """Strict interior clearance on the fixed grid; <=0 cannot be runner GREEN."""
    return min(enclosure.lo-(p*UNIT-UNIT//2), (p*UNIT+UNIT//2)-enclosure.hi)


def original_slot(s: int) -> tuple[Fraction, Fraction]:
    """Direct dyadic transcription of frozen C++ Input(s); no transform.
    The full runner alone calls this for all slots. Scalar checks use O(1) slots.
    """
    require(type(s) is int and 0 <= s < N//2, 'slot outside frozen domain')
    t=s//2
    a=Fraction(1015,1024)-Fraction(t%16,65536)+Fraction(s,1<<75)
    b=Fraction(1+(t//16)%8,1024)
    if (t//512)%2:
        b=-b
    return ((a,b),(-b,a),(-a,-b),(b,-a))[(t//128)%4]


def closed_form_two_coefficients() -> tuple[int,int]:
    """Algebraic block sum of the exact Input(s), not a length-N computation.

    In each 1024 block, base/a and imaginary/b repeat through four rotations
    and cancel. For the dyadic s perturbation, each 256-quarter sum is
    262144*block+65536*r+32640. Only 65536*r survives rotation.
    Sixteen blocks give sum z = -2^-54(1+i). 5^s == 1 mod 4 makes
    coefficient N/2 the scaled imaginary mean, with a POSITIVE sign.
    """
    quarter_real = -2*65536
    quarter_imag = (1-3)*65536
    total_real = Fraction(16*quarter_real,1<<75)
    total_imag = Fraction(16*quarter_imag,1<<75)
    re=total_real*SCALE/(N//2)
    im=total_imag*SCALE/(N//2)
    require(re.denominator == im.denominator == 1, 'unexpected nonintegral mean')
    return re.numerator, im.numerator
