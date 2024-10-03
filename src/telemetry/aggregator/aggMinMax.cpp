/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Implementaion of the MIN aggregation method for telemetry data.
 *
 * @note SPDX-License-Identifier: BSD-3-Clause
 */

#include "aggMinMax.hpp"

#include "aggCommon.hpp"

#include <telemetry/node.hpp>

namespace telemetry {

using ResultType = std::variant<Scalar, ScalarWithUnit>;

static void findMin(const Scalar& value, Scalar& result)
{
	if (std::holds_alternative<std::monostate>(result)) {
		result = value;
		return;
	}

	if (std::holds_alternative<uint64_t>(value)) {
		if (std::get<uint64_t>(value) < std::get<uint64_t>(result)) {
			result = value;
		}
	} else if (std::holds_alternative<int64_t>(value)) {
		if (std::get<int64_t>(value) < std::get<int64_t>(result)) {
			result = value;
		}
	} else if (std::holds_alternative<double>(value)) {
		if (std::get<double>(value) < std::get<double>(result)) {
			result = value;
		}
	} else {
		throw TelemetryException("Invalid scalar alternative type for min operation.");
	}
}

static void findMax(const Scalar& value, Scalar& result)
{
	if (std::holds_alternative<std::monostate>(result)) {
		result = value;
		return;
	}

	if (std::holds_alternative<uint64_t>(value)) {
		if (std::get<uint64_t>(value) > std::get<uint64_t>(result)) {
			result = value;
		}
	} else if (std::holds_alternative<int64_t>(value)) {
		if (std::get<int64_t>(value) > std::get<int64_t>(result)) {
			result = value;
		}
	} else if (std::holds_alternative<double>(value)) {
		if (std::get<double>(value) > std::get<double>(result)) {
			result = value;
		}
	} else {
		throw TelemetryException("Invalid scalar alternative type for max operation.");
	}
}

static Scalar
aggregateScalar(std::vector<AggContent>& values, const AggMethodMinMax::AggMethod& aggMethod)
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
		aggMethod(scalar, result);
	}

	return result;
}

static ScalarWithUnit aggregateScalarWithUnit(
	std::vector<AggContent>& values,
	const AggMethodMinMax::AggMethod& aggMethod)
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
		aggMethod(scalar, result);
	}

	[[maybe_unused]] const auto& [_, unit] = std::get<ScalarWithUnit>(values.front());

	return {result, unit};
}

static ResultType aggregateGatheredValues(
	std::vector<AggContent>& values,
	const AggMethodMinMax::AggMethod& aggMethod)
{
	if (std::holds_alternative<Scalar>(values.front())) {
		return aggregateScalar(values, aggMethod);
	}

	if (std::holds_alternative<ScalarWithUnit>(values.front())) {
		return aggregateScalarWithUnit(values, aggMethod);
	}

	throw TelemetryException("Unexpected variant alternative.");
}

static Content createDictContent(const std::string& dictKey, const ResultType& result)
{
	Dict dict;

	auto visitor = [&](const auto& arg) -> DictValue { return arg; };
	dict[dictKey] = std::visit(visitor, result);

	return dict;
}

static Content createContent(const std::string& dictKey, const ResultType& result)
{
	if (!dictKey.empty()) {
		return createDictContent(dictKey, result);
	}

	auto visitor = [&](const auto& arg) -> Content { return arg; };
	return std::visit(visitor, result);
}

AggMethodMinMax::AggMethodMinMax(const AggMethodType& method)
{
	if (method == AggMethodType::MIN) {
		m_agregateFunction = findMin;
	} else if (method == AggMethodType::MAX) {
		m_agregateFunction = findMax;
	} else {
		throw TelemetryException("Invalid aggregation method.");
	}
}

Content AggMethodMinMax::aggregate(const std::vector<Content>& contents)
{
	std::vector<AggContent> values;

	for (const auto& content : contents) {
		const auto& aggContent = getAggContent(content);
		values.emplace_back(aggContent);
	}

	if (!hasOneOfThisAlternative<ScalarWithUnit, Scalar>(values)) {
		throw TelemetryException("The contents data does not contain the same variant alternative");
	}

	if (!hasValidScalarType<uint64_t, int64_t, double, std::monostate>(values)) {
		throw TelemetryException("Invalid scalar variant alternative");
	}

	const auto& result = aggregateGatheredValues(values, m_agregateFunction);
	return createContent(getDictResultName(), result);
}

} // namespace telemetry

#ifdef TELEMETRY_ENABLE_TESTS
#include "tests/testAggMinMax.cpp"
#endif
