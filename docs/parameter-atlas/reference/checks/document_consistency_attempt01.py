#!/usr/bin/env python3
"""Read-only source/atlas consistency audit. No C++/FHE/FFT/sampler execution.

Usage: python check_document_consistency.py INPUT_ROOT ATLAS_DIR OUTPUT_JSON
Scope: exact source identities/ranges, lexical inventories, dictionary shape,
local links and already-produced scalar records. This is not a semantic proof.
"""
from __future__ import annotations
import argparse
import collections
import csv
import hashlib
import json
import pathlib
import re
import sys


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('input_root', type=pathlib.Path)
    ap.add_argument('atlas_dir', type=pathlib.Path)
    ap.add_argument('output_json', type=pathlib.Path)
    args = ap.parse_args()
    root, out = args.input_root.resolve(), args.atlas_dir.resolve()
    failures: list[str] = []
    checks: dict[str, object] = {}
    def check(label: str, valid: bool, detail: object) -> None:
        checks[label] = {'passed': bool(valid), 'detail': detail}
        if not valid:
            failures.append(label)
    def load(name: str) -> object:
        return json.loads((out / name).read_text(encoding='utf-8'))
    def text(path: pathlib.Path) -> str:
        return path.read_bytes().decode('utf-8', 'replace')
    def source(rel: str) -> pathlib.Path:
        result = (root / rel).resolve()
        if not result.is_relative_to(root):
            raise ValueError(f'Unsafe source path: {rel}')
        return result

    params = load('PARAMETERS.json')
    refs = load('SOURCE_REFERENCES.json')['references']
    setters = load('PROJECT_SETTER_OCCURRENCES.json')
    constants = load('PROJECT_PROFILE_CONSTANTS.json')
    surface = load('CONTEXT_API_SURFACE.json')
    scalar = load('checks/scalar_constants.json')
    recs = params['records']
    ids = {r['id'] for r in recs}
    check('unique_parameter_ids', len(ids) == len(recs), len(recs))
    check('dictionary_reference_copy_equal', params['references'] == refs, len(refs))
    needed = {'id', 'meaning_zh', 'symbol_or_unit', 'upstream_default',
              'project_request', 'project_effective', 'set_at',
              'derive_or_validate_at', 'consumed_at', 'can_change_independently',
              'impact', 'evidence_grade', 'source_refs', 'runtime_confirmation'}
    malformed = [r['id'] for r in recs if not needed.issubset(r) or
                 set(r['impact']) != {'precision', 'noise', 'security', 'performance', 'compatibility'}
                 or any(k not in refs for k in r['source_refs'])]
    check('parameter_required_fields_and_refs', not malformed, malformed)
    source_errors = []
    for key, ref in refs.items():
        raw = source(ref['path']).read_bytes()
        lines = raw.decode('utf-8', 'replace').splitlines()
        if not (key == ref['id'] and hashlib.sha256(raw).hexdigest() == ref['sha256']
                and 1 <= ref['start_line'] <= ref['end_line'] <= len(lines)):
            source_errors.append(key)
        url = ref['fixed_url']
        if url:
            pin = ref['commit']
            suffix = f'#L{ref["start_line"]}-L{ref["end_line"]}'
            if not (f'/blob/{pin}/' in url and url.endswith(suffix)):
                source_errors.append(key + ':not-pinned')
    check('source_reference_hashes_ranges_fixed_urls', not source_errors,
          {'references': len(refs), 'distinct_paths': len({r['path'] for r in refs.values()}),
           'errors': source_errors, 'http_requests_executed': False})

    defaults = text(source('official/src/pke/include/scheme/gen-cryptocontext-params-defaults.h'))
    ckks_defaults = defaults.split('namespace CKKSRNS_SCHEME_DEFAULTS {', 1)[1].split('};', 1)[0]
    fields = set(re.findall(r'constexpr\s+\w+\s+(\w+)\s*=', ckks_defaults))
    ccrecords = [r for r in recs if 'cc_field' in r]
    check('all_ccparams_fields_covered', fields == {r['cc_field'] for r in ccrecords},
          {'fields': len(fields), 'records': len(ccrecords)})
    gen = text(source('official/src/pke/include/scheme/gen-cryptocontext-params.h'))
    generic = set(re.findall(r'virtual\s+void\s+(Set\w+)\s*\(', gen))
    represented = {s for r in ccrecords for s in r['setters']}
    check('all_generic_setters_covered', generic == represented,
          {'generic_setters': len(generic), 'missing': sorted(generic - represented),
           'extra': sorted(represented - generic)})
    specialized = text(source('official/src/pke/include/scheme/ckksrns/gen-cryptocontext-ckksrns-params.h'))
    disabled = set(re.findall(r'void\s+(Set\w+)\s*\([^)]*\)\s*override', specialized))
    check('ckks_disabled_setters_covered', disabled == set(params['disabled_ckks_setters']), len(disabled))
    cpp = text(source('official/src/pke/include/cryptocontext.h'))
    own = set(re.findall(r'\bvoid\s+(Set\w+)\s*\(', cpp))
    declared = {r['symbol'] for r in surface['set_named_methods']}
    decl_errors = [x['symbol'] for x in surface['set_named_methods']
                   if x['symbol'] not in cpp.splitlines()[x['declaration_line'] - 1]]
    check('context_own_set_named_methods', own == declared and not decl_errors,
          {'methods': len(own), 'missing': sorted(own - declared), 'declaration_errors': decl_errors})

    suffixes = {'.cpp', '.h', '.hpp', '.cc', '.cxx', '.c'}
    lexical: list[tuple[str, int, str]] = []
    pat = re.compile(r'\b(Set[A-Z]\w*|Enable)\s*\(')
    for path in sorted((root / 'project').rglob('*')):
        if path.is_file() and path.suffix in suffixes:
            rel = path.relative_to(root).as_posix()
            for i, line in enumerate(text(path).splitlines(), 1):
                lexical.extend((rel, i, m.group(1)) for m in pat.finditer(line))
    recorded = [(x['path'], x['line'], x['symbol']) for x in setters['occurrences']]
    lc, rc = collections.Counter(lexical), collections.Counter(recorded)
    mapping_errors = [x for x in setters['occurrences'] if x['parameter_id'] not in ids]
    line_errors = [x for x in setters['occurrences']
                   if text(source(x['path'])).splitlines()[x['line'] - 1].strip() != x['text'].strip()]
    check('project_setter_lexical_inventory_exact', lc == rc and not mapping_errors and not line_errors,
          {'occurrences': len(lexical), 'unique_symbols': len({v[2] for v in lexical}),
           'missing': list(lc - rc), 'extra': list(rc - lc),
           'mapping_errors': len(mapping_errors), 'line_errors': len(line_errors),
           'not_ast_or_reachability_proof': True})
    counts = collections.Counter(x['parameter_id'] for x in setters['occurrences'])
    bad_counts = [r['id'] for r in recs if r.get('project_setter_occurrence_count', 0) != counts[r['id']]]
    check('per_parameter_setter_counts', not bad_counts, {'total': sum(counts.values()), 'bad': bad_counts})
    const_bad = []
    for item in constants['entries']:
        lines = text(source(item['path'])).splitlines()
        if not (1 <= item['line'] <= len(lines) and lines[item['line'] - 1].strip() == item['text'].strip()):
            const_bad.append(item)
    check('profile_constant_lexical_index_source_lines', not const_bad,
          {'entries': len(constants['entries']), 'errors': len(const_bad),
           'not_all_cpp_constant_expressions_or_runtime_values': True})

    main_doc = text(out / 'OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md')
    md = {p.name: text(p) for p in out.glob('*.md')}
    anchors = set(re.findall(r'<a id="([^"]+)"', main_doc))
    linked_ids = {v for v in re.findall(r'#src-([a-z0-9]+)', '\n'.join(md.values()))}
    expected_ids = {k.lower() for k in refs}
    check('all_source_anchors_and_document_links_resolve',
          all('src-' + v in anchors for v in expected_ids | linked_ids)
          and linked_ids.issubset(expected_ids),
          {'expected': len(expected_ids), 'referenced': len(linked_ids), 'anchors': len(anchors)})
    link_errors = []
    local_links = 0
    for name, content in md.items():
        for target in re.findall(r'\]\(([^)]+)\)', content):
            if target.startswith(('https:', 'http:', 'mailto:', '#')):
                continue
            file_part = target.split('#')[0]
            if not file_part:
                continue
            local_links += 1
            if not (out / file_part).is_file():
                link_errors.append({'file': name, 'target': target})
    check('markdown_local_file_links', not link_errors, {'checked': local_links, 'errors': link_errors})
    check('no_unexpanded_template_markers', not any(re.search(r'\{\{[A-Z_]+\}\}', v) for v in md.values()), len(md))
    tables_errors = []
    for name, content in md.items():
        count = None
        for i, line in enumerate(content.splitlines(), 1):
            if line.startswith('|'):
                cells = len(re.split(r'(?<!\\)\|', line)) - 2
                if count is not None and cells != count:
                    tables_errors.append({'file': name, 'line': i, 'expected': count, 'actual': cells})
                count = cells
            else:
                count = None
    check('markdown_table_column_counts', not tables_errors, tables_errors)
    prime_missing = [f'{name}:{v["role"]}:{key}'
                     for name, profile in scalar['profiles'].items()
                     for v in profile['ordered_primes'] for key in ['q', 'root'] if v[key] not in main_doc]
    check('all_profile_exact_prime_root_rows_in_main', not prime_missing,
          {'profile_rows': sum(len(p['ordered_primes']) for p in scalar['profiles'].values()), 'missing': prime_missing})
    check('stored_scalar_profiles_complete', not scalar['failures'] and
          all(len(p['steps']) == 8 and all(s['closed_form_equals_recursive'] for s in p['steps'])
              for p in scalar['profiles'].values()),
          {'profiles': len(scalar['profiles']), 'steps_total': sum(len(p['steps']) for p in scalar['profiles'].values()),
           'not_an_independent_reexecution': True})
    exact = next(r for r in recs if r['id'] == 'numeric.one_norm')
    check('observer_norm_exponent_not_power_conflated',
          'k=ceil(log2(C/Δ))' in exact['symbol_or_unit'] and
          'K=2^ceil(log2(C/Δ))' not in main_doc, exact['symbol_or_unit'])
    check('binary_sampler_h128_path_not_omitted',
          'BUG' in md['RANDOMNESS_PATHS.md'] and '内' in next(r for r in recs if r['id'] == 'rng.binary')['notes_zh'],
          'Presence/terminology consistency only, not an independent semantic proof')
    reads = [json.loads(line) for line in (out / 'READ_REQUESTS.jsonl').read_text().splitlines()]
    bad = []
    for item in reads:
        raw = source(item['path']).read_bytes()
        if not (hashlib.sha256(raw).hexdigest() == item['sha256'] and
                1 <= item['start'] <= item['end'] <= len(raw.decode('utf-8', 'replace').splitlines())):
            bad.append(item)
    check('read_request_hashes_and_physical_line_bounds', not bad,
          {'requests': len(reads), 'paths': len({x['path'] for x in reads}), 'bad': len(bad),
           'requested_display_not_full_read_claim': True})
    coverage = list(csv.DictReader((out / 'SOURCE_COVERAGE.tsv').open(encoding='utf-8'), delimiter='\t'))
    source_manifest = json.loads((root / 'MANIFEST.json').read_bytes())
    expected_paths = {x['path'] for x in source_manifest['files']} | {'MANIFEST.json'}
    errors = [r['path'] for r in coverage if hashlib.sha256(source(r['path']).read_bytes()).hexdigest() != r['sha256']]
    check('coverage_all_input_members_and_source_unchanged',
          len(coverage) == len(expected_paths) and {r['path'] for r in coverage} == expected_paths and not errors,
          {'rows': len(coverage), 'errors': errors})
    required = ['OPENFHE_CKKS_PARAMETER_ATLAS.zh-CN.md','PARAMETERS.json','RANDOMNESS_PATHS.md',
                'CHANGE_IMPACT.md','SOURCE_COVERAGE.tsv','FINDINGS.md','EXECUTION_LEDGER.md']
    check('required_nonmanifest_deliverables_exist', all((out/n).is_file() for n in required), required)
    result = {'scope': 'Document/static inventory audit; no algorithm or independent semantic review',
              'python': sys.version.split()[0], 'checks': checks, 'checks_count': len(checks),
              'failures': failures, 'passed': not failures}
    args.output_json.parent.mkdir(parents=True, exist_ok=True)
    args.output_json.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'checks': len(checks), 'failures': failures, 'passed': not failures}, ensure_ascii=False))
    if failures:
        raise SystemExit(1)

if __name__ == '__main__':
    main()
