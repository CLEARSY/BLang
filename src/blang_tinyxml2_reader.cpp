/* @file blang_tinyxml2_reader.h
   @brief Header file for reading XML to BLang objects with TinyXML2

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

#include <exception>
#include <functional>
#include <vector>
using std::exception;
using std::shared_ptr;
using std::string;
using std::vector;

#include "blang_exception.h"
#include "blang_expression.h"
#include "blang_predicate.h"
#include "blang_tinyxml2_reader.h"
#include "blang_type.h"
#include "tinyxml2.h"

namespace BLang {

void readRichTypeInfos(const tinyxml2::XMLElement* root) {
  vector<const tinyxml2::XMLElement*> richTypeElements;
  vector<shared_ptr<Type>> types;
  // First pass: create placeholder for all types to handle forward references
  for (auto typeElem = root->FirstChildElement("RichType"); typeElem;
       typeElem = typeElem->NextSiblingElement("RichType")) {
    int id = -1;
    if (typeElem->QueryIntAttribute("id", &id) != tinyxml2::XML_SUCCESS ||
        id < 0) {
      throw Exception("Invalid or missing id attribute");
    }
    if (id < 0 || static_cast<size_t>(id) != richTypeElements.size()) {
      throw Exception("RichType indexing is not contiguous");
    }
    richTypeElements.push_back(typeElem);
    types.push_back(nullptr);
  }

  std::function<void(const size_t)> typeOfXmlElement;

  typeOfXmlElement = [&](const size_t pos) -> void {
    if (types.at(pos)) {
      return;
    }
    const tinyxml2::XMLElement* typeElem = richTypeElements.at(pos);
    const tinyxml2::XMLElement* typeDefElem = typeElem->FirstChildElement();
    if (!typeDefElem) {
      throw Exception("Empty RichType element");
    }
    string elemName = typeDefElem->Name();
    shared_ptr<Type> type;

    if (elemName == "BOOL") {
      type = TypeFactory::Boolean();
    } else if (elemName == "INTEGER") {
      type = TypeFactory::Integer();
    } else if (elemName == "REAL") {
      type = TypeFactory::Real();
    } else if (elemName == "FLOAT") {
      type = TypeFactory::Float();
    } else if (elemName == "STRING") {
      type = TypeFactory::String();
    } else if (elemName == "PowerSet") {
      int argId = -1;
      if (typeDefElem->QueryIntAttribute("arg", &argId) !=
              tinyxml2::XML_SUCCESS ||
          argId < 0 || static_cast<size_t>(argId) >= types.size()) {
        throw Exception("Invalid PowerSet arg reference");
      }
      typeOfXmlElement(argId);
      type = TypeFactory::PowerSet(types.at(argId));
    } else if (elemName == "CartesianProduct") {
      int arg1Id = -1, arg2Id = -1;
      if (typeDefElem->QueryIntAttribute("arg1", &arg1Id) !=
              tinyxml2::XML_SUCCESS ||
          typeDefElem->QueryIntAttribute("arg2", &arg2Id) !=
              tinyxml2::XML_SUCCESS ||
          arg1Id < 0 || static_cast<size_t>(arg1Id) >= types.size() ||
          arg2Id < 0 || static_cast<size_t>(arg2Id) >= types.size()) {
        throw Exception("Invalid CartesianProduct arg references");
      }
      typeOfXmlElement(arg1Id);
      typeOfXmlElement(arg2Id);
      type = TypeFactory::Product(types.at(arg1Id), types.at(arg2Id));
    } else if (elemName == "AbstractSet") {
      const char* name = typeDefElem->Attribute("name");
      if (!name) {
        throw Exception("Missing AbstractSet name attribute");
      }
      type = TypeFactory::AbstractSet(name);
    } else if (elemName == "EnumeratedSet") {
      const char* name = typeDefElem->Attribute("name");
      if (!name) {
        throw Exception("Missing EnumeratedSet name attribute");
      }
      std::vector<std::string> values;
      for (auto valueElem = typeDefElem->FirstChildElement("EnumeratedValue");
           valueElem;
           valueElem = valueElem->NextSiblingElement("EnumeratedValue")) {
        const char* valueName = valueElem->Attribute("name");
        if (!valueName) {
          throw Exception("Missing EnumeratedValue name attribute");
        }
        values.push_back(valueName);
      }
      type = TypeFactory::EnumeratedSet(name, values);
    } else if (elemName == "StructType") {
      vector<std::pair<string, shared_ptr<Type>>> fields;
      for (auto fieldElem = typeDefElem->FirstChildElement("Field"); fieldElem;
           fieldElem = fieldElem->NextSiblingElement("Field")) {
        const char* fieldName = fieldElem->Attribute("name");
        int fieldTypeId = -1;
        if (!fieldName ||
            fieldElem->QueryIntAttribute("type", &fieldTypeId) !=
                tinyxml2::XML_SUCCESS ||
            fieldTypeId < 0 ||
            static_cast<size_t>(fieldTypeId) >= types.size()) {
          throw Exception("Invalid Struct field definition");
        }
        typeOfXmlElement(fieldTypeId);
        fields.emplace_back(fieldName, types.at(fieldTypeId));
      }
      type = TypeFactory::Struct(fields);
    } else {
      throw Exception("Unknown type element: " + elemName);
    }
    types[pos] = type;
  };

  for (auto i = 0u; i < richTypeElements.size(); i++) {
    typeOfXmlElement(i);
  }

  // Verify no gaps in the type vector
  for (size_t i = 0; i < types.size(); ++i) {
    if (!types[i]) {
      throw Exception("Missing type definition for id " + std::to_string(i));
    }
  }
}

shared_ptr<Expression> readExpression(const tinyxml2::XMLElement* root) {
  if (!root) {
    throw Exception("Null root element");
  }
  if (strcmp(root->Name(), "Boolean_Literal") == 0) {
    const char* value = root->Attribute("value");
    if (!value) {
      throw Exception("Missing value attribute");
    }
    if (strcmp(value, "TRUE") == 0) {
      return ExpressionFactory::TRUE();
    } else if (strcmp(value, "FALSE") == 0) {
      return ExpressionFactory::FALSE();
    } else {
      throw Exception("Invalid value attribute: " + std::string(value));
    }
  } else if (strcmp(root->Name(), "Boolean_Exp") == 0) {
    const tinyxml2::XMLElement* predElem = root->FirstChildElement();
    if (predElem == nullptr) {
      throw Exception("Empty Boolean_Exp element");
    }
    std::shared_ptr<Predicate> predicate = readPredicate(predElem);
    if (predicate == nullptr) {
      throw Exception("Failed to read predicate from Boolean_Exp");
    }
    return ExpressionFactory::ConversionBool(predicate);
  } else if (strcmp(root->Name(), "Integer_Literal") == 0) {
    const char* value = root->Attribute("value");
    if (!value) {
      throw Exception("Missing value attribute");
    }
    return ExpressionFactory::IntegerLiteral(value);
  } else if (strcmp(root->Name(), "Boolean_Exp") == 0) {
    const tinyxml2::XMLElement* predElem = root->FirstChildElement();
    if (predElem == nullptr) {
      throw Exception("Empty Boolean_Exp element");
    }
    std::shared_ptr<Predicate> predicate = readPredicate(predElem);
    if (predicate == nullptr) {
      throw Exception("Failed to read predicate from Boolean_Exp");
    }
    return ExpressionFactory::ConversionBool(predicate);
  } else {
    throw Exception("Unknown root element: " + std::string(root->Name()));
  }
}

shared_ptr<Predicate> readPredicate(const tinyxml2::XMLElement* root) {
  if (!root) {
    throw Exception("Null root element");
  }
  const char* op = root->Attribute("op");
  if (!op) {
    throw Exception("Missing op attribute");
  }
  if (strcmp(root->Name(), "Nary_Pred") == 0) {
    if (!(strcmp(op, "&") == 0 || strcmp(op, "or") == 0)) {
      throw Exception("Unknown n-ary predicate operator: " + std::string(op));
    } else {
      std::vector<std::shared_ptr<Predicate>> predicates;
      for (auto predElem = root->FirstChildElement(); predElem;
           predElem = predElem->NextSiblingElement()) {
        predicates.push_back(readPredicate(predElem));
      }
      if (strcmp(op, "&") == 0) {
        return PredicateFactory::Conjunction(predicates);
      } else {
        return PredicateFactory::Disjunction(predicates);
      }
    }
  } else if (strcmp(root->Name(), "Binary_Pred") == 0) {
    if (!(strcmp(op, "=>") == 0 || strcmp(op, "<=>") == 0)) {
      throw Exception("Unknown binary predicate operator: " + std::string(op));
    } else {
      std::shared_ptr<Predicate> lhs = readPredicate(root->FirstChildElement());
      std::shared_ptr<Predicate> rhs =
          readPredicate(root->FirstChildElement()->NextSiblingElement());
      if (strcmp(op, "=>") == 0) {
        return PredicateFactory::Implication(lhs, rhs);
      } else {
        return PredicateFactory::Equivalence(lhs, rhs);
      }
    }
  } else if (strcmp(root->Name(), "Unary_Pred") == 0) {
    if (!(strcmp(op, "not") == 0)) {
      throw Exception("Unknown unary predicate operator: " + std::string(op));
    } else {
      return PredicateFactory::Negation(
          readPredicate(root->FirstChildElement()));
    }
  } else if (strcmp(root->Name(), "Exp_Comparison") == 0) {
    if (!(strcmp(op, "=") == 0)) {
      throw Exception("Unknown expression comparison operator: " +
                      std::string(op));
    } else {
      std::shared_ptr<Expression> lhs =
          readExpression(root->FirstChildElement());
      std::shared_ptr<Expression> rhs =
          readExpression(root->FirstChildElement()->NextSiblingElement());
      // if (strcmp(op, "=") == 0) {
      return PredicateFactory::Equality(lhs, rhs);
    }
  } else {
    throw Exception("Unknown root element: " + std::string(root->Name()));
  }
}

};  // namespace BLang
