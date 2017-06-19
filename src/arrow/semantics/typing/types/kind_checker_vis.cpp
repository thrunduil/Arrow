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

#include "kind_checker_vis.h"
#include "error/error.h"
#include "scopes/module.h"
#include "semantics/semantics.h"
#include "unifier_kinds.h"
#include "semantics/unique_names_provider.h"
#include "computation_on_types.h"
#include "check_mu_type.h"
#include "semantics/typing/terms/instantiate_term.h"
#include "semantics/typing/types/normal_form_type.h"
#include "root/options.h"

#include <sstream>
#include <iostream>

#pragma warning(push)
#pragma warning(disable:4127)   // conditional expression is constant

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              kind_check_vis
//------------------------------------------------------------------
void kind_check_vis::make(rec_group& group, bool is_recursive, inst_collector* collector)
{
    kind_inference_context ctx;
    m_check_mu.clear();

    m_collector = collector;
    auto scope  = m_eh->push_error_scope();

    for (const auto& pos : group)
    {
        const auto& elem    = std::get<0>(pos);
        ctx.add_recursive_symbol(elem);
    };

    if (group.size() > 1 || is_recursive == true)
    {
        bool ok = make_mu_types(group);

        if (ok == false)
            return make_error(group);
    };

    for (auto& pos : group)
    {
        m_type_root         = std::get<1>(pos);
        m_current_symbol    = std::get<0>(pos);
        ast::type& t        = std::get<1>(pos);
        const ast::kind& k  = std::get<2>(pos);

        bool error;
        bool show_error     = true;
        t                   = visit(t, k, ctx, error, show_error);
    };

    generalize_kinds(ctx);
    bool has_errors     = m_eh->has_errors();

    if (has_errors == true)
    {
        return make_error(group);
    }
    else
    {
        for (auto& pos : group)
        {
            ast::type& t    = std::get<1>(pos);
            ast::kind& k    = std::get<2>(pos);

            close_type(t, k, false);
        };    
    }
}

void kind_check_vis::make_error(rec_group& group)
{
    for (auto& pos : group)
    {
        ast::type& t    = std::get<1>(pos);
        t               = ast::error_type(t->get_location());
    };    
};

void kind_check_vis::make_from_term_checker(ast::type& t, ast::kind& k, 
                context_type& ctx, inst_collector* collector, bool kindvar_allowed)
{
    m_check_mu.clear();
    m_type_root = t;

    m_collector = collector;
    auto scope  = m_eh->push_error_scope();
    bool error;
    bool show_error = true;

    t           = visit(t, k, ctx, error, show_error);

    // if there are errors, then it is possible, that mu types
    // are invalid; we must remove these types, otherwise type checker
    // may loop later            
    if (m_eh->has_errors() == true)
        t   = ast::error_type(t->get_location());

    close_type(t, k, kindvar_allowed);
}

void kind_check_vis::close_type(ast::type& t, ast::kind& k, bool kindvar_allowed)
{
    t   = sema::close_type(t, m_eh);
    k   = sema::close_kind(k, m_eh, kindvar_allowed);

    if (ad::options::normalize_types == true)
    {
        int num_unfolding;
        t = normal_form_type(m_eh, m_owner->m_mod, m_collector, false)
                 .make(t, num_unfolding);
    }

    {
        using close_fun     = std::function<ast::type (const ast::type&)>;

        close_fun fun       = [this](const ast::type& ty) -> ast::type 
                                {
                                    ast::type loc = ty;
                                    loc   = sema::close_type(loc, m_eh);
                                    return loc;
                                };

        m_collector->close_arg_types(close_fun(fun));
    };

    {
        using close_fun     = std::function<ast::kind (const ast::kind&)>;

        close_fun fun       = [this](const ast::kind& ty) -> ast::kind 
                                {
                                    ast::kind loc = ty;
                                    return sema::close_kind(loc, m_eh, false);
                                };

        m_collector->close_arg_kinds(close_fun(fun));
    };
};

void kind_check_vis::generalize_kinds(context_type& ctx)
{
    using vec_ident     = std::vector<ast::identifier>;

    //Kind args + body + kind to check
    using gen_item      = std::tuple<vec_ident, ast::kind, ast::kind, ad::var_info>;
    using gen_item_vec  = std::vector<gen_item>;

    const gen_item_vec& items = ctx.get_generalize_items_kind();

    for (const gen_item& item : items)
    {
        const vec_ident& args   = std::get<0>(item);
        const ast::kind& body   = std::get<1>(item);
        const ast::kind& k      = std::get<2>(item);
        const ad::var_info& sym = std::get<3>(item);

        long n                  = (long)args.size();

        ast::kind ks_infered    = body;

        for (long i = n - 1; i >= 0; --i)
            ks_infered          = ast::kind_scope(args[i]->get_location(), args[i], ks_infered);        

        if (unifier_kinds(m_eh).is_subkind(ks_infered->get_location(), ks_infered, k, true) == false)
        {
            ast::fixity_type ft;
            error_kind_mismatch_for_symbol(ks_infered->get_location(), sym.get_name(ft), ks_infered, 
                                            k, m_type_root, m_type_root);
            continue;
        }
    };
};

