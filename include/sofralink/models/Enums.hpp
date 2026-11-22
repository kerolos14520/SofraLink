#pragma once
namespace sofralink {
enum class DonationStatus { Submitted, UnderReview, Accepted, Received, Aggregated, Packed, HandedOver, Delivered, Rejected, Cancelled };
enum class RequestStatus { Pending, Approved, PartiallyFulfilled, Fulfilled, Rejected, Cancelled };
enum class MatchStatus { Proposed, Approved, Completed, Rejected, Cancelled };
} // namespace sofralink
