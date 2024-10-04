/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Unit tests of Telemetry::AggMethodMinMax
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test find min of scalar values
 */
TEST(AggMinMaxTest, TestFindMin)
{
	// Test find min of uint64_t scalar
	{
		Scalar result = uint64_t(10);
		findMin(uint64_t(1), result);
		EXPECT_EQ(uint64_t(1), std::get<uint64_t>(result));
	}

	// Test find min of int64_t scalar
	{
		Scalar result = int64_t(10);
		findMin(int64_t(5), result);
		EXPECT_EQ(int64_t(5), std::get<int64_t>(result));
	}

	// Test find min of double scalar
	{
		Scalar result = double(1.0);
		findMin(double(5.0), result);
		EXPECT_EQ(double(1.0), std::get<double>(result));
	}

	// Test find min of scalar types with different types (expect failure)
	{
		Scalar result = uint64_t(5);
		EXPECT_THROW(findMin(int64_t(5), result), std::exception);
	}
}

/**
 * @test Test find max of scalar values
 */
TEST(AggMinMaxTest, TestFindMax)
{
	// Test find max of uint64_t scalar
	{
		Scalar result = uint64_t(1);
		findMax(uint64_t(10), result);
		EXPECT_EQ(uint64_t(10), std::get<uint64_t>(result));
	}

	// Test find max of int64_t scalar
	{
		Scalar result = int64_t(1);
		findMax(int64_t(5), result);
		EXPECT_EQ(int64_t(5), std::get<int64_t>(result));
	}

	// Test find max of double scalar
	{
		Scalar result = double(1.0);
		findMax(double(5.0), result);
		EXPECT_EQ(double(5.0), std::get<double>(result));
	}

	// Test find max of scalar types with different types (expect failure)
	{
		Scalar result = uint64_t(5);
		EXPECT_THROW(findMax(int64_t(5), result), std::exception);
	}
}

/**
 * @test Test aggregation of scalar values
 */
TEST(AggMinMaxTest, TestAggregateScalar)
{
	const AggMethodMinMax::AggMethod minMethod = findMin;
	const AggMethodMinMax::AggMethod maxMethod = findMax;

	// Test aggregation of scalar values (min)
	{
		std::vector<AggContent> values = {Scalar {5.0}, Scalar {10.0}, Scalar {15.0}};
		Scalar result = aggregateScalar(values, minMethod);
		EXPECT_EQ(Scalar {5.0}, result);
	}

	// Test aggregation of scalar values (max)
	{
		std::vector<AggContent> values = {Scalar {5.0}, Scalar {15.0}, Scalar {10.0}};
		Scalar result = aggregateScalar(values, maxMethod);
		EXPECT_EQ(Scalar {15.0}, result);
	}

	// Test aggregation of ScalarWithUnit values (expect failure)
	{
		std::vector<AggContent> values = {ScalarWithUnit {5.0, "unit"}};
		EXPECT_THROW(aggregateScalar(values, minMethod), TelemetryException);
	}

	// Test aggregation of empty vector
	{
		std::vector<AggContent> values = {};
		Scalar result = aggregateScalar(values, maxMethod);
		EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
	}
}

/**
 * @test Test aggregation of scalar values with units
 */
TEST(AggMinMaxTest, TestAggregateScalarWithUnit)
{
	const AggMethodMinMax::AggMethod minMethod = findMin;
	const AggMethodMinMax::AggMethod maxMethod = findMax;

	// Test aggregation of scalar values with unit (min)
	{
		std::vector<AggContent> values
			= {ScalarWithUnit {5.0, "unit"},
			   ScalarWithUnit {-10.0, "unit"},
			   ScalarWithUnit {15.0, "unit"}};
		const auto& [scalar, unit] = aggregateScalarWithUnit(values, minMethod);
		EXPECT_EQ(std::get<double>(scalar), -10.0);
		EXPECT_EQ(unit, "unit");
	}

	// Test aggregation of scalar values with unit (max)
	{
		std::vector<AggContent> values
			= {ScalarWithUnit {5.0, "unit"},
			   ScalarWithUnit {10.0, "unit"},
			   ScalarWithUnit {150.0, "unit"}};
		const auto& [scalar, unit] = aggregateScalarWithUnit(values, maxMethod);
		EXPECT_EQ(std::get<double>(scalar), 150.0);
		EXPECT_EQ(unit, "unit");
	}

	// Test aggregation of Scalar values (expect failure)
	{
		std::vector<AggContent> values = {Scalar {5.0}};
		EXPECT_THROW(aggregateScalarWithUnit(values, maxMethod), TelemetryException);
	}

	// Test aggregation of empty vector
	{
		std::vector<AggContent> values = {};
		const auto& [scalar, unit] = aggregateScalarWithUnit(values, maxMethod);
		EXPECT_TRUE(std::holds_alternative<std::monostate>(scalar));
		EXPECT_EQ(unit, "");
	}
}

/**
 * @test Test creation of dictionary content
 */
