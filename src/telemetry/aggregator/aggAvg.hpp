/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Interface of the AVG aggregation method for telemetry data.
 *
 * @note SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "aggSum.hpp"

#include <telemetry/content.hpp>

namespace telemetry {

/**
 * @brief Implementation of the AVG aggregation method.
 */
class AggMethodAvg : public AggMethodSum {
public:
	/**
	 * @brief Aggregate telemetry data using the AVG method.
	 *
	 * @param contents The vector of telemetry content to aggregate.
	 * @return The aggregated content.
	 * @throws TelemetryException if the aggregation encounters an error.
	 */
	Content aggregate(const std::vector<Content>& contents) override;
};

} // namespace telemetry
