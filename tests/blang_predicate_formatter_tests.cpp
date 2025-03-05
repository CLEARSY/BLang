/* @file blang_predicate_formatter_tests.cpp
   @brief Unit tests for the formatting of Predicate classes.

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

#include <vector>

#include "blang_fmt.h"

class BLangPredicateFmtTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Formatter Tests
TEST_F(BLangPredicateFmtTest, ConstantExpressionFormatting) {
  // Create some expressions
  auto f = BLang::ExpressionFactory::FALSE();
  auto t = BLang::ExpressionFactory::TRUE();

  // Create some atomic predicates
  auto tt = BLang::PredicateFactory::Equality(t, t);
  auto ff = BLang::PredicateFactory::Equality(f, f);
  auto ft = BLang::PredicateFactory::Equality(f, t);
  auto tf = BLang::PredicateFactory::Equality(t, f);

  EXPECT_EQ(fmt::format("{}", tt), "(TRUE = TRUE)");
  EXPECT_EQ(fmt::format("{}", tf), "(TRUE = FALSE)");
  EXPECT_EQ(fmt::format("{}", ft), "(FALSE = TRUE)");
  EXPECT_EQ(fmt::format("{}", ff), "(FALSE = FALSE)");

  auto conj = BLang::PredicateFactory::Conjunction({tt, ff});
  auto disj = BLang::PredicateFactory::Disjunction({tt, ff});
  auto neg = BLang::PredicateFactory::Negation(tt);
  auto impl = BLang::PredicateFactory::Implication(tt, ff);
  auto equiv = BLang::PredicateFactory::Equivalence(tt, ff);
  // EXPECT_EQ(fmt::format("{}", conj), "");
  // EXPECT_EQ(fmt::format("{}", disj), "");
  EXPECT_EQ(fmt::format("{}", neg), "not (TRUE = TRUE)");
  EXPECT_EQ(fmt::format("{}", impl), "((TRUE = TRUE) => (FALSE = FALSE))");
  EXPECT_EQ(fmt::format("{}", equiv), "((TRUE = TRUE) <=> (FALSE = FALSE))");
}

// main function
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}