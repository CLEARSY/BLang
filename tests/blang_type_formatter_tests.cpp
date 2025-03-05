/* @file btype_formatter_tests.cpp
   @brief Unit tests for the BLang::Type formatting.

   @note This file is part of BLang.
   @copyright Copyright © CLEARSY 2025
   @license GNU General Public License (GPL) version 3

   BLang is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <gtest/gtest.h>

#include <future>
#include <thread>
#include <vector>

#include "blang_type_fmt.h"

class BTypeFmtTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Formatter Tests
TEST_F(BTypeFmtTest, BasicTypeFormatting) {
  EXPECT_EQ(fmt::format("{}", BLang::TypeFactory::Integer()), "INTEGER");
  EXPECT_EQ(fmt::format("{}", BLang::TypeFactory::Boolean()), "BOOLEAN");
  EXPECT_EQ(fmt::format("{}", BLang::TypeFactory::Float()), "FLOAT");
  EXPECT_EQ(fmt::format("{}", BLang::TypeFactory::Real()), "REAL");
  EXPECT_EQ(fmt::format("{}", BLang::TypeFactory::String()), "STRING");
  EXPECT_EQ(fmt::format("{}", BLang::TypeFactory::Undefined()), "?");
}

TEST_F(BTypeFmtTest, ProductTypeFormatting) {
  auto product = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                             BLang::TypeFactory::Boolean());
  EXPECT_EQ(fmt::format("{}", product), "(INTEGER × BOOLEAN)");
}

TEST_F(BTypeFmtTest, PowerTypeFormatting) {
  auto powerSet = BLang::TypeFactory::PowerSet(BLang::TypeFactory::Integer());
  EXPECT_EQ(fmt::format("{}", powerSet), "ℙ(INTEGER)");
}

TEST_F(BTypeFmtTest, AbstractSetFormatting) {
  auto abstractSet = BLang::TypeFactory::AbstractSet("MySet");
  EXPECT_EQ(fmt::format("{}", abstractSet), "MySet");
}

TEST_F(BTypeFmtTest, EnumeratedSetFormatting) {
  std::vector<std::string> values = {"One", "Two", "Three"};
  auto enumSet = BLang::TypeFactory::EnumeratedSet("Colors", values);
  EXPECT_EQ(fmt::format("{}", enumSet), "Colors");
}

TEST_F(BTypeFmtTest, StructTypeFormatting) {
  std::vector<std::pair<std::string, std::shared_ptr<BLang::Type>>> fields = {
      {"field1f", BLang::TypeFactory::Integer()},
      {"field2f", BLang::TypeFactory::Boolean()}};
  auto record = BLang::TypeFactory::Struct(fields);
  EXPECT_EQ(fmt::format("{}", record),
            "struct({field1f: INTEGER, field2f: BOOLEAN})");
}

TEST_F(BTypeFmtTest, NullptrFormatting) {
  std::shared_ptr<BLang::Type> nullType;
  EXPECT_EQ(fmt::format("{}", nullType), "nullptr");
}

TEST_F(BTypeFmtTest, NestedTypeFormatting) {
  // Test nested product type
  auto nestedProduct = BLang::TypeFactory::Product(
      BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                  BLang::TypeFactory::Boolean()),
      BLang::TypeFactory::String());
  EXPECT_EQ(fmt::format("{}", nestedProduct), "((INTEGER × BOOLEAN) × STRING)");

  // Test nested power type
  auto nestedPower = BLang::TypeFactory::PowerSet(
      BLang::TypeFactory::PowerSet(BLang::TypeFactory::Integer()));
  EXPECT_EQ(fmt::format("{}", nestedPower), "ℙ(ℙ(INTEGER))");

  // Test complex nested structure
  std::vector<std::pair<std::string, std::shared_ptr<BLang::Type>>> fields = {
      {"field1g", BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                              BLang::TypeFactory::Boolean())},
      {"field2g", BLang::TypeFactory::PowerSet(BLang::TypeFactory::String())}};
  auto complexStruct = BLang::TypeFactory::Struct(fields);
  EXPECT_EQ(fmt::format("{}", complexStruct),
            "struct({field1g: (INTEGER × BOOLEAN), field2g: ℙ(STRING)})");
}

TEST_F(BTypeFmtTest, FormatterThreadSafety) {
  const int numThreads = 10;
  std::vector<std::thread> threads;
  std::vector<std::string> results(numThreads);

  auto complexType = BLang::TypeFactory::Product(
      BLang::TypeFactory::PowerSet(BLang::TypeFactory::Integer()),
      BLang::TypeFactory::Struct({{"field1t", BLang::TypeFactory::Boolean()},
                                  {"field2t", BLang::TypeFactory::String()}}));

  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([&results, complexType, i]() {
      results[i] = fmt::format("{}", complexType);
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  // All threads should produce the same formatted string
  std::string expected = fmt::format("{}", complexType);
  for (const auto& result : results) {
    EXPECT_EQ(result, expected);
  }
}

TEST_F(BTypeFmtTest, FormatterEdgeCases) {
  // Abstract set with special characters
  auto specialSet = BLang::TypeFactory::AbstractSet("Set@#$%");
  EXPECT_EQ(fmt::format("{}", specialSet), "Set@#$%");
}

// main function
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}