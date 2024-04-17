/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief String utilities
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <string_view>
#include <vector>

namespace strUtils {

std::vector<std::string_view>
splitViewByDelimiter(std::string_view str, const std::string& delimiter)
{
	std::vector<std::string_view> result;
	size_t pos = 0;
	size_t found;

	while ((found = str.find(delimiter, pos)) != std::string_view::npos) {
		result.push_back(str.substr(pos, found - pos));
		pos = found + delimiter.size();
	}
	result.push_back(str.substr(pos));

	return result;
}

std::string_view trimView(std::string_view str)
{
	size_t start = 0;
	while (start < str.length() && std::isspace(str[start])) {
		start++;
	}

	size_t end = str.length();
	while (end > start && std::isspace(str[end - 1])) {
		end--;
	}

	return str.substr(start, end - start);
}

} // namespace strUtils
