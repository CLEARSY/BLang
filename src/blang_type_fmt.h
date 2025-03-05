/* @file blang_type_fmt.h
   @brief Support for formatting for the BLang::Type classes.

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

#ifndef BLANG_TYPE_FMT_H
#define BLANG_TYPE_FMT_H

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "blang_type.h"

// Forward declare formatter specializations
template <>
struct fmt::formatter<BLang::Type>;
template <>
struct fmt::formatter<std::shared_ptr<BLang::Type>>;
template <>
struct fmt::formatter<BLang::Type::ProductType>;
template <>
struct fmt::formatter<BLang::Type::PowerType>;
template <>
struct fmt::formatter<BLang::Type::AbstractSet>;
template <>
struct fmt::formatter<BLang::Type::EnumeratedSet>;
template <>
struct fmt::formatter<BLang::Type::StructType>;

// Formatter for shared_ptr<Type>
template <>
struct fmt::formatter<std::shared_ptr<BLang::Type>> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::shared_ptr<BLang::Type>& type,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    if (!type) return fmt::format_to(ctx.out(), "nullptr");
    return fmt::format_to(ctx.out(), "{}", *type);
  }
};

// Formatter for Type
template <>
struct fmt::formatter<BLang::Type> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Type& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    class FormatterVisitor : public BLang::Type::Visitor {
     public:
      std::string result;

      void visitINTEGER() override { result = "INTEGER"; }
      void visitBOOLEAN() override { result = "BOOLEAN"; }
      void visitFLOAT() override { result = "FLOAT"; }
      void visitREAL() override { result = "REAL"; }
      void visitSTRING() override { result = "STRING"; }

      void visitProductType(const BLang::Type::ProductType& t) override {
        result = fmt::format("{}", t);
      }

      void visitPowerType(const BLang::Type::PowerType& t) override {
        result = fmt::format("{}", t);
      }

      void visitAbstractSet(const BLang::Type::AbstractSet& t) override {
        result = fmt::format("{}", t);
      }

      void visitEnumeratedSet(const BLang::Type::EnumeratedSet& t) override {
        result = fmt::format("{}", t);
      }

      void visitStructType(const BLang::Type::StructType& t) override {
        result = fmt::format("{}", t);
      }

      void visitUndefinedType() override { result = "?"; }
    };

    FormatterVisitor visitor;
    type.accept(visitor);
    return fmt::format_to(ctx.out(), "{}", visitor.result);
  }
};

// Formatter for ProductType
template <>
struct fmt::formatter<BLang::Type::ProductType> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Type::ProductType& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "({} × {})", type.lhs, type.rhs);
  }
};

// Formatter for PowerType
template <>
struct fmt::formatter<BLang::Type::PowerType> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Type::PowerType& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "ℙ({})", type.m_content);
  }
};

// Formatter for AbstractSet
template <>
struct fmt::formatter<BLang::Type::AbstractSet> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Type::AbstractSet& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", type.getName());
  }
};

// Formatter for EnumeratedSet
template <>
struct fmt::formatter<BLang::Type::EnumeratedSet> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Type::EnumeratedSet& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", type.getName());
  }
};

// Formatter for RecordType
template <>
struct fmt::formatter<BLang::Type::StructType> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Type::StructType& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    std::string fields;
    bool first = true;
    for (const auto& [name, fieldType] : type.m_fields) {
      if (!first) fields += ", ";
      fields += fmt::format("{}: {}", name, fieldType);
      first = false;
    }
    return fmt::format_to(ctx.out(), "struct({{{}}})", fields);
  }
};

#endif  // BLANG_TYPE_FMT_H
