#include "sofralink/services/DonationManager.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>

namespace sofralink {
namespace {

// Delivered, Cancelled and Rejected records are final: they cannot be edited.
bool isTerminalStatus(DonationStatus status) {
    return status == DonationStatus::Delivered || status == DonationStatus::Cancelled ||
           status == DonationStatus::Rejected;
}

std::string joinMessages(const std::vector<std::string>& messages) {
    std::string joined;
    for (const std::string& message : messages) {
        if (!joined.empty()) {
            joined += "; ";
        }
        joined += message;
    }
    return joined;
}

// Quantity rules: 0 <= availableQuantity <= totalQuantity (so total is never negative either).
bool hasValidQuantities(const Donation& donation, std::string& errorMessage) {
    if (donation.totalQuantity < 0) {
        errorMessage = "totalQuantity must not be negative.";
        return false;
    }
    if (donation.availableQuantity < 0) {
        errorMessage = "availableQuantity must not be negative.";
        return false;
    }
    if (donation.availableQuantity > donation.totalQuantity) {
        errorMessage = "availableQuantity must not exceed totalQuantity.";
        return false;
    }
    return true;
}

// Strict "YYYY-MM-DD" shape check, so string comparison equals date comparison.
bool looksLikeIsoDate(const std::string& text) {
    if (text.size() != 10 || text[4] != '-' || text[7] != '-') {
        return false;
    }
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (!std::isdigit(static_cast<unsigned char>(text[i]))) {
            return false;
        }
    }
    return true;
}

std::string todayIsoDate() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    char buffer[16];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &local);
    return buffer;
}

// A donation expiring today is still usable; it expires the day after.
// Unreadable or missing dates on perishable food are treated as expired (safe side).
bool isExpired(const Donation& donation, const std::string& today) {
    if (donation.expiryDate.empty()) {
        return !donation.isNonPerishable;
    }
    if (!looksLikeIsoDate(donation.expiryDate)) {
        return true;
    }
    return donation.expiryDate < today;
}

} // namespace

DonationManager::DonationManager(const Validation& validation) : validation_(validation) {}

Donation* DonationManager::findDonation(int donationId) {
    auto it = std::find_if(donations_.begin(), donations_.end(),
                           [donationId](const Donation& d) { return d.id == donationId; });
    return it == donations_.end() ? nullptr : &*it;
}

const Donation* DonationManager::findDonation(int donationId) const {
    auto it = std::find_if(donations_.begin(), donations_.end(),
                           [donationId](const Donation& d) { return d.id == donationId; });
    return it == donations_.end() ? nullptr : &*it;
}

bool DonationManager::validateCandidate(const Donation& candidate, std::string& errorMessage) const {
    if (!hasValidQuantities(candidate, errorMessage)) {
        return false;
    }
    const ValidationResult result = validation_.validateDonation(candidate);
    if (!result.isValid) {
        errorMessage = result.errors.empty() ? std::string("Donation failed validation.")
                                             : joinMessages(result.errors);
        return false;
    }
    return true;
}

bool DonationManager::createDonation(const Donation& donation, Donation& createdDonation,
                                     std::string& errorMessage) {
    errorMessage.clear();
    if (!validateCandidate(donation, errorMessage)) {
        return false;
    }
    Donation stored = donation;
    stored.id = nextId_++;
    donations_.push_back(stored);
    createdDonation = stored;
    return true;
}

bool DonationManager::updateDonation(int donationId, const Donation& updatedDonation,
                                     std::string& errorMessage) {
    errorMessage.clear();
    Donation* existing = findDonation(donationId);
    if (existing == nullptr) {
        errorMessage = "Donation not found.";
        return false;
    }
    if (isTerminalStatus(existing->status)) {
        errorMessage = "Donation is Delivered, Cancelled or Rejected and cannot be updated.";
        return false;
    }

    // Identity, workflow status and creation time are not editable through this method.
    Donation candidate = updatedDonation;
    candidate.id = existing->id;
    candidate.status = existing->status;
    candidate.createdAt = existing->createdAt;

    if (!validateCandidate(candidate, errorMessage)) {
        return false;
    }
    *existing = candidate;
    return true;
}

bool DonationManager::cancelDonation(int donationId, std::string& errorMessage) {
    errorMessage.clear();
    Donation* existing = findDonation(donationId);
    if (existing == nullptr) {
        errorMessage = "Donation not found.";
        return false;
    }
    if (isTerminalStatus(existing->status)) {
        errorMessage = "Donation is already Delivered, Cancelled or Rejected and cannot be cancelled.";
        return false;
    }
    existing->status = DonationStatus::Cancelled; // record stays in memory
    return true;
}

bool DonationManager::updateDonationStatus(int donationId, DonationStatus newStatus,
                                           std::string& errorMessage) {
    errorMessage.clear();
    Donation* existing = findDonation(donationId);
    if (existing == nullptr) {
        errorMessage = "Donation not found.";
        return false;
    }
    if (isTerminalStatus(existing->status)) {
        errorMessage = "Donation is Delivered, Cancelled or Rejected and its status cannot change.";
        return false;
    }
    existing->status = newStatus;
    return true;
}

bool DonationManager::allocateQuantity(int donationId, int allocatedQuantity,
                                       std::string& errorMessage) {
    errorMessage.clear();
    if (allocatedQuantity <= 0) {
        errorMessage = "Allocated quantity must be greater than zero.";
        return false;
    }
    Donation* existing = findDonation(donationId);
    if (existing == nullptr) {
        errorMessage = "Donation not found.";
        return false;
    }
    if (isTerminalStatus(existing->status)) {
        errorMessage = "Donation is Delivered, Cancelled or Rejected and cannot be allocated from.";
        return false;
    }
    if (allocatedQuantity > existing->availableQuantity) {
        errorMessage = "Allocated quantity exceeds the available quantity.";
        return false;
    }
    existing->availableQuantity -= allocatedQuantity; // can reach 0, never below
    return true;
}

std::optional<Donation> DonationManager::getDonationById(int donationId) const {
    const Donation* existing = findDonation(donationId);
    if (existing == nullptr) {
        return std::nullopt;
    }
    return *existing;
}

std::vector<Donation> DonationManager::getAllDonations() const {
    return donations_;
}

std::vector<Donation> DonationManager::getAvailableDonations() const {
    const std::string today = todayIsoDate();
    std::vector<Donation> available;
    for (const Donation& donation : donations_) {
        if (donation.status == DonationStatus::Received && donation.availableQuantity > 0 &&
            !isExpired(donation, today)) {
            available.push_back(donation);
        }
    }
    return available;
}

void DonationManager::replaceAllDonations(const std::vector<Donation>& donations) {
    donations_ = donations;
    int maxId = 0;
    for (const Donation& donation : donations_) {
        maxId = std::max(maxId, donation.id);
    }
    nextId_ = maxId + 1; // new donations never reuse a loaded id
}

} // namespace sofralink