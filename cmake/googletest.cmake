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
FetchContent_MakeAvailable(googletest)
