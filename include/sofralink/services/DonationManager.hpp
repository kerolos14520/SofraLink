#pragma once
#include <optional>
#include <string>
#include <vector>
#include "sofralink/models/Donation.hpp"
#include "sofralink/services/Validation.hpp"
namespace sofralink {
class DonationManager {
public:
    explicit DonationManager(const Validation& validation);
    bool createDonation(const Donation&, Donation& createdDonation, std::string& errorMessage);
    bool updateDonation(int donationId, const Donation& updatedDonation, std::string& errorMessage);
    bool cancelDonation(int donationId, std::string& errorMessage);
    bool updateDonationStatus(int donationId, DonationStatus newStatus, std::string& errorMessage);
    bool allocateQuantity(int donationId, int allocatedQuantity, std::string& errorMessage);
    std::optional<Donation> getDonationById(int donationId) const;
    std::vector<Donation> getAllDonations() const;
    std::vector<Donation> getAvailableDonations() const;
    void replaceAllDonations(const std::vector<Donation>& donations);
};
} // namespace sofralink
