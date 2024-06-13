list(APPEND APPFS_SOURCE_FILES
	appFs.cpp
)

if (TELEMETRY_BUILD_SHARED)
	add_library(appFs SHARED ${APPFS_SOURCE_FILES})
else()
	add_library(appFs STATIC ${APPFS_SOURCE_FILES})
endif()

add_library(telemetry::appFs ALIAS appFs)

target_compile_definitions(appFs PUBLIC FUSE_USE_VERSION=30 _FILE_OFFSET_BITS=64)
target_link_libraries(appFs PUBLIC telemetry PkgConfig::fuse stdc++fs)
target_include_directories(appFs PUBLIC ${CMAKE_SOURCE_DIR}/include)

if (TELEMETRY_INSTALL_TARGETS)
	install(TARGETS appFs LIBRARY DESTINATION ${INSTALL_DIR_LIB})
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${INSTALL_DIR_INCLUDE})
endif()
