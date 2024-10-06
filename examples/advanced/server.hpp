/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Contains the declaration of the Server class and ServerTelemetry struct for tracking
 * server telemetry data.
 *
 * This header file defines a class and associated functions that allow tracking and reporting
 * server telemetry data such as CPU usage, memory usage, network latency, and disk usage.
 * The telemetry data is stored in a telemetry directory.
 *
 * The primary class is `Server`, which allows telemetry to be set up for a specific server
 * and stores data in a telemetry system using the `telemetry::Directory` and `telemetry::Holder`
 * objects.
 *
 * @copyright Copyright (c) 2024 CESNET, z.s.p.o.
 */

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <telemetry.hpp>

namespace telemetry::example {

/**
 * @brief Struct that holds telemetry data for a server.
 *
 * This structure stores key telemetry metrics including CPU usage, memory usage,
 * latency, disk usage, and a timestamp representing when the data was collected.
 */
struct ServerTelemetry {
	double cpuUsage; //< CPU usage percentage.
	double memoryUsage; //< Memory usage percentage.
	double latency; //< Network latency in milliseconds.
	double diskUsage; //< Disk usage percentage.
	std::chrono::system_clock::time_point timestamp; //< Timestamp when the telemetry was captured.
};

/**
 * @brief Class that represents a server and its associated telemetry data.
 *
 * The `Server` class allows tracking and storing telemetry data for a specific
 * server, including CPU usage, memory usage, network latency, and disk usage.
 * It also provides methods to set up the telemetry reporting mechanism.
 */
class Server {
public:
	/**
	 * @brief Constructor for the `Server` class.
	 *
	 * @param serverId A string that uniquely identifies the server.
	 */
	explicit Server(std::string serverId);

	/**
	 * @brief Sets up telemetry reporting for the server.
	 *
	 * @param serverDir A shared pointer to a telemetry directory where server
	 * telemetry data will be stored.
	 */
	void setupTelemetry(std::shared_ptr<telemetry::Directory>& serverDir);

	/**
	 * @brief Gets the server's unique identifier.
	 *
	 * @return A string representing the server's ID.
	 */
	[[nodiscard]] std::string getId() const;

private:
	std::string m_serverId; //< Unique identifier for the server.
	ServerTelemetry m_telemetry = {}; //< Holds the server's telemetry data.
	telemetry::Holder m_holder; //< Telemetry holder for managing telemetry data files.
};

} // namespace telemetry::example
