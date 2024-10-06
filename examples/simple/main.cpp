/**
 * @file
 * @brief Simple example of a telemetry application that expose data through appFs.
 *
 * This application demonstrates how to create a simple telemetry application that provides
 * information about its process ID (PID), start time, version, parameters, and uptime.
 * The telemetry data is exposed as a filesystem using appFs, allowing users to access it through
 * standard filesystem operations.

 * ## Usage
 * The application expects one argument:
 * - `mount_point`: The directory where the telemetry data can be accessed as a filesystem.
 *
 * @note The application will run indefinitely until interrupted by a signal (e.g., SIGINT).
 *
 * @example ./simple-example "/tmp/telemetry"
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <appFs.hpp>
#include <telemetry.hpp>

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>

static const std::string g_VERSION = "1.0.0";

std::atomic<bool> g_stopFlag(false);

void signalHandler(int signum)
{
	(void) signum;
	g_stopFlag.store(true);
}

std::time_t getStartTime()
{
	return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

std::string convertTimeToString(const std::time_t time)
{
	std::ostringstream oss;
	// NOLINTNEXTLINE(concurrency-mt-unsafe,-warnings-as-errors)
	oss << std::put_time(std::localtime(&time), "%F %T");
	return oss.str();
}

int64_t getUptimeInSec(const std::time_t startTime)
{
	const auto now = std::chrono::system_clock::now();
	const auto uptime = now - std::chrono::system_clock::from_time_t(startTime);
	return std::chrono::duration_cast<std::chrono::seconds>(uptime).count();
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <mount_point>\n";
		return EXIT_FAILURE;
	}

	const std::string mountPoint = argv[1];

	signal(SIGINT, signalHandler);

	const auto startTime = getStartTime();

	std::shared_ptr<telemetry::Directory> rootDir;
	std::unique_ptr<telemetry::appFs::AppFsFuse> appFs;

	try {
		// create telemetry root directory
		rootDir = telemetry::Directory::create();

		const auto pidFile
			= rootDir->addFile("pid", {[]() { return static_cast<uint64_t>(getpid()); }});
		const auto startTimeFile = rootDir->addFile(
			"start_time",
			{
				[&startTime]() { return convertTimeToString(startTime); },
			});

		const auto versionFile = rootDir->addFile("version", {[]() { return g_VERSION; }});
		const auto parametersFile = rootDir->addFile(
			"parameters",
			{
				[argc, argv]() {
					std::ostringstream oss;
					for (int idx = 0; idx < argc; idx++) {
						oss << argv[idx] << " ";
					}
					return oss.str();
				},
			});
		const auto uptimeFile = rootDir->addFile(
			"uptime",
			{
				[&startTime]() {
					return telemetry::ScalarWithUnit(getUptimeInSec(startTime), "s");
				},
			});

		const bool tryToUnmountOnStart = true;
		const bool createMountPoint = true;
		appFs = std::make_unique<telemetry::appFs::AppFsFuse>(
			rootDir,
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
