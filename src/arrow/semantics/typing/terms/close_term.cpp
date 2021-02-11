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

#include "close_term.h"
#include "semantics/typing/types/computation_on_types.h"
#include "semantics/semantics.h"
#include "scopes/local_scope.h"
#include "error/error.h"
#include "scopes/symbol_definition.h"
#include "instantiate_term.h"
#include "semantics/typing/types/normal_form_type.h"
#include "root/options.h"

#include <sstream>
#include <iostream>

#pragma warning(push)
#pragma warning(disable:4127)   // conditional expression is constant

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//                  close_term_vis
//------------------------------------------------------------------
//abstract over free type variables
struct group_data
{
    using annot_type    = ast::annotated_type_identifier;
    using let_vec       = std::vector<ast::let_term>;

    public:
        let_vec             m_let_vec;

    public:
        group_data()
        {};
};

//probably this is not needed any more
class close_term_vis : public ast::rewrite_visitor<close_term_vis>
{
    private:
        using base_type     = ast::rewrite_visitor<close_term_vis>;
        using annot_type    = ast::annotated_type_identifier;
        using var_vec       = std::vector<annot_type>;
        using let_vec       = std::vector<ast::let_term>;
        using group_map     = std::map<ad::var_info, group_data>;

        using group_item    = std::tuple<ad::var_info, ast::term, ast::type>;
        using rec_group     = std::vector<group_item>;
        using context_type  = details::kind_inference_context;

    private:
        term_checker*       m_owner;
        error_handler_ptr   m_eh;
        module              m_mod;
        inst_collector*     m_collector;
        group_data*         m_current_item;
        group_map           m_group_map;
        bool                m_build_term;

    public:
        close_term_vis(term_checker* owner, inst_collector* col)
            : m_owner(owner), m_eh(owner->m_eh), m_mod(owner->m_mod) 
            , m_current_item(nullptr), m_build_term(false), m_collector(col)
        {};

    private:
        close_term_vis(close_term_vis* parent, inst_collector* col)
            : m_owner(parent->m_owner), m_eh(parent->m_eh), m_mod(parent->m_mod)
            , m_current_item(nullptr), m_build_term(false), m_collector(col)
        {};

        void        save_let(const ast::let_term& let);
        void        process_let(context_type& ctx);

        ast::type   make(const ast::type& t, context_type& ctx);
        ast::term   make(const ast::term& t, context_type& ctx);

        void        make_type(const ad::var_info& vi, ast::type& t, context_type& ctx);
        void        make_term(const ad::var_info& vi, ast::term& t, context_type& ctx);

    public:
        void        make(rec_group& group, context_type& ctx, bool close_instantiations);

    public:
        //forward all other functions to the base class
        template<class Ast>
        auto eval(const Ast& ast)
            -> decltype(base_type::eval(ast))
        {
            return base_type::eval(ast);
        };
        
        //------------------------------------------------------------------
        //                      TERMS
        //------------------------------------------------------------------
        auto eval(const ast::term_kind_scope& ast) -> ast::term
        {
            return base_type::eval(ast);
        };

        auto eval(const ast::let_term& ast) -> ast::term
        {
            save_let(ast);

            const ast::term& body   = ast.get_body();
            ast::term body2         = visit(body);

            if (body2 == body)
                return ast;

            return ast::let_term(ast->get_location(), ast.get_definitions(), 
                            body2, ast.get_symbol_data());
        };

        auto eval(const ast::mu_term& ast) -> ast::term
        {
            ast::mu_term lam2           = base_type::eval_mu(ast);

            if (lam2 == ast)
                return ast;

            const ast::type& ty1        = ast.get_var().get_type();
            const ast::type& ty2        = lam2.get_var().get_type();

            if (ty1 == ty2)
                return lam2;

            lam2.update_scope(m_mod);
            return lam2;
        };

        auto eval(const ast::lambda_term& ast) -> ast::term
        {
            ast::lambda_term lam2   = base_type::eval_lambda(ast);

            if (lam2 == ast)
                return ast;

            const ast::type& ty1        = ast.get_var().get_type();
            const ast::type& ty2        = lam2.get_var().get_type();

            if (ty1 == ty2)
                return lam2;

            lam2.update_scope(m_mod);
            return lam2;
        };

        auto eval(const ast::unpack_term& ast) -> ast::term
        {
            ast::unpack_term un2    = base_type::eval_unpack(ast);

            if (un2 == ast)
                return ast;

            const ast::type& ty1        = ast.get_term_identifier().get_type();
            const ast::type& ty2        = un2.get_term_identifier().get_type();

            if (ty1 == ty2)
                return un2;

            un2.update_scope(m_mod);
            return un2;
        };

        auto eval(const ast::variant_case_term& ast) -> ast::variant_case_term
        {
            ast::variant_case_term un2  = base_type::eval_case(ast);

            if (un2 == ast)
                return ast;

            un2.update_scope(m_mod);
            return un2;
        };

