cmake_minimum_required(VERSION 3.26.4)
project(LOB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(Headers
  include/Order.hpp
  include/Limit.hpp
  include/OrderBook.hpp
  include/OrderBookPrinter.hpp
  include/Util.hpp
)

set(Sources
  source/Order.cpp
  source/Limit.cpp
  source/OrderBook.cpp
  source/OrderBookPrinter.cpp
)

add_library(LOB_Library STATIC ${Sources} ${Headers})

enable_testing()
include(GoogleTest)
add_subdirectory(tests)

add_executable(main source/main.cpp)
target_link_libraries(main LOB_Library)