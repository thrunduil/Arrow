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

#include "interpret_helpers.h"
#include "scopes/symbol_data.h"
#include "error/error.h"
#include "semantics/semantics.h"

namespace arrow { namespace codegen
{

//------------------------------------------------------------------
//              is_value_vis
//------------------------------------------------------------------
void is_value_vis::eval(const ast::tuple_term& ast)
{
    m_value = true;
    long k  = ast.size();
    for (long i = 0; i < k; ++i)
    {
        visit(ast.get(i));

        if (m_value == false)
            return;
    };
};

void is_value_vis::eval(const ast::list_term& ast)
{
    m_value = true;
    long k  = ast.size();
    for (long i = 0; i < k; ++i)
    {
        visit(ast.get(i));

        if (m_value == false)
            return;
    };
};

void is_value_vis::eval(const ast::record_term& ast)
{
    m_value = true;
    long k  = ast.size();
    for (long i = 0; i < k; ++i)
    {
        visit(ast.get_term(i));

        if (m_value == false)
            return;
    };
};

void is_value_vis::eval(const ast::term_constant& ast)
{
    if (ast.is_extern_func() == true)
    {
        m_value = false;
        return;
    };

    long n              = ast.var_size(); 

    for (long i = 0; i < n; ++i)
    {
        const ast::term& ti = ast.get_term(i);
        visit(ti);

        if (m_value == false)
            return;
    }    

    m_value = true;
};

void is_value_vis::eval(const ast::pack_term& ast)
{
    visit(ast.get_body());
};

void is_value_vis::eval(const ast::variant_term& ast)
{
    visit(ast.get_term());
};

void is_value_vis::eval(const ast::closure_term& ast)
{
    //only values can be substituted => substitutions are values
    return visit(ast.get_body());
};

//------------------------------------------------------------------
//              make_fresh_binders_term_vis
//------------------------------------------------------------------
make_fresh_binders_term_vis::make_fresh_binders_term_vis
        (const error_handler_ptr& eh, const module& mod)
    :m_eh(eh), m_mod(mod), m_type_subs(eh, mod)
{};

ast::term make_fresh_binders_term_vis::make(const ast::term& ty)
{
    return visit(ty);
};

ast::term make_fresh_binders_term_vis::eval(const ast::lambda_term& ast)
{
    const ast::term& body   = ast.get_body();
    const annot_term& id    = ast.get_var();

    annot_term id2      = make_substitution(id, true);
    ast::term body2     = visit(body);

    remove_substitution(id);

    return ast::lambda_term(ast->get_location(), id2, body2, ast.get_symbol_data());
};

ast::term make_fresh_binders_term_vis::eval(const ast::mu_term& ast)
{
    const ast::term& body   = ast.get_body();
    const annot_term& id    = ast.get_var();

    annot_term id2      = make_substitution(id, true);
    ast::term body2     = visit(body);

    remove_substitution(id);

    return ast::mu_term(ast->get_location(), id2, body2, ast.get_symbol_data());
};

ast::term make_fresh_binders_term_vis::eval(const ast::unpack_term& ast)
{
    const ast::term& pack   = ast.get_package();
    const ast::term& body   = ast.get_term();
    const annot_type& id_t  = ast.get_type_identifier();
    const annot_term& id_e  = ast.get_term_identifier();

    annot_term id_e2        = make_substitution(id_e, true);
    annot_type id_t2        = m_type_subs.make_substitution(id_t);

    ast::term pack2         = visit(pack);
    ast::term body2         = visit(body);

    m_type_subs.remove_substitution(id_t);
    remove_substitution(id_e);

    return ast::unpack_term(ast->get_location(), pack2, id_t2, id_e2, body2, 
                            ast.get_symbol_data());
};

ast::term make_fresh_binders_term_vis::eval(const ast::variant_case_term& ast)
{
    long n                  = ast.size();
    const ast::term& ex     = ast.get_term();

    ast::term ex2           = visit(ex);

    ast::variant_case_term ret(ast->get_location(), ast.get_symbol_data());
    ret.set_term(ex2);

    for (long i = 0; i < n; ++i)
    {
        const annot_term& l = ast.get_label(i);
        const ast::term& m  = ast.get_bind(i);

        annot_term l2       = make_substitution(l, false);
        ast::term m2        = visit(m);

        ret.add_pattern(l2, m2);

        remove_substitution(l);
    };

    return ret;
};

ast::term make_fresh_binders_term_vis::eval(const ast::let_term& ast)
{
    //definitions do not need fresh binders

    const ast::term& body   = ast.get_body();
    ast::term body2         = visit(body);

    if (body2 == body)
    {
        return ast;
    }
    else
    {
        return ast::let_term(ast->get_location(), ast.get_definitions(), 
                        body2, ast.get_symbol_data());
    };
};

ast::term make_fresh_binders_term_vis::eval(const ast::all_term& ast)
{
    const ast::term& body   = ast.get_body();
    const annot_type& id    = ast.get_var();

    annot_type id2      = m_type_subs.make_substitution(id);
    ast::term body2     = visit(body);

    m_type_subs.remove_substitution(id);

    return ast::all_term(ast->get_location(), id2, body2);
};

ast::term make_fresh_binders_term_vis::eval(const ast::pack_term& ast)
{
    const ast::term& body   = ast.get_body();
    const annot_type& id    = ast.get_var();            
    const ast::type& vt     = ast.get_var_type();
    const ast::type& st     = ast.get_some_type();

    annot_type id2      = m_type_subs.make_substitution(id);
            
    ast::term body2     = visit(body);
    ast::type vt2       = eval_type(vt);
    ast::type st2       = eval_type(st);

    m_type_subs.remove_substitution(id);

    return ast::pack_term(ast->get_location(), id2, vt2, body2, st2);
};

ast::identifier_term make_fresh_binders_term_vis::eval(const ast::identifier_term& ast)
{
    return substitute(ast);
}        

ast::term make_fresh_binders_term_vis::eval(const ast::closure_term& ast)
{
    const ast::term& body   = ast.get_body();
    long n                  = ast.size();

    ast::term body2         = visit(body);

    bool modified           = body2 != body;

    ast::closure_term ret(ast->get_location(), body2, ast.get_kind());

    for (long i = 0; i < n; ++i)
    {
        const ast::term& s          = ast.get_substitution(i);
        const ast::identifier& id   = ast.get_internal_ident(i);
        const ast::type& ty         = ast.get_type(i);

        ast::term s2                = visit(s);
        ast::type ty2               = visit(ty);

        modified                    |= s2 != s;
        modified                    |= ty2 != ty;

        ret.add_item(s2, id, ty2);
    };

    if (modified == false)
        return ast;
    else
        return ret;
};

ast::type make_fresh_binders_term_vis::eval_type(const ast::type& ty)
{
    return m_type_subs.make(ty);
};

make_fresh_binders_term_vis::annot_term
make_fresh_binders_term_vis::make_substitution(const annot_term& aid, bool can_change)
{
    const ast::identifier& id   = aid.get_identifier();
    const ast::type& ty         = aid.get_type();
    ad::var_info vi(id);

    auto pos    = m_subs_map.find(id);
    if (pos != m_subs_map.end())
    {
        error::error_base_msg(m_eh).internal_error(aid->get_location(), "repeated binder");
        return aid;
    };

    ast::identifier bind        = sema::make_new_binder(id, m_mod, can_change);
    m_subs_map.insert(pos, subs_map::value_type(vi, bind));

    ast::type ty2               = eval_type(ty);

    return annot_term(aid->get_location(), bind, ty2);
};

void make_fresh_binders_term_vis::remove_substitution(const annot_term& aid)
{
    const ast::identifier& id   = aid.get_identifier();
    m_subs_map.erase(id);
};

ast::identifier_term make_fresh_binders_term_vis::substitute(const ast::identifier_term& it)
{
    const ast::identifier& id   = it.get_identifier();
    auto pos = m_subs_map.find(id);

    if (pos == m_subs_map.end())
        return it;

    const ast::identifier& id2  = pos->second;
    ast::identifier_term it2    = ast::identifier_term(it->get_location(), id2);
    return it2;
};

ast::term codegen::make_fresh_binders(const ast::term& t, const error_handler_ptr& eh,
                const module& mod)
{
    make_fresh_binders_term_vis vis(eh, mod);
    return vis.make(t);
};

}}
