/* 
 *  This file is a part of Arrow library.
 *
 *  Copyright (c) Pawe³ Kowal 2017 - 2021
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

#include "error.h"
#include "error_formatter.h"

#include <sstream>


namespace arrow { namespace error
{

literal_error_msg::literal_error_msg(const error_handler_ptr& eh)
    :error_base_msg(eh)
{};

void literal_error_msg::error_invalid_utf8_byte_sequence(const ast::location& loc)
{
    error_formatter os;
    os.head() << "invalid utf8 sequence";

    m_eh->report_parser_error(loc, os.str());
}

void literal_error_msg::error_invalid_escape_sequence(const ast::location& loc, const std::string& s)
{
    error_formatter os;
    os.head() << "invalid escape sequence " << s;

    m_eh->report_parser_error(loc, os.str());
}

void literal_error_msg::error_code_point_is_not_unicode_char(const ast::location& loc, const char* buffer)
{
    error_formatter os;
    os.head() << "code point U+" << buffer << " is not an unicode character";

    m_eh->report_parser_error(loc, os.str());
};

void literal_error_msg::error_invalid_unicode_char_in_escape_sequence(const ast::location& loc, 
            const std::string& escape_sequence)
{
    error_formatter os;
    os.head() << "invalid unicode character in escape sequence: " << escape_sequence;

    m_eh->report_parser_error(loc, os.str());
}

void literal_error_msg::error_integer_literal_is_too_big(const ast::location& loc, const std::string& literal)
{
    error_formatter os;
    os.head() << "integer literal is too bing: " << literal;

    m_eh->report_parser_error(loc, os.str());
}

void literal_error_msg::error_integer_literal_is_too_big(const ast::location& loc, uint64_t val)
{
    error_formatter os;
    os.head() << "integer literal is too bing: " << val;

    m_eh->report_parser_error(loc, os.str());
};

void literal_error_msg::error_invalid_integer_literal(const ast::location& loc, const std::string& literal)
{
    error_formatter os;
    os.head() << "integer literal is invalid: " << literal;
    m_eh->report_parser_error(loc, os.str());
}

void literal_error_msg::error_invalid_character_literal(const ast::location& loc, const std::string& literal)
{
    error_formatter os;
    os.head() << "character literal is invalid: " << literal;
    m_eh->report_parser_error(loc, os.str());
}

void literal_error_msg::error_invalid_string_literal(const ast::location& loc, const std::string& literal)
{
    error_formatter os;
    os.head() << "string literal is invalid: " << literal;
    m_eh->report_parser_error(loc, os.str());
}


void literal_error_msg::error_invalid_floating_point_literal(const ast::location& loc, 
            const std::string& literal)
{
    error_formatter os;
    os.head() << "floating point literal is invalid: " << literal;
    m_eh->report_parser_error(loc, os.str());
}

void literal_error_msg::error_floating_point_literal_is_not_representable(const ast::location& loc, 
            const std::string& literal)
{
    error_formatter os;
    os.head() << "floating point literal is not representable: " << literal;

    m_eh->report_parser_error(loc, os.str());
}

}}
