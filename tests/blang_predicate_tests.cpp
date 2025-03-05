/* @file blang_predicate_tests.cpp
   @brief Unit tests for the Predicate class and PredicateFactory classes.

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

class BLangPredicateTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Tests for PredicateFactory size and at methods
TEST_F(BLangPredicateTest, FactorySizeAndAt) {
  // Ensure the factory is initially empty
  EXPECT_EQ(BLang::PredicateFactory::size(), 0);

  // Create some expressions
  auto f = BLang::ExpressionFactory::FALSE();
  auto t = BLang::ExpressionFactory::TRUE();

  // Create some atomic predicates
  auto tt = BLang::PredicateFactory::Equality(t, t);
  auto ff = BLang::PredicateFactory::Equality(f, f);
  auto ft = BLang::PredicateFactory::Equality(f, t);
  auto tf = BLang::PredicateFactory::Equality(t, f);

  EXPECT_EQ(BLang::PredicateFactory::size(), 4);
  // Check the predicates at specific indices
  EXPECT_EQ(BLang::PredicateFactory::at(0)->getKind(),
            BLang::Predicate::Kind::Equality);
  EXPECT_EQ(BLang::PredicateFactory::at(1)->getKind(),
            BLang::Predicate::Kind::Equality);
  EXPECT_EQ(BLang::PredicateFactory::at(2)->getKind(),
            BLang::Predicate::Kind::Equality);
  EXPECT_EQ(BLang::PredicateFactory::at(3)->getKind(),
            BLang::Predicate::Kind::Equality);

  auto conj = BLang::PredicateFactory::Conjunction({tt, ff});
  auto disj = BLang::PredicateFactory::Disjunction({tt, ff});
  auto neg = BLang::PredicateFactory::Negation(tt);
  auto impl = BLang::PredicateFactory::Implication(tt, ff);
  auto equiv = BLang::PredicateFactory::Equivalence(tt, ff);

  EXPECT_EQ(BLang::PredicateFactory::size(), 9);
  // Check the predicates at specific indices
  EXPECT_EQ(BLang::PredicateFactory::at(4)->getKind(),
            BLang::Predicate::Kind::Conjunction);
  EXPECT_EQ(BLang::PredicateFactory::at(5)->getKind(),
            BLang::Predicate::Kind::Disjunction);
  EXPECT_EQ(BLang::PredicateFactory::at(6)->getKind(),
            BLang::Predicate::Kind::Negation);
  EXPECT_EQ(BLang::PredicateFactory::at(7)->getKind(),
            BLang::Predicate::Kind::Implication);
  EXPECT_EQ(BLang::PredicateFactory::at(8)->getKind(),
            BLang::Predicate::Kind::Equivalence);
}

// Thread Safety Tests
TEST_F(BLangPredicateTest, ThreadSafety) {
  const int numThreads = 10;
  std::vector<std::thread> threads;

  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([]() {
      // Create some expressions
      auto f = BLang::ExpressionFactory::FALSE();
      auto t = BLang::ExpressionFactory::TRUE();

      // Create some atomic predicates
      auto tt = BLang::PredicateFactory::Equality(t, t);
      auto ff = BLang::PredicateFactory::Equality(f, f);
      auto ft = BLang::PredicateFactory::Equality(f, t);
      auto tf = BLang::PredicateFactory::Equality(t, f);

      auto conj = BLang::PredicateFactory::Conjunction({tt, ff});
      auto disj = BLang::PredicateFactory::Disjunction({tt, ff});
      auto neg = BLang::PredicateFactory::Negation(tt);
      auto impl = BLang::PredicateFactory::Implication(tt, ff);
      auto equiv = BLang::PredicateFactory::Equivalence(tt, ff);

      EXPECT_EQ(BLang::PredicateFactory::at(0)->getKind(),
                BLang::Predicate::Kind::Equality);
      EXPECT_EQ(BLang::PredicateFactory::at(1)->getKind(),
                BLang::Predicate::Kind::Equality);
      EXPECT_EQ(BLang::PredicateFactory::at(2)->getKind(),
                BLang::Predicate::Kind::Equality);
      EXPECT_EQ(BLang::PredicateFactory::at(3)->getKind(),
                BLang::Predicate::Kind::Equality);
      EXPECT_EQ(BLang::PredicateFactory::at(4)->getKind(),
                BLang::Predicate::Kind::Conjunction);
      EXPECT_EQ(BLang::PredicateFactory::at(5)->getKind(),
                BLang::Predicate::Kind::Disjunction);
      EXPECT_EQ(BLang::PredicateFactory::at(6)->getKind(),
                BLang::Predicate::Kind::Negation);
      EXPECT_EQ(BLang::PredicateFactory::at(7)->getKind(),
                BLang::Predicate::Kind::Implication);
      EXPECT_EQ(BLang::PredicateFactory::at(8)->getKind(),
                BLang::Predicate::Kind::Equivalence);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

// Maximal Sharing Tests
TEST_F(BLangPredicateTest, MaximalSharing) {
  // Create some expressions
  auto f = BLang::ExpressionFactory::FALSE();
  auto t = BLang::ExpressionFactory::TRUE();

  // Create some atomic predicates
  auto tt = BLang::PredicateFactory::Equality(t, t);
  auto ff = BLang::PredicateFactory::Equality(f, f);
  auto ft = BLang::PredicateFactory::Equality(f, t);
  auto tf = BLang::PredicateFactory::Equality(t, f);

  auto tt2 = BLang::PredicateFactory::Equality(t, t);
  auto ff2 = BLang::PredicateFactory::Equality(f, f);
  auto ft2 = BLang::PredicateFactory::Equality(f, t);
  auto tf2 = BLang::PredicateFactory::Equality(t, f);

  EXPECT_EQ(tt, tt2);
  EXPECT_EQ(ff, ff2);
  EXPECT_EQ(ft, ft2);
  EXPECT_EQ(tf, tf2);

  auto conj = BLang::PredicateFactory::Conjunction({tt, ff});
  auto disj = BLang::PredicateFactory::Disjunction({tt, ff});
  auto neg = BLang::PredicateFactory::Negation(tt);
  auto impl = BLang::PredicateFactory::Implication(tt, ff);
  auto equiv = BLang::PredicateFactory::Equivalence(tt, ff);

  auto conj2 = BLang::PredicateFactory::Conjunction({tt, ff});
  auto disj2 = BLang::PredicateFactory::Disjunction({tt, ff});
  auto neg2 = BLang::PredicateFactory::Negation(tt);
  auto impl2 = BLang::PredicateFactory::Implication(tt, ff);
  auto equiv2 = BLang::PredicateFactory::Equivalence(tt, ff);

  EXPECT_EQ(conj, conj2);
  EXPECT_EQ(disj, disj2);
  EXPECT_EQ(neg, neg2);
  EXPECT_EQ(impl, impl2);
  EXPECT_EQ(equiv, equiv2);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
