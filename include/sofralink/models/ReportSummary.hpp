#pragma once
namespace sofralink {
struct ReportSummary {
    int totalDonations{0};
    int acceptedDonations{0};
    int rejectedDonations{0};
    int receivedDonations{0};
    int totalRequests{0};
    int pendingRequests{0};
    int fulfilledRequests{0};
    int totalMatches{0};
    int proposedMatches{0};
    int completedMatches{0};
    long long totalAvailableQuantity{0};
    long long totalAllocatedQuantity{0};
};
} // namespace sofralink
