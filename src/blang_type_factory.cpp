/* @file blang_type_factory.cpp
   @brief Implementation file for most of TypeFactory class.

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

#include "blang_type.h"

namespace BLang {

using std::shared_mutex;
using std::shared_ptr;
using std::string;
using std::vector;
using readlock = std::shared_lock<shared_mutex>;
using writelock = std::unique_lock<shared_mutex>;

// Hash functions for complex types
struct ProductTypeHash {
  size_t operator()(
      const std::pair<shared_ptr<Type>, shared_ptr<Type>>& p) const {
    return p.first->hash_combine(p.second->hash_combine(0));
  }
};

struct PowerTypeHash {
  size_t operator()(const shared_ptr<Type>& type) const {
    return type->hash_combine(0);
  }
};

// Thread-safe type caches
class TypeCache {
 private:
  mutable shared_mutex m_basic;
  mutable shared_mutex m_mutexProduct;
  mutable shared_mutex m_mutexPower;
  mutable shared_mutex m_mutexAbstract;
  mutable shared_mutex m_mutexEnumerated;
  mutable shared_mutex m_mutexStruct;
  mutable shared_mutex m_mutexIndex;
  std::unordered_map<std::pair<shared_ptr<Type>, shared_ptr<Type>>,
                     shared_ptr<Type::ProductType>, ProductTypeHash>
      m_productTypes;
  std::unordered_map<shared_ptr<Type>, shared_ptr<Type::PowerType>,
                     PowerTypeHash>
      m_powerTypes;
  std::unordered_map<string, shared_ptr<Type::AbstractSet>> m_abstractSets;
  std::unordered_map<string,
                     shared_ptr<Type::EnumeratedSet>>
      m_enumeratedSets;  // indexed by name
  std::unordered_map<string,
                     shared_ptr<Type::StructType>>
      m_structTypes;  // indexed by space-ended field names
  shared_ptr<Type> m_INTEGER;
  shared_ptr<Type> m_BOOLEAN;
  shared_ptr<Type> m_FLOAT;
  shared_ptr<Type> m_REAL;
  shared_ptr<Type> m_STRING;
  shared_ptr<Type> m_UNDEFINED;
  std::vector<shared_ptr<Type>> m_index;

  void index(shared_ptr<Type> type) {
    writelock rlock(m_mutexIndex);
    type->m_index = m_index.size();
    m_index.push_back(type);
  }

 public:
  TypeCache()
      : m_INTEGER(),
        m_BOOLEAN(),
        m_FLOAT(),
        m_REAL(),
        m_STRING(),
        m_UNDEFINED() {}
  size_t size() const {
    readlock rlock(m_mutexIndex);
    return m_index.size();
  }
  shared_ptr<Type> at(size_t index) const {
    readlock rlock(m_mutexIndex);
    return m_index[index];
  }
  shared_ptr<Type> getInteger() {
    {
      readlock rlock(m_basic);
      if (m_INTEGER) {
        return m_INTEGER;
      }
    }
    {
      writelock rlock(m_basic);
      if (m_INTEGER) {
        return m_INTEGER;
      }
      m_INTEGER = std::make_shared<Type>(Type::Kind::INTEGER);
    }
    index(m_INTEGER);
    return m_INTEGER;
  }
  shared_ptr<Type> getBoolean() {
    {
      readlock rlock(m_basic);
      if (m_BOOLEAN) {
        return m_BOOLEAN;
      }
    }
    {
      writelock rlock(m_basic);
      if (m_BOOLEAN) {
        return m_BOOLEAN;
      }
      m_BOOLEAN = std::make_shared<Type>(Type::Kind::BOOLEAN);
    }
    index(m_BOOLEAN);
    return m_BOOLEAN;
  }
  shared_ptr<Type> getFloat() {
    {
      readlock rlock(m_basic);
      if (m_FLOAT) {
        return m_FLOAT;
      }
    }
    {
      writelock rlock(m_basic);
      if (m_FLOAT) {
        return m_FLOAT;
      }
      m_FLOAT = std::make_shared<Type>(Type::Kind::FLOAT);
    }
    index(m_FLOAT);
    return m_FLOAT;
  }
  shared_ptr<Type> getReal() {
    {
      readlock rlock(m_basic);
      if (m_REAL) {
        return m_REAL;
      }
    }
    {
      writelock rlock(m_basic);
      if (m_REAL) {
        return m_REAL;
      }
      m_REAL = std::make_shared<Type>(Type::Kind::REAL);
    }
    index(m_REAL);
    return m_REAL;
  }
  shared_ptr<Type> getString() {
    {
      readlock rlock(m_basic);
      if (m_STRING) {
        return m_STRING;
      }
    }
    {
      writelock rlock(m_basic);
      if (m_STRING) {
        return m_STRING;
      }
      m_STRING = std::make_shared<Type>(Type::Kind::STRING);
    }
    index(m_STRING);
    return m_STRING;
  }
  shared_ptr<Type> getOrCreateProductType(shared_ptr<Type> lhs,
                                          shared_ptr<Type> rhs) {
    auto key = std::make_pair(lhs, rhs);
    {
      readlock rlock(m_mutexProduct);
      auto it = m_productTypes.find(key);
      if (it != m_productTypes.end()) {
        return it->second;
      }
    }
    shared_ptr<Type::ProductType> newType;
    {
      writelock rlock(m_mutexProduct);
      auto it = m_productTypes.find(key);
      if (it != m_productTypes.end()) {
        return it->second;
      }
      newType = std::make_shared<Type::ProductType>(lhs, rhs);
      m_productTypes[key] = newType;
    }
    index(newType);
    return newType;
  }
  shared_ptr<Type> getOrCreatePowerType(shared_ptr<Type> content) {
    {
      readlock rlock(m_mutexPower);
      auto it = m_powerTypes.find(content);
      if (it != m_powerTypes.end()) {
        return it->second;
      }
    }
    shared_ptr<Type::PowerType> newType;
    {
      writelock rlock(m_mutexPower);
      auto it = m_powerTypes.find(content);
      if (it != m_powerTypes.end()) {
        return it->second;
      }
      newType = std::make_shared<Type::PowerType>(content);
      m_powerTypes[content] = newType;
    }
    index(newType);
    return newType;
  }
  shared_ptr<Type> getOrCreateAbstractSet(const string& name) {
    {
      readlock rlock(m_mutexAbstract);
      auto it = m_abstractSets.find(name);
      if (it != m_abstractSets.end()) {
        return it->second;
      }
    }
    shared_ptr<Type::AbstractSet> newType;
    {
      writelock rlock(m_mutexAbstract);
      auto it = m_abstractSets.find(name);
      if (it != m_abstractSets.end()) {
        return it->second;
      }
      newType = std::make_shared<Type::AbstractSet>(name);
      m_abstractSets[name] = newType;
    }
    index(newType);
    return newType;
  }
  shared_ptr<Type> getOrCreateEnumeratedSet(const string& name,
                                            const std::vector<string>& values) {
    {
      readlock rlock(m_mutexEnumerated);
      auto it = m_enumeratedSets.find(name);
      if (it != m_enumeratedSets.end()) {
        return it->second;
      }
    }
    shared_ptr<Type::EnumeratedSet> newType;
    {
      writelock rlock(m_mutexEnumerated);
      auto it = m_enumeratedSets.find(name);
      if (it != m_enumeratedSets.end()) {
        return it->second;
      }
      newType = std::make_shared<Type::EnumeratedSet>(std::pair(name, values));
      m_enumeratedSets[name] = newType;
    }
    index(newType);
    return newType;
  }
  shared_ptr<Type> getOrCreateStruct(
      const std::vector<std::pair<string, shared_ptr<Type>>>& fields) {
    auto sortedFields = Type::StructType::sort(fields);
    string keyString;
    for (const auto& field : sortedFields) {
      keyString.append(field.first);
      keyString.push_back(';');
    }
    {
      readlock rlock(m_mutexStruct);
      auto it = m_structTypes.find(keyString);
      if (it != m_structTypes.end()) {
        return it->second;
      }
    }
    shared_ptr<Type::StructType> newType;
    {
      writelock rlock(m_mutexStruct);
      auto it = m_structTypes.find(keyString);
      if (it != m_structTypes.end()) {
        return it->second;
      }
      newType = std::make_shared<Type::StructType>(sortedFields);
      m_structTypes[keyString] = newType;
    }
    index(newType);
    return newType;
  }
  shared_ptr<Type> getUndefined() {
    {
      readlock rlock(m_basic);
      if (m_UNDEFINED) {
        return m_UNDEFINED;
      }
    }
    {
      writelock rlock(m_basic);
      if (m_UNDEFINED) {
        return m_UNDEFINED;
      }
      m_UNDEFINED = std::make_shared<Type>(BLang::Type::Kind::Undefined);
    }
    index(m_UNDEFINED);
    return m_UNDEFINED;
  }
};

static std::unique_ptr<TypeCache> cache = std::make_unique<TypeCache>();

// Factory methods implementation
shared_ptr<Type> TypeFactory::Integer() { return cache->getInteger(); }
shared_ptr<Type> TypeFactory::Boolean() { return cache->getBoolean(); }
shared_ptr<Type> TypeFactory::Float() { return cache->getFloat(); }
shared_ptr<Type> TypeFactory::Real() { return cache->getReal(); }
shared_ptr<Type> TypeFactory::String() { return cache->getString(); }

shared_ptr<Type> TypeFactory::Product(shared_ptr<Type> lhs,
                                      shared_ptr<Type> rhs) {
  return cache->getOrCreateProductType(lhs, rhs);
}

shared_ptr<Type> TypeFactory::PowerSet(shared_ptr<Type> content) {
  return cache->getOrCreatePowerType(content);
}

shared_ptr<Type> TypeFactory::AbstractSet(const string& name) {
  return cache->getOrCreateAbstractSet(name);
}

shared_ptr<Type> TypeFactory::EnumeratedSet(const string& name,
                                            const std::vector<string>& values) {
  if (values.empty()) {
    throw Exception(std::string("EnumeratedSet must have at least one value"));
  }
  return cache->getOrCreateEnumeratedSet(name, values);
}

shared_ptr<Type> TypeFactory::Struct(
    const std::vector<std::pair<string, shared_ptr<Type>>>& fields) {
  if (fields.empty()) {
    throw Exception(std::string("Struct must have at least one field"));
  }
  return cache->getOrCreateStruct(fields);
}

shared_ptr<Type> TypeFactory::Undefined() { return cache->getUndefined(); }

size_t TypeFactory::size() { return cache->size(); }

shared_ptr<Type> TypeFactory::at(size_t index) { return cache->at(index); }

}  // namespace BLang