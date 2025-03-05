/* @file blang_type.cpp
   @brief Implementation file for the Type class and its nested classes.

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
#include "blang_type.h"

#include <unordered_map>

#include "blang_hash.h"

namespace BLang {

// Type conversion methods
std::shared_ptr<const Type::ProductType> Type::toProductType() const {
  if (m_kind != Kind::ProductType) return nullptr;
  return std::dynamic_pointer_cast<const ProductType>(this->shared_from_this());
}

std::shared_ptr<const Type::PowerType> Type::toPowerType() const {
  if (m_kind != Kind::PowerType) return nullptr;
  return std::dynamic_pointer_cast<const PowerType>(this->shared_from_this());
}

std::shared_ptr<const Type::AbstractSet> Type::toAbstractSetType() const {
  if (m_kind != Kind::AbstractSet) return nullptr;
  return std::dynamic_pointer_cast<const AbstractSet>(shared_from_this());
}

std::shared_ptr<const Type::EnumeratedSet> Type::toEnumeratedSetType() const {
  if (m_kind != Kind::EnumeratedSet) return nullptr;
  return std::dynamic_pointer_cast<const EnumeratedSet>(shared_from_this());
}

std::shared_ptr<const Type::StructType> Type::toStructType() const {
  if (m_kind != Kind::Struct) return nullptr;
  return std::dynamic_pointer_cast<const StructType>(shared_from_this());
}

int Type::compare(const Type& v1, const Type& v2) {
  size_t hash1 = v1.hash_combine(0);
  size_t hash2 = v2.hash_combine(0);
  if (hash1 < hash2) return -1;
  if (hash1 > hash2) return 1;
  return 0;
}

size_t Type::hash_combine(size_t seed) const {
  switch (m_kind) {
    case Kind::INTEGER:
      return hash_combine_string("INTEGER", seed);
    case Kind::BOOLEAN:
      return hash_combine_string("BOOLEAN", seed);
    case Kind::FLOAT:
      return hash_combine_string("FLOAT", seed);
    case Kind::REAL:
      return hash_combine_string("REAL", seed);
    case Kind::STRING:
      return hash_combine_string("STRING", seed);
    case Kind::ProductType:
      return toProductType()->hash_combine(seed);
    case Kind::PowerType:
      return toPowerType()->hash_combine(seed);
    case Kind::AbstractSet:
      return toAbstractSetType()->hash_combine(seed);
    case Kind::EnumeratedSet:
      return toEnumeratedSetType()->hash_combine(seed);
    case Kind::Struct:
      return toStructType()->hash_combine(seed);
    case Kind::Undefined:
      return hash_combine_string("?", seed);
  }
  // Should never reach here
  return seed;
}

size_t Type::ProductType::hash_combine(size_t seed) const {
  return lhs->hash_combine(rhs->hash_combine(seed));
}

size_t Type::PowerType::hash_combine(size_t seed) const {
  static const size_t opHash = std::hash<std::string_view>{}("POW");
  return hash_combine_size_t(opHash, m_content->hash_combine(seed));
}

size_t Type::AbstractSet::hash_combine(size_t seed) const {
  return hash_combine_string(m_name, seed);
}

size_t Type::EnumeratedSet::hash_combine(size_t seed) const {
  return hash_combine_string(m_name, seed);
}

size_t Type::StructType::hash_combine(size_t seed) const {
  size_t res = seed;
  for (auto& p : m_fields)
    res = hash_combine_string(p.first, p.second->hash_combine(res));
  return res;
}

std::vector<std::pair<std::string, std::shared_ptr<Type>>>
Type::StructType::sort(
    const std::vector<std::pair<std::string, std::shared_ptr<Type>>>& fields) {
  auto sorted_fields = fields;
  std::sort(sorted_fields.begin(), sorted_fields.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
  return sorted_fields;
}

// Definition of the virtual accept function
void Type::accept(Visitor& v) const {
  switch (m_kind) {
    case Kind::INTEGER:
      v.visitINTEGER();
      break;
    case Kind::BOOLEAN:
      v.visitBOOLEAN();
      break;
    case Kind::FLOAT:
      v.visitFLOAT();
      break;
    case Kind::REAL:
      v.visitREAL();
      break;
    case Kind::STRING:
      v.visitSTRING();
      break;
    case Kind::ProductType:
      toProductType()->accept(v);
      break;
    case Kind::PowerType:
      toPowerType()->accept(v);
      break;
    case Kind::AbstractSet:
      toAbstractSetType()->accept(v);
      break;
    case Kind::EnumeratedSet:
      toEnumeratedSetType()->accept(v);
      break;
    case Kind::Struct:
      toStructType()->accept(v);
      break;
    case Kind::Undefined:
      v.visitUndefinedType();
      break;
  }
}

}  // namespace BLang
