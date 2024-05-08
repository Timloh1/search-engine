cmake_minimum_required (VERSION 3.8)

project ("search_engine")

add_executable (search_engine main.cpp search_engine.h search_engine.cpp)
#json
include(FetchContent)

FetchContent_Declare(
	json 
	URL 
	https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
#googletest
FetchContent_Declare(
	googletest
	URL
	https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
enable_testing()

target_link_libraries(search_engine PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(search_engine)


