cmake_minimum_required(VERSION 3.12)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_PATCH 2)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

project(telemetry VERSION ${VERSION})

include(cmake/build_type.cmake)
include(cmake/installation.cmake)

option(TELEMETRY_BUILD_SHARED "Build shared library" ON)
option(TELEMETRY_PACKAGE_BUILDER "Enable RPM package builder (make rpm)" ON)
option(TELEMETRY_INSTALL_TARGETS "Generate the install target" ON)
option(TELEMETRY_ENABLE_TESTS "Build Unit tests (make test)" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb3")

# This option ensures that standard include header paths are explicitly added
# to the generated compile_commands.json file.
# clang-tidy doesn't seem to find standard include headers otherwise.
if (CMAKE_EXPORT_COMPILE_COMMANDS)
	set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

include(cmake/dependencies.cmake)

if (TELEMETRY_ENABLE_TESTS)
	include(cmake/googletest.cmake)
	include(GoogleTest)
	enable_testing()
endif()

add_subdirectory(src)

if (TELEMETRY_PACKAGE_BUILDER)
	add_subdirectory(pkg)
endif()


