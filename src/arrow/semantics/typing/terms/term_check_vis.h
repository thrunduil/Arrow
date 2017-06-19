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

#include "term_checker.h"
#include "arrow/ast/ast_visitor.h"
#include "term_check_helpers.h"

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              term_check_vis
//------------------------------------------------------------------
class term_check_vis : public ast::rewrite_visitor<term_check_vis>
{
    private:
        using base_type     = ast::rewrite_visitor<term_check_vis>;
        using context_type  = details::kind_inference_context;

    public:
        using group_item    = std::tuple<ad::var_info, ast::term, ast::type>;
        using rec_group     = std::vector<group_item>;

    private:
        using vec_ident     = std::vector<ast::identifier>;
        /// kind symbols x term body x indered type x requested type x 
        ///         group data x root term
        using gen_item      = std::tuple<vec_ident, ast::term, ast::type, 
                                    ast::type, group_item*, ast::term>;
        using gen_item_vec  = std::vector<gen_item>;
        using subs_ptr      = std::shared_ptr<infered_substitutions>;

    private:
        term_checker*       m_owner;
        type_equaler&       m_type_eq;
        bound_map           m_bound_vars;
        module              m_mod;
        error_handler_ptr   m_eh;
        context_type        m_ctx;
        ad::var_info        m_current_symbol;
        group_item*         m_current_item;
        gen_item_vec        m_gen_data;
        inst_collector*     m_collector;
        bool                m_can_inst_local;

    public:
        term_check_vis(term_checker* owner)
            :m_owner(owner), m_type_eq(m_owner->m_type_eq), m_mod(owner->m_mod)
            ,m_eh(owner->m_eh), m_current_item(nullptr), m_collector(nullptr)
            ,m_can_inst_local(true)
        {};

    public:
        void        start();
        ast::term   make(const ast::term& ex, ast::type& ty, bool is_local_func,
                        inst_collector* collector);

