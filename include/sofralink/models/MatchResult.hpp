#pragma once
#include <string>
#include "sofralink/models/Enums.hpp"
namespace sofralink {
struct MatchResult {
    int matchId{0};
    int donationId{0};
    int requestId{0};
    int allocatedQuantity{0};
    std::string explanation;
    MatchStatus status{MatchStatus::Proposed};
    std::string createdAt;
};
} // namespace sofralink
