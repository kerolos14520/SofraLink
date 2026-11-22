#pragma once
#include "sofralink/models/Donation.hpp"
#include "sofralink/models/FoodRequest.hpp"
#include "sofralink/models/ValidationResult.hpp"
namespace sofralink {
class Validation {
public:
    ValidationResult validateDonation(const Donation&) const;
    ValidationResult validateFoodRequest(const FoodRequest&) const;
};
} // namespace sofralink
