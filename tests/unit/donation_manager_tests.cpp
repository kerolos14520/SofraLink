// Plain C++17 unit tests for DonationManager (no external framework).
// Exit code 0 = all passed, 1 = at least one check failed.
#include <iostream>
#include <string>
#include <vector>

#include "sofralink/models/Donation.hpp"
#include "sofralink/services/DonationManager.hpp"
#include "sofralink/services/Validation.hpp"

using namespace sofralink;

namespace {

int g_failures = 0;
int g_checks = 0;

#define CHECK(cond)                                                                  \
    do {                                                                             \
        ++g_checks;                                                                  \
        if (!(cond)) {                                                               \
            ++g_failures;                                                            \
            std::cerr << "  FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; \
        }                                                                            \
    } while (0)

constexpr const char* kFarFuture = "2999-12-31";
constexpr const char* kLongPast = "2000-01-01";

Donation makeValidDonation() {
    Donation d;
    d.donorName = "Cairo Bakery";
    d.category = "Bakery";
    d.totalQuantity = 10;
    d.availableQuantity = 10;
    d.unit = "kg";
    d.expiryDate = kFarFuture;
    d.location = "Giza";
    d.productBatchNumber = "B-001";
    d.isSealed = true;
    d.isNonPerishable = true ;
    d.createdAt = "2026-11-27";
    return d;
}

// Creates a donation through the manager and returns the stored copy (with its new id).
Donation addDonation(DonationManager& manager, const Donation& input) {
    Donation created;
    std::string error;
    const bool ok = manager.createDonation(input, created, error);
    CHECK(ok);
    return created;
}

void testBaselinePassesSharedValidation() {
    // Precondition for every other test: the sample donation must satisfy the real Validation.
    Validation validation;
    const ValidationResult result = validation.validateDonation(makeValidDonation());
    CHECK(result.isValid);
    if (!result.isValid) {
        for (const auto& e : result.errors) {
            std::cerr << "    validation error: " << e << "\n";
        }
        std::cerr << "  Adjust makeValidDonation() to match Validation::validateDonation rules.\n";
    }
}

void testCreateAssignsIncrementingIds() {
    Validation validation;
    DonationManager manager(validation);
    const Donation a = addDonation(manager, makeValidDonation());
    const Donation b = addDonation(manager, makeValidDonation());
    CHECK(a.id == 1);
    CHECK(b.id == 2);
    CHECK(manager.getAllDonations().size() == 2);
    const auto found = manager.getDonationById(a.id);
    CHECK(found.has_value());
    CHECK(found && found->donorName == "Cairo Bakery");
}

void testCreateRejectsBadQuantities() {
    Validation validation;
    DonationManager manager(validation);
    Donation created;
    std::string error;

    Donation negativeTotal = makeValidDonation();
    negativeTotal.totalQuantity = -5;
    negativeTotal.availableQuantity = 0;
    CHECK(!manager.createDonation(negativeTotal, created, error));
    CHECK(!error.empty());

    Donation negativeAvailable = makeValidDonation();
    negativeAvailable.availableQuantity = -1;
    CHECK(!manager.createDonation(negativeAvailable, created, error));
    CHECK(!error.empty());

    Donation availableTooBig = makeValidDonation();
    availableTooBig.availableQuantity = availableTooBig.totalQuantity + 1;
    CHECK(!manager.createDonation(availableTooBig, created, error));
    CHECK(!error.empty());

    CHECK(manager.getAllDonations().empty());
}

void testCreateRejectsWhenValidationFails() {
    Validation validation;
    DonationManager manager(validation);
    Donation created;
    std::string error;

    Donation bad;           // empty donor, category, unit-less expiry, zero quantity
    bad.expiryDate.clear();
    CHECK(!manager.createDonation(bad, created, error));
    CHECK(!error.empty());
    CHECK(manager.getAllDonations().empty());
}

void testUpdateChangesFieldsButKeepsIdentity() {
    Validation validation;
    DonationManager manager(validation);
    const Donation created = addDonation(manager, makeValidDonation());
    std::string error;
    CHECK(manager.updateDonationStatus(created.id, DonationStatus::Received, error));

    Donation edit = makeValidDonation();
    edit.donorName = "Nile Market";
    edit.totalQuantity = 20;
    edit.availableQuantity = 15;
    edit.id = 999;                          // must be ignored
    edit.status = DonationStatus::Packed;   // must be ignored
    edit.createdAt = "1999-01-01";          // must be ignored
    CHECK(manager.updateDonation(created.id, edit, error));

    const auto stored = manager.getDonationById(created.id);
    CHECK(stored.has_value());
    if (stored) {
        CHECK(stored->donorName == "Nile Market");
        CHECK(stored->totalQuantity == 20);
        CHECK(stored->availableQuantity == 15);
        CHECK(stored->id == created.id);
        CHECK(stored->status == DonationStatus::Received);
        CHECK(stored->createdAt == "2026-11-27");
    }
}

void testUpdateUnknownIdFails() {
    Validation validation;
    DonationManager manager(validation);
    std::string error;
    CHECK(!manager.updateDonation(42, makeValidDonation(), error));
    CHECK(!error.empty());
}

void testUpdateBlockedForTerminalStatuses() {
    Validation validation;
    DonationManager manager(validation);

    Donation delivered = makeValidDonation();
    delivered.id = 1;
    delivered.status = DonationStatus::Delivered;
    Donation cancelled = makeValidDonation();
    cancelled.id = 2;
    cancelled.status = DonationStatus::Cancelled;
    Donation rejected = makeValidDonation();
    rejected.id = 3;
    rejected.status = DonationStatus::Rejected;
    manager.replaceAllDonations({delivered, cancelled, rejected});

    for (int id = 1; id <= 3; ++id) {
        Donation edit = makeValidDonation();
        edit.donorName = "Changed";
        std::string error;
        CHECK(!manager.updateDonation(id, edit, error));
        CHECK(!error.empty());
        const auto stored = manager.getDonationById(id);
        CHECK(stored && stored->donorName == "Cairo Bakery");
    }
}

void testUpdateRejectsBadQuantitiesAndLeavesRecordUnchanged() {
    Validation validation;
    DonationManager manager(validation);
    const Donation created = addDonation(manager, makeValidDonation());
    std::string error;

    Donation edit = makeValidDonation();
    edit.donorName = "Should Not Stick";
    edit.availableQuantity = edit.totalQuantity + 5;
    CHECK(!manager.updateDonation(created.id, edit, error));
    CHECK(!error.empty());

    edit.availableQuantity = -2;
    CHECK(!manager.updateDonation(created.id, edit, error));

    const auto stored = manager.getDonationById(created.id);
    CHECK(stored && stored->donorName == "Cairo Bakery");
    CHECK(stored && stored->availableQuantity == 10);
}

void testUpdateRejectsWhenValidationFails() {
    Validation validation;
    DonationManager manager(validation);
    const Donation created = addDonation(manager, makeValidDonation());
    std::string error;

    Donation edit = makeValidDonation();
    edit.donorName.clear();
    edit.category.clear();
    edit.totalQuantity = 0;
    edit.availableQuantity = 0;
    CHECK(!manager.updateDonation(created.id, edit, error));
    CHECK(!error.empty());
    const auto stored = manager.getDonationById(created.id);
    CHECK(stored && stored->donorName == "Cairo Bakery");
}

void testCancelKeepsRecordAsCancelled() {
    Validation validation;
    DonationManager manager(validation);
    const Donation created = addDonation(manager, makeValidDonation());
    std::string error;

    CHECK(manager.cancelDonation(created.id, error));
    const auto stored = manager.getDonationById(created.id);
    CHECK(stored.has_value());
    CHECK(stored && stored->status == DonationStatus::Cancelled);
    CHECK(manager.getAllDonations().size() == 1);

    CHECK(!manager.cancelDonation(created.id, error)); // already cancelled
    CHECK(!manager.cancelDonation(12345, error));      // unknown id
}

void testCancelBlockedForDeliveredAndRejected() {
    Validation validation;
    DonationManager manager(validation);
    Donation delivered = makeValidDonation();
    delivered.id = 1;
    delivered.status = DonationStatus::Delivered;
    Donation rejected = makeValidDonation();
    rejected.id = 2;
    rejected.status = DonationStatus::Rejected;
    manager.replaceAllDonations({delivered, rejected});

    std::string error;
    CHECK(!manager.cancelDonation(1, error));
    CHECK(!manager.cancelDonation(2, error));
    CHECK(manager.getDonationById(1)->status == DonationStatus::Delivered);
    CHECK(manager.getDonationById(2)->status == DonationStatus::Rejected);
}

void testUpdateDonationStatus() {
    Validation validation;
    DonationManager manager(validation);
    const Donation created = addDonation(manager, makeValidDonation());
    std::string error;

    CHECK(manager.updateDonationStatus(created.id, DonationStatus::Received, error));
    CHECK(manager.getDonationById(created.id)->status == DonationStatus::Received);
    CHECK(manager.updateDonationStatus(created.id, DonationStatus::Delivered, error));
    CHECK(!manager.updateDonationStatus(created.id, DonationStatus::Received, error)); // final
    CHECK(manager.getDonationById(created.id)->status == DonationStatus::Delivered);
    CHECK(!manager.updateDonationStatus(777, DonationStatus::Received, error));
}

void testAllocateQuantity() {
    Validation validation;
    DonationManager manager(validation);
    const Donation created = addDonation(manager, makeValidDonation()); // available 10
    std::string error;

    CHECK(manager.allocateQuantity(created.id, 4, error));
    CHECK(manager.getDonationById(created.id)->availableQuantity == 6);
    CHECK(manager.getDonationById(created.id)->totalQuantity == 10);

    CHECK(!manager.allocateQuantity(created.id, 7, error)); // more than available
    CHECK(manager.getDonationById(created.id)->availableQuantity == 6);

    CHECK(!manager.allocateQuantity(created.id, 0, error));
    CHECK(!manager.allocateQuantity(created.id, -3, error));
    CHECK(manager.getDonationById(created.id)->availableQuantity == 6);

    CHECK(manager.allocateQuantity(created.id, 6, error)); // exactly the rest
    CHECK(manager.getDonationById(created.id)->availableQuantity == 0);
    CHECK(!manager.allocateQuantity(created.id, 1, error)); // never below zero
    CHECK(manager.getDonationById(created.id)->availableQuantity == 0);

    CHECK(!manager.allocateQuantity(555, 1, error)); // unknown id
}

void testAllocateBlockedForTerminalStatus() {
    Validation validation;
    DonationManager manager(validation);
    const Donation created = addDonation(manager, makeValidDonation());
    std::string error;
    CHECK(manager.cancelDonation(created.id, error));
    CHECK(!manager.allocateQuantity(created.id, 1, error));
    CHECK(manager.getDonationById(created.id)->availableQuantity == 10);
}

void testGetAvailableDonationsFilter() {
    Validation validation;
    DonationManager manager(validation);

    Donation good = makeValidDonation();               // Received, future, qty > 0
    good.id = 1;
    good.status = DonationStatus::Received;

    Donation notReceived = makeValidDonation();        // wrong status
    notReceived.id = 2;
    notReceived.status = DonationStatus::Submitted;

    Donation expired = makeValidDonation();            // expired
    expired.id = 3;
    expired.status = DonationStatus::Received;
    expired.expiryDate = kLongPast;

    Donation empty = makeValidDonation();              // nothing left
    empty.id = 4;
    empty.status = DonationStatus::Received;
    empty.availableQuantity = 0;

    Donation cancelled = makeValidDonation();          // cancelled
    cancelled.id = 5;
    cancelled.status = DonationStatus::Cancelled;

    Donation nonPerishable = makeValidDonation();      // no expiry but non-perishable
    nonPerishable.id = 6;
    nonPerishable.status = DonationStatus::Received;
    nonPerishable.expiryDate.clear();
    nonPerishable.isNonPerishable = true;

    Donation badDate = makeValidDonation();            // unreadable date, perishable
    badDate.id = 7;
    badDate.status = DonationStatus::Received;
    badDate.expiryDate = "tomorrow";

    manager.replaceAllDonations({good, notReceived, expired, empty, cancelled, nonPerishable, badDate});

    const std::vector<Donation> available = manager.getAvailableDonations();
    CHECK(available.size() == 2);
    bool hasGood = false;
    bool hasNonPerishable = false;
    for (const Donation& d : available) {
        hasGood = hasGood || d.id == 1;
        hasNonPerishable = hasNonPerishable || d.id == 6;
    }
    CHECK(hasGood);
    CHECK(hasNonPerishable);
    CHECK(manager.getAllDonations().size() == 7); // filtering never removes records
}

void testGetByIdMissingAndGetAll() {
    Validation validation;
    DonationManager manager(validation);
    CHECK(!manager.getDonationById(1).has_value());
    CHECK(manager.getAllDonations().empty());
}

void testReplaceAllDonationsContinuesIds() {
    Validation validation;
    DonationManager manager(validation);
    addDonation(manager, makeValidDonation());

    Donation loaded = makeValidDonation();
    loaded.id = 41;
    manager.replaceAllDonations({loaded});
    CHECK(manager.getAllDonations().size() == 1);
    CHECK(manager.getDonationById(41).has_value());
    CHECK(!manager.getDonationById(1).has_value());

    const Donation next = addDonation(manager, makeValidDonation());
    CHECK(next.id == 42);

    manager.replaceAllDonations({});
    CHECK(manager.getAllDonations().empty());
    CHECK(addDonation(manager, makeValidDonation()).id == 1);
}

struct TestCase {
    const char* name;
    void (*run)();
};

} // namespace

int main() {
    const std::vector<TestCase> tests = {
        {"baseline passes shared Validation", testBaselinePassesSharedValidation},
        {"create assigns incrementing ids", testCreateAssignsIncrementingIds},
        {"create rejects bad quantities", testCreateRejectsBadQuantities},
        {"create rejects when Validation fails", testCreateRejectsWhenValidationFails},
        {"update keeps id/status/createdAt", testUpdateChangesFieldsButKeepsIdentity},
        {"update unknown id fails", testUpdateUnknownIdFails},
        {"update blocked for terminal statuses", testUpdateBlockedForTerminalStatuses},
        {"update rejects bad quantities", testUpdateRejectsBadQuantitiesAndLeavesRecordUnchanged},
        {"update rejects when Validation fails", testUpdateRejectsWhenValidationFails},
        {"cancel keeps record as Cancelled", testCancelKeepsRecordAsCancelled},
        {"cancel blocked for Delivered/Rejected", testCancelBlockedForDeliveredAndRejected},
        {"updateDonationStatus rules", testUpdateDonationStatus},
        {"allocateQuantity rules", testAllocateQuantity},
        {"allocate blocked for terminal status", testAllocateBlockedForTerminalStatus},
        {"getAvailableDonations filter", testGetAvailableDonationsFilter},
        {"getById missing / getAll empty", testGetByIdMissingAndGetAll},
        {"replaceAllDonations continues ids", testReplaceAllDonationsContinuesIds},
    };

    for (const TestCase& t : tests) {
        const int before = g_failures;
        t.run();
        std::cout << (g_failures == before ? "[PASS] " : "[FAIL] ") << t.name << "\n";
    }
    std::cout << g_checks << " checks, " << g_failures << " failed\n";
    return g_failures == 0 ? 0 : 1;
}