/**
 * @file
 * @author Lukas Hutak <hutak@cesnet.cz>
 * @brief Unit tests of Telemetry::content
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "strUtils.hpp"

#include <gtest/gtest.h>

namespace telemetry {

/**
 * @test Test conversion of a scalar to string.
 */
TEST(TelemetryContent, scalarToString)
{
	EXPECT_EQ("<N/A>", scalarToString(Scalar {}));

	const bool boolTrue = true;
	const bool boolFalse = false;
	EXPECT_EQ("true", scalarToString(Scalar {boolTrue}));
	EXPECT_EQ("false", scalarToString(Scalar {boolFalse}));

	const int64_t intZero = 0;
	const int64_t intOnePlus = 1;
	const int64_t intOneMinus = -1;
	const int64_t intRandomPlus = 123456789;
	const int64_t intRandomMinus = -123456789;
	EXPECT_EQ("0", scalarToString(Scalar {intZero}));
	EXPECT_EQ("1", scalarToString(Scalar {intOnePlus}));
	EXPECT_EQ("-1", scalarToString(Scalar {intOneMinus}));
	EXPECT_EQ("123456789", scalarToString(Scalar {intRandomPlus}));
	EXPECT_EQ("-123456789", scalarToString(Scalar {intRandomMinus}));

	const uint64_t uintZero = 0;
	const uint64_t uintOne = 1;
	const uint64_t uintRandom = 123456789;
	EXPECT_EQ("0", scalarToString(Scalar {uintZero}));
	EXPECT_EQ("1", scalarToString(Scalar {uintOne}));
	EXPECT_EQ("123456789", scalarToString(Scalar {uintRandom}));

	const double doubleZero = 0.0;
	const double doubleOne = 1.0;
	const double doubleRandomPlus = 123.456;
	const double doubleRandomMinus = -123456789.123;
	EXPECT_EQ("0.00", scalarToString(Scalar {doubleZero}));
	EXPECT_EQ("1.00", scalarToString(Scalar {doubleOne}));
	EXPECT_EQ("123.46", scalarToString(Scalar {doubleRandomPlus}));
	EXPECT_EQ("-123456789.12", scalarToString(Scalar {doubleRandomMinus}));

	const std::string stringHello {"hello world!"};
	EXPECT_EQ("", scalarToString(Scalar {std::string("")}));
	EXPECT_EQ("hello world!", scalarToString(Scalar {stringHello}));
}

/**
 * @test Test conversion of a scalar with a unit to string.
 */
TEST(TelemetryContent, ScalarWithUnitToString)
{
	EXPECT_EQ("<N/A> (unit)", scalarWithUnitToString(ScalarWithUnit {{}, "unit"}));

	const bool boolTrue = true;
	const bool boolFalse = false;
	EXPECT_EQ("true (unit)", scalarWithUnitToString(ScalarWithUnit {boolTrue, "unit"}));
	EXPECT_EQ("false (unit)", scalarWithUnitToString(ScalarWithUnit {boolFalse, "unit"}));

	const int64_t intZero = 0;
	const int64_t intOnePlus = 1;
	const int64_t intOneMinus = -1;
	const int64_t intRandomPlus = 123456789;
	const int64_t intRandomMinus = -123456789;
	EXPECT_EQ("0 (unit)", scalarWithUnitToString(ScalarWithUnit {intZero, "unit"}));
	EXPECT_EQ("1 (unit)", scalarWithUnitToString(ScalarWithUnit {intOnePlus, "unit"}));
	EXPECT_EQ("-1 (unit)", scalarWithUnitToString(ScalarWithUnit {intOneMinus, "unit"}));
	EXPECT_EQ("123456789 (unit)", scalarWithUnitToString(ScalarWithUnit {intRandomPlus, "unit"}));
	EXPECT_EQ("-123456789 (unit)", scalarWithUnitToString(ScalarWithUnit {intRandomMinus, "unit"}));

	const uint64_t uintZero = 0;
	const uint64_t uintOne = 1;
	const uint64_t uintRandom = 123456789;
	EXPECT_EQ("0 (unit)", scalarWithUnitToString(ScalarWithUnit {uintZero, "unit"}));
	EXPECT_EQ("1 (unit)", scalarWithUnitToString(ScalarWithUnit {uintOne, "unit"}));
	EXPECT_EQ("123456789 (unit)", scalarWithUnitToString(ScalarWithUnit {uintRandom, "unit"}));

	const double doubleZero = 0.0;
	const double doubleOne = 1.0;
	const double doubleRandomPlus = 123.456;
	const double doubleRandomMinus = -123456789.123456;
	EXPECT_EQ("0.00 (unit)", scalarWithUnitToString(ScalarWithUnit {doubleZero, "unit"}));
	EXPECT_EQ("1.00 (unit)", scalarWithUnitToString(ScalarWithUnit {doubleOne, "unit"}));
	EXPECT_EQ("123.46 (unit)", scalarWithUnitToString(ScalarWithUnit {doubleRandomPlus, "unit"}));
	EXPECT_EQ(
		"-123456789.12 (unit)",
		scalarWithUnitToString(ScalarWithUnit {doubleRandomMinus, "unit"}));

	const std::string stringHello {"hello world!"};
	EXPECT_EQ(" (unit)", scalarWithUnitToString(ScalarWithUnit {std::string(""), "unit"}));
	EXPECT_EQ("hello world! (unit)", scalarWithUnitToString(ScalarWithUnit {stringHello, "unit"}));
}

