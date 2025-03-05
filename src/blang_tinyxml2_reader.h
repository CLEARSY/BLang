/* @file blang_tinyxml2_reader.h
   @brief Header file for reading XML to BLang objects with TinyXML2

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
#include <memory>

namespace tinyxml2 {
class XMLElement;
}
namespace BLang {

class Expression;
class Predicate;

extern void readRichTypeInfos(const tinyxml2::XMLElement* root);
extern std::shared_ptr<Expression> readExpression(
    const tinyxml2::XMLElement* root);
extern std::shared_ptr<Predicate> readPredicate(
    const tinyxml2::XMLElement* root);

}  // namespace BLang
