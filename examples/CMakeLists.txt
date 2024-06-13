add_executable(test1
	basicExample.cpp
)

target_link_libraries(test1 PRIVATE
	telemetry::telemetry
	telemetry::appFs
)

add_executable(test2
	contentExample.cpp
)

target_link_libraries(test2 PRIVATE
	telemetry::telemetry
	telemetry::appFs
)

add_executable(test3
	holderExample.cpp
)

target_link_libraries(test3 PRIVATE
	telemetry::telemetry
	telemetry::appFs
)

add_executable(test4
	utilsExample.cpp
)

target_link_libraries(test4 PRIVATE
	telemetry::telemetry
	telemetry::appFs
)

add_executable(test5
	aggregatedFileExample.cpp
)

target_link_libraries(test5 PRIVATE
	telemetry::telemetry
	telemetry::appFs
)
