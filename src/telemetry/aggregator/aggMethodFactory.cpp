/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Implementation of factory for creating aggregation methods for telemetry data.
 * @note SPDX-License-Identifier: BSD-3-Clause
 */
#include "aggMethodFactory.hpp"

#include "aggAvg.hpp"
#include "aggJoin.hpp"
#include "aggSum.hpp"

#include <telemetry/node.hpp>

namespace telemetry {

std::unique_ptr<AggMethod> AggMethodFactory::createAggMethod(
	const AggMethodType& aggMethodType,
	const std::string& dictFieldName,
	const std::string& dictResultName)
{
	std::unique_ptr<AggMethod> aggMethod;

	if (aggMethodType == AggMethodType::SUM) {
		aggMethod = std::make_unique<AggMethodSum>();
	} else if (aggMethodType == AggMethodType::AVG) {
		aggMethod = std::make_unique<AggMethodAvg>();
	} else if (aggMethodType == AggMethodType::JOIN) {
		aggMethod = std::make_unique<AggMethodJoin>();
	} else {
		throw TelemetryException("Invalid aggregation method.");
	}

	aggMethod->setDictField(dictFieldName, dictResultName);
	return aggMethod;
}

} // namespace telemetry
