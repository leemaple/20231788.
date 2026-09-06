#ifndef PAPER_ENDPOINT_EVIDENCE_WRITER_H
#define PAPER_ENDPOINT_EVIDENCE_WRITER_H

#include "paper_endpoint_diagnostics.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <string>

namespace paper_endpoint_contract {

// These values are supplied by the caller from its already established run
// boundary. The writer never reads environment variables or repository state.
struct EndpointEvidenceIdentity final {
    std::string scope;              // "live-single-chain" or "synthetic"
    std::string sourceCommit;       // actual compiled source, lowercase SHA-1
    std::string host;               // "linux" or "windows"
    std::string githubRunId;        // positive canonical decimal
    std::string githubRunAttempt;   // positive canonical decimal
    std::uint64_t boostVersion;     // actual BOOST_VERSION
};

// A live caller sets ownerCleanupConfirmed only after the existing owner/cache
// cleanup checks. numericGateFailures is observed, never inferred by this API.
struct EndpointPublicationBoundary final {
    std::size_t numericGateFailures;
    bool ownerCleanupConfirmed;
};

struct EndpointEvidenceFile final {
    std::filesystem::path readyPath;
    std::uintmax_t canonicalBytes;
};

// exclusiveTrustedParent must be an existing, absolute, normalized,
// non-symlink directory controlled exclusively by this invocation. The writer
// claims a new identity directory below it and stages within that directory,
// so source and destination are on the same filesystem. Publication uses one
// filesystem rename; its atomicity is the host filesystem's contract. C++17
// does not provide crash-durability or a hostile-parent race guarantee.
EndpointEvidenceFile WriteEndpointEvidence(
    const EndpointEvidence& evidence,
    const EndpointEvidenceIdentity& identity,
    const EndpointPublicationBoundary& boundary,
    const std::filesystem::path& exclusiveTrustedParent);

// Strict independent parser/semantic validator used by the writer after close
// and reopen, and exposed as the filesystem test seam. Expected failures use
// EndpointFailure with the adopted reason token; unexpected invariants are not
// caught or converted into a successful publication.
void ValidateEndpointEvidenceFile(
    const std::filesystem::path& path,
    const EndpointEvidence& expectedEvidence,
    const EndpointEvidenceIdentity& expectedIdentity,
    const EndpointPublicationBoundary& expectedBoundary);

// Separate from the legacy output path. Future integration may call this after
// cleanup and before its unchanged numeric-failure Require.
void EmitEndpointEvidencePrimary(
    std::ostream& output,
    const EndpointEvidence& evidence,
    const EndpointEvidenceIdentity& identity,
    const EndpointPublicationBoundary& boundary);

} // namespace paper_endpoint_contract
#endif
