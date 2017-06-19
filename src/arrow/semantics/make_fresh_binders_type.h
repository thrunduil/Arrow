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
#include "arrow/ast/ast.h"
#include "arrow/ast/ast_visitor.h"
#include "scopes/module.h"

/*
#include "arrow/config.h"
#include "arrow/modules.h"
#include "scopes/scope_builder/scope_builder.h"
#include "semantics.h"
#include "error/error.h"
#include "semantics/typing/terms/type_checker.h"
#include "scopes/module_impl.h"
#include "desugaring.h"
#include "codegen/interpreter.h"
#include "root/options.h"
#include "arrow/error_handler.h"
#include "unique_names_provider.h"
#include "semantics/typing/types/computation_on_types.h"
#include "semantics/utils.h"
#include "semantics/typing/terms/term_check_vis.h"
#include "semantics/typing/terms/instantiate_term.h"
#include "scopes/closure_builder/closure_builder.h"

#include <map>
#include <sstream>
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 4127)  //C4127: conditional expression is constant
*/

namespace arrow { namespace sema
{

namespace ad = arrow::details;

//------------------------------------------------------------------
//              make_fresh_binders_vis
//------------------------------------------------------------------
class make_fresh_binders_vis : public ast::rewrite_visitor<make_fresh_binders_vis>
{    
    private:
        using base_type     = ast::rewrite_visitor<make_fresh_binders_vis>;
        using annot_type    = ast::annotated_type_identifier;
        using subs_map      = std::map<ad::var_info, ast::identifier>;

    private:
        subs_map            m_subs_map;
        error_handler_ptr   m_eh;
        module              m_mod;

    public:
        make_fresh_binders_vis(const error_handler_ptr& eh, const module& mod);

        ast::type make(const ast::type& ty);

    public:
        template<class Ast>
        auto eval(const Ast& ast) -> decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };

        auto eval(const ast::some_type& ast) -> ast::type;
        auto eval(const ast::all_type& ast) -> ast::type;
        auto eval(const ast::lambda_type& ast) -> ast::type;
        auto eval(const ast::mu_type& ast) -> ast::type;
        auto eval(const ast::identifier_type& ast) -> ast::type;

    public:
        annot_type              make_substitution(const annot_type& id);
        void                    remove_substitution(const annot_type& id);

    private:
        ast::identifier_type    substitute(const ast::identifier_type& id);
};

}}
