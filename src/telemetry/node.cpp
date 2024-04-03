/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Telemetry node
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/node.hpp>

#include <algorithm>
#include <cctype>

namespace telemetry {

Node::Node(std::shared_ptr<Node> parent, std::string_view name)
	: m_parent(std::move(parent))
	, m_name(name)
{
	if (m_parent == nullptr) {
		throwTelemetryException("parent cannot be nullptr");
	}

	checkName(m_name);
}

static bool isValidCharacter(char character)
{
	if (std::isalnum(character) != 0) {
		return true;
	}

	if (character == '-' || character == '_') {
		return true;
	}

	return false;
}

std::string Node::getFullPath()
{
	std::string result;

	if (!m_parent) {
		return m_name.empty() ? "/" : getName();
	}

	result = m_parent->getFullPath();
	if (result.back() != '/') {
		result += '/';
	}

	return result + getName();
}

void Node::checkName(std::string_view name)
{
	if (name.empty()) {
		throwTelemetryException("empty name is not allowed");
	}

	const auto* const pos = std::find_if_not(name.begin(), name.end(), isValidCharacter);
	if (pos != name.end()) {
		const std::string err = "prohibited character '" + std::to_string(*pos) + "'";
		throwTelemetryException(err);
	}
}

void Node::throwTelemetryException(std::string_view err)
{
	const std::string msg = "Node('" + getFullPath() + "') has failed: ";
	throw TelemetryException(msg + std::string(err));
}

} // namespace telemetry
