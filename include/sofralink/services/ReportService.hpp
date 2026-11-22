#pragma once
#include <vector>
#include "sofralink/models/Donation.hpp"
#include "sofralink/models/FoodRequest.hpp"
#include "sofralink/models/MatchResult.hpp"
#include "sofralink/models/ReportSummary.hpp"
namespace sofralink {
class ReportService {
public:
    ReportSummary generateSummary(const std::vector<Donation>& donations,
                                  const std::vector<FoodRequest>& requests,
                                  const std::vector<MatchResult>& matches) const;
};
} // namespace sofralink
