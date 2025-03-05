/* @file blang_type.h
   @brief Header file for the Type class and TypeFactory class.

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
#ifndef BLANG_TYPE_H
#define BLANG_TYPE_H

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
class TypeFactory;
class TypeCache;

/**
 * @brief Abstract base class representing types of the B-method language.
 *
 * This class is the base for representations of B types.
 * It provides a common interface for all types and ensures maximal sharing
 * through the use of the TypeFactory.
 *
 * Type instances should never be created directly, but only through the
 * TypeFactory class.
 *
 * Types are internally represented in a table. The index of the type in
 * the table may be queried using the Type::index() method, and it is possible
 * get the type at a given index using the TypeFactory::at() method.
 */
class Type : public std::enable_shared_from_this<Type> {
 public:
  /* @brief Enumeration of the different kinds of types.
   *
   * This enumeration lists the different kinds of types that can be represented
   * by the Type class.
   */
  enum class Kind {
    INTEGER,
    BOOLEAN,
    FLOAT,
    REAL,
    STRING,
    ProductType,
    PowerType,
    Struct,
    AbstractSet,
    EnumeratedSet,
    Undefined,
  };
  Kind getKind() const { return m_kind; };

  /** @brief Nested class representing a Cartesian product type. */
  class ProductType;
  /** @brief Nested class representing a power set type. */
  class PowerType;
  /** @brief Nested class representing a struct type. */
  class StructType;
  /** @brief Nested class representing an abstract set type. */
  class AbstractSet;
  /** @brief Nested class representing an enumerated set type. */
  class EnumeratedSet;

  /** @brief converts to a ProductType, if possible
   * @return a shared pointer to the ProductType, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const ProductType> toProductType() const;
  /** @brief converts to a PowerType, if possible
   * @return a shared pointer to the PowerType, or nullptr if the conversion is
   * not possible
   */
  std::shared_ptr<const PowerType> toPowerType() const;
  /** @brief converts to a Struct, if possible
   * @return a shared pointer to the Struct, or nullptr if the conversion is not
   * possible
   */
  std::shared_ptr<const StructType> toStructType() const;
  /** @brief converts to an AbstractSet, if possible
   * @return a shared pointer to the AbstractSet, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const AbstractSet> toAbstractSetType() const;
  /** @brief converts to an EnumeratedSet, if possible
   * @return a shared pointer to the EnumeratedSet, or nullptr if the conversion
   * is not possible
   */
  std::shared_ptr<const EnumeratedSet> toEnumeratedSetType() const;

  /**
   * @brief Abstract visitor class for the Type hierarchy.
   *
   * Implementors can derive from this class to perform specific actions.
   */
  class Visitor {
   public:
    virtual void visitINTEGER() = 0;
    virtual void visitBOOLEAN() = 0;
    virtual void visitFLOAT() = 0;
    virtual void visitREAL() = 0;
    virtual void visitSTRING() = 0;
    virtual void visitAbstractSet(const AbstractSet &) = 0;
    virtual void visitEnumeratedSet(const EnumeratedSet &) = 0;
    virtual void visitProductType(const ProductType &) = 0;
    virtual void visitPowerType(const PowerType &) = 0;
    virtual void visitStructType(const StructType &) = 0;
    virtual void visitUndefinedType() = 0;
  };
  virtual void accept(Visitor &v) const;

  // Comparisons
  static int compare(const Type &v1, const Type &v2);
  static int vec_compare(const std::vector<std::shared_ptr<Type>> &v1,
                         const std::vector<std::shared_ptr<Type>> &v2);
  inline bool operator==(const Type &other) const {
    return compare(*this, other) == 0;
  }
  inline bool operator!=(const Type &other) const {
    return compare(*this, other) != 0;
  }
  inline bool operator<(const Type &other) const {
    return compare(*this, other) < 0;
  }
  inline bool operator>(const Type &other) const {
    return compare(*this, other) > 0;
  }
  inline bool operator<=(const Type &other) const {
    return compare(*this, other) <= 0;
  }
  inline bool operator>=(const Type &other) const {
    return compare(*this, other) >= 0;
  }

  /**
   * @brief Combines the hash of this Type with a seed value.
   * @param seed The initial seed value.
   * @return The combined hash value.
   */
  virtual size_t hash_combine(size_t seed) const;

  friend class TypeFactory;
  friend class TypeCache;

  /**
   * @brief Constructor for Type. Only TypeFactory can create
   * instances.
   * @param kind The kind of Type to create.
   */
  Type(Kind kind)
      : m_kind{kind}, m_index{SIZE_MAX}, m_hash_valid(false), m_hash(0) {};
  virtual ~Type() = default;
  /**
   * @brief Gets the hash value of the Type.
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

  /** @bref Gets the position in the TypeFactory table
   * @return The index of the Type in the TypeFactory table
   *
   * The index matches the creation order.
   */
  size_t index() const { return m_index; }

 private:
  /** @brief Deleted copy constructor to prevent copying. */
  Type(const Type &) = delete;
  /** @brief Deleted assignment operator to prevent assignment. */
  Type &operator=(const Type &) = delete;
  /** @brief The kind of Type. */
  Kind m_kind;

 protected:
  /** @brief The position of the Type in the TypeFactory table */
  size_t m_index;

 private:
  /** @brief Flag to indicate if the hash value is valid. */
  mutable bool m_hash_valid = false;
  /** @brief Cached hash value. */
  mutable size_t m_hash;
};

/**
 * @brief Factory class for creating and managing Type instances.
 *
 * This class ensures maximal sharing of Type instances and provides
 * thread-safe access to create and retrieve BTypes. It implements
 * the Factory pattern and Singleton-like behavior.
 */
