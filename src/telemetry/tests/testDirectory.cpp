/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Unit tests of telemetry::Dictionary class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test creating root telemetry directory.
 */
TEST(TelemetryDirectory, create)
{
	auto root = Directory::create();
	EXPECT_EQ("", root->getName());
	EXPECT_EQ("/", root->getFullPath());
}

/**
 * @test Test creating telemetry directories.
 */
TEST(TelemetryDirectory, addDir)
{
	auto root = Directory::create();

	auto info = root->addDir("info");
	EXPECT_EQ("info", info->getName());
	EXPECT_EQ("/info", info->getFullPath());

	auto app = info->addDir("app");
	EXPECT_EQ("app", app->getName());
	EXPECT_EQ("/info/app", app->getFullPath());

	// Create entry that already exists (expect previously inserted entry)
	auto info2 = root->addDir("info");
	EXPECT_EQ(info, info2);
}

/**
 * @test Test creating invalid telemetry directories.
 */
TEST(TelemetryDirectory, addDirInvalid)
{
	auto root = Directory::create();

	// Create an entry without name
	EXPECT_THROW((void) root->addDir(""), TelemetryException);
	// Create an entry with invalid character
	EXPECT_THROW((void) root->addDir("superCool!"), TelemetryException);
	// Create a directory if there is already a file with the same name
	auto version = root->addFile("version", {});
	EXPECT_THROW((void) root->addDir("version"), TelemetryException);
}

/**
 * @test Test creating telemetry directories that ceased to exist.
 */
TEST(TelemetryDirectory, addDirRemoved)
{
	auto root = Directory::create();

	{
		auto app = root->addDir("app");
		EXPECT_EQ(app, root->getEntry("app"));
		EXPECT_EQ(nullptr, root->getEntry("port"));

		{
			auto port = root->addDir("port");
			EXPECT_EQ(port, root->getEntry("port"));
			EXPECT_EQ(app, root->getEntry("app"));
		}

		EXPECT_EQ(app, root->getEntry("app"));
		EXPECT_EQ(nullptr, root->getEntry("port"));

		auto port2 = root->addDir("port");
		EXPECT_EQ(port2, root->getEntry("port"));
	}

	EXPECT_EQ(nullptr, root->getEntry("port"));
	EXPECT_EQ(nullptr, root->getEntry("app"));

	auto app2 = root->addDir("app");
	auto port3 = root->addDir("port");

	EXPECT_EQ(app2, root->getEntry("app"));
	EXPECT_EQ(port3, root->getEntry("port"));
}

/**
 * @test Test creating telemetry files.
 */
TEST(TelemetryDirectory, addFile)
{
	auto root = Directory::create();

	auto pidFile = root->addFile("pid", {});
	EXPECT_EQ("pid", pidFile->getName());
	EXPECT_EQ("/pid", pidFile->getFullPath());

	auto cache = root->addDir("cache");
	auto cacheInfo = cache->addFile("info", {});
	EXPECT_EQ("info", cacheInfo->getName());
	EXPECT_EQ("/cache/info", cacheInfo->getFullPath());
}

/**
 * @test Test creating invalid telemetry files.
 */
TEST(TelemetryDirectory, addFileInvalid)
{
	auto root = Directory::create();

	// Create an entry without name
	EXPECT_THROW((void) root->addFile("", {}), TelemetryException);
	// Create an entry with invalid character
	EXPECT_THROW((void) root->addFile("superCool!", {}), TelemetryException);
	// Create an entry that already exists
	auto info = root->addFile("info", {});
	EXPECT_THROW((void) root->addFile("info", {}), TelemetryException);
	// Create a file if there is already a directory with the same name
	auto version = root->addDir("version");
	EXPECT_THROW((void) root->addFile("version", {}), TelemetryException);
}

/**
 * @test Test creating telemetry files that ceased to exist.
 */
TEST(TelemetryDirectory, addFileRemoved)
{
	auto root = Directory::create();

	{
		auto app = root->addFile("app", {});
		EXPECT_EQ(app, root->getEntry("app"));
		EXPECT_EQ(nullptr, root->getEntry("port"));

		{
			auto port = root->addFile("port", {});
			EXPECT_EQ(port, root->getEntry("port"));
			EXPECT_EQ(app, root->getEntry("app"));
		}

		EXPECT_EQ(app, root->getEntry("app"));
		EXPECT_EQ(nullptr, root->getEntry("port"));

		auto port2 = root->addFile("port", {});
		EXPECT_EQ(port2, root->getEntry("port"));
	}

	EXPECT_EQ(nullptr, root->getEntry("port"));
	EXPECT_EQ(nullptr, root->getEntry("app"));

	auto app2 = root->addFile("app", {});
	auto port3 = root->addFile("port", {});

	EXPECT_EQ(app2, root->getEntry("app"));
	EXPECT_EQ(port3, root->getEntry("port"));
}

