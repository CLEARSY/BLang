/* @file blang_xml_writer_tests.cpp
   @brief Unit tests for BLang XML writer methods.

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
#include "blang_type.h"
#include "blang_xml_writer.h"

class BLangXMLWriterTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

// Test case for BFactory::writeXMLRichTypesInfo
TEST_F(BLangXMLWriterTest, WriteXMLRichTypesInfo) {
  // Create various types
  auto intType = BLang::TypeFactory::Integer();
  auto boolType = BLang::TypeFactory::Boolean();
  auto floatType = BLang::TypeFactory::Float();
  auto realType = BLang::TypeFactory::Real();
  auto stringType = BLang::TypeFactory::String();
  auto productType = BLang::TypeFactory::Product(intType, boolType);
  auto powerSetType = BLang::TypeFactory::PowerSet(intType);
  auto abstractSetType = BLang::TypeFactory::AbstractSet("MySet");
  std::vector<std::string> enumValues = {"One", "Two", "Three"};
  auto enumSetType = BLang::TypeFactory::EnumeratedSet("Colors", enumValues);
  std::vector<std::pair<std::string, std::shared_ptr<BLang::Type>>> fields = {
      {"field1", intType}, {"field2", boolType}};
  auto structType = BLang::TypeFactory::Struct(fields);

  // Generate XML
  std::ostringstream os;
  BLang::writeXMLRichTypesInfo(os);
  std::string xmlOutput = os.str();

  // Expected XML structure (simplified for brevity)
  std::string expectedXMLStart = "<RichTypesInfo>\n";
  std::string expectedXMLEnd = "</RichTypesInfo>\n";

  // Check if the generated XML contains the expected structure
  EXPECT_TRUE(xmlOutput.find(expectedXMLStart) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(expectedXMLEnd) != std::string::npos);

  // Additional checks for specific types can be added here
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"0\">\n    <INTEGER/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"1\">\n    <BOOL/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"2\">\n    <FLOAT/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"3\">\n    <REAL/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find("<RichType id=\"4\">\n    <STRING/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(xmlOutput.find("<RichType id=\"5\">\n    <CartesianProduct "
                             "arg1=\"0\" arg2=\"1\"/>\n  </RichType>\n") !=
              std::string::npos);
  EXPECT_TRUE(
      xmlOutput.find(
          "<RichType id=\"6\">\n    <PowerSet arg=\"0\"/>\n  </RichType>\n") !=
      std::string::npos);
  EXPECT_TRUE(xmlOutput.find("<RichType id=\"7\">\n    <AbstractSet "
                             "name=\"MySet\"/>\n  </RichType>\n") !=
              std::string::npos);
  EXPECT_TRUE(xmlOutput.find(
                  "<RichType id=\"8\">\n    <EnumeratedSet name=\"Colors\">\n  "
                  "    <EnumeratedValue name=\"One\"/>\n      <EnumeratedValue "
                  "name=\"Two\"/>\n      <EnumeratedValue name=\"Three\"/>\n   "
                  " </EnumeratedSet>\n  </RichType>\n") != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(
                  "<RichType id=\"9\">\n    <StructType>\n      <Field "
                  "name=\"field1\" type=\"0\"/>\n      <Field name=\"field2\" "
                  "type=\"1\"/>\n    </StructType>\n  </RichType>\n") !=
              std::string::npos);
}

TEST_F(BLangXMLWriterTest, WriteXMLExpression) {
  std::ostringstream os;
  std::string xmlOutput;

  auto trueExp = BLang::ExpressionFactory::TRUE();

  BLang::writeXML(os, trueExp);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find("<Boolean_Literal value=\"TRUE\"/>") !=
              std::string::npos);
  os.str(std::string());

  auto falseExp = BLang::ExpressionFactory::FALSE();
  BLang::writeXML(os, falseExp);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find("<Boolean_Literal value=\"FALSE\"/>") !=
              std::string::npos);
  os.str(std::string());

  auto conversionBoolExp = BLang::ExpressionFactory::ConversionBool(
      BLang::PredicateFactory::Equality(trueExp, falseExp));
  BLang::writeXML(os, conversionBoolExp);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find(R"(<Boolean_Exp>)") != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(R"(</Boolean_Exp>)") != std::string::npos);

  auto integerExp = BLang::ExpressionFactory::IntegerLiteral("42");
  BLang::writeXML(os, integerExp);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find("<Integer_Literal value=\"42\"/>") !=
              std::string::npos);
  os.str(std::string());
}

TEST_F(BLangXMLWriterTest, WriteXMLPredicate) {
  // Generate XML
  std::ostringstream os;
  std::string xmlOutput;

  const std::string closeNary = R"(</Nary_Pred>)";
  const std::string closeBinary = R"(</Binary_Pred>)";
  const std::string closeUnary = R"(</Unary_Pred>)";

  auto trueExp = BLang::ExpressionFactory::TRUE();
  auto falseExp = BLang::ExpressionFactory::FALSE();
  auto equalityPred = BLang::PredicateFactory::Equality(trueExp, falseExp);
  const std::string equalityXml = R"(<Binary_Pred op="=">)";
  BLang::writeXML(os, equalityPred);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find(equalityXml) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(closeBinary) != std::string::npos);
  os.str(std::string());

  auto negationPred = BLang::PredicateFactory::Negation(equalityPred);
  const std::string negationXml = R"(<Unary_Pred op="not">)";
  BLang::writeXML(os, negationPred);
  xmlOutput = os.str();
  std::cout << xmlOutput << std::endl;
  EXPECT_TRUE(xmlOutput.find(negationXml) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(closeUnary) != std::string::npos);
  os.str(std::string());

  auto conjunctionPred =
      BLang::PredicateFactory::Conjunction({equalityPred, negationPred});
  const std::string conjunctionXml = R"(<Nary_Pred op="&amp;">)";
  BLang::writeXML(os, conjunctionPred);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find(conjunctionXml) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(closeNary) != std::string::npos);
  os.str(std::string());

  auto disjunctionPred =
      BLang::PredicateFactory::Disjunction({equalityPred, negationPred});
  const std::string disjunctionXml = R"(<Nary_Pred op="or">)";
  BLang::writeXML(os, disjunctionPred);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find(disjunctionXml) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(closeNary) != std::string::npos);
  os.str(std::string());

  auto implicationPred =
      BLang::PredicateFactory::Implication(equalityPred, negationPred);
  const std::string implicationXml = R"(<Binary_Pred op="=>">)";
  BLang::writeXML(os, implicationPred);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find(implicationXml) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(closeBinary) != std::string::npos);
  os.str(std::string());

  auto equivalencePred =
      BLang::PredicateFactory::Equivalence(equalityPred, negationPred);
  const std::string equivalenceXml = R"(<Binary_Pred op="&lt;=>">)";
  BLang::writeXML(os, equivalencePred);
  xmlOutput = os.str();
  EXPECT_TRUE(xmlOutput.find(equivalenceXml) != std::string::npos);
  EXPECT_TRUE(xmlOutput.find(closeBinary) != std::string::npos);
  os.str(std::string());
}
int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
