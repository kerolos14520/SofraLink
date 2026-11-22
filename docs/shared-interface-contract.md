# Shared Interface Contract — v1.0

This contract is based on the six task instruction PDFs distributed to the team. It is the source of truth for public C++ model names, fields, enum values and function signatures. Do not independently rename anything here.

## Conventions
- C++17; namespace `sofralink`.
- Date: `YYYY-MM-DD`; time: ISO 8601.
- `urgencyLevel`: integer 1–5, where 5 is highest.
- JSON keys: lowerCamelCase.
- Enum values in JSON: UPPER_SNAKE_CASE strings, not numeric values.

## Enums
- DonationStatus: Submitted, UnderReview, Accepted, Received, Aggregated, Packed, HandedOver, Delivered, Rejected, Cancelled.
- RequestStatus: Pending, Approved, PartiallyFulfilled, Fulfilled, Rejected, Cancelled.
- MatchStatus: Proposed, Approved, Completed, Rejected, Cancelled.

## Models
### Donation
`id`, `donorName`, `category`, `totalQuantity`, `availableQuantity`, `unit`, `expiryDate`, `location`, `productBatchNumber`, `isSealed`, `isNonPerishable`, `status`, `createdAt`.

### FoodRequest
`id`, `organizationName`, `category`, `requestedQuantity`, `remainingQuantity`, `unit`, `urgencyLevel`, `location`, `neededByDate`, `status`, `createdAt`.

### MatchResult
`matchId`, `donationId`, `requestId`, `allocatedQuantity`, `explanation`, `status`, `createdAt`.

### ValidationResult
`isValid`, `errors`, `warnings`.

### ReportSummary
`totalDonations`, `acceptedDonations`, `rejectedDonations`, `receivedDonations`, `totalRequests`, `pendingRequests`, `fulfilledRequests`, `totalMatches`, `proposedMatches`, `completedMatches`, `totalAvailableQuantity`, `totalAllocatedQuantity`.

## Fixed class methods

### DonationManager — Member 2
- `createDonation(const Donation&, Donation& createdDonation, std::string& errorMessage)`
- `updateDonation(int donationId, const Donation& updatedDonation, std::string& errorMessage)`
- `cancelDonation(int donationId, std::string& errorMessage)`
- `updateDonationStatus(int donationId, DonationStatus newStatus, std::string& errorMessage)`
- `allocateQuantity(int donationId, int allocatedQuantity, std::string& errorMessage)`
- `getDonationById(int donationId) const`
- `getAllDonations() const`
- `getAvailableDonations() const`
- `replaceAllDonations(const std::vector<Donation>& donations)`

### RequestManager — Member 3
- `createRequest(const FoodRequest&, FoodRequest& createdRequest, std::string& errorMessage)`
- `updateRequest(int requestId, const FoodRequest& updatedRequest, std::string& errorMessage)`
- `cancelRequest(int requestId, std::string& errorMessage)`
- `updateRequestStatus(int requestId, RequestStatus newStatus, std::string& errorMessage)`
- `allocateQuantity(int requestId, int allocatedQuantity, std::string& errorMessage)`
- `getRequestById(int requestId) const`
- `getAllRequests() const`
- `getPendingRequests() const`
- `replaceAllRequests(const std::vector<FoodRequest>& requests)`

### Validation — Member 4
- `validateDonation(const Donation&) const`
- `validateFoodRequest(const FoodRequest&) const`

### SearchService — Member 4
- `searchDonations(const std::vector<Donation>&, const DonationSearchCriteria&) const`
- `searchRequests(const std::vector<FoodRequest>&, const RequestSearchCriteria&) const`
- `sortDonationsByExpiry(const std::vector<Donation>&) const`

### MatchingEngine — Member 1
- `generateMatches(const std::vector<Donation>& donations, const std::vector<FoodRequest>& requests) const`

### JsonStorage — Member 6
- `saveDonations` / `loadDonations`
- `saveRequests` / `loadRequests`
- `saveMatches` / `loadMatches`
- Constructor: `JsonStorage(std::filesystem::path dataDirectory = "data/runtime")`

### ReportService — Member 6
- `generateSummary(const std::vector<Donation>&, const std::vector<FoodRequest>&, const std::vector<MatchResult>&) const`

## Rules
- Member 1 creates the shared models once before other members start.
- Member 2 and Member 3 call Member 4's Validation service.
- Managers do not write JSON; the API/application layer coordinates persistence through JsonStorage.
- MatchingEngine returns proposals and does not mutate input vectors.
- Never allocate more than donation.availableQuantity or request.remainingQuantity.
- A missing interface or conflict must be reported to Member 1; do not silently invent a new signature.
