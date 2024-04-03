list(APPEND TELEMETRY_SOURCE_FILES
	content.cpp
	node.cpp
	file.cpp
	directory.cpp
)

if (TELEMETRY_BUILD_SHARED)
	add_library(telemetry SHARED ${TELEMETRY_SOURCE_FILES})
else()
	add_library(telemetry STATIC ${TELEMETRY_SOURCE_FILES})
endif()

add_library(telemetry::telemetry ALIAS telemetry)
target_include_directories(telemetry PUBLIC ${PROJECT_SOURCE_DIR}/include)

if (TELEMETRY_INSTALL_TARGETS)
	install(TARGETS telemetry LIBRARY DESTINATION ${INSTALL_DIR_LIB})
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_DIR_INCLUDE})
endif()
