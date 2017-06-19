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

#include "kind_checker.h"
#include "arrow/ast/ast_visitor.h"
#include "scopes/symbol_data.h"
#include "kind_inference_context.h"
#include "check_mu_type.h"

#include <map>
#include <set>

namespace arrow { namespace typing { namespace details
{

//polarities are addes as in "Fixed Points of Type Constructors
//and Primitive Recursion", Abel and Matthes in order to allow
//for recursive type constructors

//------------------------------------------------------------------
//              kind_check_vis
//------------------------------------------------------------------
class kind_check_vis : public ast::rewrite_visitor<kind_check_vis>
{
    public:
        using group_item    = std::tuple<ad::var_info, ast::type, ast::kind>;
        using rec_group     = std::vector<group_item>;

    private:
        using base_type     = ast::rewrite_visitor<kind_check_vis>;
        using context_type  = details::kind_inference_context;
        using kind_vec      = std::vector<ast::kind>;

    private:
        kind_checker*       m_owner;
        error_handler_ptr   m_eh;
        ast::type           m_type_root;
        ad::var_info        m_current_symbol;
        check_mu_type       m_check_mu;
        inst_collector*     m_collector;

    public:
        kind_check_vis(kind_checker* owner)
            :m_owner(owner), m_eh(m_owner->m_eh), m_check_mu(m_owner->m_eh)
            ,m_collector(nullptr)
        {};
        
        /// is_recursive is used only if group has one element
        void        make(rec_group& group, bool is_recursive, inst_collector* collector);
        void        make_from_term_checker(ast::type& t, ast::kind& k, 
                        context_type& ctx, inst_collector* collector, bool kindvar_allowed);

        /// should be called after checking all types in connected group
        void        close_type(ast::type& t, ast::kind& k, bool kindvar_allowed);

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast, const ast::kind& k, context_type& ctx, bool& error, bool show_error)
                    -> decltype(base_type::eval(ast, k, ctx, error, show_error))
        {
            return base_type::eval(ast, k, ctx, error, show_error);
        };

        ast::type   eval(const ast::predefined_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::tuple_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::bracket_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::annotated_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::arrow_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::ref_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::if_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::list_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::identifier_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::some_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::all_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::lambda_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::mu_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::variant_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::record_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);        
        ast::type   eval(const ast::type_constant& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::type_application& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);        
        ast::type   eval(const ast::type_var& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::path_int_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::product_of_types& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::type_sequence& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::int_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::error_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::bool_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::char_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::string_type& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::type_kind_scope& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::type_kind_application& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);
        ast::type   eval(const ast::type_type_application& ast, const ast::kind& k, context_type& ctx, 
                        bool& error, bool show_error);

    private:        
        void        make_error(rec_group& group);

        ast::type   eval_application(const ast::type& ast, std::vector<applic_arg_type>& args, 
                        const ast::type& head, const ast::kind& k, context_type& ctx, bool& error,
                        bool show_error);
        ast::type   eval_application_head(const ast::type& head, const std::vector<applic_arg_type>& args,
                        const ast::kind_var& kid_head, kind_subs_map& subs_map, bool& is_overloaded,
                        bool& error, bool show_error, context_type& ctx);
        ast::type   eval_identifier(const ast::identifier_type& id, const std::vector<applic_arg_type>& args, 
                        const ast::kind_var& ki, kind_subs_map& subs_map, bool& is_overloaded,
                        bool& error, bool show_error, context_type& ctx);

        ast::type   instantiate_kind_application(const ast::type& ty);
        ast::kind   get_return_overloaded(const ast::type& ex, bool& error);
        ast::type   instantiate_type_application(const ast::type& ty);
        ast::type   resolve_overload(const ast::identifier_type& id, const std::vector<applic_arg_type>& args, 
                        const ast::kind_var& kv, kind_subs_map& subs_map, bool& is_overloaded,
                        bool& error, bool show_error);

