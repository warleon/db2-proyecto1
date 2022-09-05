cmake_minimum_required(VERSION 3.14)

# set the project name
project(Project_1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

SET(GCC_COMPILE_FLAGS "-Wall -Werror -fsanitize=address -pthread")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")

include(FetchContent)
#include <googletest>
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

include_directories(src libs)
link_directories(src libs)

enable_testing()

#--------------------------------GENERAL RECORD------------------------------------------#
add_executable(
  generalRecord
  tests/genRecordTest.cpp
)
target_link_libraries(
  generalRecord
  PRIVATE
  gtest_main
)
#----------------------------HASH TABLE----------------------------------------------#
add_executable(
  hashTable
  tests/hashTableTest.cpp
)
target_link_libraries(
  hashTable
  PRIVATE
  gtest_main
)
#----------------------------SEQUENTIAL----------------------------------------------#
add_executable(
  sequential
  tests/sequentialTest.cpp
)
target_link_libraries(
  sequential
  PRIVATE
  gtest_main
)
gtest_discover_tests(hashTable)
gtest_discover_tests(sequential)

#----------------------------DEMO----------------------------------------------#
add_executable(
  program 
  src/main.cpp
)