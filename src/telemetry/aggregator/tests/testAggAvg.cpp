/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Unit tests of Telemetry::AggMethodAvg
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test making average from a result and count
 */
TEST(AggAvgTest, TestMakeAverage)
{
	// Test making average from uint64_t
	{
		Scalar result = uint64_t(100);
		makeAverage(result, 10);
		EXPECT_EQ(10.0, std::get<double>(result));
	}

	// Test making average from int64_t
	{
		Scalar result = int64_t(100);
		makeAverage(result, 20);
		EXPECT_EQ(5.0, std::get<double>(result));
	}

	// Test making average from double
	{
		Scalar result = 100.0;
		makeAverage(result, 50);
		EXPECT_EQ(2.0, std::get<double>(result));
	}

	// Test making average from unsupported type (expect failure)
	{
		Scalar result = true;
		EXPECT_THROW(makeAverage(result, 10), TelemetryException);
	}
}

/**
 * @test Test converting aggregated content to an average value
 */
TEST(AggAvgTest, convertToAverage)
{
	// Test converting Scalar to average
	{
		AggContent aggContent = Scalar {5.0};
		ResultType result = convertToAverage(aggContent, 10);
		EXPECT_TRUE(std::holds_alternative<Scalar>(result));
		const auto& scalar = std::get<Scalar>(result);
		EXPECT_EQ(0.5, std::get<double>(scalar));
	}

	// Test converting ScalarWithUnit to average
	{
		AggContent aggContent = ScalarWithUnit {5.0, "unit"};
		ResultType result = convertToAverage(aggContent, 2);
		EXPECT_TRUE(std::holds_alternative<ScalarWithUnit>(result));
		const auto& [scalar, unit] = std::get<ScalarWithUnit>(result);
		EXPECT_EQ(2.5, std::get<double>(scalar));
		EXPECT_EQ("unit", unit);
	}

	// Test converting unsupported type to average (expect failure)
	{
		AggContent aggContent = std::monostate();
		EXPECT_THROW(convertToAverage(aggContent, 2), TelemetryException);
	}
}

/**
 * @test Test aggregation method for averaging values
 */
TEST(AggAvgTest, TestAggregate)
{
	// Test aggregation of scalar values
	{
		AggMethodAvg aggMethodAvg;
		std::vector<Content> contents = {Scalar {5.0}, Scalar {10.0}, Scalar {15.0}};
		Content content = aggMethodAvg.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Scalar>(content));
		Scalar& scalar = std::get<Scalar>(content);
		EXPECT_TRUE(std::holds_alternative<double>(scalar));
		double result = std::get<double>(scalar);
		EXPECT_EQ(10.0, result);
	}

	// Test aggregation of ScalarWithUnit values
	{
		AggMethodAvg aggMethodAvg;
		std::vector<Content> contents = {ScalarWithUnit {5.0, "unit"}};
		Content content = aggMethodAvg.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<ScalarWithUnit>(content));
		const auto& [scalar, unit] = std::get<ScalarWithUnit>(content);
		EXPECT_EQ(5.0, std::get<double>(scalar));
		EXPECT_EQ("unit", unit);
	}

	{
		AggMethodAvg aggMethodAvg;
		std::vector<Content> contents = {ScalarWithUnit {5.0, "unit"}, Scalar {5.0}};
		EXPECT_THROW(aggMethodAvg.aggregate(contents), TelemetryException);
	}

	{
		AggMethodAvg aggMethodAvg;
		std::vector<Content> contents = {Scalar {true}, Scalar {5.0}};
		EXPECT_THROW(aggMethodAvg.aggregate(contents), TelemetryException);
	}

	{
		AggMethodAvg aggMethodAvg;
		std::vector<Content> contents = {Scalar {uint64_t(20)}, Scalar {5.0}};
		EXPECT_THROW(aggMethodAvg.aggregate(contents), TelemetryException);
	}

	{
		AggMethodAvg aggMethodAvg;
		std::vector<Content> contents = {Scalar {uint64_t(20)}, uint64_t {5}};
		Content content = aggMethodAvg.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Scalar>(content));
		const auto& scalar = std::get<Scalar>(content);
		EXPECT_EQ(12.5, std::get<double>(scalar));
	}

	// Test aggregation of dictionaries
	{
		AggMethodAvg aggMethodAvg;
		aggMethodAvg.setDictField("packets", "packetsSum");
		std::vector<Content> contents
			= {Dict({{"packets", Scalar {uint64_t(1)}}}),
			   Dict({{"packets", Scalar {uint64_t(5)}}})};
		Content content = aggMethodAvg.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Dict>(content));

		const Dict& dict = std::get<Dict>(content);
		EXPECT_EQ(1, dict.size());

		const Scalar& scalarValueAvg = std::get<Scalar>(dict.at("packetsSum"));
		EXPECT_EQ(3.0, std::get<double>(scalarValueAvg));
	}
}

} // namespace telemetry
