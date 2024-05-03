/**
 * @file
 * @author Pavel Siska <siska@cesnet.cz>
 * @brief Unit tests of Telemetry::AggMethodJoin
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <telemetry/directory.hpp>

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test aggregation of gathered values
 */
TEST(AggJoinTest, TestAggregateGatheredValues)
{
	// Test aggregation of scalar values
	{
		std::vector<AggContent> values = {Scalar {5.0}, Scalar {10.0}, Scalar {15.0}};
		ResultType result = aggregateGatheredValues(values);
		EXPECT_EQ(result.size(), 3);
		EXPECT_EQ(std::get<double>(result[0]), 5.0);
		EXPECT_EQ(std::get<double>(result[1]), 10.0);
		EXPECT_EQ(std::get<double>(result[2]), 15.0);
	}

	// Test aggregation of array values
	{
		std::vector<AggContent> values
			= {Array {Scalar {5.0}, Scalar {-5.0}}, Array {Scalar {10.0}}, Array {Scalar {15.0}}};
		ResultType result = aggregateGatheredValues(values);
		EXPECT_EQ(result.size(), 4);
		EXPECT_EQ(std::get<double>(result[0]), 5.0);
		EXPECT_EQ(std::get<double>(result[1]), -5.0);
		EXPECT_EQ(std::get<double>(result[2]), 10.0);
		EXPECT_EQ(std::get<double>(result[3]), 15.0);
	}
}

/**
 * @test Test aggregation method for joining values
 */
TEST(AggJoinTest, TestAggregate)
{
	// Test aggregation of scalar values
	{
		AggMethodJoin aggMethodJoin;
		std::vector<Content> contents = {Scalar {5.0}, Scalar {10.0}, Scalar {15.0}};
		Content content = aggMethodJoin.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Array>(content));
		Array& array = std::get<Array>(content);
		EXPECT_EQ(array.size(), 3);
		EXPECT_EQ(std::get<double>(array[0]), 5.0);
		EXPECT_EQ(std::get<double>(array[1]), 10.0);
		EXPECT_EQ(std::get<double>(array[2]), 15.0);
	}

	// Test aggregation of ScalarWithUnit values (expect failure)
	{
		AggMethodJoin aggMethodJoin;
		std::vector<Content> contents
			= {ScalarWithUnit {5.0, "unit"}, ScalarWithUnit {5.0, "unit1"}};
		EXPECT_THROW(aggMethodJoin.aggregate(contents), TelemetryException);
	}

	{
		AggMethodJoin aggMethodJoin;
		std::vector<Content> contents = {ScalarWithUnit {5.0, "unit"}, Scalar {5.0}};
		EXPECT_THROW(aggMethodJoin.aggregate(contents), TelemetryException);
	}

	{
		AggMethodJoin aggMethodJoin;
		std::vector<Content> contents = {Scalar {true}, Scalar {5.0}};
		EXPECT_THROW(aggMethodJoin.aggregate(contents), TelemetryException);
	}

	{
		AggMethodJoin aggMethodJoin;
		std::vector<Content> contents = {Scalar {uint64_t(20)}, Scalar {5.0}};
		EXPECT_THROW(aggMethodJoin.aggregate(contents), TelemetryException);
	}

	// Test aggregation of uint64_t values
	{
		AggMethodJoin aggMethodJoin;
		std::vector<Content> contents = {uint64_t(20), uint64_t {5}};
		Content content = aggMethodJoin.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Array>(content));
		Array& array = std::get<Array>(content);
		EXPECT_EQ(array.size(), 2);
		EXPECT_EQ(std::get<uint64_t>(array[0]), 20.0);
		EXPECT_EQ(std::get<uint64_t>(array[1]), 5.0);
	}

	// Test aggregation of dictionaries
	{
		AggMethodJoin aggMethodJoin;
		aggMethodJoin.setDictField("packets", "packetsSum");
		std::vector<Content> contents
			= {Dict({{"packets", Scalar {uint64_t(1)}}}),
			   Dict({{"packets", Scalar {uint64_t(5)}}})};
		Content content = aggMethodJoin.aggregate(contents);
		EXPECT_TRUE(std::holds_alternative<Dict>(content));

		const Dict& dict = std::get<Dict>(content);
		EXPECT_EQ(1, dict.size());

		const Array& array = std::get<Array>(dict.at("packetsSum"));
		EXPECT_EQ(array.size(), 2);
		EXPECT_EQ(std::get<uint64_t>(array[0]), 1);
		EXPECT_EQ(std::get<uint64_t>(array[1]), 5);
	}
}

} // namespace telemetry
