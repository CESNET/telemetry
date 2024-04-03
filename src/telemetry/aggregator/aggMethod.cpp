/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Aggregation method interface
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/aggMethod.hpp>
#include <telemetry/node.hpp>

#include <stdexcept>

namespace telemetry {

static DictValue getDictValue(const Dict& dict, const std::string& dictKeyName)
{
	auto iter = dict.find(dictKeyName);
	if (iter == dict.end()) {
		throw TelemetryException("Dict does not contain the specified key { " + dictKeyName + "}.");
	}

	return iter->second;
}

void AggMethod::setDictField(const std::string& dictFieldName, const std::string& dictResultName)
{
	m_dictFieldName = dictFieldName;
	if (!dictResultName.empty()) {
		m_dictResultname = dictResultName;
	} else {
		m_dictResultname = dictFieldName;
	}
}

AggContent AggMethod::getAggContent(const Content& content, bool useDictResultName)
{
	auto visitor = [&](const auto& arg) -> AggContent {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, Dict>) {
			const std::string key = useDictResultName ? m_dictResultname : m_dictFieldName;
			return getDictValue(std::get<Dict>(content), key);
		} else {
			if (!m_dictFieldName.empty()) {
				throw TelemetryException(
					"Dict key is set, but variant does not contains Dictionary.");
			}
			return arg;
		}
	};

	return std::visit(visitor, content);
}

} // namespace telemetry
