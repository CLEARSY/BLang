/* @file blang_predicate.h
   @brief Header file for the BLang::Predicate class and its derived classes
   and for the BLang::PredicateFactory class.

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
#ifndef BLANG_PREDICATE_H
#define BLANG_PREDICATE_H

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <utility>
#include <vector>

namespace tinyxml2 {
class XMLElement;
};

namespace BLang {
// Forward declaration
class PredicateFactory;
class PredicateCache;
class Expression;

/**
 * @brief Abstract base class representing predicates of the B-method language.
 *
 * This class is the base for representations of B predicates.
 * It provides a common interface for all predicates and ensures maximal sharing
 * through the use of the PredicateFactory.
 *
 * Predicate instances should never be created directly, but only through the
 * PredicateFactory class.
 *
 * Predicates are internally represented in a table. The index of the predicate
 * in the table may be queried using the Predicate::index() method, and it is
 * possible get the predicate at a given index using the PredicateFactory::at()
 * method.
 */
class Predicate : public std::enable_shared_from_this<Predicate> {
 public:
  /* @brief Enumeration of the different kinds of predicates.
   *
   * This enumeration lists the different kinds of predicates that can be
   * represented by the Predicate class.
   */
  enum class Kind {
    Conjunction,
    Disjunction,
    Negation,
    Implication,
    Equivalence,
    Equality,
  };
  Kind getKind() const { return m_kind; };

  // Forward declarations of derived classes
  class Binary;
  class Nary;
  class Conjunction;
  class Disjunction;
  class Negation;
  class Implication;
  class Equivalence;
  class Equality;

  /** @brief converts to a Conjunction, if possible
   * @return a shared pointer to the Conjunction, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const Conjunction> toConjunction() const;

  /** @brief converts to a Disjunction, if possible
   * @return a shared pointer to the Disjunction, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const Disjunction> toDisjunction() const;

  /** @brief converts to a Negation, if possible
   * @return a shared pointer to the Negation, or nullptr if the conversion is
   * not possible
   */
  std::shared_ptr<const Negation> toNegation() const;

  /** @brief converts to an Implication, if possible
   * @return a shared pointer to the Implication, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const Implication> toImplication() const;

  /** @brief converts to an Equivalence, if possible
   * @return a shared pointer to the Equivalence, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const Equivalence> toEquivalence() const;

  /** @brief converts to an Equality, if possible
   * @return a shared pointer to the Equality, or nullptr if the conversion is
   * not possible
   */
  std::shared_ptr<const Equality> toEquality() const;

  /**
   * @brief Abstract visitor class for the Predicate hierarchy.
   *
   * Implementors can derive from this class to perform specific actions.
   */
  class Visitor {
   public:
    virtual void visitConjunction(const Conjunction &) = 0;
    virtual void visitDisjunction(const Disjunction &) = 0;
    virtual void visitNegation(const Negation &) = 0;
    virtual void visitImplication(const Implication &) = 0;
    virtual void visitEquivalence(const Equivalence &) = 0;
    virtual void visitEquality(const Equality &) = 0;
  };
  virtual void accept(Visitor &v) const = 0;

  // Comparisons
  static int compare(const Predicate &v1, const Predicate &v2);
  static int vec_compare(const std::vector<std::shared_ptr<Predicate>> &v1,
                         const std::vector<std::shared_ptr<Predicate>> &v2);
  inline bool operator==(const Predicate &other) const {
    return compare(*this, other) == 0;
  }
  inline bool operator!=(const Predicate &other) const {
    return compare(*this, other) != 0;
  }
  inline bool operator<(const Predicate &other) const {
    return compare(*this, other) < 0;
  }
  inline bool operator>(const Predicate &other) const {
    return compare(*this, other) > 0;
  }
  inline bool operator<=(const Predicate &other) const {
    return compare(*this, other) <= 0;
  }
  inline bool operator>=(const Predicate &other) const {
    return compare(*this, other) >= 0;
  }

  /**
   * @brief Combines the hash of this Predicate with a seed value.
   * @param seed The initial seed value.
   * @return The combined hash value.
   */
  virtual size_t hash_combine(size_t seed) const;

  friend class PredicateFactory;
  friend class PredicateCache;

  /**
   * @brief Constructor for Predicate. Only PredicateFactory can create
   * instances.
   * @param kind The kind of Predicate to create.
   */
  Predicate(Kind kind)
      : m_kind{kind}, m_index{SIZE_MAX}, m_hash_valid(false), m_hash(0) {};
  virtual ~Predicate() = default;
  /**
   * @brief Gets the hash value of the Predicate.
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

  /** @bref Gets the position in the PredicateFactory table
   * @return The index of the Predicate in the PredicateFactory table
   *
   * The index matches the creation order.
   */
  size_t index() const { return m_index; }

 private:
  /** @brief Deleted copy constructor to prevent copying. */
  Predicate(const Predicate &) = delete;
  /** @brief Deleted assignment operator to prevent assignment. */
  Predicate &operator=(const Predicate &) = delete;
  /** @brief The kind of Predicate. */
  Kind m_kind;

 protected:
  /** @brief The position of the Predicate in the PredicateFactory table */
  size_t m_index;

 private:
  /** @brief Flag to indicate if the hash value is valid. */
  mutable bool m_hash_valid = false;
  /** @brief Cached hash value. */
  mutable size_t m_hash;
};

/**
 * @brief Factory class for creating and managing Predicate instances.
 *
 * This class ensures maximal sharing of Predicate instances and provides
 * thread-safe access to create and retrieve BPredicates. It implements
 * the Factory pattern and Singleton-like behavior.
 */
