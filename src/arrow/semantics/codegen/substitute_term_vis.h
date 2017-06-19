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

#include "arrow/ast/ast_visitor.h"
#include "scopes/module.h"

namespace arrow { namespace codegen
{

namespace ad = arrow::details;

//------------------------------------------------------------------
//              substitute_term_vis
//------------------------------------------------------------------
class substitute_term_vis : public ast::rewrite_visitor<substitute_term_vis>
{
    private:
        using base_type = ast::rewrite_visitor<substitute_term_vis>;
        using subs_map  = std::map<ad::var_info, ast::term>;

    private:
        subs_map            m_subs_map;
        module              m_mod;
        error_handler_ptr   m_eh;

    public:
        substitute_term_vis(const module& mod, const error_handler_ptr& eh);

        /// it is assumed, that subs has fresh binders
        substitute_term_vis(const ast::identifier& id, const ast::term& subs,
                        const module& mod, const error_handler_ptr& eh);        

        /// it is assumed, that subs has fresh binders
        void        add(const ast::identifier& id, const ast::term& subs);

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            ->decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

    public:
        ast::term eval(const ast::identifier_term& ast);
        ast::term eval(const ast::closure_term& ast);
        ast::term eval(const ast::overloaded_symbol& ast);
        ast::term eval(const ast::let_term& ast);
        ast::term eval(const ast::lambda_term& ast);
        ast::term eval(const ast::mu_term& ast);
        ast::term eval(const ast::unpack_term& ast);
        ast::variant_case_term eval(const ast::variant_case_term& ast);

        //---------------------------------------------------------------
        //               IGNORE TYPES
        //---------------------------------------------------------------
        ast::type eval(const ast::arrow_type& ast)       { return ast; }
        ast::type eval(const ast::ref_type& ast)         { return ast; }
        ast::type eval(const ast::if_type& ast)          { return ast; }
        ast::type eval(const ast::path_int_type& ast)    { return ast; }
        ast::type eval(const ast::list_type& ast)        { return ast; }
        ast::type eval(const ast::identifier_type& ast)  { return ast; }
        ast::type eval(const ast::some_type& ast)        { return ast; }
        ast::type eval(const ast::all_type& ast)         { return ast; }
        ast::type eval(const ast::lambda_type& ast)      { return ast; }
        ast::type eval(const ast::mu_type& ast)          { return ast; }
        ast::type eval(const ast::variant_type& ast)     { return ast; }
        ast::type eval(const ast::record_type& ast)      { return ast; }
        ast::type eval(const ast::type_constant& ast)    { return ast; }
        ast::type eval(const ast::tuple_type& ast)       { return ast; }
        ast::type eval(const ast::bracket_type& ast)     { return ast; }
        ast::type eval(const ast::annotated_type& ast)   { return ast; }
        ast::type eval(const ast::type_application& ast) { return ast; }
        ast::type eval(const ast::predefined_type& ast)  { return ast; }
        ast::type eval(const ast::type_var& ast)         { return ast; }
        ast::type eval(const ast::product_of_types& ast) { return ast; }
        ast::type eval(const ast::type_sequence& ast)    { return ast; }
        ast::type eval(const ast::error_type& ast)       { return ast; }
        ast::type eval(const ast::int_type& ast)         { return ast; }
        ast::type eval(const ast::bool_type& ast)        { return ast; }
        ast::type eval(const ast::char_type& ast)        { return ast; }
        ast::type eval(const ast::string_type& ast)      { return ast; }
        ast::type eval(const ast::type_kind_scope& ast)         { return ast; }
        ast::type eval(const ast::type_kind_application& ast)   { return ast; }
        ast::type eval(const ast::type_type_application& ast)   { return ast; }
};

};};