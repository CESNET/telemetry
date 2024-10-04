/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Aggregation method interface
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "content.hpp"

#include <string>
#include <vector>

namespace telemetry {

/**
 * @brief Supported aggregation methods.
 *
 * Supported methods and types:
 * - @p AVG: Scalar(WithUnit) value, [uint64_t, int64_t, double] -> result double
 * - @p SUM: Scalar(WithUnit) value, [uint64_t, int64_t, double]
 * - @p MIN: Scalar(WithUnit) value, [uint64_t, int64_t, double]
 * - @p MAX: Scalar(WithUnit) value, [uint64_t, int64_t, double]
 * - @p JOIN: Scalar value (array included), [bool, uint64_t, int64_t, double, string,
 * std::monostate()]
 */
enum class AggMethodType { AVG, SUM, MIN, MAX, JOIN };

/**
 * @brief Structure representing an aggregation operation
 *
 * An AggOperation structure represents an aggregation operation that specifies the method
 * to use for aggregation and the field name in the dictionary to aggregate.
 *
 * @note Set the empty string for @p dictFieldName if you dont want to use value from Dict.
 */
struct AggOperation {
	AggMethodType method; ///< Aggregation method
	// NOLINTNEXTLINE(readability-redundant-string-init)
	std::string dictFieldName = ""; ///< Name of the field in the dictionary
	// NOLINTNEXTLINE(readability-redundant-string-init)
	std::string dictResultName = ""; ///< Name of the field in the aggregated dictionary
};

/** @brief Value used as a aggregationFile content. */
using AggContent = DictValue;

/**
 * @brief Interface for aggregation methods
 *
 * The AggMethod class represents an interface for aggregation methods. It defines a virtual
 * function `aggregate()` that must be implemented by derived classes to perform the aggregation.
 */
class AggMethod {
public:
	/**
	 * @brief Perform aggregation
	 *
	 * This function performs the aggregation of content using the specified aggregation method.
	 *
	 * @param contents Vector of content to aggregate
	 *
	 * @throw TelemetryException if the aggregation fails
	 * @return Aggregated content
	 */
	virtual Content aggregate(const std::vector<Content>& contents) = 0;

	/**
	 * @brief Set dictionary field names
	 *
	 * This function sets the field names in the dictionary to be used for aggregation.
	 *
	 * @param dictFieldName   Name of the field in the dictionary
	 * @param dictResultName  Name of the field in the aggregated dictionary
	 */
	void setDictField(const std::string& dictFieldName, const std::string& dictResultName);

	virtual ~AggMethod() = default;

protected:
	AggContent getAggContent(const Content& content, bool useDictResultName = false);

	[[nodiscard]] std::string getDictResultName() const { return m_dictResultname; }

private:
	std::string m_dictFieldName;
	std::string m_dictResultname;
};

} // namespace telemetry
