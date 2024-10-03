/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief aggregator common utilities.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <telemetry/aggMethod.hpp>
#include <telemetry/content.hpp>
#include <telemetry/node.hpp>

#include <algorithm>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace telemetry {

/**
 * @brief Checks if all variants in the vector are of the same alternative type.
 *
 * @tparam T The alternative type to check for.
 * @tparam Variant The variant type.
 * @param variants Vector of variants to check.
 * @return true if all variants are of the same alternative type, false otherwise.
 */
template <typename T, typename Variant>
static bool containsSameAlternative(const std::vector<Variant>& variants)
{
	return std::all_of(variants.begin(), variants.end(), [](const Variant& variant) {
		return std::holds_alternative<T>(variant);
	});
}

/**
 * @brief Checks if any of the given alternative types are present in the vector of variants.
 *
 * @tparam AllowedAlternatives The allowed alternative types to check for.
 * @tparam Variant The variant type.
 * @param variants Vector of variants to check.
 * @return true if any of the allowed alternative types are present, false otherwise.
 */
template <typename... AllowedAlternatives, typename Variant>
static bool hasOneOfThisAlternative(const std::vector<Variant>& variants)
{
	return (... || containsSameAlternative<AllowedAlternatives>(variants));
}

/**
 * @brief Gets the reference scalar with unit from the vector of AggContent variants.
 *
 * @param values Vector of AggContent variants.
 * @return The reference ScalarWithUnit variant.
 *
 * @throw TelemetryException if the variant alternative is invalid.
 */
static ScalarWithUnit getReferenceVariant(const std::vector<AggContent>& values)
{
	if (std::holds_alternative<ScalarWithUnit>(values.front())) {
		return std::get<ScalarWithUnit>(values.front());
	}
	if (std::holds_alternative<Scalar>(values.front())) {
		return {std::get<Scalar>(values.front()), ""};
	}

	if (std::holds_alternative<Array>(values.front())) {
		for (const auto& value : values) {
			const auto& array = std::get<Array>(value);
			if (!array.empty()) {
				return {array.front(), ""};
			}
		}
	}

	if (std::holds_alternative<std::monostate>(values.front())) {
		return {std::monostate {}, ""};
	}

	throw TelemetryException("Invalid variant alternative.");
}

/**
 * @brief Checks if all AggContent variants in the vector have the same scalar alternative type.
 *
 * @tparam T The scalar alternative type to check for.
 * @param values Vector of AggContent variants.
 * @return true if all variants have the same scalar alternative type, false otherwise.
 */
template <typename T>
static bool containsSameScalarAlternative(const std::vector<AggContent>& values)
{
	if (values.empty()) {
		return true;
	}

	const auto& [refScalar, refUnit] = getReferenceVariant(values);
	if (!std::holds_alternative<T>(refScalar)) {
		return false;
	}

	const size_t refIndex = refScalar.index();

	for (const auto& value : values) {
		if (std::holds_alternative<Scalar>(value)) {
			if (std::get<Scalar>(value).index() != refIndex) {
				return false;
			}
		} else if (std::holds_alternative<ScalarWithUnit>(value)) {
			const auto& [scalar, unit] = std::get<ScalarWithUnit>(value);
			if (scalar.index() != refIndex) {
				return false;
			}

			if (unit != refUnit) {
				return false;
			}
		} else if (std::holds_alternative<Array>(value)) {
			const auto& array = std::get<Array>(value);
			if (std::any_of(array.begin(), array.end(), [&](const auto& scalar) {
					return scalar.index() != refIndex;
				})) {
				return false;
			}
		}
	}

	return true;
}

/**
 * @brief Checks if all AggContent variants in the vector have one of the allowed scalar alternative
 * types.
 *
 * @tparam AllowedTypes The allowed scalar alternative types to check for.
 * @param variants Vector of AggContent variants.
 * @return true if all variants have one of the allowed scalar alternative types, false otherwise.
 */
template <typename... AllowedTypes>
static bool hasValidScalarType(const std::vector<AggContent>& variants)
{
	return (... || containsSameScalarAlternative<AllowedTypes>(variants));
}

} // namespace telemetry
