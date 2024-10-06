add_executable(advanced-example
	main.cpp
	dataCenter.cpp
	server.cpp
)


target_link_libraries(advanced-example PRIVATE
	telemetry::telemetry
	telemetry::appFs
)
