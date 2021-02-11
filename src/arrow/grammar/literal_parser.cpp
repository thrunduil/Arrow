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
#include "literal_parser.h"
#include "unicode.h"
#include "error/error.h"

#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "unicode/uchar.h"

#include <boost/lexical_cast.hpp>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 4996)  //'sprintf': This function or variable may be unsafe

namespace arrow
{

literal_parser::literal_parser(const error_handler_ptr& err_handler, const location& loc)
    :m_eh(err_handler), m_loc(loc)
{};

void literal_parser::report_error(const std::string& msg)
{
    if (m_eh)
        m_eh->report_parser_error(m_loc, msg);
}

void literal_parser::report_warning(const std::string& msg)
{
    if (m_eh)
        m_eh->report_parser_warning(m_loc, msg);
}

bool literal_parser::horner_mul(uint64_t& value, unsigned base, char next)
{
    uint64_t result = value * base;
    if (result / base != value)
        return false;

    value = result;

    if ('0' <= next && next <= '9')
        result = value + next - '0';
    else if ('a' <= next && next <= 'z')
        result = value + next - 'a' + 10;
    else if ('A' <= next && next <= 'Z')
        result = value + next - 'A' + 10;
    else
    {
        error::literal_error_msg(m_eh).error_invalid_integer_literal(m_loc,"");
        throw std::runtime_error("");
    }

    if (result > (uint64_t)INT64_MAX)
        return false;
    value = result;
    return true;
}

void literal_parser::check_code_point(int32_t code_point, const ast::location& loc)
{
    if (code_point < 0)
    {
        error::literal_error_msg(m_eh).error_invalid_utf8_byte_sequence(loc);
        throw std::runtime_error("");
    };

    if (!is_unicode_character(code_point))
    {
        char buffer[16];
        if (code_point <= 0xFFFF)
            sprintf(buffer, "%04X", code_point);
        else
            sprintf(buffer, "%06X", code_point);

        error::literal_error_msg(m_eh).error_code_point_is_not_unicode_char(loc, buffer);
        throw std::runtime_error("");
    }
}

void literal_parser::unescape_uescape_sequence(std::string &literal, int32_t &i, int32_t &o,
                                const ast::location& loc)
{
    bool valid  = true;
    valid       = valid && (i >= 2); // "\u
    valid       = valid && (static_cast< size_t >(i) < literal.length());
    valid       = valid && (literal[i] == 'u' || literal[i] == 'U');

    if (valid == false)
    {
        error::literal_error_msg(m_eh).error_invalid_escape_sequence(loc, literal);
        throw std::runtime_error("");
    };

    size_t code_length = ( literal[i++] == 'u' ? 4 : 6 );

    if (static_cast< size_t >(i) + code_length >= literal.length())
    {
        error::literal_error_msg(m_eh).error_invalid_escape_sequence(loc, literal.substr(i-2));
        throw std::runtime_error("");
    };

    uint64_t value = 0;
    for (size_t j = 0; j < code_length; ++j)
        horner_mul(value, 16, literal[i++]);

    if (value <= std::numeric_limits< int32_t >::max())
    {
        error::literal_error_msg(m_eh).error_invalid_escape_sequence(loc, literal);
        throw std::runtime_error("");
    };

    int32_t code_point = static_cast< int32_t >(value);

    if (!is_unicode_character(code_point))
    {
        std::string escape_sequence = literal.substr((size_t)i - code_length - 2,
                                                    2 + code_length);
        error::literal_error_msg(m_eh).error_invalid_unicode_char_in_escape_sequence
                                    (loc, escape_sequence);
        throw std::runtime_error("");
    }

    U8_APPEND_UNSAFE(&literal[0], o, code_point);
}

void literal_parser::unescape_escape_sequence(std::string& literal, int32_t& i, int32_t& o, 
                                            const ast::location& loc)
{
    bool valid  = true;
    valid       = valid && (i >= 2); // "\i
    
    if (valid == false)
    {
        error::literal_error_msg(m_eh).error_invalid_escape_sequence
                                (loc, literal.substr(i - 1));
        throw std::runtime_error("");
    };

    if (static_cast< size_t >(i) + 2 > literal.length()) // \i"
    {
        error::literal_error_msg(m_eh).error_invalid_escape_sequence
                                (loc, literal.substr(i - 1));
        throw std::runtime_error("");
    };

    switch (literal[i])
    {
        case '\'':
            literal[o++] = '\'';
            break;
        case '"':
            literal[o++] = '"';
            break;
        case '\\':
            literal[o++] = '\\';
            break;
        case '0':
            literal[o++] = 0;
            break;
        case 'a':
            literal[o++] = '\a';
            break;
        case 'b':
            literal[o++] = '\b';
            break;
        case 'f':
            literal[o++] = '\f';
            break;
        case 'n':
            literal[o++] = '\n';
            break;
        case 'r':
            literal[o++] = '\r';
            break;
        case 't':
            literal[o++] = '\t';
            break;
        case 'v':
            literal[o++] = '\v';
            break;
        case 'e':
            literal[o++] = 27;
            break;
        case 'u':
        case 'U':
            return unescape_uescape_sequence(literal, i, o, loc);
        default:
        {
            error::literal_error_msg(m_eh).error_invalid_escape_sequence
                                    (loc, literal.substr(i - 1, 2));
            throw std::runtime_error("");
        }
    };

    i += 1;
}

void literal_parser::unescape(std::string& literal, const ast::location& loc)
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t o = 0;