/**
 * @test Test listing telemetry directory entries.
 */
TEST(TelemetryDirectory, listEntries)
{
	std::vector<std::string> entries;

	// "root" directory
	auto root = Directory::create();
	EXPECT_TRUE(root->listEntries().empty());

	[[maybe_unused]] auto info = root->addFile("info", {});
	entries = root->listEntries();
	ASSERT_EQ(1, entries.size());
	EXPECT_EQ("info", entries[0]);

	auto ports = root->addDir("ports");
	entries = root->listEntries();
	ASSERT_EQ(2, entries.size());
	EXPECT_EQ("info", entries[0]);
	EXPECT_EQ("ports", entries[1]);

	// "ports" subdirectory
	EXPECT_TRUE(ports->listEntries().empty());

	[[maybe_unused]] auto eth0 = ports->addDir("eth0");
	[[maybe_unused]] auto eth1 = ports->addDir("eth1");
	[[maybe_unused]] auto eth2 = ports->addDir("eth2");
	[[maybe_unused]] auto summary = ports->addFile("summary", {});
	entries = ports->listEntries();
	ASSERT_EQ(4, entries.size());
	EXPECT_EQ("eth0", entries[0]);
	EXPECT_EQ("eth1", entries[1]);
	EXPECT_EQ("eth2", entries[2]);
	EXPECT_EQ("summary", entries[3]);

	// Check that it didn't have impact on the root directory
	entries = root->listEntries();
	ASSERT_EQ(2, entries.size());
	EXPECT_EQ("info", entries[0]);
	EXPECT_EQ("ports", entries[1]);
}

/**
 * @test Test listing telemetry directory entries that doesn't exist anymore.
 */
TEST(TelemetryDirectory, listEntriesRemoved)
{
	std::vector<std::string> entries;
	auto root = Directory::create();

	entries = root->listEntries();
	EXPECT_TRUE(entries.empty());

	{
		[[maybe_unused]] auto app = root->addFile("app", {});
		entries = root->listEntries();
		ASSERT_EQ(1, entries.size());
		EXPECT_EQ("app", entries[0]);

		{
			[[maybe_unused]] auto ports = root->addDir("ports");
			entries = root->listEntries();
			ASSERT_EQ(2, entries.size());
			EXPECT_EQ("app", entries[0]);
			EXPECT_EQ("ports", entries[1]);

			{
				[[maybe_unused]] auto info = root->addFile("info", {});
				entries = root->listEntries();
				ASSERT_EQ(3, entries.size());
				EXPECT_EQ("app", entries[0]);
				EXPECT_EQ("info", entries[1]);
				EXPECT_EQ("ports", entries[2]);
			}

			entries = root->listEntries();
			ASSERT_EQ(2, entries.size());
			EXPECT_EQ("app", entries[0]);
			EXPECT_EQ("ports", entries[1]);
		}

		entries = root->listEntries();
		ASSERT_EQ(1, entries.size());
		EXPECT_EQ("app", entries[0]);
	}

	entries = root->listEntries();
	EXPECT_TRUE(entries.empty());
}

/**
 * @test Test getting telemetry directory entries.
 */
TEST(TelemetryDirectory, getEntry)
{
	auto root = Directory::create();
	EXPECT_EQ(nullptr, root->getEntry("info"));
	EXPECT_EQ(nullptr, root->getEntry("version"));

	auto info = root->addDir("info");
	EXPECT_EQ(info, root->getEntry("info"));

	auto version = root->addFile("version", {});
	EXPECT_EQ(version, root->getEntry("version"));
}

/**
 * @test Test getting telemetry entries that doesn't exists anymore.
 */
TEST(TelemetryDirectory, getEntryRemoved)
{
	auto root = Directory::create();

	{
		auto app = root->addFile("app", {});
		EXPECT_EQ(app, root->getEntry("app"));

		{
			auto ports = root->addDir("ports");
			EXPECT_EQ(ports, root->getEntry("ports"));
			EXPECT_EQ(app, root->getEntry("app"));

			{
				auto info = root->addFile("info", {});
				EXPECT_EQ(info, root->getEntry("info"));
				EXPECT_EQ(ports, root->getEntry("ports"));
				EXPECT_EQ(app, root->getEntry("app"));
			}

			EXPECT_EQ(nullptr, root->getEntry("info"));
			EXPECT_EQ(ports, root->getEntry("ports"));
			EXPECT_EQ(app, root->getEntry("app"));
		}

		EXPECT_EQ(nullptr, root->getEntry("info"));
		EXPECT_EQ(nullptr, root->getEntry("ports"));
		EXPECT_EQ(app, root->getEntry("app"));
	}

	EXPECT_EQ(nullptr, root->getEntry("info"));
	EXPECT_EQ(nullptr, root->getEntry("ports"));
	EXPECT_EQ(nullptr, root->getEntry("app"));
}

} // namespace telemetry
