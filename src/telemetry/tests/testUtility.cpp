/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Unit tests of telemetry::utils functions
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test checking the parsing of a path
 */
TEST(TelemetryUtility, parsePath)
{
	std::vector<std::string> path;

	path = utils::parsePath("/dir/file");
	EXPECT_EQ(path.size(), 2);
	EXPECT_EQ(path[0], "dir");
	EXPECT_EQ(path[1], "file");

	path = utils::parsePath("dir/////file");
	EXPECT_EQ(path.size(), 2);
	EXPECT_EQ(path[0], "dir");
	EXPECT_EQ(path[1], "file");

	path = utils::parsePath("dir/");
	EXPECT_EQ(path.size(), 1);
	EXPECT_EQ(path[0], "dir");

	path = utils::parsePath("dir/subDir/subDir/subDir/file");
	EXPECT_EQ(path.size(), 5);
	EXPECT_EQ(path[0], "dir");
	EXPECT_EQ(path[1], "subDir");
	EXPECT_EQ(path[2], "subDir");
	EXPECT_EQ(path[3], "subDir");
	EXPECT_EQ(path[4], "file");

	path = utils::parsePath("");
	EXPECT_EQ(path.size(), 0);
}

/**
 * @test Test checking the getting of a node from a path
 */
TEST(TelemetryUtility, getNodeFromPath)
{
	auto root = Directory::create();
	auto dir = root->addDir("dir");
	auto file = dir->addFile("file", {});

	std::shared_ptr<Node> node;

	node = utils::getNodeFromPath(root, "/dir/file");
	EXPECT_TRUE(node != nullptr);
	EXPECT_EQ(node, file);

	node = utils::getNodeFromPath(root, "dir/file");
	EXPECT_TRUE(node != nullptr);
	EXPECT_EQ(node, file);

	node = utils::getNodeFromPath(root, "dir");
	EXPECT_TRUE(node != nullptr);
	EXPECT_EQ(node, dir);

	node = utils::getNodeFromPath(root, "dir/");
	EXPECT_TRUE(node != nullptr);
	EXPECT_EQ(node, dir);

	node = utils::getNodeFromPath(root, "/");
	EXPECT_TRUE(node != nullptr);
	EXPECT_EQ(node, root);

	node = utils::getNodeFromPath(root, "");
	EXPECT_TRUE(node == nullptr);

	node = utils::getNodeFromPath(root, "nonexistent");
	EXPECT_TRUE(node == nullptr);
}

/**
 * @test Test checking if the node is a file
 */
TEST(TelemetryUtility, isFile)
{
	auto root = Directory::create();
	auto dir = root->addDir("dir");
	auto file = root->addFile("file", {});

	EXPECT_FALSE(utils::isFile(dir));
	EXPECT_FALSE(utils::isFile(root));

	EXPECT_TRUE(utils::isFile(file));
}

/**
 * @test Test checking if the node is a directory
 */
TEST(TelemetryUtility, isDirectory)
{
	auto root = Directory::create();
	auto dir = root->addDir("dir");
	auto file = root->addFile("file", {});

	EXPECT_TRUE(utils::isDirectory(dir));
	EXPECT_TRUE(utils::isDirectory(root));

	EXPECT_FALSE(utils::isDirectory(file));
}

/**
 * @test Test checking if the directory is root
 */
TEST(TelemetryUtility, isRootDirectory)
{
	auto root = Directory::create();
	auto dir = root->addDir("dir");

	EXPECT_FALSE(utils::isRootDirectory(dir->getFullPath()));
	EXPECT_TRUE(utils::isRootDirectory(root->getFullPath()));
}

} // namespace telemetry
