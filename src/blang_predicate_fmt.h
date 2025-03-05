/* @file blang_predicate_fmt.h
   @brief Support for formatting for the BLang::Predicate classes.

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

#ifndef BLANG_PREDICATE_FMT_H
#define BLANG_PREDICATE_FMT_H

#include <fmt/format.h>
#include <fmt/ranges.h>

#include "blang_predicate.h"

// Forward declare formatter specializations
template <>
struct fmt::formatter<BLang::Predicate>;
template <>
struct fmt::formatter<BLang::Predicate>;
template <>
struct fmt::formatter<BLang::Predicate::Conjunction>;
template <>
struct fmt::formatter<BLang::Predicate::Disjunction>;
template <>
struct fmt::formatter<BLang::Predicate::Negation>;
template <>
struct fmt::formatter<BLang::Predicate::Implication>;
template <>
struct fmt::formatter<BLang::Predicate::Equivalence>;
template <>
struct fmt::formatter<BLang::Predicate::Equality>;

// Formatter for shared_ptr<Predicate>
template <>
struct fmt::formatter<std::shared_ptr<BLang::Predicate>> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::shared_ptr<BLang::Predicate>& type,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    if (!type) return fmt::format_to(ctx.out(), "nullptr");
    return fmt::format_to(ctx.out(), "{}", *type);
  }
};

// Formatter for Predicate
template <>
struct fmt::formatter<BLang::Predicate> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Predicate& pred, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    class FormatterVisitor : public BLang::Predicate::Visitor {
     public:
      std::string result;

      void visitConjunction(const BLang::Predicate::Conjunction& p) override {
        result = fmt::format("{}", p);
      }

      void visitDisjunction(const BLang::Predicate::Disjunction& p) override {
        result = fmt::format("{}", p);
      }
      void visitNegation(const BLang::Predicate::Negation& p) override {
        result = fmt::format("{}", p);
      }
      void visitImplication(const BLang::Predicate::Implication& p) override {
        result = fmt::format("{}", p);
      }
      void visitEquivalence(const BLang::Predicate::Equivalence& p) override {
        result = fmt::format("{}", p);
      }
      void visitEquality(const BLang::Predicate::Equality& p) override {
        result = fmt::format("{}", p);
      }
    };

    FormatterVisitor visitor;
    pred.accept(visitor);
    return fmt::format_to(ctx.out(), "{}", visitor.result);
  }
};

// Formatter for Conjunction
template <>
struct fmt::formatter<BLang::Predicate::Conjunction> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Predicate::Conjunction& pred,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{:n:}", pred.args());
  }
};

// Formatter for Disjunction
template <>
struct fmt::formatter<BLang::Predicate::Disjunction> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Predicate::Disjunction& pred,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{:n:}", pred.args());
  }
};

// Formatter for Negation
template <>
struct fmt::formatter<BLang::Predicate::Negation> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Predicate::Negation& pred, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "not {}", pred.operand());
  }
};

// Formatter for Implication
template <>
struct fmt::formatter<BLang::Predicate::Implication> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Predicate::Implication& pred,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "({} => {})", pred.lhs(), pred.rhs());
  }
};

// Formatter for Equivalence
template <>
struct fmt::formatter<BLang::Predicate::Equivalence> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Predicate::Equivalence& pred,
              FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "({} <=> {})", pred.lhs(), pred.rhs());
  }
};

// Formatter for Equality
template <>
struct fmt::formatter<BLang::Predicate::Equality> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const BLang::Predicate::Equality& pred, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "({} = {})", pred.lhs(), pred.rhs());
  }
};

#endif  // BLANG_PREDICATE_FMT_H