TEST(AggMinMaxTest, TestCreateDictContent)
{
	ResultType result = Scalar {uint64_t(30)};
	Content content = createDictContent("min", result);

	EXPECT_TRUE(std::holds_alternative<Dict>(content));

	Dict& contentDict = std::get<Dict>(content);
	EXPECT_EQ(1, contentDict.size());

	auto iter = contentDict.cbegin();
	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("min", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<uint64_t>(scalar));
		EXPECT_EQ(uint64_t(30), std::get<uint64_t>(scalar));
	}
}

/**
 * @test Test aggregation method for sum
 */
TEST(AggMinMaxTest, TestAggregate)
{
	// Test aggregation of scalar values (min)
	{
		AggMethodMinMax aggMethodMin(AggMethodType::MIN);
		std::vector<Content> contents = {Scalar {5.0}, Scalar {100.0}, Scalar {-105.0}};
		Content content = aggMethodMin.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Scalar>(content));
		Scalar& scalar = std::get<Scalar>(content);
		EXPECT_TRUE(std::holds_alternative<double>(scalar));
		double result = std::get<double>(scalar);
		EXPECT_EQ(-105.0, result);
	}

	// Test aggregation of scalar values (max)
	{
		AggMethodMinMax aggMethodMax(AggMethodType::MAX);
		std::vector<Content> contents = {Scalar {5.0}, Scalar {100.0}, Scalar {-105.0}};
		Content content = aggMethodMax.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Scalar>(content));
		Scalar& scalar = std::get<Scalar>(content);
		EXPECT_TRUE(std::holds_alternative<double>(scalar));
		double result = std::get<double>(scalar);
		EXPECT_EQ(100.0, result);
	}

	// Test aggregation of ScalarWithUnit values (min)
	{
		AggMethodMinMax aggMethodMin(AggMethodType::MIN);
		std::vector<Content> contents = {ScalarWithUnit {5.0, "unit"}};
		Content content = aggMethodMin.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<ScalarWithUnit>(content));
		const auto& [scalar, unit] = std::get<ScalarWithUnit>(content);
		EXPECT_EQ(5.0, std::get<double>(scalar));
		EXPECT_EQ("unit", unit);
	}

	// Test aggregation of mixed types (expect failure)
	{
		AggMethodMinMax aggMethodMin(AggMethodType::MIN);
		std::vector<Content> contents = {ScalarWithUnit {5.0, "unit"}, Scalar {5.0}};
		EXPECT_THROW(aggMethodMin.aggregate(contents), TelemetryException);
	}

	// Test aggregation of incompatible types (expect failure)
	{
		AggMethodMinMax aggMethodMin(AggMethodType::MIN);
		std::vector<Content> contents = {Scalar {true}, Scalar {5.0}};
		EXPECT_THROW(aggMethodMin.aggregate(contents), TelemetryException);
	}

	// Test aggregation of incompatible scalar types (expect failure)
	{
		AggMethodMinMax aggMethodMin(AggMethodType::MIN);
		std::vector<Content> contents = {Scalar {uint64_t(20)}, Scalar {5.0}};
		EXPECT_THROW(aggMethodMin.aggregate(contents), TelemetryException);
	}

	// Test aggregation of scalar and uint64_t types
	{
		AggMethodMinMax aggMethodMax(AggMethodType::MAX);
		std::vector<Content> contents = {Scalar {uint64_t(20)}, uint64_t {5}};
		Content content = aggMethodMax.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Scalar>(content));
		const auto& scalar = std::get<Scalar>(content);
		EXPECT_EQ(20, std::get<uint64_t>(scalar));
	}

	// Test aggregation of dictionary values (min)
	{
		AggMethodMinMax aggMethodMin(AggMethodType::MIN);
		aggMethodMin.setDictField("packets", "packetsSum");
		std::vector<Content> contents
			= {Dict({{"packets", Scalar {uint64_t(1)}}}),
			   Dict({{"packets", Scalar {uint64_t(5)}}})};
		Content content = aggMethodMin.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Dict>(content));

		const Dict& dict = std::get<Dict>(content);
		EXPECT_EQ(1, dict.size());

		const Scalar& scalarValueSum = std::get<Scalar>(dict.at("packetsSum"));
		EXPECT_EQ(uint64_t(1), std::get<uint64_t>(scalarValueSum));
	}

	// Test aggregation of dictionary values (max)
	{
		AggMethodMinMax aggMethodMax(AggMethodType::MAX);
		aggMethodMax.setDictField("packets", "packetsSum");
		std::vector<Content> contents
			= {Dict({{"packets", Scalar {uint64_t(1)}}}),
			   Dict({{"packets", Scalar {uint64_t(5)}}})};
		Content content = aggMethodMax.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Dict>(content));

		const Dict& dict = std::get<Dict>(content);
		EXPECT_EQ(1, dict.size());

		const Scalar& scalarValueSum = std::get<Scalar>(dict.at("packetsSum"));
		EXPECT_EQ(uint64_t(5), std::get<uint64_t>(scalarValueSum));
	}
}

} // namespace telemetry
