/**
 * @file
 * @author Daniel Pelanek <xpeland00@vutbr.cz>
 * @brief Shows how the aggregated files are used and it's methods
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <appFs.hpp>
#include <telemetry.hpp>

// example help
#include <iostream>

telemetry::Content returnValue(int64_t value)
{
	return telemetry::Scalar(value);
}

telemetry::Content returnValueDict(int64_t value)
{
	telemetry::Dict dict;
	dict["1"] = telemetry::Scalar(value * 2);

	return dict;
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

	// We create two fileops that just return a constant value
	const telemetry::FileOps fileOps1 = {[]() { return returnValue(2); }, nullptr};

	const telemetry::FileOps fileOps2 = {[]() { return returnValue(5); }, nullptr};

	// It is better to keep the files in another directory and not
	// aggregate them straight from root dir.
	auto aggFileDir = telemetryRootNode->addDir("aggDir");
	auto file1 = aggFileDir->addFile("file1", fileOps1);
	auto file2 = aggFileDir->addFile("file2", fileOps2);

	// Here we init three different aggregation operations.
	// One of them is join meaning it will add values from all files into an array.
	// The two arguments after the method type are source dict field name and result dict field name
	// If they are left empty ("") the values are taken directly from file->read() and
	// directly returned as telemetry::Scalar, telemetry::Array, ...
	// The result here is 1 : [2, 5].
	const telemetry::AggOperation aggOp0 = {telemetry::AggMethodType::JOIN, "", "1"};

	// Average of read from files in aggfile if there are different types of Scalar(WithUnit)s the
	// return type is same as in cpp meaning avg of uint64_t, int64_t and double the result is
	// double.
	const telemetry::AggOperation aggOp1 = {telemetry::AggMethodType::AVG};
	// Sum of read from files in aggfile
	const telemetry::AggOperation aggOp2 = {telemetry::AggMethodType::SUM};

	// We have to add these aggregation operations into a vector.
	std::vector<telemetry::AggOperation> aggOps0 = {aggOp0};
	std::vector<telemetry::AggOperation> aggOps1 = {aggOp1};
	std::vector<telemetry::AggOperation> aggOps2 = {aggOp2};

	// The method for creating an aggregated file is addAggFile
	// You can specify which of the files from a directory are picked
	// by regex.
	auto aggFile0 = aggFileDir->addAggFile("aggFile", "file[0-9]+", aggOps0);
	auto aggFile1 = aggFileDir->addAggFile("aggFile1", "file[0-9]+", aggOps1);
	auto aggFile2 = aggFileDir->addAggFile("aggFile2", "file[0-9]+", aggOps2);

	// Just for showing the results to terminal. The same results will be in the
	// actual filesystem.
	std::cout << "Single aggregation operations: \n";
	std::cout << telemetry::contentToString(aggFile0->read()) << "\n";
	std::cout << telemetry::contentToString(aggFile1->read()) << "\n";
	std::cout << telemetry::contentToString(aggFile2->read()) << "\n";

	// Using two aggregation operations in a single file is only possible when
	// the sources are dicts.
	const telemetry::FileOps fileOpsDict1 = {[]() { return returnValueDict(2); }, nullptr};

	const telemetry::FileOps fileOpsDict2 = {[]() { return returnValueDict(5); }, nullptr};

	auto aggFileDirDict = telemetryRootNode->addDir("aggDirDict");
	auto fileDict1 = aggFileDirDict->addFile("fileDict1", fileOpsDict1);
	auto fileDict2 = aggFileDirDict->addFile("fileDict2", fileOpsDict2);

	// The result dict keys have to be different. If they are the same only
	// the first result is valid and is read.
	const telemetry::AggOperation aggOpDict0 = {telemetry::AggMethodType::JOIN, "1", "1"};
	const telemetry::AggOperation aggOpDict1 = {telemetry::AggMethodType::AVG, "1", "2"};

	std::vector<telemetry::AggOperation> aggOpsDict = {aggOpDict0, aggOpDict1};

	auto aggFileDict = aggFileDirDict->addAggFile("aggFileDict", "fileDict[0-9]+", aggOpsDict);

	std::cout << "\nMultiple aggregation operations: \n";
	std::cout << telemetry::contentToString(aggFileDict->read()) << "\n";
}
