#include "sofralink/services/Validation.hpp"

#include <string>

#include "DateUtils.hpp"

namespace sofralink {
namespace {

using detail::isBlank;
using detail::isValidIsoDate;
using detail::todayIsoDate;

void addError(ValidationResult& result, const std::string& message) {
    result.isValid = false;
    result.errors.push_back(message);
}

} // namespace

ValidationResult Validation::validateDonation(const Donation& donation) const {
    ValidationResult result;

    if (isBlank(donation.donorName)) {
        addError(result, "donorName must not be empty.");
    }
    if (isBlank(donation.category)) {
        addError(result, "category must not be empty.");
    }
    if (isBlank(donation.unit)) {
        addError(result, "unit must not be empty.");
    }
    if (isBlank(donation.location)) {
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

    if (isBlank(donation.productBatchNumber)) {
        result.warnings.push_back("productBatchNumber is missing.");
    }

    return result;
}

ValidationResult Validation::validateFoodRequest(const FoodRequest& request) const {
    ValidationResult result;

    if (isBlank(request.organizationName)) {
        addError(result, "organizationName must not be empty.");
    }
    if (isBlank(request.category)) {
        addError(result, "category must not be empty.");
    }
    if (isBlank(request.unit)) {
        addError(result, "unit must not be empty.");
    }
    if (isBlank(request.location)) {
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
