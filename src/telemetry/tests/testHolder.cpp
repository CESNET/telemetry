/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Unit tests of telemetry::Holder class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test checking availability of read operation.
 */
TEST(TelemetryHolder, create)
{
	Holder holder;
}

/**
 * @test Test adding various entries to the holder.
 */
TEST(TelemetryHolder, add)
{
	auto root = Directory::create();

	{
		Holder holder;

		{
			auto file = root->addFile("file", {});
			auto dir = root->addDir("dir");

			holder.add(file);
			holder.add(dir);
		}

		// Check if entries still exists
		EXPECT_NE(nullptr, root->getEntry("file"));
		EXPECT_NE(nullptr, root->getEntry("dir"));
	}

	// After holder destruction, entries should be gone...
	EXPECT_EQ(nullptr, root->getEntry("file"));
	EXPECT_EQ(nullptr, root->getEntry("dir"));
}

/**
 * @test Test disabling callbacks of held files.
 */
TEST(TelemetryHolder, disableFiles)
{
	auto root = Directory::create();
	FileOps ops = {};
	ops.read = []() { return Scalar {"value"}; };
	ops.clear = []() {};

	Holder holder;

	auto dir = root->addDir("dir");
	auto file = root->addFile("file", ops);
	holder.add(file);

	EXPECT_TRUE(file->hasRead());
	EXPECT_TRUE(file->hasClear());

	holder.disableFiles();

	EXPECT_FALSE(file->hasRead());
	EXPECT_FALSE(file->hasClear());
}

} // namespace telemetry
