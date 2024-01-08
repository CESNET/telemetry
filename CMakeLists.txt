cmake_minimum_required(VERSION 3.12)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

project(telemetry VERSION ${VERSION})

include(cmake/build_type.cmake)
include(cmake/installation.cmake)

option(TELEMETRY_BUILD_SHARED "Build shared library" ON)
option(TELEMETRY_PACKAGE_BUILDER "Enable RPM package builder (make rpm)" ON)
option(TELEMETRY_INSTALL_TARGETS "Generate the install target" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb3")
