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

#include "arrow/config.h"
#include "arrow/ast/ast.h"
#include "scopes/symbol_data.h"
#include "substitution.h"
#include "semantics/typing/types/bound_map.h"
#include "semantics/typing/types/kind_inference_context.h"

#include <map>

namespace arrow { namespace typing
{

namespace ad = arrow::details;

class kind_checker;

/// beta type equality; recursive types are weakly equi-recursive
/// it must be already checked, that recursive types are contractive
class type_equaler
{
    private:
        using context_type  = details::kind_inference_context;

    private:
        kind_checker*       m_kind_checker;
        bound_map*          m_bound_map;
        module              m_mod;
        error_handler_ptr   m_eh;
        inst_collector*     m_collector;

    public:
        type_equaler(kind_checker* kc, const module& mod, const error_handler_ptr& eh,
                     inst_collector* col);

        void                set_collector(inst_collector* col);

        bool                is_all_type(const ast::type& t, ast::all_type& ret, ast::type_var& tv, 
                                bool& is_var, const context_type& ctx);
        bool                is_some_type(const ast::type& t, ast::some_type& ret, ast::type_var& tv, 
                                bool& is_var, const context_type& ctx);
        bool                is_tuple_type(const ast::type& t, ast::tuple_type& ret, 
                                const context_type& ctx);
        bool                is_record_type(const ast::type& t, ast::record_type& ret, ast::type_var& tv,
                                bool& is_var, const context_type& ctx);
        bool                is_variant_type(const ast::type& t, ast::variant_type& ret,
                                const context_type& ctx);
        bool                is_int_value_kind(const ast::type& t, ast::int_type& it, 
                                const context_type& ctx);
        bool                is_bool_value_kind(const ast::type& t, ast::bool_type& it,
                                const context_type& ctx);
        bool                is_char_value_kind(const ast::type& t, ast::char_type& it,
                                const context_type& ctx);
        bool                is_string_value_kind(const ast::type& t, ast::string_type& it,
                                const context_type& ctx);

        bool                unify_types(const ast::location& loc, const ast::type& t1, const ast::type& t2,
                                bound_map& bound, const context_type& ctx, bool show_errors);

        bool                deduce_arrow_all(const ast::type& t, ast::arrow_type& ret, 
                                var_substitution& subs, const context_type& ctx);

        bool                deduce_subs(const ast::location& loc, const ast::type& subs_type, 
                                const ast::type& target_type, bound_map& bound, const context_type& ctx, 
                                var_substitution& subs, bool show_errors);

        bool                unify_with_predefined(const ast::type& t, ast::predefined_type_code tc, 
                                const context_type& ctx);
        bool                unify_with_tuple(const ast::type& t, long size, ast::tuple_type& ret, 
                                const context_type& ctx);
        bool                unify_with_list(const ast::type& t, ast::list_type& ret, 
                                const context_type& ctx);
        bool                unify_with_ref(const ast::type& t, ast::ref_type& ret, 
                                const context_type& ctx);
        bool                unify_with_arrow(const ast::type& t, ast::arrow_type& ret, 
                                const context_type& ctx);
        bool                unify_with_value_kind(const ast::type& t, ast::predefined_type_code& code,
                                const context_type& ctx);

        ast::kind           infer_kind(ast::type& t, context_type& ctx, inst_collector* collector);

    private:
        void                start();

        /// compute weak head normal form of types; 
        /// unfold top-level mu types
        ast::type           simplify_type_rec(const ast::type& t);
        bool                make_substitution(ast::type& t, const context_type& ctx);

        bool                unify_types_impl(const ast::location& loc, const ast::type& t1, 
                                const ast::type& t2, const context_type& ctx, bool show_errors);
        void                prepare_deduction_all(var_substitution& subs, const ast::type& at, 
                                ast::type& tail, const context_type& ctx);
};

};};