#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include "sofralink/models/Donation.hpp"
#include "sofralink/models/FoodRequest.hpp"
#include "sofralink/models/MatchResult.hpp"
namespace sofralink {
class JsonStorage {
public:
    explicit JsonStorage(std::filesystem::path dataDirectory = "data/runtime");
    bool saveDonations(const std::vector<Donation>&, std::string& errorMessage) const;
    bool loadDonations(std::vector<Donation>&, std::string& errorMessage) const;
    bool saveRequests(const std::vector<FoodRequest>&, std::string& errorMessage) const;
    bool loadRequests(std::vector<FoodRequest>&, std::string& errorMessage) const;
    bool saveMatches(const std::vector<MatchResult>&, std::string& errorMessage) const;
    bool loadMatches(std::vector<MatchResult>&, std::string& errorMessage) const;
};
} // namespace sofralink
