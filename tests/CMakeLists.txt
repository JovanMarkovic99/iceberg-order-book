cmake_minimum_required(VERSION 3.26.4)

set(This UnitTests)

set(Sources
    engine_test.cpp
)

add_executable(${This} ${Sources})
target_link_libraries(${This} PUBLIC
    GTest::gtest_main
    LOB_Library
)

add_test(
    NAME ${This}
    COMMAND ${This}
)