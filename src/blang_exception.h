/* @file blang_exception.h
   @brief Header file for the BLang's exceptions

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
#ifndef BLANG_EXCEPTION_H
#define BLANG_EXCEPTION_H

#include <exception>
#include <string>

namespace BLang {

class Exception : public std::exception {
 public:
  Exception(const std::string &msg) : m_msg{msg} {}
  const char *what() const noexcept override { return m_msg.c_str(); }

 private:
  std::string m_msg;
};

}  // namespace BLang

#endif  // BLANG_EXCEPTION_H