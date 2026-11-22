#pragma once
#include <optional>
#include <string>
#include <vector>
#include "sofralink/models/FoodRequest.hpp"
#include "sofralink/services/Validation.hpp"
namespace sofralink {
class RequestManager {
public:
    explicit RequestManager(const Validation& validation);
    bool createRequest(const FoodRequest&, FoodRequest& createdRequest, std::string& errorMessage);
    bool updateRequest(int requestId, const FoodRequest& updatedRequest, std::string& errorMessage);
    bool cancelRequest(int requestId, std::string& errorMessage);
    bool updateRequestStatus(int requestId, RequestStatus newStatus, std::string& errorMessage);
    bool allocateQuantity(int requestId, int allocatedQuantity, std::string& errorMessage);
    std::optional<FoodRequest> getRequestById(int requestId) const;
    std::vector<FoodRequest> getAllRequests() const;
    std::vector<FoodRequest> getPendingRequests() const;
    void replaceAllRequests(const std::vector<FoodRequest>& requests);
};
} // namespace sofralink
