/* @file blang_expression_tests.cpp
   @brief Unit tests for the Expression class and ExpressionFactory classes.

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

#include <thread>
#include <vector>

#include "blang_expression.h"
#include "blang_predicate.h"

class BLangExpressionTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Tests for ExpressionFactory size and at methods
TEST_F(BLangExpressionTest, FactorySizeAndAt) {
  // Ensure the factory is initially empty
  EXPECT_EQ(BLang::ExpressionFactory::size(), 0);

  // Create some expressions
  auto falseExpr = BLang::ExpressionFactory::FALSE();
  auto trueExpr = BLang::ExpressionFactory::TRUE();

  // Check the size after creating expressions
  EXPECT_EQ(BLang::ExpressionFactory::size(), 2);

  // Check the expressions at specific indices
  EXPECT_EQ(BLang::ExpressionFactory::at(0)->getKind(),
            BLang::Expression::Kind::FALSE);
  EXPECT_EQ(BLang::ExpressionFactory::at(1)->getKind(),
            BLang::Expression::Kind::TRUE);

  // Create more expressions
  auto falseExpr2 = BLang::ExpressionFactory::FALSE();
  auto trueExpr2 = BLang::ExpressionFactory::TRUE();

  EXPECT_EQ(BLang::ExpressionFactory::size(), 2);
}

// Comparison Tests
TEST_F(BLangExpressionTest, ExpressionComparisons) {
  auto true1 = BLang::ExpressionFactory::TRUE();
  auto true2 = BLang::ExpressionFactory::TRUE();
  auto false1 = BLang::ExpressionFactory::FALSE();

  EXPECT_EQ(*true1, *true2);
  EXPECT_NE(*true1, *false1);
}

// Thread Safety Tests
TEST_F(BLangExpressionTest, ThreadSafety) {
  const int numThreads = 10;
  std::vector<std::thread> threads;

  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([]() {
      auto truei = BLang::ExpressionFactory::TRUE();
      auto falsei = BLang::ExpressionFactory::FALSE();

      EXPECT_EQ(truei->getKind(), BLang::Expression::Kind::TRUE);
      EXPECT_EQ(falsei->getKind(), BLang::Expression::Kind::FALSE);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

// Maximal Sharing Tests
TEST_F(BLangExpressionTest, MaximalSharing) {
  auto true1 = BLang::ExpressionFactory::TRUE();
  auto true2 = BLang::ExpressionFactory::TRUE();

  auto false1 = BLang::ExpressionFactory::FALSE();
  auto false2 = BLang::ExpressionFactory::FALSE();

  EXPECT_EQ(true1, true2);
  EXPECT_EQ(false2, false1);
}

// Visitor Pattern Tests
class TestVisitor : public BLang::Expression::Visitor {
 public:
  void visitTRUE() override { lastVisited = "TRUE"; }
  void visitFALSE() override { lastVisited = "FALSE"; }
  void visitConversionBool(const BLang::Expression::ConversionBool &) override {
    lastVisited = "ConversionBool";
  }
  void visitIntegerLiteral(const BLang::Expression::IntegerLiteral &) override {
    lastVisited = "IntegerLiteral";
  }
  std::string lastVisited;
};

TEST_F(BLangExpressionTest, VisitorPattern) {
  TestVisitor visitor;

  BLang::ExpressionFactory::TRUE()->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "TRUE");

  BLang::ExpressionFactory::FALSE()->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "FALSE");

  BLang::ExpressionFactory::ConversionBool(
      BLang::PredicateFactory::Equality(BLang::ExpressionFactory::TRUE(),
                                        BLang::ExpressionFactory::FALSE()))
      ->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "ConversionBool");

  BLang::ExpressionFactory::IntegerLiteral("42")->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "IntegerLiteral");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
