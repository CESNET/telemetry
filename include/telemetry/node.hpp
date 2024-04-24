/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Telemetry node
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>

namespace telemetry {

/**
 * @brief Exception of a Telemetry node.
 */
class TelemetryException : public std::runtime_error {
public:
	TelemetryException(const char* whatArg)
		: std::runtime_error(whatArg) {};
	TelemetryException(const std::string& whatArg)
		: std::runtime_error(whatArg) {};
};

/**
 * @brief Common type for all Telemetry nodes.
 *
 * Each node contains a name, reference to its parent (might be empty)
 * and a lock that is used to protect node modification.
 */
class Node : public std::enable_shared_from_this<Node> {
public:
	/**
	 * @brief Construct a root node (i.e. without name and parent).
	 */
	Node() = default;
	/**
	 * @brief Construct a new node with the given @p name and @p parent.
	 *
	 * The name can contain only digits (0-9), letters (A-Z, a-z), and a few
	 * special characters ("-", "_"). If the node doesn't have a parent, its
	 * name can be empty.
	 * @param[in] parent Parent node of the node (cannot be nullptr).
	 * @param[in] name   Name of the node.
	 */
	Node(std::shared_ptr<Node> parent, std::string_view name);
	virtual ~Node() = default;

	Node(const Node& other) = delete;
	Node& operator=(const Node& other) = delete;
	Node(Node&& other) = delete;
	Node& operator=(Node&& other) = delete;

	/** @brief Get reference to the internal mutex. */
	std::mutex& getMutex() { return m_mutex; };
	/** @brief Get the name of the node. */
	const std::string& getName() const noexcept { return m_name; };
	/** @brief Get full path from the root to this node (including this node name). */
	std::string getFullPath();

protected:
	std::shared_ptr<Node> m_parent;

private:
	std::mutex m_mutex;
	std::string m_name;

	void checkName(std::string_view name);
	[[noreturn]] void throwTelemetryException(std::string_view err);
};

} // namespace telemetry
