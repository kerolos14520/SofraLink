// Unit tests for sofralink::RequestManager (Member 3).
//
// Plain C++17, no test framework. Exit code 0 = all checks passed.
// These tests link against the real Validation implementation (Member 4).
//
// ---------------------------------------------------------------------------
// DATE FORMAT - TEMPORARY DEFAULT
// AGENTS.md says YYYY-MM-DD; the Member 3 PDF says DD-MM-YYYY. Pending
// Member 1's decision, these tests use YYYY-MM-DD.
// If Member 1 chooses DD-MM-YYYY, change ONLY the two constants below
// (and, if needed, the unusual-date example in testCreateRejectsEmptyFieldsAndBadDate).
// ---------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "sofralink/services/RequestManager.hpp"

namespace {

// DATE-FORMAT: adjust these two values if the contract changes.
const char* const kValidDate = "2026-12-31";      // DD-MM-YYYY form: "31-12-2026"
const char* const kInvalidDate = "31/12/2026";    // wrong separators in either format

int g_failures = 0;
int g_checks = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        ++g_checks;                                                             \
        if (!(condition)) {                                                     \
            ++g_failures;                                                       \
            std::cerr << "FAILED " << __FILE__ << ":" << __LINE__ << "  "       \
                      << #condition << std::endl;                               \
        }                                                                       \
    } while (false)

using sofralink::FoodRequest;
using sofralink::RequestManager;
using sofralink::RequestStatus;
using sofralink::Validation;

FoodRequest makeValidRequest() {
    FoodRequest r;
    r.id = 0;
    r.organizationName = "Sample Relief Org";
    r.category = "Rice";
    r.requestedQuantity = 100;
    r.remainingQuantity = 0; // deliberately not equal: manager must set it
    r.unit = "kg";
    r.urgencyLevel = 3;
    r.location = "Sample City";
    r.neededByDate = kValidDate;
    r.status = RequestStatus::Approved; // deliberately wrong: manager must set Pending
    r.createdAt = "";
    return r;
}

// Creates a valid request and returns it; fails the test run if creation fails.
FoodRequest createOne(RequestManager& manager, int requestedQuantity = 100) {
    FoodRequest in = makeValidRequest();
    in.requestedQuantity = requestedQuantity;
    FoodRequest out;
    std::string err;
    const bool ok = manager.createRequest(in, out, err);
    CHECK(ok);
    return out;
}

// ---- 1. Create ------------------------------------------------------------
void testCreateValidAndIdGeneration() {
    Validation validation;
    RequestManager manager(validation);

    FoodRequest created;
    std::string err;
    CHECK(manager.createRequest(makeValidRequest(), created, err));
    CHECK(err.empty());
    CHECK(created.id > 0);
    CHECK(created.requestedQuantity == 100);
    CHECK(created.remainingQuantity == created.requestedQuantity);
    CHECK(created.status == RequestStatus::Pending);
    CHECK(!created.createdAt.empty());

    FoodRequest second;
    CHECK(manager.createRequest(makeValidRequest(), second, err));
    CHECK(second.id != created.id);

    CHECK(manager.getAllRequests().size() == 2);
    const auto fetched = manager.getRequestById(created.id);
    CHECK(fetched.has_value());
    CHECK(fetched && fetched->organizationName == "Sample Relief Org");
}

void testCreateWithExplicitIdAndDuplicates() {
    Validation validation;
    RequestManager manager(validation);

    FoodRequest in = makeValidRequest();
    in.id = 50;
    FoodRequest created;
    std::string err;
    CHECK(manager.createRequest(in, created, err));
    CHECK(created.id == 50);

    // Same explicit id again must be rejected and must not add a record.
    FoodRequest dup;
    CHECK(!manager.createRequest(in, dup, err));
    CHECK(!err.empty());
    CHECK(manager.getAllRequests().size() == 1);

    // Generated ids must not collide with an explicit one.
    FoodRequest generated;
    CHECK(manager.createRequest(makeValidRequest(), generated, err));
    CHECK(generated.id != 50);

    // Negative id is rejected.
    FoodRequest negative = makeValidRequest();
    negative.id = -3;
    FoodRequest out;
    CHECK(!manager.createRequest(negative, out, err));
    CHECK(manager.getAllRequests().size() == 2);
}

