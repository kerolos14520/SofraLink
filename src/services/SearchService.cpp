#include "sofralink/services/SearchService.hpp"

#include <algorithm>
#include <cctype>
#include <string>

#include "DateUtils.hpp"

namespace sofralink {
namespace {

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool containsIgnoreCase(const std::string& value, const std::string& query) {
    return query.empty() || lower(value).find(lower(query)) != std::string::npos;
}

// Expiry policy (explicit): a donation is "currently valid" only if its expiryDate
// is a real calendar date that is today or later. Records with an invalid expiryDate
// are NOT considered valid, so they are returned only when includeExpired == true.
bool hasValidUnexpiredDate(const Donation& donation) {
    return detail::isValidIsoDate(donation.expiryDate) &&
           donation.expiryDate >= detail::todayIsoDate();
}

} // namespace

std::vector<Donation> SearchService::searchDonations(
    const std::vector<Donation>& donations,
    const DonationSearchCriteria& criteria) const {
    std::vector<Donation> results;
    results.reserve(donations.size());

    for (const Donation& donation : donations) {
        if (!containsIgnoreCase(donation.category, criteria.category) ||
            !containsIgnoreCase(donation.location, criteria.location) ||
            (criteria.filterByStatus && donation.status != criteria.status) ||
            (!criteria.includeExpired && !hasValidUnexpiredDate(donation))) {
            continue;
        }
        results.push_back(donation);
    }

    return results;
}

std::vector<FoodRequest> SearchService::searchRequests(
    const std::vector<FoodRequest>& requests,
    const RequestSearchCriteria& criteria) const {
    std::vector<FoodRequest> results;
    results.reserve(requests.size());

    for (const FoodRequest& request : requests) {
        if (!containsIgnoreCase(request.category, criteria.category) ||
            !containsIgnoreCase(request.organizationName, criteria.organizationName) ||
            !containsIgnoreCase(request.location, criteria.location) ||
            (criteria.filterByStatus && request.status != criteria.status) ||
            request.urgencyLevel < criteria.minUrgencyLevel) {
            continue;
        }
        results.push_back(request);
    }

    return results;
}

std::vector<Donation> SearchService::sortDonationsByExpiry(
    const std::vector<Donation>& donations) const {
    // Copy so the caller's vector is never modified. Valid dates first (ascending),
    // records with an invalid expiryDate last; stable so equal dates keep input order.
    std::vector<Donation> sorted = donations;
    std::stable_sort(sorted.begin(), sorted.end(), [](const Donation& left, const Donation& right) {
        const bool leftValid = detail::isValidIsoDate(left.expiryDate);
        const bool rightValid = detail::isValidIsoDate(right.expiryDate);
        if (leftValid != rightValid) {
            return leftValid;
        }
        return leftValid && left.expiryDate < right.expiryDate;
    });
    return sorted;
}

} // namespace sofralink
