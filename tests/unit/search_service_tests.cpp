#include "sofralink/services/SearchService.hpp"

#include <cassert>
#include <vector>

int main() {
    using namespace sofralink;
    const SearchService search;

    Donation rice;
    rice.id = 1;
    rice.category = "Rice";
    rice.location = "North City";
    rice.expiryDate = "2099-03-01";
    rice.status = DonationStatus::Accepted;
    Donation pasta = rice;
    pasta.id = 2;
    pasta.category = "Pasta";
    pasta.expiryDate = "2099-01-01";
    Donation expired = rice;
    expired.id = 3;
    expired.expiryDate = "2000-01-01";
    Donation invalidDate = rice;
    invalidDate.id = 4;
    invalidDate.expiryDate = "2099-02-29";
    const std::vector<Donation> donations{rice, pasta, expired, invalidDate};

    DonationSearchCriteria donationCriteria;
    donationCriteria.category = "ri";
    donationCriteria.location = "north";
    donationCriteria.filterByStatus = true;
    donationCriteria.status = DonationStatus::Accepted;
    const auto foundDonations = search.searchDonations(donations, donationCriteria);
    assert(foundDonations.size() == 1 && foundDonations.front().id == 1);
    assert(donations.size() == 4 && donations.front().id == 1);

    donationCriteria.includeExpired = true;
    const auto includingExpired = search.searchDonations(donations, donationCriteria);
    assert(includingExpired.size() == 3);

    const auto sorted = search.sortDonationsByExpiry(donations);
    assert(sorted.size() == 4);
    assert(sorted[0].id == 3 && sorted[1].id == 2 && sorted[2].id == 1 && sorted[3].id == 4);
    assert(donations[0].id == 1 && donations[1].id == 2 && donations[2].id == 3 &&
           donations[3].id == 4);

    FoodRequest request;
    request.id = 10;
    request.organizationName = "Helping Hands";
    request.category = "Rice";
    request.location = "North City";
    request.urgencyLevel = 4;
    request.status = RequestStatus::Pending;
    FoodRequest lowUrgency = request;
    lowUrgency.id = 11;
    lowUrgency.urgencyLevel = 2;
    FoodRequest other = request;
    other.id = 12;
    other.organizationName = "Other Group";
    const std::vector<FoodRequest> requests{request, lowUrgency, other};

    RequestSearchCriteria requestCriteria;
    requestCriteria.category = "rice";
    requestCriteria.organizationName = "HELPING";
    requestCriteria.location = "north";
    requestCriteria.filterByStatus = true;
    requestCriteria.status = RequestStatus::Pending;
    requestCriteria.minUrgencyLevel = 3;
    const auto foundRequests = search.searchRequests(requests, requestCriteria);
    assert(foundRequests.size() == 1 && foundRequests.front().id == 10);
    assert(requests.size() == 3 && requests.front().id == 10);


    // ---- Extra coverage ----
    {
        // Empty criteria: everything that is currently valid (not expired, real date).
        const DonationSearchCriteria none;
        const auto all = search.searchDonations(donations, none);
        assert(all.size() == 2 && all[0].id == 1 && all[1].id == 2);

        // includeExpired also returns expired and invalid-date records.
        DonationSearchCriteria everything;
        everything.includeExpired = true;
        assert(search.searchDonations(donations, everything).size() == 4);

        // No match -> empty vector, no crash; empty input -> empty vector.
        DonationSearchCriteria nothing;
        nothing.category = "does-not-exist";
        assert(search.searchDonations(donations, nothing).empty());
        assert(search.searchDonations({}, none).empty());

        // Status filter only applies when filterByStatus is true.
        DonationSearchCriteria statusIgnored;
        statusIgnored.status = DonationStatus::Delivered;
        assert(search.searchDonations(donations, statusIgnored).size() == 2);
        statusIgnored.filterByStatus = true;
        assert(search.searchDonations(donations, statusIgnored).empty());

        // Case-insensitive location + category.
        DonationSearchCriteria ci;
        ci.category = "PASTA";
        ci.location = "NORTH CITY";
        const auto pastaOnly = search.searchDonations(donations, ci);
        assert(pastaOnly.size() == 1 && pastaOnly.front().id == 2);

        // Stable sort: equal dates keep input order; invalid dates go last; input untouched.
        Donation a = rice; a.id = 21; a.expiryDate = "2099-05-05";
        Donation b = rice; b.id = 22; b.expiryDate = "2099-05-05";
        Donation c = rice; c.id = 23; c.expiryDate = "2099-04-04";
        Donation bad1 = rice; bad1.id = 24; bad1.expiryDate = "garbage";
        Donation bad2 = rice; bad2.id = 25; bad2.expiryDate = "";
        const std::vector<Donation> input{a, bad1, b, bad2, c};
        const auto out = search.sortDonationsByExpiry(input);
        assert(out.size() == 5);
        assert(out[0].id == 23 && out[1].id == 21 && out[2].id == 22);
        assert(out[3].id == 24 && out[4].id == 25);
        assert(input[0].id == 21 && input[1].id == 24 && input[2].id == 22 &&
               input[3].id == 25 && input[4].id == 23);
        assert(search.sortDonationsByExpiry({}).empty());

        // Requests: empty criteria returns all; minUrgencyLevel boundary; filter combos.
        FoodRequest r1; r1.id = 1; r1.organizationName = "Alpha"; r1.category = "Rice";
        r1.location = "East"; r1.urgencyLevel = 1; r1.status = RequestStatus::Pending;
        FoodRequest r2 = r1; r2.id = 2; r2.urgencyLevel = 5; r2.status = RequestStatus::Approved;
        const std::vector<FoodRequest> reqs{r1, r2};
        const RequestSearchCriteria noReq;
        assert(search.searchRequests(reqs, noReq).size() == 2);
        RequestSearchCriteria urgent;
        urgent.minUrgencyLevel = 5;
        assert(search.searchRequests(reqs, urgent).size() == 1);
        RequestSearchCriteria byStatus;
        byStatus.filterByStatus = true;
        byStatus.status = RequestStatus::Approved;
        assert(search.searchRequests(reqs, byStatus).front().id == 2);
        RequestSearchCriteria miss;
        miss.organizationName = "zzz";
        assert(search.searchRequests(reqs, miss).empty());
        assert(search.searchRequests({}, noReq).empty());
    }

    return 0;
}
