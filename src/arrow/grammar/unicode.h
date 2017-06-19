/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#pragma once

#include <stdint.h>
#include <string>

namespace arrow
{

bool        is_unicode_character(int32_t codePoint);

bool        is_lowercase_identifier_start(int32_t codePoint);
bool        is_uppercase_identifier_start(int32_t codePoint);
bool        is_identifier_start(int32_t codePoint);
bool        is_identifier_part(int32_t codePoint);

/// note Unicode identifier with underscore characters
/// see http://www.unicode.org/reports/tr31/
bool        is_identifier(const std::string& utf8);


/// see http://www.fileformat.info/info/unicode/category/Zs/list.htm
/// see http://en.wikipedia.org/wiki/Space_(punctuation)#Spaces_in_Unicode
bool        is_white_space(int32_t codePoint);

/// see http://en.wikipedia.org/wiki/Newline#Unicode
bool        is_line_break(int32_t codePoint);

/// returns negative value when invalid UTF-8 byte sequence
/// warning Result should be checked in is_unicode_character()
/// pre utf8.length() <= std::numeric_limits< int32_t >::max()
/// pre index < utf8.length()
int32_t     read_code_point_from_utf8(const std::string& utf8, int32_t& index);

/// returns negative value when invalid UTF-8 byte sequence
/// warning Result should be checked in is_unicode_character()
/// pre utf8.length() <= std::numeric_limits< int32_t >::max()
/// pre index > 0
/// pre index <= utf8.length()
int32_t     read_previous_code_point_from_utf8(const std::string& utf8, int32_t& index);

/// returns negative value when: invalid UTF-8 byte sequence, empty utf8
/// or more then one code point in utf8
/// warning Result should be checked in is_unicode_character()
int32_t     decode_utf8_code_point(const std::string& utf8);

bool        is_operator_char(int32_t codePoint);

/// see http://unicode-table.com/en/#mathematical-operators
/// see http://unicode-table.com/en/#supplemental-mathematical-operators
bool        is_operator(const std::string& utf8);

bool        is_superscript_or_subscript_char(int32_t codePoint);

bool        is_valid_utf8(const std::string& utf8);

/// appends Unicode code point to UTF-8 string.
/// warning: Replace invalid or noncharacter code point with replacement
///     character (U+FFFD)
/// returns false when codePoint replacement occurrs
bool        append_code_point_to_utf8(std::string& utf8, int32_t codePoint);

std::string create_utf8_from_code_point(int32_t codePoint);

/// returns: surrogate when invalid word sequence
/// warning: Result should be checked in is_unicode_character()
/// param: length number of uint16_t words in utf16 parameter
/// pre: index < length
int32_t     read_code_point_from_utf16(const uint16_t* utf16, int32_t length, int32_t& index);

inline 
bool is_unicode_code_point(int32_t codePoint)
{
    return codePoint >= 0 && codePoint <= 0x10FFFF;
}

inline 
bool is_unicode_surrogate(int32_t codePoint)
{
    return codePoint >= 0xD800 && codePoint <= 0xDFFF;
}

/// see http://www.fileformat.info/info/unicode/category/Ps/list.htm
bool        is_opening_bracket(int32_t codePoint);

/// \see http://www.fileformat.info/info/unicode/category/Pe/list.htm
bool        is_closing_bracket(int32_t codePoint);

bool        do_brackets_match(int32_t opening, int32_t closing);
}
