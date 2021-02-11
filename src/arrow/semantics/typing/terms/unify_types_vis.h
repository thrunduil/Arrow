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

#include "arrow/ast/ast_visitor.h"
#include "semantics/typing/types/bound_map.h"
#include "semantics/typing/types/kind_inference_context.h"

namespace arrow { namespace typing
{
    class kind_check_obj;
};};

namespace arrow { namespace typing { namespace details
{

class unify_types_vis : public ast::traversal_visitor<unify_types_vis>
{
    private:
        using base_type     = ast::traversal_visitor<unify_types_vis>;
        using annot_type_id = ast::annotated_type_identifier;
        using kind_check_obj= arrow::typing::kind_check_obj;
        using context_type  = kind_inference_context;

        enum unfolding_state
        {
            initial,
            unrolled_first,
            unrolled_second,
            unrolled_both,
        };

    private:
        ast::location       m_loc;
        bound_map&          m_bound_map;
        error_handler_ptr   m_eh;
        module              m_mod;
        unfolding_state     m_unfolding_state;
        const context_type& m_ctx;
        bool                m_show_errors;
        bool                m_deduction_mode;
        var_substitution*   m_var_subs;
        inst_collector*     m_collector;

    public:
        unify_types_vis(const ast::location& loc, const error_handler_ptr& eh, const module& mod,
                        bound_map& bm, const context_type& ctx, inst_collector* col);

        bool make(const ast::type& t1, const ast::type& t2, bool show_errors);
        bool deduce(const ast::type& subs_ty, const ast::type& target_ty, var_substitution& subs,
                bool show_errors);

    public:
        void eval(const ast::identifier_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::predefined_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::ref_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::if_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::list_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::arrow_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::type_application& t1p, const ast::type& t2, bool& res);
        void eval(const ast::variant_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::tuple_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::bracket_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::annotated_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::record_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::all_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::some_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::lambda_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::mu_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::type_constant& nt1, const ast::type& t2, bool& res);
        void eval(const ast::type_kind_scope& tks1, const ast::type& t2, bool& res);
        void eval(const ast::path_int_type& t1p, const ast::type& t2, bool& res);
        void eval(const ast::product_of_types& ast, ast::type& t2, bool& equal);
        void eval(const ast::type_sequence& ast, ast::type& t2, bool& equal);

        void eval(const ast::type_var& ast, ast::type& t2, bool& equal);        
        void eval(const ast::int_type& ast, ast::type& t2, bool& equal);
        void eval(const ast::error_type& ast, ast::type& t2, bool& equal);
        void eval(const ast::bool_type& ast, ast::type& t2, bool& equal);
        void eval(const ast::char_type& ast, ast::type& t2, bool& equal);
        void eval(const ast::string_type& ast, ast::type& t2, bool& equal);
        void eval(const ast::type_kind_application& ast, ast::type& t2, bool& equal);
        void eval(const ast::type_type_application& ast, ast::type& t2, bool& equal);

    private:

        bool                make(ast::type t1, ast::type t2);

        ast::type           simplify_type(const ast::type& t, bool first, bool unfold_mu, 
                                int& num_unfolding);
        bool                unify_type_vars(const ast::type_var& t1, const ast::type_var& t2);
        bool                unify_type_vars(const ast::type_var& t1, const ast::type& t2);
        bool                unify_type_vars(const ast::type& t1, const ast::type_var& t2);
        bool                unify_type_vars(const ast::type& t1, const ast::type& t2, bool is_tv1,
                                bool is_tv2);
        bool                unify_identifier_types(const ast::identifier& t1, const ast::identifier& t2);
        bool                unify_kinds(const ast::kind& k_inf, const ast::kind& k_req);
        bool                is_subkind(const ast::kind& k_spec, const ast::kind& k_gen);
        bool                unify_mu(const ast::type& t1, const ast::type& t2, bool is_mu1, bool is_mu2);

    public:
        template<class Ast>
        void eval(const Ast& t1, const ast::type& t2, bool& res)
        {
            base_type::eval(t1, t2, res);
        };
};

};};}