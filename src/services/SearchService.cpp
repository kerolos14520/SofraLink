#include "sofralink/services/SearchService.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

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

bool isValidIsoDate(const std::string& value) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') {
        return false;
    }
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (i != 4 && i != 7 && (value[i] < '0' || value[i] > '9')) {
            return false;
        }
    }
    const int year = std::stoi(value.substr(0, 4));
    const int month = std::stoi(value.substr(5, 2));
    const int day = std::stoi(value.substr(8, 2));
    if (month < 1 || month > 12) {
        return false;
    }
    const bool leapYear = year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
    const int daysPerMonth[] = {31, leapYear ? 29 : 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31};
    return day >= 1 && day <= daysPerMonth[month - 1];
}

std::string todayIsoDate() {
    const std::time_t now = std::time(nullptr);
    std::tm localDate{};
#ifdef _WIN32
    localtime_s(&localDate, &now);
#else
    localtime_r(&now, &localDate);
#endif
    std::ostringstream output;
    output << std::put_time(&localDate, "%Y-%m-%d");
    return output.str();
}

bool isExpired(const Donation& donation) {
    return !isValidIsoDate(donation.expiryDate) || donation.expiryDate < todayIsoDate();
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
            (!criteria.includeExpired && isExpired(donation))) {
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
    std::vector<Donation> sorted = donations;
    std::stable_sort(sorted.begin(), sorted.end(), [](const Donation& left, const Donation& right) {
        return left.expiryDate < right.expiryDate;
    });
    return sorted;
}

} // namespace sofralink
