#pragma once
#include <string>
#include "sofralink/models/Enums.hpp"
namespace sofralink {
struct Donation {
    int id{0};
    std::string donorName;
    std::string category;
    int totalQuantity{0};
    int availableQuantity{0};
    std::string unit{"kg"};
    std::string expiryDate;
    std::string location;
    std::string productBatchNumber;
    bool isSealed{false};
    bool isNonPerishable{false};
    DonationStatus status{DonationStatus::Submitted};
    std::string createdAt;
};
} // namespace sofralink