class TypeFactory {
 public:
  // Delete constructor, copy constructor and assignment operator
  TypeFactory() = delete;
  TypeFactory(const TypeFactory &) = delete;
  TypeFactory &operator=(const TypeFactory &) = delete;

  // Basic types
  static std::shared_ptr<Type> Integer();
  static std::shared_ptr<Type> Boolean();
  static std::shared_ptr<Type> Float();
  static std::shared_ptr<Type> Real();
  static std::shared_ptr<Type> String();
  static std::shared_ptr<Type> Undefined();
  // Complex type constructors
  static std::shared_ptr<Type> Product(std::shared_ptr<Type> lhs,
                                       std::shared_ptr<Type> rhs);
  static std::shared_ptr<Type> PowerSet(std::shared_ptr<Type> content);
  static std::shared_ptr<Type> AbstractSet(const std::string &name);
  static std::shared_ptr<Type> EnumeratedSet(
      const std::string &, const std::vector<std::string> &values);
  static std::shared_ptr<Type> Struct(
      const std::vector<std::pair<std::string, std::shared_ptr<Type>>> &fields);

  /**
   * @brief Gets the number of BTypes created by the factory.
   * @return The number of BTypes.
   */
  static size_t size();
  /**
   * @brief Gets the Type at a specific index in the factory's internal index.
   * @param index The index of the Type to retrieve.
   * @return A shared pointer to the Type at the given index.
   */
  static std::shared_ptr<Type> at(size_t index);

  /**
   * @brief Gets a named Type (AbstractSet or EnumeratedSet) by name.
   * @param name The name of the Type.
   * @return A shared pointer to the named Type, or nullptr if not found.
   * @note Not yet implemented.
   */
  static std::shared_ptr<Type> Named(const std::string &name);

  class Exception : public std::exception {
   public:
    Exception(const std::string &msg) : msg{msg} {}
    const char *what() const noexcept override { return msg.c_str(); }

   private:
    std::string msg;
  };

  /*@desc writes the type table in XML format to the output stream
   * @param os the output stream
   * @return void
   *
   * @note the produced XML is compatible with the schema in RichTypesInfo.xsd
   */
  static void writeXMLRichTypesInfo(std::ostream &os);

 public:
  /**
   * @brief Builds B types from an XML document following RichTypesInfo schema
   * @param root The tinyxml2 XML element RichTypeInfos
   * @throw TypeFactory::Exception if the XML is invalid or parsing fails
   */
  static void buildFromXML(const tinyxml2::XMLElement *root);
};

class Type::ProductType : public Type {
 public:
  void accept(Visitor &v) const override { v.visitProductType(*this); }

  size_t hash_combine(size_t seed) const override;
  std::shared_ptr<Type> lhs;
  std::shared_ptr<Type> rhs;
  ProductType(std::shared_ptr<Type> lhs, std::shared_ptr<Type> rhs)
      : Type(Kind::ProductType), lhs{lhs}, rhs{rhs} {}
  virtual ~ProductType() = default;
  friend class TypeFactory;
  friend class TypeCache;
};

class Type::PowerType : public Type {
 public:
  size_t hash_combine(size_t seed) const override;
  void accept(Visitor &v) const override { v.visitPowerType(*this); }
  const std::shared_ptr<Type> m_content;
  PowerType(std::shared_ptr<Type> content)
      : Type(Type::Kind::PowerType), m_content{content} {};
  virtual ~PowerType() = default;
  friend class TypeFactory;
  friend class TypeCache;
};

class Type::AbstractSet : public Type {
 public:
  size_t hash_combine(size_t seed) const override;
  void accept(Visitor &v) const override { v.visitAbstractSet(*this); }
  const std::string &getName() const { return m_name; }
  AbstractSet(const std::string &name)
      : Type(Type::Kind::AbstractSet), m_name{name} {};
  virtual ~AbstractSet() = default;
  const std::string m_name;
  friend class TypeFactory;
  friend class TypeCache;
};

class Type::EnumeratedSet : public Type {
 public:
  size_t hash_combine(size_t seed) const override;
  void accept(Visitor &v) const override { v.visitEnumeratedSet(*this); }

  const std::string &getName() const { return m_name; }
  const std::vector<std::string> &getValues() const { return m_values; }
  const std::string m_name;
  const std::vector<std::string> m_values;
  EnumeratedSet(const std::pair<std::string, std::vector<std::string>> &values)
      : Type(Type::Kind::EnumeratedSet),
        m_name(values.first),
        m_values{values.second} {}
  virtual ~EnumeratedSet() = default;
  friend class TypeFactory;
  friend class TypeCache;
};

class Type::StructType : public Type {
 public:
  size_t hash_combine(size_t seed) const override;

  void accept(Visitor &v) const override { v.visitStructType(*this); }
  //
  const std::vector<std::pair<std::string, std::shared_ptr<Type>>>
      m_fields;  // invariant: fields are sorted alphabetically
  static std::vector<std::pair<std::string, std::shared_ptr<Type>>> sort(
      const std::vector<std::pair<std::string, std::shared_ptr<Type>>> &fields);
  const std::vector<std::pair<std::string, std::shared_ptr<Type>>> &getFields()
      const {
    return m_fields;
  }
  StructType(
      const std::vector<std::pair<std::string, std::shared_ptr<Type>>> &fields)
      : Type(Type::Kind::Struct), m_fields{sort(fields)} {}
  virtual ~StructType() = default;

  friend class TypeFactory;
  friend class TypeCache;
};

}  // namespace BLang

#endif  // BLANG_TYPE_H
