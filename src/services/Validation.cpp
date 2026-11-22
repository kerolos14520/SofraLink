#include "sofralink/services/Validation.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace sofralink {
namespace {

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

void addError(ValidationResult& result, const std::string& message) {
    result.isValid = false;
    result.errors.push_back(message);
}

} // namespace

ValidationResult Validation::validateDonation(const Donation& donation) const {
    ValidationResult result;

    if (donation.donorName.empty()) {
        addError(result, "donorName must not be empty.");
    }
    if (donation.category.empty()) {
        addError(result, "category must not be empty.");
    }
    if (donation.unit.empty()) {
        addError(result, "unit must not be empty.");
    }
    if (donation.location.empty()) {
        addError(result, "location must not be empty.");
    }
    if (!donation.isSealed) {
        addError(result, "Donation must be sealed.");
    }
    if (!donation.isNonPerishable) {
        addError(result, "Donation must be non-perishable.");
    }
    if (donation.totalQuantity <= 0) {
        addError(result, "totalQuantity must be greater than zero.");
    }
    if (donation.availableQuantity < 0 ||
        donation.availableQuantity > donation.totalQuantity) {
        addError(result, "availableQuantity must be between zero and totalQuantity.");
    }
    if (!isValidIsoDate(donation.expiryDate)) {
        addError(result, "expiryDate must be a valid YYYY-MM-DD date.");
    } else if (donation.expiryDate < todayIsoDate()) {
        addError(result, "expiryDate must not be in the past.");
    }

    if (donation.productBatchNumber.empty()) {
        result.warnings.push_back("productBatchNumber is missing.");
    }

    return result;
}

ValidationResult Validation::validateFoodRequest(const FoodRequest& request) const {
    ValidationResult result;

    if (request.organizationName.empty()) {
        addError(result, "organizationName must not be empty.");
    }
    if (request.category.empty()) {
        addError(result, "category must not be empty.");
    }
    if (request.unit.empty()) {
        addError(result, "unit must not be empty.");
    }
    if (request.location.empty()) {
        addError(result, "location must not be empty.");
    }
    if (request.requestedQuantity <= 0) {
        addError(result, "requestedQuantity must be greater than zero.");
    }
    if (request.remainingQuantity < 0 ||
        request.remainingQuantity > request.requestedQuantity) {
        addError(result, "remainingQuantity must be between zero and requestedQuantity.");
    }
    if (request.urgencyLevel < 1 || request.urgencyLevel > 5) {
        addError(result, "urgencyLevel must be between 1 and 5.");
    }
    if (!isValidIsoDate(request.neededByDate)) {
        addError(result, "neededByDate must be a valid YYYY-MM-DD date.");
    }

    return result;
}

} // namespace sofralink