class PredicateFactory {
 public:
  // Delete constructor, copy constructor and assignment operator
  PredicateFactory() = delete;
  PredicateFactory(const PredicateFactory &) = delete;
  PredicateFactory &operator=(const PredicateFactory &) = delete;

  // Factory methods to create instances of derived classes
  static std::shared_ptr<Predicate> Conjunction(
      const std::vector<std::shared_ptr<Predicate>> &args);

  static std::shared_ptr<Predicate> Disjunction(
      const std::vector<std::shared_ptr<Predicate>> &args);

  static std::shared_ptr<Predicate> Negation(
      std::shared_ptr<Predicate> operand);

  static std::shared_ptr<Predicate> Implication(std::shared_ptr<Predicate> lhs,
                                                std::shared_ptr<Predicate> rhs);

  static std::shared_ptr<Predicate> Equivalence(std::shared_ptr<Predicate> lhs,
                                                std::shared_ptr<Predicate> rhs);

  static std::shared_ptr<Predicate> Equality(std::shared_ptr<Expression> lhs,
                                             std::shared_ptr<Expression> rhs);

  /**
   * @brief Gets the number of BPredicates created by the factory.
   * @return The number of BPredicates.
   */
  static size_t size();
  /**
   * @brief Gets the Predicate at a specific index in the factory's internal
   * index.
   * @param index The index of the Predicate to retrieve.
   * @return A shared pointer to the Predicate at the given index.
   */
  static std::shared_ptr<Predicate> at(size_t index);

  class Exception : public std::exception {
   public:
    Exception(const std::string &msg) : msg{msg} {}
    const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
  };
};

class Predicate::Binary {
 protected:
  std::shared_ptr<Predicate> m_lhs;
  std::shared_ptr<Predicate> m_rhs;
  size_t hash_combine(size_t seed) const;
  Binary(std::shared_ptr<Predicate> lhs, std::shared_ptr<Predicate> rhs)
      : m_lhs{lhs}, m_rhs{rhs} {}
  virtual ~Binary() = default;

 public:
  std::shared_ptr<Predicate> lhs() const { return m_lhs; }
  std::shared_ptr<Predicate> rhs() const { return m_rhs; }
};

class Predicate::Nary {
 protected:
  const std::vector<std::shared_ptr<Predicate>> m_args;
  size_t hash_combine(size_t seed) const;
  Nary(const std::vector<std::shared_ptr<Predicate>> &args) : m_args{args} {}
  virtual ~Nary() = default;

 public:
  const std::vector<std::shared_ptr<Predicate>> &args() const { return m_args; }
};

class Predicate::Conjunction : public Predicate, public Predicate::Nary {
 public:
  void accept(Visitor &v) const override { v.visitConjunction(*this); }

  size_t hash_combine(size_t seed) const override;
  Conjunction(const std::vector<std::shared_ptr<Predicate>> &args)
      : Predicate(Kind::Conjunction), Predicate::Nary(args) {}
  virtual ~Conjunction() = default;
  friend class PredicateFactory;
  friend class PredicateCache;
};

class Predicate::Disjunction : public Predicate, public Predicate::Nary {
 public:
  void accept(Visitor &v) const override { v.visitDisjunction(*this); }

  size_t hash_combine(size_t seed) const override;
  Disjunction(const std::vector<std::shared_ptr<Predicate>> &args)
      : Predicate(Kind::Disjunction), Predicate::Nary(args) {}
  virtual ~Disjunction() = default;
  friend class PredicateFactory;
  friend class PredicateCache;
};

class Predicate::Negation : public Predicate {
 public:
  void accept(Visitor &v) const override { v.visitNegation(*this); }

  size_t hash_combine(size_t seed) const override;
  std::shared_ptr<Predicate> m_operand;
  Negation(std::shared_ptr<Predicate> operand)
      : Predicate(Kind::Negation), m_operand{operand} {}
  virtual ~Negation() = default;
  std::shared_ptr<Predicate> operand() const { return m_operand; }
  friend class PredicateFactory;
  friend class PredicateCache;
};

class Predicate::Implication : public Predicate, public Predicate::Binary {
 public:
  void accept(Visitor &v) const override { v.visitImplication(*this); }

  size_t hash_combine(size_t seed) const override;
  Implication(std::shared_ptr<Predicate> lhs, std::shared_ptr<Predicate> rhs)
      : Predicate(Kind::Implication), Predicate::Binary(lhs, rhs) {}
  virtual ~Implication() = default;
  friend class PredicateFactory;
  friend class PredicateCache;
};

class Predicate::Equivalence : public Predicate, public Predicate::Binary {
 public:
  void accept(Visitor &v) const override { v.visitEquivalence(*this); }

  size_t hash_combine(size_t seed) const override;
  Equivalence(std::shared_ptr<Predicate> lhs, std::shared_ptr<Predicate> rhs)
      : Predicate(Kind::Equivalence), Predicate::Binary(lhs, rhs) {}
  virtual ~Equivalence() = default;
  friend class PredicateFactory;
  friend class PredicateCache;
};

class Predicate::Equality : public Predicate {
 public:
  void accept(Visitor &v) const override { v.visitEquality(*this); }

  size_t hash_combine(size_t seed) const override;
  std::shared_ptr<Expression> m_lhs;
  std::shared_ptr<Expression> m_rhs;
  Equality(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
      : Predicate(Kind::Equality), m_lhs{lhs}, m_rhs{rhs} {}
  virtual ~Equality() = default;
  std::shared_ptr<Expression> lhs() const { return m_lhs; }
  std::shared_ptr<Expression> rhs() const { return m_rhs; }
  friend class PredicateFactory;
  friend class PredicateCache;
};

}  // namespace BLang

#endif  // BLANG_PREDICATE_H