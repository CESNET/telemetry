/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Unit tests of telemetry::AggFile class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test checking matching of files/directories by given regex.
 */
TEST(TelemetryAggFile, getMatchesInDirectory)
{
	auto root = Directory::create();

	auto dir1 = root->addDir("dir1");
	auto dir2 = root->addDir("dir2");
	auto dir3 = root->addDir("dir3");

	auto file1 = dir1->addFile("file1", {});
	auto file2 = dir1->addFile("file2", {});
	auto file3 = dir1->addFile("file3", {});

	// match all files in dir1
	std::regex matchAllFilesRegex("file.*");
	auto matchesAllFiles = getMatchesInDirectory<File>(matchAllFilesRegex, dir1);
	EXPECT_EQ(3, matchesAllFiles.size());
	for (const auto& match : matchesAllFiles) {
		const std::string matchName = match->getName();
		EXPECT_TRUE(
			matchName == file1->getName() || matchName == file2->getName()
			|| matchName == file3->getName());
	}

	// match only file2 in dir1
	std::regex matchExactOneFileRegex("^file2$");
	auto matchExactFile = getMatchesInDirectory<File>(matchExactOneFileRegex, dir1);
	EXPECT_EQ(1, matchExactFile.size());
	for (const auto& match : matchExactFile) {
		const std::string matchName = match->getName();
		EXPECT_TRUE(matchName == file2->getName());
	}

	// do not match any files in dir1
	std::regex matchNothingFileRegex("File.*");
	auto matchNothingFile = getMatchesInDirectory<File>(matchNothingFileRegex, dir1);
	EXPECT_EQ(0, matchNothingFile.size());

	// match all dirs in root directory
	std::regex matchAllDirsRegex(R"(dir\d+)");
	auto matchesAllDirs = getMatchesInDirectory<Directory>(matchAllDirsRegex, root);
	EXPECT_EQ(3, matchesAllDirs.size());
	for (const auto& match : matchesAllDirs) {
		const std::string matchName = match->getName();
		EXPECT_TRUE(
			matchName == dir1->getName() || matchName == dir2->getName()
			|| matchName == dir3->getName());
	}

	// match only dir2 in root directory
	std::regex matchExactOneDirRegex("^dir2$");
	auto matchExactDir = getMatchesInDirectory<Directory>(matchExactOneDirRegex, root);
	EXPECT_EQ(1, matchExactDir.size());
	for (const auto& match : matchExactDir) {
		const std::string matchName = match->getName();
		EXPECT_TRUE(matchName == dir2->getName());
	}

	// do not match any dirs in dir1 directory
	auto matchNothingDir = getMatchesInDirectory<Directory>(matchExactOneDirRegex, dir1);
	EXPECT_EQ(0, matchNothingDir.size());
}

/**
 * @test Test checking matching of files/directories by given regex.
 */
TEST(TelemetryAggFile, getFilesMatchingPattern)
{
	auto root = Directory::create();

	auto dir1 = root->addDir("dir1");
	auto dir2 = root->addDir("dir2");
	auto dir3 = root->addDir("dir3");

	auto file1 = dir1->addFile("file1", {});
	auto file2 = dir2->addFile("file2", {});
	auto file3 = dir3->addFile("file3", {});

	// match all files in dir1
	const std::string matchAllFilesPattern(R"(dir\d+/file\d+)");
	auto matchesAllFiles = getFilesMatchingPattern(matchAllFilesPattern, root);
	EXPECT_EQ(3, matchesAllFiles.size());
	for (const auto& match : matchesAllFiles) {
		const std::string matchName = match->getName();
		EXPECT_TRUE(
			matchName == file1->getName() || matchName == file2->getName()
			|| matchName == file3->getName());
	}

	// match only file2 in dir1
	const std::string matchExactOneFilePattern(R"(dir\d+/^file2$)");
	auto matchExactFile = getFilesMatchingPattern(matchExactOneFilePattern, root);
	EXPECT_EQ(1, matchExactFile.size());
	for (const auto& match : matchExactFile) {
		const std::string matchName = match->getName();
		EXPECT_TRUE(matchName == file2->getName());
	}

	// do not match any files in all dirs
	const std::string matchNothingFilePattern(R"(.*/File.*)");
	auto matchNothingFile = getFilesMatchingPattern(matchNothingFilePattern, root);
	EXPECT_EQ(0, matchNothingFile.size());
}

