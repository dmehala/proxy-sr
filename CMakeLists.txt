cmake_minimum_required(VERSION 3.14)
project(proxy-sr)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES src/helper.cpp src/proxy.cpp)
set(TEST_SOURCES src/helper.cpp tests/helper_test.cpp)

include(FetchContent)

# GTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Poco
# FetchContent_Declare(
#   poco
#   GIT_REPOSITORY "https://github.com/pocoproject/poco.git"
#   GIT_TAG        "poco-1.10.0-release"
# )
# FetchContent_MakeAvailable(poco)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

enable_testing()

include_directories(include)

add_executable(proxy-sr ${SOURCES} src/main.cpp)

target_include_directories(proxy-sr PRIVATE include)
target_link_libraries(proxy-sr OpenSSL::SSL)
target_link_libraries(proxy-sr OpenSSL::Crypto)
target_link_libraries(proxy-sr Threads::Threads)

# TESTS

add_executable(proxy-sr_tests ${TEST_SOURCES})

target_link_libraries(proxy-sr_tests gtest_main)
# target_link_libraries(proxy-sr_tests OpenSSL::SSL)
# target_link_libraries(proxy-sr_tests OpenSSL::Crypto)
# target_link_libraries(proxy-sr_tests Threads::Threads)

include(GoogleTest)
gtest_discover_tests(proxy-sr_tests)