/* @file blang_predicate_factory.cpp
   @brief Implementation file for most of BLang::PredicateFactory class.

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

#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "blang_expression.h"
#include "blang_predicate.h"

namespace BLang {

using std::pair;
using std::shared_mutex;
using std::shared_ptr;
using std::string;
using std::vector;
using readlock = std::shared_lock<shared_mutex>;
using writelock = std::unique_lock<shared_mutex>;
using std::unordered_map;

// Hash functions for complex types
struct BinaryHash {
  size_t operator()(
      const pair<shared_ptr<Predicate>, shared_ptr<Predicate>>& p) const {
    return p.first->hash_combine(p.second->hash_combine(0));
  }
};

struct NaryHash {
  size_t operator()(const vector<shared_ptr<Predicate>>& args) const {
    size_t seed = 0;
    for (const auto& arg : args) {
      seed = arg->hash_combine(seed);
    }
    return seed;
  }
};

struct UnaryHash {
  size_t operator()(const shared_ptr<Predicate>& p) const {
    return p->hash_combine(0);
  }
};

struct EqualityHash {
  size_t operator()(
      const pair<shared_ptr<Expression>, shared_ptr<Expression>>& p) const {
    return p.first->hash_combine(p.second->hash_combine(0));
  }
};

// Thread-safe type caches
class PredicateCache {
 private:
  mutable shared_mutex m_mutexIndex;
  mutable shared_mutex m_mutexConjunction;
  mutable shared_mutex m_mutexDisjunction;
  mutable shared_mutex m_mutexNegation;
  mutable shared_mutex m_mutexImplication;
  mutable shared_mutex m_mutexEquivalence;
  mutable shared_mutex m_mutexEquality;

  unordered_map<vector<shared_ptr<Predicate>>,
                shared_ptr<Predicate::Conjunction>, NaryHash>
      m_conjunctions;

  unordered_map<vector<shared_ptr<Predicate>>,
                shared_ptr<Predicate::Disjunction>, NaryHash>
      m_disjunctions;

  unordered_map<shared_ptr<Predicate>, shared_ptr<Predicate::Negation>,
                UnaryHash>
      m_negations;

  unordered_map<pair<shared_ptr<Predicate>, shared_ptr<Predicate>>,
                shared_ptr<Predicate::Implication>, BinaryHash>
      m_implications;

  unordered_map<pair<shared_ptr<Predicate>, shared_ptr<Predicate>>,
                shared_ptr<Predicate::Equivalence>, BinaryHash>
      m_equivalences;

  unordered_map<pair<shared_ptr<Expression>, shared_ptr<Expression>>,
                shared_ptr<Predicate::Equality>, EqualityHash>
      m_equalities;

  std::vector<shared_ptr<Predicate>> m_index;

  void index(shared_ptr<Predicate> predicate) {
    writelock lock(m_mutexIndex);
    predicate->m_index = m_index.size();
    m_index.push_back(predicate);
  }

 public:
  PredicateCache() {}

  size_t size() const {
    readlock lock(m_mutexIndex);
    return m_index.size();
  }

  shared_ptr<Predicate> at(size_t index) const {
    readlock lock(m_mutexIndex);
    return m_index[index];
  }

  shared_ptr<Predicate> getOrCreateConjunction(
      const vector<shared_ptr<Predicate>>& args) {
    const auto& key = args;
    {
      readlock lock(m_mutexConjunction);
      auto it = m_conjunctions.find(key);
      if (it != m_conjunctions.end()) {
        return it->second;
      }
    }
    shared_ptr<Predicate::Conjunction> newPredicate;
    {
      writelock lock(m_mutexConjunction);
      auto it = m_conjunctions.find(key);
      if (it != m_conjunctions.end()) {
        return it->second;
      }
      newPredicate = std::make_shared<Predicate::Conjunction>(args);
      m_conjunctions[key] = newPredicate;
    }
    index(newPredicate);
    return newPredicate;
  }

  shared_ptr<Predicate> getOrCreateDisjunction(
      const vector<shared_ptr<Predicate>>& args) {
    const auto& key = args;
    {
      readlock lock(m_mutexDisjunction);
      auto it = m_disjunctions.find(key);
      if (it != m_disjunctions.end()) {
        return it->second;
      }
    }
    shared_ptr<Predicate::Disjunction> newPredicate;
    {
      writelock lock(m_mutexDisjunction);
      auto it = m_disjunctions.find(key);
      if (it != m_disjunctions.end()) {
        return it->second;
      }
      newPredicate = std::make_shared<Predicate::Disjunction>(args);
      m_disjunctions[key] = newPredicate;
    }
    index(newPredicate);
    return newPredicate;
  }

  shared_ptr<Predicate> getOrCreateNegation(shared_ptr<Predicate> operand) {
    {
      readlock lock(m_mutexNegation);
      auto it = m_negations.find(operand);
      if (it != m_negations.end()) {
        return it->second;
      }
    }
    shared_ptr<Predicate::Negation> newPredicate;
    {
      writelock lock(m_mutexNegation);
      auto it = m_negations.find(operand);
      if (it != m_negations.end()) {
        return it->second;
      }
      newPredicate = std::make_shared<Predicate::Negation>(operand);
      m_negations[operand] = newPredicate;
    }
    index(newPredicate);
    return newPredicate;
  }

  shared_ptr<Predicate> getOrCreateImplication(shared_ptr<Predicate> lhs,
                                               shared_ptr<Predicate> rhs) {
    auto key = std::make_pair(lhs, rhs);
    {
      readlock lock(m_mutexImplication);
      auto it = m_implications.find(key);
      if (it != m_implications.end()) {
        return it->second;
      }
    }
    shared_ptr<Predicate::Implication> newPredicate;
    {
      writelock lock(m_mutexImplication);
      auto it = m_implications.find(key);
      if (it != m_implications.end()) {
        return it->second;
      }
      newPredicate = std::make_shared<Predicate::Implication>(lhs, rhs);
      m_implications[key] = newPredicate;
    }
    index(newPredicate);
    return newPredicate;
  };

  shared_ptr<Predicate> getOrCreateEquivalence(shared_ptr<Predicate> lhs,
                                               shared_ptr<Predicate> rhs) {
    auto key = std::make_pair(lhs, rhs);
    {
      readlock lock(m_mutexEquivalence);
      auto it = m_equivalences.find(key);
      if (it != m_equivalences.end()) {
        return it->second;
      }
    }
    shared_ptr<Predicate::Equivalence> newPredicate;
    {
      writelock lock(m_mutexEquivalence);
      auto it = m_equivalences.find(key);
      if (it != m_equivalences.end()) {
        return it->second;
      }
      newPredicate = std::make_shared<Predicate::Equivalence>(lhs, rhs);
      m_equivalences[key] = newPredicate;
    }
    index(newPredicate);
    return newPredicate;
  }

  shared_ptr<Predicate> getOrCreateEquality(shared_ptr<Expression> lhs,
                                            shared_ptr<Expression> rhs) {
    auto key = std::make_pair(lhs, rhs);
    {
      readlock lock(m_mutexEquality);
      auto it = m_equalities.find(key);
      if (it != m_equalities.end()) {
        return it->second;
      }
    }
    shared_ptr<Predicate::Equality> newPredicate;
    {
      writelock lock(m_mutexEquality);
      auto it = m_equalities.find(key);
      if (it != m_equalities.end()) {
        return it->second;
      }
      newPredicate = std::make_shared<Predicate::Equality>(lhs, rhs);
      m_equalities[key] = newPredicate;
    }
    index(newPredicate);
    return newPredicate;
  }
};

static std::unique_ptr<PredicateCache> cache =
    std::make_unique<PredicateCache>();

// Factory methods implementation

shared_ptr<Predicate> PredicateFactory::Conjunction(
    const std::vector<std::shared_ptr<Predicate>>& args) {
  return cache->getOrCreateConjunction(args);
}

shared_ptr<Predicate> PredicateFactory::Disjunction(
    const std::vector<std::shared_ptr<Predicate>>& args) {
  return cache->getOrCreateDisjunction(args);
}

shared_ptr<Predicate> PredicateFactory::Negation(
    shared_ptr<Predicate> operand) {
  return cache->getOrCreateNegation(operand);
}

shared_ptr<Predicate> PredicateFactory::Implication(shared_ptr<Predicate> lhs,
                                                    shared_ptr<Predicate> rhs) {
  return cache->getOrCreateImplication(lhs, rhs);
}

shared_ptr<Predicate> PredicateFactory::Equivalence(shared_ptr<Predicate> lhs,
                                                    shared_ptr<Predicate> rhs) {
  return cache->getOrCreateEquivalence(lhs, rhs);
}

shared_ptr<Predicate> PredicateFactory::Equality(shared_ptr<Expression> lhs,
                                                 shared_ptr<Expression> rhs) {
  return cache->getOrCreateEquality(lhs, rhs);
}

size_t PredicateFactory::size() { return cache->size(); }

shared_ptr<Predicate> PredicateFactory::at(size_t index) {
  return cache->at(index);
}

}  // namespace BLang