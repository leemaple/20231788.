#!/usr/bin/env python3
"""Bounded read-only source comparison within this clean-room Git history.

This does not execute the codec, compile, transform, or prove that two historical
binaries emitted identical coefficients. It pins the narrower source facts.
"""
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
HISTORICAL = 'ed5fd192a89d6d4728ad295e87cf06a3f4abc832'
BASE = 'a4b815a733efe81897325e2a8e4c826a4ebfa439'


def blob(commit, path):
    return subprocess.check_output(['git', 'show', commit+':'+path], cwd=ROOT)


def section(source, first, after):
    if source.count(first) != 1 or source.count(after) != 1:
        raise ValueError('source section anchor is missing or ambiguous')
    start = source.index(first)
    end = source.index(after)
    if end <= start:
        raise ValueError('source section order changed')
    return source[start:end]


def main():
    paths = ['src/high_precision_client_io.cpp',
             'include/openfhe_2023_1788/high_precision_client_io.h',
             'tests/paper_full_eight_square_oracle.h']
    old = {p: blob(HISTORICAL, p) for p in paths}
    new = {p: blob(BASE, p) for p in paths}
    before = old[paths[0]].decode('utf-8')
    after = new[paths[0]].decode('utf-8')
    transform_start = '// Independently generated immutable roots at EACH working precision.'
    transform_end = 'template <class Real>\nstd::vector<Complex<Real>> Forward('
    core_start = '    for (const auto& value : values) { RequireFinite(value.real); RequireFinite(value.imag); }'
    historical_core = section(before, core_start, '    // Official large-Poly/DCRT constructor, not test-fixture tower injection.')
    current_core = section(after, core_start, '    return {std::move(coefficients), std::move(residues)};')
    replacements = [('impl_->primary', 'primaryTable'), ('impl_->check', 'checkTable')]
    for original, successor in replacements:
        if historical_core.count(original) != 1:
            raise ValueError('unexpected original table reference count')
        historical_core = historical_core.replace(original, successor)
    matches = {
        'entire_original_input_oracle_header_identical': old[paths[2]] == new[paths[2]],
        'root_tables_transform_inverse_and_stable_round_identical':
            section(before, transform_start, transform_end) == section(after, transform_start, transform_end),
        'encoding_arithmetic_and_residue_conversion_identical_after_two_table_parameter_renames':
            historical_core == current_core,
        'client_real_integer_and_complex_types_identical':
            section(old[paths[1]].decode('utf-8'), 'using ClientReal =', 'class PositiveRationalScale final') ==
            section(new[paths[1]].decode('utf-8'), 'using ClientReal =', 'class PositiveRationalScale final'),
        'working_precision_types_identical':
            section(before, 'template <unsigned Digits>', 'constexpr std::size_t kSlots') ==
            section(after, 'template <unsigned Digits>', 'constexpr std::size_t kSlots'),
    }
    if not all(matches.values()):
        raise ValueError('relevant source bridge changed: '+repr(matches))
    print(json.dumps({
        'status': 'SOURCE_SECTIONS_MATCH_NOT_BINARY_EQUIVALENCE',
        'historical_source': HISTORICAL, 'production_base': BASE,
        'whole_codec_file_identical': old[paths[0]] == new[paths[0]],
        'matches': matches,
        'table_parameter_renames': replacements,
        'file_hashes': {p: {'historical_sha256': hashlib.sha256(old[p]).hexdigest(),
                            'base_sha256': hashlib.sha256(new[p]).hexdigest()} for p in paths},
        'compiled_or_encoded': False,
        'historical_coefficient_identity': None,
        'remaining': ['same admitted geometry/full basis/exact scale',
                      'relevant numeric dependency and compiler semantics',
                      'actual coefficient or sufficient robust-equivalence evidence'],
    }, indent=2))


if __name__ == '__main__':
    main()
