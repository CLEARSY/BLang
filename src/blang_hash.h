/* @file blang_hash.h
   @brief Header file for the hashing functions

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
#ifndef BLANG_HASH_H
#define BLANG_HASH_H

#include <stddef.h>

#include <string>
#include <utility>

namespace BLang {
inline size_t hash_combine_string(const std::string& str, size_t seed) {
  return seed ^ (std::hash<std::string>{}(str) + 0x9e3779b9 + (seed << 6) +
                 (seed >> 2));
}
inline size_t hash_combine_size_t(size_t combine, size_t seed) {
  return seed ^ (combine + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}
}  // namespace BLang
#endif  // BLANG_HASH_H