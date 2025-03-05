/* @file blang_type_tests.cpp
   @brief Unit tests for the Type and TypeFactory classes.

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

#include "blang_type.h"

class BLangTypeTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Tests for TypeFactory size and at methods
TEST_F(BLangTypeTest, FactorySizeAndAt) {
  // Ensure the factory is initially empty
  EXPECT_EQ(BLang::TypeFactory::size(), 0);

  // Create some types
  auto intType = BLang::TypeFactory::Integer();
  auto boolType = BLang::TypeFactory::Boolean();
  auto floatType = BLang::TypeFactory::Float();

  // Check the size after creating types
  EXPECT_EQ(BLang::TypeFactory::size(), 3);

  // Check the types at specific indices
  EXPECT_EQ(BLang::TypeFactory::at(0)->getKind(), BLang::Type::Kind::INTEGER);
  EXPECT_EQ(BLang::TypeFactory::at(1)->getKind(), BLang::Type::Kind::BOOLEAN);
  EXPECT_EQ(BLang::TypeFactory::at(2)->getKind(), BLang::Type::Kind::FLOAT);

  // Create more types
  auto realType = BLang::TypeFactory::Real();
  auto stringType = BLang::TypeFactory::String();

  // Check the size after creating more types
  EXPECT_EQ(BLang::TypeFactory::size(), 5);

  // Check the types at specific indices
  EXPECT_EQ(BLang::TypeFactory::at(3)->getKind(), BLang::Type::Kind::REAL);
  EXPECT_EQ(BLang::TypeFactory::at(4)->getKind(), BLang::Type::Kind::STRING);
}

// Basic Types Tests
TEST_F(BLangTypeTest, BasicTypesCreation) {
  auto intType = BLang::TypeFactory::Integer();
  auto boolType = BLang::TypeFactory::Boolean();
  auto floatType = BLang::TypeFactory::Float();
  auto realType = BLang::TypeFactory::Real();
  auto stringType = BLang::TypeFactory::String();

  EXPECT_EQ(intType->getKind(), BLang::Type::Kind::INTEGER);
  EXPECT_EQ(boolType->getKind(), BLang::Type::Kind::BOOLEAN);
  EXPECT_EQ(floatType->getKind(), BLang::Type::Kind::FLOAT);
  EXPECT_EQ(realType->getKind(), BLang::Type::Kind::REAL);
  EXPECT_EQ(stringType->getKind(), BLang::Type::Kind::STRING);
}

// Product Type Tests
TEST_F(BLangTypeTest, ProductTypeCreation) {
  auto product = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                             BLang::TypeFactory::Boolean());
  EXPECT_EQ(product->getKind(), BLang::Type::Kind::ProductType);

  auto productType = product->toProductType();
  ASSERT_NE(productType, nullptr);
  EXPECT_EQ(productType->lhs->getKind(), BLang::Type::Kind::INTEGER);
  EXPECT_EQ(productType->rhs->getKind(), BLang::Type::Kind::BOOLEAN);
}

// Power Type Tests
TEST_F(BLangTypeTest, PowerTypeCreation) {
  auto powerSet = BLang::TypeFactory::PowerSet(BLang::TypeFactory::Integer());
  EXPECT_EQ(powerSet->getKind(), BLang::Type::Kind::PowerType);

  auto powerType = powerSet->toPowerType();
  ASSERT_NE(powerType, nullptr);
  EXPECT_EQ(powerType->m_content->getKind(), BLang::Type::Kind::INTEGER);
}

// Abstract Set Tests
TEST_F(BLangTypeTest, AbstractSetCreation) {
  auto abstractSet = BLang::TypeFactory::AbstractSet("MySet");
  EXPECT_EQ(abstractSet->getKind(), BLang::Type::Kind::AbstractSet);

  auto setType = abstractSet->toAbstractSetType();
  ASSERT_NE(setType, nullptr);
  EXPECT_EQ(setType->getName(), "MySet");
}

// Enumerated Set Tests
TEST_F(BLangTypeTest, EnumeratedSetCreation) {
  std::vector<std::string> values = {"One", "Two", "Three"};
  auto enumSet = BLang::TypeFactory::EnumeratedSet("Colors", values);
  EXPECT_EQ(enumSet->getKind(), BLang::Type::Kind::EnumeratedSet);

  auto enumType = enumSet->toEnumeratedSetType();
  ASSERT_NE(enumType, nullptr);
  EXPECT_EQ(enumType->getName(), "Colors");
  EXPECT_EQ(enumType->getValues(), values);
}

// Record Type Tests
TEST_F(BLangTypeTest, StructTypeCreation) {
  std::vector<std::pair<std::string, std::shared_ptr<BLang::Type>>> fields = {
      {"field1", BLang::TypeFactory::Integer()},
      {"field2", BLang::TypeFactory::Boolean()}};
  auto record = BLang::TypeFactory::Struct(fields);
  EXPECT_EQ(record->getKind(), BLang::Type::Kind::Struct);

  auto structType = record->toStructType();
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->m_fields.size(), 2);
  EXPECT_EQ(structType->m_fields[0].first, "field1");
  EXPECT_EQ(structType->m_fields[0].second->getKind(),
            BLang::Type::Kind::INTEGER);
}

TEST_F(BLangTypeTest, StructTypeCreationOrder) {
  std::vector<std::pair<std::string, std::shared_ptr<BLang::Type>>> fields1 = {
      {"field1", BLang::TypeFactory::Integer()},
      {"field2", BLang::TypeFactory::Boolean()}};
  std::vector<std::pair<std::string, std::shared_ptr<BLang::Type>>> fields2 = {
      {"field2", BLang::TypeFactory::Boolean()},
      {"field1", BLang::TypeFactory::Integer()}};
  auto struct1 = BLang::TypeFactory::Struct(fields1);
  auto struct2 = BLang::TypeFactory::Struct(fields2);
  EXPECT_EQ(struct1->getKind(), BLang::Type::Kind::Struct);
  EXPECT_EQ(struct2->getKind(), BLang::Type::Kind::Struct);
  EXPECT_EQ(struct1, struct2);

  auto structType1 = struct1->toStructType();
  auto structType2 = struct2->toStructType();
  EXPECT_EQ(structType1, structType2);
}

// Comparison Tests
TEST_F(BLangTypeTest, TypeComparisons) {
  auto int1 = BLang::TypeFactory::Integer();
  auto int2 = BLang::TypeFactory::Integer();
  auto bool1 = BLang::TypeFactory::Boolean();

  EXPECT_EQ(*int1, *int2);
  EXPECT_NE(*int1, *bool1);
  EXPECT_TRUE(*int1 <= *int2);
  EXPECT_TRUE(*int1 >= *int2);
}

// Thread Safety Tests
TEST_F(BLangTypeTest, ThreadSafety) {
  const int numThreads = 10;
  std::vector<std::thread> threads;

  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([i]() {
      auto product = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                                 BLang::TypeFactory::Boolean());
      auto powerSet =
          BLang::TypeFactory::PowerSet(BLang::TypeFactory::Integer());
      auto abstractSet =
          BLang::TypeFactory::AbstractSet("Set" + std::to_string(i));

      EXPECT_EQ(product->getKind(), BLang::Type::Kind::ProductType);
      EXPECT_EQ(powerSet->getKind(), BLang::Type::Kind::PowerType);
      EXPECT_EQ(abstractSet->getKind(), BLang::Type::Kind::AbstractSet);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

// Maximal Sharing Tests
TEST_F(BLangTypeTest, MaximalSharing) {
  auto product1 = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                              BLang::TypeFactory::Boolean());
  auto product2 = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                              BLang::TypeFactory::Boolean());

  EXPECT_EQ(product1->toProductType(), product2->toProductType());
}

// Hash Consistency Tests
TEST_F(BLangTypeTest, HashConsistency) {
  auto product1 = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                              BLang::TypeFactory::Boolean());
  auto product2 = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                              BLang::TypeFactory::Boolean());

  EXPECT_EQ(product1->hash_combine(0), product2->hash_combine(0));
}

// Visitor Pattern Tests
class TestVisitor : public BLang::Type::Visitor {
 public:
  void visitINTEGER() override { lastVisited = "INTEGER"; }
  void visitBOOLEAN() override { lastVisited = "BOOLEAN"; }
  void visitFLOAT() override { lastVisited = "FLOAT"; }
  void visitREAL() override { lastVisited = "REAL"; }
  void visitSTRING() override { lastVisited = "STRING"; }
  void visitAbstractSet(const BLang::Type::AbstractSet &) override {
    lastVisited = "AbstractSet";
  }
  void visitEnumeratedSet(const BLang::Type::EnumeratedSet &) override {
    lastVisited = "EnumeratedSet";
  }
  void visitProductType(const BLang::Type::ProductType &) override {
    lastVisited = "ProductType";
  }
  void visitPowerType(const BLang::Type::PowerType &) override {
    lastVisited = "PowerType";
  }
  void visitStructType(const BLang::Type::StructType &) override {
    lastVisited = "StructType";
  }
  void visitUndefinedType() override { lastVisited = "UNDEFINED"; }

  std::string lastVisited;
};

TEST_F(BLangTypeTest, VisitorPattern) {
  TestVisitor visitor;

  BLang::TypeFactory::Integer()->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "INTEGER");

  auto product = BLang::TypeFactory::Product(BLang::TypeFactory::Integer(),
                                             BLang::TypeFactory::Boolean());
  product->accept(visitor);
  EXPECT_EQ(visitor.lastVisited, "ProductType");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