/**
 * @test Test conversion of an array to string.
 */
TEST(TelemetryContent, arrayToString)
{
	EXPECT_EQ("[]", arrayToString(Array {}));
	EXPECT_EQ("[true]", arrayToString(Array {true}));

	const uint64_t uintOne = 1;
	const int64_t intMinusOne = -1;
	EXPECT_EQ("[1, -1]", arrayToString(Array {uintOne, intMinusOne}));

	const int64_t intOne = 1;
	const uint64_t uintTwo = 2;
	const uint64_t uintThree = 3;
	EXPECT_EQ("[1, 2, 3]", arrayToString(Array {intOne, uintTwo, uintThree}));
	EXPECT_EQ("[eth0, eth1]", arrayToString(Array {std::string("eth0"), std::string("eth1")}));
}

/**
 * @test Test conversion of a dictionary to string.
 */
TEST(TelemetryContent, dictToString)
{
	const Dict dictEmpty {};
	EXPECT_EQ("", dictToString(dictEmpty));

	const Dict dictSimple {{"key", Scalar {std::string("value")}}};
	EXPECT_EQ("key: value", dictToString(dictSimple));

	const Dict dictComplex {
		{"unknown", Scalar {}},
		{"boolean", Scalar {true}},
		{"int", Scalar {int64_t(-1)}},
		{"uint", Scalar {uint64_t(1)}},
		{"double", Scalar {123.456}},
		{"string", Scalar {std::string("eth")}},
		{"number and unit", ScalarWithUnit {uint64_t(123), "pkts"}},
		{"array", Array {int64_t(1), uint64_t(2), uint64_t(3)}}};
	const std::string complexStr = dictToString(dictComplex);
	const auto complexLines = strUtils::splitViewByDelimiter(complexStr, "\n");
	auto iter = complexLines.cbegin();
	ASSERT_EQ(8, complexLines.size());

	// Note: since results are stored in a map, they should be in alphabetical order
	std::vector<std::string_view> pieces;

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("array", strUtils::trimView(pieces[0]));
	EXPECT_EQ("[1, 2, 3]", strUtils::trimView(pieces[1]));

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("boolean", strUtils::trimView(pieces[0]));
	EXPECT_EQ("true", strUtils::trimView(pieces[1]));

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("double", strUtils::trimView(pieces[0]));
	EXPECT_EQ("123.46", strUtils::trimView(pieces[1]));

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("int", strUtils::trimView(pieces[0]));
	EXPECT_EQ("-1", strUtils::trimView(pieces[1]));

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("number and unit", strUtils::trimView(pieces[0]));
	EXPECT_EQ("123 (pkts)", strUtils::trimView(pieces[1]));

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("string", strUtils::trimView(pieces[0]));
	EXPECT_EQ("eth", strUtils::trimView(pieces[1]));

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("uint", strUtils::trimView(pieces[0]));
	EXPECT_EQ("1", strUtils::trimView(pieces[1]));

	pieces = strUtils::splitViewByDelimiter(*(iter++), ":");
	ASSERT_EQ(2, pieces.size());
	EXPECT_EQ("unknown", strUtils::trimView(pieces[0]));
	EXPECT_EQ("<N/A>", strUtils::trimView(pieces[1]));

	EXPECT_EQ(iter, complexLines.cend());
}

/**
 * @test Test conversion of a content to string.
 */
TEST(TelemetryContent, contentToString)
{
	const uint64_t uintZero = 0;
	const bool boolTrue = true;
	const Dict dictEmpty {};
	const Dict dictSimple {{"key", Scalar {std::string("value")}}};

	// Scalar
	EXPECT_EQ("0", contentToString(Scalar {uintZero}));
	EXPECT_EQ("true", contentToString(Scalar {boolTrue}));

	// Scalar with unit
	EXPECT_EQ("0 (pkts)", contentToString(ScalarWithUnit {uintZero, "pkts"}));
	EXPECT_EQ("true (unit)", contentToString(ScalarWithUnit {boolTrue, "unit"}));

	// Array
	EXPECT_EQ("[]", contentToString(Array {}));

	const int64_t intOne = 1;
	const uint64_t uintTwo = 2;
	const int64_t intThree = 3;
	EXPECT_EQ("[1, 2, 3]", contentToString(Array {intOne, uintTwo, intThree}));

	// Dictionary
	EXPECT_EQ("", contentToString(dictEmpty));
	EXPECT_EQ("key: value", contentToString(dictSimple));
}

} // namespace telemetry
