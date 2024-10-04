/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Interface of the MIN and MAX aggregation method for telemetry data.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <telemetry/aggMethod.hpp>
#include <telemetry/content.hpp>

#include <functional>
#include <variant>
#include <vector>

namespace telemetry {

/**
 * @brief Implementation of the SUM aggregation method.
 */
class AggMethodMinMax : public AggMethod {
public:
	/**
	 * @brief Construct a new AggMethodMinMax object.
	 *
	 * @param method The aggregation method to use (MIN or MAX).
	 * @throws TelemetryException if the aggregation method is invalid.
	 */
	explicit AggMethodMinMax(const AggMethodType& method);

	/**
	 * @brief Aggregate telemetry data using the MIN and MAX method.
	 *
	 * @param contents The vector of telemetry content to aggregate.
	 * @return The aggregated content.
	 * @throws TelemetryException if the aggregation encounters an error.
	 */
	Content aggregate(const std::vector<Content>& contents) override;

	/**
	 * @brief Get the result type of the aggregation
	 */
	using AggMethod = std::function<void(const Scalar&, Scalar&)>;

private:
	AggMethod m_agregateFunction;
};

} // namespace telemetry
