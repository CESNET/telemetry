/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Unit tests of Telemetry::AggMethodSum
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test summarization of scalar values
 */
TEST(AggSumTest, TestSumarize)
{
	// Test summation of uint64_t scalar
	{
		Scalar result = uint64_t(0);
		sumarize(uint64_t(5), result);
		EXPECT_EQ(uint64_t(5), std::get<uint64_t>(result));
	}

	// Test summation of int64_t scalar
	{
		Scalar result = std::monostate();
		sumarize(int64_t(5), result);
		EXPECT_EQ(int64_t(5), std::get<int64_t>(result));
	}

	// Test summation of double scalar
	{
		Scalar result = double(0);
		sumarize(double(5.0), result);
		EXPECT_EQ(double(5.0), std::get<double>(result));
	}

	// Test summation of scalar types with different types (expect failure)
	{
		Scalar result = uint64_t(5);
		EXPECT_THROW(sumarize(int64_t(5), result), std::exception);
	}
}

/**
 * @test Test aggregation of scalar values
 */
TEST(AggSumTest, TestAggregateScalar)
{
	// Test aggregation of scalar values
	{
		std::vector<AggContent> values = {Scalar {5.0}, Scalar {10.0}, Scalar {15.0}};
		Scalar result = aggregateScalar(values);
		EXPECT_EQ(Scalar {30.0}, result);
	}

	// Test aggregation of ScalarWithUnit values (expect failure)
	{
		std::vector<AggContent> values = {ScalarWithUnit {5.0, "unit"}};
		EXPECT_THROW(aggregateScalar(values), TelemetryException);
	}

	// Test aggregation of empty vector
	{
		std::vector<AggContent> values = {};
		Scalar result = aggregateScalar(values);
		EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
	}
}

/**
 * @test Test aggregation of scalar values with units
 */
TEST(AggSumTest, TestAggregateScalarWithUnit)
{
	// Test aggregation of scalar values with unit
	{
		std::vector<AggContent> values
			= {ScalarWithUnit {5.0, "unit"},
			   ScalarWithUnit {10.0, "unit"},
			   ScalarWithUnit {15.0, "unit"}};
		const auto& [scalar, unit] = aggregateScalarWithUnit(values);
		EXPECT_EQ(std::get<double>(scalar), 30.0);
		EXPECT_EQ(unit, "unit");
	}

	// Test aggregation of Scalar values (expect failure)
	{
		std::vector<AggContent> values = {Scalar {5.0}};
		EXPECT_THROW(aggregateScalarWithUnit(values), TelemetryException);
	}

	// Test aggregation of empty vector
	{
		std::vector<AggContent> values = {};
		const auto& [scalar, unit] = aggregateScalarWithUnit(values);
		EXPECT_TRUE(std::holds_alternative<std::monostate>(scalar));
		EXPECT_EQ(unit, "");
	}
}

/**
 * @test Test creation of dictionary content
 */
TEST(AggSumTest, TestCreateDictContent)
{
	ResultType result = Scalar {uint64_t(30)};
	Content content = createDictContent("sum", result);

	EXPECT_TRUE(std::holds_alternative<Dict>(content));

	Dict& contentDict = std::get<Dict>(content);
	EXPECT_EQ(1, contentDict.size());

	auto iter = contentDict.cbegin();
	{
		const auto& [key, value] = *(iter++);
		EXPECT_EQ("sum", key);
		EXPECT_TRUE(std::holds_alternative<Scalar>(value));
		const auto& scalar = std::get<Scalar>(value);
		EXPECT_TRUE(std::holds_alternative<uint64_t>(scalar));
		EXPECT_EQ(uint64_t(30), std::get<uint64_t>(scalar));
	}
}

/**
 * @test Test aggregation method for sum
 */
TEST(AggSumTest, TestAggregate)
{
	// Test aggregation of scalar values
	{
		AggMethodSum aggMethodSum;
		std::vector<Content> contents = {Scalar {5.0}, Scalar {10.0}, Scalar {15.0}};
		Content content = aggMethodSum.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Scalar>(content));
		Scalar& scalar = std::get<Scalar>(content);
		EXPECT_TRUE(std::holds_alternative<double>(scalar));
		double result = std::get<double>(scalar);
		EXPECT_EQ(30.0, result);
	}

	// Test aggregation of ScalarWithUnit values
	{
		AggMethodSum aggMethodSum;
		std::vector<Content> contents = {ScalarWithUnit {5.0, "unit"}};
		Content content = aggMethodSum.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<ScalarWithUnit>(content));
		const auto& [scalar, unit] = std::get<ScalarWithUnit>(content);
		EXPECT_EQ(5.0, std::get<double>(scalar));
		EXPECT_EQ("unit", unit);
	}

	// Test aggregation of mixed types (expect failure)
	{
		AggMethodSum aggMethodSum;
		std::vector<Content> contents = {ScalarWithUnit {5.0, "unit"}, Scalar {5.0}};
		EXPECT_THROW(aggMethodSum.aggregate(contents), TelemetryException);
	}

	// Test aggregation of incompatible types (expect failure)
	{
		AggMethodSum aggMethodSum;
		std::vector<Content> contents = {Scalar {true}, Scalar {5.0}};
		EXPECT_THROW(aggMethodSum.aggregate(contents), TelemetryException);
	}

	// Test aggregation of incompatible scalar types (expect failure)
	{
		AggMethodSum aggMethodSum;
		std::vector<Content> contents = {Scalar {uint64_t(20)}, Scalar {5.0}};
		EXPECT_THROW(aggMethodSum.aggregate(contents), TelemetryException);
	}

	// Test aggregation of scalar and uint64_t types
	{
		AggMethodSum aggMethodSum;
		std::vector<Content> contents = {Scalar {uint64_t(20)}, uint64_t {5}};
		Content content = aggMethodSum.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Scalar>(content));
		const auto& scalar = std::get<Scalar>(content);
		EXPECT_EQ(25, std::get<uint64_t>(scalar));
	}

	// Test aggregation of dictionary values
	{
		AggMethodSum aggMethodSum;
		aggMethodSum.setDictField("packets", "packetsSum");
		std::vector<Content> contents
			= {Dict({{"packets", Scalar {uint64_t(1)}}}),
			   Dict({{"packets", Scalar {uint64_t(5)}}})};
		Content content = aggMethodSum.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Dict>(content));

		const Dict& dict = std::get<Dict>(content);
		EXPECT_EQ(1, dict.size());

		const Scalar& scalarValueSum = std::get<Scalar>(dict.at("packetsSum"));
		EXPECT_EQ(uint64_t(6), std::get<uint64_t>(scalarValueSum));
	}
}

} // namespace telemetry
