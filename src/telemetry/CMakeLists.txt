add_library(telemetry SHARED
	content.cpp
)

target_include_directories(telemetry PUBLIC ${CMAKE_SOURCE_DIR}/include)

install(TARGETS telemetry LIBRARY DESTINATION ${INSTALL_DIR_LIB})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${INSTALL_DIR_INCLUDE})
