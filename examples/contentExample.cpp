/**
 * @file
 * @author Daniel Pelanek <xpeland00@vutbr.cz>
 * @brief Shows how telemetry content data types are used
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry.hpp>

// example help
#include <iostream>

// Simplest type. Only contains one value .
// The values can be of type std::monostate, bool, uint64_t, int64_t, double, std::string.
telemetry::Scalar scalarReturn()
{
	return telemetry::Scalar(static_cast<uint64_t>(100));
}

// Contains not only scalar but also has a unit.
// The unit is of type string.
telemetry::ScalarWithUnit scalartWithUnitReturn()
{
	return telemetry::ScalarWithUnit(static_cast<double>(42), "%");
}

// Array is a vector of Scalars.
telemetry::Array arrayReturn()
{
	telemetry::Array arr;

	const uint64_t nScalars = 10;

	for (uint64_t index = 0; index < nScalars; index++) {
		arr.push_back(telemetry::Scalar(index));
	}

	return arr;
}

// Dict is a map with string as a key.
// Value can be std::monostate or any one of the three previous types
telemetry::Dict dictReturn()
{
	telemetry::Dict dict;

	dict["1"] = telemetry::Scalar(static_cast<int64_t>(10));
	dict["2"] = telemetry::ScalarWithUnit(static_cast<bool>(10), "bool");
	dict["3"] = telemetry::Array(std::vector<telemetry::Scalar>());

	return dict;
}

int main()
{
	// telemetry::Content can be easily printed or stored as string with contentToString()
	std::cout << telemetry::contentToString(scalarReturn()) << "\n";
	std::cout << telemetry::contentToString(scalartWithUnitReturn()) << "\n";
	std::cout << telemetry::contentToString(arrayReturn()) << "\n";
	std::cout << telemetry::contentToString(dictReturn()) << "\n";
}
