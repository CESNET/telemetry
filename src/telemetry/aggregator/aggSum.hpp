/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Interface of the SUM aggregation method for telemetry data.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <telemetry/aggMethod.hpp>
#include <telemetry/content.hpp>

#include <variant>
#include <vector>

namespace telemetry {

using ResultType = std::variant<Scalar, ScalarWithUnit>;

/**
 * @brief Implementation of the SUM aggregation method.
 */
class AggMethodSum : public AggMethod {
public:
	/**
	 * @brief Aggregate telemetry data using the SUM method.
	 *
	 * @param contents The vector of telemetry content to aggregate.
	 * @return The aggregated content.
	 * @throws TelemetryException if the aggregation encounters an error.
	 */
	Content aggregate(const std::vector<Content>& contents) override;

protected:
	Content createContent(const ResultType& result);
};

} // namespace telemetry
