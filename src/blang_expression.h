/* @file blang_expression.h
   @brief Header file for the Expression class and ExpressionFactory class.

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
#ifndef BLANG_EXPRESSION_H
#define BLANG_EXPRESSION_H

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <utility>
#include <vector>

#include "blang_type.h"

namespace BLang {
// Forward declaration
class ExpressionFactory;
class ExpressionCache;

class Predicate;
class Type;

/**
 * @brief Abstract base class representing expressions of the B-method language.
 *
 * This class is the base for representations of B expressions.
 * It provides a common interface for all expressions and ensures maximal
 * sharing through the use of the ExpressionFactory.
 *
 * Expression instances should never be created directly, but only through the
 * ExpressionFactory class.
 *
 * Expressions are internally represented in a table. The index of the
 * expression in the table may be queried using the Expression::index() method,
 * and it is possible get the expression at a given index using the
 * ExpressionFactory::at() method.
 */
class Expression : public std::enable_shared_from_this<Expression> {
 public:
  /* @brief Enumeration of the different kinds of expressions.
   *
   * This enumeration lists the different kinds of expressions that can be
   * represented by the Expression class.
   */
  enum class Kind {
    TRUE,
    FALSE,
  };
  Kind getKind() const { return m_kind; };

  /**
   * @brief Abstract visitor class for the Expression hierarchy.
   *
   * Implementors can derive from this class to perform specific actions.
   */
  class Visitor {
   public:
    virtual void visitTRUE() = 0;
    virtual void visitFALSE() = 0;
  };
  virtual void accept(Visitor &v) const;

  // Comparisons
  static int compare(const Expression &v1, const Expression &v2);
  static int vec_compare(const std::vector<std::shared_ptr<Expression>> &v1,
                         const std::vector<std::shared_ptr<Expression>> &v2);
  inline bool operator==(const Expression &other) const {
    return compare(*this, other) == 0;
  }
  inline bool operator!=(const Expression &other) const {
    return compare(*this, other) != 0;
  }
  inline bool operator<(const Expression &other) const {
    return compare(*this, other) < 0;
  }
  inline bool operator>(const Expression &other) const {
    return compare(*this, other) > 0;
  }
  inline bool operator<=(const Expression &other) const {
    return compare(*this, other) <= 0;
  }
  inline bool operator>=(const Expression &other) const {
    return compare(*this, other) >= 0;
  }

  /**
   * @brief Combines the hash of this Expression with a seed value.
   * @param seed The initial seed value.
   * @return The combined hash value.
   */
  virtual size_t hash_combine(size_t seed) const;

  friend class ExpressionFactory;
  friend class ExpressionCache;

  /**
   * @brief Constructor for Expression. Only ExpressionFactory can create
   * instances.
   * @param kind The kind of Expression to create.
   * @param type The type of the Expression.
   */
  Expression(Kind kind, std::shared_ptr<Type> type)
      : m_kind{kind},
        m_type{type},
        m_index{SIZE_MAX},
        m_hash_valid(false),
        m_hash(0),
        m_xml{} {}
  /**
   * @brief Constructor for Expression. Only ExpressionFactory can create
   * instances.
   * @param kind The kind of Expression to create.
   */
  Expression(Kind kind)
      : m_kind{kind},
        m_type{TypeFactory::Undefined()},
        m_index{SIZE_MAX},
        m_hash_valid(false),
        m_hash(0),
        m_xml{} {}
  virtual ~Expression() = default;
  /**
   * @brief Gets the hash value of the Expression.
   * @return The hash value.
   * @note The hash is computed lazily and cached.
   */
  size_t hash() const {
    if (!m_hash_valid) {
      m_hash = hash_combine(0);
      m_hash_valid = true;
    }
    return m_hash;
  }

  /** @bref Gets the position in the ExpressionFactory table
   * @return The index of the Expression in the ExpressionFactory table
   *
   * The index matches the creation order.
   */
  size_t index() const { return m_index; }

 private:
  /** @brief Deleted copy constructor to prevent copying. */
  Expression(const Expression &) = delete;
  /** @brief Deleted assignment operator to prevent assignment. */
  Expression &operator=(const Expression &) = delete;
  /** @brief The kind of Expression. */
  Kind m_kind;

 protected:
  std::shared_ptr<Type> m_type;
  /** @brief The position of the Expression in the ExpressionFactory table */
  size_t m_index;

 private:
  /** @brief Flag to indicate if the hash value is valid. */
  mutable bool m_hash_valid = false;
  /** @brief Cached hash value. */
  mutable size_t m_hash;
  /** @brief XML representation for the expression */
  mutable std::string m_xml;
};

/**
 * @brief Factory class for creating and managing Expression instances.
 *
 * This class ensures maximal sharing of Expression instances and provides
 * thread-safe access to create and retrieve Expressions. It implements
 * the Factory pattern and Singleton-like behavior.
 */
class ExpressionFactory {
 public:
  // Delete constructor, copy constructor and assignment operator
  ExpressionFactory() = delete;
  ExpressionFactory(const ExpressionFactory &) = delete;
  ExpressionFactory &operator=(const ExpressionFactory &) = delete;

  static std::shared_ptr<Expression> TRUE();
  static std::shared_ptr<Expression> FALSE();

  /**
   * @brief Gets the number of Expressions created by the factory.
   * @return The number of Expressions.
   */
  static size_t size();
  /**
   * @brief Gets the Expression at a specific index in the factory's internal
   * index.
   * @param index The index of the Expression to retrieve.
   * @return A shared pointer to the Expression at the given index.
   */
  static std::shared_ptr<Expression> at(size_t index);

  class Exception : public std::exception {
   public:
    Exception(const std::string &msg) : msg{msg} {}
    const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
  };
};

}  // namespace BLang

#endif  // BLANG_EXPRESSION_H