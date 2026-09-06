#!/usr/bin/env python3
"""Bounded source/patch/scalar checks. NEVER builds OpenFHE or runs cryptography.

Usage: python3 -B checks/verify_draft.py INPUT.zip --report results/static_checks.json
The package containing this script must include RED.patch, GREEN.patch, RED/, GREEN/.
Only a constants-only C++ excerpt may be syntax-checked with installed compilers.
This is NOT a project/API compilation, runtime test, independent review or E80 pass.
"""
from __future__ import annotations
import argparse
import difflib
from fractions import Fraction
import hashlib
import json
from math import prod
from pathlib import Path, PurePosixPath
import platform
import re
import shutil
import stat
import subprocess
import sys
import tempfile
import zipfile

INPUT_SHA = '75c29c9f1305c44fc64679827dcfc1e8b3ba475b48ab46900f71849c255f9a7b'
MANIFEST_SHA = '40a4e8ee30121c85800b6eea4a541b52c11df4fd7b56cdd8da1c4da11207be91'
ENGINEERING = 'dbbbee0d20d8a7ae3c138e42f633414db621a173'
PACKAGING = '21d94c23a7163c4770e8ec377219ea54d4428e2d'
PIN = 'df495ba2e91739a6dc8f1de254fc5a41155ce504'
RED_PATHS = {'CMakeLists.txt', 'tests/paper_full_eight_square_contract_test.cpp',
             'tests/experimental_precision116_profile_seam.h'}
GREEN_PATHS = {'include/openfhe_2023_1788/repeated_mult2.h', 'src/repeated_mult2.cpp',
               'src/high_precision_client_io.cpp', 'src/double_ckks.cpp'}
NEW_FLAG = '--experimental-precision116-profile-seam'
NEW_TEST = 'experimental_precision116_profile_seam'
INCLUDE = '#include "experimental_precision116_profile_seam.h"\n'
DISPATCH = '''    if (argc == 2 && std::string(argv[1]) == "--experimental-precision116-profile-seam") {
        try { experimental_precision116_test::Run(); return 0; }
        catch (const std::exception& error) {
            std::cerr << "EXPERIMENTAL_PRECISION116_PROFILE_SEAM result=FAIL profile=experimental-s116-d56-b58-v1"
                         " squares=1 full_eight_square_E80=NOT_TESTED security=UNRESOLVED reason="
                      << error.what() << '\\n';
            return 1;
        }
    }
'''


def require(ok: bool, label: str) -> None:
    if not ok:
        raise ValueError(label)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def blob(data: bytes) -> str:
    return hashlib.sha1(b'blob ' + str(len(data)).encode() + b'\0' + data).hexdigest()


def tree(path: Path) -> dict[str, bytes]:
    return {p.relative_to(path).as_posix(): p.read_bytes() for p in path.rglob('*') if p.is_file()}


def changed(left: dict[str, bytes], right: dict[str, bytes]) -> set[str]:
    return {key for key in left.keys() | right.keys() if left.get(key) != right.get(key)}


# Strip only C++ comments and literals while preserving offsets/newlines. This
# supports delimiter/source-region checks, NOT full C++ grammar or type checking.
SKIP = re.compile(r'//[^\n]*|/\*.*?\*/|(?:u8|u|U|L)?R"(?P<delim>[^ ()\\\t\r\n]{0,16})\(.*?\)(?P=delim)"|'
                  r'(?:u8|u|U|L)?"(?:\\.|[^"\\])*"|(?:u8|u|U|L)?\'(?:\\.|[^\'\\])*\'', re.S)


def code_only(text: str) -> str:
    return SKIP.sub(lambda match: ''.join('\n' if c == '\n' else ' ' for c in match.group()), text)


def delimiters(text: str, label: str) -> None:
    stack: list[str] = []
    matching = {')': '(', '}': '{', ']': '['}
    for ch in code_only(text):
        if ch in '({[':
            stack.append(ch)
        elif ch in ')}]':
            require(bool(stack) and stack.pop() == matching[ch], f'{label}: unmatched delimiter')
    require(not stack, f'{label}: unclosed delimiter')


