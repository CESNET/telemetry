/**
 * @file
 * @author Daniel Pelanek <xpeland00@vutbr.cz>
 * @brief Shows how to use utility functions
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <appFs.hpp>
#include <telemetry.hpp>

// example help
#include <iostream>
#include <string>

void traverseDir(std::shared_ptr<telemetry::Directory>& dir, int depth)
{
	// Using utils to check if a directory is a root directory
	if (telemetry::utils::isRootDirectory(dir->getFullPath())) {
		std::cout << "root\n";
	}

	// looping through files and dirs in directory
	// listEntries returns their names as string
	for (auto node_name : dir->listEntries()) {
		auto node = dir->getEntry(node_name);

		std::cout << std::string(depth * 4, ' ') << node_name << "\n";

		// Using utils to check if a node is a directory
		if (telemetry::utils::isDirectory(node)) {
			// dynamic_pointer_cast is needed to get a file or dir from node
			auto newDir = std::dynamic_pointer_cast<telemetry::Directory>(node);
			traverseDir(newDir, depth + 1);

			// Using utils to check if a node is a file
		} else if (telemetry::utils::isFile(node)) {
			auto file = std::dynamic_pointer_cast<telemetry::File>(node);

			// Manually reading file content
			try {
				std::cout << telemetry::contentToString(file->read());
			} catch (std::exception& ex) {
				// File::read operation not supported
				// std::cout << ex.what() << "\n";
			}
		}
	}
}

int main()
{
	// Same as basic example
	std::shared_ptr<telemetry::Directory> telemetryRootNode;
	telemetryRootNode = telemetry::Directory::create();

	std::string fusePath = "fusedir";

	telemetry::appFs::AppFsFuse fuse
		= telemetry::appFs::AppFsFuse(telemetryRootNode, fusePath, true, true);
	fuse.start();

	// Let's create a more complex dir structure
	const telemetry::FileOps emptyFileOps = {nullptr, nullptr};

	auto file1 = telemetryRootNode->addFile("file1", emptyFileOps);
	auto file2 = telemetryRootNode->addFile("file2", emptyFileOps);
	auto dir1 = telemetryRootNode->addDir("dir1");

	auto file3 = dir1->addFile("file3", emptyFileOps);
	auto dir2 = dir1->addDir("dir2");
	auto dir3 = dir1->addDir("dir3");

	auto file4 = dir2->addFile("file4", emptyFileOps);
	auto file5 = dir2->addFile("file5", emptyFileOps);
	auto file6 = dir2->addFile("file6", emptyFileOps);
	auto file7 = dir2->addFile("file7", emptyFileOps);

	auto file8 = dir3->addFile("file8", emptyFileOps);

	// And now traverse the directory with the use of telemetry utils
	traverseDir(telemetryRootNode, 1);

	// Utils can also give you a node from path
	auto node = telemetry::utils::getNodeFromPath(telemetryRootNode, "/dir1/dir2/file5");

	// To silence compiler warnings.
	(void) node;
	(void) file1;
	(void) file2;
	(void) file3;
	(void) file4;
	(void) file5;
	(void) file6;
	(void) file7;
	(void) file8;
}
