/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Implementation of the DataCenter class for managing multiple servers and their telemetry
 * data.
 *
 * This source file implements the methods of the `DataCenter` class, which manages a collection
 * of `Server` objects and their associated telemetry data. It includes functionalities for adding
 * servers, setting up telemetry reporting, and aggregating data across all servers for summary
 * statistics.
 *
 * @copyright Copyright (c) 2024 CESNET, z.s.p.o.
 */

#include "dataCenter.hpp"

namespace telemetry::example {

/**
 * @brief Creates a summary file with aggregated telemetry data.
 *
 * @param filename The name of the summary file to be created.
 * @param filePattern The pattern to match server telemetry files.
 * @param patternRootDir Shared pointer to the working directory for patterns.
 * @param dir Shared pointer to the directory where the summary file will be added.
 * @return A shared pointer to the created aggregated file.
 */
static std::shared_ptr<telemetry::AggregatedFile> createSummaryFile(
	const std::string& filename,
	const std::string& filePattern,
	std::shared_ptr<telemetry::Directory>& patternRootDir,
	std::shared_ptr<telemetry::Directory>& dir)
{
	const std::vector<telemetry::AggOperation> aggOps {
		{telemetry::AggMethodType::AVG, "cpu_usage", "cpu_usage [avg]"},
		{telemetry::AggMethodType::AVG, "memory_usage", "memory_usage [avg]"},
		{telemetry::AggMethodType::AVG, "latency", "latency [avg]"},
		{telemetry::AggMethodType::MIN, "latency", "latency [min]"},
		{telemetry::AggMethodType::MAX, "latency", "latency [max]"},
		{telemetry::AggMethodType::AVG, "disk_usage", "disk_usage [avg]"},
	};

	return dir->addAggFile(filename, filePattern, aggOps, patternRootDir);
}

DataCenter::DataCenter(
	std::string location,
	uint64_t dataCenterId,
	std::shared_ptr<telemetry::Directory>& dataCenterDir)
	: m_rootDir(dataCenterDir)
	, m_location(std::move(location))
	, m_dataCenterId(dataCenterId)
{
	(void) m_dataCenterId;
	setupTelemetry(dataCenterDir);
}

void DataCenter::addServer(Server server)
{
	auto serverDir = m_rootDir->addDirs("servers/" + server.getId());
	server.setupTelemetry(serverDir);

	m_servers.emplace_back(std::move(server));
}

/**
 * @brief Sets up telemetry reporting for the data center.
 *
 * This method initializes the telemetry reporting structure for the data center. It creates
 * the necessary directories for storing server-specific telemetry data and summary statistics.
 *
 * It performs the following actions:
 * - Creates a directory to hold all server directories (`servers`).
 * - Creates a directory to store aggregated summary statistics (`summary`).
 * - Adds a file that tracks the count of servers currently managed by the data center.
 * - Creates an aggregated summary file that calculates average telemetry metrics (such as
 *   CPU usage, memory usage, latency, and disk usage) across all servers.
 *
 * The generated directories and files are added to the telemetry holder to manage their lifecycle
 * and ensure they are updated as telemetry data is collected from individual servers.
 *
 * @param dataCenterDir Shared pointer to the telemetry directory where the telemetry structure will
 * be established.
 */
void DataCenter::setupTelemetry(std::shared_ptr<telemetry::Directory>& dataCenterDir)
{
	auto serversDir = dataCenterDir->addDir("servers");
	auto summaryDir = dataCenterDir->addDir("summary");

	const auto serverCountFile = dataCenterDir->addFile(
		"server_count",
		{[&]() -> telemetry::Scalar { return m_servers.size(); }, nullptr});

	const auto summaryFile
		= createSummaryFile("summary_stats", "server_\\d+/stats", serversDir, summaryDir);

	m_holder.add(serversDir);
	m_holder.add(summaryDir);
	m_holder.add(serverCountFile);
	m_holder.add(summaryFile);
}

} // namespace telemetry::example
