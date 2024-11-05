/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Telemetry utilities
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "directory.hpp"
#include "node.hpp"

#include <memory>
#include <string>
#include <vector>

namespace telemetry::utils {

/**
 * @brief Parse a telemetry path into segments separated by '/'.
 * @param path The telemetry path to parse.
 * @return A vector of path segments.
 */
std::vector<std::string> parsePath(const std::string& path);

/**
 * @brief Get a node from a given path in a directory structure.
 * @param parentDir The parent directory from which to start the search.
 * @param path The path of the node to retrieve.
 * @return A shared pointer to the retrieved node, or nullptr if not found.
 */
std::shared_ptr<Node>
getNodeFromPath(const std::shared_ptr<Directory>& parentDir, const std::string& path);

/**
 * @brief Check if a node represents a file.
 * @param node The node to check.
 * @return True if the node is a file, false otherwise.
 */
bool isFile(const std::shared_ptr<Node>& node) noexcept;

/**
 * @brief Check if a node represents a directory.
 * @param node The node to check.
 * @return True if the node is a directory, false otherwise.
 */
bool isDirectory(const std::shared_ptr<Node>& node) noexcept;

/**
 * @brief Check if a node represents a symlink.
 * @param node The node to check.
 * @return True if the node is a symlink, false otherwise.
 */
bool isSymlink(const std::shared_ptr<Node>& node) noexcept;

/**
 * @brief Check if a given path is the root directory.
 * @param path The path to check.
 * @return True if the path is the root directory, false otherwise.
 */
bool isRootDirectory(const std::string& path) noexcept;

} // namespace telemetry::utils
