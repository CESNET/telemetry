/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Unit tests of telemetry::File class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test checking availability of read operation.
 */
TEST(TelemetryFile, hasRead)
{
	auto root = Directory::create();

	auto noOpsFile = root->addFile("no", {});
	EXPECT_FALSE(noOpsFile->hasRead());

	FileOps ops {};
	ops.read = []() { return Scalar {}; };
	auto opsFile = root->addFile("yes", ops);
	EXPECT_TRUE(opsFile->hasRead());
}

/**
 * @test Test checking availability of clear operation.
 */
TEST(TelemetryFile, hasClear)
{
	auto root = Directory::create();

	auto noOpsFile = root->addFile("no", {});
	EXPECT_FALSE(noOpsFile->hasClear());

	FileOps ops {};
	ops.clear = []() {};
	auto opsFile = root->addFile("yes", ops);
	EXPECT_TRUE(opsFile->hasClear());
}

/**
 * @test Test executing read operation.
 */
TEST(TelemetryFile, read)
{
	auto root = Directory::create();

	auto noOpsFile = root->addFile("no", {});
	EXPECT_THROW(noOpsFile->read(), TelemetryException);

	FileOps ops {};
	ops.read = []() { return Scalar {"hello"}; };
	auto opsFile = root->addFile("yes", ops);
	EXPECT_EQ(Content {Scalar {"hello"}}, opsFile->read());
}

/**
 * @test Test executing read operation.
 */
TEST(TelemetryFile, clear)
{
	auto root = Directory::create();
	int valueToClear = 1;

	auto noOpsFile = root->addFile("no", {});
	EXPECT_THROW(noOpsFile->clear(), TelemetryException);

	FileOps ops {};
	ops.clear = [&]() { valueToClear = 0; };
	auto opsFile = root->addFile("yes", ops);
	opsFile->clear();
	EXPECT_EQ(0, valueToClear);
}

/**
 * @test Test that disabling of all operations works as expected.
 */
TEST(TelemetryFile, disable)
{
	int64_t counter = 0;
	FileOps ops {};
	ops.read = [&]() { return Scalar {counter++}; };
	ops.clear = [&]() { counter = 0; };

	auto root = Directory::create();
	auto file = root->addFile("file", ops);

	EXPECT_TRUE(file->hasRead());
	EXPECT_TRUE(file->hasClear());

	EXPECT_EQ(Content {Scalar {int64_t {0}}}, file->read());
	EXPECT_EQ(Content {Scalar {int64_t {1}}}, file->read());
	EXPECT_EQ(Content {Scalar {int64_t {2}}}, file->read());
	EXPECT_NO_THROW(file->clear());
	EXPECT_EQ(Content {Scalar {int64_t {0}}}, file->read());

	file->disable();

	EXPECT_FALSE(file->hasRead());
	EXPECT_FALSE(file->hasClear());
	EXPECT_THROW(file->read(), TelemetryException);
	EXPECT_THROW(file->clear(), TelemetryException);
}

} // namespace telemetry
