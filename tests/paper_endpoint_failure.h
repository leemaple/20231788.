#ifndef PAPER_ENDPOINT_FAILURE_H
#define PAPER_ENDPOINT_FAILURE_H

#include <stdexcept>
#include <string>

namespace paper_endpoint_contract {
// Test-local outcome translation, not recovery. The outer executable emits the
// first cause once; synthetic rejection tests do not emit live failure records.
class EndpointFailure final : public std::runtime_error {
public:
    EndpointFailure(const char* reason, const std::string& detail)
        : std::runtime_error(std::string("endpoint ") + reason + ": " + detail),
          reason_(reason) {}
    const std::string& Reason() const noexcept { return reason_; }
private:
    std::string reason_;
};

[[noreturn]] inline void FailEndpoint(const char* reason, const std::string& detail) {
    throw EndpointFailure(reason, detail);
}
} // namespace paper_endpoint_contract
#endif
