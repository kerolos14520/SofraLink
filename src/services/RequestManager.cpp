#include "sofralink/services/RequestManager.hpp"

#include <algorithm>
#include <ctime>

namespace sofralink {
namespace {

// Terminal states: a request in one of these can no longer be edited,
// re-statused, or allocated against.
bool isTerminalStatus(RequestStatus status) {
    return status == RequestStatus::Cancelled ||
           status == RequestStatus::Rejected ||
           status == RequestStatus::Fulfilled;
}

// Active states returned by getPendingRequests().
bool isActiveStatus(RequestStatus status) {
    return status == RequestStatus::Pending ||
           status == RequestStatus::Approved ||
           status == RequestStatus::PartiallyFulfilled;
}

// Human-readable name used only in error messages (not for JSON).
const char* statusName(RequestStatus status) {
    switch (status) {
        case RequestStatus::Pending:            return "Pending";
        case RequestStatus::Approved:           return "Approved";
        case RequestStatus::PartiallyFulfilled: return "PartiallyFulfilled";
        case RequestStatus::Fulfilled:          return "Fulfilled";
        case RequestStatus::Rejected:           return "Rejected";
        case RequestStatus::Cancelled:          return "Cancelled";
    }
    return "Unknown";
}

// Allowed manual status transitions for updateRequestStatus().
// Fulfilled and PartiallyFulfilled are NOT reachable here: they are set only
// by allocateQuantity() so remainingQuantity and status never disagree.
bool isAllowedTransition(RequestStatus from, RequestStatus to) {
    switch (from) {
        case RequestStatus::Pending:
            return to == RequestStatus::Approved ||
                   to == RequestStatus::Rejected ||
                   to == RequestStatus::Cancelled;
        case RequestStatus::Approved:
        case RequestStatus::PartiallyFulfilled:
            return to == RequestStatus::Cancelled;
        case RequestStatus::Fulfilled:
        case RequestStatus::Rejected:
        case RequestStatus::Cancelled:
            return false;
    }
    return false;
}

std::string joinErrors(const ValidationResult& result) {
    if (result.errors.empty()) {
        return "Invalid food request.";
    }
    std::string joined;
    for (const std::string& error : result.errors) {
        if (!joined.empty()) {
            joined += "; ";
        }
        joined += error;
    }
    return joined;
}

// ISO 8601 UTC timestamp, e.g. 2026-11-22T14:05:09Z.
std::string currentTimestampIso8601() {
    const std::time_t now = std::time(nullptr);
    std::tm utc{};
#if defined(_WIN32)
    gmtime_s(&utc, &now);
#else
    gmtime_r(&now, &utc);
#endif
    char buffer[32];
    if (std::strftime(buffer, sizeof buffer, "%Y-%m-%dT%H:%M:%SZ", &utc) == 0) {
        return std::string{};
    }
    return std::string{buffer};
}

} // namespace

RequestManager::RequestManager(const Validation& validation)
    : validation_(validation) {}

std::vector<FoodRequest>::iterator RequestManager::findById(int requestId) {
    return std::find_if(requests_.begin(), requests_.end(),
                        [requestId](const FoodRequest& r) { return r.id == requestId; });
}

std::vector<FoodRequest>::const_iterator RequestManager::findById(int requestId) const {
    return std::find_if(requests_.begin(), requests_.end(),
                        [requestId](const FoodRequest& r) { return r.id == requestId; });
}

bool RequestManager::createRequest(const FoodRequest& request,
                                   FoodRequest& createdRequest,
                                   std::string& errorMessage) {
    errorMessage.clear();

    if (request.id < 0) {
        errorMessage = "Request id must not be negative.";
        return false;
    }
    if (request.requestedQuantity <= 0) {
        errorMessage = "requestedQuantity must be a positive integer.";
        return false;
    }

    // Normalize server-controlled fields BEFORE validating, so callers cannot
    // inject their own remainingQuantity/status at creation time.
    FoodRequest candidate = request;
    candidate.remainingQuantity = candidate.requestedQuantity;
    candidate.status = RequestStatus::Pending;
    if (candidate.createdAt.empty()) {
        candidate.createdAt = currentTimestampIso8601();
    }

    const ValidationResult validation = validation_.validateFoodRequest(candidate);
    if (!validation.isValid) {
        errorMessage = joinErrors(validation);
        return false;
    }

    if (candidate.id == 0) {
        while (findById(nextId_) != requests_.end()) {
            ++nextId_;
        }
        candidate.id = nextId_;
    } else if (findById(candidate.id) != requests_.end()) {
        errorMessage = "A request with id " + std::to_string(candidate.id) + " already exists.";
        return false;
    }
    nextId_ = std::max(nextId_, candidate.id + 1);

    requests_.push_back(candidate);
    createdRequest = candidate;
    return true;
}

bool RequestManager::updateRequest(int requestId,
                                   const FoodRequest& updatedRequest,
                                   std::string& errorMessage) {
    errorMessage.clear();

    auto it = findById(requestId);
    if (it == requests_.end()) {
        errorMessage = "Request not found: " + std::to_string(requestId) + ".";
        return false;
    }
    if (isTerminalStatus(it->status)) {
        errorMessage = std::string("Cannot update a request in status ") +
                       statusName(it->status) + ".";
        return false;
    }

    // Fields that are never changed through updateRequest().
    FoodRequest candidate = updatedRequest;
    candidate.id = it->id;
    candidate.createdAt = it->createdAt;
    candidate.requestedQuantity = it->requestedQuantity;
    candidate.remainingQuantity = it->remainingQuantity;
    candidate.status = it->status;

    // Changing the unit after part of the request was allocated would make the
    // already-allocated quantity meaningless (units must never be mixed).
    if (candidate.unit != it->unit && it->remainingQuantity != it->requestedQuantity) {
        errorMessage = "Cannot change unit after quantity has been allocated.";
        return false;
    }

    const ValidationResult validation = validation_.validateFoodRequest(candidate);
    if (!validation.isValid) {
        errorMessage = joinErrors(validation);
        return false;
    }

    *it = candidate;
    return true;
}

bool RequestManager::cancelRequest(int requestId, std::string& errorMessage) {
    errorMessage.clear();

    auto it = findById(requestId);
    if (it == requests_.end()) {
        errorMessage = "Request not found: " + std::to_string(requestId) + ".";
        return false;
    }
    if (isTerminalStatus(it->status)) {
        errorMessage = std::string("Cannot cancel a request in status ") +
                       statusName(it->status) + ".";
        return false;
    }

    it->status = RequestStatus::Cancelled; // record is kept, never deleted
    return true;
}

bool RequestManager::updateRequestStatus(int requestId,
                                         RequestStatus newStatus,
                                         std::string& errorMessage) {
    errorMessage.clear();

    auto it = findById(requestId);
    if (it == requests_.end()) {
        errorMessage = "Request not found: " + std::to_string(requestId) + ".";
        return false;
    }
    if (newStatus == RequestStatus::Fulfilled ||
        newStatus == RequestStatus::PartiallyFulfilled) {
        errorMessage = "Fulfilled and PartiallyFulfilled are set only by allocateQuantity().";
        return false;
    }
    if (!isAllowedTransition(it->status, newStatus)) {
        errorMessage = std::string("Status change not allowed: ") +
                       statusName(it->status) + " -> " + statusName(newStatus) + ".";
        return false;
    }

    it->status = newStatus;
    return true;
}

bool RequestManager::allocateQuantity(int requestId,
                                      int allocatedQuantity,
                                      std::string& errorMessage) {
    errorMessage.clear();

    auto it = findById(requestId);
    if (it == requests_.end()) {
        errorMessage = "Request not found: " + std::to_string(requestId) + ".";
        return false;
    }
    if (isTerminalStatus(it->status)) {
        errorMessage = std::string("Cannot allocate to a request in status ") +
                       statusName(it->status) + ".";
        return false;
    }
    if (allocatedQuantity <= 0) {
        errorMessage = "Allocated quantity must be a positive integer.";
        return false;
    }
    if (allocatedQuantity > it->remainingQuantity) {
        errorMessage = "Allocated quantity (" + std::to_string(allocatedQuantity) +
                       ") exceeds remaining quantity (" +
                       std::to_string(it->remainingQuantity) + ").";
        return false;
    }

    it->remainingQuantity -= allocatedQuantity;
    it->status = (it->remainingQuantity == 0) ? RequestStatus::Fulfilled
                                              : RequestStatus::PartiallyFulfilled;
    return true;
}

std::optional<FoodRequest> RequestManager::getRequestById(int requestId) const {
    const auto it = findById(requestId);
    if (it == requests_.end()) {
        return std::nullopt;
    }
    return *it;
}

std::vector<FoodRequest> RequestManager::getAllRequests() const {
    return requests_;
}

std::vector<FoodRequest> RequestManager::getPendingRequests() const {
    std::vector<FoodRequest> active;
    for (const FoodRequest& request : requests_) {
        if (isActiveStatus(request.status)) {
            active.push_back(request);
        }
    }
    return active;
}

// Intended for the storage load path. Records are taken as already-persisted
// data and are NOT re-validated. Records with id <= 0 or an id already seen in
// this call are skipped (first occurrence wins), so no record is duplicated.
// The method returns void, so skipped records are dropped silently.
void RequestManager::replaceAllRequests(const std::vector<FoodRequest>& requests) {
    requests_.clear();
    nextId_ = 1;

    for (const FoodRequest& request : requests) {
        if (request.id <= 0 || findById(request.id) != requests_.end()) {
            continue;
        }
        requests_.push_back(request);
        nextId_ = std::max(nextId_, request.id + 1);
    }
}

} // namespace sofralink
