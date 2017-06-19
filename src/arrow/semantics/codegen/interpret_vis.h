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

/*
#include "interpreter.h"
#include "substitute_term_vis.h"
#include "error/error.h"
#include "semantics/typing/types/computation_on_types.h"
#include "semantics/typing/terms/instantiate_term.h"
#include "semantics/utils.h"
#include "store.h"
#include "interpret_helpers.h"
*/

namespace arrow { namespace codegen
{

//------------------------------------------------------------------
//              interpreter_vis
//------------------------------------------------------------------
class interpreter_vis : public ast::rewrite_visitor<interpreter_vis>
{
    private:
        using base_type = ast::rewrite_visitor<interpreter_vis>;
        using def_map   = std::map<ad::var_info, ast::term>;

    private:
        error_handler_ptr   m_eh;
        module              m_mod;
        bool                m_error;

    public:
        interpreter_vis(const error_handler_ptr& eh, const module& mod);

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast, bool& progress)
            ->decltype(base_type::eval(ast, progress))
        {
            return base_type::eval(ast, progress);
        };

        ast::term   make(const ast::term& ex, bool& progress);

    public:
        bool        is_value(const ast::term& ex);
        
        ast::term   substitute(const ast::identifier& id, const ast::term& subs, 
                        const ast::term& body);
        ast::term   substitute_type(const ast::identifier& id, const ast::type& subs, 
                        const ast::term& body);

        ast::term   eval(const ast::int_literal& ast, bool& progress);
        ast::term   eval(const ast::bool_literal& ast, bool& progress);
        ast::term   eval(const ast::float_literal& ast, bool& progress);
        ast::term   eval(const ast::char_literal& ast, bool& progress);
        ast::term   eval(const ast::string_literal& ast, bool& progress);        
        ast::term   eval(const ast::unpack_term& ast, bool& progress);
        ast::term   eval(const ast::lambda_term& ast, bool& progress);
        ast::term   eval(const ast::mu_term& ast, bool& progress);
        ast::term   eval(const ast::all_term& ast, bool& progress);
        ast::term   eval(const ast::term_type_application& ast, bool& progress);
        ast::term   eval(const ast::term_kind_application& ast, bool& progress);
        ast::term   eval(const ast::tuple_term& ast, bool& progress);
        ast::term   eval(const ast::term_sequence& ast, bool& progress);
        ast::term   eval(const ast::list_term& ast, bool& progress);
        ast::term   eval(const ast::bracket_term& ast, bool& progress);
        ast::term   eval(const ast::record_term& ast, bool& progress);
        ast::term   eval(const ast::term_constant& ast, bool& progress);
        ast::term   eval(const ast::error_term& ast, bool& progress);
        ast::term   eval(const ast::ref_term& ast, bool& progress);
        ast::term   eval(const ast::assign_term& ast, bool& progress);
        ast::term   eval(const ast::deref_term& ast, bool& progress);
        ast::term   eval(const ast::cell_term& ast, bool& progress);
        ast::term   eval(const ast::identifier_term& ast, bool& progress);
        ast::term   eval(const ast::overloaded_symbol& ast, bool& progress);
        ast::term   eval(const ast::closure_term& ast, bool& progress);
        ast::term   eval(const ast::annotated_term& ast, bool& progress);
        ast::term   eval(const ast::if_term& ast, bool& progress);
        ast::term   eval(const ast::term_application& ast, bool& progress);
        ast::term   eval(const ast::let_term& ast, bool& progress);
        ast::term   eval(const ast::path_term& ast, bool& progress);
        ast::term   eval(const ast::path_int_term& ast, bool& progress);
        ast::term   eval(const ast::pack_term& ast, bool& progress);
        ast::term   eval(const ast::variant_term& ast, bool& progress);
        ast::term   eval(const ast::variant_case_term& ast, bool& progress);
        ast::term   eval(const ast::val_term& ast, bool& progress);
        ast::term   eval(const ast::term_kind_scope& ast, bool& progress);

    private:
        bool        can_instantiate_local() const;
};

};};