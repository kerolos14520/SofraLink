#pragma once
#include <string>
#include <vector>
namespace sofralink {
struct ValidationResult {
    bool isValid{true};
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};
} // namespace sofralink