void testCreateRejectsBadQuantityAndUrgency() {
    Validation validation;
    RequestManager manager(validation);
    FoodRequest out;
    std::string err;

    FoodRequest r = makeValidRequest();
    r.requestedQuantity = 0;
    CHECK(!manager.createRequest(r, out, err));
    CHECK(!err.empty());

    r = makeValidRequest();
    r.requestedQuantity = -5;
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.urgencyLevel = 0;
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.urgencyLevel = 6;
    CHECK(!manager.createRequest(r, out, err));

    // Boundary values are accepted.
    r = makeValidRequest();
    r.urgencyLevel = 1;
    CHECK(manager.createRequest(r, out, err));
    r = makeValidRequest();
    r.urgencyLevel = 5;
    CHECK(manager.createRequest(r, out, err));

    CHECK(manager.getAllRequests().size() == 2);
}

void testCreateRejectsEmptyFieldsAndBadDate() {
    Validation validation;
    RequestManager manager(validation);
    FoodRequest out;
    std::string err;

    FoodRequest r = makeValidRequest();
    r.organizationName.clear();
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.category.clear();
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.unit.clear();
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.location.clear();
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.neededByDate.clear();
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.neededByDate = kInvalidDate;
    CHECK(!manager.createRequest(r, out, err));

    r = makeValidRequest();
    r.neededByDate = "not-a-date";
    CHECK(!manager.createRequest(r, out, err));

    CHECK(manager.getAllRequests().empty());
}

// ---- 2. Update --------------------------------------------------------------
void testUpdateExistingAndUnknown() {
    Validation validation;
    RequestManager manager(validation);
    const FoodRequest created = createOne(manager);

    FoodRequest changes = makeValidRequest();
    changes.id = 9999;                 // must be ignored
    changes.organizationName = "Updated Org";
    changes.location = "Updated City";
    changes.urgencyLevel = 5;
    changes.requestedQuantity = 1;     // must be ignored
    changes.remainingQuantity = 1;     // must be ignored
    changes.status = RequestStatus::Rejected; // must be ignored
    changes.createdAt = "1999-01-01T00:00:00Z"; // must be ignored

    std::string err;
    CHECK(manager.updateRequest(created.id, changes, err));

    const auto after = manager.getRequestById(created.id);
    CHECK(after.has_value());
    if (after) {
        CHECK(after->id == created.id);
        CHECK(after->organizationName == "Updated Org");
        CHECK(after->location == "Updated City");
        CHECK(after->urgencyLevel == 5);
        CHECK(after->requestedQuantity == created.requestedQuantity);
        CHECK(after->remainingQuantity == created.remainingQuantity);
        CHECK(after->status == created.status);
        CHECK(after->createdAt == created.createdAt);
    }
    CHECK(manager.getAllRequests().size() == 1);

    // Unknown id.
    CHECK(!manager.updateRequest(424242, makeValidRequest(), err));
    CHECK(!err.empty());
}

void testUpdateRejectsInvalidAndTerminal() {
    Validation validation;
    RequestManager manager(validation);
    const FoodRequest created = createOne(manager);
    std::string err;

    // Invalid update leaves the stored record untouched.
    FoodRequest bad = makeValidRequest();
    bad.urgencyLevel = 9;
    CHECK(!manager.updateRequest(created.id, bad, err));
    CHECK(!err.empty());
    const auto unchanged = manager.getRequestById(created.id);
    CHECK(unchanged && unchanged->urgencyLevel == created.urgencyLevel);

    // Unit cannot change once some quantity has been allocated.
    CHECK(manager.allocateQuantity(created.id, 10, err));
    FoodRequest unitChange = makeValidRequest();
    unitChange.unit = "box";
    CHECK(!manager.updateRequest(created.id, unitChange, err));

    // Cancelled requests cannot be updated.
    CHECK(manager.cancelRequest(created.id, err));
    CHECK(!manager.updateRequest(created.id, makeValidRequest(), err));
}

