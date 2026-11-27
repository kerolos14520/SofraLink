#pragma once
// Internal helpers shared by Validation.cpp and SearchService.cpp.
// Not part of the public contract: do not include from other modules.
#include <cctype>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace sofralink::detail {

// True when the string is empty or contains only whitespace.
inline bool isBlank(const std::string& value) {
    for (const char ch : value) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return true;
}

// Strict "YYYY-MM-DD" calendar check (month range, days per month, leap years).
inline bool isValidIsoDate(const std::string& value) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') {
        return false;
    }
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (i != 4 && i != 7 && (value[i] < '0' || value[i] > '9')) {
            return false;
        }
    }
    const int year = std::stoi(value.substr(0, 4));
    const int month = std::stoi(value.substr(5, 2));
    const int day = std::stoi(value.substr(8, 2));
    if (year < 1 || month < 1 || month > 12) {
        return false;
    }
    const bool leapYear = year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
    const int daysPerMonth[] = {31, leapYear ? 29 : 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31};
    return day >= 1 && day <= daysPerMonth[month - 1];
}

// Today's local date as "YYYY-MM-DD".
inline std::string todayIsoDate() {
    const std::time_t now = std::time(nullptr);
    std::tm localDate{};
#ifdef _WIN32
    localtime_s(&localDate, &now);
#else
    localtime_r(&now, &localDate);
#endif
    std::ostringstream output;
    output << std::put_time(&localDate, "%Y-%m-%d");
    return output.str();
}

} // namespace sofralink::detail