        //---------------------------------------------------------------
        //               TYPES
        //---------------------------------------------------------------
        ast::type eval_type(const ast::type& ty)
        {
            return sema::close_type(ty, m_eh);
        }

        ast::type eval(const ast::arrow_type& ast)       { return eval_type(ast); }
        ast::type eval(const ast::ref_type& ast)         { return eval_type(ast); }
        ast::type eval(const ast::if_type& ast)          { return eval_type(ast); }
        ast::type eval(const ast::path_int_type& ast)    { return eval_type(ast); }
        ast::type eval(const ast::list_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::identifier_type& ast)  { return eval_type(ast); }
        ast::type eval(const ast::some_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::all_type& ast)         { return eval_type(ast); }
        ast::type eval(const ast::lambda_type& ast)      { return eval_type(ast); }
        ast::type eval(const ast::mu_type& ast)          { return eval_type(ast); }
        ast::type eval(const ast::variant_type& ast)     { return eval_type(ast); }
        ast::type eval(const ast::record_type& ast)      { return eval_type(ast); }
        ast::type eval(const ast::type_constant& ast)    { return eval_type(ast); }
        ast::type eval(const ast::tuple_type& ast)       { return eval_type(ast); }
        ast::type eval(const ast::bracket_type& ast)     { return eval_type(ast); }
        ast::type eval(const ast::annotated_type& ast)   { return eval_type(ast); }
        ast::type eval(const ast::type_application& ast) { return eval_type(ast); }
        ast::type eval(const ast::predefined_type& ast)  { return eval_type(ast); }
        ast::type eval(const ast::type_var& ast)         { return eval_type(ast); }
        ast::type eval(const ast::product_of_types& ast) { return eval_type(ast); }
        ast::type eval(const ast::type_sequence& ast)    { return eval_type(ast); }
        ast::type eval(const ast::error_type& ast)       { return eval_type(ast); }
        ast::type eval(const ast::int_type& ast)         { return eval_type(ast); }
        ast::type eval(const ast::bool_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::char_type& ast)        { return eval_type(ast); }
        ast::type eval(const ast::string_type& ast)      { return eval_type(ast); }
        ast::type eval(const ast::type_kind_scope& ast)      { return eval_type(ast); }
        ast::type eval(const ast::type_kind_application& ast){ return eval_type(ast); }
        ast::type eval(const ast::type_type_application& ast){ return eval_type(ast); }

        //---------------------------------------------------------------
        //               KINDS
        //---------------------------------------------------------------
        ast::kind eval_kind(const ast::kind& ty)
        {
            return sema::close_kind(ty, m_eh, false);
        }

        ast::kind eval(const ast::star_kind& ast)       { return eval_kind(ast); }
        ast::kind eval(const ast::product_kind& ast)    { return eval_kind(ast); }
        ast::kind eval(const ast::star_tuple_kind& ast) { return eval_kind(ast); }
        ast::kind eval(const ast::arrow_kind& ast)      { return eval_kind(ast); }
        ast::kind eval(const ast::star_arrow_kind& ast) { return eval_kind(ast); }
        ast::kind eval(const ast::value_kind& ast)      { return eval_kind(ast); }
        ast::kind eval(const ast::kind_identifier& ast) { return eval_kind(ast); }
        ast::kind eval(const ast::kind_scope& ast)      { return eval_kind(ast); }
        ast::kind eval(const ast::star_all_kind& ast)   { return eval_kind(ast); }
        ast::kind eval(const ast::star_some_kind& ast)  { return eval_kind(ast); }
        ast::kind eval(const ast::star_list_kind& ast)  { return eval_kind(ast); }
        ast::kind eval(const ast::star_record_kind& ast){ return eval_kind(ast); }
        ast::kind eval(const ast::star_ref_kind& ast)   { return eval_kind(ast); }
        ast::kind eval(const ast::kind_var& ast)        { return eval_kind(ast); }

        ast::kind eval(const ast::star_variant_kind& ast)   { return eval_kind(ast); }
        ast::kind eval(const ast::star_predefined_kind& ast){ return eval_kind(ast); }
};

