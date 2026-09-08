#include "openfhe_2023_1788/public_s100_encoding_probe.h"
#include "paper_full_eight_square_oracle.h"
#include <boost/version.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#ifndef OPENFHE1788_SOURCE_COMMIT
#define OPENFHE1788_SOURCE_COMMIT "unknown"
#endif
namespace io = openfhe_2023_1788::client_io;
namespace fs = std::filesystem;
namespace {
void Quoted(std::ostream& out, const std::string& value) {
    out << '"';
    for (unsigned char c : value) {
        if (c == '"' || c == '\\') out << '\\' << static_cast<char>(c);
        else if (c < 32) throw std::runtime_error("control character in output metadata");
        else out << static_cast<char>(c);
    }
    out << '"';
}
void Strings(std::ostream& out, const std::vector<std::string>& values) {
    out << '[';
    for (std::size_t i=0;i<values.size();++i) {
        if (i) out << ',';
        Quoted(out, values[i]);
    }
    out << ']';
}
}
int main(int argc, char** argv) {
    try {
        const std::string sourceCommit=OPENFHE1788_SOURCE_COMMIT;
        if (sourceCommit.size()!=40 || sourceCommit.find_first_not_of("0123456789abcdef")!=std::string::npos)
            throw std::runtime_error("build source identity must be 40 lowercase hexadecimal characters");
        if (argc==2 && std::string(argv[1])=="--metadata") {
            std::cout << "{\"build_source_commit\":";
            Quoted(std::cout, sourceCommit);
            std::cout << ",\"encoding_calls\":0,\"crypto_calls\":0}\n";
            return 0;
        }
        if (argc==2 && std::string(argv[1])=="--api-negative") {
            bool rejected=false;
            try { (void)io::diagnostic::InspectFixedS100PublicEncoding({}); }
            catch (const std::invalid_argument&) { rejected=true; }
            if (!rejected) throw std::runtime_error("wrong-shape input was accepted");
            std::cout << "API_NEGATIVE_PASS encoding_calls=0\n";
            return 0;
        }
        if (argc!=2) throw std::invalid_argument("usage: public_s100_encoding_dump OUTPUT.json | --api-negative | --metadata");
        const fs::path destination(argv[1]);
        if (fs::exists(destination)) throw std::runtime_error("refusing to overwrite output");
        // The only original-input preparation and the only ComputeEncoding call.
        // No setup, crypto context, key, sampling, encryption or decryption.
        const auto values=paper_full_test::ClientInputs(paper_full_test::Inputs());
        const auto inspection=io::diagnostic::InspectFixedS100PublicEncoding(values);
        if (inspection.signedCoefficients.size()!=32768 || inspection.slots!=16384 || inspection.strideGap!=1)
            throw std::runtime_error("unexpected inspection geometry");
        std::ofstream out(destination, std::ios::binary | std::ios::out);
        out.exceptions(std::ios::badbit | std::ios::failbit);
        out << "{\n\"schema\":\"initial-lift-public-encoding-v1\",\n"
               "\"profile\":\"original-s100-near-unit-v1\",\n"
               "\"base_source_commit\":\"a4b815a733efe81897325e2a8e4c826a4ebfa439\",\n"
               "\"build_source_commit\":";
        Quoted(out, OPENFHE1788_SOURCE_COMMIT);
        out << ",\n\"boost_version\":"; Quoted(out, BOOST_LIB_VERSION);
        out << ",\n\"compiler\":"; Quoted(out, __VERSION__);
        out << ",\n\"n\":32768,\n\"slots\":16384,\n\"gap\":1,\n\"cyclotomic_order\":65536,\n"
               "\"scale_num\":";
        Quoted(out, inspection.logicalScale.Numerator().convert_to<std::string>());
        out << ",\n\"scale_den\":";
        Quoted(out, inspection.logicalScale.Denominator().convert_to<std::string>());
        out << ",\n\"full_moduli\":"; Strings(out, inspection.basis.moduliDecimal);
        out << ",\n\"full_roots\":"; Strings(out, inspection.basis.rootsOfUnityDecimal);
        out << ",\n\"encoding_calls\":1,\n\"crypto_calls\":0,\n\"coefficients\":[";
        for (std::size_t i=0;i<inspection.signedCoefficients.size();++i) {
            if(i) out << ',';
            Quoted(out, inspection.signedCoefficients[i].convert_to<std::string>());
        }
        out << "]\n}\n";
        out.flush(); out.close();
        std::cout << "PUBLIC_ENCODING_WRITTEN encoding_calls=1 crypto_calls=0\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "PUBLIC_ENCODING_FAILED: " << error.what() << '\n';
        return 1;
    }
}
