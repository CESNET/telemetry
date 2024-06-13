/**
 * @file
 * @brief Definition of the AppFsFuse class for managing FUSE filesystem
 * @author Pavel Siska <siska@cesnet.cz>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <fuse3/fuse.h>
#include <memory>
#include <string>
#include <telemetry.hpp>
#include <thread>

namespace telemetry::appFs {

/**
 * @brief The AppFsFuse class for managing FUSE filesystem.
 */
class AppFsFuse {
public:
	/**
	 * @brief Sets up and mount the FUSE filesystem.
	 *
	 * This method sets up the FUSE filesystem with the provided mount point directory path.
	 *
	 * @param rootDirectory A shared pointer to the telemetry root directory.
	 * @param mountPoint The mount point directory path.
	 * @param tryToUnmountOnStart Whether to attempt unmounting the mount point if it's already
	 * mounted.
	 * @param createMountPoint Whether to create the mount point directory if it doesn't exist.
	 *
	 * @throws std::runtime_error if rootDirectory is nullptr.
	 * @throws std::runtime_error if setup and mount process fails.
	 */
	AppFsFuse(
		std::shared_ptr<Directory> rootDirectory,
		const std::string& mountPoint,
		bool tryToUnmountOnStart = true,
		bool createMountPoint = false);

	/**
	 * @brief Creates a new thread to run the FUSE event loop.
	 *
	 * After the thread is created, you can access the FUSE filesystem through the mount point.
	 *
	 * @throws std::runtime_error if the FUSE thread is already running.
	 */
	void start();

	/**
	 * @brief Unmount the FUSE filesystem and join the FUSE thread.
	 *
	 * @note It's not possible to start the FUSE filesystem again after calling this method.
	 * If needed, a new instance of the class must be created.
	 */
	void stop();

	/**
	 * @brief Destructor for AppFsFuse.
	 */
	~AppFsFuse();

private:
	void unmount();

	std::unique_ptr<struct fuse, decltype(&fuse_destroy)> m_fuse {nullptr, &fuse_destroy};
	std::shared_ptr<Directory> m_rootDirectory;
	bool m_isStarted = false;
	std::thread m_fuseThread;
};

} // namespace telemetry::appFs
