/* @file blang_predicate.cpp
   @brief Implementation file for the Predicate class and its nested classes.

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
#include "blang_predicate.h"

#include <unordered_map>

#include "blang_expression.h"
#include "blang_hash.h"

namespace BLang {

// Type conversion methods
std::shared_ptr<const Predicate::Conjunction> Predicate::toConjunction() const {
  if (m_kind != Kind::Conjunction) return nullptr;
  return std::dynamic_pointer_cast<const Conjunction>(this->shared_from_this());
}
std::shared_ptr<const Predicate::Disjunction> Predicate::toDisjunction() const {
  if (m_kind != Kind::Disjunction) return nullptr;
  return std::dynamic_pointer_cast<const Disjunction>(this->shared_from_this());
}
std::shared_ptr<const Predicate::Negation> Predicate::toNegation() const {
  if (m_kind != Kind::Negation) return nullptr;
  return std::dynamic_pointer_cast<const Negation>(this->shared_from_this());
}
std::shared_ptr<const Predicate::Implication> Predicate::toImplication() const {
  if (m_kind != Kind::Implication) return nullptr;
  return std::dynamic_pointer_cast<const Implication>(this->shared_from_this());
}
std::shared_ptr<const Predicate::Equivalence> Predicate::toEquivalence() const {
  if (m_kind != Kind::Equivalence) return nullptr;
  return std::dynamic_pointer_cast<const Equivalence>(this->shared_from_this());
}
std::shared_ptr<const Predicate::Equality> Predicate::toEquality() const {
  if (m_kind != Kind::Equality) return nullptr;
  return std::dynamic_pointer_cast<const Equality>(this->shared_from_this());
}

int Predicate::compare(const Predicate& v1, const Predicate& v2) {
  size_t hash1 = v1.hash_combine(0);
  size_t hash2 = v2.hash_combine(0);
  if (hash1 < hash2) return -1;
  if (hash1 > hash2) return 1;
  return 0;
}

size_t Predicate::hash_combine(size_t seed) const {
  switch (m_kind) {
    case Kind::Conjunction:
      return toConjunction()->hash_combine(seed);
    case Kind::Disjunction:
      return toDisjunction()->hash_combine(seed);
    case Kind::Negation:
      return toNegation()->hash_combine(seed);
    case Kind::Implication:
      return toImplication()->hash_combine(seed);
    case Kind::Equivalence:
      return toEquivalence()->hash_combine(seed);
    case Kind::Equality:
      return toEquality()->hash_combine(seed);
  }
  // Should never reach here
  return seed;
}

size_t Predicate::Nary::hash_combine(size_t seed) const {
  size_t hash = seed;
  for (const auto& arg : m_args) {
    hash = arg->hash_combine(hash);
  }
  return hash;
}

size_t Predicate::Binary::hash_combine(size_t seed) const {
  return m_lhs->hash_combine(m_rhs->hash_combine(seed));
}

size_t Predicate::Conjunction::hash_combine(size_t seed) const {
  static const size_t opHash = std::hash<std::string_view>{}("&");
  return hash_combine_size_t(opHash, Nary::hash_combine(seed));
}
size_t Predicate::Disjunction::hash_combine(size_t seed) const {
  static const size_t opHash = std::hash<std::string_view>{}("or");
  return hash_combine_size_t(opHash, Nary::hash_combine(seed));
}
size_t Predicate::Negation::hash_combine(size_t seed) const {
  static const size_t opHash = std::hash<std::string_view>{}("not");
  return hash_combine_size_t(opHash, m_operand->hash_combine(seed));
}
size_t Predicate::Implication::hash_combine(size_t seed) const {
  static const size_t opHash = std::hash<std::string_view>{}("=>");
  return hash_combine_size_t(opHash, Binary::hash_combine(seed));
}
size_t Predicate::Equivalence::hash_combine(size_t seed) const {
  static const size_t opHash = std::hash<std::string_view>{}("<=>");
  return hash_combine_size_t(opHash, Binary::hash_combine(seed));
}
size_t Predicate::Equality::hash_combine(size_t seed) const {
  static const size_t opHash = std::hash<std::string_view>{}("=");
  return hash_combine_size_t(opHash,
                             m_rhs->hash_combine(m_lhs->hash_combine(seed)));
}

// Definition of the virtual accept function
void Predicate::accept(Visitor& v) const {
  switch (m_kind) {
    case Kind::Conjunction:
      toConjunction()->accept(v);
      break;
    case Kind::Disjunction:
      toDisjunction()->accept(v);
      break;
    case Kind::Negation:
      toNegation()->accept(v);
      break;
    case Kind::Implication:
      toImplication()->accept(v);
      break;
    case Kind::Equivalence:
      toEquivalence()->accept(v);
      break;
    case Kind::Equality:
      toEquality()->accept(v);
      break;
  }
}

}  // namespace BLang