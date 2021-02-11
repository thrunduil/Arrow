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
#include "arrow/error_handler.h"

using namespace arrow;

class lexer_arrow;

#pragma warning(disable:4101) //unreferenced local variable

class unwind_all : public std::runtime_error
{
    public:
        unwind_all()            :std::runtime_error(""){};
};

class unwind_until_command : public std::runtime_error
{
    public:
        unwind_until_command()  :std::runtime_error(""){};
};

class unwind_until_semi : public std::runtime_error
{
    public:
        unwind_until_semi()     :std::runtime_error(""){};
};

class term_collector
{
    private:
        ast::term_sequence  m_term;

    public:
        term_collector(const ast::location& loc);

        void            add(const ast::term& ex);
        void            add_dot_brack(ast::bracket_term& ex);
        void            add_dot_tuple(ast::tuple_term& ex);
        void            add_proj(const ast::location& loc, const ast::int_literal& pos);
        void            add_dot(const ast::location& loc, const ast::identifier& id);
        void            add_type_applic(const ast::location& loc, 
                            const std::vector<ast::type>& args);
        void            add_kind_applic(const ast::location& loc, 
                            const std::vector<ast::kind>& args);
        ast::term       make_term() const;
};

class type_collector
{
    private:
        ast::type_sequence  m_type;

    public:
        type_collector(const ast::location& loc);

        void            add(const ast::type& ex);
        void            add_dot_brack(ast::bracket_type& ex);
        void            add_dot_tuple(ast::tuple_type& ex);
        void            add_proj(const ast::location& loc, const ast::int_literal& pos);
        void            add_dot(const ast::location& loc, const ast::identifier& id);
        void            add_type_applic(const ast::location& loc, 
                            const std::vector<ast::type>& args);
        void            add_kind_applic(const ast::location& loc, 
                            const std::vector<ast::kind>& args);
        ast::type       make_type() const;
};
