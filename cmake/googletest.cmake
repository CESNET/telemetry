# Google Test library
#
# Google Test is a testing framework for C++ developed by Google.
#
# This CMake script integrates Google Test into the project, allowing easy usage.
# Usage example: target_link_libraries(my_target PRIVATE GTest::gtest_main)

include(FetchContent)

FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        v1.14.0
)

# Library does not compile with -Wsign-conversion that we use in builds
string(REPLACE "-Wsign-conversion " " " CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")
string(REPLACE "-Wsign-conversion " " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ")
string(REPLACE "-Wsign-conversion " " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

FetchContent_MakeAvailable(googletest)
