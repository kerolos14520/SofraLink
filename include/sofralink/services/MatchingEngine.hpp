#pragma once
#include <vector>
#include "sofralink/models/Donation.hpp"
#include "sofralink/models/FoodRequest.hpp"
#include "sofralink/models/MatchResult.hpp"
namespace sofralink {
class MatchingEngine {
public:
    std::vector<MatchResult> generateMatches(const std::vector<Donation>& donations,
                                             const std::vector<FoodRequest>& requests) const;
};
} // namespace sofralink
