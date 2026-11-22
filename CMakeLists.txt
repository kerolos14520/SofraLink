cmake_minimum_required(VERSION 3.16)
project(SofraLink VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(SofraLink
    src/main.cpp
)

target_include_directories(SofraLink PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(SofraLink PRIVATE /W4)
else()
    target_compile_options(SofraLink PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Member 1 will add cpp-httplib and the approved nlohmann/json dependency
# centrally here when implementing the HTTP API and C++ integration.

add_executable(SofraLinkHeaderSmoke tests/header_smoke.cpp)
target_include_directories(SofraLinkHeaderSmoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(MSVC)
    target_compile_options(SofraLinkHeaderSmoke PRIVATE /W4)
else()
    target_compile_options(SofraLinkHeaderSmoke PRIVATE -Wall -Wextra -Wpedantic)
endif()
