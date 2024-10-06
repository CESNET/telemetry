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

#pragma once

#include "node.hpp"

#include <memory>
#include <string_view>

namespace telemetry {

/**
 * @brief Represents a symbolic link in the telemetry filesystem.
 *
 * The Symlink class models a symbolic link (symlink) within the telemetry filesystem. A symlink
 * is a node that refers to another node (target), which can either be a file or a directory.
 * It enables transparent access to the target node by redirecting file system operations
 * through the symlink.
 */
class Symlink : public Node {
public:
	~Symlink() override = default;

	Symlink(const Symlink& other) = delete;
	Symlink& operator=(const Symlink& other) = delete;
	Symlink(Symlink&& other) = delete;
	Symlink& operator=(Symlink&& other) = delete;

	/**
	 * @brief Retrieves the target node that the symlink points to.
	 *
	 * This method returns a shared pointer to the node that the symlink is referencing. If the
	 * target node is no longer available, the method will return `nullptr`.
	 *
	 * @return A shared pointer to the target node, or nullptr if the target is no longer valid.
	 */
	std::shared_ptr<Node> getTarget() const;

private:
	/**
	 * @brief Constructs a Symlink object.
	 *
	 * The constructor is private and can only be called by the `Directory` class (via friend
	 * relationship), ensuring that symbolic links can only be created from within a directory
	 * context. It initializes the symbolic link with a reference to the parent directory, the name
	 * of the symlink, and the target node to which the symlink points.
	 *
	 * @param parent The parent node, which is typically a directory, where the symlink resides.
	 * @param name The name of the symbolic link.
	 * @param target The target node (file or directory) that the symlink points to.
	 */
	Symlink(
		const std::shared_ptr<Node>& parent,
		std::string_view name,
		const std::shared_ptr<Node>& target);

	std::weak_ptr<Node> m_target;

	// Directory class can create Symlink objects.
	friend class Directory;
};

} // namespace telemetry
