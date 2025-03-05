/* @file blang_xml_writer.h
   @brief Header file for the BLang XML dump functions

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
#ifndef BLANG_XML_WRITER_H
#define BLANG_XML_WRITER_H

#include <iostream>
#include <memory>

namespace BLang {

class Expression;
class Predicate;

void writeXMLRichTypesInfo(std::ostream &os);
void writeXML(std::ostream &os, const std::shared_ptr<Expression> expr);
void writeXML(std::ostream &os, const Expression &expr);
void writeXML(std::ostream &os, const std::shared_ptr<Predicate> pred);
void writeXML(std::ostream &os, const Predicate &pred);

};  // namespace BLang

#endif  // BLANG_XML_WRITER_H