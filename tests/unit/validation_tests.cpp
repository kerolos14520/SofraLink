#include "sofralink/services/Validation.hpp"

#include <cassert>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

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


    // ---- Extra coverage: whitespace-only, leap years, formats, boundaries ----
    {
        Donation good;
        good.donorName = "Sample Donor";
        good.category = "Rice";
        good.totalQuantity = 10;
        good.availableQuantity = 10;
        good.unit = "kg";
        good.expiryDate = "2099-12-31";
        good.location = "Sample City";
        good.productBatchNumber = "B-1";
        good.isSealed = true;
        good.isNonPerishable = true;
        assert(validation.validateDonation(good).isValid);
        assert(validation.validateDonation(good).errors.empty());
        assert(validation.validateDonation(good).warnings.empty());

        // Whitespace-only strings are rejected like empty ones.
        Donation d = good;
        d.donorName = "   ";
        assert(!validation.validateDonation(d).isValid);
        d = good; d.category = "\t ";
        assert(!validation.validateDonation(d).isValid);
        d = good; d.unit = " ";
        assert(!validation.validateDonation(d).isValid);
        d = good; d.location = "\n";
        assert(!validation.validateDonation(d).isValid);

        // Missing/blank batch number is only a warning, never an error.
        d = good; d.productBatchNumber = "  ";
        assert(validation.validateDonation(d).isValid);
        assert(validation.validateDonation(d).warnings.size() == 1);
        assert(validation.validateDonation(d).errors.empty());

        // Errors make the result invalid and are reported in `errors`.
        d = good; d.isSealed = false;
        assert(!validation.validateDonation(d).isValid);
        assert(!validation.validateDonation(d).errors.empty());

        // Leap years: 2096 and 2400 are leap years, 2100 and 2099 are not.
        d = good; d.expiryDate = "2096-02-29";
        assert(validation.validateDonation(d).isValid);
        d.expiryDate = "2400-02-29";
        assert(validation.validateDonation(d).isValid);
        d.expiryDate = "2100-02-29";
        assert(!validation.validateDonation(d).isValid);
        d.expiryDate = "2099-02-29";
        assert(!validation.validateDonation(d).isValid);

        // Invalid date formats / calendar values.
        const char* badDates[] = {"", "2099-13-01", "2099-00-10", "2099-04-31", "2099-12-00",
                                  "31-12-2099", "2099/12/31", "2099-1-1", "20991231", "abcd-ef-gh"};
        for (const char* bad : badDates) {
            d = good;
            d.expiryDate = bad;
            assert(!validation.validateDonation(d).isValid);
        }

        // Past vs today: today is still acceptable, yesterday-or-earlier is not.
        std::time_t now = std::time(nullptr);
        std::tm local{};
#ifdef _WIN32
        localtime_s(&local, &now);
#else
        localtime_r(&now, &local);
#endif
        std::ostringstream today;
        today << std::put_time(&local, "%Y-%m-%d");
        d = good; d.expiryDate = today.str();
        assert(validation.validateDonation(d).isValid);
        d.expiryDate = "1999-12-31";
        assert(!validation.validateDonation(d).isValid);

        // Quantity boundaries.
        d = good; d.totalQuantity = 1; d.availableQuantity = 1;
        assert(validation.validateDonation(d).isValid);
        d = good; d.availableQuantity = 0;
        assert(validation.validateDonation(d).isValid);
        d = good; d.availableQuantity = d.totalQuantity;
        assert(validation.validateDonation(d).isValid);
        d = good; d.availableQuantity = d.totalQuantity + 1;
        assert(!validation.validateDonation(d).isValid);
        d = good; d.availableQuantity = -1;
        assert(!validation.validateDonation(d).isValid);
        d = good; d.totalQuantity = -5;
        assert(!validation.validateDonation(d).isValid);

        // ---- FoodRequest ----
        FoodRequest r;
        r.organizationName = "Sample Organization";
        r.category = "Rice";
        r.requestedQuantity = 10;
        r.remainingQuantity = 10;
        r.unit = "kg";
        r.urgencyLevel = 3;
        r.location = "Sample City";
        r.neededByDate = "2099-12-31";
        assert(validation.validateFoodRequest(r).isValid);

        FoodRequest q = r;
        q.organizationName = "  ";
        assert(!validation.validateFoodRequest(q).isValid);
        q = r; q.category = " ";
        assert(!validation.validateFoodRequest(q).isValid);
        q = r; q.unit = "";
        assert(!validation.validateFoodRequest(q).isValid);
        q = r; q.location = "\t";
        assert(!validation.validateFoodRequest(q).isValid);

        q = r; q.urgencyLevel = 1;
        assert(validation.validateFoodRequest(q).isValid);
        q.urgencyLevel = 5;
        assert(validation.validateFoodRequest(q).isValid);
        q.urgencyLevel = 0;
        assert(!validation.validateFoodRequest(q).isValid);
        q.urgencyLevel = 6;
        assert(!validation.validateFoodRequest(q).isValid);

        q = r; q.remainingQuantity = 0;
        assert(validation.validateFoodRequest(q).isValid);
        q = r; q.remainingQuantity = -1;
        assert(!validation.validateFoodRequest(q).isValid);
        q = r; q.remainingQuantity = q.requestedQuantity + 1;
        assert(!validation.validateFoodRequest(q).isValid);
        q = r; q.requestedQuantity = -3;
        assert(!validation.validateFoodRequest(q).isValid);
        q = r; q.neededByDate = "2099-02-30";
        assert(!validation.validateFoodRequest(q).isValid);
        q = r; q.neededByDate = "2096-02-29";
        assert(validation.validateFoodRequest(q).isValid);
    }

    return 0;
}
