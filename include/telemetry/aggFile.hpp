/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Aggregated telemetry file
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "aggMethod.hpp"
#include "content.hpp"
#include "file.hpp"
#include "node.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace telemetry {

/**
 * @brief Class representing an aggregated file
 *
 * AggregatedFile is a subclass of File and is responsible for aggregating telemetry data from
 * multiple files. It reads data from matched files based on a regex pattern and applies aggregation
 * operations defined by aggregation methods.
 */
class AggregatedFile : public File {
public:
	~AggregatedFile() override = default;

	// Object cannot be copied or moved as it would break references from directories.
	AggregatedFile(const AggregatedFile& other) = delete;
	AggregatedFile& operator=(const AggregatedFile& other) = delete;
	AggregatedFile(AggregatedFile&& other) = delete;
	AggregatedFile& operator=(AggregatedFile&& other) = delete;

	/**
	 * @brief Execute read operation over all matched files and aggregate them.
	 *
	 * This function reads data from all matched files based on the regex pattern and aggregates it
	 * using aggregation methods specified during object creation.
	 *
	 * @return Aggregated content.
	 * @throw TelemetryException if an error occurs during aggregation.
	 */
	Content read();

private:
	// Allow directory to call AggregatedFile constructor
	friend class Directory;
	// Can be created only from a directory. Must be always created as a shared_ptr.
	AggregatedFile(
		const std::shared_ptr<Node>& parent,
		std::string_view name,
		std::string aggFilesPattern,
		const std::vector<AggOperation>& ops,
		std::shared_ptr<Directory> patternRootDir = nullptr);

	FileOps getOps();

	const std::string m_filesRegexPattern;

	std::shared_ptr<Directory> m_patternRootDir;
	std::vector<std::string> m_paths;
	std::vector<std::unique_ptr<AggMethod>> m_aggMethods;
};

} // namespace telemetry