// ---- 3. Cancel ----------------------------------------------------------------
void testCancelKeepsRecord() {
    Validation validation;
    RequestManager manager(validation);
    const FoodRequest created = createOne(manager);
    std::string err;

    CHECK(manager.cancelRequest(created.id, err));
    CHECK(manager.getAllRequests().size() == 1);
    const auto after = manager.getRequestById(created.id);
    CHECK(after.has_value());
    CHECK(after && after->status == RequestStatus::Cancelled);

    CHECK(!manager.cancelRequest(created.id, err)); // already cancelled
    CHECK(!err.empty());
    CHECK(!manager.cancelRequest(424242, err));      // unknown id
    CHECK(manager.getPendingRequests().empty());
}

// ---- 4. Allocation --------------------------------------------------------------
void testPartialAllocation() {
    Validation validation;
    RequestManager manager(validation);
    const FoodRequest created = createOne(manager, 100);
    std::string err;

    CHECK(manager.allocateQuantity(created.id, 30, err));
    auto after = manager.getRequestById(created.id);
    CHECK(after.has_value());
    if (after) {
        CHECK(after->remainingQuantity == 70);
        CHECK(after->requestedQuantity == 100); // never changes
        CHECK(after->status == RequestStatus::PartiallyFulfilled);
    }

    CHECK(manager.allocateQuantity(created.id, 20, err));
    after = manager.getRequestById(created.id);
    CHECK(after && after->remainingQuantity == 50);
    CHECK(after && after->status == RequestStatus::PartiallyFulfilled);
}

void testFullAllocationAndRejections() {
    Validation validation;
    RequestManager manager(validation);
    const FoodRequest created = createOne(manager, 100);
    std::string err;

    // Invalid amounts are rejected and change nothing.
    CHECK(!manager.allocateQuantity(created.id, 0, err));
    CHECK(!manager.allocateQuantity(created.id, -4, err));
    CHECK(!manager.allocateQuantity(created.id, 101, err)); // over-allocation
    CHECK(!err.empty());
    CHECK(!manager.allocateQuantity(424242, 5, err));       // unknown id
    auto after = manager.getRequestById(created.id);
    CHECK(after && after->remainingQuantity == 100);
    CHECK(after && after->status == RequestStatus::Pending);

    // Exactly the remaining amount fulfils the request.
    CHECK(manager.allocateQuantity(created.id, 100, err));
    after = manager.getRequestById(created.id);
    CHECK(after && after->remainingQuantity == 0);
    CHECK(after && after->status == RequestStatus::Fulfilled);
    CHECK(after && after->requestedQuantity == 100);

    // Anything after Fulfilled is rejected.
    CHECK(!manager.allocateQuantity(created.id, 1, err));
    CHECK(!err.empty());
    after = manager.getRequestById(created.id);
    CHECK(after && after->remainingQuantity == 0);
}

void testAllocationAfterPartialToFull() {
    Validation validation;
    RequestManager manager(validation);
    const FoodRequest created = createOne(manager, 10);
    std::string err;

    CHECK(manager.allocateQuantity(created.id, 4, err));
    CHECK(!manager.allocateQuantity(created.id, 7, err)); // 7 > remaining 6
    CHECK(manager.allocateQuantity(created.id, 6, err));
    const auto after = manager.getRequestById(created.id);
    CHECK(after && after->status == RequestStatus::Fulfilled);
}