void close_term_vis::make(rec_group& group, context_type& ctx, bool close_instantiations)
{    
    long n = (long)group.size();
    for (long i = 0; i < n; ++i)
    {
        ad::var_info vi     = std::get<0>(group[i]);
        ast::type& ty       = std::get<2>(group[i]);
        
        //sema::print_type(std::cout, ty); std::cout << "\n";
        make_type(vi, ty, ctx);
    };

    for (long i = 0; i < n; ++i)
    {
        ad::var_info vi     = std::get<0>(group[i]);
        ast::term& ex       = std::get<1>(group[i]);
        
        if (ex)
        {
            //sema::print_term(std::cout, ex, m_eh, m_mod); std::cout << "\n";
            make_term(vi, ex, ctx);
        };
    };

    if (close_instantiations == true)
    {
        using close_fun     = std::function<ast::type (const ast::type&)>;

        close_fun fun       = [&ctx,this](const ast::type& ty) -> ast::type 
                                {
                                    ast::type loc = ty;
                                    this->make_type(ad::var_info(), loc, ctx);
                                    return loc;
                                };

        m_collector->close_arg_types(close_fun(fun));
    };

    if (close_instantiations == true)
    {
        using close_fun     = std::function<ast::kind (const ast::kind&)>;

        close_fun fun       = [&ctx,this](const ast::kind& ty) -> ast::kind 
                                {
                                    ast::kind loc = ty;
                                    return sema::close_kind(loc, m_eh, false);
                                };

        m_collector->close_arg_kinds(close_fun(fun));
    };
};

void close_term_vis::make_type(const ad::var_info& vi, ast::type& t, context_type& ctx)
{
    m_build_term    = false;
    auto pos        = m_group_map.insert(group_map::value_type(vi, group_data()));
    m_current_item  = &pos.first->second;

    t               = make(t, ctx);

    if (ad::options::normalize_types == true)
    {
        int num_unfolding;
        t   = normal_form_type(m_eh, m_owner->m_mod,  m_collector, false)
                 .make(t, num_unfolding);
    };
};

void close_term_vis::make_term(const ad::var_info& vi, ast::term& ex, context_type& ctx)
{
    m_build_term    = true;
    auto pos        = m_group_map.find(vi);

    if (pos == m_group_map.end())
    {
        //not possible
        error::error_base_msg(m_eh).internal_error(ex->get_location(), "impossible case");
        return;
    }

    m_current_item  = &pos->second;
    ex              = make(ex, ctx);
};

ast::type close_term_vis::make(const ast::type& t, context_type& ctx)
{
    (void)ctx;
    ast::type t2 = visit(t);
    return t2;
}
ast::term close_term_vis::make(const ast::term& t, context_type& ctx)
{
    ast::term t2 = visit(t);

    process_let(ctx);
    return t2;
}    

void close_term_vis::save_let(const ast::let_term& let)
{
    m_current_item->m_let_vec.push_back(let);
}

void close_term_vis::process_let(context_type& ctx)
{
    //we can process let terms only when all type variables are collected

    using key_type      = ad::var_info;
    using components    = std::vector<std::vector<key_type>>;

    for (const auto& let : m_current_item->m_let_vec)
    {
        const local_scope& ls   = m_owner->m_mod.get_scope_for_let(let);
        using fun_sym_map       = local_scope::fun_sym_map;
        fun_sym_map& loc        = ls.get_local_vars_modify();
        const components& comp  = ls.get_connected_components();

        for (const auto& group : comp)
        {
            using group_item    = std::tuple<ad::var_info, ast::term, ast::type>;
            using rec_group     = std::vector<group_item>;

            rec_group rec;
            long n              = (long)group.size();

            for(long i = 0; i < n; ++i)
            {
                const key_type& key = group[i];
                auto pos            = loc.find(key);

                if (pos == loc.end())
                    continue;
        
                if (pos->second.has_error() == true)
                {
                    m_eh->add_error();
                    continue;
                };

                bool has;
                const ast::term& ex     = pos->second.get_body(has);

                if (has == false)
                    continue;

                const ast::type& ty     = pos->second.get_signature();
                rec.push_back(group_item(key, ex, ty));
            };

            close_term_vis vis(this, m_collector);
            vis.make(rec, ctx, false);

            for(long i = 0, j = 0; i < n; ++i)
            {
                const key_type& key = group[i];
                auto pos            = loc.find(key);

                if (pos == loc.end())
                    continue;

                if (pos->second.has_error() == true)
                {
                    m_eh->add_error();
                    continue;
                };

                bool has;
                const ast::term& ex = pos->second.get_body(has);

                if (has == false)
                    continue;

                const ast::type& ty = pos->second.get_signature();
                ast::identifier id  = pos->second.get_symbol();

                ast::term ex2       = std::get<1>(rec[j]);
                ast::type ty2       = std::get<2>(rec[j]);
                ++j;

                if (ex2 == ex && ty2 == ty)
                    continue;

                ast::term_definition def2   = ast::term_definition(pos->second.get_location(), 
                                                id, ty2, ex2, pos->second.is_overload());
                pos->second.update_term_definition(def2);
            };
        };
    };
};

close_term::close_term(term_checker* owner)
    :m_owner(owner)
{};

void close_term::make(rec_group& group, context_type& ctx, inst_collector& col)
{    
    close_term_vis vis(m_owner, &col);
    vis.make(group, ctx, true);
};

}}}

#pragma warning(pop)