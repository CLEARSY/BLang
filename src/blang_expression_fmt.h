/* @file blang_expression_fmt.h
   @brief Support for formatting for the BLang::Expression classes.

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

#ifndef BLANG_EXPRESSION_FMT_H
#define BLANG_EXPRESSION_FMT_H

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "blang_expression.h"

// Forward declare formatter specializations
template <>
struct fmt::formatter<BLang::Expression>;
template <>
struct fmt::formatter<std::shared_ptr<BLang::Expression>>;

// Formatter for shared_ptr<Type>
template <>
struct fmt::formatter<std::shared_ptr<BLang::Expression>> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::shared_ptr<BLang::Expression>& type,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    if (!type) return fmt::format_to(ctx.out(), "nullptr");
    return fmt::format_to(ctx.out(), "{}", *type);
  }
};

// Formatter for Type
template <>
struct fmt::formatter<BLang::Expression> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Expression& type, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    class FormatterVisitor : public BLang::Expression::Visitor {
     public:
      std::string result;

      void visitTRUE() override { result = "TRUE"; }
      void visitFALSE() override { result = "FALSE"; }
    };

    FormatterVisitor visitor;
    type.accept(visitor);
    return fmt::format_to(ctx.out(), "{}", visitor.result);
  }
};

#endif  // BLANG_EXPRESSION_FMT_H
