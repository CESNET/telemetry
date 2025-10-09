cmake_minimum_required(VERSION 3.18)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 3)
set(VERSION_PATCH 1)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

project(telemetry VERSION ${VERSION})

include(cmake/build_type.cmake)
include(cmake/installation.cmake)
include(cmake/dependencies.cmake)

option(TELEMETRY_BUILD_SHARED       "Build shared library" ON)
option(TELEMETRY_PACKAGE_BUILDER    "Enable RPM package builder (make rpm)" OFF)
option(TELEMETRY_INSTALL_TARGETS    "Generate the install target" ON)
option(TELEMETRY_ENABLE_TESTS       "Build Unit tests (make test)" OFF)
option(TELEMETRY_ENABLE_DOC_DOXYGEN "Enable build of code documentation" OFF)
option(TELEMETRY_BUILD_EXAMPLES     "Build included examples files (make examples)" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Wunused -Wconversion -Wsign-conversion")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb3")

if (TELEMETRY_ENABLE_TESTS)
	include(cmake/googletest.cmake)
	include(GoogleTest)
	enable_testing()
endif()

add_subdirectory(src)
add_subdirectory(doc)

if (TELEMETRY_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if (TELEMETRY_PACKAGE_BUILDER)
	add_subdirectory(pkg)
endif()
