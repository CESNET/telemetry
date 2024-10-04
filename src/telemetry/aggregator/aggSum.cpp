/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Implementation of the SUM aggregation method for telemetry data.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "aggSum.hpp"

#include "aggCommon.hpp"

#include <telemetry/node.hpp>

namespace telemetry {

static void sumarize(const Scalar& value, Scalar& result)
{
	if (std::holds_alternative<std::monostate>(result)) {
		result = value;
		return;
	}

	if (std::holds_alternative<uint64_t>(value)) {
		std::get<uint64_t>(result) += std::get<uint64_t>(value);
	} else if (std::holds_alternative<int64_t>(value)) {
		std::get<int64_t>(result) += std::get<int64_t>(value);
	} else if (std::holds_alternative<double>(value)) {
		std::get<double>(result) += std::get<double>(value);
	} else {
		throw TelemetryException("Invalid scalar alternative type for sum operation.");
	}
}

static Scalar aggregateScalar(std::vector<AggContent>& values)
{
	Scalar result = std::monostate();

	if (values.empty()) {
		return result;
	}

	if (!std::holds_alternative<Scalar>(values.front())) {
		throw TelemetryException("Unexpected variant alternative.");
	}

	for (const auto& value : values) {
		const auto& scalar = std::get<Scalar>(value);
		sumarize(scalar, result);
	}

	return result;
}

static ScalarWithUnit aggregateScalarWithUnit(std::vector<AggContent>& values)
{
	Scalar result = std::monostate();

	if (values.empty()) {
		return {};
	}

	if (!std::holds_alternative<ScalarWithUnit>(values.front())) {
		throw TelemetryException("Unexpected variant alternative.");
	}

	for (const auto& value : values) {
		[[maybe_unused]] const auto& [scalar, _] = std::get<ScalarWithUnit>(value);
		sumarize(scalar, result);
	}

	[[maybe_unused]] const auto& [_, unit] = std::get<ScalarWithUnit>(values.front());

	return {result, unit};
}

static AggMethodSum::ResultType aggregateGatheredValues(std::vector<AggContent>& values)
{
	if (std::holds_alternative<Scalar>(values.front())) {
		return aggregateScalar(values);
	}

	if (std::holds_alternative<ScalarWithUnit>(values.front())) {
		return aggregateScalarWithUnit(values);
	}

	throw TelemetryException("Unexpected variant alternative.");
}

static Content createDictContent(const std::string& dictKey, const AggMethodSum::ResultType& result)
{
	Dict dict;

	auto visitor = [&](const auto& arg) -> DictValue { return arg; };
	dict[dictKey] = std::visit(visitor, result);

	return dict;
}

Content AggMethodSum::createContent(const AggMethodSum::ResultType& result)
{
	const auto dictResultName = getDictResultName();
	if (!dictResultName.empty()) {
		return createDictContent(dictResultName, result);
	}

	auto visitor = [&](const auto& arg) -> Content { return arg; };
	return std::visit(visitor, result);
}

Content AggMethodSum::aggregate(const std::vector<Content>& contents)
{
	std::vector<AggContent> values;

	for (const auto& content : contents) {
		const auto& aggContent = getAggContent(content);
		values.emplace_back(aggContent);
	}

	if (!hasOneOfThisAlternative<ScalarWithUnit, Scalar>(values)) {
		throw TelemetryException(
			"The contents data does not contain the same variant alternative.");
	}

	if (!hasValidScalarType<uint64_t, int64_t, double>(values)) {
		throw TelemetryException("Invalid scalar variant alternative.");
	}

	const auto& result = aggregateGatheredValues(values);
	return createContent(result);
}

} // namespace telemetry

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testAggSum.cpp"
#endif
