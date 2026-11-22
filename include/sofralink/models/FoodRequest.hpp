#pragma once
#include <string>
#include "sofralink/models/Enums.hpp"
namespace sofralink {
struct FoodRequest {
    int id{0};
    std::string organizationName;
    std::string category;
    int requestedQuantity{0};
    int remainingQuantity{0};
    std::string unit{"kg"};
    int urgencyLevel{1};
    std::string location;
    std::string neededByDate;
    RequestStatus status{RequestStatus::Pending};
    std::string createdAt;
};
} // namespace sofralink
