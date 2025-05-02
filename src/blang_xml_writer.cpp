/* @file blang_xml_writer.h
   @brief Header file for the BLang XML dump functions

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
#include "blang_xml_writer.h"

#include "blang_expression.h"
#include "blang_predicate.h"
#include "blang_type.h"

namespace BLang {

/*
    <xs:choice>
    <xs:element name="BOOL" />
    <xs:element name="INTEGER" />
    <xs:element name="REAL" />
    <xs:element name="FLOAT" />
    <xs:element name="STRING" />
    <xs:element name="PowerSet" type="PowerSet" />
    <xs:element name="CartesianProduct" type="CartesianProduct" />
    <xs:element name="AbstractSet" type="AbstractSet" />
    <xs:element name="EnumeratedSet" type="EnumeratedSet" />
</xs:choice>
*/
void writeXMLRichTypesInfo(std::ostream &os) {
  std::size_t nbTypes = TypeFactory::size();
  os << "<RichTypesInfo>\n";
  for (auto i = 0u; i < nbTypes; i++) {
    auto type = TypeFactory::at(i);
    os << "  <RichType id=\"" << i << "\">\n";
    switch (type->getKind()) {
      case Type::Kind::BOOLEAN:
        os << "    <BOOL/>\n";
        break;
      case Type::Kind::INTEGER:
        os << "    <INTEGER/>\n";
        break;
      case Type::Kind::REAL:
        os << "    <REAL/>\n";
        break;
      case Type::Kind::FLOAT:
        os << "    <FLOAT/>\n";
        break;
      case Type::Kind::STRING:
        os << "    <STRING/>\n";
        break;
      case Type::Kind::PowerType:
        /*
        <xs:complexType name="PowerSet">
          <xs:attribute name="arg" type="xs:integer">
        </xs:complexType>
        */
        os << "    <PowerSet arg=\"" << type->toPowerType()->m_content->index()
           << "\"/>\n";
        break;
      case Type::Kind::ProductType:
        /*
        <xs:complexType name="CartesianProduct">
        <xs:attribute name="arg1" type="xs:integer"/>
        <xs:attribute name="arg2" type="xs:integer"/>
        </xs:complexType>
        */
        os << "    <CartesianProduct"
           << " arg1=\"" << type->toProductType()->lhs->index() << "\""
           << " arg2=\"" << type->toProductType()->rhs->index() << "\""
           << "/>\n";
        break;
      case Type::Kind::AbstractSet:
        /*
          <xs:complexType name="AbstractSet">
          <xs:attribute name="name" type="xs:string"/>
          </xs:complexType>
        */
        os << "    <AbstractSet name=\"" << type->toAbstractSetType()->getName()
           << "\"/>\n";
        break;
      case Type::Kind::EnumeratedSet:
        /*
          <xs:complexType name="EnumeratedSet">
          <xs:attribute name="name" type="xs:string"/>
          <xs:sequence>
            <xs:element name="EnumeratedValue" type="EnumeratedValue"/>
          </xs:sequence>
          </xs:complexType>
        */
        os << "    <EnumeratedSet name=\""
           << type->toEnumeratedSetType()->getName() << "\">\n";
        for (const auto &value :
             type->toEnumeratedSetType()
                 ->getValues()) {  // std::vector<std::string>
          /*
            <xs:complexType name="EnumeratedValue">
            <xs:attribute name="name" type="xs:string"/>
            </xs:complexType>
          */
          os << "      <EnumeratedValue name=\"" << value << "\"/>\n";
        }
        os << "    </EnumeratedSet>\n";
        break;
      case Type::Kind::Struct:
        /*
          <xs:complexType name="StructType">
          <xs:sequence>
            <xs:element name="Field" type="Field"/>
          </xs:sequence>
          </xs:complexType>
        */
        os << "    <StructType>\n";
        for (const auto &field :
             type->toStructType()
                 ->getFields()) {  // std::vector<std::pair<std::string,
          /*
            <xs:complexType name="Field">
            <xs:attribute name="name" type="xs:string"/>
            <xs:attribute name="type" type="xs:integer"/>
            </xs:complexType>
          */
          os << "      <Field name=\"" << field.first << "\" type=\""
             << field.second->index() << "\"/>\n";
        }
        os << "    </StructType>\n";
        break;
      case Type::Kind::Undefined:
        os << "    <Undefined/>\n";
        break;
    }
    os << "  </RichType>\n";
  }
  os << "</RichTypesInfo>\n";
}

