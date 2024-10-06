/**
 * @file main.cpp
 * @brief Main entry point for the telemetry data center application.
 *
 * This program initializes telemetry data centers, each containing a set of servers that report
 * telemetry data. It uses a signal handler to allow for graceful termination of the application.
 * The telemetry data is organized in a hierarchical structure and can be mounted as a filesystem.
 *
 * ## Usage
 *
 * The application expects one argument:
 * - `mount_point`: The directory where the telemetry data can be accessed as a filesystem.
 *
 * @note The application will run indefinitely until interrupted by a signal (e.g., SIGINT).
 *
 * @example ./advanced-example "/tmp/telemetry"
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "dataCenter.hpp"
#include "server.hpp"

#include <appFs.hpp>
#include <telemetry.hpp>

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace telemetry::example;

std::atomic<bool> g_stopFlag(false);

void signalHandler(int signum)
{
	(void) signum;
	g_stopFlag.store(true);
}

/**
 * @brief Creates a DataCenter object for a given location.
 *
 * This function initializes a DataCenter for the specified location and associates it with a
 * telemetry directory.
 *
 * @param location The location of the data center.
 * @param dataCentersDir Shared pointer to the directory where the data center will be created.
 * @return A DataCenter object initialized for the specified location.
 */
DataCenter
createDataCenter(const std::string& location, std::shared_ptr<telemetry::Directory>& dataCentersDir)
{
	auto dataCenterDir = dataCentersDir->addDir(location);
	return {location, dataCenterDir};
}

/**
 * @brief Creates multiple DataCenter objects for predefined locations.
 *
 * This function initializes a set of data centers, each containing a predefined number of servers.
 *
 * @param dataCentersDir Shared pointer to the directory where the data centers will be created.
 * @return A vector of initialized DataCenter objects.
 */
std::vector<DataCenter> createDataCenters(std::shared_ptr<telemetry::Directory>& dataCentersDir)
{
	std::vector<DataCenter> dataCenters;
	dataCenters.emplace_back(createDataCenter("prague", dataCentersDir));
	dataCenters.emplace_back(createDataCenter("new_york", dataCentersDir));
	dataCenters.emplace_back(createDataCenter("tokyo", dataCentersDir));

	const std::size_t serversPerDatacenter = 3;

	// add servers to each data center
	for (auto& dataCenter : dataCenters) {
		for (std::size_t serverId = 0; serverId < serversPerDatacenter; serverId++) {
			dataCenter.addServer(Server("server_" + std::to_string(serverId)));
		}
	}

	return dataCenters;
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <mount_point>\n";
		return EXIT_FAILURE;
	}

	const std::string mountPoint = argv[1];

	signal(SIGINT, signalHandler);

	std::shared_ptr<telemetry::Directory> telemetryRootDirectory;
	std::unique_ptr<telemetry::appFs::AppFsFuse> appFs;

	try {
		// create telemetry root directory
		telemetryRootDirectory = telemetry::Directory::create();

		auto dataCentersDir = telemetryRootDirectory->addDir("data_centers");
		auto dataCenters = createDataCenters(dataCentersDir);

		const bool tryToUnmountOnStart = true;
		const bool createMountPoint = true;
		appFs = std::make_unique<telemetry::appFs::AppFsFuse>(
			telemetryRootDirectory,
			mountPoint,
			tryToUnmountOnStart,
			createMountPoint);
		/**
		 * Starts the application filesystem.
		 *
		 * Telemetry data is now accessible as a filesystem mounted at the specified mount point
		 */
		appFs->start();

		while (!g_stopFlag.load()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

	} catch (const std::exception& ex) {
		std::cerr << ex.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