TEST(TelemetryAggFile, mergeContent)
{
	const std::string key2Value = "value";
	Dict dict1 {{"key2", Scalar {key2Value}}, {"key1", Scalar {uint64_t(1)}}};
	Dict dict2 {
		{"key3", Scalar {int64_t(-1)}},
		{"key4", Scalar {true}},
		{"key5", Array {10.5, 11.5}}};

	Content result = {};
	mergeContent(result, dict1);

	EXPECT_TRUE(std::holds_alternative<Dict>(result));

	Dict& resultDict = std::get<Dict>(result);
	EXPECT_EQ(2, resultDict.size());

	auto iter = resultDict.cbegin();

	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("key1", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<uint64_t>(scalar));
		EXPECT_EQ(uint64_t(1), std::get<uint64_t>(scalar));
	}

	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("key2", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<std::string>(scalar));
		EXPECT_TRUE(key2Value == std::get<std::string>(scalar));
	}

	mergeContent(result, dict2);
	EXPECT_TRUE(std::holds_alternative<Dict>(result));

	resultDict = std::get<Dict>(result);
	EXPECT_EQ(5, resultDict.size());

	iter = resultDict.cbegin();

	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("key1", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<uint64_t>(scalar));
		EXPECT_EQ(uint64_t(1), std::get<uint64_t>(scalar));
	}

	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("key2", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<std::string>(scalar));
		EXPECT_TRUE(key2Value == std::get<std::string>(scalar));
	}

	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("key3", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<int64_t>(scalar));
		EXPECT_EQ(int64_t(-1), std::get<int64_t>(scalar));
	}

	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("key4", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<bool>(scalar));
		EXPECT_TRUE(true == std::get<bool>(scalar));
	}

	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("key5", key);
		EXPECT_TRUE(std::holds_alternative<Array>(value));
		const auto& array = std::get<Array>(value);
		EXPECT_EQ(2, array.size());
		EXPECT_TRUE(std::holds_alternative<double>(array[0]));
		EXPECT_EQ(10.5, std::get<double>(array[0]));
		EXPECT_TRUE(std::holds_alternative<double>(array[1]));
		EXPECT_EQ(11.5, std::get<double>(array[1]));
	}

	{
		Scalar scalar {uint64_t(1)};
		Content scalarContent = {};
		mergeContent(scalarContent, scalar);

		EXPECT_TRUE(std::holds_alternative<Scalar>(scalarContent));
		EXPECT_EQ(scalar, std::get<Scalar>(scalarContent));
	}
}

TEST(TelemetryAggFile, validateAggOperations)
{
	validateAggOperations({});

	AggOperation op1Dict {AggMethodType::SUM, "packets", "sumPackets"};
	AggOperation op2Dict {AggMethodType::AVG, "packets"};

	validateAggOperations({op1Dict, op2Dict});

	AggOperation op1Scalar {AggMethodType::SUM};

	EXPECT_THROW(validateAggOperations({op1Dict, op1Scalar}), TelemetryException);
	EXPECT_THROW(validateAggOperations({op1Scalar, op1Scalar}), TelemetryException);

	validateAggOperations({op1Scalar});
}

