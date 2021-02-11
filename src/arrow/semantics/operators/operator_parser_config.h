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
#pragma once

#include "arrow/ast/ast.h"

namespace arrow { namespace oper
{

/// bracket type is not defined; use a proxy
struct operator_config_type
{
    using iterm         = ast::type;
    using itype         = ast::kind;
    using ident_term    = ast::identifier_type;
    using itype_vec     = std::vector<ast::type>;
    using ikind_vec     = std::vector<ast::kind>;
    using ituple_term   = ast::tuple_type;
    using ibracket_term = ast::bracket_type;
    using iterm_applic  = ast::type_application;

    static bool         is_ident_term(const iterm& elem);
    static bool         is_tuple(const iterm& elem);
    static bool         is_bracket(const iterm& elem);

    static ident_term   cast_to_ident_term(const iterm& elem);
    static ituple_term  cast_to_tuple(const iterm& elem);
    static ibracket_term cast_to_bracket(const iterm& elem);

    static iterm        hide_int(const ast::int_literal& elem);
    static iterm        hide_type_applic(const ast::location& loc, const itype_vec& args);
    static iterm        hide_kind_applic(const ast::location& loc, const ikind_vec& args);
    static iterm        make_error(const ast::location& loc);
    static iterm        make_string_literal(const ast::location& loc, const std::string& str);
    static iterm        make_projection(const ast::location& loc, const iterm& lhs, 
                            const iterm& path);
    static iterm        make_dot(const ast::location& loc, const iterm& lhs, 
                            const ast::identifier& path, const error_handler_ptr& eh);
    static iterm        make_applic_type(const ast::location& loc, const iterm& lhs, 
                            const iterm& rhs);
    static iterm        make_applic_kind(const ast::location& loc, const iterm& lhs, 
                            const iterm& rhs);
};

struct operator_config_term
{
    using iterm         = ast::term;
    using itype         = ast::type;
    using ident_term    = ast::identifier_term;
    using itype_vec     = std::vector<ast::type>;
    using ikind_vec     = std::vector<ast::kind>;
    using ituple_term   = ast::tuple_term;
    using ibracket_term = ast::bracket_term;
    using iterm_applic  = ast::term_application;

    static bool         is_ident_term(const iterm& elem);
    static bool         is_tuple(const iterm& elem);
    static bool         is_bracket(const iterm& elem);

    static ident_term   cast_to_ident_term(const iterm& elem);
    static ituple_term  cast_to_tuple(const iterm& elem);
    static ibracket_term cast_to_bracket(const iterm& elem);

    static iterm        hide_int(const ast::int_literal& elem);
    static iterm        hide_type_applic(const ast::location& loc, const itype_vec& args);
    static iterm        hide_kind_applic(const ast::location& loc, const ikind_vec& args);
    static iterm        make_error(const ast::location& loc);
    static iterm        make_string_literal(const ast::location& loc, const std::string& str);
    static iterm        make_projection(const ast::location& loc, const iterm& lhs, 
                            const iterm& path);
    static iterm        make_dot(const ast::location& loc, const iterm& lhs, 
                            const ast::identifier& path, const error_handler_ptr& eh);
    static iterm        make_applic_type(const ast::location& loc, const iterm& lhs, 
                            const iterm& rhs);
    static iterm        make_applic_kind(const ast::location& loc, const iterm& lhs, 
                            const iterm& rhs);
};

};};

#include "operator_parser_config.inl"