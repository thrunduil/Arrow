/* 
 *  This file is a part of Hasky Lib.
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

#include "arrow/config.h"
#include "arrow/ast/ast_visitor.h"
#include "arrow/error_handler.h"
#include "semantics/operators/operator_parser.h"

namespace arrow { namespace sema
{

class desugaring_visitor : public ast::rewrite_visitor<desugaring_visitor>
{
    private:
        using base_type = ast::rewrite_visitor<desugaring_visitor>;

    private:
        error_handler_ptr   m_eh;
        module              m_mod;
        local_scope         m_ls;
        bool                m_expand_tuples;
        bool                m_make_star_kind;

    public:
        desugaring_visitor(const error_handler_ptr& eh, const module& mod);

        ast::term           make(const ast::term& in);
        ast::type           make(const ast::type& in);

    public:
        //forward all other functions to the base class
        template<class Ast, class ... Args>
        auto eval(const Ast& ast)
            -> decltype(base_type::eval(ast, std::forward<Args>(args)...))
        {
            return base_type::eval(ast, std::forward<Args>(args)...);
        };

        //---------------------------------------------------------------
        //                  TERMS
        //---------------------------------------------------------------
        ast::term   eval(const ast::lambda_term& ast);
        ast::term   eval(const ast::mu_term& ast);
        ast::variant_case_term eval(const ast::variant_case_term& ast);        
        ast::term   eval(const ast::let_term& ast);
        ast::term   eval(const ast::term_sequence& ast_in);
        ast::term   eval(const ast::tuple_term& ast_in);
        ast::term   eval(const ast::unpack_term& ast_in);

        /// kind variables in type binders defaulted to *        
        ast::term   eval(const ast::all_term& ast_in);
        ast::term   eval(const ast::pack_term& ast);

        //---------------------------------------------------------------
        //                  TYPES
        //---------------------------------------------------------------
        /// expand tuple
        ast::type   eval(const ast::tuple_type& ast_in);

        /// parse operators
        ast::type   eval(const ast::type_sequence& ast_in);

        /// kind variables in binders defaulted to *
        ast::type   eval(const ast::some_type& ast);
        ast::type   eval(const ast::all_type& ast);
        ast::type   eval(const ast::lambda_type& ast);
        ast::type   eval(const ast::mu_type& ast);

        //kind of record, variant, ect. labels can be infered
        //ast::type eval(const ast::variant_type& ast);
        //ast::type eval(const ast::record_type& ast);
        //ast::type eval(const ast::type_constant& ast);

        /// kind variables in kind application defaulted to *
        ast::type   eval(const ast::type_kind_application& ast);

        //---------------------------------------------------------------
        //               KINDS
        //---------------------------------------------------------------
        /// default to * if required
        ast::kind eval(const ast::kind_var& ast);
};

};};