void kind_check_vis::make_type_stack(std::vector<ast::type>& stack, const ast::type& root_type, 
                                     const ast::type& cur_type)
{
    if (root_type == cur_type)
    {
        stack.push_back(cur_type);
    }
    else
    {
        stack.push_back(cur_type);
        stack.push_back(root_type);
    }
};

bool kind_check_vis::make_mu_types(rec_group& group)
{
    using annot_id          = ast::annotated_type_identifier;

    long n                  = (long)group.size();

    for (long i = 0; i < n; ++i)
    {
        const auto& elem    = group[i];
        const ast::kind& k  = std::get<2>(elem);

        if (sema::has_kind_vars(k) == true)
        {
            std::vector<ad::var_info> rec_sym;
            rec_sym.reserve(group.size());

            for (const auto& pos : group)
            {
                const ad::var_info& v  = std::get<0>(pos);
                rec_sym.push_back(v);
            };

            const ad::var_info& var = std::get<0>(elem);
            error::type_error_msg(m_eh).error_recursive_symbol_not_annotated(var, rec_sym);
            return false;
        }
    };

    auto scope  = m_eh->push_error_scope();

    const auto& elem        = group[0];
    const ad::var_info& var = std::get<0>(elem);
    ast::identifier id      = var.make_var_identifier();

    m_owner->m_mod.build_fresh_type_variable(id);

    ast::kind k;
    
    if (n == 1)
    {
        k                   = std::get<2>(elem);
    }
    else
    {
        ast::product_kind pk = ast::product_kind(id->get_location(), true);

        for (long i = 0; i < n; ++i)
        {
            const auto& elem_i  = group[i];
            const ast::kind& ki = std::get<2>(elem_i);
            pk.add(ki);
        };

        k                   = pk;
    }

    annot_id aid            = annot_id(id->get_location(), id, k);
    ast::identifier_type it = ast::identifier_type(id->get_location(), id);
    ast::type ty;

    type_substitution ts(m_eh, m_owner->m_mod);

    if (n == 1)
    {
        ts.add_type(var, it, false);
        const ast::type& et = std::get<1>(elem);
        ty                  = ts.make(et);
    }
    else
    {
        for (long i = 0; i < n; ++i)
        {
            const auto& elem_i      = group[i];
            const ad::var_info& vi  = std::get<0>(elem_i);
            ast::int_literal index  = ast::int_literal(vi.get_location(), i);
            ast::type it_i          = ast::path_int_type(vi.get_location(), it, index);
            ts.add_type(vi, it_i, false);
        };

        ast::product_of_types pt    = ast::product_of_types(var.get_location()); 

        for (long i = 0; i < n; ++i)
        {
            const auto& elem_i      = group[i];
            const ast::type& ti     = std::get<1>(elem_i);

            ast::type subs_ty       = ts.make(ti);
            pt.add(subs_ty);
        };

        ty                          = pt;
    };

    ast::mu_type mu         = ast::mu_type(id->get_location(), aid, ty);

    if (n == 1)
    {
        ast::type& elem_ty  = std::get<1>(group[0]);
        elem_ty             = mu;
    }
    else
    {
        for (long i = 0; i < n; ++i)
        {
            auto& elem_i            = group[i];
            ast::type& ti           = std::get<1>(elem_i);
            const ad::var_info& vi  = std::get<0>(elem_i);

            ast::int_literal index  = ast::int_literal(vi.get_location(), i);
            ast::type mu_i          = ast::path_int_type(vi.get_location(), mu, index);

            ti                      = mu_i;
        };
    };

    return m_eh->has_errors() == false;
};

void kind_check_vis::check_mu_contractive(const ast::mu_type& mu, bool& error, bool show_error)
{    
    error   = false;

    if (m_check_mu.is_checked(mu) == true)
        return;

    error = (m_check_mu.make(mu, show_error) == false);
};

ast::type kind_check_vis::instantiate_type_application(const ast::type& ex)
{
    if (ex->get_type_code() != ast::details::type_code::type_type_application)
        return ex;

    bool can_instantiate_local = true;
    return m_collector->make_type_type_application(ex, can_instantiate_local);
};
ast::type kind_check_vis::instantiate_kind_application(const ast::type& ex)
{
    if (ex->get_type_code() != ast::details::type_code::type_kind_application)
        return ex;

    bool can_instantiate_local = true;
    return m_collector->make_type_kind_application(ex, can_instantiate_local);
};

void kind_check_vis::make_star_star(const ast::kind& k)
{
    unifier_kinds(m_eh).is_star_star_kind(k, true);
};

}}}

#pragma warning(pop)