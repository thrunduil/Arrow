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

#include "make_fresh_binders_type.h"
#include "scopes/symbol_data.h"
#include "error/error.h"
#include "semantics/semantics.h"

namespace arrow { namespace sema
{

make_fresh_binders_vis::make_fresh_binders_vis(const error_handler_ptr& eh, const module& mod)
    :m_eh(eh), m_mod(mod)
{};

ast::type make_fresh_binders_vis::make(const ast::type& ty)
{
    return visit(ty);
};

auto make_fresh_binders_vis::eval(const ast::some_type& ast) -> ast::type
{
    const ast::type& body   = ast.get_body();
    const annot_type& id    = ast.get_var();

    annot_type id2  = make_substitution(id);
    ast::type body2 = visit(body);
    remove_substitution(id);

    return ast::some_type(ast->get_location(), id2, body2);
};

auto make_fresh_binders_vis::eval(const ast::all_type& ast) -> ast::type
{
    const ast::type& body   = ast.get_body();
    const annot_type& id    = ast.get_var();

    annot_type id2  = make_substitution(id);
    ast::type body2 = visit(body);
    remove_substitution(id);

    return ast::all_type(ast->get_location(), id2, body2);
};

auto make_fresh_binders_vis::eval(const ast::lambda_type& ast) -> ast::type
{
    const ast::type& body   = ast.get_body();
    const annot_type& id    = ast.get_var();

    annot_type id2          = make_substitution(id);
    ast::type body2         = visit(body);
    remove_substitution(id);

    ast::polarity_type pol  = ast.get_polarity();
    return ast::lambda_type(ast->get_location(), pol, id2, body2);
};

auto make_fresh_binders_vis::eval(const ast::mu_type& ast) -> ast::type
{
    const ast::type& body   = ast.get_body();
    const annot_type& id    = ast.get_var();

    annot_type id2  = make_substitution(id);
    ast::type body2 = visit(body);
    remove_substitution(id);

    ast::mu_type ret = ast::mu_type(ast->get_location(), id2, body2);
    return ret;
};

auto make_fresh_binders_vis::eval(const ast::identifier_type& ast) -> ast::type
{
    return substitute(ast);
}

make_fresh_binders_vis::annot_type 
make_fresh_binders_vis::make_substitution(const annot_type& aid)
{
    const ast::identifier& id   = aid.get_identifier();
    const ast::kind& ki         = aid.get_kind();
    ad::var_info vi(id);

    auto pos    = m_subs_map.find(id);
    if (pos != m_subs_map.end())
    {
        error::error_base_msg(m_eh).internal_error(aid->get_location(), "repeated binder");
        return aid;
    };

    bool can_change             = true;
    ast::identifier bind        = sema::make_new_binder(id, m_mod, can_change);
    m_subs_map.insert(pos, subs_map::value_type(vi, bind));

    return annot_type(aid->get_location(), bind, ki);
};

void make_fresh_binders_vis::remove_substitution(const annot_type& aid)
{
    const ast::identifier& id   = aid.get_identifier();
    m_subs_map.erase(id);
};

ast::identifier_type make_fresh_binders_vis::substitute(const ast::identifier_type& it)
{
    const ast::identifier& id   = it.get_identifier();
    auto pos = m_subs_map.find(id);

    if (pos == m_subs_map.end())
        return it;

    const ast::identifier& id2  = pos->second;
    ast::identifier_type it2    = ast::identifier_type(it->get_location(), id2);
    return it2;
};

}}
