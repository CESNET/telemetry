/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Declaration of the DataCenter class for managing multiple servers and their telemetry
 * data.
 *
 * This header file defines the `DataCenter` class, which is responsible for managing a collection
 * of `Server` objects and their associated telemetry data. The `DataCenter` allows adding servers,
 * setting up their telemetry reporting, and aggregating data across all servers for summary
 * statistics.
 *
 * The telemetry data is organized in a hierarchical directory structure and can include information
 * such as CPU usage, memory usage, latency, and disk usage for each server.
 *
 * @copyright Copyright (c) 2024 CESNET, z.s.p.o.
 */

#pragma once

#include "server.hpp"

#include <memory>
#include <string>
#include <telemetry.hpp>
#include <vector>

namespace telemetry::example {

/**
 * @brief Class representing a data center that manages multiple servers.
 *
 * This class provides functionality to add servers, setup telemetry reporting,
 * and aggregate telemetry data across all managed servers.
 */
class DataCenter {
public:
	/**
	 * @brief Constructs a new DataCenter object with a specified location and telemetry directory.
	 *
	 * @param location The physical location of the data center.
	 * @param dataCenterDir Shared pointer to the telemetry directory for this data center.
	 */
	DataCenter(
		std::string location,
		uint64_t dataCenterId,
		std::shared_ptr<telemetry::Directory>& dataCenterDir);

	DataCenter(const DataCenter& other) = delete;
	DataCenter& operator=(const DataCenter& other) = delete;
	DataCenter(DataCenter&& other) = default;
	DataCenter& operator=(DataCenter&& other) = default;

	/**
	 * @brief Adds a server to the data center.
	 * @param server The server to be added.
	 */
	void addServer(Server server);

private:
	/**
	 * @brief Sets up telemetry reporting for the data center.
	 * @param dataCenterDir Shared pointer to the telemetry directory.
	 */
	void setupTelemetry(std::shared_ptr<telemetry::Directory>& dataCenterDir);

	std::shared_ptr<telemetry::Directory>
		m_rootDir; ///< Pointer to the root data center telemetry directory.
	std::string m_location; ///< The location of the data center.
	uint64_t m_dataCenterId; ///< The unique identifier of the data center.
	telemetry::Holder m_holder; ///< Holder for managing telemetry files.
	std::vector<Server> m_servers; ///< Vector to store added servers.
};

} // namespace telemetry::example