class XMLIndent {
 public:
  explicit XMLIndent(std::ostream &os) : m_os(os), m_indent(0u) {}
  XMLIndent(const XMLIndent &) = delete;
  ~XMLIndent() = default;
  void write() {
    for (size_t i = 0; i < m_indent; ++i) {
      m_os << "  ";
    }
  }
  void increase() { ++m_indent; }
  void decrease() { --m_indent; }

 private:
  std::ostream &m_os;
  size_t m_indent = 0;
};

class XMLWriter : public Expression::Visitor, public Predicate::Visitor {
 public:
  explicit XMLWriter(std::ostream &os) : m_os(os), m_indent(os) {}
  XMLWriter(const XMLWriter &) = delete;
  ~XMLWriter() = default;
  virtual void visitTRUE() override {
    m_indent.write();
    m_os << "<Boolean_Literal value=\"TRUE\"/>\n";
  };
  virtual void visitFALSE() override {
    m_indent.write();
    m_os << "<Boolean_Literal value=\"FALSE\"/>\n";
  };
  virtual void visitConversionBool(
      const Expression::ConversionBool &p) override {
    m_indent.write();
    m_os << "<Boolean_Exp>\n";
    m_indent.increase();
    p.pred()->accept(*this);
    m_indent.decrease();
    m_indent.write();
    m_os << "</Boolean_Exp>\n";
  };
  virtual void visitIntegerLiteral(
      const Expression::IntegerLiteral &p) override {
    m_indent.write();
    m_os << "<Integer_Literal value=\"" << p.value() << "\"/>\n";
  };
  virtual void visitConjunction(const Predicate::Conjunction &p) override {
    m_indent.write();
    m_os << "<Nary_Pred op=\"&amp;\">\n";
    m_indent.increase();
    for (const auto &arg : p.args()) {
      arg->accept(*this);
    }
    m_indent.decrease();
    m_indent.write();
    m_os << "</Nary_Pred>\n";
  };
  virtual void visitDisjunction(const Predicate::Disjunction &p) override {
    m_indent.write();
    m_os << "<Nary_Pred op=\"or\">\n";
    m_indent.increase();
    for (const auto &arg : p.args()) {
      arg->accept(*this);
    }
    m_indent.decrease();
    m_indent.write();
    m_os << "</Nary_Pred>\n";
  };
  virtual void visitNegation(const Predicate::Negation &p) override {
    m_indent.write();
    m_os << "<Unary_Pred op=\"not\">\n";
    m_indent.increase();
    p.m_operand->accept(*this);
    m_indent.decrease();
    m_indent.write();
    m_os << "</Unary_Pred>\n";
  };
  virtual void visitImplication(const Predicate::Implication &p) override {
    m_indent.write();
    m_os << "<Binary_Pred op=\"=>\">\n";
    m_indent.increase();
    p.lhs()->accept(*this);
    p.rhs()->accept(*this);
    m_indent.decrease();
    m_indent.write();
    m_os << "</Binary_Pred>\n";
  };
  virtual void visitEquivalence(const Predicate::Equivalence &p) override {
    m_indent.write();
    m_os << "<Binary_Pred op=\"&lt;=>\">\n";
    m_indent.increase();
    p.lhs()->accept(*this);
    p.rhs()->accept(*this);
    m_indent.decrease();
    m_indent.write();
    m_os << "</Binary_Pred>\n";
  };
  virtual void visitEquality(const Predicate::Equality &p) override {
    m_indent.write();
    m_os << "<Binary_Pred op=\"=\">\n";
    m_indent.increase();
    p.m_lhs->accept(*this);
    p.m_rhs->accept(*this);
    m_indent.decrease();
    m_indent.write();
    m_os << "</Binary_Pred>\n";
  };

 private:
  std::ostream &m_os;
  XMLIndent m_indent;
};

void writeXML(std::ostream &os, const Expression &expr) {
  XMLWriter writer(os);
  expr.accept(writer);
  return;
}

void writeXML(std::ostream &os, const std::shared_ptr<Expression> expr) {
  if (expr) {
    writeXML(os, *expr);
  } else {
    os << "<NULL/>\n";
  }
}

void writeXML(std::ostream &os, const std::shared_ptr<Predicate> pred) {
  if (pred) {
    writeXML(os, *pred);
  } else {
    os << "<NULL/>\n";
  }
}

void writeXML(std::ostream &os, const Predicate &pred) {
  XMLWriter writer(os);
  pred.accept(writer);
  return;
}

};  // namespace BLang