        /// is_recursive is used only if group has one element
        void        make(rec_group& group, bool is_recursive, bool is_local_func,
                        inst_collector* collector);

        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast, const ast::type& t)
            -> decltype(base_type::eval(ast,t))
        {
            return base_type::eval(ast, t);
        };        

    public:
        ast::term eval(const ast::int_literal& ast, const ast::type& t);
        ast::term eval(const ast::bool_literal& ast, const ast::type& t);
        ast::term eval(const ast::float_literal& ast, const ast::type& t);
        ast::term eval(const ast::char_literal& ast, const ast::type& t);
        ast::term eval(const ast::string_literal& ast, const ast::type& t);

        ast::term eval(const ast::unpack_term& ast, const ast::type& t);
        ast::term eval(const ast::lambda_term& ast, const ast::type& t);
        ast::term eval(const ast::mu_term& ast, const ast::type& t);
        ast::term eval(const ast::all_term& ast, const ast::type& t);
        ast::term eval(const ast::term_type_application& ast, const ast::type& t);
        ast::term eval(const ast::term_kind_application& ast, const ast::type& t);
        ast::term eval(const ast::tuple_term& ast, const ast::type& t);
        ast::term eval(const ast::term_sequence& ast, const ast::type& t);
        ast::term eval(const ast::list_term& ast, const ast::type& t);
        ast::term eval(const ast::bracket_term& ast, const ast::type& t);
        ast::term eval(const ast::record_term& ast, const ast::type& t);
        ast::term eval(const ast::term_constant& ast, const ast::type& t);
        ast::term eval(const ast::error_term& ast, const ast::type& t);
        ast::term eval(const ast::ref_term& ast, const ast::type& t);
        ast::term eval(const ast::assign_term& ast, const ast::type& t);
        ast::term eval(const ast::deref_term& ast, const ast::type& t);
        ast::term eval(const ast::cell_term& ast, const ast::type& t);
        ast::term eval(const ast::identifier_term& ast, const ast::type& t);
        ast::term eval(const ast::overloaded_symbol& ast, const ast::type& t);
        ast::term eval(const ast::closure_term& ast, const ast::type& t);
        ast::term eval(const ast::annotated_term& ast, const ast::type& t);
        ast::term eval(const ast::if_term& ast, const ast::type& t);
        ast::term eval(const ast::term_application& ast, const ast::type& t);
        ast::term eval(const ast::let_term& ast, const ast::type& t);
        ast::term eval(const ast::path_term& ast, const ast::type& t);
        ast::term eval(const ast::path_int_term& ast, const ast::type& t);
        ast::term eval(const ast::pack_term& ast, const ast::type& t);
        ast::term eval(const ast::variant_term& ast, const ast::type& t);
        ast::term eval(const ast::variant_case_term& ast, const ast::type& t);
        ast::term eval(const ast::val_term& ast, const ast::type& t);
        ast::term eval(const ast::term_kind_scope& ast, const ast::type& t);

    private:
        ast::term   eval_closure(const ast::closure_term& cl, const ast::term& body2, 
                        bool body_modified);        
        void        make_impl(rec_group& group, bool is_recursive, bool local_func);
        ast::type   check_type(const ast::type& ty);
        bool        is_some_type(const ast::type& t, ast::some_type& ret, 
                        ast::type_var& tv, bool& is_var, const context_type& ctx);
        bool        is_all_type(const ast::type& t, ast::all_type& ret, ast::type_var& tv, bool& is_var);
        ast::kind   infer_kind(ast::type& t);
        bool        unify_kinds(const ast::location& loc, const ast::kind& k1, const ast::kind& k2,
                        bool show_errors);
        bool        is_subkind(const ast::location& loc, const ast::kind& k_spec, const ast::kind& k_gen,
                        bool show_errors);

        bool        unify_types(const ast::location& loc, const ast::type& t1, const ast::type& t2,
                        bool show_errors);

        ast::term   infer_all_type(const ast::all_term& ast, ast::type_var& tv, 
                        ast::all_type& at);
        void        infer_some_type(const ast::pack_term& ast, ast::type_var& tv, 
                        ast::some_type& st);
        void        infer_record_type(const ast::record_term& ast, ast::type_var& tv,
                        ast::record_type& rt);                

        ast::type   make_nil_type(const ast::location& loc);
        void        check_and_set(const ast::location& loc, const ast::type_var& tv, 
                        const ast::type& t);
        ast::term   eval_application(const ast::term& ast, std::vector<applic_arg_term>& args,
                        ast::term& head, const ast::type& t);
        ast::term   eval_identifier(const ast::identifier_term& id, const std::vector<applic_arg_term>& args, 
                        const ast::type_var& ty, kind_subs_map& subs_map, bool& is_overloaded,
                        bool& has_errors);
        ast::term   eval_closure(const ast::closure_term& id, const std::vector<applic_arg_term>& args, 
                        const ast::type_var& ty, kind_subs_map& subs_map, bool& is_overloaded,
                        bool& has_errors);
        ast::term   eval_application_head(const ast::term& head, const std::vector<applic_arg_term>& args,
                        const ast::type_var& ty_head, kind_subs_map& subs_map, bool& is_overloaded,
                        bool& has_errors);

        ast::term   resolve_overload(const ast::identifier_term& id, const std::vector<applic_arg_term>& args, 
                        const ast::type_var& ty, kind_subs_map& subs_map, bool& is_overloaded,
                        bool& has_errors);

        void        generalize_kind();
        ast::term   generalize_kind(const ast::term& root, const ast::term& body, 
                        const std::vector<ast::identifier>& args, const ast::type& infered, 
                        ast::type& requested);
        bool        check_type_annotations_rec(const rec_group& group);
        void        make_error(rec_group& group);
        bool        can_instantiate_local() const;

        ast::term   instantiate_type_application(const ast::term& ex);
        ast::term   instantiate_kind_application(const ast::term& ex);

        /// ex can be an overloaded symbol or a closure with overloaded symbol
        ast::type   get_return_overloaded(const ast::term& ex);

    public:
        static ast::term   
                    resolve_overload(const ast::identifier_term& id, 
                        const std::vector<details::applic_arg_term>& args,const ast::type& ret_ty,
                        const module& mod, const error_handler_ptr& eh, subs_ptr& subs, 
                        const ast::overload_index* oi, inst_collector* col, bool& success);
        static ast::type
                    instantiate_kind(const error_handler_ptr& eh, const ast::type& t, 
                        const std::vector<applic_arg_term>& args, bool show_errors,
                        kind_subs_map& subs_map, bool& has_errors);
};

};};};