    while (static_cast< size_t >(i) < literal.length())
    {
        j = i;
        int32_t code_point = read_code_point_from_utf8(literal, i);

        check_code_point(code_point, loc);

        if (code_point == '\\')
        {
            unescape_escape_sequence(literal, i, o, loc);
            continue;
        }

        while (j < i)
            literal[o++] = literal[j++];
    }

    literal.resize(o);
};

void literal_parser::check_text_literal(const std::string& literal, const ast::location& loc)
{
    int32_t prev_index = 0;
    int32_t index = 0;
    int32_t code_point;

    while (static_cast< size_t >(index) < literal.length())
    {
        prev_index = index;
        code_point = read_code_point_from_utf8(literal, index);
        check_code_point(code_point, loc);
    }
}

int32_t literal_parser::parse_char(std::string literal)
{
    bool valid      = true;
    valid           = valid && (literal.length() > 2);
    valid           = valid && (literal.front() == '\'');
    valid           = valid && (literal.back() == '\'');

    if (valid == false)
    {
        error::literal_error_msg(m_eh).error_invalid_character_literal(m_loc, literal);
        throw std::runtime_error("");
    };

    unescape(literal, m_loc);
    int32_t i = 1;
    int32_t result = read_code_point_from_utf8(literal, i);

    if (is_unicode_character(result) == false)
    {
        error::literal_error_msg(m_eh).error_invalid_character_literal(m_loc, literal);
        throw std::runtime_error("");
    };

    return result;
}

std::string literal_parser::parse_string(std::string literal)
{
    bool valid  = true;
    valid       = valid && (literal.length() >= 2);
    valid       = valid && (literal.front() == '"');
    valid       = valid && (literal.back() == '"');

    if (valid == false)
    {
        error::literal_error_msg(m_eh).error_invalid_string_literal(m_loc, literal);
        throw std::runtime_error("");
    }

    if (literal.length() >= 6 && literal[1] == '"' && literal[2] == '"')
    {
        valid   = valid && (*(literal.end() - 2) == '"');
        valid   = valid && (*(literal.end() - 3) == '"');

        if (valid == false)
        {
            error::literal_error_msg(m_eh).error_invalid_string_literal(m_loc, literal);
            throw std::runtime_error("");
        }

        check_text_literal(literal, m_loc);
        return literal.substr(3, literal.length() - 6);
    }
    else
    {
        unescape(literal, m_loc);
        return literal.substr(1, literal.length() - 2);
    }
}

uint64_t literal_parser::parse_int(const std::string& literal)
{
    if (literal.empty() == true)
    {
        error::literal_error_msg(m_eh).error_invalid_integer_literal(m_loc, literal);
        throw std::runtime_error("");
    }

    size_t i = 0;
    unsigned base = 10;

    if (literal.length() >= 3 && literal[0] == '0')
    {
        switch (literal[1])
        {
            case 'x':
            case 'X':
                i = 2;
                base = 16;
                break;
            case 'o':
            case 'O':
                i = 2;
                base = 8;
                break;
            case 'b':
            case 'B':
                i = 2;
                base = 2;
                break;
        }
    }

    uint64_t result = 0;
    
    for (; i < literal.length(); ++i)
    {
        if (literal[i] != '_')
        {
            if (!horner_mul(result, base, literal[i]))
            {
                error::literal_error_msg(m_eh).error_integer_literal_is_too_big(m_loc, literal);
                throw std::runtime_error("");
            }
        }
    }


    return result;
};

double literal_parser::parse_float(const std::string& original_literal)
{
    std::string literal = original_literal;

    auto literal_end    = std::remove(literal.begin(), literal.end(), '_');
    literal.resize(literal_end - literal.begin());

    if ((literal.size() >= 3) == false) // 9.9 | 9e9
    {
        error::literal_error_msg(m_eh).error_invalid_floating_point_literal(m_loc, original_literal);
        throw std::runtime_error("");
    }    

    bool is_hex = (literal[1] == 'x' || literal[1] == 'X');

    if (is_hex && literal.rfind('p') == std::string::npos && literal.rfind('P') == std::string::npos)
        literal.append("p0"); // implicit exponent

    double res  = 0.0;
    
    try
    {
        res = std::stod(literal);
    }
    catch(std::out_of_range&)
    {
        error::literal_error_msg(m_eh).error_floating_point_literal_is_not_representable
                                    (m_loc, original_literal);
        throw std::runtime_error("");
    }
    catch(std::exception& ex)
    {
        (void)ex;
        error::literal_error_msg(m_eh).error_invalid_floating_point_literal(m_loc, original_literal);
        throw std::runtime_error("");
    };

    return res;
}

};

#pragma warning(pop)