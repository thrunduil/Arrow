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

#include "term_check_vis.h"
#include "type_errors.h"
#include "error/error.h"
#include "scopes/local_scope.h"
#include "semantics/typing/types/kind_checker.h"
#include "semantics/typing/types/kind_inference_context.h"
#include "semantics/semantics.h"
#include "semantics/typing/types/computation_on_types.h"
#include "close_term.h"
#include "instantiate_term.h"
#include "semantics/typing/types/instantiate_type_kind.h"

#include <iostream>

namespace arrow { namespace typing { namespace details
{

//------------------------------------------------------------------
//              term_check_vis
//------------------------------------------------------------------
void term_check_vis::start()
{
    m_bound_vars.clear();
    m_ctx.clear();
    m_gen_data.clear();
    m_can_inst_local = true;
}

ast::term term_check_vis::make(const ast::term& ex, ast::type& t, bool is_local_func,
                               inst_collector* collector)
{
    std::vector<group_item> items;
    group_item it(ad::var_info(), ex, t);

    items.push_back(it);

    this->make(items, false, is_local_func, collector);

    t = std::get<2>(items[0]);
    return std::get<1>(items[0]);
};

void term_check_vis::make(rec_group& group, bool is_recursive, bool is_local_func,
                          inst_collector* collector)
{    
    m_collector = collector;
    return make_impl(group, is_recursive, is_local_func);
}

void term_check_vis::make_error(rec_group& group)
{
    for (auto& pos : group)
    {
        ast::term& ex   = std::get<1>(pos);
        ast::type& ty   = std::get<2>(pos);
        bool has_term   = ex != ast::term();

        if (has_term == true)
            ex          = ast::error_term(ex->get_location());

        ty              = ast::error_type(ty->get_location());
    };

    m_collector->clear_instantiations();
}

void term_check_vis::make_impl(rec_group& group, bool is_recursive, bool local_func)
{    
    auto scope              = m_eh->push_error_scope();

    for (const auto& pos : group)
    {
        const auto& elem    = std::get<0>(pos);
        m_ctx.add_recursive_symbol(elem);
    };
    
    ad::var_info old_sym    = m_current_symbol;

    //Kind args + body + kind to check
    using gen_item_vec  = std::vector<gen_item>;

    long n  = (long)group.size();
    
    if (n > 1 || is_recursive == true)
    {
        bool ok = check_type_annotations_rec(group);

        if (ok == false)
            return make_error(group);
    }

    for (long i = 0; i < n; ++i)
    {
        ast::type& ty       = std::get<2>(group[i]);

        if (sema::is_type_var(ty) == false)
            m_owner->check_subkind_star(ty, m_ctx, m_collector);
    };

    for (long i = 0; i < n; ++i)
    {
        auto scope2         = m_eh->push_error_scope();

        m_current_symbol    = std::get<0>(group[i]);
        ast::term& ex       = std::get<1>(group[i]);
        ast::type& ty       = std::get<2>(group[i]);
        bool has_term       = ex != ast::term();

        m_current_item      = &group[i];        

        if (m_eh->has_errors() == false && has_term == true)         
            ex              = visit(ex, ty);

        if (m_eh->has_errors() == true)
        {
            if (has_term == true)
                ex          = ast::error_term(ex->get_location());

            ty              = ast::error_type(ty->get_location());
            continue;
        }
    };

    if (m_eh->has_errors() == true)
        return make_error(group);

    generalize_kind();

    if (m_eh->has_errors() == true)
        return make_error(group);

    // do not generalize types and terms of local functions, we assume
    // that let is not polymorphic; do not default kind variables to *,
    // this cannot be done until top level function is fully processed
    if (local_func == false)
    {
        details::close_term vis(this->m_owner);
        vis.make(group, m_ctx, *m_collector);
    };

    if (m_eh->has_errors() == true)
        return make_error(group);

    m_current_symbol        = old_sym;

    for (const auto& pos : group)
    {
        const auto& elem    = std::get<0>(pos);
        m_ctx.remove_recursive_symbol(elem);
    };    
}

bool term_check_vis::check_type_annotations_rec(const rec_group& group)
{
    bool ok = true;

    for (const auto& pos : group)
    {
        ad::var_info vi     = std::get<0>(pos);
        const ast::type& ty = std::get<2>(pos);

        if (sema::has_type_vars(ty) == true)
        {
            type_errors(m_eh).error_recursive_symbol_not_annotated(vi, ty, group);
            ok = false;
        }
    };

    return ok;
};

bool term_check_vis::is_all_type(const ast::type& t, ast::all_type& ret, ast::type_var& tv, 
                                 bool& is_var)
{
    return m_type_eq.is_all_type(t, ret, tv, is_var, m_ctx);
}
bool term_check_vis::is_some_type(const ast::type& t, ast::some_type& ret, ast::type_var& tv, 
                                  bool& is_var, const context_type& ctx)
{
    return m_type_eq.is_some_type(t, ret, tv, is_var, ctx); 
}

ast::kind term_check_vis::infer_kind(ast::type& t)
{
    return m_owner->infer_kind(t, m_ctx, m_collector);
}
bool term_check_vis::unify_kinds(const ast::location& loc, const ast::kind& k1, const ast::kind& k2,
                                 bool show_errors)
{
    return m_owner->unify_kinds(loc, k1, k2, show_errors);
};
bool term_check_vis::is_subkind(const ast::location& loc, const ast::kind& k_spec, const ast::kind& k_gen,
                bool show_errors)
{
    return m_owner->is_subkind(loc, k_spec, k_gen, show_errors);
};

bool term_check_vis::unify_types(const ast::location& loc, const ast::type& t1, const ast::type& t2,
                                 bool show_errors)
{
    return m_type_eq.unify_types(loc, t1, t2, m_bound_vars, m_ctx, show_errors);
};

ast::term term_check_vis::infer_all_type(const ast::all_term& ast, ast::type_var& tv, 
                                         ast::all_type& at)
{
    using annot_id          = ast::annotated_type_identifier;
    const annot_id& aid     = ast.get_var();

    const ast::term& body   = ast.get_body();
    ast::kind ki            = aid.get_kind();
    ast::identifier id      = aid.get_identifier();

    auto scope              = m_ctx.add_var(id, ki, ast::polarity_type::neutral);

    ast::type tv_body       = ast::type_var(ast->get_location());
    ast::term body2         = visit(body, tv_body);

    //we may reuse existing binder; def indices cannot be ambiguous
    //since term abstraction and type abstraction are in different universes
    at                      = ast::all_type(ast->get_location(), aid, tv_body);
    check_and_set(ast->get_location(), tv, at);

    if (body2 == body)
        return ast;
    
    ast::term ret           = ast::all_term(ast->get_location(), aid, body2);
    return ret;
};

void term_check_vis::infer_some_type(const ast::pack_term& ast, ast::type_var& tv, 
                                         ast::some_type& st)
{
    using annot_id          = ast::annotated_type_identifier;
    const annot_id& aid     = ast.get_var();
    ast::kind ki            = aid.get_kind();
    ast::identifier id      = aid.get_identifier();
    ast::type id_ty         = ast.get_var_type();

    const ast::term& body   = ast.get_body();

    m_owner->check_kind(id_ty, ki, m_ctx, m_collector);

    auto scope1             = m_ctx.add_var(id, ki, ast::polarity_type::neutral);
    ast::type id_ty2        = sema::make_fresh_binders(id_ty, m_eh, m_mod);
    auto scope2             = m_ctx.add_type_subs(id, id_ty2);

    ast::type tv_body       = ast::type_var(ast->get_location());
    ast::term body2         = visit(body, tv_body);

    //we may reuse existing binder; def indices cannot be ambiguous
    //since term abstraction and type abstraction are in different universes
    st                      = ast::some_type(ast->get_location(), aid, tv_body);
    check_and_set(ast->get_location(), tv, st);

    return;
};

void term_check_vis::infer_record_type(const ast::record_term& ast, ast::type_var& tv, 
                                       ast::record_type& rt)
{
    long n              = ast.size();
    rt                  = ast::record_type(ast->get_location());

    for (long i = 0; i < n; ++i)
    {
        const ast::term& t                      = ast.get_term(i);
        const ast::annotated_identifier& aid    = ast.get_label(i);
        const ast::identifier& id               = aid.get_identifier();
        ast::type id_ty                         = aid.get_type();

        visit(t, id_ty);        

        ast::kind term_kind                     = infer_kind(id_ty);

        m_owner->check_subkind_star(id_ty->get_location(), term_kind, 
                                    details::star_kind_reason::field_type, id_ty);

        ast::annotated_type_identifier ati(id->get_location(), id, term_kind);

        rt.add(ati, id_ty);
    };

    check_and_set(ast->get_location(), tv, rt);
};

ast::type term_check_vis::make_nil_type(const ast::location& loc)
{
    ast::fixity_type ft     = ast::fixity_type::undefined;

    ast::identifier_kind ik;
    ik.set_lex(ast::identifier_kind::lexical_type::ident);
    ik.set_sector(ast::identifier_kind::sector_type::type);
    ik.set_function(ast::identifier_kind::function_type::var);

    ast::identifier var     = ast::identifier(loc, ast::unique_string("A"), ft, 
                                ik, ast::identifier::fresh());
    ast::kind ki            = ast::star_kind(loc);

    m_mod.build_fresh_type_variable(var);

    ast::identifier_type it = ast::identifier_type(loc, var);
    ast::list_type lt       = ast::list_type(loc, it);

    ast::kind k             = ast::star_kind(loc);
    ast::annotated_type_identifier ati  = ast::annotated_type_identifier(loc, var, k);
    ast::all_type at        = ast::all_type(loc, ati, lt);

    return at;
};

void term_check_vis::check_and_set(const ast::location& loc, const ast::type_var& tv, 
                                   const ast::type& t)
{
    if (m_type_eq.unify_types(loc, tv, t, m_bound_vars, m_ctx, true) == false)
    {
        //this must always succeed unless occurs check fails; but
        //in this case error is already printed
        return;
    }

    tv.set_checked(t);
}

void term_check_vis::generalize_kind()
{    
    for (auto& pos : m_gen_data)
    {
        const vec_ident& args   = std::get<0>(pos);
        ast::term& e_body       = std::get<1>(pos);
        ast::type& t_body       = std::get<2>(pos);
        ast::type& t_check      = std::get<3>(pos);
        group_item* item        = std::get<4>(pos);
        const ast::term& root   = std::get<5>(pos);

        e_body  = generalize_kind(root, e_body, args, t_body, t_check);

        std::get<1>(*item)      = e_body;
        std::get<2>(*item)      = t_check;
    };
};

ast::term term_check_vis::generalize_kind(const ast::term& root, const ast::term& ex, 
            const std::vector<ast::identifier>& args, const ast::type& tv, ast::type& t)
{    
    ast::type t_inf     = tv;    
    ast::term ex_ret    = ex;
    bool has_term       = ex_ret != ast::term();

    long n              = (long)args.size();

    for (long i = n - 1; i >= 0; --i)
        t_inf           = ast::type_kind_scope(t_inf->get_location(), args[i], t_inf);

    if (m_type_eq.unify_types(t_inf->get_location(), t_inf, t, m_bound_vars, m_ctx, true) == false)
    {
        type_errors(m_eh).error_type_mismatch_kind_abs(t_inf->get_location(), t_inf, t);

        if (has_term == true)
            return ast::error_term(t_inf->get_location());
        else
            return ex_ret;
    }        

    if (has_term == false)
        return ex_ret;

    std::vector<ast::identifier> arg2;
    ast::term body2;
    sema::decompose_term(root, arg2, body2);

    if (body2 == ex)
    {
        ex_ret  = root;
    }
    else
    {
        for (long i = n - 1; i >= 0; --i)
            ex_ret      = ast::term_kind_scope(ex->get_location(), args[i], ex_ret);
    };

    return ex_ret;
};

ast::term term_check_vis::instantiate_type_application(const ast::term& ex)
{
    if (ex->get_term_code() != ast::details::term_code::term_type_application)
        return ex;

    return m_collector->make_term_type_application(ex, can_instantiate_local());
};
ast::term term_check_vis::instantiate_kind_application(const ast::term& ex)
{
    if (ex->get_term_code() != ast::details::term_code::term_kind_application)
        return ex;

    return m_collector->make_term_kind_application(ex, can_instantiate_local());
};

bool term_check_vis::can_instantiate_local() const
{
    return m_can_inst_local;
};

ast::type term_check_vis::get_return_overloaded(const ast::term& ex)
{
    if (ex->get_term_code() == ast::details::term_code::overloaded_symbol)
    {
        ast::overloaded_symbol os   = ex.static_cast_to<ast::overloaded_symbol>();
        return os.get_return_type();
    }
    else if (ex->get_term_code() == ast::details::term_code::closure_term)
    {
        ast::closure_term cl   = ex.static_cast_to<ast::closure_term>();
        return get_return_overloaded(cl.get_body());
    };

    error::error_base_msg(m_eh).internal_error(ex->get_location(), 
                                 "expecting overloaded symbol");
    return ast::error_type(ex->get_location());
};

}}}
