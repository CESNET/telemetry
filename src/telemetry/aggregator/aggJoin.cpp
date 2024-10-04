/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Implemetaion of the JOIN aggregation method for telemetry data.
 *
 * @note SPDX-License-Identifier: BSD-3-Clause
 */

#include "aggJoin.hpp"

#include "aggCommon.hpp"

#include <telemetry/node.hpp>

namespace telemetry {

using ResultType = Array;

static ResultType aggregateGatheredValues(const std::vector<AggContent>& values)
{
	ResultType result;

	for (const auto& value : values) {
		if (std::holds_alternative<Scalar>(value)) {
			result.emplace_back(std::get<Scalar>(value));
		}

		if (std::holds_alternative<Array>(value)) {
			const auto& array = std::get<Array>(value);
			result.insert(result.end(), array.begin(), array.end());
		}
	}

	return result;
}

static Content createContent(const std::string& dictKey, const ResultType& result)
{
	if (!dictKey.empty()) {
		return Dict {{dictKey, result}};
	}

	return result;
}

Content AggMethodJoin::aggregate(const std::vector<Content>& contents)
{
	std::vector<AggContent> values;

	for (const auto& content : contents) {
		const auto& aggContent = getAggContent(content);
		values.emplace_back(aggContent);
	}

	if (!hasOneOfThisAlternative<Scalar, Array>(values)) {
		throw TelemetryException("The contents data does not contain the same variant alternative");
	}

	if (!hasValidScalarType<uint64_t, int64_t, double, std::string, bool, std::monostate>(values)) {
		throw TelemetryException("Invalid scalar variant alternative");
	}

	const auto& result = aggregateGatheredValues(values);
	return createContent(getDictResultName(), result);
}

} // namespace telemetry

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testAggJoin.cpp"
#endif