// ---- 5. Status changes -------------------------------------------------------------
void testUpdateRequestStatus() {
    Validation validation;
    RequestManager manager(validation);
    const FoodRequest created = createOne(manager);
    std::string err;

    CHECK(!manager.updateRequestStatus(424242, RequestStatus::Approved, err));
    CHECK(manager.updateRequestStatus(created.id, RequestStatus::Approved, err));
    CHECK(manager.getRequestById(created.id)->status == RequestStatus::Approved);

    // Fulfilled / PartiallyFulfilled are reserved for allocateQuantity().
    CHECK(!manager.updateRequestStatus(created.id, RequestStatus::Fulfilled, err));
    CHECK(!manager.updateRequestStatus(created.id, RequestStatus::PartiallyFulfilled, err));
    CHECK(manager.getRequestById(created.id)->status == RequestStatus::Approved);

    // Approved requests can still receive allocations.
    CHECK(manager.allocateQuantity(created.id, 10, err));
    CHECK(manager.getRequestById(created.id)->status == RequestStatus::PartiallyFulfilled);

    // Reject path from Pending, then terminal.
    const FoodRequest other = createOne(manager);
    CHECK(manager.updateRequestStatus(other.id, RequestStatus::Rejected, err));
    CHECK(!manager.updateRequestStatus(other.id, RequestStatus::Approved, err));
    CHECK(!manager.allocateQuantity(other.id, 1, err));
    CHECK(!manager.cancelRequest(other.id, err));
}

// ---- 6. Pending / active listing ----------------------------------------------------------
void testGetPendingRequestsFiltering() {
    Validation validation;
    RequestManager manager(validation);

    std::vector<FoodRequest> loaded;
    const RequestStatus statuses[] = {
        RequestStatus::Pending,  RequestStatus::Approved,  RequestStatus::PartiallyFulfilled,
        RequestStatus::Fulfilled, RequestStatus::Rejected, RequestStatus::Cancelled};
    int id = 1;
    for (RequestStatus s : statuses) {
        FoodRequest r = makeValidRequest();
        r.id = id++;
        r.remainingQuantity = r.requestedQuantity;
        r.status = s;
        loaded.push_back(r);
    }
    manager.replaceAllRequests(loaded);

    const auto active = manager.getPendingRequests();
    CHECK(active.size() == 3);
    for (const FoodRequest& r : active) {
        CHECK(r.status == RequestStatus::Pending ||
              r.status == RequestStatus::Approved ||
              r.status == RequestStatus::PartiallyFulfilled);
    }
    CHECK(manager.getAllRequests().size() == 6);
}

// ---- 7. replaceAllRequests ----------------------------------------------------------------
void testReplaceAllRequests() {
    Validation validation;
    RequestManager manager(validation);
    createOne(manager);
    createOne(manager);

    FoodRequest a = makeValidRequest();
    a.id = 10;
    a.remainingQuantity = a.requestedQuantity;
    a.organizationName = "First";
    FoodRequest dupOfA = a;
    dupOfA.organizationName = "Duplicate id, must be skipped";
    FoodRequest b = a;
    b.id = 20;
    b.organizationName = "Second";
    FoodRequest noId = a;
    noId.id = 0;

    manager.replaceAllRequests({a, dupOfA, b, noId});

    const auto all = manager.getAllRequests();
    CHECK(all.size() == 2);                       // old records gone, duplicates skipped
    CHECK(manager.getRequestById(10).has_value());
    CHECK(manager.getRequestById(10)->organizationName == "First"); // first occurrence wins
    CHECK(manager.getRequestById(20).has_value());
    CHECK(!manager.getRequestById(1).has_value());

    // Id generation continues after the highest loaded id.
    FoodRequest created;
    std::string err;
    CHECK(manager.createRequest(makeValidRequest(), created, err));
    CHECK(created.id == 21);
    CHECK(manager.getAllRequests().size() == 3);

    // Replacing with an empty list clears everything and resets ids.
    manager.replaceAllRequests({});
    CHECK(manager.getAllRequests().empty());
    CHECK(manager.createRequest(makeValidRequest(), created, err));
    CHECK(created.id == 1);
}

} // namespace

int main() {
    testCreateValidAndIdGeneration();
    testCreateWithExplicitIdAndDuplicates();
    testCreateRejectsBadQuantityAndUrgency();
    testCreateRejectsEmptyFieldsAndBadDate();
    testUpdateExistingAndUnknown();
    testUpdateRejectsInvalidAndTerminal();
    testCancelKeepsRecord();
    testPartialAllocation();
    testFullAllocationAndRejections();
    testAllocationAfterPartialToFull();
    testUpdateRequestStatus();
    testGetPendingRequestsFiltering();
    testReplaceAllRequests();

    std::cout << g_checks << " checks, " << g_failures << " failed" << std::endl;
    return g_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
