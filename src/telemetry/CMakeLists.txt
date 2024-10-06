list(APPEND TELEMETRY_SOURCE_FILES
	content.cpp
	node.cpp
	file.cpp
	directory.cpp
	holder.cpp
	utility.cpp
	aggFile.cpp
	symlink.cpp
	aggregator/aggMethod.cpp
	aggregator/aggSum.cpp
	aggregator/aggAvg.cpp
	aggregator/aggMinMax.cpp
	aggregator/aggJoin.cpp
	aggregator/aggMethodFactory.cpp
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

if (TELEMETRY_ENABLE_TESTS)
	add_executable(testTelemetry ${TELEMETRY_SOURCE_FILES})
	target_compile_definitions(testTelemetry PRIVATE TELEMETRY_ENABLE_TESTS)
	target_include_directories(testTelemetry PRIVATE ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(testTelemetry GTest::gtest_main)
	gtest_discover_tests(testTelemetry)
endif()
