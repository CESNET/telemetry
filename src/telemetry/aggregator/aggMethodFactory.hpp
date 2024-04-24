/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Factory for creating aggregation methods for telemetry data.
 * @note SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <telemetry/aggMethod.hpp>

#include <memory>
#include <string>

namespace telemetry {

/**
 * @brief Factory class for creating aggregation methods.
 */
class AggMethodFactory {
public:
	/**
	 * @brief Create an aggregation method based on the specified type.
	 *
	 * @param aggMethodType The type of aggregation method to create.
	 * @param dictFieldName The name of the dictionary field.
	 * @param dictResultName The name of the dictionary result.
	 * @return A unique pointer to the created aggregation method.
	 * @throws TelemetryException if the specified aggregation method type is invalid.
	 */
	static std::unique_ptr<AggMethod> createAggMethod(
		const AggMethodType& aggMethodType,
		const std::string& dictFieldName = "",
		const std::string& dictResultName = "");
};

} // namespace telemetry
