/* @file blang_tinyxml2_reader_tests.cpp
   @brief Unit tests for BLang XML reader methods, based on TinyXML2.

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

#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

#include "blang_expression.h"
#include "blang_predicate.h"
#include "blang_tinyxml2_reader.h"
#include "blang_type.h"
#include "tinyxml2.h"

class BLangTinyXml2ReaderTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(BLangTinyXml2ReaderTest, ReadType) {
  const char* xmlContent = R"(
    <RichTypesInfo>
      <RichType id="0">
        <INTEGER/>
      </RichType>
      <RichType id="1">
        <BOOL/>
      </RichType>
      <RichType id="2">
        <PowerSet arg="0"/>
      </RichType>
      <RichType id="3">
        <CartesianProduct arg1="0" arg2="1"/>
      </RichType>
      <RichType id="4">
        <AbstractSet name="MyAbstractSet"/>
      </RichType>
      <RichType id="5">
        <EnumeratedSet name="MyEnumSet">
          <EnumeratedValue name="Value1"/>
          <EnumeratedValue name="Value2"/>
        </EnumeratedSet>
      </RichType>
      <RichType id="6">
        <StructType>
          <Field name="field1" type="0"/>
          <Field name="field2" type="1"/>
        </StructType>
      </RichType>
    </RichTypesInfo>
  )";

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError result = doc.Parse(xmlContent);
  ASSERT_EQ(result, tinyxml2::XML_SUCCESS)
      << "XML Parsing failed: " << doc.ErrorStr();

  tinyxml2::XMLElement* root = doc.FirstChildElement("RichTypesInfo");
  ASSERT_NE(root, nullptr) << "Root element 'RichTypesInfo' not found";

  try {
    BLang::readRichTypeInfos(root);
  } catch (const BLang::TypeFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }

  ASSERT_EQ(BLang::TypeFactory::size(), 7);
  ASSERT_EQ(BLang::TypeFactory::at(0)->getKind(), BLang::Type::Kind::INTEGER);
  ASSERT_EQ(BLang::TypeFactory::at(1)->getKind(), BLang::Type::Kind::BOOLEAN);
  ASSERT_EQ(BLang::TypeFactory::at(2)->getKind(), BLang::Type::Kind::PowerType);
  ASSERT_EQ(BLang::TypeFactory::at(3)->getKind(),
            BLang::Type::Kind::ProductType);
  ASSERT_EQ(BLang::TypeFactory::at(4)->getKind(),
            BLang::Type::Kind::AbstractSet);
  ASSERT_EQ(BLang::TypeFactory::at(5)->getKind(),
            BLang::Type::Kind::EnumeratedSet);
  ASSERT_EQ(BLang::TypeFactory::at(6)->getKind(), BLang::Type::Kind::Struct);

  ASSERT_EQ(BLang::TypeFactory::at(2)->toPowerType()->m_content->getKind(),
            BLang::Type::Kind::INTEGER);
  ASSERT_EQ(BLang::TypeFactory::at(3)->toProductType()->lhs->getKind(),
            BLang::Type::Kind::INTEGER);
  ASSERT_EQ(BLang::TypeFactory::at(3)->toProductType()->rhs->getKind(),
            BLang::Type::Kind::BOOLEAN);
  ASSERT_EQ(BLang::TypeFactory::at(4)->toAbstractSetType()->getName(),
            "MyAbstractSet");
  ASSERT_EQ(BLang::TypeFactory::at(5)->toEnumeratedSetType()->getName(),
            "MyEnumSet");
  ASSERT_EQ(BLang::TypeFactory::at(5)->toEnumeratedSetType()->getValues(),
            std::vector<std::string>({"Value1", "Value2"}));
  ASSERT_EQ(BLang::TypeFactory::at(6)->toStructType()->getFields().size(), 2);
  ASSERT_EQ(BLang::TypeFactory::at(6)->toStructType()->getFields()[0].first,
            "field1");
  ASSERT_EQ(BLang::TypeFactory::at(6)
                ->toStructType()
                ->getFields()[0]
                .second->getKind(),
            BLang::Type::Kind::INTEGER);
  ASSERT_EQ(BLang::TypeFactory::at(6)->toStructType()->getFields()[1].first,
            "field2");
  ASSERT_EQ(BLang::TypeFactory::at(6)
                ->toStructType()
                ->getFields()[1]
                .second->getKind(),
            BLang::Type::Kind::BOOLEAN);
}

TEST_F(BLangTinyXml2ReaderTest, ReadExpression) {
  const char* xmlContent = R"(
  <Expressions>
    <Boolean_Literal value="TRUE"/>
    <Boolean_Literal value="FALSE"/>
    <Boolean_Exp>
      <Exp_Comparison op="=">
        <Boolean_Literal value="FALSE"/>
        <Boolean_Literal value="TRUE"/>
      </Exp_Comparison>
    </Boolean_Exp>
    <Integer_Literal value="42"/>
    <Id value="foo"/>
  </Expressions>
  )";

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError result = doc.Parse(xmlContent);
  ASSERT_EQ(result, tinyxml2::XML_SUCCESS)
      << "XML Parsing failed: " << doc.ErrorStr();

  tinyxml2::XMLElement* root = doc.FirstChildElement();
  ASSERT_NE(root, nullptr) << "Root element not found";

  int count = 1;
  tinyxml2::XMLElement* elem1 = root->FirstChildElement();
  ASSERT_NE(elem1, nullptr) << "Child element " << count << " not found";
  std::shared_ptr<BLang::Expression> expr1;
  try {
    expr1 = BLang::readExpression(elem1);
  } catch (const BLang::ExpressionFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }
  ASSERT_EQ(expr1, BLang::ExpressionFactory::TRUE());

  ++count;
  tinyxml2::XMLElement* elem2 = elem1->NextSiblingElement();
  ASSERT_NE(elem1, nullptr) << "Child element " << count << " not found";
  std::shared_ptr<BLang::Expression> expr2;
  try {
    expr2 = BLang::readExpression(elem2);
  } catch (const BLang::ExpressionFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }
  ASSERT_EQ(expr2, BLang::ExpressionFactory::FALSE());

  ++count;
  tinyxml2::XMLElement* elem3 = elem2->NextSiblingElement();
  ASSERT_NE(elem1, nullptr) << "Child element " << count << " not found";
  std::shared_ptr<BLang::Expression> expr3;
  try {
    expr3 = BLang::readExpression(elem3);
  } catch (const BLang::ExpressionFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }
  ASSERT_EQ(expr3, BLang::ExpressionFactory::ConversionBool(
                       BLang::PredicateFactory::Equality(
                           BLang::ExpressionFactory::FALSE(),
                           BLang::ExpressionFactory::TRUE())));

  ++count;
  tinyxml2::XMLElement* elem4 = elem3->NextSiblingElement();
  ASSERT_NE(elem1, nullptr) << "Child element " << count << " not found";
  std::shared_ptr<BLang::Expression> expr4;
  try {
    expr4 = BLang::readExpression(elem4);
  } catch (const BLang::ExpressionFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }
  ASSERT_EQ(expr4, BLang::ExpressionFactory::IntegerLiteral("42"));

  ++count;
  tinyxml2::XMLElement* elem5 = elem4->NextSiblingElement();
  ASSERT_NE(elem1, nullptr) << "Child element " << count << " not found";
  std::shared_ptr<BLang::Expression> expr5;
  try {
    expr5 = BLang::readExpression(elem5);
  } catch (const BLang::ExpressionFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }
  ASSERT_EQ(expr5, BLang::ExpressionFactory::Data("foo"));
}

TEST_F(BLangTinyXml2ReaderTest, ReadPredicate) {
  const char* xmlContent = R"(
<Nary_Pred op="&amp;">
  <Unary_Pred op="not">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Unary_Pred>
  <Nary_Pred op="or">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Nary_Pred>
  <Nary_Pred op="&amp;">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Nary_Pred>
  <Binary_Pred op="=>">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Binary_Pred>
  <Binary_Pred op="&lt;=>">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Binary_Pred>
  <Unary_Pred op="not">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Unary_Pred>
  <Nary_Pred op="or">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Nary_Pred>
  <Nary_Pred op="&amp;">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Nary_Pred>
  <Binary_Pred op="=>">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Binary_Pred>
  <Binary_Pred op="&lt;=>">
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Boolean_Literal value="TRUE"/>
      <Boolean_Literal value="FALSE"/>
    </Exp_Comparison>
  </Binary_Pred>
</Nary_Pred>
)";

  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError result = doc.Parse(xmlContent);
  ASSERT_EQ(result, tinyxml2::XML_SUCCESS)
      << "XML Parsing failed: " << doc.ErrorStr();

  tinyxml2::XMLElement* root = doc.FirstChildElement();
  ASSERT_NE(root, nullptr) << "Root element not found";

  std::shared_ptr<BLang::Predicate> predicate;
  try {
    predicate = BLang::readPredicate(root);
  } catch (const BLang::PredicateFactory::Exception& e) {
    FAIL() << "Exception during XML build: " << e.what();
  }
  ASSERT_NE(predicate, nullptr) << "Predicate is null";
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Conjunction);

  auto conj = predicate->toConjunction();
  ASSERT_NE(conj, nullptr) << "Conjunction is null";
  ASSERT_EQ(conj->args().size(), 10) << "Conjunction args size mismatch";

  predicate = conj->args()[0];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Negation)
      << "First arg is not Negation";
  auto neg1 = predicate->toNegation();

  predicate = conj->args()[1];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Disjunction)
      << "Second arg is not Disjunction";
  auto disj1 = predicate->toDisjunction();

  predicate = conj->args()[2];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Conjunction)
      << "Third arg is not Conjunction";
  auto conj1 = predicate->toConjunction();

  predicate = conj->args()[3];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Implication)
      << "Fourth arg is not Implication";
  auto impl1 = predicate->toImplication();

  predicate = conj->args()[4];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Equivalence)
      << "Fifth arg is not Equivalence";
  auto equiv1 = predicate->toEquivalence();

  predicate = conj->args()[5];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Negation)
      << "Sixth arg is not Negation";
  auto neg2 = predicate->toNegation();

  predicate = conj->args()[6];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Disjunction)
      << "Seventh arg is not Disjunction";
  auto disj2 = predicate->toDisjunction();

  predicate = conj->args()[7];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Conjunction)
      << "Eighth arg is not Conjunction";
  auto conj2 = predicate->toConjunction();

  predicate = conj->args()[8];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Implication)
      << "Ninth arg is not Implication";
  auto impl2 = predicate->toImplication();

  predicate = conj->args()[9];
  ASSERT_EQ(predicate->getKind(), BLang::Predicate::Kind::Equivalence)
      << "Tenth arg is not Equivalence";
  auto equiv2 = predicate->toEquivalence();

  ASSERT_EQ(neg1, neg2) << "Negations mismatch";
  ASSERT_EQ(disj1, disj2) << "Disjunctions mismatch";
  ASSERT_EQ(conj1, conj2) << "Conjunctions mismatch";
  ASSERT_EQ(impl1, impl2) << "Implications mismatch";
  ASSERT_EQ(equiv1, equiv2) << "Equivalences mismatch";

  ASSERT_EQ(neg1->operand()->getKind(), BLang::Predicate::Kind::Equality)
      << "Negation operand is not Equality";
  auto eq1 = neg1->operand()->toEquality();
  ASSERT_EQ(eq1->lhs()->getKind(), BLang::Expression::Kind::TRUE)
      << "Equality lhs is not TRUE";
  ASSERT_EQ(eq1->rhs()->getKind(), BLang::Expression::Kind::FALSE)
      << "Equality rhs is not FALSE";
  ASSERT_EQ(disj1->args().size(), 3) << "Disjunction args size mismatch";
  ASSERT_EQ(disj1->args()[0], eq1) << "Disjunction arg 1 is not Equality";
  ASSERT_EQ(disj1->args()[1], eq1) << "Disjunction arg 2 is not Equality";
  ASSERT_EQ(disj1->args()[2], eq1) << "Disjunction arg 3 is not Equality";
  ASSERT_EQ(conj1->args().size(), 3) << "Conjunction args size mismatch";
  ASSERT_EQ(conj1->args()[0], eq1) << "Conjunction arg 1 is not Equality";
  ASSERT_EQ(conj1->args()[1], eq1) << "Conjunction arg 2 is not Equality";
  ASSERT_EQ(conj1->args()[2], eq1) << "Conjunction arg 3 is not Equality";
  ASSERT_EQ(impl1->lhs(), eq1) << "Implication lhs is not Equality";
  ASSERT_EQ(impl1->rhs(), eq1) << "Implication rhs is not Equality";
  ASSERT_EQ(equiv1->lhs(), eq1) << "Equivalence lhs is not Equality";
  ASSERT_EQ(equiv1->rhs(), eq1) << "Equivalence rhs is not Equality";
}
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
