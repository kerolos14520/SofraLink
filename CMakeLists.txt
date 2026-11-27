cmake_minimum_required(VERSION 3.16)
project(SofraLink VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Core library: every service implementation lives here (single shared target).
# A source is added automatically once its file exists in the repo, so each
# member's PR only needs to add the file itself (no CMake edits, fewer conflicts).
# Member 1 will add MatchingEngine/ApiServer and the JSON/HTTP dependencies
# (nlohmann/json, cpp-httplib) centrally here.
# ---------------------------------------------------------------------------
set(SOFRALINK_CORE_CANDIDATES
    src/services/Validation.cpp
    src/services/SearchService.cpp
    src/services/DonationManager.cpp
    src/services/RequestManager.cpp
    src/services/MatchingEngine.cpp
)
set(SOFRALINK_CORE_SOURCES "")
foreach(src IN LISTS SOFRALINK_CORE_CANDIDATES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND SOFRALINK_CORE_SOURCES ${src})
    endif()
endforeach()

add_library(sofralink_core STATIC ${SOFRALINK_CORE_SOURCES})
target_include_directories(sofralink_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

function(sofralink_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()
sofralink_warnings(sofralink_core)

add_executable(SofraLink src/main.cpp)
target_link_libraries(SofraLink PRIVATE sofralink_core)
sofralink_warnings(SofraLink)

add_executable(SofraLinkHeaderSmoke tests/header_smoke.cpp)
target_link_libraries(SofraLinkHeaderSmoke PRIVATE sofralink_core)
sofralink_warnings(SofraLinkHeaderSmoke)

# ---------------------------------------------------------------------------
# Tests (CTest). Run:  ctest --test-dir build -C Release --output-on-failure
# Tests use plain assert(); Release builds define NDEBUG which would silently
# turn every assert into a no-op, so assertions are force-enabled for tests.
# ---------------------------------------------------------------------------
enable_testing()

add_test(NAME header_smoke COMMAND SofraLinkHeaderSmoke)

set(SOFRALINK_UNIT_TESTS
    validation_tests
    search_service_tests
    donation_manager_tests
    request_manager_tests
)
foreach(test_name IN LISTS SOFRALINK_UNIT_TESTS)
    set(test_src "tests/unit/${test_name}.cpp")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_src}")
        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} PRIVATE sofralink_core)
        sofralink_warnings(${test_name})
        if(MSVC)
            target_compile_options(${test_name} PRIVATE /UNDEBUG)
        else()
            target_compile_options(${test_name} PRIVATE -UNDEBUG)
        endif()
        add_test(NAME ${test_name} COMMAND ${test_name})
    endif()
endforeach()
# json_storage_tests / report_service_tests (Member 6) are registered once
# nlohmann/json is added to this file.
