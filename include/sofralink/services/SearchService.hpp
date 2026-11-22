#pragma once
#include <string>
#include <vector>
#include "sofralink/models/Donation.hpp"
#include "sofralink/models/FoodRequest.hpp"
namespace sofralink {
struct DonationSearchCriteria {
    std::string category;
    std::string location;
    DonationStatus status{DonationStatus::Submitted};
    bool filterByStatus{false};
    bool includeExpired{false};
};
struct RequestSearchCriteria {
    std::string category;
    std::string organizationName;
    std::string location;
    RequestStatus status{RequestStatus::Pending};
    bool filterByStatus{false};
    int minUrgencyLevel{1};
};
class SearchService {
public:
    std::vector<Donation> searchDonations(const std::vector<Donation>&, const DonationSearchCriteria&) const;
    std::vector<FoodRequest> searchRequests(const std::vector<FoodRequest>&, const RequestSearchCriteria&) const;
    std::vector<Donation> sortDonationsByExpiry(const std::vector<Donation>&) const;
};
} // namespace sofralink