TEST(TelemetryAggFile, read)
{
	auto root = Directory::create();

	auto dir1 = root->addDir("dir1");
	auto dir2 = root->addDir("dir2");
	auto dir3 = root->addDir("dir3");

	FileOps ops1;
	ops1.read = []() { return Dict({{"packets", Scalar {uint64_t(1)}}}); };
	FileOps ops2;
	ops2.read = []() { return Dict({{"packets", Scalar {uint64_t(4)}}}); };
	FileOps ops3;
	ops3.read = []() { return Dict({{"packets", Scalar {uint64_t(10)}}}); };

	auto file1 = dir1->addFile("file1", ops1);
	auto file2 = dir1->addFile("file2", ops2);
	auto file3 = dir1->addFile("file3", ops3);

	AggOperation aggOp1 {AggMethodType::SUM, "packets", "sumPackets"};
	AggOperation aggOp2 {AggMethodType::AVG, "packets", "avgPackets"};
	AggOperation aggOp3 {AggMethodType::JOIN, "packets", "joinPackets"};

	auto aggFile = root->addAggFile("aggFile", R"(dir\d+/file\d+)", {aggOp1, aggOp2, aggOp3});
	const auto content = aggFile->read();

	EXPECT_TRUE(std::holds_alternative<Dict>(content));

	const Dict& dict = std::get<Dict>(content);
	EXPECT_EQ(3, dict.size());

	const Scalar& scalarValueSum = std::get<Scalar>(dict.at("sumPackets"));
	EXPECT_EQ(uint64_t(15), std::get<uint64_t>(scalarValueSum));

	const Scalar& scalarValueAvg = std::get<Scalar>(dict.at("avgPackets"));
	EXPECT_EQ(5.00, std::get<double>(scalarValueAvg));

	const Array& ArrayValueJoin = std::get<Array>(dict.at("joinPackets"));
	EXPECT_EQ(3, ArrayValueJoin.size());
	EXPECT_EQ(uint64_t(1), std::get<uint64_t>(ArrayValueJoin[0]));
	EXPECT_EQ(uint64_t(4), std::get<uint64_t>(ArrayValueJoin[1]));
	EXPECT_EQ(uint64_t(10), std::get<uint64_t>(ArrayValueJoin[2]));
}

TEST(TelemetryAggFile, readPatternDir)
{
	auto root = Directory::create();

	auto dir = root->addDir("dir");
	auto data0 = root->addDirs("dir/data_0/");
	auto data1 = root->addDirs("dir/data_1/");
	auto data2 = root->addDirs("dir/data_2/");

	FileOps ops1;
	ops1.read = []() { return Dict({{"packets", Scalar {uint64_t(1)}}}); };
	FileOps ops2;
	ops2.read = []() { return Dict({{"packets", Scalar {uint64_t(4)}}}); };
	FileOps ops3;
	ops3.read = []() { return Dict({{"packets", Scalar {uint64_t(10)}}}); };

	auto file1 = data0->addFile("file1", ops1);
	auto file2 = data1->addFile("file2", ops2);
	auto file3 = data2->addFile("file3", ops3);

	AggOperation aggOp1 {AggMethodType::SUM, "packets", "sumPackets"};
	AggOperation aggOp2 {AggMethodType::AVG, "packets", "avgPackets"};
	AggOperation aggOp3 {AggMethodType::JOIN, "packets", "joinPackets"};

	auto aggFile
		= root->addAggFile("aggFile", R"(data_\d+/file\d+)", {aggOp1, aggOp2, aggOp3}, dir);
	const auto content = aggFile->read();

	EXPECT_TRUE(std::holds_alternative<Dict>(content));

	const Dict& dict = std::get<Dict>(content);
	EXPECT_EQ(3, dict.size());

	const Scalar& scalarValueSum = std::get<Scalar>(dict.at("sumPackets"));
	EXPECT_EQ(uint64_t(15), std::get<uint64_t>(scalarValueSum));

	const Scalar& scalarValueAvg = std::get<Scalar>(dict.at("avgPackets"));
	EXPECT_EQ(5.00, std::get<double>(scalarValueAvg));

	const Array& ArrayValueJoin = std::get<Array>(dict.at("joinPackets"));
	EXPECT_EQ(3, ArrayValueJoin.size());
	EXPECT_EQ(uint64_t(1), std::get<uint64_t>(ArrayValueJoin[0]));
	EXPECT_EQ(uint64_t(4), std::get<uint64_t>(ArrayValueJoin[1]));
	EXPECT_EQ(uint64_t(10), std::get<uint64_t>(ArrayValueJoin[2]));
}

TEST(TelemetryAggFile, readNoMatchingPattern)
{
	auto root = Directory::create();

	auto data0 = root->addDirs("dir/data_0/");

	FileOps ops1;
	ops1.read = []() { return Dict({{"packets", Scalar {uint64_t(1)}}}); };

	auto file1 = data0->addFile("file1", ops1);

	AggOperation aggOp1 {AggMethodType::SUM, "packets", "sumPackets"};

	auto aggFile = root->addAggFile("aggFile", R"(data_\d+/file\d+)", {aggOp1});
	const auto content = aggFile->read();

	EXPECT_TRUE(std::holds_alternative<Scalar>(content));
}

} // namespace telemetry
