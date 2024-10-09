/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Defines the Symlink class for representing symbolic links in the telemetry filesystem.
 *
 * This file contains the declaration of the Symlink class, which represents a symbolic link node
 * in the telemetry filesystem. A symbolic link is a special type of node that points to another
 * node (either a file or directory) and enables indirect access to it.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/symlink.hpp>

namespace telemetry {

std::shared_ptr<Node> Symlink::getTarget() const
{
	return m_target.lock();
}

Symlink::Symlink(
	const std::shared_ptr<Node>& parent,
	std::string_view name,
	const std::shared_ptr<Node>& target)
	: Node(parent, name)
	, m_target(target)
{
}

} // namespace telemetry

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testSymlink.cpp"
#endif
