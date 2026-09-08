"""Exact interval primitives copied verbatim from supplied reviewed cap checker.
Original project/diagnostics/certify_public_encoder.py lines 39-140.
See source/SOURCE_BINDINGS.json. This module has NO transform at import.
Taylor/root routines exist for a SEPARATELY reviewed runner; not called by
this review's scalar checks. Not independent from the cap interval primitives;
independent from the production Boost special-FFT encoder.
"""
from __future__ import annotations
from dataclasses import dataclass
from fractions import Fraction
import math
BITS = 224
UNIT = 1 << BITS

def ceil_div(a: int, b: int) -> int:
    if b <= 0: raise ValueError('positive divisor required')
    return -((-a)//b)


@dataclass(frozen=True)
class Interval:
    lo: int
    hi: int

    def __post_init__(self):
        if type(self.lo) is not int or type(self.hi) is not int or self.lo > self.hi:
            raise ValueError('invalid interval endpoints')

    @classmethod
    def rational(cls, p: int, q: int=1) -> 'Interval':
        if q <= 0: raise ValueError('positive denominator required')
        return cls((p*UNIT)//q, ceil_div(p*UNIT,q))

    def __add__(self, other: 'Interval') -> 'Interval':
        return Interval(self.lo+other.lo,self.hi+other.hi)

    def __neg__(self) -> 'Interval':
        return Interval(-self.hi,-self.lo)

    def __sub__(self, other: 'Interval') -> 'Interval':
        return self+-other

    def __mul__(self, other: 'Interval') -> 'Interval':
        products=(self.lo*other.lo,self.lo*other.hi,self.hi*other.lo,self.hi*other.hi)
        return Interval(min(products)//UNIT,ceil_div(max(products),UNIT))

    def divide_positive_integer(self, divisor: int) -> 'Interval':
        if divisor <= 0: raise ValueError('positive divisor required')
        return Interval(self.lo//divisor,ceil_div(self.hi,divisor))

    def square(self) -> 'Interval':
        upper=max(self.lo*self.lo,self.hi*self.hi)
        lower=0 if self.lo<=0<=self.hi else min(self.lo*self.lo,self.hi*self.hi)
        return Interval(lower//UNIT,ceil_div(upper,UNIT))

    def expanded(self, radius: Fraction) -> 'Interval':
        if radius < 0: raise ValueError('nonnegative radius required')
        r=ceil_div(radius.numerator*UNIT,radius.denominator)
        return Interval(self.lo-r,self.hi+r)

    def contains(self, x: Fraction) -> bool:
        return self.lo*x.denominator <= x.numerator*UNIT <= self.hi*x.denominator

    def json(self) -> dict:
        return {'lower_numerator':str(self.lo),'upper_numerator':str(self.hi),
                'denominator':str(UNIT)}


ZERO=Interval(0,0)
ONE=Interval(UNIT,UNIT)
# Complex intervals are (real interval, imaginary interval).
ComplexInterval=tuple[Interval,Interval]


def cadd(a: ComplexInterval,b: ComplexInterval) -> ComplexInterval:
    return a[0]+b[0],a[1]+b[1]


def csub(a: ComplexInterval,b: ComplexInterval) -> ComplexInterval:
    return a[0]-b[0],a[1]-b[1]


def cmul(a: ComplexInterval,b: ComplexInterval) -> ComplexInterval:
    return a[0]*b[0]-a[1]*b[1],a[0]*b[1]+a[1]*b[0]


def atan_reciprocal_bounds(denominator: int) -> tuple[Fraction,Fraction]:
    if denominator < 2: raise ValueError('alternating convergence domain')
    terms=96
    partial=sum((Fraction((-1)**j,(2*j+1)*denominator**(2*j+1))
                 for j in range(terms)),Fraction(0))
    adjacent=partial+Fraction((-1)**terms,(2*terms+1)*denominator**(2*terms+1))
    return min(partial,adjacent),max(partial,adjacent)


def pi_interval() -> Interval:
    a,b=atan_reciprocal_bounds(5)
    c,d=atan_reciprocal_bounds(239)
    lo,hi=16*a-4*d,16*b-4*c
    lower=Interval.rational(lo.numerator,lo.denominator)
    upper=Interval.rational(hi.numerator,hi.denominator)
    return Interval(lower.lo,upper.hi)


def sincos_small(x: Interval) -> ComplexInterval:
    """Returns (cos(x),sin(x)); |x|<=1. Taylor remainder is explicitly added."""
    bound=Fraction(max(abs(x.lo),abs(x.hi)),UNIT)
    if bound>1: raise ValueError('Taylor argument outside fixed certified domain')
    powers=[ONE]
    for _ in range(63):powers.append(powers[-1]*x)
    cosine=ZERO;sine=ZERO
    for k in range(32):
        cosine=cosine+powers[2*k]*Interval.rational((-1)**k,math.factorial(2*k))
        sine=sine+powers[2*k+1]*Interval.rational((-1)**k,math.factorial(2*k+1))
    return (cosine.expanded(bound**64/math.factorial(64)),
            sine.expanded(bound**65/math.factorial(65)))
