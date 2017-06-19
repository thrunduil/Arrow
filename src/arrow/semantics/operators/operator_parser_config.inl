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

#include "operator_parser_config.h"

namespace arrow { namespace oper
{

//---------------------------------------------------------------
//               operator_config_type
//---------------------------------------------------------------
inline bool operator_config_type::is_ident_term(const iterm& elem)
{
    return elem->get_type_code() == ast::details::type_code::identifier_type;
};
inline bool operator_config_type::is_tuple(const iterm& elem)
{
    return elem->get_type_code() == ast::details::type_code::tuple_type;
};
inline bool operator_config_type::is_bracket(const iterm& elem)
{
    return elem->get_type_code() == ast::details::type_code::bracket_type;
};

inline operator_config_type::ident_term 
operator_config_type::cast_to_ident_term(const iterm& elem)
{
    return elem.static_cast_to<ast::identifier_type>();
}

inline operator_config_type::ituple_term
operator_config_type::cast_to_tuple(const iterm& elem)
{
    return elem.static_cast_to<ast::tuple_type>();
};

inline operator_config_type::ibracket_term 
operator_config_type::cast_to_bracket(const iterm& elem)
{
    return elem.static_cast_to<ast::bracket_type>();
};

inline operator_config_type::iterm 
operator_config_type::hide_int(const ast::int_literal& elem)
{
    return ast::int_type(elem->get_location(), elem.get_value());
};

inline ast::type operator_config_type::make_error(const ast::location& loc)
{
    return ast::error_type(loc);
};
inline ast::type 
operator_config_type::make_string_literal(const ast::location& loc, const std::string& str)
{
    return ast::string_type(loc, str);
};

//---------------------------------------------------------------
//               operator_config_term
//---------------------------------------------------------------
inline bool operator_config_term::is_ident_term(const iterm& elem)
{
    return elem->get_term_code() == ast::details::term_code::identifier_term;
};
inline bool operator_config_term::is_tuple(const iterm& elem)
{
    return elem->get_term_code() == ast::details::term_code::tuple_term;
};
inline bool operator_config_term::is_bracket(const iterm& elem)
{
    return elem->get_term_code() == ast::details::term_code::bracket_term;
};

inline operator_config_term::ident_term 
operator_config_term::cast_to_ident_term(const iterm& elem)
{
    return elem.static_cast_to<ast::identifier_term>();
}

inline operator_config_term::ituple_term
operator_config_term::cast_to_tuple(const iterm& elem)
{
    return elem.static_cast_to<ast::tuple_term>();
};

inline operator_config_term::ibracket_term 
operator_config_term::cast_to_bracket(const iterm& elem)
{
    return elem.static_cast_to<ast::bracket_term>();
};

inline operator_config_term::iterm 
operator_config_term::hide_int(const ast::int_literal& elem)
{
    return elem;
};

inline ast::term operator_config_term::make_error(const ast::location& loc)
{
    return ast::error_term(loc);
};
inline ast::term 
operator_config_term::make_string_literal(const ast::location& loc, const std::string& str)
{
    return ast::string_literal(loc, str);
};

};};

#include "operator_parser_config.inl"