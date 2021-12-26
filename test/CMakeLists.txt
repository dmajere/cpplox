cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 17)

set(This cpploxtest)
set(Sources
    ScannerTests.cpp
    ParserTests.cpp
)

add_executable(${This} ${Sources})
find_package(GTest CONFIG REQUIRED)
target_link_libraries(${This} GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main cpploxlib)

include(GoogleTest)
gtest_discover_tests(${This})