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
    assert(sorted[0].id == 3 && sorted[1].id == 2 && sorted[2].id == 4 && sorted[3].id == 1);
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

    return 0;
}
