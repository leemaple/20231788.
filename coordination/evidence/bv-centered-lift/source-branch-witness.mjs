// Bounded exact-integer model of cited source branches; NOT OpenFHE execution.
// No key generation, ciphertext, build, network, filesystem writes or imports.
const check = (ok, message) => { if (!ok) throw Error(message); };
const mod = (x, q) => ((x % q) + q) % q;
const center = (x, q) => { const r = mod(x, q); return r > q / 2n ? r - q : r; };
const abs = x => x < 0n ? -x : x;
const max = (a, b) => a > b ? a : b;
function sourceSwitch(v, oldQ, newQ) {
  check(0n <= v && v < oldQ, 'canonical source residue required');
  const negative = v > oldQ / 2n;
  if (newQ > oldQ) return v + (negative ? newQ - oldQ : 0n);
  return mod(v - (negative ? oldQ - newQ : 0n), newQ);
}
const primes = [3n, 5n, 7n, 11n, 13n, 17n, 19n, 23n, 29n, 31n, 37n, 41n, 43n];
let scalarCases = 0, lazyUnsignedDisagreements = 0;
for (const oldQ of primes) for (const newQ of primes) {
  for (let v = 0n; v < oldQ; ++v) {
    const actual = sourceSwitch(v, oldQ, newQ);
    const expected = mod(center(v, oldQ), newQ);
    check(actual === expected && 0n <= actual && actual < newQ, 'centered-lift identity');
    if (mod(v, newQ) !== expected) ++lazyUnsignedDisagreements;
    ++scalarCases;
  }
}
check(sourceSwitch(2n, 3n, 5n) === 4n, 'negative boundary witness');
check(sourceSwitch(1n, 3n, 5n) === 1n, 'positive boundary witness');
function convolution(a, b) { return [a[0]*b[0]-a[1]*b[1], a[0]*b[1]+a[1]*b[0]]; }
function toy(c, primes, gadgets, residuals, modulus) {
  const sum = [0n, 0n], gadget = [0n, 0n];
  const digits = primes.map(q => c.map(v => center(v, q)));
  for (let i = 0; i < primes.length; ++i) {
    const product = convolution(digits[i], residuals[i]);
    for (let j = 0; j < 2; ++j) {
      sum[j] += product[j];
      gadget[j] += gadgets[i]*digits[i][j];
    }
  }
  check(gadget.every((v, j) => mod(v, modulus) === mod(c[j], modulus)), 'CRT gadget identity');
  return {digits, error: sum.map(v => center(v, modulus))};
}
const prefixPrimes = [3n, 5n], prefixGadgets = [10n, 6n], residuals = [[1n, 0n], [0n, 1n]];
const fullPrimes = [3n, 5n, 7n], fullGadgets = [70n, 21n, 15n];
const fullResiduals = [...residuals, [4n, -3n]];
const bound = prefixPrimes.reduce((s, q, i) => s + q/2n * residuals[i].reduce((a, v) => a + abs(v), 0n), 0n);
check(bound === 3n, 'pre-ciphertext fixed-row bound');
const lowErrors = [], highErrors = [];
let maxLow = 0n, maxHigh = 0n, maxPair = 0n;
for (let a = 0n; a < 15n; ++a) for (let b = 0n; b < 15n; ++b) {
  const low = toy([a,b], prefixPrimes, prefixGadgets, residuals, 15n);
  const high = toy([7n*a,7n*b], fullPrimes, fullGadgets, fullResiduals, 105n);
  check(high.digits[2].every(v => v === 0n), 'raised-high last digit must vanish');
  const highRestricted = high.error.map(v => center(v,15n));
  const directPrefixHigh = toy([7n*a,7n*b], prefixPrimes, prefixGadgets, residuals, 15n);
  check(highRestricted.every((v,j) => v === directPrefixHigh.error[j]), 'restriction homomorphism');
  for (const v of low.error) { check(abs(v) <= bound, 'low fixed-row bound'); maxLow=max(maxLow,abs(v)); }
  for (const v of highRestricted) { check(abs(v) <= bound, 'high fixed-row bound'); maxHigh=max(maxHigh,abs(v)); }
  lowErrors.push(low.error); highErrors.push(highRestricted);
}
for (const high of highErrors) for (const low of lowErrors) {
  for (let j=0;j<2;++j) {
    const e=center(high[j]+low[j],15n);
    check(abs(e)<=2n*bound,'pair fixed-row bound'); maxPair=max(maxPair,abs(e));
  }
}
check(center(40n+40n,101n)===-21n && abs(center(80n,101n))<=80n, 'modular triangle does not imply no-wrap');
console.log(JSON.stringify({
  scope:'exact small-integer source-branch and toy-ring model only; NOT OpenFHE/CKKS execution',
  scalarCases, lazyUnsignedDisagreements, toyRingN:2, prefixQ:'15', fullQ:'105', qDiv:'7',
  fixedRowBound:bound.toString(), fullRaisedHighCases:highErrors.length, prefixLowCases:lowErrors.length,
  independentHighLowPairs:highErrors.length*lowErrors.length,
  maxHigh:maxHigh.toString(), maxLow:maxLow.toString(), maxPair:maxPair.toString(),
  zeroFinalDigit:true, prefixRestrictionIdentity:true, centeredLiftIdentity:true,
  universalKeyNoiseOrIntegerNoWrapProof:false, projectCTestExecuted:false
},null,2));
