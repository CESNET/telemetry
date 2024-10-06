/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Implementation of the Server class and associated telemetry functions.
 *
 * This source file implements the methods of the `Server` class, which is responsible for
 * managing server telemetry data. It includes functionalities for generating random telemetry
 * data, converting timestamps to string format, and reporting telemetry data to a specified
 * telemetry directory.
 *
 * The `Server` class allows tracking of various performance metrics such as CPU usage, memory
 * usage, latency, and disk usage. It utilizes the `ServerTelemetry` struct to hold the telemetry
 * data and interacts with the telemetry system to store and update this information.
 *
 * @copyright Copyright (c) 2024 CESNET, z.s.p.o.
 */

#include "server.hpp"

#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>

namespace telemetry::example {

/**
 * @brief Converts a `std::chrono::system_clock::time_point` to a formatted string.
 *
 * @param timePoint The time point to convert.
 * @return A formatted string representation of the time point in the format "YYYY-MM-DD HH:MM:SS".
 */
static std::string timePointToString(const std::chrono::system_clock::time_point& timePoint)
{
	const std::time_t time = std::chrono::system_clock::to_time_t(timePoint);

	std::stringstream timeStream;
	// NOLINTNEXTLINE(concurrency-mt-unsafe)
	timeStream << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

	return timeStream.str();
}

/**
 * @brief Generates random telemetry data for the server.
 *
 * This function generates random values for CPU usage, memory usage, latency, and disk usage,
 * and assigns the current system time as the timestamp.
 *
 * @return A `ServerTelemetry` structure populated with randomly generated telemetry data.
 */
static ServerTelemetry generateServerTelemetry()
{
	// NOLINTBEGIN
	std::random_device randDevice;
	std::mt19937 gen(randDevice());
	std::uniform_real_distribution<> cpuDist(0.0, 100.0);
	std::uniform_real_distribution<> memoryDist(0.0, 100.0);
	std::uniform_real_distribution<> latencyDist(1.0, 200.0);
	std::uniform_real_distribution<> diskDist(0.0, 100.0);
	// NOLINTEND

	ServerTelemetry telemetry;
	telemetry.cpuUsage = cpuDist(gen);
	telemetry.memoryUsage = memoryDist(gen);
	telemetry.latency = latencyDist(gen);
	telemetry.diskUsage = diskDist(gen);
	telemetry.timestamp = std::chrono::system_clock::now();

	return telemetry;
}

/**
 * @brief Converts `ServerTelemetry` data to a `telemetry::Dict`.
 *
 * @param telemetry The `ServerTelemetry` structure containing telemetry data.
 * @return A `telemetry::Dict` with telemetry values formatted and ready for reporting.
 */
static telemetry::Dict getServerTelemetry(const ServerTelemetry& telemetry)
{
	telemetry::Dict dict;
	dict["cpu_usage"] = telemetry::ScalarWithUnit {telemetry.cpuUsage, "%"};
	dict["memory_usage"] = telemetry::ScalarWithUnit {telemetry.memoryUsage, "%"};
	dict["latency"] = telemetry::ScalarWithUnit {telemetry.latency, "ms"};
	dict["disk_usage"] = telemetry::ScalarWithUnit {telemetry.diskUsage, "%"};
	dict["timestamp"] = timePointToString(telemetry.timestamp);
	return dict;
}

Server::Server(std::string serverId)
	: m_serverId(std::move(serverId))
{
}

std::string Server::getId() const
{
	return m_serverId;
}

/**
 * @brief Sets up telemetry for the server and adds a telemetry file to the directory.
 *
 * This function generates random telemetry data and sets up a file in the provided directory
 * to report the telemetry stats.
 *
 * @param serverDir A shared pointer to a telemetry directory where the telemetry data will be
 * stored.
 */
void Server::setupTelemetry(std::shared_ptr<telemetry::Directory>& serverDir)
{
	const auto statsFile = serverDir->addFile(
		"stats",
		{
			[this]() {
				m_telemetry = generateServerTelemetry();
				return getServerTelemetry(m_telemetry);
			},
			nullptr,
		});

	m_holder.add(statsFile);
}

} // namespace telemetry::example
