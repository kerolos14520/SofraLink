#include "sofralink/services/Validation.hpp"

#include <cassert>

int main() {
    using namespace sofralink;
    const Validation validation;

    Donation donation;
    donation.donorName = "Sample Donor";
    donation.category = "Rice";
    donation.totalQuantity = 20;
    donation.availableQuantity = 12;
    donation.unit = "kg";
    donation.expiryDate = "2099-12-31";
    donation.location = "Sample City";
    donation.isSealed = true;
    donation.isNonPerishable = true;
    assert(validation.validateDonation(donation).isValid);
    assert(validation.validateDonation(donation).warnings.size() == 1);

    donation.productBatchNumber = "BATCH-1";
    donation.isSealed = false;
    assert(!validation.validateDonation(donation).isValid);
    donation.isSealed = true;
    donation.isNonPerishable = false;
    assert(!validation.validateDonation(donation).isValid);
    donation.isNonPerishable = true;
    donation.totalQuantity = 0;
    assert(!validation.validateDonation(donation).isValid);
    donation.totalQuantity = 20;
    donation.availableQuantity = 21;
    assert(!validation.validateDonation(donation).isValid);
    donation.availableQuantity = 1;
    donation.expiryDate = "2099-02-29";
    assert(!validation.validateDonation(donation).isValid);
    donation.expiryDate = "2000-01-01";
    assert(!validation.validateDonation(donation).isValid);

    FoodRequest request;
    request.organizationName = "Sample Organization";
    request.category = "Rice";
    request.requestedQuantity = 30;
    request.remainingQuantity = 10;
    request.unit = "kg";
    request.urgencyLevel = 5;
    request.location = "Sample City";
    request.neededByDate = "2099-12-31";
    assert(validation.validateFoodRequest(request).isValid);

    request.urgencyLevel = 0;
    assert(!validation.validateFoodRequest(request).isValid);
    request.urgencyLevel = 6;
    assert(!validation.validateFoodRequest(request).isValid);
    request.urgencyLevel = 1;
    request.requestedQuantity = 0;
    assert(!validation.validateFoodRequest(request).isValid);
    request.requestedQuantity = 30;
    request.remainingQuantity = 31;
    assert(!validation.validateFoodRequest(request).isValid);
    request.remainingQuantity = 1;
    request.neededByDate = "not-a-date";
    assert(!validation.validateFoodRequest(request).isValid);

    return 0;
}