def function(text: str, signature: str) -> str:
    require(text.count(signature) == 1, f'unique function signature: {signature}')
    start = text.index(signature)
    masked = code_only(text)
    opening = masked.index('{', start)
    depth = 0
    for pos in range(opening, len(masked)):
        if masked[pos] == '{':
            depth += 1
        elif masked[pos] == '}':
            depth -= 1
            if depth == 0:
                return text[start:pos + 1]
    raise ValueError(f'unclosed function: {signature}')


def literal_primes(text: str, start: str, end: str) -> list[tuple[int, int]]:
    block = text.split(start, 1)[1].split(end, 1)[0]
    return [(int(m), int(r)) for m, r in re.findall(r'\{\s*(\d+)ULL\s*,\s*(\d+)ULL\s*\}', block)]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input_zip', type=Path)
    parser.add_argument('--report', type=Path, required=True)
    args = parser.parse_args()
    package = Path(__file__).resolve().parent.parent
    report: dict = {'scope': 'patch/source/scalar checks only; project NOT COMPILED; crypto NOT RUN',
                    'environment': {'python': sys.version, 'platform': platform.platform()},
                    'commands': [], 'checks': []}

    def checked(label: str, detail=None) -> None:
        report['checks'].append({'name': label, 'result': 'PASS', 'detail': detail})

    def run(argv: list[str], cwd: Path, label: str, expected: int = 0) -> subprocess.CompletedProcess:
        completed = subprocess.run(argv, cwd=cwd, text=True, capture_output=True, timeout=30)
        report['commands'].append({'label': label, 'argv': argv, 'cwd': str(cwd),
                                   'exit': completed.returncode, 'stdout': completed.stdout,
                                   'stderr': completed.stderr})
        require(completed.returncode == expected, f'{label}: exit {completed.returncode}, expected {expected}')
        return completed

    try:
        raw = args.input_zip.read_bytes()
        require(len(raw) == 1543581 and sha(raw) == INPUT_SHA, 'input archive identity')
        with zipfile.ZipFile(args.input_zip) as z:
            members = z.infolist()
            names = [item.filename for item in members]
            require(len(members) == 198 and len(set(names)) == 198, 'exact regular member count')
            for member in members:
                p = PurePosixPath(member.filename)
                require(not p.is_absolute() and '..' not in p.parts and '\\' not in member.filename and
                        ':' not in member.filename and not member.is_dir() and
                        stat.S_ISREG(member.external_attr >> 16), 'safe regular ZIP member')
            require(z.testzip() is None, 'input CRC')
            files = {name: z.read(name) for name in names}
        require(sha(files['MANIFEST.json']) == MANIFEST_SHA, 'input manifest identity')
        manifest = json.loads(files['MANIFEST.json'])
        require(manifest['source_commit'] == ENGINEERING and manifest['packaging_documentation_head'] == PACKAGING and
                manifest['official_pin'] == PIN and manifest['manifest_self_excluded'], 'input authority identities')
        records = manifest['files']
        require(len(records) == 197 and {r['path'] for r in records} | {'MANIFEST.json'} == set(files), 'manifest closure')
        blobs = 0
        for record in records:
            data = files[record['path']]
            require(len(data) == record['bytes'] and sha(data) == record['sha256'], f'payload: {record["path"]}')
            if record.get('origin', {}).get('git_blob'):
                require(blob(data) == record['origin']['git_blob'], f'Git blob: {record["path"]}')
                blobs += 1
        checked('input identity, CRC, closure and payload hashes', {'members': 198, 'payloads': 197, 'git_blobs': blobs,
                'input_sha256': INPUT_SHA, 'manifest_sha256': MANIFEST_SHA,
                'engineering_source': ENGINEERING, 'packaging_commit': PACKAGING, 'official_pin': PIN})
        base = {key[len('project/'):]: value for key, value in files.items() if key.startswith('project/')}
        require(not any(key.startswith('.git/') for key in base), 'packet is source slice, not a Git database')
        require(shutil.which('git') is not None, 'git is needed for actual patch application checks')
        report['environment']['git'] = run(['git', '--version'], package, 'Git tool version').stdout.strip()
        with tempfile.TemporaryDirectory(prefix='precision116-static-') as tmp:
            work = Path(tmp)
            for name, data in base.items():
                p = work / 'replay' / name; p.parent.mkdir(parents=True, exist_ok=True); p.write_bytes(data)
            replay = work / 'replay'
            for name, paths in [('RED', RED_PATHS), ('GREEN', GREEN_PATHS)]:
                patch = package / f'{name}.patch'
                payload = patch.read_text()
                actual_paths = set(re.findall(r'^diff --git a/(\S+) b/\1$', payload, flags=re.M))
                require(actual_paths == paths, f'{name} path allowlist')
                require(not any(p.startswith(('project/', '.github/')) or p.startswith('/') for p in actual_paths), 'patch relativity')
                run(['git', 'apply', '--check', '--whitespace=error-all', str(patch)], replay, f'{name} patch apply check')
                run(['git', 'apply', '--whitespace=error-all', str(patch)], replay, f'{name} actual application to replay tree')
                now = tree(replay)
                prior = base if name == 'RED' else red
                require(changed(prior, now) == paths, f'{name} exact changed path set')
                copies = tree(package / name)
                require(set(copies) == paths and all(now[path] == copies[path] for path in paths), f'{name} lossless full-file copies')
                if name == 'RED':
                    red = now
                else:
                    green = now
                stat_result = run(['git', 'apply', '--numstat', str(patch)], replay, f'{name} numstat (not a build)')
                checked(f'{name} applicable patch, allowlist and full-file copies', {'paths': sorted(paths), 'patch_sha256': sha(patch.read_bytes()), 'numstat': stat_result.stdout})
            require(changed(red, green) == GREEN_PATHS and all(red[p] == green[p] for p in RED_PATHS), 'RED assertions unchanged by GREEN')
            checked('RED/GREEN separation; no GREEN edits to RED assertions')

            header = 'include/openfhe_2023_1788/repeated_mult2.h'
            factory = 'RepeatedMult2ClientSetup CreateExperimentalPrecision116Setup();'
            require('CreateExperimentalPrecision116Setup' not in red[header].decode() and factory in green[header].decode(), 'missing-factory RED API design')
            new_header = green['tests/experimental_precision116_profile_seam.h'].decode()
            require(new_header.count('openfhe_2023_1788::CreateExperimentalPrecision116Setup()') == 1 and
                    new_header.count('evaluator.Mult2(pair, pair)') == 1 and
                    'RunPaper(' not in new_header and 'paper_endpoint_contract' not in new_header,
                    'one named factory, one Mult2, no old chain/publisher')
            require('CreateExperimentalPrecision116Setup' not in red['src/repeated_mult2.cpp'].decode(), 'RED has no implementation')
            checked('RED missing-factory reference only (expected hosted compile failure, NOT observed here)')
            old_test = base['tests/paper_full_eight_square_contract_test.cpp'].decode()
            new_test = green['tests/paper_full_eight_square_contract_test.cpp'].decode()
            require(new_test.count(INCLUDE) == 1 and new_test.count(DISPATCH) == 1 and
                    new_test.replace(INCLUDE, '', 1).replace(DISPATCH, '', 1) == old_test, 'exact original test body/modes preservation')
            checked('original no-argument body, both old modes, outputs and predicates preserved byte-for-byte')
            old_cmake = base['CMakeLists.txt'].decode(); new_cmake = green['CMakeLists.txt'].decode()
            require(new_cmake.startswith(old_cmake), 'CMake original prefix preservation')
            old_names = re.findall(r'add_test\(NAME\s+(\w+)', old_cmake)
            new_names = re.findall(r'add_test\(NAME\s+(\w+)', new_cmake)
            require(len(old_names) == 61 and new_names == old_names + [NEW_TEST], 'exact CTest names/order')
            for bits in ('TIMEOUT 1200', 'RUN_SERIAL TRUE', 'ENVIRONMENT "OMP_NUM_THREADS=2"',
                         'COMMAND paper_full_eight_square_contract_test ' + NEW_FLAG):
                require(bits in new_cmake[len(old_cmake):], 'new CTest mode/properties')
            require('add_executable(paper_full_eight_square_contract_test EXCLUDE_FROM_ALL' in old_cmake, 'excluded target preserved')
            api = ['relin2', 'rs2', 'mult2', 'add', 'sub']
            for target in api:
                require(f'add_executable({target}_api_contract_test EXCLUDE_FROM_ALL' in old_cmake, 'five unchanged API targets')
            excluded57 = {'precision_client_io_first_mult2_contract', 'repeated_mult2_semantic_two_square_contract',
                          'paper_h128_client_keypair_contract', 'paper_full_eight_square_contract', NEW_TEST}
            require(len([t for t in new_names if t not in excluded57]) == 57 and
                    len([t for t in new_names if t not in {NEW_TEST, 'paper_full_eight_square_contract'}]) == 60,
                    'legacy 57/60 selection counts')
            checked('all 61 old CTests unchanged; appended #62; legacy57/60 and five API target definitions preserved',
                    {'old_count': len(old_names), 'new_count': len(new_names), 'new_test': NEW_TEST, 'timeout_seconds': 1200})

            old_repeat = base['src/repeated_mult2.cpp'].decode(); new_repeat = green['src/repeated_mult2.cpp'].decode()
            old_q = literal_primes(old_repeat, 'constexpr std::array<PaperPrime,11> kPaperQ', 'constexpr PaperPrime kPaperP')
            green_q = literal_primes(new_repeat, 'constexpr std::array<PaperPrime,11> kPaperQ', 'constexpr PaperPrime kPaperP')
            old_block = old_repeat[old_repeat.index('struct PaperPrime'):old_repeat.index('[[noreturn]]')]
            require(old_block in new_repeat and old_q == green_q and len(old_q) == 11, 'original exact Q/P constants unchanged')
            require('constexpr PaperGeometryProfile kPaperProfile{kPaperQ,kPaperP,50};' in new_repeat, 'old descriptor remains metadata50')
            for signature in ('void InstallFamilyKeys(', 'void CheckSignedH128(',
                              'RepeatedMult2ClientSetup CreateRepeatedMult2DiagnosticSetup()', '~Data()'):
                require(function(old_repeat, signature) == function(new_repeat, signature), f'preserved {signature}')
            old_factory = function(old_repeat, 'RepeatedMult2ClientSetup CreatePaperRepeatedMult2Setup()')
            builder = function(new_repeat, 'RepeatedMult2ClientSetup RepeatedMult2Plan::Data::CreatePaperGeometrySetup(')
            reconstructed = builder.replace('RepeatedMult2ClientSetup RepeatedMult2Plan::Data::CreatePaperGeometrySetup(const PaperGeometryProfile& profile)',
                                            'RepeatedMult2ClientSetup CreatePaperRepeatedMult2Setup()')
            reconstructed = reconstructed.replace('std::make_unique<Data>(); data->profile=&profile;', 'std::make_unique<RepeatedMult2Plan::Data>(); data->paper=true;')
            reconstructed = reconstructed.replace('prime:profile.q', 'prime:kPaperQ').replace('MakeFamily(moduli,roots,&profile)', 'MakeFamily(moduli,roots,true)')
            require(reconstructed == old_factory, 'old factory body preserved modulo private descriptor factoring')
            require('return RepeatedMult2Plan::Data::CreatePaperGeometrySetup(kPaperProfile);' in new_repeat and
                    'return RepeatedMult2Plan::Data::CreatePaperGeometrySetup(kExperimentalPrecision116Profile);' in new_repeat,
                    'factories bound to separate immutable profiles')
            old_double = base['src/double_ckks.cpp'].decode(); new_double = green['src/double_ckks.cpp'].decode()
            require(new_double.replace('root.ValidateCiphertext(wrapped, prefix, absoluteLevel, 2, plan_->ExpectedRecordedScalingFactor(),',
                                       'root.ValidateCiphertext(wrapped, prefix, absoluteLevel, 2, std::ldexp(1.0, 100),') == old_double,
                    'only terminal root metadata check changed; all arithmetic unchanged')
            old_io = base['src/high_precision_client_io.cpp'].decode(); new_io = green['src/high_precision_client_io.cpp'].decode()
            for signature in ('BoundCiphertext HighPrecisionClientIO::BindFirstMult2Rcb(',
                              'BoundCiphertext HighPrecisionClientIO::BindRepeatedRcb(',
                              'DecodedSlots HighPrecisionClientIO::Decrypt(', 'void CheckProfile(',
                              'void CheckSharedBasis(', 'void CheckCiphertext(', 'double FreshRecorded('):
                require(function(old_io, signature) == function(new_io, signature), f'preserved {signature}')
            require('if (!plan) return PositiveRationalScale::FromPositive(ExactInteger(1) << 100, 1);' in new_io and
                    'return plan ? plan->BaseMetadataExponent() : 50;' in new_io and
                    'plan->ReceiptFor(0, RepeatedPhase::Input)->GetExactScale()' in new_io, 'narrow plan-bound scale authority')
            transform_start = 'template <class Real>\nstruct Complex final'
            require(old_io.split(transform_start, 1)[1].split('}  // namespace', 1)[0] ==
                    new_io.split(transform_start, 1)[1].split('}  // namespace', 1)[0], 'all codec transforms/rounding unchanged')
            checked('old profile, diagnostic setup, projection/cleanup, terminal-only adoption, codecs and all evaluator arithmetic preserved')
            require(base['src/paper_h128_client_keypair.cpp'] == green['src/paper_h128_client_keypair.cpp'] and
                    base['include/openfhe_2023_1788/double_ckks.h'] == green['include/openfhe_2023_1788/double_ckks.h'] and
                    base['include/openfhe_2023_1788/high_precision_client_io.h'] == green['include/openfhe_2023_1788/high_precision_client_io.h'],
                    'adapter and other public headers unchanged')
            checked('fixed-Q adapter unchanged; no evaluator private-key/public I/O API expansion')

            seam = 'project/coordination/fs-precision-profile-feasibility-01/'
            candidate = json.loads(files[seam + 'candidate.json'])
            certificate = json.loads(files[seam + 'STATIC_CERTIFICATE.json'])
            # Parse the actual new C++ descriptor, resolving only literal kPaperQ references.
            descriptor = new_repeat.split('constexpr PaperGeometryProfile kExperimentalPrecision116Profile', 1)[1].split(';', 1)[0]
            require(descriptor.endswith('}},kPaperP,58}'), 'frozen experimental P and metadata58')
            ordered = []
            for item in re.finditer(r'\{\s*(\d+)ULL\s*,\s*(\d+)ULL\s*\}|kPaperQ\[(\d+)\]', descriptor):
                ordered.append((int(item[1]), int(item[2])) if item[1] is not None else old_q[int(item[3])])
            expected = [(r['modulus'], r['root']) for r in candidate['new_primes'][:2]] + old_q[2:10] + [(candidate['new_primes'][2]['modulus'], candidate['new_primes'][2]['root'])]
            cert_q = [(r['modulus'], r['root']) for r in certificate['ordered_Q']]
            test_q = literal_primes(new_header, 'constexpr std::array<Prime, 11> kQ', 'constexpr Prime kP')
            require(ordered == expected == cert_q == test_q and len(ordered) == 11, 'all ordered Q/root literals bind exact candidate')
            p_match = re.search(r'constexpr Prime kP\{(\d+)ULL, (\d+)ULL\}', new_header)
            require(p_match is not None, 'test reserved P literal')
            reserved = (int(p_match[1]), int(p_match[2]))
            require(reserved == (certificate['reserved_P']['modulus'], certificate['reserved_P']['root']) and
                    reserved == (1152921504606584833, 4443670208963), 'exact reserved P/root')
            require('constexpr std::array<std::uint64_t, 3> kWitness{{5, 7, 11}};' in new_header, 'test witnesses')
            require(tuple(r['witness'] for r in candidate['new_primes']) == (5, 7, 11), 'candidate witnesses')
            for item in candidate['new_primes']:
                q, witness = item['modulus'], item['witness']
                multiplier = (q - 1) >> 32
                require((q - 1) % 2**32 == 0 and multiplier % 2 == 1 and 0 < multiplier < 2**32 and
                        pow(witness, (q - 1)//2, q) == q - 1, 'deterministic new-prime witness')
            for q, root in ordered + [reserved]:
                require(q.bit_length() <= 60 and q % 65536 == 1 and pow(root, 32768, q) == q - 1 and
                        pow(root, 65536, q) == 1, 'exact native root/ceiling')
            require(candidate['scale_bits'] == 116 and candidate['base_metadata_bits'] == 58 and
                    'ExactScale input(Int(1)<<(2*BaseMetadataExponent()),1);' in new_repeat and
                    'std::ldexp(1.0,2*BaseMetadataExponent())' in new_repeat, 'coherent descriptor-derived initial scale')
            q = [p[0] for p in ordered]; active = list(q); d = q[-1]
            s0 = Fraction(2**116); scale = s0; summaries = []
            for stage in range(9):
                cert_scale = certificate['scales'][stage]
                cert_value = Fraction(int(cert_scale['numerator_hex'], 16), int(cert_scale['denominator_hex'], 16))
                closed = Fraction(2**(116 * 2**stage), prod((d * q[9-j])**(2**(stage-j-1)) for j in range(stage)))
                require(scale == cert_value == closed, 'all nine exact scales, recurrence and independent product')
                summaries.append({'stage': stage, 'numerator_bits': scale.numerator.bit_length(),
                                  'denominator_bits': scale.denominator.bit_length(),
                                  'canonical_fraction_sha256': sha(f'{scale.numerator:x}/{scale.denominator:x}'.encode())})
                if stage < 8:
                    require(active == certificate['families'][stage]['Q'] and len(active) == 11-stage and
                            active[-2] == certificate['families'][stage]['consumed_modulus'], 'actual family deletion/consumption')
                    scale = scale**2 / (d * active[-2]); del active[-2]
            require(active == q[:2] + [d], 'terminal static base/divisor survival')
            require(certificate['security_status'] == 'UNRESOLVED' and certificate['E80_status'] == 'NOT_TESTED' and
                    certificate['adoption_status'] == 'NOT_ADOPTED' and certificate['intermediate_nonwrap'] == 'NOT_PROVED',
                    'conditional statuses not promoted')
            checked('candidate binding, deterministic witnesses, native roots, all families and all nine exact scales',
                    {'candidate_sha256': sha(files[seam+'candidate.json']), 'certificate_sha256': sha(files[seam+'STATIC_CERTIFICATE.json']),
                     'scales': summaries, 'scale8_over_scale0_approx': float(scale/s0),
                     'terminal_Q_over_scale8_approx': float(Fraction(q[0]*q[1])/scale), 'security': 'UNRESOLVED', 'E80': 'NOT_TESTED'})

            for path in sorted(RED_PATHS | GREEN_PATHS):
                if path.endswith(('.cpp', '.h')):
                    delimiters(green[path].decode(), path)
            checked('C++ delimiter balance after removing comments/literals (not grammar/type checking)')
            # The excerpt uses exact production constants only, without stubs,
            # OpenFHE headers, factory declarations, key generation or execution.
            excerpt = '#include <array>\n#include <cstdint>\n' + new_repeat[new_repeat.index('struct PaperPrime'):new_repeat.index('[[noreturn]]')]
            excerpt += '''static_assert(kPaperProfile.baseMetadataBits == 50);
static_assert(kPaperProfile.q[0].modulus == 1125899904679937ULL);
static_assert(kExperimentalPrecision116Profile.baseMetadataBits == 58);
static_assert(kExperimentalPrecision116Profile.q[10].modulus == 72057589742960641ULL);
static_assert(kExperimentalPrecision116Profile.q[9].root == kPaperProfile.q[9].root);
static_assert(kExperimentalPrecision116Profile.p.modulus == kPaperProfile.p.modulus);
'''
            excerpt_path = work / 'descriptor_excerpt.cpp'; excerpt_path.write_text(excerpt)
            compilers = []
            for compiler in ['g++', 'clang++']:
                binary = shutil.which(compiler)
                if binary:
                    run([binary, '-std=c++17', '-Wall', '-Wextra', '-Wpedantic', '-Werror', '-fsyntax-only', str(excerpt_path)],
                        work, f'{compiler} constants-only excerpt syntax; NOT project compilation')
                    compilers.append(compiler)
            report['descriptor_excerpt'] = {'sha256': sha(excerpt.encode()), 'bytes': len(excerpt.encode()),
                                             'compilers': compilers, 'does_not_compile_project': True}
            if compilers:
                checked('constants-only C++17 descriptor excerpt syntax', compilers)
            else:
                report['checks'].append({'name': 'constants-only excerpt syntax', 'result': 'NOT COMPILED', 'detail': 'no compiler'})

            # Run exactly the supplied seven stdlib scalar certificate tests.
            static_dir = replay / 'coordination/fs-precision-profile-feasibility-01'
            supplied = run([sys.executable, '-B', '-m', 'unittest', '-v', 'test_static_profile'], static_dir,
                           'supplied seven scalar certificate tests; no crypto')
            require('Ran 7 tests' in supplied.stderr and '\nOK\n' in supplied.stderr, 'seven static tests completed')
            emitted = run([sys.executable, '-B', '-I', 'static_profile.py'], static_dir, 'supplied standalone scalar certificate')
            require(emitted.stdout.encode() == files[seam+'STATIC_CERTIFICATE.json'], 'standalone certificate identical bytes')
            checked('supplied seven scalar tests and exact standalone certificate bytes')
            require(tree(replay) == green, 'checks did not modify the applied project tree')
            checked('all non-allowlisted project/evidence/workflow files unchanged; applied tree unchanged by checks')
        # Re-read original archive to detect any accidental input replacement.
        require(args.input_zip.read_bytes() == raw, 'original archive unchanged')
        checked('original archive unchanged at end')
        report['result'] = 'STATIC_DRAFT_CHECKS_PASS'
        report['pending'] = {'project_compilation': 'NOT COMPILED', 'actual_hosted_compile_RED': 'NOT RUN',
                             'candidate_one_Mult2': 'NOT RUN', 'legacy60_and_five_APIs': 'NOT RUN',
                             'full_eight_square_E80': 'NOT TESTED; original failure unchanged',
                             'security': 'UNRESOLVED', 'independent_review': 'NOT PERFORMED'}
    except Exception as error:
        report['result'] = 'STATIC_DRAFT_CHECKS_FAIL'
        report['error'] = f'{type(error).__name__}: {error}'
        raise
    finally:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(report, indent=2) + '\n')
        print(json.dumps({'result': report.get('result', 'INCOMPLETE'), 'checks': len(report['checks']),
                          'report': str(args.report), 'error': report.get('error')}, indent=2))


if __name__ == '__main__':
    main()
