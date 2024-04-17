/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Telemetry file aggregator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "aggregator/aggMethodFactory.hpp"

#include <telemetry/aggFile.hpp>
#include <telemetry/directory.hpp>
#include <telemetry/file.hpp>
#include <telemetry/utility.hpp>

#include <iomanip>
#include <regex>

namespace telemetry {

template <typename T>
static std::vector<std::shared_ptr<T>>
getMatchesInDirectory(const std::regex& regex, const std::shared_ptr<Directory>& directory)
{
	std::vector<std::shared_ptr<T>> matches;

	const auto& entries = directory->listEntries();
	for (const auto& entry : entries) {
		if (!std::regex_match(entry, regex)) {
			continue;
		}
		auto node = directory->getEntry(entry);
		if (const auto& derivedNode = std::dynamic_pointer_cast<T>(node)) {
			matches.push_back(derivedNode);
		}
	}

	return matches;
}

static std::vector<std::shared_ptr<File>>
getFilesMatchingPattern(const std::string& regexPath, std::shared_ptr<Directory> parentDir)
{
	std::vector<std::shared_ptr<File>> matchingFiles;

	auto pathSegments = utils::parsePath(regexPath);
	if (pathSegments.empty()) {
		return matchingFiles;
	}

	const std::string topLevelName = pathSegments.back();
	pathSegments.pop_back();

	std::vector<std::shared_ptr<Directory>> matchedDirs = {std::move(parentDir)};

	for (const auto& subDir : pathSegments) {
		std::vector<std::shared_ptr<Directory>> matchesInCurrentDir;
		const std::regex dirRegex(subDir);
		for (const auto& dir : matchedDirs) {
			const auto matchedSubDirs = getMatchesInDirectory<Directory>(dirRegex, dir);
			matchesInCurrentDir.insert(
				matchesInCurrentDir.end(),
				matchedSubDirs.begin(),
				matchedSubDirs.end());
		}
		matchedDirs = matchesInCurrentDir;
	}

	const std::regex fileRegex(topLevelName);
	for (const auto& dir : matchedDirs) {
		const auto filesInDir = getMatchesInDirectory<File>(fileRegex, dir);
		matchingFiles.insert(matchingFiles.end(), filesInDir.begin(), filesInDir.end());
	}

	return matchingFiles;
}

static void mergeContent(Content& content, const Content& newContent)
{
	if (std::holds_alternative<Dict>(content) && std::holds_alternative<Dict>(newContent)) {
		auto& dict = std::get<Dict>(content);
		const auto& newDict = std::get<Dict>(newContent);
		dict.insert(newDict.begin(), newDict.end());
		return;
	}
	content = newContent;
}

static void validateAggOperations(const std::vector<AggOperation>& ops)
{
	const bool hasDictFieldName
		= std::any_of(ops.begin(), ops.end(), [](const AggOperation& aggOp) {
			  return !aggOp.dictFieldName.empty();
		  });

	const bool hasNoDictFieldName
		= std::any_of(ops.begin(), ops.end(), [](const AggOperation& aggOp) {
			  return aggOp.dictFieldName.empty();
		  });

	if (hasDictFieldName && hasNoDictFieldName) {
		throw TelemetryException(
			"Inconsistent AggOperation configurations: Some operations have 'dictFieldName' "
			"specified while others don't.");
	}

	if (hasNoDictFieldName && ops.size() > 1) {
		throw TelemetryException(
			"Invalid AggOperation configuration: When 'dictFieldName' is specified, only one "
			"operation is allowed.");
	}
}

Content AggregatedFile::read()
{
	Content content;

	const auto files = getFilesMatchingPattern(
		m_filesRegexPattern,
		std::dynamic_pointer_cast<Directory>(m_parent));

	std::vector<Content> fileContents;
	fileContents.reserve(files.size());
	for (const auto& file : files) {
		if (!file->hasRead()) {
			throw TelemetryException(
				"File " + file->getName() + " does not support read operation.");
		}
		fileContents.emplace_back(file->read());
	}

	for (const auto& aggMethod : m_aggMethods) {
		const Content methodResult = aggMethod->aggregate(fileContents);
		mergeContent(content, methodResult);
	}

	return content;
}

FileOps AggregatedFile::getOps()
{
	FileOps ops = {};
	ops.read = [this]() { return read(); };
	return ops;
}

AggregatedFile::AggregatedFile(
	const std::shared_ptr<Node>& parent,
	std::string_view name,
	std::string aggFilesPattern,
	const std::vector<AggOperation>& ops)
	: File(parent, name, getOps())
	, m_filesRegexPattern(std::move(aggFilesPattern))
{
	validateAggOperations(ops);

	for (const auto& aggOp : ops) {
		m_aggMethods.push_back(AggMethodFactory::createAggMethod(
			aggOp.method,
			aggOp.dictFieldName,
			aggOp.dictResultName));
	}
}

} // namespace telemetry

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testAggFile.cpp"
#endif
