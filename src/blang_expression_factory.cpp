/* @file blang_expression_factory.cpp
   @brief Implementation file for most of ExpressionFactory class.

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

struct UnaryPredicateHash {
  size_t operator()(const shared_ptr<Predicate>& p) const {
    return p->hash_combine(0);
  }
};

// Thread-safe expression caches
class ExpressionCache {
 private:
  mutable shared_mutex m_basic;
  mutable shared_mutex m_mutexConversionBool;
  mutable shared_mutex m_mutexIndex;

  unordered_map<shared_ptr<Predicate>, shared_ptr<Expression::ConversionBool>,
                UnaryPredicateHash>
      m_conversionsBool;

  shared_ptr<Expression> m_TRUE;
  shared_ptr<Expression> m_FALSE;

  vector<shared_ptr<Expression>> m_index;

  void index(shared_ptr<Expression> expression) {
    writelock lock(m_mutexIndex);
    expression->m_index = m_index.size();
    m_index.push_back(expression);
  }

 public:
  ExpressionCache() : m_TRUE(), m_FALSE() {}
  size_t size() const {
    readlock lock(m_mutexIndex);
    return m_index.size();
  }

  shared_ptr<Expression> at(size_t index) const {
    readlock lock(m_mutexIndex);
    return m_index[index];
  }

  shared_ptr<Expression> getTRUE() {
    {
      readlock rlock(m_basic);
      if (m_TRUE) {
        return m_TRUE;
      }
    }
    {
      writelock wlock(m_basic);
      if (m_TRUE) {
        return m_TRUE;
      }
      m_TRUE = std::make_shared<Expression>(Expression::Kind::TRUE,
                                            TypeFactory::Boolean());
    }
    index(m_TRUE);
    return m_TRUE;
  }

  shared_ptr<Expression> getFALSE() {
    {
      readlock rlock(m_basic);
      if (m_FALSE) {
        return m_FALSE;
      }
    }
    {
      writelock wlock(m_basic);
      if (m_FALSE) {
        return m_FALSE;
      }
      m_FALSE = std::make_shared<Expression>(Expression::Kind::FALSE,
                                             TypeFactory::Boolean());
    }
    index(m_FALSE);
    return m_FALSE;
  }

  shared_ptr<Expression> getConversionBool(shared_ptr<Predicate> pred) {
    {
      readlock rlock(m_mutexConversionBool);
      auto it = m_conversionsBool.find(pred);
      if (it != m_conversionsBool.end()) {
        return it->second;
      }
    }
    shared_ptr<Expression::ConversionBool> newExpression;
    {
      writelock wlock(m_mutexConversionBool);
      auto it = m_conversionsBool.find(pred);
      if (it != m_conversionsBool.end()) {
        return it->second;
      }
      newExpression = std::make_shared<Expression::ConversionBool>(pred);
      m_conversionsBool[pred] = newExpression;
    }
    index(newExpression);
    return newExpression;
  }
};

static std::unique_ptr<ExpressionCache> cache =
    std::make_unique<ExpressionCache>();

// Factory methods implementation

shared_ptr<Expression> ExpressionFactory::TRUE() { return cache->getTRUE(); }

shared_ptr<Expression> ExpressionFactory::FALSE() { return cache->getFALSE(); }

shared_ptr<Expression> ExpressionFactory::ConversionBool(
    shared_ptr<Predicate> pred) {
  return cache->getConversionBool(pred);
}
size_t ExpressionFactory::size() { return cache->size(); }

shared_ptr<Expression> ExpressionFactory::at(size_t index) {
  return cache->at(index);
}

}  // namespace BLang