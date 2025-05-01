/* @file blang_expression_formatter_tests.cpp
   @brief Unit tests for the formatting of Expression classes.

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

#include "blang_expression_fmt.h"

class BLangExpressionFmtTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Formatter Tests
TEST_F(BLangExpressionFmtTest, ConstantExpressionFormatting) {
  EXPECT_EQ(fmt::format("{}", BLang::ExpressionFactory::TRUE()), "TRUE");
  EXPECT_EQ(fmt::format("{}", BLang::ExpressionFactory::FALSE()), "FALSE");
  EXPECT_EQ(fmt::format("{}", BLang::ExpressionFactory::ConversionBool(
                                  BLang::PredicateFactory::Equality(
                                      BLang::ExpressionFactory::TRUE(),
                                      BLang::ExpressionFactory::FALSE()))),
            "bool(TRUE = FALSE)");
}

// main function
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}