        void        generalize_kinds(context_type& ctx);
        bool        make_mu_types(rec_group& group);
        void        check_mu_contractive(const ast::mu_type& mu, bool& error, bool show_error);
        void        make_star_star(const ast::kind& k);

    private:
        void    make_type_stack(std::vector<ast::type>& stack, const ast::type& root_type, 
                    const ast::type& cur_type);

        void    error_product_kind_required(const ast::location& loc, const ast::kind& given, long size, 
                    const ast::type& root_type, const ast::type& current_type);
        void    error_star_kind_required_field(const ast::location& loc, const ast::kind& given,
                    const ast::identifier& field_name, const ast::type& root_type, 
                    const ast::type& current_type);
        void    error_value_kind_required(const ast::location& loc, ast::value_kind_code vkc, 
                    const ast::kind& given, const ast::type& root_type, const ast::type& current_type);
        void    error_poly_kind_required(const ast::location& loc, const ast::kind& given,
                    const ast::type& root_type, const ast::type& current_type);
        void    error_arrow_kind_required(const ast::location& loc, const ast::kind& given,
                    details::arrow_kind_reason, const ast::type& root_type, const ast::type& current_type);
        void    error_all_kind_required(const ast::location& loc, const ast::kind& given, 
                    const ast::type& root_type, const ast::type& current_type);
        void    error_kind_mismatch(const ast::location& loc, const ast::kind& req, 
                    const ast::kind& given, const ast::type& root_type, const ast::type& current_type);
        void    error_type_constant_ord(const ast::location& loc, const ast::type& root_type, 
                    const ast::type& current_type);

        void    error_kind_mismatch_for_symbol(const ast::location& loc, 
                    ast::unique_string id, const ast::kind& id_kind, const ast::kind& req,
                    const ast::type& root_type, const ast::type& current_type);
        void    error_kind_polarity_mismatch_for_symbol(const ast::location& loc, 
                    const ast::identifier& id, ast::polarity_type pol, ast::polarity_type pol_exp, 
                    const ast::type& root_type, const ast::type& current_type);
        void    error_kind_polarity_mismatch_for_symbol_rec(const ast::location& loc, 
                    const ast::identifier& id, ast::polarity_type pol, ast::polarity_type pol_exp, 
                    const ast::type& root_type, const ast::type& current_type);
        void    error_polarized_symbol_on_neutral_pos(const ast::location& loc, 
                    const ast::identifier& id, ast::polarity_type pol, const ast::type& root_type,
                    const ast::type& current_type);

        void    error_invalid_kind_application(const ast::location& loc, const ast::type& root_type, 
                    const ast::type& current_type);
        void    error_invalid_kind_application_star(const ast::location& loc, const ast::type& root_type, 
                    const ast::type& current_type);
        void    error_invalid_kind_application_sym(const ast::location& loc, const ast::identifier& id,
                    const ast::kind& ki, const ast::type& root_type, const ast::type& current_type);

        void    error_kind_mismatch_extern_func_ret(const ast::location& loc, const ast::identifier& name,
                    const ast::kind& ret_type, const ast::kind& func_ret_type);
        void    error_kind_mismatch_extern_func_size(const ast::location& loc, const ast::identifier& name,
                    long req_arg_size, long func_arg_size);
        void    error_kind_mismatch_extern_func_arg(const ast::location& loc, const ast::identifier& name,
                    long arg_num, const ast::kind& ret_type, const ast::kind& func_ret_type);

    public:
        void    error_kind_var_not_allowed_polykind(const ast::location& loc, const ast::identifier& id,
                    const ast::kind& kind, const ast::type& root_type, const ast::type& current_type);
        void    error_too_many_kind_args(const ast::location& loc, const ast::identifier& id, 
                    const ast::kind& ki, const ast::type& root_type, const ast::type& current_type);
        void    error_star_kind_required(const ast::location& loc, const ast::kind& given,
                    details::star_kind_reason, const ast::type& root_type, const ast::type& current_type);
};

};};}