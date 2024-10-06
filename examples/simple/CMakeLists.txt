add_executable(simple-example
	main.cpp
)

target_link_libraries(simple-example PUBLIC
	telemetry::telemetry
	telemetry::appFs
)
