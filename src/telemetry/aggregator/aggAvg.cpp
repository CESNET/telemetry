/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Implementation of the AVG aggregation method for telemetry data.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "aggAvg.hpp"

#include <stdexcept>
#include <telemetry/node.hpp>
#include <variant>

namespace telemetry {

static void makeAverage(Scalar& result, size_t count)
{
	const auto countAsDouble = static_cast<double>(count);

	if (std::holds_alternative<uint64_t>(result)) {
		result = static_cast<double>(std::get<uint64_t>(result)) / countAsDouble;
	} else if (std::holds_alternative<int64_t>(result)) {
		result = static_cast<double>(std::get<int64_t>(result)) / countAsDouble;
	} else if (std::holds_alternative<double>(result)) {
		result = std::get<double>(result) / countAsDouble;
	} else {
		throw TelemetryException("Invalid scalar alternative type for average operation.");
	}
}

static ResultType convertToAverage(AggContent& aggContent, size_t count)
{
	if (std::holds_alternative<Scalar>(aggContent)) {
		makeAverage(std::get<Scalar>(aggContent), count);
		return std::get<Scalar>(aggContent);
	}

	if (std::holds_alternative<ScalarWithUnit>(aggContent)) {
		auto& [scalar, _] = std::get<ScalarWithUnit>(aggContent);
		makeAverage(scalar, count);
		return std::get<ScalarWithUnit>(aggContent);
	}

	throw TelemetryException("Unexpected variant alternative.");
}

Content AggMethodAvg::aggregate(const std::vector<Content>& contents)
{
	const Content aggregatedSum = AggMethodSum::aggregate(contents);
	const bool useDictResultNameAsKey = true;
	AggContent aggContent = getAggContent(aggregatedSum, useDictResultNameAsKey);
	const ResultType result = convertToAverage(aggContent, contents.size());
	return createContent(result);
}

} // namespace telemetry

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testAggAvg.cpp"
#endif
