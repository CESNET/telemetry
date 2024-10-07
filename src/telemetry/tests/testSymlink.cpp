/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Unit tests of telemetry::File class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test checking symlink target
 */
TEST(TelemetrySymlink, hasTarget)
{
	auto root = Directory::create();
	auto dir = root->addDir("dir");
	auto file = dir->addFile("file", {});
	auto symlink = root->addSymlink("symlink", file);

	auto target = symlink->getTarget();

	EXPECT_EQ(file, target);
}

/**
 * @test Test checking symlink target lifetime
 */
TEST(TelemetrySymlink, targetScopeLifetime)
{
	auto root = Directory::create();
	auto dir = root->addDir("dir");

	std::shared_ptr<Symlink> symlink;

	{
		auto file = dir->addFile("file", {});
		symlink = root->addSymlink("symlink", file);

		auto target = symlink->getTarget();
		EXPECT_EQ(file, target);
	}

	auto target = symlink->getTarget();
	EXPECT_EQ(nullptr, target);
}

} // namespace telemetry
