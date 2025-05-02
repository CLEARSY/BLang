/* @file blang_expression.cpp
   @brief Implementation file for the Expression class and its nested classes.

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
#include "blang_expression.h"

#include "blang_hash.h"
#include "blang_predicate.h"

namespace BLang {

// Expression conversion methods

std::shared_ptr<const Expression::ConversionBool> Expression::toConversionBool()
    const {
  if (m_kind != Kind::ConversionBool) return nullptr;
  return std::dynamic_pointer_cast<const ConversionBool>(
      this->shared_from_this());
}

std::shared_ptr<const Expression::IntegerLiteral> Expression::toIntegerLiteral()
    const {
  if (m_kind != Kind::IntegerLiteral) return nullptr;
  return std::dynamic_pointer_cast<const IntegerLiteral>(
      this->shared_from_this());
}

int Expression::compare(const Expression& v1, const Expression& v2) {
  size_t hash1 = v1.hash_combine(0);
  size_t hash2 = v2.hash_combine(0);
  if (hash1 < hash2) return -1;
  if (hash1 > hash2) return 1;
  return 0;
}

static const size_t trueHash = std::hash<std::string_view>{}("TRUE");
static const size_t falseHash = std::hash<std::string_view>{}("FALSE");

size_t Expression::hash_combine(size_t seed) const {
  switch (m_kind) {
    case Kind::TRUE:
      return hash_combine_size_t(trueHash, seed);
    case Kind::FALSE:
      return hash_combine_size_t(falseHash, seed);
    case Kind::ConversionBool:
      return toConversionBool()->hash_combine(seed);
    case Kind::IntegerLiteral:
      return toIntegerLiteral()->hash_combine(seed);
  }
  // Should never reach here
  return seed;
}

size_t Expression::ConversionBool::hash_combine(size_t seed) const {
  return hash_combine_size_t(m_pred->hash_combine(seed), seed);
}

size_t Expression::IntegerLiteral::hash_combine(size_t seed) const {
  return hash_combine_string(m_value, seed);
}

// Definition of the virtual accept function
void Expression::accept(Visitor& v) const {
  switch (m_kind) {
    case Kind::TRUE:
      v.visitTRUE();
      break;
    case Kind::FALSE:
      v.visitFALSE();
      break;
    case Kind::ConversionBool:
      toConversionBool()->accept(v);
      break;
    case Kind::IntegerLiteral:
      toIntegerLiteral()->accept(v);
      break;
  }
}

};  // namespace BLang