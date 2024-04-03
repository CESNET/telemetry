/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Interface of the JOIN aggregation method for telemetry data.
 *
 * @note SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <telemetry/aggMethod.hpp>
#include <telemetry/content.hpp>
#include <vector>

namespace telemetry {

/**
 * @brief Implementation of the JOIN aggregation method.
 */
class AggMethodJoin : public AggMethod {
public:
	/**
	 * @brief Aggregate telemetry data using the JOIN method.
	 * @param contents The vector of telemetry content to aggregate.
	 * @return The aggregated content.
	 *
	 * @throws TelemetryException if the aggregation encounters an error.
	 */
	Content aggregate(const std::vector<Content>& contents) override;
};

} // namespace telemetry
