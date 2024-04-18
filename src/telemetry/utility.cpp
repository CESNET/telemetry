/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Telemetry utilities
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/utility.hpp>

#include <sstream>
#include <telemetry/directory.hpp>
#include <telemetry/file.hpp>

namespace telemetry::utils {

std::vector<std::string> parsePath(const std::string& path)
{
	std::vector<std::string> pathSegments;

	std::istringstream iss(path);
	std::string token;
	while (std::getline(iss, token, '/')) {
		if (!token.empty()) {
			pathSegments.push_back(token);
		}
	}

	return pathSegments;
}

std::shared_ptr<Node>
getNodeFromPath(const std::shared_ptr<Directory>& parentDir, const std::string& path)
{
	if (isRootDirectory(path)) {
		return parentDir;
	}

	auto pathSegments = parsePath(path);
	if (pathSegments.empty()) {
		return nullptr;
	}

	const std::string topLevelName = pathSegments.back();
	pathSegments.pop_back();

	auto directory = parentDir;
	std::shared_ptr<telemetry::Node> node;

	for (const auto& part : pathSegments) {
		node = directory->getEntry(part);
		if (!node) {
			return nullptr;
		}

		if (isDirectory(node)) {
			directory = std::dynamic_pointer_cast<telemetry::Directory>(node);
		} else {
			return nullptr;
		}
	}
	return directory->getEntry(topLevelName);
}

bool isFile(const std::shared_ptr<Node>& node) noexcept
{
	return std::dynamic_pointer_cast<File>(node) != nullptr;
}

bool isDirectory(const std::shared_ptr<Node>& node) noexcept
{
	return std::dynamic_pointer_cast<Directory>(node) != nullptr;
}

bool isRootDirectory(const std::string& path) noexcept
{
	return path == "/";
}

} // namespace telemetry::utils

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testUtility.cpp"
#endif
